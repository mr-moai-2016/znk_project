#include <Est_get.h>
#include <Est_config.h>
#include <Est_link.h>
#include <Est_base.h>
#include <Est_filter.h>
#include <Est_img_viewer.h>

#include <Rano_module_ary.h>
#include <Rano_module.h>
#include <Rano_htp_boy.h>
#include <Rano_log.h>

#include <Znk_htp_util.h>
#include <Znk_htp_hdrs.h>
#include <Znk_date.h>
#include <Znk_dir.h>
#include <Znk_str_fio.h>
#include <Znk_str_ptn.h>
#include <Znk_str_path.h>
#include <Znk_str_ex.h>
#include <Znk_cookie.h>

#include <string.h>

#define IS_OK( val ) (bool)( (val) != NULL )

static bool
isUseHeadTarget( const char* target )
{
	return !ZnkS_eq(target, "5ch");
}

static void
initHtpHdr_forHead( ZnkHtpHdrs htp_hdrs, const char* hostname, const char* req_urp, const char* ua, ZnkVarpAry cookie, RanoCGIEVar* evar )
{
	ZnkVarp varp;
	ZnkSRef sref = { 0 };
	Znk_UNUSED(varp);

	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Host",   Znk_strlen_literal("Host"),
			hostname, Znk_strlen(hostname), true );

	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"User-Agent", Znk_strlen_literal("User-Agent"),
			ua, strlen(ua), true );

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

	ZnkSRef_set_literal( &sref, "http://www.google.com" );
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Referer", Znk_strlen_literal("Referer"),
			sref.cstr_, sref.leng_, true );
	if( varp ){
		ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
		ZnkStr_set( val, "http://" );
		ZnkStr_add( val, hostname );
		ZnkStr_add( val, req_urp );
	}

	{
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

}

static bool
do_head2( RanoCGIEVar* evar, const char* hostname, const char* unesc_req_urp, const char* target,
		const char* ua, ZnkVarpAry cookie,
		const char* parent_proxy, ZnkStr msg, RanoModule mod )
{
	bool        result = false;
	const char* tmpdir = EstConfig_getTmpDirPID( true );
	struct ZnkHtpHdrs_tag htp_hdrs = { 0 };

	ZnkHtpHdrs_compose( &htp_hdrs );
	//initHtpHdr( &htp_hdrs, hostname, ua, cookie );
	initHtpHdr_forHead( &htp_hdrs, hostname, unesc_req_urp, ua, cookie, evar );

	if( mod ){
		bool is_all_replace = true;
		RanoModule_filtHtpHeader(  mod, htp_hdrs.vars_ );
		RanoModule_filtCookieVars( mod, htp_hdrs.vars_, is_all_replace, NULL );
	}

	{
		ZnkStr result_filename = ZnkStr_new( "" );

		result = RanoHtpBoy_do_head( hostname, unesc_req_urp, target,
				&htp_hdrs,
				parent_proxy,
				tmpdir, result_filename );
		if( !result && msg ){
			ZnkStr_addf( msg,
					"  RanoHtpBoy_do_head : result=[%d] hostname=[%s] req_urp=[%s]<br>"
					"                     : target=[%s] parent_proxy=[%s] tmpdir=[%s]<br>"
					"                     : result_filename=[%s].<br>",
					result, hostname, unesc_req_urp, target, parent_proxy, tmpdir, ZnkStr_cstr(result_filename) );
		}
		ZnkStr_delete( result_filename );

	}
	ZnkHtpHdrs_dispose( &htp_hdrs );
	return result;
}


