#include "cgi_helper.h"

#include <Doc_html.h>
#include <Doc_source.h>

#include <Rano_cgi_util.h>
#include <Rano_html_ui.h>
#include <Rano_dir_util.h>
#include <Rano_conf_util.h>

#include <Znk_s_base.h>
#include <Znk_stdc.h>
#include <Znk_varp_ary.h>
#include <Znk_str_path.h>
#include <Znk_htp_util.h>
#include <Znk_envvar.h>

#include <stdio.h>
#include <ctype.h>

static bool
isInstallFile( const char* file_path, void* arg )
{
	ZnkStrAry sda = Znk_force_ptr_cast( ZnkStrAry, arg );
	const size_t size = ZnkStrAry_size( sda );
	size_t idx;
	const char*  p = Znk_strrchr( file_path, '/' );
	const char*  tail = p ? ( p+1 ) : file_path;
	const size_t tail_leng = Znk_strlen( tail );
	for( idx=1; idx<size; ++idx ){
		const char* filt = ZnkStrAry_at_cstr( sda, idx );
		const char* q = Znk_strrchr( filt, '/' );
		if( p && q ){
			if( ZnkS_isEndEx( file_path, p - file_path, filt, q - filt ) ){
				const char* ptn  = q+1;
				if( ZnkS_isMatchSWC( ptn, Znk_NPOS, tail, tail_leng ) ){
					return true;
				}
			}
		} else if( p ){
			const char* ptn  = filt;
			if( ZnkS_isMatchSWC( ptn, Znk_NPOS, tail, tail_leng ) ){
				return true;
			}
		} else if( q ){
			/* none */
		} else {
			const char* ptn  = filt;
			if( ZnkS_isMatchSWC( ptn, Znk_NPOS, tail, tail_leng ) ){
				return true;
			}
		}
	}
	return false;
}

static void
installFiles( ZnkPrimAry prims, const char* query_name, size_t query_name_idx, const char* src_dir, const char* dst_dir, ZnkStr ermsg )
{
	bool result = true;
	if( prims ){
		ZnkPrim prim;
		const size_t size = ZnkPrimpAry_size( prims );
		size_t idx;
		for( idx=0; idx<size; ++idx ){
			prim = ZnkPrimpAry_at( prims, idx );
			if( prim && ZnkPrim_type( prim ) == ZnkPrim_e_StrAry ){
				ZnkStrAry   sda  = ZnkPrim_strAry( prim );
				const char* name = ZnkStrAry_at_cstr( sda, query_name_idx );
				if( ZnkS_eq( name, query_name ) ){
					RanoDirUtil_installDir( src_dir, dst_dir,
							"install", ermsg,
							isInstallFile, sda,
							NULL, NULL );
				}
			}
		}
	}
}

static ZnkPrim
findPrim_byQueryName( const ZnkPrimAry prims, const char* query_name, size_t query_name_idx )
{
	ZnkPrim prim = NULL;
	if( prims ){
		const size_t size = ZnkPrimpAry_size( prims );
		size_t idx;
		for( idx=0; idx<size; ++idx ){
			prim = ZnkPrimpAry_at( prims, idx );
			if( prim && ZnkPrim_type( prim ) == ZnkPrim_e_StrAry ){
				ZnkStrAry   sda  = ZnkPrim_strAry( prim );
				const char* name = ZnkStrAry_at_cstr( sda, query_name_idx );
				if( ZnkS_eq( name, query_name ) ){
					return prim;
				}
			}
		}
	}
	/* not found */
	return NULL;
}

