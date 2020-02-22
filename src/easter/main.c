/***
 * Easter : CGI Proxy System (introduced by Ver2.0)
 */
#include <Est_config.h>
#include <Est_get.h>
#include <Est_post.h>
#include <Est_filter.h>
#include <Est_link_manager.h>
#include <Est_boxmap_viewer.h>
#include <Est_search_manager.h>
#include <Est_img_viewer.h>
#include <Est_topic.h>
#include <Est_tag_manager.h>

#include <Rano_log.h>
#include <Rano_cgi_util.h>
#include <Rano_htp_boy.h>
#include <Rano_conf_util.h>

#include <Znk_str.h>
#include <Znk_s_base.h>
#include <Znk_dir.h>
#include <Znk_varp_ary.h>
#include <Znk_net_base.h>
#include <Znk_zlib.h>
#include <Znk_process.h>

#include <stdlib.h>


#define IS_OK( val ) (bool)( (val) != NULL )

static void
procSystemError( RanoCGIEVar* evar, const char* result_view, const char* msg )
{
	const char* xhr_auth_host = EstConfig_XhrAuthHost();
	const char* template_html_file = "templates/system_error.html";
	ZnkBird bird = ZnkBird_create( "#[", "]#" );
	ZnkBird_regist( bird, "xhr_auth_host", xhr_auth_host );
	ZnkBird_regist( bird, "result_view",   result_view );
	ZnkBird_regist( bird, "msg",           msg );
	RanoCGIUtil_printTemplateHTML( evar, bird, template_html_file );
	ZnkBird_destroy( bird );
}

static void
parseQueryString( RanoCGIEVar* evar )
{
	const char* query_string = RanoCGIUtil_getQueryString( evar );
	ZnkVarpAry post_vars = ZnkVarpAry_create( true );
	bool is_unescape_val = false;
	ZnkVarp varp;
	ZnkStr msg = ZnkStr_new( "" );
	const bool is_xhr_authhost = EstConfig_isXhrAuthHost( evar );

	RanoCGIUtil_splitQueryString( post_vars, query_string, Znk_NPOS, is_unescape_val );


	if(        IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_get", false ) )){
		/* XhrDMZにおいても許可 */
		RanoLog_printf( "parseQueryString: procGet\n" );
		EstGet_procGet( evar, post_vars, ZnkVar_cstr(varp), false );
	} else if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_view", false ) )){
		/* XhrDMZにおいても許可 */
		RanoLog_printf( "parseQueryString: procView\n" );
		EstGet_procGet( evar, post_vars, ZnkVar_cstr(varp), true );
	} else if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_reload", false ) )){
		/* Desuetude: est_get と全く同じなので廃止する予定 */
		RanoLog_printf( "parseQueryString: procReload\n" );
		EstGet_procGet( evar, post_vars, ZnkVar_cstr(varp), false );
	} else if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_head", false ) )){
		/* XhrDMZにおいても許可 */
		RanoLog_printf( "parseQueryString: procHead\n" );
		EstGet_procHead( evar, post_vars, ZnkVar_cstr(varp) );
	} else if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_post", false ) )){
		/* XhrDMZにおいても許可 */
		RanoLog_printf( "parseQueryString: procPost\n" );
		EstPost_procPost( evar, post_vars, ZnkVar_cstr(varp), msg, EstFilter_main );
	} else if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_post_ex", false ) )){
		if( is_xhr_authhost ){
			RanoLog_printf( "parseQueryString: procHyperPost\n" );
			EstPost_procPostEx( evar, post_vars, ZnkVar_cstr(varp), msg );
		} else {
			procSystemError( evar,
					"非正規な方法によるアクセスとなっています.<br>"
					"一旦トップページにお戻り下さい.",
					ZnkStr_cstr(msg) );
		}
	} else if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_manager", false ) )){
		const char* manager = ZnkVar_cstr(varp);
		if( is_xhr_authhost ){
			if( ZnkS_eq( manager, "link" ) ){
				EstLinkManager_main( evar, post_vars, msg, EstConfig_authenticKey() );
			} else if( ZnkS_eq( manager, "cache" ) ) {
				EstBoxMapViewer_main( evar, post_vars, msg, EstConfig_authenticKey() );
			} else if( ZnkS_eq( manager, "search" ) ) {
				EstSearchManager_main( evar, post_vars, msg, EstConfig_authenticKey() );
			} else if( ZnkS_eq( manager, "img_viewer" ) ) {
				EstImgViewer_main( evar, post_vars, msg, EstConfig_authenticKey(), false );
			} else if( ZnkS_eq( manager, "video_viewer" ) ) {
				EstImgViewer_main( evar, post_vars, msg, EstConfig_authenticKey(), true );
			} else if( ZnkS_eq( manager, "config" ) ) {
				EstConfigManager_main( evar, post_vars, msg, EstConfig_authenticKey() );
			} else if( ZnkS_eq( manager, "topic" ) ) {
				EstTopicManager_main( evar, post_vars, msg, EstConfig_authenticKey() );
			} else if( ZnkS_eq( manager, "tag_manager" ) ) {
				EstTagManager_main( evar, post_vars, msg, EstConfig_authenticKey() );
			} else if( ZnkS_eq( manager, "hyper_post" ) ) {
				EstHyperPost_main( evar, post_vars, msg );
			}
		} else {
			procSystemError( evar,
					"非正規な方法によるアクセスとなっています.<br>"
					"一旦トップページにお戻り下さい.",
					ZnkStr_cstr(msg) );
		}

	} else {
		/* default */
		if( is_xhr_authhost ){
			EstLinkManager_main( evar, post_vars, msg, EstConfig_authenticKey() );
		} else {
			procSystemError( evar,
					"非正規な方法によるアクセスとなっています.<br>"
					"一旦トップページにお戻り下さい.",
					ZnkStr_cstr(msg) );
		}
	}

	ZnkStr_delete( msg );
	ZnkVarpAry_destroy( post_vars );
}