static const char*
getContentType( const char* filename )
{
	const char* content_type = "";
	char ext[ 32 ] = "";
	const char* p = strchr( filename, '?' );
	char* r = NULL;
	if( p ){
		const char* q = Znk_memrchr( (uint8_t*)filename, '.', p-filename );
		if( q ){
			/***
			 * ?より前に拡張子が存在する場合はそれを採用.
			 * 例. http://hostname/easter.cgi?...
			 */
			ZnkS_copy( ext, sizeof(ext), q+1, p-q-1 );
		} else {
			/***
			 * ?より前に拡張子が存在しない場合は最後尾にそれがあるならばそれを採用.
			 * 例. http://hostname/easter?target/data.jpg
			 */
			ZnkS_copy( ext, sizeof(ext), ZnkS_get_extension( filename, '.' ), Znk_NPOS );
		}
	} else {
		ZnkS_copy( ext, sizeof(ext), ZnkS_get_extension( filename, '.' ), Znk_NPOS );
	}

	/***
	 * 同ページへの参照リンク指定子 # が含まれている場合、
	 * 例1. http://hostname/easter.cgi#PageEnd?...
	 * 例2. http://hostname/easter?target/data.html#PageEnd
	 * この時点では extは # が含まれた形で取得されているはずであり、
	 * 実際の拡張子はその # の位置で終端すべきである.
	 */
	r = strchr( ext, '#' );
	if( r ){
		*r = '\0';
	}
	/***
	 * / が含まれている場合、
	 * 例1. http://mevius.5ch.net/test/read.cgi/internet/...
	 * PATH_INFO部を含むURLである可能性がある.
	 * 実際の拡張子はその / の位置で終端すべきである.
	 */
	r = strchr( ext, '/' );
	if( r ){
		*r = '\0';
	}

	if( ZnkS_eqCase( ext, "jpg" ) ){
		content_type = "image/jpeg";
	} else if( ZnkS_eqCase( ext, "png" ) ){
		content_type = "image/png";
	} else if( ZnkS_eqCase( ext, "gif" ) ){
		content_type = "image/gif";
	} else if( ZnkS_eqCase( ext, "webm" ) ){
		content_type = "video/webm";
	} else if( ZnkS_eqCase( ext, "htm" ) ){
		content_type = "text/html";
	} else if( ZnkS_eqCase( ext, "html" ) ){
		content_type = "text/html";
	} else if( ZnkS_eqCase( ext, "php" ) ){
		content_type = "text/html";
	} else if( ZnkS_eqCase( ext, "cgi" ) ){
		content_type = "text/html";
	} else if( ZnkS_eqCase( ext, "css" ) ){
		content_type = "text/css";
	} else if( ZnkS_eqCase( ext, "js" ) ){
		content_type = "text/javascript";
	} else if( ZnkS_eqCase( ext, "txt" ) ){
		content_type = "text/plain";
	} else {
		content_type = "application/octet-stream";

	}
	return content_type;
}


static bool
isCachableContent( const char* content_type )
{
	RanoTextType txt_type = RanoCGIUtil_getTextType( content_type );
	switch( txt_type ){
	case RanoText_CSS:
	case RanoText_JS:
	case RanoText_Image:
	case RanoText_Video:
		return true;
	default:
		break;
	}
	return false;
}

static void
parseEstCmd_byPostVars( ZnkVarpAry post_vars, char* content_type, size_t content_type_size,
		const char* unesc_req_urp,
		bool* save_img_cache, bool* static_cached,
		const char* hostname, ZnkStr result_filename, const char* target,
		ZnkStr console_msg )
{
	static const char* cachebox = "./cachebox/";
	ZnkVarp varp = NULL;

	/***
	 * est_content_typeが明示的に指定されている場合は優先的にそれに従う.
	 */
	varp = ZnkVarpAry_find_byName_literal( post_vars, "est_content_type", false );
	if( varp ){
		ZnkS_copy( content_type, content_type_size,
				ZnkVar_cstr(varp), Znk_NPOS );
	} else {
		/***
		 * 厳密にはResponseヘッダーのContent-typeを調べるべきであるが
		 * とりあえず拡張子で判断する.
		 */
		ZnkS_copy( content_type, content_type_size,
				getContentType( unesc_req_urp ), Znk_NPOS );
	}
	varp = ZnkVarpAry_find_byName_literal( post_vars, "est_save_img_cache", false );
	if( varp ){
		*save_img_cache = ZnkS_eq( ZnkVar_cstr(varp), "on" );
		ZnkStr_addf( console_msg, "save_img_cache=[%d]\n", *save_img_cache );
	}

	/***
	 * キャッシュ可能なcontent_typeである場合はキャッシュに存在するかどうかを調べ
	 * 存在する場合はTCP/IPコネクションを発生させず単にそれを返すようにする.
	 */
	if( isCachableContent( content_type ) ){
		RanoHtpBoy_getResultFile( hostname, unesc_req_urp, result_filename, cachebox, target );
		/* ファイルの存在性をチェック */
		if( ZnkDir_getType( ZnkStr_cstr(result_filename) ) == ZnkDirType_e_File ){
			*static_cached = true;
		}
	}
}

