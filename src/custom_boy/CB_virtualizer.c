#include <CB_virtualizer.h>
#include <CB_config.h>
#include <CB_fgp_info.h>
#include <CB_ua_conf.h>
#include <CB_snippet.h>
#include <CB_wgt_prim.h>
#include <CB_finger.h>
#include <CB_vars_base.h>

#include <Rano_module_ary.h>
#include <Rano_module.h>
#include <Rano_type.h>
#include <Rano_post.h>
#include <Rano_htp_boy.h>
#include <Rano_htp_modifier.h>
#include <Rano_html_ui.h>
#include <Rano_cgi_util.h>

#include <Znk_str.h>
#include <Znk_str_ex.h>
#include <Znk_str_fio.h>
#include <Znk_str_ptn.h>
#include <Znk_s_base.h>
#include <Znk_mem_find.h>
#include <Znk_stdc.h>
#include <Znk_md5.h>
#include <Znk_dir.h>
#include <Znk_vpod.h>
#include <Znk_thread.h>
#include <Znk_varp_ary.h>
#include <Znk_envvar.h>
#include <Znk_missing_libc.h>
#include <Znk_htp_post.h>
#include <Znk_htp_util.h>
#include <Znk_htp_hdrs.h>
#include <Znk_myf.h>
#include <Znk_bird.h>
#include <Znk_socket.h>
#include <Znk_cookie.h>
#include <Znk_str_path.h>
#include <Znk_str_ptn.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#if defined(Znk_TARGET_WINDOWS)
static bool st_is_dos_path = true;
#else
static bool st_is_dos_path = false;
#endif

static char   st_filters_dir[ 256 ] = "filters";
static double st_RE_random_factor = 0.7;



static const char*
getHostnameAndRequrp( char* hostname, size_t hostname_size, ZnkStr req_urp, const char* cb_src )
{
	const char* p;
	const char* q;
	ZnkHtpURL_unescape_toStr( req_urp, cb_src, Znk_strlen(cb_src) );
	p = RanoCGIUtil_skipProtocolPrefix( ZnkStr_cstr(req_urp) );
	Znk_printf( "getHostnameAndRequrp : cb_src=[%s] p=[%s].\n", cb_src, p );
	q = strchr( p, '/' );
	if( q ){
		ZnkS_copy( hostname, hostname_size, p, q-p );
		ZnkStr_set( req_urp, q );
	} else {
		ZnkS_copy( hostname, hostname_size, p, Znk_NPOS );
		ZnkStr_set( req_urp, "/" );
	}
	return cb_src;
}

static bool
getThreURI( const char* result_filename, ZnkStr thre_uri, const char* board_id )
{
	ZnkFile fp = Znk_fopen( result_filename, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				/* eof */
				break;
			}
			if( strstr( ZnkStr_cstr(line), "<a href='res/" ) ){
				if( ZnkStrPtn_getBetween( thre_uri, (uint8_t*)ZnkStr_cstr(line), ZnkStr_leng(line), "<a href='", "' " ) ){
					/* lineはもう不要なのでこれを一時バッファとして用いる */
					ZnkStr_setf( line, "/%s/%s", board_id, ZnkStr_cstr(thre_uri) );
					ZnkStr_swap( line, thre_uri );
					break;
				}
			}
		}
		ZnkStr_delete( line );
		Znk_fclose( fp );
	} else {
		return false;
	}
	return true;
}

static bool
getCookie( ZnkVarpAry cookie, ZnkStr posttime, ZnkStr cfduid )
{
	bool result = false;
	const char* tmpdir = st_is_dos_path ? ".\\tmp\\" : "./tmp/";
	char cookie_file[ 256 ];
	const char* negotiating_target = CBConfig_theNegotiatingTarget();
	Znk_snprintf( cookie_file, sizeof(cookie_file), "%s%s/cookie.txt", tmpdir, negotiating_target );
	if( ZnkCookie_load( cookie, cookie_file ) ){
		ZnkVarp varp;
		varp = ZnkVarpAry_find_byName( cookie, "posttime", Znk_strlen_literal( "posttime" ), false );
		if( varp ){
			ZnkStr_set( posttime, ZnkVar_cstr(varp) );
		}
		varp = ZnkVarpAry_find_byName( cookie, "__cfduid", Znk_strlen_literal( "__cfduid" ), false );
		if( varp ){
			ZnkStr_set( cfduid, ZnkVar_cstr(varp) );
		}
		result = true;
	}
	return result;
}


static bool
getCaco2( const char* srv_name, ZnkStr caco,
		const char* ua, ZnkVarpAry cookie, ZnkStr thre_uri,
		const char* parent_proxy, ZnkStr msg )
{
	bool result = false;
	const char* target = CBConfig_theNegotiatingTarget();
	struct ZnkHtpHdrs_tag htp_hdrs = { 0 };
	char hostname[ 256 ];
	char referer[ 4096 ];
	ZnkVarp varp;
	ZnkStr req_urp         = ZnkStr_new( "/bin/cachemt7.php" );
	ZnkStr result_filename = ZnkStr_new( "caco.txt" );

	ZnkHtpHdrs_compose( &htp_hdrs );
	Znk_snprintf( hostname, sizeof(hostname), "%s.2chan.net", srv_name );
	Znk_snprintf( referer, sizeof(referer), "http://%s%s", hostname, ZnkStr_cstr(thre_uri) );

	{
		ZnkSRef sref = { 0 };

		varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"Host",   Znk_strlen_literal("Host"),
				hostname, Znk_strlen(hostname), true );

		varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"User-Agent", Znk_strlen_literal("User-Agent"),
				ua, Znk_strlen(ua), true );

		ZnkSRef_set_literal( &sref, "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" );
		varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"Accept", Znk_strlen_literal("Accept"),
				sref.cstr_, sref.leng_, true );

		ZnkSRef_set_literal( &sref, "ja,en-US;q=0.7,en;q=0.3" );
		varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"Accept-Language", Znk_strlen_literal("Accept-Language"),
				sref.cstr_, sref.leng_, true );

		ZnkSRef_set_literal( &sref, "gzip, deflate" );
		varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"Accept-Encoding", Znk_strlen_literal("Accept-Encoding"),
				sref.cstr_, sref.leng_, true );

		varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"Referer", Znk_strlen_literal("Referer"),
				referer, Znk_strlen(referer), true );

		/* Cookie */
		{
			const size_t size = ZnkVarpAry_size( cookie );
			if( size ){
				size_t idx;
				ZnkStr str = ZnkStr_new( "" );
				for( idx=0; idx<size; ++idx ){
					varp = ZnkVarpAry_at( cookie, idx );
					ZnkStr_addf( str, "%s=%s", ZnkStr_cstr(varp->name_), ZnkVar_cstr(varp) );
					if( idx < size-1 ){
						ZnkStr_add( str, "; " );
					}
				}
				varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
						"Cookie", Znk_strlen_literal("Cookie"),
						ZnkStr_cstr(str), ZnkStr_leng(str), true );
				ZnkStr_delete( str );
			}
		}

		ZnkSRef_set_literal( &sref, "keep-alive" );
		varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"Connection", Znk_strlen_literal("Connection"),
				sref.cstr_, sref.leng_, true );

	}

	RanoHtpModifier_modifySendHdrs( htp_hdrs.vars_, ua, msg );

	{
		const char* tmpdir   = "./tmp/";
		const char* cachebox = "./cachebox/";
		result = RanoHtpBoy_do_get( hostname, ZnkStr_cstr(req_urp), target,
				&htp_hdrs,
				parent_proxy,
				tmpdir, cachebox, result_filename );
		if( !result ){
			ZnkStr_addf( msg,
					"  RanoHtpBoy_do_get : result=[%d] hostname=[%s] req_urp=[%s]<br>"
					"                    : target=[%s] parent_proxy=[%s] tmpdir=[%s]<br>"
					"                    : result_filename=[%s].<br>",
					result, hostname, ZnkStr_cstr(req_urp), target, parent_proxy, tmpdir, ZnkStr_cstr(result_filename) );
		}

	}

	{
		ZnkFile fp = Znk_fopen( ZnkStr_cstr(result_filename), "rb" );
		if( fp ){
			ZnkStr line = ZnkStr_new( "" );
			ZnkStrFIO_fgets( line, 0, 4096, fp );
			ZnkStrPtn_getBetween( caco, (uint8_t*)ZnkStr_cstr(line), ZnkStr_leng(line), "\"", "\"" );
			result = true;
			ZnkStr_delete( line );
			Znk_fclose( fp );
		}
	}

	ZnkHtpHdrs_dispose( &htp_hdrs );
	ZnkStr_delete( req_urp );
	ZnkStr_delete( result_filename );
	return result;

}