static bool
runCacheTask( const char* lasttime_dat_path )
{
	const char* argv[] = { "easter_maintainer.cgi", NULL };
	bool        is_wait = false;
	ZnkDate date = { 0 };
	ZnkDate current = { 0 };
	ZnkFile fp = Znk_fopen( lasttime_dat_path, "rb" );
	bool    is_run = false;
	if( fp ){
		char buf[ 4096 ] = "";
		if( Znk_fgets( buf, sizeof(buf), fp ) ){
			ZnkDate_scanStr( &date, buf, ZnkDateStr_e_Std );
			ZnkDate_getCurrent( &current );
		}
		Znk_fclose( fp );
		if( ZnkDate_compareDay( &current, &date ) ){
			is_run = true;
		}
	} else {
		is_run = true;
	}
	if( is_run ){
		argv[ 1 ] = EstConfig_authenticKey();
		/***
		 * 標準出力を継承さぜずに子プロセスを起動する.
		 */
		ZnkProcess_execChild( Znk_NARY(argv), argv, is_wait, ZnkProcessConsole_e_Detached );
		fp = Znk_fopen( lasttime_dat_path, "wb" );
		if( fp ){
			ZnkStr str = ZnkStr_new( "" );
			ZnkDate_getStr( str, Znk_NPOS, &current, ZnkDateStr_e_Std );
			Znk_fputs( ZnkStr_cstr(str), fp );
			Znk_fclose( fp );
			ZnkStr_delete( str );
		}
	}
	return true;
}

int main( int argc, char** argv ) 
{
	int          exit_result = EXIT_FAILURE;
	RanoCGIEVar* evar        = RanoCGIEVar_create();
	size_t       count       = 0;
	const char*  moai_dir    = NULL;
	ZnkStr       ermsg       = ZnkStr_new( "" );

	/* rano app setup */

	if( RanoConfUtil_rano_app_initiate( ".", true, ermsg ) ){
		static const bool is_multi_dir = true;
		count = RanoCGIUtil_rano_app_init_log( "easter", is_multi_dir );
	} else {
		RanoCGIUtil_rano_app_print_error( ZnkStr_cstr(ermsg) );
		goto FUNC_END;
	}

	if( !ZnkZlib_initiate() ){
		RanoCGIUtil_rano_app_print_error( "ZnkZlib_initiate is failure.\n" );
		goto FUNC_END;
	}
	if( !ZnkNetBase_initiate( false ) ){
		RanoCGIUtil_rano_app_print_error( "ZnkNetBase_initiate is failure.\n" );
		goto FUNC_END;
	}

	moai_dir = RanoConfUtil_moai_dir( NULL );
	if( moai_dir == NULL ){
		RanoCGIUtil_rano_app_print_error( "moai_dir is not found." );
		goto FUNC_END;
	}

	/* Application setup */

	{
		char cert_pem_path[ 256 ] = "";
		Znk_snprintf( cert_pem_path, sizeof(cert_pem_path), "%s/cert.pem", moai_dir );
#if defined(__CYGWIN__)
		RanoHtpBoy_initiateHttpsModule( "cygtls-17", cert_pem_path );
#else
		RanoHtpBoy_initiateHttpsModule( "libtls-17", cert_pem_path );
#endif
	}

	EstConfig_initiate( evar, moai_dir, count );

	parseQueryString( evar );

	/***
	 * Cache-Task.
	 */
	{
		const char* tmpdir_common = RanoHtpBoy_getTmpDirCommon();
		char lasttime_dat_path[ 256 ] = "";
		Znk_snprintf( lasttime_dat_path, sizeof(lasttime_dat_path), "%s/easter_maintainer_lasttime.dat", tmpdir_common );
		runCacheTask( lasttime_dat_path );
	}

	exit_result = EXIT_SUCCESS;
FUNC_END:
	RanoCGIEVar_destroy( evar );
	EstConfig_finalize();

	RanoConfUtil_rano_app_finalize();
	RanoConfUtil_moai_dir_finalize();
	ZnkStr_delete( ermsg );

	return exit_result;
}