static bool
displayByView( const char* result_filename, const char* content_type )
{
	bool result_of_display = false;
	RanoTextType txt_type = RanoCGIUtil_getTextType( content_type );
	switch( txt_type ){
	case RanoText_Image:
	case RanoText_Video:
	{
		ZnkStr ans = ZnkStr_new( "" );
		ZnkStr result_filename_mod = ZnkStr_new( result_filename );
		RanoCGIMsg_initiate( false, NULL );

		ZnkStrPath_replaceDSP( result_filename_mod, '/' );
		if( ZnkStr_isBegin( result_filename_mod, "./" ) ){
			ZnkStr_cut_front( result_filename_mod, 2 );
		}

		EstImgViewer_makeNowLoading( ans, txt_type, ZnkStr_cstr(result_filename_mod) );
		Znk_fputs( ZnkStr_cstr( ans ), Znk_stdout() );

		RanoCGIMsg_finalize();
		ZnkStr_delete( result_filename_mod );
		ZnkStr_delete( ans );
		result_of_display = true;
		break;
	}
	default:
		result_of_display = RanoCGIUtil_displayBinary( result_filename, content_type );
		break;
	}
	return result_of_display;
}

void
EstGet_procHead( RanoCGIEVar* evar, ZnkVarpAry post_vars, const char* est_val )
{
	ZnkVarp varp;
	char hostname[ 1024 ] = "";
	ZnkStr req_urp = ZnkStr_new( "" );
	char content_type[ 1024 ] = "application/octet-stream";
	const char* target = "";
	RanoModule  mod    = NULL;
	const char* src    = EstBase_getHostnameAndRequrp_fromEstVal( hostname, sizeof(hostname), req_urp, est_val );

	Znk_UNUSED( src );

	target = EstConfig_getTargetAndModule( &mod, hostname );

	if( target && !ZnkS_empty(hostname) ){
		static const char* cachebox = "./cachebox/";
		const char* tmpdir_pid = EstConfig_getTmpDirPID( true );
		const char* tmpdir_com = EstConfig_getTmpDirCommon( true );

		ZnkStr      result_filename = ZnkStr_newf( "./%sresult.dat", tmpdir_pid );
		const char* ua              = "Firefox";
		const char* parent_proxy    = EstConfig_parent_proxy();
		bool        static_cached      = false;

		/***
		 * est_content_typeが明示的に指定されている場合は優先的にそれに従う.
		 */
		varp = ZnkVarpAry_find_byName_literal( post_vars, "est_content_type", false );
		if( varp ){
			ZnkS_copy( content_type, sizeof(content_type),
					ZnkVar_cstr(varp), Znk_NPOS );
		} else {
			/***
			 * 厳密にはResponseヘッダーのContent-typeを調べるべきであるが
			 * とりあえず拡張子で判断する.
			 */
			ZnkS_copy( content_type, sizeof(content_type),
					getContentType( ZnkStr_cstr(req_urp) ), Znk_NPOS );
		}

		/***
		 * キャッシュ可能なcontent_typeである場合はキャッシュに存在するかどうかを調べ
		 * 存在する場合はTCP/IPコネクションを発生させず単にそれを返すようにする.
		 */
		if( isCachableContent( content_type ) ){
			const char* unesc_req_urp = ZnkStr_cstr(req_urp);
			RanoHtpBoy_getResultFile( hostname, unesc_req_urp, result_filename, cachebox, target );
			/* ファイルの存在性をチェック */
			if( ZnkDir_getType( ZnkStr_cstr(result_filename) ) == ZnkDirType_e_File ){
				static_cached = true;
			}
		}
		if( !static_cached ){
			ZnkVarpAry cookie = ZnkVarpAry_create( true );
			char cookie_filename[ 256 ] = "";
			Znk_snprintf( cookie_filename, sizeof(cookie_filename), "%s%s/cookie.txt", tmpdir_com, target );
			ZnkCookie_load( cookie, cookie_filename );
			/***
			 * req_urp に .. などを含む場合、適切に展開する必要がある.
			 * (通常ブラウザ側でそれは行われてからRequestされる)
			 */
			do_head2( evar, hostname, ZnkStr_cstr(req_urp), target,
					ua, cookie,
					parent_proxy, NULL, mod );
			EstConfig_readRecvHdrSetCookie( mod, target );
			RanoHtpBoy_readRecvHdrVar( "Content-Type", content_type, sizeof(content_type), target, tmpdir_pid );
			ZnkVarpAry_destroy( cookie );
		}

	} else {
		RanoCGIMsg_initiate( true, NULL );
		RanoCGIMsg_begin();
		Znk_printf( "est_src is not specified or it's hostname is not contained in target.myf.\n" );
		RanoCGIMsg_end();
		RanoCGIMsg_finalize();
	}
	ZnkStr_delete( req_urp );
}