Znk_INLINE ZnkVarp
refPostVar( ZnkMyf myf, const char* var_name )
{
	return ZnkMyf_refVar( myf, "post_vars", var_name );
}
Znk_INLINE ZnkVarp
refCookieVar( ZnkMyf myf, const char* var_name )
{
	return ZnkMyf_refVar( myf, "cookie_vars", var_name );
}
static void
updateSendMyf( ZnkMyf myf, ZnkStr cfduid, ZnkStr caco, ZnkStr posttime, bool all_cookie_clear )
{
	ZnkVarp varp;

	varp = refCookieVar( myf, "__cfduid" );
	if( varp ){
		ZnkVar_set_val_Str( varp, ZnkStr_cstr(cfduid), ZnkStr_leng(cfduid) );
	}

	/***
	 * この変数は現在ではcacoコードの値がそのまま格納されるようである.
	 */
	varp = refPostVar( myf, "pthc" );
	if( varp ){
		ZnkVar_set_val_Str( varp, ZnkStr_cstr(caco), ZnkStr_leng(caco) );
	}

	/***
	 * この変数は現状では、古いcacoコードの値を出来るだけ残すことを狙いとしているものと思われる.
	 * 実際、Cookieやキャッシュをクリアしたとしても localStorage(futabapt)の値が消去されていない場合は、
	 * 古いcaco値がここに格納される仕組みとなっている.
	 * おそらく、pthcの値が変わりpthbには以前のcacoコード値がそのまま残っていた場合に
	 * futaba.php側でのユーザ特定アルゴリズムはpthcよりpthbの値を優先するものと思われる.
	 *
	 * 従って、pthbへ我々が設定すべき値は空値か現在のpthcの値ということになるが、
	 * とりあえずこのshuffle処理においては空値を設定しておけばよい.
	 */
	varp = refPostVar( myf, "pthb" );
	if( varp ){
		ZnkVar_set_val_Str( varp, "", 0 );
	}

	/***
	 * この変数は実際にはPOST変数群の中に存在せず、localStorageにこの名前で存在する.
	 * これは初回のPOST時にcacoコード値によって初期化される.
	 * そして２回目のPOSTにおいて参照され、pthb値として使われる.
	 * ここでは、on_post の呼び出しによってこの挙動を実現するものとする.
	 *
	 * とりあえずこのshuffle処理においてはlocalStorageの完全消去をシミュレートすればよい.
	 * つまり空値を設定しておけばよい.
	 */
	varp = refPostVar( myf, "USERS_futabapt" );
	if( varp ){
		ZnkVar_set_val_Str( varp, "", 0 );
	}

	/***
	 * この値は最終的にptuaへとセットされる.
	 */
	varp = refPostVar( myf, "USERS_ptua" );
	if( varp ){
		ZnkVarp varp_ptua = refPostVar( myf, "ptua" );
		ZnkVar_set_val_Str( varp, ZnkVar_cstr(varp_ptua), Znk_NPOS );
	}

	varp = refCookieVar( myf, "posttime" );
	if( varp ){
		ZnkVar_set_val_Str( varp, ZnkStr_cstr(posttime), ZnkStr_leng(posttime) );
	}

	/***
	 * pwdcの内容はここで一旦確実に消去しておくべきであろう.
	 */
	varp = refCookieVar( myf, "pwdc" );
	if( varp ){
		ZnkVar_set_val_Str( varp, "", 0 );
	}

	/***
	 * 試験的:
	 * その他のCookieも強制消去したい場合.
	 */
	if( all_cookie_clear ){
		ZnkVarpAry cookie_vars = ZnkMyf_find_vars( myf, "cookie_vars" );
		if( cookie_vars ){
			size_t idx = Znk_NPOS;
			idx = ZnkVarpAry_findIdx_byName_literal( cookie_vars, "verifyc", false );
			if( idx != Znk_NPOS ){
				ZnkVarpAry_erase_byIdx( cookie_vars, idx );
			}
		}
	}
}

static bool
isMainCtxVarName( const char* name )
{
	static const char* tbl[] = {
		"UserAgent",
		"pthb",
		"pthc",
		"pthd",
		"ptua",
		"scsz",
		"pwd",
		"flrv",
		"flvv",
	};
	const size_t size = Znk_NARY( tbl );
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		if( ZnkS_eq( name, tbl[idx] ) ){
			/* found */
			return true;
		}
	}
	/* not found */
	return false;
}


void
CBVirtualizer_copyMainCtx_byVars( ZnkVarpAry dst, const ZnkVarpAry src )
{
	size_t idx;
	const size_t size = ZnkVarpAry_size( src );
	ZnkVarp src_varp;
	ZnkStr tmp = ZnkStr_new( "" );
	const char* name = NULL;
	for( idx=0; idx<size; ++idx ){
		src_varp = ZnkVarpAry_at( src, idx );
		name = ZnkVar_name_cstr( src_varp );
		if( isMainCtxVarName( name ) ){
			ZnkStr_clear( tmp );
			ZnkHtpURL_unescape_toStr( tmp, ZnkVar_cstr(src_varp), ZnkVar_str_leng(src_varp) );
			CBVarsBase_registStr_toVars( dst, name, ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );
		}
	}
	ZnkStr_delete( tmp );
}

void
CBVirtualizer_initVars_byFutabaSendMyf( ZnkVarpAry main_vars, const char* moai_dir )
{
	char       path[ 256 ];
	ZnkMyf     send_myf = ZnkMyf_create();
	ZnkVarpAry vars = NULL;
	ZnkVarp    ua = NULL;
	CBConfigInfo* info = CBConfig_theInfo(); 

	CBVarsBase_registStr_toVars( main_vars, "UserAgent", "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "pthb", "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "pthc", "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "pthd", "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "ptua", "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "scsz", "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "pwd",  "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "flrv", "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "flvv", "", 0 );

	Znk_snprintf( path, sizeof(path), "%s%s/%s", moai_dir, st_filters_dir, info->send_myf_filename_ );

	ZnkMyf_load( send_myf, path );

	vars = ZnkMyf_find_vars( send_myf, "header_vars" );
	if( vars ){
		ua = ZnkVarpAry_find_byName( vars, "User-Agent", Znk_NPOS, false );
		//ZnkVar_set_val_Str( ua, ZnkStr_cstr(ua_str), Znk_NPOS );
	}

	vars = ZnkMyf_find_vars( send_myf, "post_vars" );
	if( vars ){
		size_t idx;
		const size_t size = ZnkVarpAry_size( main_vars );
		const char* var_name = NULL;
		ZnkVarp fltp;
		for( idx=0; idx<size; ++idx ){
			ZnkVarp varp = ZnkVarpAry_at( main_vars, idx );
			var_name = ZnkVar_name_cstr(varp);
			fltp = ZnkVarpAry_find_byName( vars, var_name, Znk_NPOS, false );
			if( ZnkS_eq(var_name, "UserAgent" ) ){
				ZnkVar_set_val_Str( varp, ZnkVar_cstr( ua ), Znk_NPOS );
				continue;
			}
			if( fltp ){
				ZnkVar_set_val_Str( varp, ZnkVar_cstr( fltp ), Znk_NPOS );
			}
		}
	}
	ZnkMyf_destroy( send_myf );
}

