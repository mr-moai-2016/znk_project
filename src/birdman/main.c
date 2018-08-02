#include <Bdm_base.h>

#include <Rano_htp_boy.h>
#include <Rano_module.h>
#include <Rano_htp_modifier.h>
#include <Rano_vtag_util.h>
#include <Rano_log.h>
#include <Rano_dir_util.h>
#include <Rano_cgi_util.h>

#include <Znk_base.h>
#include <Znk_s_base.h>
#include <Znk_stdc.h>
#include <Znk_str_ary.h>
#include <Znk_str_ex.h>
#include <Znk_str_path.h>
#include <Znk_str_fio.h>
#include <Znk_net_base.h>
#include <Znk_cookie.h>
#include <Znk_zip.h>
#include <Znk_process.h>
#include <Znk_thread.h>
#include <Znk_mbc_jp.h>
#include <Znk_bfr_bif.h>

#include <stdio.h>
#include <stdlib.h>

#define SJIS_NOU "\x94\x5c" /* 能 */

static bool
getVTagStr( ZnkStr str, const char* vtag_path )
{
	bool result = false;
	ZnkFile fp = Znk_fopen( vtag_path, "rb" );
	if( fp ){
		if( ZnkStrFIO_fgets( str, 0, 4096, fp ) ){
			ZnkStr_chompNL( str );
			result = true;
		}
		Znk_fclose( fp );
	}
	return result;
}
static bool
convertFullVer_toParentVer( ZnkStr parent_ver, const char* full_ver )
{
	bool      result = false;
	ZnkStrAry tkns = ZnkStrAry_create( true );
	size_t    tkns_size = 0;

	ZnkStrEx_addSplitC( tkns, full_ver, Znk_NPOS, '.', false, 3 );
	tkns_size = ZnkStrAry_size( tkns );
	if( tkns_size >= 2 ){
		ZnkStr_setf( parent_ver, "%s.%s", ZnkStrAry_at_cstr(tkns,0), ZnkStrAry_at_cstr(tkns,1) );
		result = true;
	} else if( tkns_size == 1 ){
		ZnkStr_setf( parent_ver, "%s", ZnkStrAry_at_cstr(tkns,0) );
		result = true;
	} else {
		result = false;
	}
	ZnkStrAry_destroy( tkns );
	return result;
}

static bool
isNewPatch( const char* pat_ver,  const char* cur_ver )
{
	bool result = false;
	int pat_ver_iary[ 3 ] = { 0 };
	int cur_ver_iary[ 3 ] = { 0 };
	ZnkStrAry pat_tkns = ZnkStrAry_create( true );
	ZnkStrAry cur_tkns = ZnkStrAry_create( true );
	size_t pat_tkns_size = 0;
	size_t cur_tkns_size = 0;
	size_t idx;

	ZnkStrEx_addSplitC( pat_tkns, pat_ver, Znk_NPOS, '.', false, 3 );
	ZnkStrEx_addSplitC( cur_tkns, cur_ver, Znk_NPOS, '.', false, 3 );
	pat_tkns_size = ZnkStrAry_size( pat_tkns );
	cur_tkns_size = ZnkStrAry_size( cur_tkns );

	for( idx=0; idx<pat_tkns_size; ++idx ){
		const char* tkn = ZnkStrAry_at_cstr( pat_tkns, idx );
		ZnkS_getIntD( pat_ver_iary + idx, tkn );
	}
	for( idx=0; idx<cur_tkns_size; ++idx ){
		const char* tkn = ZnkStrAry_at_cstr( cur_tkns, idx );
		ZnkS_getIntD( cur_ver_iary + idx, tkn );
	}
	for( idx=0; idx<Znk_NARY(cur_ver_iary); ++idx ){
		if( pat_ver_iary[ idx ] > cur_ver_iary[ idx ] ){
			/* patchの方が新しい */
			result = true;
			break;
		} else if( pat_ver_iary[ idx ] < cur_ver_iary[ idx ] ){
			/* patchの方が古い */
			break;
		}
		/* 数字が同じため、次を比較 */
	}
	ZnkStrAry_destroy( cur_tkns );
	ZnkStrAry_destroy( pat_tkns );

	return result;
}

static bool
queryVTagRule( const char* vtag_rule_path, const char* query_parent_ver, ZnkStr pat_ver )
{
	bool result = false;
	ZnkFile fp = Znk_fopen( vtag_rule_path, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		ZnkStrAry tkns = ZnkStrAry_create( true );
		size_t tkns_size = 0;
		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				break;
			}
			ZnkStr_chompNL( line );
			ZnkStrAry_clear( tkns );
			ZnkStrEx_addSplitC( tkns, ZnkStr_cstr(line), Znk_NPOS, ' ', false, 2 );
			tkns_size = ZnkStrAry_size( tkns );
			if( tkns_size == 2 ){
				const char* parent_ver = ZnkStrAry_at_cstr(tkns,0);
				if( ZnkS_eq( parent_ver, query_parent_ver ) ){
					/* found */
					ZnkStr_set( pat_ver, ZnkStrAry_at_cstr(tkns,1) );
					result = true;
					break;
				}
			}
		}
		Znk_fclose( fp );
		ZnkStrAry_destroy( tkns );
		ZnkStr_delete( line );
	}
	return result;
}
static bool
parseVTagRule( const char* vtag_rule_path, const char* cur_vtag_path, ZnkStr pat_ver, ZnkStr cur_ver, ZnkStr parent_ver )
{
	bool result = false;

	if( !getVTagStr( cur_ver, cur_vtag_path ) ){
		/* 2.0.0 とみなす */
		ZnkStr_set( cur_ver, "2.0.0" );
	}
	convertFullVer_toParentVer( parent_ver, ZnkStr_cstr(cur_ver) );
	result = queryVTagRule( vtag_rule_path, ZnkStr_cstr(parent_ver), pat_ver );
	return result;
}