static bool
saveLastModifiedData( const ZnkVarpAry lmd )
{
	ZnkFile fp = NULL;
	const char* tmpdir_com = EstConfig_getTmpDirCommon( false );
	char dat_path[ 256 ] = "";

	ZnkDir_mkdirPath( tmpdir_com, Znk_NPOS, '/', NULL );
	Znk_snprintf( dat_path, sizeof(dat_path), "./%s/last_modified.dat", tmpdir_com );
	fp = Znk_fopen( dat_path, "wb" );
	if( fp ){
		const size_t size = ZnkVarpAry_size( lmd );
		size_t idx;
		const char* key = NULL;
		const char* val = NULL;
		ZnkVarp varp;
		for( idx=0; idx<size; ++idx ){
			varp = ZnkVarpAry_at( lmd, idx );
			key = ZnkVar_name_cstr( varp );
			val = ZnkVar_cstr( varp );
			if( !ZnkS_empty( key ) && !ZnkS_empty( val ) ){
				Znk_fprintf( fp, "%s %s\n", key, val );
			}
		}
		Znk_fclose( fp );
		return true;
	}
	return false;
}

static bool
loadLastModifiedData( ZnkVarpAry lmd )
{
	ZnkFile fp = NULL;
	const char* tmpdir_com = EstConfig_getTmpDirCommon( false );
	char dat_path[ 256 ] = "";
	char file_path[ 256 ] = "";

	Znk_snprintf( dat_path, sizeof(dat_path), "./%s/last_modified.dat", tmpdir_com );
	fp = Znk_fopen( dat_path, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		char* p = NULL;
		ZnkVarpAry_clear( lmd );
		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				/* eof */
				break;
			}
			ZnkStr_chompNL( line );
			p = strchr( ZnkStr_cstr(line), ' ' );
			if( p ){
				const char* key = ZnkStr_cstr(line);
				const char* val = p + 1;
				ZnkS_copy( file_path, sizeof(file_path), key, p-key );
				if( !ZnkS_empty( val ) && ZnkDir_getType( file_path ) == ZnkDirType_e_File ){
					/* 存在するファイルのみ */
					ZnkVarp varp = ZnkVarpAry_find_byName( lmd, file_path, p-key, false );
					if( varp == NULL ){
						/* 新規追加 */
						varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
						ZnkStr_assign( varp->name_, 0, key, p - key );
						ZnkVarpAry_push_bk( lmd, varp );
					}
					ZnkVar_set_val_Str( varp, p+1, Znk_NPOS );
				}
			}
		}
		ZnkStr_delete( line );
		Znk_fclose( fp );
		return true;
	} else {
		if( saveLastModifiedData( lmd ) ){
			return true;
		}
	}
	return false;
}

typedef struct {
	bool   is_404_;
	bool   updated_;
	ZnkStr console_msg_ref_;
} DisplayTextInfo;

static int
swapConsoleMsg( ZnkStr str, void* arg )
{
	ZnkStr console_msg_ref = Znk_force_ptr_cast( ZnkStr, arg );
	ZnkStr_swap( str, console_msg_ref );
	return 1;
}