static void
makeTargetSelectBarUI( ZnkStr ans, ZnkMyf conf_myf, const char* target_current_val )
{
	size_t        idx;
	ZnkMyfSection sec      = NULL;
	const char*   sec_name = NULL;
	ZnkPrimAry    prims    = NULL;
	const char*   hostname = NULL;
	ZnkStrAry     val_list  = ZnkStrAry_create( true );
	ZnkStrAry     name_list = ZnkStrAry_create( true );

	prims = ZnkMyf_find_prims( conf_myf, "html_doc" );
	if( prims ){
		const size_t  size = ZnkPrimpAry_size( prims );
		for( idx=0; idx<size; ++idx ){
			ZnkPrim prim = ZnkPrimpAry_at( prims, idx );
			if( prim ){
				ZnkStrAry   sda  = ZnkPrim_strAry( prim );
				const char* name = ZnkStrAry_at_cstr( sda, 0 );
				if( target_current_val == NULL ){
					target_current_val = name;
				}
				ZnkStrAry_push_bk_cstr( val_list,  name, Znk_NPOS );
				ZnkStrAry_push_bk_cstr( name_list, name, Znk_NPOS );
			}
		}
	}
	ZnkStr_add( ans, "" );
	RanoHtmlUI_makeSelectBox( ans,
			"target", target_current_val, true,
			val_list, name_list );

	ZnkStrAry_destroy( val_list );
	ZnkStrAry_destroy( name_list );
}

static ZnkStr     st_moai_dir = NULL;
static ZnkStr     st_filters_dir = NULL;
static ZnkStr     st_profile_dir = NULL;

static void
setupConfigDir( ZnkStr ans, const char* given_dir, const char* base_dir, const char* when_empty_dir )
{
	bool is_relative = true;
	if( ZnkS_empty(given_dir) ){
		given_dir = when_empty_dir;
	} else {
		if( given_dir[ 0 ] == '/' ){
			/* UNIX fullpath */
			is_relative = false;
		} else if( given_dir[ 1 ] == ':' && isalpha( given_dir[ 0 ] ) ){
			/* Windows fullpath */
			is_relative = false;
		}
	}
	if( is_relative ){
		ZnkStr_setf( ans, "%s/%s", base_dir, given_dir );
	} else {
		ZnkStr_setf( ans, "%s", given_dir );
	}
}

static void
initBasicDirs( void )
{
	char filename[ 1024 ] = "";
	ZnkMyf config = ZnkMyf_create();

	const char*  moai_dir = NULL;
	moai_dir = RanoConfUtil_moai_dir( NULL );
	if( moai_dir == NULL ){
		RanoCGIUtil_rano_app_print_error( "moai_dir is not found." );
		goto FUNC_END;
	}
	st_moai_dir = ZnkStr_new( moai_dir );

	Znk_snprintf( filename, sizeof(filename), "%sconfig.myf", ZnkStr_cstr(st_moai_dir) );
	st_filters_dir = ZnkStr_newf( "%s/filters",      ZnkStr_cstr(st_moai_dir) );
	st_profile_dir = ZnkStr_newf( "%s/moai_profile", ZnkStr_cstr(st_moai_dir) );
	if( ZnkMyf_load( config, filename ) ){
		ZnkVarAry vars = ZnkMyf_find_vars( config, "config" );
		if( vars ){
			ZnkVar var = NULL;

			var = ZnkVarpAry_findObj_byName_literal( vars, "filters_dir", false );
			if( var ){
				setupConfigDir( st_filters_dir, ZnkVar_cstr(var), ZnkStr_cstr(st_moai_dir), "filters" );
			}

			var = ZnkVarpAry_findObj_byName_literal( vars, "profile_dir", false );
			if( var ){
				setupConfigDir( st_profile_dir, ZnkVar_cstr(var), ZnkStr_cstr(st_moai_dir), "moai_profile" );
			}
		}
	}

FUNC_END:
	ZnkMyf_destroy( config );

	/***
	 * filters_dir and profile_dir setting.
	 * This must be relative directory from st_moai_dir
	 */
	{
		{
			char* envvar_val = ZnkEnvVar_get( "MOAI_PROFILE_DIR" );
			if( envvar_val ){
				/* defined in OS */
				ZnkStr_set( st_profile_dir, envvar_val );
			}
			ZnkEnvVar_free( envvar_val );
		}
	}
}

