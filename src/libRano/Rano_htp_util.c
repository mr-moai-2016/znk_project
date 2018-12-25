#include <Rano_htp_util.h>
#include <Rano_htp_boy.h>
#include <Rano_htp_modifier.h>

#include <Znk_htp_hdrs.h>
#include <Znk_s_base.h>
#include <Znk_str.h>
#include <Znk_cookie.h>
#include <Znk_missing_libc.h>
#include <Znk_rand.h>

static void
initHtpHdr( ZnkHtpHdrs htp_hdrs, const char* hostname, const char* ua, ZnkVarpAry cookie, bool is_https, const char* explicit_referer )
{
	ZnkVarp varp;
	ZnkSRef sref = { 0 };
	const int rand_factor = ZnkRand_getRandI( 0, 100 );
	char referer_bur[ 512 ] = "";

	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Host",   Znk_strlen_literal("Host"),
			hostname, Znk_strlen(hostname), true );

	/* dummy(順番を維持するため) */
	ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"User-Agent", Znk_strlen_literal("User-Agent"),
			ua, Znk_strlen(ua), true );

	ZnkSRef_set_literal( &sref, "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" );
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Accept", Znk_strlen_literal("Accept"),
			sref.cstr_, sref.leng_, true );

	ZnkSRef_set_literal( &sref, "ja,en-US;q=0.7,en;q=0.3" );
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Accept-Language", Znk_strlen_literal("Accept-Language"),
			sref.cstr_, sref.leng_, true );

	ZnkSRef_set_literal( &sref, "gzip, deflate" );
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Accept-Encoding", Znk_strlen_literal("Accept-Encoding"),
			sref.cstr_, sref.leng_, true );

	if( !ZnkS_empty( explicit_referer ) ){
		Znk_snprintf( referer_bur, sizeof(referer_bur), "%s", explicit_referer );
		sref.cstr_ = referer_bur;
		sref.leng_ = Znk_strlen( referer_bur );
	} else { 
		if( is_https ){
			if( rand_factor < 30 ){
				ZnkSRef_set_literal( &sref, "https://www.google.com" );
			} else if( rand_factor < 50 ){
				ZnkSRef_set_literal( &sref, "https://www.2chan.net" );
			} else {
				Znk_snprintf( referer_bur, sizeof(referer_bur), "https://%s", hostname );
				sref.cstr_ = referer_bur;
				sref.leng_ = Znk_strlen( referer_bur );
			}
		} else {
			if( rand_factor < 30 ){
				ZnkSRef_set_literal( &sref, "http://www.google.com" );
			} else if( rand_factor < 50 ){
				ZnkSRef_set_literal( &sref, "http://www.2chan.net" );
			} else {
				Znk_snprintf( referer_bur, sizeof(referer_bur), "http://%s", hostname );
				sref.cstr_ = referer_bur;
				sref.leng_ = Znk_strlen( referer_bur );
			}
		}
	}
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Referer", Znk_strlen_literal("Referer"),
			sref.cstr_, sref.leng_, true );


	if( cookie ){
		ZnkStr     cok_stmt = ZnkStr_new( "" );
		ZnkCookie_extend_toCookieStatement( cookie, cok_stmt );
		varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
				"Cookie",   Znk_strlen_literal("Cookie"),
				ZnkStr_cstr(cok_stmt), ZnkStr_leng(cok_stmt), true );
		ZnkStr_delete( cok_stmt );
	}

	ZnkSRef_set_literal( &sref, "keep-alive" );
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Connection", Znk_strlen_literal("Connection"),
			sref.cstr_, sref.leng_, true );

	Znk_UNUSED( varp );
}
bool
RanoHtpUtil_download( const char* hostname, const char* unesc_req_urp, const char* target,
		const char* ua, ZnkVarpAry cookie,
		const char* parent_proxy,
		ZnkStr result_filename, ZnkStr msg, RanoModule mod, int* status_code, bool is_https,
		const char* tmpdir, const char* explicit_referer, const char* default_ua )
{
	bool        result   = false;
	//const char* tmpdir   = EstConfig_getTmpDirPID( true );
	//const char* tmpdir   = "tmp/pid";
	const char* cachebox = "./cachebox/";
	struct ZnkHtpHdrs_tag htp_hdrs = { 0 };
	bool   is_without_404 = true;

	ZnkHtpHdrs_compose( &htp_hdrs );
	initHtpHdr( &htp_hdrs, hostname, ua, cookie, is_https, explicit_referer );

	/***
	 * Header and Cookie filtering
	 * PostVar filteringにおけるon_post関数の呼び出しで、headerやcookie_varsの値を
	 * 参照/加工するような処理にも対応するため、この filteringをここで呼び出す.
	 */
	if( mod ){
		bool is_all_replace = true;
		RanoModule_filtHtpHeader(  mod, htp_hdrs.vars_ );
		RanoModule_filtCookieVars( mod, htp_hdrs.vars_, is_all_replace, NULL );
	}

	if( mod ){
		ZnkMyf ftr_send = RanoModule_ftrSend( mod );
		const ZnkVarpAry ftr_vars = ZnkMyf_find_vars( ftr_send, "header_vars" );
		if( ftr_vars ){
			const ZnkVarp ua_var = ZnkVarpAry_find_byName( ftr_vars, "User-Agent", Znk_strlen_literal("User-Agent"), false );

			/***
			 * header_varsにUser-Agentの項目がないか、値がUNTOUCHである場合.
			 * default_uaを使う.
			 */
			const char* ua = ( ua_var == NULL ) ? default_ua :
				ZnkS_eq( ZnkVar_cstr(ua_var), "UNTOUCH" ) ? default_ua : ZnkVar_cstr(ua_var);

			if( RanoHtpModifier_modifySendHdrs( htp_hdrs.vars_, ua, msg ) ){
				if( msg ){
					ZnkStr_add( msg, "  RanoHtpModifier_modifySendHdrs : true\n" );
				}
			} else {
				if( msg ){
					ZnkStr_add( msg, "  RanoHtpModifier_modifySendHdrs : false\n" );
				}
			}
		}
	}

	result = RanoHtpBoy_cipher_get_with404( hostname, unesc_req_urp, target,
			&htp_hdrs,
			parent_proxy,
			tmpdir, cachebox, result_filename,
			is_without_404, status_code, is_https,
			NULL, msg );

	if( !result && msg ){
		ZnkStr_addf( msg,
				"  RanoHtpBoy_cipher_get_with404 : result=[%d] hostname=[%s] req_urp=[%s]<br>"
				"                    : target=[%s] parent_proxy=[%s] tmpdir=[%s]<br>"
				"                    : result_filename=[%s].<br>",
				result, hostname, unesc_req_urp, target, parent_proxy, tmpdir, ZnkStr_cstr(result_filename) );
	}

	ZnkHtpHdrs_dispose( &htp_hdrs );
	return result;
}