static bool
checkCBVars( const ZnkVarpAry cb_vars, const char* target, ZnkStr msg, ZnkStr lacking_var )
{
	ZnkVarp vp;

	if( ZnkS_eq(target, "futaba") ){
		vp = ZnkVarpAry_find_byName( cb_vars, "ptua", Znk_NPOS, false );
		if( vp == NULL || ZnkVar_str_leng( vp ) == 0 ){
			ZnkStr_set( lacking_var, "ptua" );
			return false;
		}
		vp = ZnkVarpAry_find_byName( cb_vars, "scsz", Znk_NPOS, false );
		if( vp == NULL || ZnkVar_str_leng( vp ) == 0 ){
			ZnkStr_set( lacking_var, "scsz" );
			return false;
		}
		vp = ZnkVarpAry_find_byName( cb_vars, "flrv", Znk_NPOS, false );
		if( vp == NULL || ZnkVar_str_leng( vp ) == 0 ){
			ZnkStr_set( lacking_var, "flrv" );
			return false;
		}
		vp = ZnkVarpAry_find_byName( cb_vars, "flvv", Znk_NPOS, false );
		if( vp == NULL || ZnkVar_str_leng( vp ) == 0 ){
			ZnkStr_set( lacking_var, "flvv" );
			return false;
		}
	}
	return true;
}


bool
CBVirtualizer_initiateAndSave( RanoCGIEVar* evar, const ZnkVarpAry cb_vars,
		ZnkStr msg, const char* cb_src, ZnkStr caco, ZnkStr ua_str, ZnkStr lacking_var,
		bool all_cookie_clear )
{
	const char* moai_dir = CBConfig_moai_dir();
	bool        result   = false;
	const char* tmpdir = st_is_dos_path ? ".\\tmp\\" : "./tmp/";

	char hostname[ 1024 ] = "";
	ZnkStr req_urp = ZnkStr_new( "" );
	const char* target = CBConfig_theNegotiatingTarget();
	struct ZnkHtpHdrs_tag htp_hdrs = { 0 };
	ZnkVarp varp;
	ZnkStr pst_str = ZnkStr_new( "" );
	ZnkStr src = ZnkStr_new( "" );
	CBConfigInfo* info = CBConfig_theInfo(); 

	ZnkStr_add( msg, "<b>Step2:</b><br>" );

	if( ZnkStr_empty( ua_str ) ){
		ZnkStr_add( msg, "&nbsp;&nbsp;Your input is Invalid : UserAgent is empty.<br>" );
		ZnkStr_set( lacking_var, "UserAgent" );
		goto FUNC_END;
	}
	if( !checkCBVars( cb_vars, target, msg, lacking_var ) ){
		ZnkStr_addf( msg, "&nbsp;&nbsp;Your input is Invalid : %s is empty.<br>", ZnkStr_cstr( lacking_var ) );
		goto FUNC_END;
	}

	if( cb_src ){
		ZnkStr_set( src, getHostnameAndRequrp( hostname, sizeof(hostname), req_urp, cb_src ) );
	}
	ZnkStr_addf( msg, "&nbsp;&nbsp;We are accessing to %s, and negotiating...<br>", CBConfig_theNegotiatingTarget() );

	{
		char path[256];
		Znk_snprintf(path,sizeof(path), "%s%s", tmpdir, target);
		ZnkDir_mkdirPath( path, Znk_NPOS, '\\', NULL );
	}

	{
		ZnkSRef sref = { 0 };
		ZnkHtpHdrs_compose( &htp_hdrs );

		varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"Host",   Znk_strlen_literal("Host"),
				hostname, Znk_strlen(hostname), true );

		varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"User-Agent", Znk_strlen_literal("User-Agent"),
				ZnkStr_cstr(ua_str), ZnkStr_leng(ua_str), true );

		ZnkSRef_set_literal( &sref, "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" );
		varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"Accept", Znk_strlen_literal("Accept"),
				sref.cstr_, sref.leng_, true );

		ZnkSRef_set_literal( &sref, "ja,en-US;q=0.7,en;q=0.3" );
		varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"Accept-Language", Znk_strlen_literal("Accept-Language"),
				sref.cstr_, sref.leng_, true );

		ZnkSRef_set_literal( &sref, "gzip, deflate" );
		varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"Accept-Encoding", Znk_strlen_literal("Accept-Encoding"),
				sref.cstr_, sref.leng_, true );

		ZnkSRef_set_literal( &sref, "http://www.2chan.net" );
		varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"Referer", Znk_strlen_literal("Referer"),
				sref.cstr_, sref.leng_, true );

		/* 最初はCookieは設定しない */

		ZnkSRef_set_literal( &sref, "keep-alive" );
		varp = ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"Connection", Znk_strlen_literal("Connection"),
				sref.cstr_, sref.leng_, true );

	}

	{
		char         content_type[ 1024 ] = "application/octet-stream";
		const char*  parent_proxy  = "NONE";
		char         body_fname[ 256 ] = "";
		ZnkVarpAry   cookie   = ZnkVarpAry_create( true );
		ZnkStr       cfduid   = ZnkStr_new( "" );
		ZnkStr       posttime = ZnkStr_new( "" );
		ZnkStr       result_filename = ZnkStr_new( "result.dat" );

		/* 内容消去 */
		{
			ZnkFile      fp = NULL;
			Znk_snprintf( body_fname, sizeof(body_fname), "%s%s/send_body.dat", tmpdir, target );
			fp = Znk_fopen( body_fname, "wb" );
			if( fp ){ Znk_fclose( fp ); }
		}

		{
			static const char* tmpdir   = "./tmp/";
			static const char* cachebox = "./cachebox/";
			result = RanoHtpBoy_do_get( hostname, ZnkStr_cstr(req_urp), target,
					&htp_hdrs,
					parent_proxy,
					tmpdir, cachebox, result_filename );
			if( !result ){
				ZnkStr_addf( msg,
						"  RanoHtpBoy_do_get : result=[%d] hostname=[%s] req_urp=[%s]<br>"
						"                    : target=[%s] parent_proxy=[%s] tmpdir=[%s]<br>"
						"                    : result_filename=[%s].<br>",
						result, hostname, ZnkStr_cstr(req_urp), target, parent_proxy, tmpdir, ZnkStr_cstr(result_filename) );
			}
		}

		RanoHtpBoy_readRecvHdrVar( "Content-Type", content_type, sizeof(content_type), target, tmpdir );

		{
			ZnkStr thre_uri = ZnkStr_new( "" );
			const char* parent_proxy = "";
			if( getThreURI( ZnkStr_cstr(result_filename), thre_uri, "b" ) ){
				getCookie( cookie, posttime, cfduid );
			}
			if( ZnkS_eq(target, "futaba") ){
				getCaco2( "may", caco, ZnkStr_cstr(ua_str), cookie, thre_uri, parent_proxy, msg );
			}
			ZnkStr_addf( msg, "&nbsp;&nbsp;Using Host : [%s]<br>", hostname );
			ZnkStr_addf( msg, "&nbsp;&nbsp;Using Thread : [%s]<br>", ZnkStr_cstr(thre_uri) );
			ZnkStr_addf( msg, "&nbsp;&nbsp;Getting posttime   : [%s]<br>", ZnkStr_cstr(posttime) );
			ZnkStr_addf( msg, "&nbsp;&nbsp;Getting cfduid     : [%s]<br>", ZnkStr_cstr(cfduid) );
			if( ZnkS_eq(target, "futaba") ){
				ZnkStr_addf( msg, "&nbsp;&nbsp;Getting caco code(issued by bin/cachemt7.php) : [%s]<br>", ZnkStr_cstr(caco) );
			}
			ZnkStr_delete( thre_uri );
		}

		ZnkVarpAry_destroy( cookie );

		{
			char       path[ 256 ];
			ZnkMyf     send_myf = ZnkMyf_create();
			ZnkVarpAry vars = NULL;
			Znk_snprintf( path, sizeof(path), "%s%s/%s", moai_dir, st_filters_dir, info->send_myf_filename_ );

			ZnkMyf_load( send_myf, path );

			vars = ZnkMyf_find_vars( send_myf, "header_vars" );
			if( vars ){
				ZnkVarp ua = ZnkVarpAry_find_byName( vars, "User-Agent", Znk_NPOS, false );
				ZnkVar_set_val_Str( ua, ZnkStr_cstr(ua_str), Znk_NPOS );
			}

			vars = ZnkMyf_find_vars( send_myf, "post_vars" );
			if( vars ){
				size_t idx;
				const size_t size = ZnkVarpAry_size( cb_vars );
				const char* cb_var_name = NULL;
				ZnkVarp fltp;
				for( idx=0; idx<size; ++idx ){
					varp = ZnkVarpAry_at( cb_vars, idx );
					cb_var_name = ZnkVar_name_cstr(varp);
					fltp = ZnkVarpAry_find_byName( vars, cb_var_name, Znk_NPOS, false );
					if( ZnkS_eq(cb_var_name, "UserAgent" ) ){
						continue;
					}
					if( ZnkS_isBegin(cb_var_name, "Moai_" ) ){
						continue;
					}
					if( ZnkS_isBegin(cb_var_name, "cb_" ) ){
						continue;
					}
					if( ZnkS_isBegin(cb_var_name, "Fgp_" ) ){
						continue;
					}
					if( ZnkS_isBegin(cb_var_name, "Snp_" ) ){
						continue;
					}
					if( fltp ){
						/* 上書き */
						ZnkVar_set_val_Str( fltp, ZnkVar_cstr( varp ), Znk_NPOS );
					} else {
						/* 新規追加 */
						ZnkVarp new_varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
						ZnkStr_assign( new_varp->name_, 0, cb_var_name, Znk_NPOS );
						ZnkVar_set_val_Str( new_varp, ZnkVar_cstr(varp), Znk_NPOS );
						ZnkVarpAry_push_bk( vars, new_varp );
					}
				}
			}

			updateSendMyf( send_myf, cfduid, caco, posttime, all_cookie_clear );

			ZnkMyf_save( send_myf, info->send_save_myf_filename_ );
			ZnkMyf_save( send_myf, path );
			ZnkStr_addf( msg, "&nbsp;&nbsp;Saving your %s/%s<br>", st_filters_dir, info->send_myf_filename_ );
			ZnkMyf_destroy( send_myf );
		}

		ZnkStr_delete( cfduid );
		ZnkStr_delete( posttime );
		ZnkStr_delete( result_filename );
	}