static void
initHtpHdr( ZnkHtpHdrs htp_hdrs, const char* hostname, const char* ua, ZnkVarpAry cookie )
{
	ZnkVarp varp;
	ZnkSRef sref = { 0 };

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

	ZnkSRef_set_literal( &sref, "http://www.2chan.net" );
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

static char st_state_dat_path[ 256 ] = "state.dat";

static void
writeState( int count, const char* msg )
{
	ZnkFile fp = Znk_fopen( st_state_dat_path, "wb" );
	if( fp ){
		struct ZnkBif_tag bif_instance = { 0 };
		struct ZnkBif_tag tmp_instance = { 0 };
		ZnkErr_D( err );
		ZnkBif bif = &bif_instance;
		ZnkBif tmp = &tmp_instance;
		ZnkBfrBif_create( bif );
		ZnkBfrBif_create( tmp );
		ZnkBif_append( bif, (uint8_t*)msg, Znk_strlen(msg) );
		ZnkMbc_convert_self( bif, ZnkMbcType_SJIS, ZnkMbcType_UTF8, tmp, &err );
		{
			ZnkStr str = ZnkStr_new( "" ); /* bifはNULL終端していない */
			ZnkStr_assign( str, 0, (char*)ZnkBif_data(bif), ZnkBif_size(bif) );
			RanoCGIUtil_replaceNLtoHtmlBR( str );
			Znk_fprintf( fp, "%zu %s", count, ZnkStr_cstr(str) );
			ZnkStr_delete( str );
		}
		Znk_fclose( fp );
		ZnkBfrBif_destroy( bif );
		ZnkBfrBif_destroy( tmp );
	}
}

struct DownloadInfo {
	size_t bytes_;
	size_t pitch_;
	ZnkStr msg_;
};

static int
writeProgress( void* arg, const uint8_t* buf, size_t buf_size )
{
	struct DownloadInfo* dwn_info = Znk_force_ptr_cast( struct DownloadInfo*, arg );
	dwn_info->bytes_ += buf_size;
	dwn_info->pitch_ += buf_size;
	if( dwn_info->pitch_ > 1000000 ){
		ZnkStr prog_msg = ZnkStr_new( ZnkStr_cstr(dwn_info->msg_) );
		ZnkStr_addf( prog_msg, "Downloaded [%zu] byte...\n", dwn_info->bytes_ );
		writeState( 20, ZnkStr_cstr(prog_msg) );
		ZnkStr_delete( prog_msg );
		dwn_info->pitch_ = 0;
	} else {
		/* none */
		/* 報告処理は重いため、あまり頻繁にやると全体としての処理を遅くしてしまう */
	}
	return (int)buf_size;
}

static bool
download( const char* hostname, const char* unesc_req_urp, const char* target,
		const char* ua, ZnkVarpAry cookie,
		const char* parent_proxy,
		ZnkStr result_filename, ZnkStr ermsg, struct DownloadInfo* dwn_info, RanoModule mod, int* status_code, bool is_https )
{
	bool        result   = false;
	const char* tmpdir   = "tmp/";
	const char* cachebox = "tmp/cachebox/";
	struct ZnkHtpHdrs_tag htp_hdrs = { 0 };
	bool is_without_404 = true;
	ZnkHtpOnRecvFuncArg prog_fnca = { 0 };

	prog_fnca.func_ = writeProgress;
	prog_fnca.arg_  = dwn_info;

	ZnkHtpHdrs_compose( &htp_hdrs );
	initHtpHdr( &htp_hdrs, hostname, ua, cookie );

	if( mod ){
		ZnkMyf ftr_send = RanoModule_ftrSend( mod );
		const ZnkVarpAry ftr_vars = ZnkMyf_find_vars( ftr_send, "header_vars" );
		if( ftr_vars ){
			const ZnkVarp ua_var = ZnkVarpAry_find_byName( ftr_vars, "User-Agent", Znk_strlen_literal("User-Agent"), false );
			if( RanoHtpModifier_modifySendHdrs( htp_hdrs.vars_, ZnkVar_cstr(ua_var), ermsg ) ){
				if( ermsg ){
					ZnkStr_add( ermsg, "  RanoHtpModifier_modifySendHdrs : true\n" );
				}
			} else {
				if( ermsg ){
					ZnkStr_add( ermsg, "  RanoHtpModifier_modifySendHdrs : false\n" );
				}
			}
		}
	}

	result = RanoHtpBoy_cipher_get_with404( hostname, unesc_req_urp, target,
			&htp_hdrs,
			parent_proxy,
			tmpdir, cachebox, result_filename,
			is_without_404, status_code, is_https,
			&prog_fnca, ermsg );

	if( !result && ermsg ){
		ZnkStr_addf( ermsg,
				"  RanoHtpBoy_cipher_get_with404 : result=[%d] hostname=[%s] req_urp=[%s]\n"
				"                    : target=[%s] parent_proxy=[%s] tmpdir=[%s]\n"
				"                    : result_filename=[%s].\n",
				result, hostname, unesc_req_urp, target, parent_proxy, tmpdir, ZnkStr_cstr(result_filename) );
	}

	ZnkHtpHdrs_dispose( &htp_hdrs );
	return result;
}

#if 0
static const char*
getPlatformID_fromCompilerIntrinsic( void )
{
#if defined( __CYGWIN__ )
#  if Znk_CPU_BIT == 64
	return "cygwin64";
#  else
	return "cygwin32";
#  endif

#elif defined( __ANDROID__ )
	/* とりあえず */
	return "android-armeabi";

#elif defined( Znk_TARGET_WINDOWS )
#  if Znk_CPU_BIT == 64
	return "win64";
#  else
	return "win32";
#  endif

#elif defined( Znk_TARGET_UNIX )
#  if Znk_CPU_BIT == 64
	return "linux64";
#  else
	return "linux32";
#  endif

#else
	return NULL;
#endif
}
#endif


static bool
confirm( const char* inst_dir, const char* cur_ver, const char* pat_ver, const char* moai_authentic_key )
{
	ZnkStr msg = ZnkStr_new( "" );
	//ZnkStr_addf( msg, "Birdman : v%s(current_version) => v%s(after applying patch)<br>\n", cur_ver, pat_ver );
	ZnkStr_addf( msg, "現在の Moai は Ver %s です.\n", cur_ver );
	ZnkStr_addf( msg, "Moai を Ver %s へアップグレードすることができます.\n", pat_ver );
	ZnkStr_addf( msg, "\n" );
	ZnkStr_addf( msg, "まず最新版の修正パッチ(zipファイル)をダウンロードする必要があります.\n" );
	ZnkStr_addf( msg, "(birdmanフォルダの直下にダウンロードされます).\n" );
	ZnkStr_addf( msg, "「修正パッチのダウンロード」ボタンを押してください.\n" );
	ZnkStr_addf( msg, "\n" );
	ZnkStr_add(  msg, "<form action=\"/config?mode=upgrade\" method=\"POST\" enctype=\"multipart/form-data\">\n" );
	ZnkStr_addf( msg, "<input type=hidden name=Moai_AuthenticKey value=\"%s\">\n", moai_authentic_key );
	ZnkStr_add(  msg, "<input type=hidden name=Moai_UpgradeCmd   value=\"download\">\n" );
	ZnkStr_add(  msg, "<input class=MstyWideButton type=submit value='修正パッチのダウンロード'>\n" );
	ZnkStr_add(  msg, "</form>\n" );
	writeState( 100, ZnkStr_cstr(msg) );
	ZnkStr_delete( msg );
	return false;
}

const char*
getPatchPlatform()
{
#if defined( __CYGWIN__ )
	return "cygwin";

#elif defined( __ANDROID__ )
	return "android";

#elif defined( Znk_TARGET_WINDOWS )
	return "windows";

#else
	return "linux";
#endif
}

static bool
upgradeQuery( const char* cur_vtag_path, const char* main_app_dir, const char* authentic_key )
{
	bool result = false;
	const bool  is_https = true;
	const char* hostname = "mr-moai-2016.github.io";
	const char* vtag_rule_urp = "/patch/vtag_rule";
	const char* parent_proxy  = NULL;
	ZnkStr result_filename = ZnkStr_new("");
	ZnkStr ermsg           = ZnkStr_new( "" );
	int    status_code     = 0;
	ZnkStr pat_ver      = ZnkStr_new( "" );
	ZnkStr cur_ver      = ZnkStr_new( "" );
	ZnkStr parent_ver   = ZnkStr_new( "" );
	ZnkStr patch_basename = ZnkStr_new( "" );
	const char* vtag_rule_path = NULL;
	ZnkStr msg = ZnkStr_new( "" );
	struct DownloadInfo dwn_info = { 0 };

	dwn_info.bytes_ = 0;
	dwn_info.pitch_ = 0;
	dwn_info.msg_   = msg;

	ZnkStr_addf( msg, "Moaiを最新バージョンへアップグレード可" SJIS_NOU "かどうかを確認します.\n" );

	ZnkStr_addf( msg, "Birdman : Downloading [%s] ...\n", vtag_rule_urp );
	ZnkStr_addf( msg, "Birdman : Please wait...\n" );
	writeState( 50, ZnkStr_cstr(msg) );

	ZnkStr_addf( msg, "Birdman : cur_vtag_path=[%s].\n", cur_vtag_path );
	result = download( hostname, vtag_rule_urp, "znk_project",
			"Firesexy", NULL,
			parent_proxy,
			result_filename, ermsg, &dwn_info, NULL, &status_code, is_https );
	if( !result ){
		ZnkStr_addf( msg, "Birdman : download is failure.\n" );
		ZnkStr_addf( msg, "[%s].\n", ZnkStr_cstr(ermsg) );
		writeState( 100, ZnkStr_cstr(msg) );
		goto FUNC_END;
	}

	vtag_rule_path = ZnkStr_cstr(result_filename);
	ZnkStr_addf( msg, "Birdman : vtag_rule_path=[%s].\n", vtag_rule_path );
	result = parseVTagRule( vtag_rule_path, cur_vtag_path, pat_ver, cur_ver, parent_ver );
	if( !result ){
		ZnkStr_addf( msg, "Birdman : parseVTagRule is false.\n" );
		ZnkStr_addf( msg, "\n" );
		ZnkStr_addf( msg, "現在ご利用の Moai は古すぎるため、Moaiアップグレードを行うことができません.\n" );
		ZnkStr_addf( msg, "お手数ですが、最新バージョンの新規インストールをお願いします.\n" );
		writeState( 100, ZnkStr_cstr(msg) );
		goto FUNC_END;
	}

	result  = isNewPatch( ZnkStr_cstr(pat_ver), ZnkStr_cstr(cur_ver) );
	if( !result ){
		ZnkStr_addf( msg, "Birdman : v%s(current_version) => v%s(after applying patch)\n", ZnkStr_cstr(cur_ver), ZnkStr_cstr(pat_ver) );
		ZnkStr_addf( msg, "\n" );
		ZnkStr_addf( msg, "現在ご利用の Moai は最新バージョンのようです.\n" );
		ZnkStr_addf( msg, "従ってMoaiアップグレードを行う必要は今はありません.\n" );
		writeState( 100, ZnkStr_cstr(msg) );
		goto FUNC_END;
	}

	confirm( main_app_dir, ZnkStr_cstr(cur_ver), ZnkStr_cstr(pat_ver), authentic_key );

	ZnkDir_mkdirPath( "tmp", Znk_NPOS, '/', NULL );
	{
		ZnkFile fp = Znk_fopen( "tmp/patch_basename.dat", "wb" );
		const char* patch_platform = getPatchPlatform();
		ZnkStr_setf( patch_basename, "moai-v%s-patch-v%s-%s", ZnkStr_cstr(parent_ver), ZnkStr_cstr(pat_ver), patch_platform );
		Znk_fputs( ZnkStr_cstr(patch_basename), fp );
		Znk_fclose( fp );
	}

FUNC_END:
	ZnkStr_delete( msg );
	ZnkStr_delete( patch_basename );
	ZnkStr_delete( pat_ver );
	ZnkStr_delete( cur_ver );
	ZnkStr_delete( parent_ver );
	ZnkStr_delete( result_filename );
	ZnkStr_delete( ermsg );
	return true;
}

static bool
progress_func( const char* label, size_t num_of_entry, size_t idx_of_entry, void* param )
{
	ZnkStr msg = Znk_force_ptr_cast( ZnkStr, param );
	size_t count = idx_of_entry * 100 / num_of_entry;
	ZnkStr msg2  = ZnkStr_new( ZnkStr_cstr(msg) );
	ZnkStr_addf( msg2, "Birdman : Extract Progress [%zu]% : %s\n", count, label );
	writeState( (int)count, ZnkStr_cstr(msg2) );
	return true;
}

static bool
downloadZip( const char* authentic_key )
{
	bool result = false;
	const bool  is_https = true;
	const char* hostname = "mr-moai-2016.github.io";
	const char* parent_proxy  = NULL;
	ZnkStr result_filename = ZnkStr_new("");
	ZnkStr ermsg           = ZnkStr_new( "" );
	int    status_code     = 0;
	ZnkStr zip_urp         = ZnkStr_new( "" );
	ZnkStr patch_basename  = ZnkStr_new( "" );
	ZnkStr msg             = ZnkStr_new( "" );
	ZnkFile fp = Znk_fopen( "tmp/patch_basename.dat", "rb" );
	struct DownloadInfo dwn_info = { 0 };

	dwn_info.bytes_ = 0;
	dwn_info.pitch_ = 0;
	dwn_info.msg_   = msg;

	if( fp == NULL ){
		writeState( 100, "Cannot open patch_basename.dat" );
		goto FUNC_END;
	}

	if( ZnkStrFIO_fgets( patch_basename, 0, 4096, fp ) ){
		ZnkStr_chompNL( patch_basename );
	}


	/* cur_ver が parent_ver に属する場合はこのパッチを適用可能である.
	 * またそれを pat_ver へアップデートすることを意味する. */
	ZnkStr_setf( zip_urp, "/patch/%s.zip", ZnkStr_cstr(patch_basename) );

	ZnkStr_addf( msg, "修正パッチ %s%s のダウンロードを開始します.\n", hostname, ZnkStr_cstr(zip_urp) );

	writeState( 10, ZnkStr_cstr(msg) );
	result = download( hostname, ZnkStr_cstr(zip_urp), "znk_project",
			"Firesexy", NULL,
			parent_proxy,
			result_filename, NULL, &dwn_info, NULL, &status_code, is_https );

	if( status_code == 404 ){
		ZnkStr_addf( msg, "Birdman : download [%s%s%s] 404 not found.\n", 
				is_https ? "https://" : "http://", hostname, ZnkStr_cstr(zip_urp) );
	} else if( result ){
		ZnkStr_addf( msg, "Birdman : download status_code=[%d]\n", status_code );
		ZnkStr_addf( msg, "修正パッチのダウンロードは完了しました.\n" );
		ZnkStr_addf( msg, "\n" );

		ZnkStr_addf( msg, "修正パッチを解凍します.\n" );
		ZnkStr_addf( msg, "Birdman : Now extract...\n" );
		writeState( 50, ZnkStr_cstr(msg) );

		ZnkDir_mkdirPath( "tmp", Znk_NPOS, '/', NULL );
		if( !ZnkZip_extract( ZnkStr_cstr(result_filename), ermsg, progress_func, msg, "tmp" ) ){
			ZnkStr_addf( msg, "Birdman : %s\n", ZnkStr_cstr(ermsg) );
			ZnkStr_addf( msg, "修正パッチの解凍に失敗しました.\n" );
			ZnkStr_addf( msg, "この修正パッチは壊れているため適用できません.\n" );
		} else {
			RanoLog_printf( "result_filename=[%s]\n", ZnkStr_cstr(result_filename) );
			ZnkStr_addf( msg, "Birdman : Extract done.\n" );
			ZnkStr_addf( msg, "修正パッチの解凍は完了しました.\n" );

			ZnkStr_addf( msg, "\n" );
			ZnkStr_addf( msg, "最後にこの修正パッチを実際に適用します.\n" );
			ZnkStr_addf( msg, "(このときMoai本体の上書きも行われるため、古いMoaiのプロセスが一旦終了し、新しいMoaiが起動します).\n" );
			ZnkStr_addf( msg, "「修正パッチを適用」を押してください.\n" );
			ZnkStr_addf( msg, "\n" );

			ZnkStr_add(  msg, "<form action=\"/config?mode=upgrade\" method=\"POST\" enctype=\"multipart/form-data\">\n" );
			ZnkStr_addf( msg, "<input type=hidden name=Moai_AuthenticKey value=\"%s\">\n", authentic_key );
			ZnkStr_add(  msg, "<input type=hidden name=Moai_UpgradeCmd   value=\"apply\">\n" );
			ZnkStr_add(  msg, "<input class=MstyWideButton type=submit value=\"修正パッチを適用\">\n" );
			ZnkStr_add(  msg, "</form>\n" );
		}
	} else {
		ZnkStr_addf( msg, "Birdman : download [%s%s] : Error : result is false. status_code=[%d].\n",
				is_https ? "https://" : "http://", hostname, ZnkStr_cstr(zip_urp),
				status_code );
	}
	writeState( 100, ZnkStr_cstr(msg) );

FUNC_END:
	ZnkStr_delete( patch_basename );
	ZnkStr_delete( zip_urp );
	ZnkStr_delete( result_filename );
	ZnkStr_delete( ermsg );
	ZnkStr_delete( msg );
	return true;
}

static const char*
getOneArg( ZnkStr arg, const char* p )
{
	const char* q;
	p = Znk_strstr( p, "['" );
	if( p ){
		p += 2;
		q = Znk_strstr( p, "']" );
		if( q ){
			ZnkStr_assign( arg, 0, p, q-p );
			return q + 2;
		}
	}
	return NULL;
}


static bool
applyCore( const ZnkStrAry list, const char* patch_dir, const char* dst_dir_prefix, const char* platform_id, ZnkStr ermsg, bool is_write_state, bool skip_self )
{
	const size_t size = ZnkStrAry_size( list );
	size_t idx;
	ZnkStr src_file = ZnkStr_new( "" );
	ZnkStr dst_file = ZnkStr_new( "" );
	ZnkStr cmd      = ZnkStr_new( "" );
	ZnkStr arg1     = ZnkStr_new( "" );
	bool result = true;

	RanoLog_printf( "Birdman : applyCore : %s : size=[%zu]\n", platform_id, size );

	ZnkStr_addf( ermsg, "Birdman : %s : src_dir_prefix=[%s-%s/].\n",
			platform_id, patch_dir, platform_id );
	ZnkStr_addf( ermsg, "Birdman : %s : dst_dir_prefix=[%s].\n",
			platform_id, dst_dir_prefix );

	for( idx=0; idx<size; ++idx ){
		const char* line = ZnkStrAry_at_cstr( list, idx );
		const char* under_path = NULL;
		size_t pos = Znk_NPOS;
		size_t cmd_leng = 0;

		pos = ZnkS_lfind_arg( line, 0, Znk_strlen(line),
				0, &cmd_leng,
				" \t", 2 );
		if( pos != Znk_NPOS ){
			const char* p = line + pos;
			ZnkStr_assign( cmd, 0, p, cmd_leng );
			p += cmd_leng;

			if( ZnkStr_eq( cmd, "copy" ) ){
				p = getOneArg( arg1, p );
				if( p ){
					under_path = ZnkStr_cstr( arg1 );
					ZnkStr_setf( src_file, "%s-%s/%s", patch_dir, platform_id, under_path );
					ZnkStr_setf( dst_file, "%s%s", dst_dir_prefix, under_path );

					ZnkStr_addf( ermsg, "Birdman : %s : copy [%s] ", platform_id, under_path );

					if( skip_self &&
						( ZnkS_eq( under_path, "birdman/birdman" ) || ZnkS_eq( under_path, "birdman/birdman.exe" ) ) )
					{
						ZnkStr_addf( ermsg, " skip.\n" );
					} else {
						RanoDirUtil_mkdirOfTailContain( ZnkStr_cstr(dst_file), ermsg );
						if( ZnkDir_copyFile_byForce( ZnkStr_cstr(src_file), ZnkStr_cstr(dst_file), ermsg ) ){
							ZnkStr_addf( ermsg, " success.\n" );
						} else {
							ZnkStr_addf( ermsg, " failure.\n" );
							result = false;
						}
					}
				}
			} else if( ZnkStr_eq( cmd, "delete" ) ){
				p = getOneArg( arg1, p );
				if( p ){
					under_path = ZnkStr_cstr( arg1 );
					ZnkStr_setf( dst_file, "%s%s", dst_dir_prefix, under_path );
					if( ZnkDir_deleteFile_byForce( ZnkStr_cstr(dst_file) ) ){
						ZnkStr_addf( ermsg, "Birdman : %s : delete file=[%s] success.\n", platform_id, ZnkStr_cstr(dst_file) );
					} else {
						ZnkStr_addf( ermsg, "Birdman : %s : delete file=[%s] failure.\n", platform_id, ZnkStr_cstr(dst_file) );
						//result = false;
					}
				}
			} else if( ZnkStr_eq( cmd, "mkdir" ) ){
				p = getOneArg( arg1, p );
				if( p ){
					under_path = ZnkStr_cstr( arg1 );
					ZnkStr_setf( dst_file, "%s%s", dst_dir_prefix, under_path );
					if( ZnkDir_mkdirPath( ZnkStr_cstr(dst_file), Znk_NPOS, '/', NULL ) ){
						ZnkStr_addf( ermsg, "Birdman : %s : mkdir file=[%s] success.\n", platform_id, ZnkStr_cstr(dst_file) );
					} else {
						ZnkStr_addf( ermsg, "Birdman : %s : mkdir file=[%s] failure.\n", platform_id, ZnkStr_cstr(dst_file) );
						result = false;
					}
				}
			} else if( ZnkStr_eq( cmd, "rmdir" ) ){
				p = getOneArg( arg1, p );
				if( p ){
					under_path = ZnkStr_cstr( arg1 );
					ZnkStr_setf( dst_file, "%s%s", dst_dir_prefix, under_path );
					if( ZnkDir_rmdir( ZnkStr_cstr(dst_file) ) ){
						ZnkStr_addf( ermsg, "Birdman : %s : rmdir file=[%s] success.\n", platform_id, ZnkStr_cstr(dst_file) );
					} else {
						ZnkStr_addf( ermsg, "Birdman : %s : rmdir file=[%s] failure.\n", platform_id, ZnkStr_cstr(dst_file) );
						//result = false;
					}
				}
			}
			if( is_write_state ){
				writeState( idx, ZnkStr_cstr(ermsg) );
			}
		}
	}

	ZnkStr_delete( src_file );
	ZnkStr_delete( dst_file );
	ZnkStr_delete( cmd );
	ZnkStr_delete( arg1 );
	return result;
}

static bool
applyFiles( ZnkStr ermsg, const char* platform_id, const char* app_name, const char* patch_dir, const char* dst_dir_prefix )
{
	bool skip_self = true;
	ZnkMyf myf = ZnkMyf_create();
	ZnkStrAry  common   = NULL;
	ZnkStrAry  platform = NULL;
	ZnkStr platform_str = ZnkStr_new( "" );
	bool   result = false;
	ZnkStr patch_basename = ZnkStr_new( "" );
	ZnkStr apply_list_filename = ZnkStr_new( "" );
	ZnkFile fp = Znk_fopen( "tmp/patch_basename.dat", "rb" );

	if( fp == NULL ){
		writeState( 100, "Cannot open patch_basename.dat" );
		goto FUNC_END;
	}
	if( ZnkStrFIO_fgets( patch_basename, 0, 4096, fp ) ){
		ZnkStr_chompNL( patch_basename );
	}

	ZnkStr_setf( apply_list_filename, "tmp/%s/apply_list.myf", ZnkStr_cstr(patch_basename) );

	ZnkStr_addf( ermsg, "Birdman : apply_list_filename=[%s]\n", ZnkStr_cstr(apply_list_filename) );
	writeState( 1, ZnkStr_cstr(ermsg) );

	if( !ZnkMyf_load( myf, ZnkStr_cstr(apply_list_filename) ) ){
		RanoLog_printf( "Birdman : loading apply_list.myf is failure.\n" );
		goto FUNC_END;
	}

	ZnkStr_addf( ermsg, "Birdman : apply_list_filename=[%s]\n", ZnkStr_cstr(apply_list_filename) );
	writeState( 1, ZnkStr_cstr(ermsg) );

	common = ZnkMyf_find_lines( myf, "common" );
	result = applyCore( common, patch_dir, dst_dir_prefix, "common", ermsg, true, skip_self );

	if( platform_id == NULL ){
		if( RanoVTagUtil_getPlatformID( platform_str, app_name, "birdman" ) ){
			platform_id = ZnkStr_cstr( platform_str );
		}
	}
	if( platform_id ){
		platform = ZnkMyf_find_lines( myf, platform_id );
		if( !applyCore( platform, patch_dir, dst_dir_prefix, platform_id, ermsg, true, skip_self ) ){
			result = false;
		}
	}

	if( result ){
		ZnkStr_addf( ermsg, "Birdman : OK. All upgrade is successfully done.\n" );
	} else {
		ZnkStr_addf( ermsg, "Birdman : upgrade is failure.\n" );
	}
	writeState( 100, ZnkStr_cstr(ermsg) );

FUNC_END:
	ZnkStr_delete( apply_list_filename );
	ZnkStr_delete( patch_basename );
	ZnkStr_delete( platform_str );
	ZnkMyf_destroy( myf );
	return result;
}

static bool
installFiles( ZnkStr ermsg, const char* platform_id,
		const char* app_name, const char* patch_dir, const char* dst_dir,
		const char* old_parent_ver, const char* new_parent_ver )
{
	bool skip_self = false;
	ZnkMyf myf = ZnkMyf_create();
	ZnkStrAry  common   = NULL;
	ZnkStrAry  platform = NULL;
	ZnkStr platform_str = ZnkStr_new( "" );
	bool   result = false;
	ZnkStr apply_list_filename = ZnkStr_new( "apply_list.myf" );

	if( !ZnkMyf_load( myf, ZnkStr_cstr(apply_list_filename) ) ){
		RanoLog_printf( "Birdman : loading apply_list.myf is failure.\n" );
		goto FUNC_END;
	}

	ZnkStr_addf( ermsg, "Birdman : apply_list_filename=[%s]\n", ZnkStr_cstr(apply_list_filename) );

	common = ZnkMyf_find_lines( myf, "common" );
	result = applyCore( common, patch_dir, dst_dir, "common", ermsg, false, skip_self );

	if( platform_id == NULL ){
		if( RanoVTagUtil_getPlatformID( platform_str, app_name, "birdman" ) ){
			platform_id = ZnkStr_cstr( platform_str );
		}
	}
	if( platform_id ){
		platform = ZnkMyf_find_lines( myf, platform_id );
		if( !applyCore( platform, patch_dir, dst_dir, platform_id, ermsg, false, skip_self ) ){
			result = false;
		}
	}

	if( result ){
		if( new_parent_ver ){
			ZnkStr moai_dir_old = ZnkStr_new( dst_dir );
			ZnkStr moai_dir_new = ZnkStr_new( dst_dir );
			size_t tail_pos = 0;
			char old_ptn[ 256 ] = "";
			char new_ptn[ 256 ] = "";

			ZnkStrPath_cutLastDSP( moai_dir_old );
			ZnkStrPath_cutLastDSP( moai_dir_new );

			Znk_snprintf( old_ptn, sizeof(old_ptn), "%s-v%s-%s", app_name, old_parent_ver, platform_id );
			Znk_snprintf( new_ptn, sizeof(new_ptn), "%s-v%s-%s", app_name, new_parent_ver, platform_id );
			ZnkStrEx_replace_BF( moai_dir_new, tail_pos,
					old_ptn, Znk_strlen(old_ptn),
					new_ptn, Znk_strlen(new_ptn),
					1, Znk_NPOS );
			if( ZnkDir_getType( ZnkStr_cstr(moai_dir_new) ) != ZnkDirType_e_Directory ){
				if( ZnkDir_rename( ZnkStr_cstr( moai_dir_old ), ZnkStr_cstr( moai_dir_new ), ermsg ) ){
					ZnkStr_addf( ermsg, "Birdman : Rename directory [%s] => [%s] success.\n", ZnkStr_cstr(moai_dir_old), ZnkStr_cstr(moai_dir_new) );
				} else {
					ZnkStr_addf( ermsg, "Birdman : Rename directory [%s] => [%s] failure.\n", ZnkStr_cstr(moai_dir_old), ZnkStr_cstr(moai_dir_new) );
				}
			} else {
				ZnkStr_addf( ermsg, "Birdman : new directory [%s] is already exist. renaming is skip.\n", ZnkStr_cstr(moai_dir_new) );
			}
			ZnkStr_delete( moai_dir_old );
			ZnkStr_delete( moai_dir_new );
		}

		ZnkStr_addf( ermsg, "Birdman : OK. All upgrade is successfully done.\n" );
	} else {
		ZnkStr_addf( ermsg, "Birdman : upgrade is failure.\n" );
	}

FUNC_END:
	ZnkStr_delete( apply_list_filename );
	ZnkStr_delete( platform_str );
	ZnkMyf_destroy( myf );
	return result;
}

static void
runMainAppFile( const char* main_app_file, const char* main_app_arg, const char* main_app_dir )
{
	ZnkStr curdir_save = ZnkStr_new( "" );

	ZnkDir_getCurrentDir( curdir_save );
	ZnkDir_changeCurrentDir( main_app_dir );
	if( main_app_arg ){
		const char* argv[ 2 ];
		argv[ 0 ] = main_app_file;
		argv[ 1 ] = main_app_arg;
		RanoLog_printf( "Birdman : upgrade_apply : ZnkProcess_execRestart begin main_app_file=[%s] main_app_arg=[%s].\n", main_app_file, main_app_arg );
		ZnkProcess_execRestart( 2, argv, ZnkProcessConsole_e_CreateNew );
		RanoLog_printf( "Birdman : upgrade_apply : ZnkProcess_execRestart end.\n" );
	} else {
		RanoLog_printf( "Birdman : upgrade_apply : runMainAppFile begin. main_app_file=[%s]\n", main_app_file );
		ZnkProcess_execRestart( 1, &main_app_file, ZnkProcessConsole_e_CreateNew );
		RanoLog_printf( "Birdman : upgrade_apply : ZnkProcess_execRestart end.\n" );
	}
	//ZnkDir_changeCurrentDir( ZnkStr_cstr(curdir_save) );
	ZnkStr_delete( curdir_save );
}

static ZnkStr
findMoaiDir( const char* moai_dir_landmark )
{
	const char dsp = '/';
	size_t depth = 5;
	ZnkStr moai_dir = ZnkStr_new( "../" );
	if( !ZnkStrPath_searchParentDir( moai_dir, depth, moai_dir_landmark, ZnkDirType_e_Directory, dsp ) ){
		RanoLog_printf( "Birdman : Searching %s ... Skip( Not found ).\n", moai_dir_landmark );
		ZnkStr_delete( moai_dir );
		return NULL;
	}
	ZnkStr_add( moai_dir, moai_dir_landmark );
	ZnkStr_add_c( moai_dir, '/' );

	/***
	 * target.myfが存在するかどうかをverifyする.
	 */
	{
		ZnkStr verify_path = ZnkStr_newf( "%starget.myf", ZnkStr_cstr(moai_dir) );
		if( ZnkDir_getType( ZnkStr_cstr(verify_path) ) != ZnkDirType_e_File ){
			/* missing target.myf  */
			RanoLog_printf( "Birdman : Searching %s ... Detected. But this directory is broken. missing target.myf.\n", moai_dir_landmark );
			ZnkStr_delete( moai_dir );
			moai_dir = NULL;
		} else {
			/* OK */
			RanoLog_printf( "Birdman : Searching %s ... Detected. Now install to it.\n", moai_dir_landmark );
		}
		ZnkStr_delete( verify_path );
	}
	return moai_dir;
}

static void
printUsage( void )
{
	Znk_printf_e( "Usage : birdman [yesno|is_new_patch|upgrade_query|upgrade_download|upgrade_apply|install] options...\n" );
	Znk_printf_e( "  birdman yesno [y|n]\n" );
	Znk_printf_e( "  birdman is_new_patch pat_vtag_path cur_vtag_path\n" );
	Znk_printf_e( "  birdman upgrade_query (cur_vtag_path)\n" );
	Znk_printf_e( "  birdman upgrade_download\n" );
	Znk_printf_e( "  birdman upgrade_apply (main_app_file platform_id)\n" );
	Znk_printf_e( "  birdman install platform_id old_parent_ver (new_parent_ver)\n" );
}

int main( int argc, char** argv )
{
	const char* tls_module = "../libtls-17";
	const char* cert_pem   = "../cert.pem";
	bool   result  = false;
	ZnkMyf myf      = ZnkMyf_create();
	ZnkStr moai_dir = BdmBase_getMoaiDir();

	if( !ZnkMyf_load( myf, "birdman.myf" ) ){
		RanoLog_printf( "Birdman : loading birdman.myf is failure.\n" );
		return EXIT_FAILURE;
	}

	if( argc >= 2 ){
		char authentic_key[ 256 ] = "";
		const char* opt = argv[ 1 ];

		if( moai_dir ){
			BdmBase_loadAuthenticKey( authentic_key, sizeof(authentic_key), ZnkStr_cstr(moai_dir) );
			Znk_snprintf( st_state_dat_path, sizeof(st_state_dat_path), "%sdoc_root/state.dat", ZnkStr_cstr(moai_dir) );
		}

		if( ZnkS_eq( opt, "yesno" ) ){
			int ch = 0;
			if( argc >= 3 ){
				const char* prompt = argv[ 2 ];
				Znk_fputs( prompt, Znk_stdout() );
			}
			ch = getchar();
			result = (bool)( ch == 'y' );

		} else if( ZnkS_eq( opt, "is_new_patch" ) ){
			if( argc >= 4 ){
				ZnkStr pat_ver = ZnkStr_new( "" );
				ZnkStr cur_ver = ZnkStr_new( "" );

				/***
				 * vtagファイルが導入されたのはv2.0.2から.
				 * (v2.0およびv2.0.1にはvtagは存在しない)
				 */
				if( getVTagStr( pat_ver, argv[ 2 ] ) ){
					if( !getVTagStr( cur_ver, argv[ 3 ] ) ){
						/* 0.0.0 とみなす */
						ZnkStr_set( cur_ver, "0.0.0" );
					}
					result  = isNewPatch( ZnkStr_cstr(pat_ver), ZnkStr_cstr(cur_ver) );
				}

				ZnkStr_delete( pat_ver );
				ZnkStr_delete( cur_ver );
				/***
				 * result が trueのとき EXIT_SUCCESS を返し、適用対象を示すものとする.
				 */
			}

		} else if( ZnkS_eq( opt, "upgrade_query" ) ){
			if( moai_dir ){
				ZnkStr cur_vtag_path = ZnkStr_new( "" );
				ZnkStr msg = ZnkStr_new( "" );

				ZnkVarpAry config = ZnkMyf_find_vars( myf, "config" );
				if( config ){
					ZnkVarp var = NULL;
					var = ZnkVarpAry_findObj_byName_literal( config, "tls_module", false );
					if( var ){
						tls_module = ZnkVar_cstr( var );
					}
					var = ZnkVarpAry_findObj_byName_literal( config, "cert_pem", false );
					if( var ){
						cert_pem = ZnkVar_cstr( var );
					}
				}

				if( argc >= 3 ){
					ZnkStr_set( cur_vtag_path, argv[ 2 ] );
				}

				if( !ZnkNetBase_initiate( false ) ){
					RanoLog_printf( "Birdman : ZnkNetBase_initiate is failure.\n" );
					goto FUNC_END;
				}
				if( !RanoHtpBoy_initiateHttpsModule( tls_module, cert_pem ) ){
					RanoLog_printf( "Birdman : RanoHtpBoy_initiateHttpsModule is failure.\n" );
					goto FUNC_END;
				}

				if( ZnkStr_empty( cur_vtag_path ) ){
					/* 自動取得 */
					ZnkStr_setf( cur_vtag_path, "%svtag", ZnkStr_cstr( moai_dir ) );
				}

				RanoLog_printf( "Birdman : cur_vtag_path=[%s]\n", ZnkStr_cstr(cur_vtag_path) );

				/* clean tmp dir */
				{
					const char* tmpdir   = "tmp/";
					RanoDirUtil_removeDir( tmpdir,
							"birdman tmpdir", msg,
							NULL, NULL );
				}

				result = upgradeQuery( ZnkStr_cstr(cur_vtag_path), ZnkStr_cstr(moai_dir), authentic_key );
				ZnkStr_delete( cur_vtag_path );
				ZnkStr_delete( msg );
			} else {
				RanoLog_printf( "Birdman : Searching parent moai_dir ... Skip( target.myf does not found ).\n" );
			}

		} else if( ZnkS_eq( opt, "upgrade_download" ) ){

			ZnkVarpAry config = ZnkMyf_find_vars( myf, "config" );
			if( config ){
				ZnkVarp var = NULL;
				var = ZnkVarpAry_findObj_byName_literal( config, "tls_module", false );
				if( var ){
					tls_module = ZnkVar_cstr( var );
				}
				var = ZnkVarpAry_findObj_byName_literal( config, "cert_pem", false );
				if( var ){
					cert_pem = ZnkVar_cstr( var );
				}
			}

			if( !ZnkNetBase_initiate( false ) ){
				RanoLog_printf( "Birdman : ZnkNetBase_initiate is failure.\n" );
				goto FUNC_END;
			}
			if( !RanoHtpBoy_initiateHttpsModule( tls_module, cert_pem ) ){
				RanoLog_printf( "Birdman : RanoHtpBoy_initiateHttpsModule is failure.\n" );
				goto FUNC_END;
			}

			result = downloadZip( authentic_key );

		} else if( moai_dir && ZnkS_eq( opt, "upgrade_apply" ) ){
			if( moai_dir ){
				/* moai本体は終了していることを前提とする. */
				ZnkStr ermsg = ZnkStr_new( "" );
				ZnkStr main_app_file = ZnkStr_new( "" );
				ZnkStr patch_dir = ZnkStr_new( "" );
				const char* platform_id = NULL;
				const char* dst_dir_prefix = "../";
				const char* app_name = "moai";
				const char* main_app_arg = NULL;

				if( argc >= 3 ){
					ZnkStr_set( main_app_file, argv[ 2 ] );
				}
				if( argc >= 4 ){
					platform_id = argv[ 3 ];
				}
				RanoVTagUtil_getPatchDir( patch_dir, app_name, false, "birdman", "tmp/" );

				applyFiles( ermsg, platform_id, app_name, ZnkStr_cstr(patch_dir), dst_dir_prefix );

				/* moai本体を起動 */
				if( ZnkStr_empty( main_app_file ) ){
					/* 自動取得 */
#if defined(__ANDROID__)
					ZnkStr_setf( main_app_file, "/system/bin/sh" );
					main_app_arg = "moai_for_android.sh";
#elif defined(Znk_TARGET_UNIX)
					ZnkStr_setf( main_app_file, "/bin/sh" );
					main_app_arg = "moai_for_linux.sh";
#else
					ZnkStr_setf( main_app_file, "moai" );
#endif
				}

				ZnkStr_delete( patch_dir );
				result = true;

				RanoLog_printf( "Birdman : upgrade_apply : runMainAppFile begin.\n" );

				/***
				 * Moaiがまだ再起動しておらず、state.dat が読み込めない状況であっても、
				 * javascript側で 1000msec 後に再度読み込みを行う.
				 * したがって、この時点で100%とマークしておいても問題はない.
				 */
				writeState( 100, ZnkStr_cstr(ermsg) );
				ZnkStr_delete( ermsg );
				ZnkMyf_destroy( myf );

				runMainAppFile( ZnkStr_cstr(main_app_file), main_app_arg, ZnkStr_cstr(moai_dir) );
				/***
				 * これ以降は実行されない.
				 * main_app_file 等のZnkStr(ヒープ文字列)の解放処理を明示的には行っていないが、
				 * processが終了するため自動的に行われるはずであり、無視してよかろう.
				 */
			} else {
				RanoLog_printf( "Birdman : Searching parent moai_dir ... Skip( target.myf does not found ).\n" );
			}
		} else if( ZnkS_eq( opt, "install" ) ){
			/* moai本体は終了していることを前提とする. */
			ZnkStr ermsg = ZnkStr_new( "" );
			const char* patch_dir = "moai";
			const char* platform_id = NULL;
			const char* old_parent_ver = NULL;
			const char* new_parent_ver = NULL;
			ZnkStr      dst_dir  = NULL;
			const char* app_name = "moai";

			if( argc < 4 ){
				printUsage();
			}
			platform_id    = argv[ 2 ];
			old_parent_ver = argv[ 3 ];
			if( argc >= 5 ){
				new_parent_ver = argv[ 4 ];
			}

			if( old_parent_ver && platform_id ){
				ZnkStr moai_dir_landmark = ZnkStr_new( "" );
				ZnkStr_setf( moai_dir_landmark, "%s-v%s-%s", app_name, old_parent_ver, platform_id );
				dst_dir = findMoaiDir( ZnkStr_cstr(moai_dir_landmark) );
				ZnkStr_delete( moai_dir_landmark );
			}
			if( dst_dir ){
				ZnkDir_mkdirPath( ZnkStr_cstr(dst_dir), Znk_NPOS, '/', NULL );
				installFiles( ermsg, platform_id, app_name, patch_dir, ZnkStr_cstr(dst_dir),
						old_parent_ver, new_parent_ver );
				RanoLog_printf( "%s", ZnkStr_cstr(ermsg) );
			}
			ZnkStr_delete( ermsg );
			ZnkStr_delete( dst_dir );
		} else {
			printUsage();
		}
	} else {
		printUsage();
	}

FUNC_END:
	ZnkStr_delete( moai_dir );
	ZnkMyf_destroy( myf );
	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