static void
parseMainPostVars( RanoCGIEVar* evar, const ZnkVarpAry post_vars )
{
	const char* moai_dir = NULL;
	const char* authentic_key = NULL;
	bool        is_authenticated = false;

	const char* template_html_file = "templates/docgen.html";
	ZnkBird     bird     = ZnkBird_create( "#[", "]#" );
	ZnkStr      ermsg    = ZnkStr_new( "" );
	ZnkMyf      conf_myf = ZnkMyf_create();
	ZnkVar      command  = NULL;
	ZnkVar      target_var = NULL;

	ZnkBird_regist( bird, "docgen_src_dir", "" );
	ZnkBird_regist( bird, "docgen_dst_dir", "" );
	ZnkBird_regist( bird, "install_src_dir", "" );
	ZnkBird_regist( bird, "install_dst_dir", "" );
	ZnkBird_regist( bird, "profile_dir", ZnkStr_cstr( st_profile_dir ) );

	/***
	 * CGIHelper_initEsscentialでは、moai_dir, Moai authentic_keyを取得します.
	 * またpost_varsに設定されたMoai_AuthenticKeyとauthentic_keyの値を比較することより
	 * Moai認証に成功したか否かの真偽値を示すis_authenticatedの値を取得します.
	 */
	if( !CGIHelper_initEsscential( post_vars,
			&moai_dir, &authentic_key, &is_authenticated,
			ermsg ) ){
		goto FUNC_END;
	}

	/***
	 * 設定ファイルのロード.
	 * 今回は認証されていない場合でもロードを許可してあります.
	 */
	if( !ZnkMyf_load( conf_myf, "docgen.myf" ) ){
		/* error */
		ZnkStr_addf( ermsg, "Error : docgen.myf does not exist.\n" );
		goto FUNC_END;
	}

	/***
	 * コマンドの処理.
	 * post_varsにcommandが指定されている場合、その値に応じて実行するコマンド処理を行います.
	 */
	command    = ZnkVarpAry_findObj_byName_literal( post_vars, "command", false );
	target_var = ZnkVarpAry_findObj_byName_literal( post_vars, "target", false );
	if( command ){
		const char* cmd = ZnkVar_cstr(command);
		ZnkStr_addf( ermsg, "Report : command=[%s]. is_authenticated=[%d]\n", cmd, is_authenticated );
		ZnkStr_addf( ermsg, "Report : profile_dir=[%s].\n", ZnkStr_cstr(st_profile_dir) );

		if( ZnkS_eq( cmd, "html_doc" ) && is_authenticated ){
			ZnkVar src_dir = ZnkVarpAry_findObj_byName_literal( post_vars, "docgen_src_dir", false );
			ZnkVar dst_dir = ZnkVarpAry_findObj_byName_literal( post_vars, "docgen_dst_dir", false );
			if( target_var == NULL ){
				ZnkStr_addf( ermsg, "Error : target is not found.\n" );
				goto FUNC_END;
			}
			if( src_dir && dst_dir ){
				const char* s_dir = ZnkVar_cstr( src_dir );
				const char* d_dir = ZnkVar_cstr( dst_dir );
				bool is_valid_s = false;
				bool is_valid_d = false;
				ZnkStr s_path = ZnkStr_new( "" );
				const char* target = ZnkVar_cstr( target_var );
				ZnkPrimAry prims = ZnkMyf_find_prims( conf_myf, "html_doc" );
				ZnkPrim    prim  = findPrim_byQueryName( prims, target, 0 );
				if( prim ){
					ZnkStrAry sda  = ZnkPrim_strAry( prim );
					ZnkStr src_str = ZnkStrAry_at( sda, 1 );
					ZnkStr dst_str = ZnkStrAry_at( sda, 2 );
					ZnkBird_extend_self( bird, src_str, ZnkStr_leng(src_str) );
					ZnkBird_extend_self( bird, dst_str, ZnkStr_leng(dst_str) );
					s_dir = ZnkStr_cstr( src_str );
					d_dir = ZnkStr_cstr( dst_str );
				}

				ZnkBird_regist( bird, "docgen_src_dir", s_dir );
				ZnkBird_regist( bird, "docgen_dst_dir", d_dir );

				if( ZnkDir_getType( s_dir ) == ZnkDirType_e_Directory ){
					ZnkStr_setf( s_path, "%s/menu.myf", s_dir );
					if( ZnkDir_getType( ZnkStr_cstr(s_path) ) == ZnkDirType_e_File ){
						is_valid_s = true;
					}
				}
				if( ZnkDir_getType( d_dir ) == ZnkDirType_e_Directory ){
					is_valid_d = true;
				}

				if( is_valid_s ){
					ZnkStr_addf( ermsg, "Report : OK. s_dir=[%s] is valid.\n", s_dir );
				} else {
					ZnkStr_addf( ermsg, "Report : NG. s_dir=[%s] is invalid\n", s_dir );
				}
				if( is_valid_d ){
					ZnkStr_addf( ermsg, "Report : OK. d_dir=[%s] is valid.\n", d_dir );
				} else {
					ZnkStr_addf( ermsg, "Report : NG. d_dir=[%s] is invalid\n", d_dir );
				}
				ZnkStr_delete( s_path );
				if( is_valid_s && is_valid_d ){
					DocHtml_make( s_dir, d_dir, ermsg );
					ZnkStr_addf( ermsg, "Report : DocHtml_make done.\n" );
				}
			}
	
		} else if( ZnkS_eq( cmd, "source_doc" ) && is_authenticated ){
			ZnkVar src_dir = ZnkVarpAry_findObj_byName_literal( post_vars, "src_dir", false );
			ZnkVar dst_dir = ZnkVarpAry_findObj_byName_literal( post_vars, "dst_dir", false );
			if( src_dir && dst_dir ){
				const char* s_dir = ZnkVar_cstr( src_dir );
				const char* d_dir = ZnkVar_cstr( dst_dir );
				DocSource_make( s_dir, d_dir );
			}

		} else if( ZnkS_eq( cmd, "install" ) && is_authenticated ){
			ZnkVar src_dir = ZnkVarpAry_findObj_byName_literal( post_vars, "install_src_dir", false );
			ZnkVar dst_dir = ZnkVarpAry_findObj_byName_literal( post_vars, "install_dst_dir", false );
			if( target_var == NULL ){
				ZnkStr_addf( ermsg, "Error : target is not found.\n" );
				goto FUNC_END;
			}
			if( src_dir && dst_dir ){
				const char* s_dir = ZnkVar_cstr( src_dir );
				const char* d_dir = ZnkVar_cstr( dst_dir );
				bool is_valid_s = false;
				bool is_valid_d = false;
				ZnkStr s_path = ZnkStr_new( "" );
				const char* target = ZnkVar_cstr( target_var );
				ZnkPrimAry prims = ZnkMyf_find_prims( conf_myf, "install" );
				ZnkPrim    prim  = findPrim_byQueryName( prims, target, 0 );
				if( prim ){
					ZnkStrAry sda  = ZnkPrim_strAry( prim );
					ZnkStr src_str = ZnkStrAry_at( sda, 1 );
					ZnkStr dst_str = ZnkStrAry_at( sda, 2 );
					ZnkBird_extend_self( bird, src_str, ZnkStr_leng(src_str) );
					ZnkBird_extend_self( bird, dst_str, ZnkStr_leng(dst_str) );
					s_dir = ZnkStr_cstr( src_str );
					d_dir = ZnkStr_cstr( dst_str );
				}

				ZnkBird_regist( bird, "install_src_dir", s_dir );
				ZnkBird_regist( bird, "install_dst_dir", d_dir );

				{
					ZnkPrimAry install_filter = ZnkMyf_find_prims( conf_myf, "install_filter" );
					installFiles( install_filter, target, 0, s_dir, d_dir, ermsg );
				}
			}
	
		} else {
			ZnkStr_addf( ermsg, "Report : Error : This request is not authenticated.\n" );
		}

	} else {
		/***
		 * 何もコマンドを実行せず、単純にスタートとなるUIを表示する場合.
		 * この場合は認証されていない場合でも表示可能としておきます.
		 */
	}

FUNC_END:
	{
		const char* target_current_val = NULL;
		ZnkStr ui   = ZnkStr_new( "" );
		ZnkVar varp = ZnkVarpAry_findObj_byName_literal( post_vars, "target", false );
		if( varp ){
			target_current_val = ZnkVar_cstr( varp );
		}
		makeTargetSelectBarUI( ui, conf_myf, target_current_val );
		ZnkBird_regist( bird, "target_select_bar", ZnkStr_cstr(ui) );
		ZnkStr_delete( ui );
	}
	/***
	 * 正規のUIにのみMoai_AuthenticKeyの値を埋め込みます.
	 * template_html_fileの中身は、例えば以下のようにして form から post した際に
	 * このMoai_AuthenticKeyがpost_vars変数に含まれるようにしておきます.
	 *
	 * <input type=hidden name=Moai_AuthenticKey value="#[Moai_AuthenticKey]#">
	 *
	 * XhrDMZ側でブラウザにて実行されるJavascriptは、このUIのHTMLに対してXHtmlRequestを発行することができず、
	 * 従ってMoai_AuthenticKeyの値をユーザ以外が自動的には読むことができないという仕組みになっています.
	 *
	 * また ermsgの値を埋め込むことで問題が発生した場合にその原因を容易に追跡できます.
	 * template_html_fileの中身は、例えば以下のように #[ermsg]# という文字列を適当な場所に埋め込んでおきます.
	 *
	 * <b>System Console:</b><br>#[ermsg]#<br>
	 *
	 * CGIHelper_printTemplateHTML ではこのような埋め込みを展開する処理を行っています.
	 */
	if( !CGIHelper_printTemplateHTML( evar, bird, template_html_file, authentic_key, ermsg ) ){
		Znk_printf( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
		Znk_printf( "<html><body>\n" );
		Znk_printf( "Cannot open template HTML file. [%s]\n", template_html_file );
		Znk_printf( "</body></html>\n" );
		Znk_fflush( Znk_stdout() );
	}

	ZnkBird_destroy( bird );
	ZnkMyf_destroy( conf_myf );
	ZnkStr_delete( ermsg );
}

int main( int argc, char** argv )
{
	/***
	 * Create+Get Environment Variables for CGI
	 */
	RanoCGIEVar* evar = RanoCGIEVar_create();

	/***
	 * Get query_string;
	 */
	const char* query_string = evar->query_string_ ? evar->query_string_ : "";

	/***
	 * Create post_vars.
	 */
	ZnkVarpAry post_vars = ZnkVarpAry_create( true );

	/***
	 * for Windows : change stdin/stdout to binary-mode.
	 */
	Znk_Internal_setMode( 0, true );
	Znk_Internal_setMode( 1, true );

	initBasicDirs();

	/***
	 * Get post_vars from query_string.
	 */
	RanoCGIUtil_splitQueryString( post_vars, query_string, Znk_NPOS, false );

	/***
	 * CGIにおけるフォームの投稿データをpost_varsへと取得します.
	 * またこの関数においては第1引数evarの指定も必要となります.
	 */
	RanoCGIUtil_getPostedFormData( evar, post_vars, NULL, NULL, NULL, NULL, true );

	/***
	 * MoaiCGIのメイン処理.
	 */
	parseMainPostVars( evar, post_vars );

#if 0
	if( DocHtml_isPauseOnExit() ){
		Znk_getchar();
	}
#endif

	ZnkVarpAry_destroy( post_vars );
	RanoCGIEVar_destroy( evar );
	return 0;
}