FUNC_END:

	ZnkStr_delete( req_urp );
	ZnkHtpHdrs_dispose( &htp_hdrs );
	ZnkStr_delete( pst_str );
	ZnkStr_delete( src );

	return result;
}

void
CBVirtualizer_registBird_byFutabaSendMyf( ZnkBird bird, const char* moai_dir )
{
	char       path[ 256 ];
	ZnkMyf     send_myf = ZnkMyf_create();
	ZnkVarpAry vars = NULL;
	CBConfigInfo* info = CBConfig_theInfo(); 
	Znk_snprintf( path, sizeof(path), "%s%s/%s", moai_dir, st_filters_dir, info->send_myf_filename_ );

	ZnkMyf_load( send_myf, path );

	vars = ZnkMyf_find_vars( send_myf, "header_vars" );
	if( vars ){
		ZnkVarp ua = ZnkVarpAry_find_byName( vars, "User-Agent", Znk_NPOS, false );
		ZnkBird_regist( bird, "user_agent", ZnkVar_cstr(ua) );
	}

	vars = ZnkMyf_find_vars( send_myf, "post_vars" );
	if( vars ){
		size_t idx;
		const size_t size = ZnkVarpAry_size( vars );
		ZnkVarp varp;
		for( idx=0; idx<size; ++idx ){
			varp = ZnkVarpAry_at( vars, idx );
			if( ZnkS_eq(ZnkVar_name_cstr(varp), "pthb" ) ){
				ZnkBird_regist( bird, "pthb", ZnkVar_cstr(varp) );
			}
			if( ZnkS_eq(ZnkVar_name_cstr(varp), "pthc" ) ){
				ZnkBird_regist( bird, "pthc", ZnkVar_cstr(varp) );
			}
			if( ZnkS_eq(ZnkVar_name_cstr(varp), "pthd" ) ){
				ZnkBird_regist( bird, "pthd", ZnkVar_cstr(varp) );
			}
			if( ZnkS_eq(ZnkVar_name_cstr(varp), "ptua" ) ){
				ZnkBird_regist( bird, "ptua", ZnkVar_cstr(varp) );
			}
			if( ZnkS_eq(ZnkVar_name_cstr(varp), "scsz" ) ){
				ZnkBird_regist( bird, "scsz", ZnkVar_cstr(varp) );
			}
			if( ZnkS_eq(ZnkVar_name_cstr(varp), "pwd" ) ){
				ZnkBird_regist( bird, "pwd", ZnkVar_cstr(varp) );
			}
			if( ZnkS_eq(ZnkVar_name_cstr(varp), "flrv" ) ){
				ZnkBird_regist( bird, "flrv", ZnkVar_cstr(varp) );
			}
			if( ZnkS_eq(ZnkVar_name_cstr(varp), "flvv" ) ){
				ZnkBird_regist( bird, "flvv", ZnkVar_cstr(varp) );
			}
		}
	}
}

static void
registBird_byCBVar( ZnkBird bird, const char* bird_var_name, const char* cb_var_name, ZnkVarp varp, ZnkStr tmp )
{
	if( ZnkS_eq(ZnkVar_name_cstr(varp), cb_var_name ) ){
		ZnkStr_clear( tmp );
		ZnkHtpURL_unescape_toStr( tmp, ZnkVar_cstr(varp), ZnkVar_str_leng(varp) );
		ZnkBird_regist( bird, bird_var_name, ZnkStr_cstr(tmp) );
	}
}
void
CBVirtualizer_registBird_byVars( ZnkBird bird, ZnkVarpAry vars )
{
	if( vars ){
		size_t idx;
		const size_t size = ZnkVarpAry_size( vars );
		ZnkVarp varp;
		ZnkStr tmp = ZnkStr_new( "" );
		for( idx=0; idx<size; ++idx ){
			varp = ZnkVarpAry_at( vars, idx );
			registBird_byCBVar( bird, "user_agent", "UserAgent", varp, tmp );
			registBird_byCBVar( bird, "pthb", "pthb", varp, tmp );
			registBird_byCBVar( bird, "pthc", "pthc", varp, tmp );
			registBird_byCBVar( bird, "pthd", "pthd", varp, tmp );
			registBird_byCBVar( bird, "ptua", "ptua", varp, tmp );
			registBird_byCBVar( bird, "scsz", "scsz", varp, tmp );
			registBird_byCBVar( bird, "pwd",  "pwd", varp, tmp );
			registBird_byCBVar( bird, "flrv", "flrv", varp, tmp );
			registBird_byCBVar( bird, "flvv", "flvv", varp, tmp );
		}
		ZnkStr_delete( tmp );
	}
}