static int
modifyTextForce_onReload( ZnkStr text, void* arg )
{
	DisplayTextInfo* display_text_info = Znk_force_ptr_cast( DisplayTextInfo*, arg );
	ZnkSRef old_ptn = { 0 };
	ZnkSRef new_ptn = { 0 };

	ZnkStrPtn_invokeInQuote( text,
			"<pre class=msg_console><b>Easter BBS Console:</b>\n", "</pre> ",
			NULL, NULL,
			swapConsoleMsg, display_text_info->console_msg_ref_, false );

	if( display_text_info->is_404_ ){
		ZnkSRef_set_literal( &old_ptn, "</body></html>" );
		ZnkSRef_set_literal( &new_ptn,
				"<script type=\"text/javascript\"><!--\n"
				"document.write( window.document.lastModified );\n"
				"contd=document.getElementById(\"contdisp\");\n"
				"contd.innerHTML = \"404 Not found.\";\n"
				"--></script>\n"
				"</body></html>" );
		ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	} else if( display_text_info->updated_ ){
		ZnkSRef_set_literal( &old_ptn, "</body></html>" );
		ZnkSRef_set_literal( &new_ptn,
				"<script type=\"text/javascript\"><!--\n"
				"document.write( window.document.lastModified );\n"
				"contd=document.getElementById(\"contdisp\");\n"
				"contd.innerHTML = \"更新されました\";\n"
				"--></script>\n"
				"</body></html>" );
		ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	} else {
		ZnkSRef_set_literal( &old_ptn, "</body></html>" );
		ZnkSRef_set_literal( &new_ptn,
				"<script type=\"text/javascript\"><!--\n"
				"document.write( window.document.lastModified );\n"
				"contd=document.getElementById(\"contdisp\");\n"
				"contd.innerHTML = \"新着なし\";\n"
				"--></script>\n"
				"</body></html>" );
		ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	return 0;
}

static int
downloadWithCookie( const char* tmpdir_com, const char* tmpdir_pid,
		const char* target, const char* evar_http_cookie,
		const char* hostname, const char* unesc_req_urp,
		RanoModule mod, const char* ua, const char* parent_proxy,
		ZnkStr result_filename, ZnkStr ermsg,
		char* content_type,      size_t content_type_size,
		char* new_last_modified, size_t new_last_modified_size )
{
	int status_code = -1; /* 広義解釈して、通信エラーの場合は -1 と定める */
	ZnkVarpAry cookie = ZnkVarpAry_create( true );
	char cookie_filename[ 256 ] = "";
	Znk_snprintf( cookie_filename, sizeof(cookie_filename), "%s%s/cookie.txt", tmpdir_com, target );
	ZnkCookie_load( cookie, cookie_filename );
	if( evar_http_cookie ){
		ZnkCookie_regist_byCookieStatement( cookie, evar_http_cookie, Znk_strlen(evar_http_cookie) );
	}
	/***
	 * req_urp に .. などを含む場合、適切に展開する必要がある.
	 * (通常ブラウザ側でそれは行われてからRequestされる)
	 */
	if( !EstBase_download( hostname, unesc_req_urp, target,
			ua, cookie, evar_http_cookie,
			parent_proxy,
			result_filename, ermsg, mod, &status_code ) ){
		RanoLog_printf( "EstBase_download ermsg=[%s]\n", ZnkStr_cstr(ermsg) );
		status_code = -1;
	}

	/* Set-Cookieの検出とsend_filterへの更新. */
	EstConfig_readRecvHdrSetCookie( mod, target );

	/* 厳密なcontent_typeの取得 */
	RanoHtpBoy_readRecvHdrVar( "Content-Type", content_type, content_type_size, target, tmpdir_pid );

	/* 最終更新時刻情報の最新版を取得 */
	if( new_last_modified ){
		RanoHtpBoy_readRecvHdrVar( "Last-Modified", new_last_modified, new_last_modified_size, target, tmpdir_pid );
	}

	ZnkVarpAry_destroy( cookie );
	return status_code;
}
static bool
updateCookieAndLastModified_byHead( const char* tmpdir_com, const char* tmpdir_pid,
		const char* target, RanoCGIEVar* evar,
		const char* hostname, const char* unesc_req_urp,
		RanoModule mod, const char* ua, const char* parent_proxy,
		ZnkStr result_filename,
		char* content_type,      size_t content_type_size,
		char* new_last_modified, size_t new_last_modified_size )
{
	static const char* cachebox = "./cachebox/";

	bool is_404 = false;
	ZnkVarpAry cookie = ZnkVarpAry_create( true );
	char cookie_filename[ 256 ] = "";
	ZnkStr status = ZnkStr_new( "" );

	Znk_snprintf( cookie_filename, sizeof(cookie_filename), "%s%s/cookie.txt", tmpdir_com, target );
	ZnkCookie_load( cookie, cookie_filename );
	/***
	 * req_urp に .. などを含む場合、適切に展開する必要がある.
	 * (通常ブラウザ側でそれは行われてからRequestされる)
	 */
	do_head2( evar, hostname, unesc_req_urp, target,
			ua, cookie,
			parent_proxy, NULL, mod );

	/* Set-Cookieの検出とsend_filterへの更新. */
	EstConfig_readRecvHdrSetCookie( mod, target );

	/* 厳密なcontent_typeの取得 */
	RanoHtpBoy_readRecvHdrVar( "Content-Type", content_type, content_type_size, target, tmpdir_pid );

	/* 最終更新時刻情報の最新版を取得 */
	if( new_last_modified ){
		RanoHtpBoy_readRecvHdrVar( "Last-Modified", new_last_modified, new_last_modified_size, target, tmpdir_pid );
	}

	RanoHtpBoy_getResultFile( hostname, unesc_req_urp, result_filename, cachebox, target );

#if 0
	{
		char result_dir[ 256 ] = "";
		Znk_snprintf( result_dir, sizeof(result_dir), "./%s%s", tmpdir_pid, target );
		ZnkDir_mkdirPath( result_dir, Znk_NPOS, '/', NULL );
	}
#endif
	RanoHtpBoy_readRecvHdrStatus( status, 1, target, tmpdir_pid );
	//readRecvHdrStatus( status, 1, target );
	if( ZnkStr_eq( status, "404" ) ){
		is_404 = true;
	}
	ZnkVarpAry_destroy( cookie );
	ZnkStr_delete( status );
	return is_404;
}

static void
filterJS( const char* result_file, const char* src, const char* target, RanoTextType txt_type )
{
#if 0
	ZnkFile fp = Znk_fopen( result_file, "rb" );
	if( fp ){
		ZnkStr text = ZnkStr_new( "" );
		ZnkStr unesc_src = ZnkStr_new( "" );
		struct EstLinkInfo link_info = { 0 };

		preloadTextAndInfos( text, unesc_src, src, &link_info, fp, false );

		if( ZnkS_eq( target, "5ch" ) ){
			/* for smart phone version : asset/js/android/application.js */
			ZnkSRef old_ptn = { 0 };
			ZnkSRef new_ptn = { 0 };

			/* Easter URL変換 : location_origin */
			ZnkSRef_set_literal( &old_ptn, "location_origin:location.protocol+'//'+ location.hostname" );
			ZnkSRef_set_literal( &new_ptn, "location_origin:'/easter?est_view=//itest.5ch.net'" );
			ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 

			/* Easter URL変換 : & => %26  ? => %3F */
			ZnkSRef_set_literal( &old_ptn, "\"&\":\"?\"" );
			ZnkSRef_set_literal( &new_ptn, "\"%26\":\"%3F\"" );
			ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 

			/* Easter URL変換 : & => %26  ? => %3F */
			ZnkSRef_set_literal( &old_ptn, "'/public/newapi/client.php?subdomain='+subdomain+'&board='+_5ch.request.board+'&dat='+_5ch.request.dat_number+'&rand='+_5ch.text.rand();" );
			ZnkSRef_set_literal( &new_ptn, "'/public/newapi/client.php%3Fsubdomain='+subdomain+'%26board='+_5ch.request.board+'%26dat='+_5ch.request.dat_number+'%26rand='+_5ch.text.rand();" );
			ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
		}


		/* final process */
		{
			RanoModule mod = EstConfig_findModule( target );
			if( mod ){
				RanoModule_invokeOnResponse( mod, NULL, text, ZnkStr_cstr(unesc_src) );
				RanoModule_filtTxt( mod, text, txt_type );
			}
		}

		ZnkStr_delete( unesc_src );
		ZnkStr_delete( link_info.base_href_ );
		ZnkStr_delete( link_info.hostname_ );
		Znk_fclose( fp );

		fp = Znk_fopen( result_file, "wb" );
		if( fp ){
			Znk_fputs( ZnkStr_cstr(text), fp );
			Znk_fclose( fp );
		}

		ZnkStr_delete( text );
	}
#endif
}

static void
filter( RanoTextType txt_type, ZnkStr console_msg,
		const char* cmd_name, const char* hostname, const char* unesc_req_urp,
		const char* evar_http_cookie,
		const char* result_filename, const char* src, const char* target, bool save_img_cache  )
{
	switch( txt_type ){
	case RanoText_HTML:
	case RanoText_CSS:
		ZnkStr_addf( console_msg, "%s : URL=[%s%s]\n", cmd_name, hostname, unesc_req_urp );
		EstBase_addConsoleMsg_HttpCookie( console_msg, evar_http_cookie );
		EstFilter_main( result_filename, src, target, txt_type, console_msg, save_img_cache );
		break;
	case RanoText_JS:
		filterJS( result_filename, src, target, txt_type );
		break;
	default:
		break;
	}
}

static void
displayResult( bool is_404, bool updated, ZnkStr console_msg,
		const char* content_type, const char* result_filename, bool is_view )
{
	RanoTextType txt_type = RanoCGIUtil_getTextType( content_type );
	bool result_of_display = false;
	DisplayTextInfo display_text_info = { 0 };

	display_text_info.is_404_  = is_404;
	display_text_info.updated_ = updated;
	display_text_info.console_msg_ref_ = console_msg;
	switch( txt_type ){
	case RanoText_HTML:
	case RanoText_CSS:
		result_of_display = RanoCGIUtil_displayText( result_filename, content_type,
				modifyTextForce_onReload, &display_text_info );
		break;
	default:
		if( is_view ){
			result_of_display = displayByView( result_filename, content_type );
		} else {
			result_of_display = RanoCGIUtil_displayBinary( result_filename, content_type );
		}
		break;
	}

	if( !result_of_display ){
		RanoCGIMsg_initiate( false, NULL );
		Znk_printf( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
		Znk_printf( "<html>\n" );
		Znk_printf( "<head>\n" );
		Znk_printf( "<link href=\"/cgis/easter/publicbox/style.css\" rel=\"stylesheet\" type=\"text/css\" />\n" );
		Znk_printf( "</head>\n" );
		Znk_printf( "<body bgcolor=\"#EEFFEE\" text=\"#808000\" link=\"#0000EE\" vlink=\"#0000EE\" alink=\"#FF0000\">" );
		Znk_printf( "<span class=MstyAutoLinkOther>" );
		if( is_404 ){
			Znk_printf( "404 Not found." );
		} else if( updated ){
			Znk_printf( "Updated." );
		} else {
			Znk_printf( "Not new." );
		}
		Znk_printf( "</span>" );
		RanoCGIMsg_finalize();
	}
}

static bool
updateLastModifiedData( const char* result_filename, const char* new_last_modified )
{
	bool updated = false;
	if( ZnkS_empty(new_last_modified) ){
		/* この値が空なら常に更新すると見なす */
		updated = true;
	} else {
		ZnkVarpAry lmd = ZnkVarpAry_create( true );
		if( loadLastModifiedData( lmd ) ){
			ZnkVarp varp = ZnkVarpAry_find_byName( lmd, result_filename, Znk_NPOS, false );
			if( varp == NULL ){
				/* 新規追加 */
				varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
				ZnkStr_assign( varp->name_, 0, result_filename, Znk_NPOS );
				ZnkVarpAry_push_bk( lmd, varp );
				ZnkVar_set_val_Str( varp, new_last_modified, Znk_NPOS );
				updated = true;
			} else {
				if( ZnkS_eq( ZnkVar_cstr( varp ), new_last_modified ) ){
					/* 更新なし */
					updated = false;
				} else{
					/* 更新あり */
					ZnkVar_set_val_Str( varp, new_last_modified, Znk_NPOS );
					updated = true;
				}
			}
			if( updated ){
				saveLastModifiedData( lmd );
			}
		}
		ZnkVarpAry_destroy( lmd );
	}
	return updated;
}

/***
 * est_valがホスト名だけの場合、ここで得られるreq_urpは 単に / という文字である場合もあり得る.
 *
 * また5chのjump.5ch.net(外部サイトへのリンクをクリックした場合にまず飛ばされる確認ページ)では
 * そのリンクは jump.5ch.net/?http://... のようになっている.
 * つまり、このまま素直に / 以下をとると、req_urpが /?http://... のようになる.
 *
 * このような文字列は以降の処理でresult_filenameやキャッシュファイルの名前を決定する際に問題になるため、
 * 慎重に処理する必要がある.
 *
 * Note.
 * & : %26
 * ? : %3f
 * = : %3d
 * # : %23
 * http://may.2chan.net/b/futaba.php?mode=cat&sort=1
 * http://may.2chan.net/b/futaba.php%3fmode=cat%26sort=1
 * http://127.0.0.1:8124/cgi-bin/easter.cgi?est_mode=get&est_src=http://may.2chan.net/b/futaba.php%3fmode%3dcat%26sort%3d1
 *
 * Note.
 * static_cachedフラグ :
 * これがtrueになるのは静的なドキュメント(例えば画像やjavascript本体のようなもの)で
 * かつCacheとして存在する場合に限る.
 *
 * 以下ではこれが false の場合のみ更新処理を試みる.
 * (従って静的なドキュメントであってもCacheとして存在しない初アクセスの場合では、
 * 以下の処理は行われる)
 */
void
EstGet_procGet( RanoCGIEVar* evar, ZnkVarpAry post_vars, const char* est_val, bool is_view )
{
	RanoModule  mod    = NULL;
	const char* target = "";
	const char* src    = "";
	char        hostname[ 1024 ]          = "";
	char        content_type[ 1024 ]      = "application/octet-stream";
	char        new_last_modified[ 1024 ] = "";
	bool        updated        = false;
	bool        is_404         = false;
	bool        use_head       = true;
	bool        save_img_cache = false;
	ZnkStr      req_urp        = ZnkStr_new( "" );
	ZnkStr      console_msg    = ZnkStr_new( "" );
	ZnkStr      ermsg          = ZnkStr_new( "" );
	ZnkDate     date = { 0 };
	const char* est_cmd_name = is_view ? "est_view" : "est_get";

	{
		ZnkDate_getCurrent( &date );
		ZnkDate_getStr( console_msg, Znk_NPOS, &date, ZnkDateStr_e_Std );
		ZnkStr_addf( console_msg, " %s begin\n", est_cmd_name );
	}

	src    = EstBase_getHostnameAndRequrp_fromEstVal( hostname, sizeof(hostname), req_urp, est_val );
	target = EstConfig_getTargetAndModule( &mod, hostname );

	if( target && !ZnkS_empty(hostname) ){
		const char* tmpdir_com = EstConfig_getTmpDirCommon( true );
		const char* tmpdir_pid = EstConfig_getTmpDirPID( true );

		ZnkStr       result_filename = ZnkStr_newf( "./%sresult.dat", tmpdir_pid );
		const char*  ua              = "Firefox";
		const char*  parent_proxy    = EstConfig_parent_proxy();
		bool         static_cached   = false;
		RanoTextType txt_type        = RanoText_Binary;
		const char*  unesc_req_urp   = ZnkStr_cstr(req_urp);

		use_head = isUseHeadTarget( target );

		parseEstCmd_byPostVars( post_vars, content_type, sizeof(content_type),
				unesc_req_urp, &save_img_cache, &static_cached,
				hostname, result_filename, target,
				console_msg );

		if( use_head ){
			/***
			 * HEAD が有効なサイトの場合.
			 */
			if( !static_cached ){
				/***
				 * これを行うのは動的なドキュメント(例えば掲示板におけるHTML出力)のようなコンテンツ、
				 * あるいは静的なドキュメントであってもまだキャッシュされていない場合である.
				 */
				is_404 = updateCookieAndLastModified_byHead( tmpdir_com, tmpdir_pid,
						target, evar,
						hostname, unesc_req_urp,
						mod, ua, parent_proxy,
						result_filename,
						content_type, sizeof(content_type),
						new_last_modified, sizeof(new_last_modified) );

				if( !is_404 ){
					updated = updateLastModifiedData( ZnkStr_cstr(result_filename), new_last_modified );
					/***
					 * 「サムネ保存」が押された場合におけるprocGetでは
					 * スレ全体を強制的にupdateさせる.
					 */
					if( save_img_cache ){
						updated = true;
					}
					if( updated ){
						int status_code = downloadWithCookie( tmpdir_com, tmpdir_pid,
								target, evar->http_cookie_,
								hostname, unesc_req_urp,
								mod, ua, parent_proxy,
								result_filename, ermsg,
								content_type, sizeof(content_type),
								new_last_modified, sizeof(new_last_modified) );

						if( status_code == 200 ){
							txt_type = RanoCGIUtil_getTextType( content_type );
							filter( txt_type, console_msg,
									est_cmd_name, hostname, unesc_req_urp,
									evar->http_cookie_,
									ZnkStr_cstr(result_filename), src, target, save_img_cache );
						}
					}
				}
			}

		} else {
			/***
			 * HEAD が無効なサイトも存在する.
			 * その場合は常にupdatedとする.
			 */
			updated = true;
			if( !static_cached ){
				int status_code = downloadWithCookie( tmpdir_com, tmpdir_pid,
						target, evar->http_cookie_,
						hostname, unesc_req_urp,
						mod, ua, parent_proxy,
						result_filename, ermsg,
						content_type, sizeof(content_type),
						NULL, 0 );

				txt_type = RanoCGIUtil_getTextType( content_type );
				if( status_code == 200 ){
					filter( txt_type, console_msg,
							est_cmd_name, hostname, unesc_req_urp,
							evar->http_cookie_,
							ZnkStr_cstr(result_filename), src, target, save_img_cache );
				}
			}
		}

		{
			ZnkDate_getCurrent( &date );
			ZnkDate_getStr( console_msg, Znk_NPOS, &date, ZnkDateStr_e_Std );
			ZnkStr_addf( console_msg, " %s end\n", est_cmd_name );
		}

		displayResult( is_404, updated, console_msg,
				content_type, ZnkStr_cstr(result_filename), is_view );

		ZnkStr_delete( result_filename );
	} else {
		RanoCGIMsg_initiate( true, NULL );
		RanoCGIMsg_begin();
		Znk_printf( "est_src is not specified or it's hostname is not contained in target.myf.\n" );
		RanoCGIMsg_end();
		RanoCGIMsg_finalize();
	}
	ZnkStr_delete( req_urp );
	ZnkStr_delete( ermsg );
	ZnkStr_delete( console_msg );
}