static uint64_t
generatePtuaResult( CBUAInfo ua_info )
{
#define ADDBIT_SNP( key ) \
	if( Snippet_##key( ua_info ) ){ ptua64 |= ( UINT64_C( 1 ) << i ); } ++i;

	uint64_t ptua64 = 0;
	int i = 0;

	ADDBIT_SNP( Snp_LtIE6 );
	ADDBIT_SNP( Snp_LtIE7 );
	ADDBIT_SNP( Snp_LtIE8 );
	ADDBIT_SNP( Snp_IE9 );
	ADDBIT_SNP( Snp_IE10 );
	ADDBIT_SNP( Snp_IE11 );
	ADDBIT_SNP( Snp_Trident );
	ADDBIT_SNP( Snp_Edge );
	ADDBIT_SNP( Snp_Gecko );
	ADDBIT_SNP( Snp_MozillaSidebar );
	ADDBIT_SNP( Snp_NavigatorOnline );
	ADDBIT_SNP( Snp_SessionStorage );
	ADDBIT_SNP( Snp_FunctionX5 );
	ADDBIT_SNP( Snp_DocCurrentScript );
	ADDBIT_SNP( Snp_EventSource );
	ADDBIT_SNP( Snp_Crypto );
	ADDBIT_SNP( Snp_PerformanceNow );
	ADDBIT_SNP( Snp_AudioContext );
	ADDBIT_SNP( Snp_IndexedDB );
	ADDBIT_SNP( Snp_WindowStyles );
	ADDBIT_SNP( Snp_SendBeacon );
	ADDBIT_SNP( Snp_GetGamepads );
	ADDBIT_SNP( Snp_NavLanguages );
	ADDBIT_SNP( Snp_NavMediaDevices );
	ADDBIT_SNP( Snp_WinCaches );
	ADDBIT_SNP( Snp_CreateImageBitmap );
	ADDBIT_SNP( Snp_Onstorage );
	ADDBIT_SNP( Snp_NavGetBattery );
	ADDBIT_SNP( Snp_Presto );
	ADDBIT_SNP( Snp_LtChrome14 );
	ADDBIT_SNP( Snp_ChromiumInFutaba );
	ADDBIT_SNP( Snp_Touch );
	ADDBIT_SNP( Snp_FacileMobile );

	return ptua64;
}

static void
generateVirtualUSERS( RanoCGIEVar* evar, ZnkVarpAry cb_vars, struct CBUAInfo_tag* ua_info,
		ZnkStr ua_str, ZnkStr RE_key, CBFgpInfo fgp_info, ZnkStr scsz_str, ZnkStr msg, ZnkStr category )
{

	const char* tmpdir = st_is_dos_path ? ".\\tmp\\" : "./tmp/";

	const char* target = CBConfig_theNegotiatingTarget();
	struct ZnkHtpHdrs_tag htp_hdrs = { 0 };
	ZnkStr pst_str  = ZnkStr_new( "" );
	ZnkStr src      = ZnkStr_new( "" );
	ZnkStr fgp_str  = ZnkStr_new( "" );
	ZnkStr browser  = ZnkStr_new( "" );
	int major_ver = 0;
//	ZnkFile fp = NULL;

	ZnkStr_add( msg, "<b>Step1:</b><br>" );
	ZnkStr_add( msg, "&nbsp;&nbsp;Basically post variables have been generated.</b><br>" );



	{
		char path[256];
		Znk_snprintf(path,sizeof(path), "%s%s", tmpdir, target);
		ZnkDir_mkdirPath( path, Znk_NPOS, '\\', NULL );
	}

	CBWgtRand_init();

	{
		ZnkVarp varp = ZnkVarpAry_find_byName( cb_vars, "cb_category", Znk_NPOS, false );
		if( varp == NULL ){
			goto FUNC_END;
		}
		ZnkStr_set( category, ZnkVar_cstr(varp) );
		if(  ZnkStr_eq( category, "pc" )
		  || ZnkStr_eq( category, "mobile" )
		  || ZnkStr_eq( category, "random" ) )
		{ 
			CBUAConf_getBrowser_fromCategory( browser, ZnkStr_cstr(category) );
		} else {
			ZnkStr_set( browser, ZnkStr_cstr(category) );
		}
	}

	if( !CBUAConf_getUA( ua_str, ZnkStr_cstr(browser), &major_ver, &ua_info->os_, &ua_info->machine_, &ua_info->browser_ ) ){
		ZnkStr_add( msg, "&nbsp;&nbsp;<b><font color=\"#ff0000\">Error : CBUAConf_getUA is failure.</font></b><br>" );
		goto FUNC_END;
	}
	if( !CBUAConf_getScsz( scsz_str ) ){
		ZnkStr_add( msg, "&nbsp;&nbsp;<b><font color=\"#ff0000\">Error : CBUAConf_getScsz is failure.</font></b><br>" );
		goto FUNC_END;
	}
	{
		unsigned int sc_width  = 0;
		unsigned int sc_height = 0;
		unsigned int sc_depth  = 0;
		sscanf( ZnkStr_cstr(scsz_str), "%ux%ux%u", &sc_width, &sc_height, &sc_depth );
		ua_info->sc_width_  = sc_width;
		ua_info->sc_height_ = sc_height;
		ua_info->sc_depth_  = sc_depth;
	}


	ua_info->major_ver_ = major_ver;

	ZnkStr_set( fgp_info->Fgp_userAgent_, ZnkStr_cstr(ua_str) );
	CBFgpInfo_generate_byCBUAInfo( fgp_info, RE_key, ua_info, st_RE_random_factor );

	ZnkStr_addf( msg, "&nbsp;&nbsp;Selected Browser/Ver : [%s/%d]<br>", CBUABrowser_getCStr( ua_info->browser_ ), ua_info->major_ver_ );
	ZnkStr_addf( msg, "&nbsp;&nbsp;Selected OS/Machine : [%s/%s]<br>", CBUAOS_getCStr( ua_info->os_ ), CBUAMachine_getCStr( ua_info->machine_ ) );
	ZnkStr_addf( msg, "&nbsp;&nbsp;Selected RE_key : [%s].</b><br>", ZnkStr_cstr(RE_key) );
	ZnkStr_addf( msg, "&nbsp;&nbsp;Selected SC(%zux%zux%zu)</b><br>", ua_info->sc_width_, ua_info->sc_height_, ua_info->sc_depth_ );

FUNC_END:
	ZnkHtpHdrs_dispose( &htp_hdrs );
	ZnkStr_delete( fgp_str );
	ZnkStr_delete( pst_str );
	ZnkStr_delete( src );
	ZnkStr_delete( browser );
}


static void
reportCookieUI( ZnkStr cookie_recs, ZnkStrAry known_names, const char* moai_dir )
{
	const char* hint_base = "/moai2.0/customboy_hints_Cookie.html#";
	char       path[ 256 ];
	ZnkMyf     send_myf = ZnkMyf_create();
	ZnkVarpAry vars = NULL;
	CBConfigInfo* info = CBConfig_theInfo(); 

	Znk_snprintf( path, sizeof(path), "%s%s/%s", moai_dir, st_filters_dir, info->send_myf_filename_ );

	ZnkMyf_load( send_myf, path );

	ZnkStr_clear( cookie_recs );
	vars = ZnkMyf_find_vars( send_myf, "cookie_vars" );
	if( vars ){
		size_t idx;
		const size_t size = ZnkVarpAry_size( vars );
		const char* name = NULL;
		const char* val  = NULL;
		const char* cls  = "odd";
		ZnkVarp varp;
		for( idx=0; idx<size; ++idx ){
			varp = ZnkVarpAry_at( vars, idx );
			name = ZnkVar_name_cstr( varp );
			val  = ZnkVar_cstr( varp );
			cls  = ( idx % 2 ) ? "odd" : "evn";
			if( ZnkStrAry_find( known_names, 0, name, Znk_NPOS ) != Znk_NPOS ){
				ZnkStr_addf( cookie_recs,
						"<tr class=%s><td><b>%s</b></td><td><input type=text id=ck_%s name=ck_%s size=\"60\" value=\"%s\">"
						"&nbsp; <a href=\"%sck_%s\" target=\"_blank\">Hint</a></td></tr>\n",
						cls, name, name, name, val,
						hint_base, name );
			} else {
				ZnkStr_addf( cookie_recs,
						"<tr class=%s><td><b>%s</b></td><td><input type=text id=ck_%s name=ck_%s size=\"60\" value=\"%s\">"
						"&nbsp; <a href=\"%sck_unknown_cookie_var\" target=\"_blank\">Unknown</a></td></tr>\n",
						cls, name, name, name, val,
						hint_base );
			}
		}
	}
	ZnkMyf_destroy( send_myf );
}
static void
reportCookieView( ZnkStr cookie_recs, ZnkStrAry known_names, const char* moai_dir )
{
	const char* hint_base = "/moai2.0/customboy_hints_Cookie.html#";
	char       path[ 256 ];
	ZnkMyf     send_myf = ZnkMyf_create();
	ZnkVarpAry vars = NULL;
	CBConfigInfo* info = CBConfig_theInfo(); 

	Znk_snprintf( path, sizeof(path), "%s%s/%s", moai_dir, st_filters_dir, info->send_myf_filename_ );

	ZnkMyf_load( send_myf, path );

	ZnkStr_clear( cookie_recs );
	vars = ZnkMyf_find_vars( send_myf, "cookie_vars" );
	if( vars ){
		size_t idx;
		const size_t size = ZnkVarpAry_size( vars );
		const char* name = NULL;
		const char* val  = NULL;
		const char* cls  = "odd";
		ZnkVarp varp;
		for( idx=0; idx<size; ++idx ){
			varp = ZnkVarpAry_at( vars, idx );
			name = ZnkVar_name_cstr( varp );
			val  = ZnkVar_cstr( varp );
			cls  = ( idx % 2 ) ? "odd" : "evn";
			if( ZnkStrAry_find( known_names, 0, name, Znk_NPOS ) != Znk_NPOS ){
				ZnkStr_addf( cookie_recs,
						"<tr class=%s><td class=NoneWordBreak><b>%s</b></td><td>%s&nbsp;</td><td><a href=\"%sck_%s\" target=\"_blank\">Hint</a></td></tr>\n",
						cls, name, val, hint_base, name );
			} else {
				ZnkStr_addf( cookie_recs,
						"<tr class=%s><td class=NoneWordBreak><b>%s</b></td><td>%s&nbsp;</td><td><a href=\"%sck_unknown_cookie_var\" target=\"_blank\">Unknown</a></td></tr>\n",
						cls, name, val, hint_base );
			}
		}
	}
	ZnkMyf_destroy( send_myf );
}
void
CBVirtualizer_saveCookie_byCBVars( ZnkVarpAry cb_vars, const char* moai_dir )
{
	bool       erase_empty = false;
	char       path[ 256 ];
	ZnkMyf     send_myf = ZnkMyf_create();
	ZnkVarpAry ck_vars = NULL;
	CBConfigInfo* info = CBConfig_theInfo(); 

	Znk_snprintf( path, sizeof(path), "%s%s/%s", moai_dir, st_filters_dir, info->send_myf_filename_ );

	if( ZnkVarpAry_find_byName_literal( cb_vars, "cb_erase_empty", false ) ){
		erase_empty = true;
	}

	if( ZnkMyf_load( send_myf, path ) ){
		ck_vars = ZnkMyf_find_vars( send_myf, "cookie_vars" );
		if( ck_vars ){
			size_t idx;
			const size_t size = ZnkVarpAry_size( cb_vars );
			const char* name = NULL;
			ZnkVarp cb_varp;
			ZnkVarp ck_varp;
			ZnkVarpAry_clear( ck_vars );
			for( idx=0; idx<size; ++idx ){
				cb_varp = ZnkVarpAry_at( cb_vars, idx );
				name = ZnkVar_name_cstr(cb_varp);
				if( ZnkS_isBegin( name, "ck_" ) ){
					if( ZnkVar_str_leng(cb_varp) || !erase_empty ){
						ck_varp = ZnkVarp_create( "", "", ZnkHtpPostVar_e_None, ZnkPrim_e_Str, NULL );
						ZnkVar_set_name( ck_varp, name + 3, Znk_NPOS );
						ZnkHtpURL_unescape_toStr( ZnkVar_str(ck_varp), ZnkVar_cstr(cb_varp), ZnkVar_str_leng(cb_varp) );
						//ZnkVar_set_val_Str( ck_varp, ZnkVar_cstr(cb_varp), ZnkVar_str_leng(cb_varp) );
						ZnkVarpAry_push_bk( ck_vars, ck_varp );
					}
				}
			}
		}
		ZnkMyf_save( send_myf, path );
	}

	ZnkMyf_destroy( send_myf );
}



#if 0
static void
registSnippet( ZnkVarpAry cb_vars, ZnkVarpAry main_vars, CBUAInfo ua_info, uint64_t* ptua64 )
{
	ZnkStr tmp      = ZnkStr_new( "" );
	if( ua_info ){
		*ptua64 = generatePtuaResult( ua_info );
		ZnkStr_setf( tmp, "%" Znk_PFMD_64 "u", *ptua64 );
		CBVarsBase_registStr_toVars( cb_vars,   "ptua", ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );
		CBVarsBase_registStr_toVars( main_vars, "ptua", ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );
	}
	ZnkStr_delete( tmp );
}
#endif

static void
registMain( ZnkVarpAry cb_vars, ZnkVarpAry main_vars, ZnkStr ua_str, ZnkStr RE_key, CBFgpInfo fgp_info, ZnkStr scsz_str, CBUAInfo ua_info, uint64_t* ptua64 )
{
	bool with_UA = false;
	bool with_PluginStrings = false;
	bool with_ScreenResolution = false;
	uint32_t hs = 0;
	ZnkStr tmp      = ZnkStr_new( "" );

	CBVarsBase_registStr_toVars( cb_vars,   "UserAgent", ZnkStr_cstr(ua_str), ZnkStr_leng(ua_str) );
	CBVarsBase_registStr_toVars( main_vars, "UserAgent", ZnkStr_cstr(ua_str), ZnkStr_leng(ua_str) );

	with_UA = false;
	with_PluginStrings = false;
	with_ScreenResolution = false;
	hs = CBFgpInfo_calcHash( fgp_info, with_UA, with_PluginStrings, with_ScreenResolution, ZnkStr_cstr(RE_key) );
	ZnkStr_setf( tmp, "%u", hs );
	CBVarsBase_registStr_toVars( cb_vars,   "flrv", ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );
	CBVarsBase_registStr_toVars( main_vars, "flrv", ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );

	with_UA = false;
	with_PluginStrings = true;
	with_ScreenResolution = false;
	hs = CBFgpInfo_calcHash( fgp_info, with_UA, with_PluginStrings, with_ScreenResolution, ZnkStr_cstr(RE_key) );
	ZnkStr_setf( tmp, "%u", hs );
	CBVarsBase_registStr_toVars( cb_vars,   "flvv", ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );
	CBVarsBase_registStr_toVars( main_vars, "flvv", ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );

	if( ua_info ){
		*ptua64 = generatePtuaResult( ua_info );
		ZnkStr_setf( tmp, "%" Znk_PFMD_64 "u", *ptua64 );
		CBVarsBase_registStr_toVars( cb_vars,   "ptua", ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );
		CBVarsBase_registStr_toVars( main_vars, "ptua", ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );
	}

	CBVarsBase_registStr_toVars( cb_vars,   "scsz", ZnkStr_cstr(scsz_str), ZnkStr_leng(scsz_str) );
	CBVarsBase_registStr_toVars( main_vars, "scsz", ZnkStr_cstr(scsz_str), ZnkStr_leng(scsz_str) );

	ZnkStr_delete( tmp );
}

bool
CBVirtualizer_doMainProc( RanoCGIEVar* evar, ZnkVarpAry cb_vars, const char* cb_src, ZnkBird bird, bool is_step1, bool is_step2, ZnkStr RE_key,
		CBFgpInfo fgp_info, CBUAInfo ua_info, ZnkVarpAry main_vars, uint64_t* ptua64, ZnkStr msg, ZnkStr category, ZnkStr lacking_var,
		const char* moai_dir, bool all_cookie_clear )
{
	bool result = false;

	ZnkStr ua_str = ZnkStr_new( "" );
	ZnkStr caco   = ZnkStr_new( "" );

	CBVarsBase_registStr_toVars( main_vars, "UserAgent", "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "pthb", "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "pthc", "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "pthd", "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "ptua", "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "scsz", "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "pwd",  "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "flrv", "", 0 );
	CBVarsBase_registStr_toVars( main_vars, "flvv", "", 0 );

	if( is_step1 ){
		ZnkStr scsz_str = ZnkStr_new( "" );

		generateVirtualUSERS( evar, cb_vars, ua_info, ua_str, RE_key, fgp_info, scsz_str, msg, category );
		ZnkVarpAry_clear( cb_vars );
		registMain( cb_vars, main_vars, ua_str, RE_key, fgp_info, scsz_str, ua_info, ptua64 );
	
		ZnkStr_delete( scsz_str );
	} else {
		ZnkVarp fltp = ZnkVarpAry_find_byName( cb_vars, "UserAgent", Znk_NPOS, false );
		ZnkHtpURL_unescape_toStr( ua_str, ZnkVar_cstr(fltp), ZnkVar_str_leng(fltp) );
	}

	if( is_step2 ){
		if( !CBVirtualizer_initiateAndSave( evar, cb_vars, msg, cb_src, caco, ua_str, lacking_var, all_cookie_clear ) ){
			return false;
		}
		if( is_step1 ){
			ZnkStr_add( msg, "<b><font color=\"#773300\">OK. VirtualUSERS Initiation done successfully.</font></b>" );
		}
	}

	result = true;

	CBVarsBase_registStr_toVars( main_vars, "pthc", ZnkStr_cstr(caco), ZnkStr_leng(caco) );
	CBVarsBase_registStr_toVars( cb_vars,   "pthc", ZnkStr_cstr(caco), ZnkStr_leng(caco) );

	ZnkStr_delete( caco );
	ZnkStr_delete( ua_str );

	return result;
}
static bool
getRE_select( ZnkStr resel, const char* select_id )
{
	ZnkMyf myf = ZnkMyf_create();
	bool result = ZnkMyf_load( myf, "RE/RE.myf" );
	if( result ){
		const size_t num_of_sec = ZnkMyf_numOfSection( myf );
		size_t sec_idx;
		ZnkMyfSection sec;
		const char* sec_name;
		for( sec_idx=0; sec_idx<num_of_sec; ++sec_idx ){
			sec = ZnkMyf_atSection( myf, sec_idx );
			sec_name = ZnkMyfSection_name( sec );
			if( ZnkS_eq( sec_name, select_id ) ){
				ZnkStr_addf( resel, "<option value=\"%s\" selected>%s</option>\n", sec_name, sec_name );
			} else {
				ZnkStr_addf( resel, "<option value=\"%s\">%s</option>\n", sec_name, sec_name );
			}
		}
	}
	ZnkMyf_destroy( myf );
	return result;
}

bool
CBVirtualizer_load( ZnkVarpAry main_vars, const char* filename, const char* sec_name )
{
	ZnkMyf myf = ZnkMyf_create();
	bool result = false;

	result = ZnkMyf_load( myf, filename );
	if( result ){
		ZnkVarp varp = NULL;
		ZnkVarpAry vars;
		vars = ZnkMyf_find_vars( myf, sec_name );
		if( vars ){
			size_t idx;
			const size_t size = ZnkVarpAry_size( vars );
			ZnkVarpAry_clear( main_vars );
			for( idx=0; idx<size; ++idx ){
				varp = ZnkVarpAry_at( vars, idx );
				CBVarsBase_addNewStr_toVars( main_vars, ZnkVar_name_cstr(varp), ZnkVar_cstr(varp), ZnkVar_str_leng(varp) );
			}
		}
	}

	ZnkMyf_destroy( myf );
	return result;
}
bool
CBVirtualizer_save( ZnkVarpAry main_vars, const char* filename, const char* sec_name )
{
	ZnkMyf myf = ZnkMyf_create();
	bool result = false;

	result = ZnkMyf_load( myf, filename );
	if( result ){
		ZnkVarp varp = NULL;
		ZnkVarpAry vars;
		vars = ZnkMyf_find_vars( myf, sec_name );
		if( vars ){
			size_t idx;
			const size_t size = ZnkVarpAry_size( main_vars );
			ZnkVarpAry_clear( vars );
			for( idx=0; idx<size; ++idx ){
				varp = ZnkVarpAry_at( main_vars, idx );
				CBVarsBase_addNewStr_toVars( vars, ZnkVar_name_cstr(varp), ZnkVar_cstr(varp), ZnkVar_str_leng(varp) );
			}
		}
		result = ZnkMyf_save( myf, filename );
	}

	ZnkMyf_destroy( myf );
	return result;
}

void
CBVirtualizer_registBirdFgp( ZnkBird bird, CBFgpInfo fgp_info, const char* RE_key, bool is_update_main )
{
	ZnkBird_regist( bird, "Fgp_RE_key", RE_key );
	{
		ZnkStr resel = ZnkStr_new( "" );
		getRE_select( resel, RE_key );
		ZnkBird_regist( bird, "Fgp_RE_key_select", ZnkStr_cstr(resel) );
		ZnkStr_delete( resel );
	}

	ZnkBird_regist( bird, "Fgp_userAgent",        ZnkStr_cstr(fgp_info->Fgp_userAgent_) );
	ZnkBird_regist( bird, "Fgp_language",         ZnkStr_cstr(fgp_info->Fgp_language_) );
	ZnkBird_regist( bird, "Fgp_colorDepth",       ZnkStr_cstr(fgp_info->Fgp_colorDepth_) );
	ZnkBird_regist( bird, "Fgp_ScreenResolution", ZnkStr_cstr(fgp_info->Fgp_ScreenResolution_) );
	ZnkBird_regist( bird, "Fgp_TimezoneOffset",   ZnkStr_cstr(fgp_info->Fgp_TimezoneOffset_) );
	ZnkBird_regist( bird, "Fgp_SessionStorage",   ZnkStr_cstr(fgp_info->Fgp_SessionStorage_) );
	ZnkBird_regist( bird, "Fgp_LocalStorage",     ZnkStr_cstr(fgp_info->Fgp_LocalStorage_) );
	ZnkBird_regist( bird, "Fgp_IndexedDB",        ZnkStr_cstr(fgp_info->Fgp_IndexedDB_) );
	ZnkBird_regist( bird, "Fgp_BodyAddBehavior",  ZnkStr_cstr(fgp_info->Fgp_BodyAddBehavior_) );
	ZnkBird_regist( bird, "Fgp_OpenDatabase",     ZnkStr_cstr(fgp_info->Fgp_OpenDatabase_) );
	ZnkBird_regist( bird, "Fgp_CpuClass",         ZnkStr_cstr(fgp_info->Fgp_CpuClass_) );
	ZnkBird_regist( bird, "Fgp_Platform",         ZnkStr_cstr(fgp_info->Fgp_Platform_) );
	ZnkBird_regist( bird, "Fgp_doNotTrack",       ZnkStr_cstr(fgp_info->Fgp_doNotTrack_) );
	ZnkBird_regist( bird, "Fgp_PluginsString",    ZnkStr_cstr(fgp_info->Fgp_PluginsString_) );
	{
		ZnkStr fgp_str = ZnkStr_new( "" );
		CBFinger_Fgp_Canvas( fgp_str, RE_key );
		ZnkBird_regist( bird, "Fgp_Canvas", ZnkStr_cstr(fgp_str) );
		ZnkStr_delete( fgp_str );
	}
	{
		bool with_UA = false;
		bool with_PluginStrings = false;
		bool with_ScreenResolution = false;
		uint32_t hs = 0;
		ZnkStr tmp  = ZnkStr_new( "" );

		with_UA = false;
		with_PluginStrings = false;
		with_ScreenResolution = false;
		hs = CBFgpInfo_calcHash( fgp_info, with_UA, with_PluginStrings, with_ScreenResolution, RE_key );
		ZnkStr_setf( tmp, "%u", hs );
		ZnkBird_regist( bird, "Fgp_flrv", ZnkStr_cstr(tmp) );
		if( is_update_main ){
			ZnkBird_regist( bird, "flrv", ZnkStr_cstr(tmp) );
		}
	
		with_UA = false;
		with_PluginStrings = true;
		with_ScreenResolution = false;
		hs = CBFgpInfo_calcHash( fgp_info, with_UA, with_PluginStrings, with_ScreenResolution, RE_key );
		ZnkStr_setf( tmp, "%u", hs );
		ZnkBird_regist( bird, "Fgp_flvv", ZnkStr_cstr(tmp) );
		if( is_update_main ){
			ZnkBird_regist( bird, "flvv", ZnkStr_cstr(tmp) );
		}

		ZnkStr_delete( tmp );
	}
}

void
CBVirtualizer_registBirdCookie( ZnkBird bird, bool is_view, const char* moai_dir )
{
	ZnkStr cookie_recs = ZnkStr_new( "" );
	ZnkStrAry known_names = ZnkStrAry_create( true );
	ZnkStrAry_push_bk_cstr( known_names, "posttime", Znk_NPOS );
	ZnkStrAry_push_bk_cstr( known_names, "__cfduid", Znk_NPOS );
	ZnkStrAry_push_bk_cstr( known_names, "cxyl",     Znk_NPOS );
	ZnkStrAry_push_bk_cstr( known_names, "namec",    Znk_NPOS );
	ZnkStrAry_push_bk_cstr( known_names, "pwdc",     Znk_NPOS );
	ZnkStrAry_push_bk_cstr( known_names, "verifyc",  Znk_NPOS );
	if( is_view ){
		reportCookieView( cookie_recs, known_names, moai_dir );
	} else {
		reportCookieUI( cookie_recs, known_names, moai_dir );
	}
	ZnkBird_regist( bird, "CustomCookie_ui",  ZnkStr_cstr(cookie_recs) );
	ZnkStr_delete( cookie_recs );
	ZnkStrAry_destroy( known_names );
}

void
CBVirtualizer_registPtuaCtx( ZnkBird bird, uint64_t ptua64 )
{
	ZnkStr ptua_str = ZnkStr_new( "" );
	ZnkStr_setf( ptua_str, "%" Znk_PFMD_64 "u", ptua64 );
	ZnkBird_regist( bird, "ptua", ZnkStr_cstr(ptua_str) );
	ZnkStr_delete( ptua_str );
}


void
CBVirtualizer_loadProxyList( const char* moai_dir, ZnkStr parent_proxy, ZnkStrAry proxy_list )
{
	char filename[ 1024 ] = "";
	ZnkFile fp = NULL;

	Znk_snprintf( filename, sizeof(filename), "%sproxy_finder/parent_proxy.txt", moai_dir );
	fp = Znk_fopen( filename, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		ZnkStrAry_clear( proxy_list );
		ZnkStrAry_push_bk_cstr( proxy_list, "NONE", 4 );
		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				break;
			}
			ZnkStr_chompNL( line );
			if( ZnkStr_first( line ) == '#' ){
				continue; /* comment out */
			}
			ZnkStrEx_removeSideCSet( line, " \t", 2 );
			if( ZnkStr_empty( line ) ){
				continue; /* 空行をskip */
			}
			ZnkStrAry_push_bk_cstr( proxy_list, ZnkStr_cstr(line), ZnkStr_leng(line) );
		}
		Znk_fclose( fp );
		ZnkStr_delete( line );
	}

	{
		ZnkMyf config = ZnkMyf_create();
		Znk_snprintf( filename, sizeof(filename), "%sconfig.myf", moai_dir );
		if( ZnkMyf_load( config, filename ) ){
			ZnkVarpAry vars = ZnkMyf_find_vars( config, "config" );
			if( vars ){
				ZnkVarp var = ZnkVarpAry_find_byName_literal( vars, "parent_proxy", false );
				ZnkStr_set( parent_proxy, ZnkVar_cstr(var) );
			}
		}
		ZnkMyf_destroy( config );
	}
}
void
CBVirtualizer_addProxyList( const char* prxy, const char* moai_dir )
{
	char filename[ 1024 ] = "";
	ZnkFile fp = NULL;

	Znk_snprintf( filename, sizeof(filename), "%sproxy_finder/parent_proxy.txt", moai_dir );
	fp = Znk_fopen( filename, "ab" );
	if( fp ){
		Znk_fprintf( fp, "%s\n", prxy );
		Znk_fclose( fp );
	}
}
void
CBVirtualizer_setFiltersDir( const char* filters_dir )
{
	ZnkS_copy( st_filters_dir, sizeof(st_filters_dir), filters_dir, Znk_NPOS );
}
void
CBVirtualizer_setRERandomFactor( double RE_random_factor )
{
	st_RE_random_factor = RE_random_factor;
}

