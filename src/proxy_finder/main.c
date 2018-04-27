#include "proxy_finder.h"
#include "cgi_helper.h"
#include <Rano_cgi_util.h>
#include <Rano_html_ui.h>

#include <Znk_varp_ary.h>
#include <Znk_str_path.h>
#include <Znk_htp_util.h>

#include <stdio.h>
#include <string.h>


static bool
getProxy( ZnkVarpAry post_vars, ZnkMyf conf_myf, const char* moai_dir, ZnkStr ermsg )
{
	bool        result          = false;
	ZnkStr      result_filename = ZnkStr_new( "" );
	const char* target          = NULL;
	ZnkVarpAry  target_vars     = NULL;
	ZnkVarp     varp            = NULL;

	/***
	 * post_varsからtarget変数を得て、その値を取得します.
	 */
	varp = ZnkVarpAry_findObj_byName_literal( post_vars, "target", false );
	if( varp ){
		target      = ZnkVar_cstr( varp );
		target_vars = ZnkMyf_find_vars( conf_myf, target );
	}
	ZnkStr_addf( ermsg, "Report : target is [%s].\n", target );

	/* Phase-1 : download cache */
	if( target_vars ){
		const char* hostname      = NULL;
		const char* url_underpath = NULL;

		/***
		 * 設定ファイル(conf_myf)から以下の項目を取得します.
		 */
		varp = ZnkVarpAry_findObj_byName_literal( target_vars, "hostname", false );
		if( varp ){
			hostname = ZnkVar_cstr( varp );
		}
		varp = ZnkVarpAry_findObj_byName_literal( target_vars, "url_underpath", false );
		if( varp ){
			url_underpath = ZnkVar_cstr( varp );
		}

		/***
		 * http://hostname/url_underpath へ接続し、そのHTMLファイルを得ます.
		 * 成功した場合、それがキャッシュとして保存され、そのキャッシュのファイル名が
		 * result_filenameに格納されます.
		 */
		result = ProxyFinder_download( result_filename,
				hostname, url_underpath, target, ermsg );
		if( result ){
			ZnkStr_addf( ermsg, "Report : download cache file as filename=[%s].\n", ZnkStr_cstr(result_filename) );
		}
	}

	/* Phase-2 : analysis */
	if( target_vars && result ){
		char parent_proxy_filename[ 256 ] = "";
		const char* ip_ptn_begin   = "IP_PTN_BEGIN";   /* dummy pattern */
		const char* ip_ptn_end     = "IP_PTN_END";     /* dummy pattern */
		const char* port_ptn_begin = "PORT_PTN_BEGIN"; /* dummy pattern */
		const char* port_ptn_end   = "PORT_PTN_END";   /* dummy pattern */
		bool        is_hex_port    = false;

		/***
		 * 最終的な結果(Moaiが使用するparent_proxy.txt)を示す相対パスとなります.
		 */
		Znk_snprintf( parent_proxy_filename, sizeof(parent_proxy_filename),
				"%sparent_proxy.txt", moai_dir );

		/***
		 * 設定ファイル(conf_myf)から以下の項目を取得します.
		 */
		varp = ZnkVarpAry_findObj_byName_literal( target_vars, "ip_ptn_begin", false );
		if( varp ){
			ip_ptn_begin = ZnkVar_cstr( varp );
		}
		varp = ZnkVarpAry_findObj_byName_literal( target_vars, "ip_ptn_end", false );
		if( varp ){
			ip_ptn_end = ZnkVar_cstr( varp );
		}
		varp = ZnkVarpAry_findObj_byName_literal( target_vars, "port_ptn_begin", false );
		if( varp ){
			port_ptn_begin = ZnkVar_cstr( varp );
		}
		varp = ZnkVarpAry_findObj_byName_literal( target_vars, "port_ptn_end", false );
		if( varp ){
			port_ptn_end = ZnkVar_cstr( varp );
		}
		varp = ZnkVarpAry_findObj_byName_literal( target_vars, "is_hex_port", false );
		if( varp ){
			is_hex_port = ZnkS_eq( ZnkVar_cstr( varp ), "true" );
		}

		/**
		 * ip_ptn_begin と ip_ptn_end の間にある文字列をIP,
		 * port_ptn_begin と port_ptn_end の間にある文字列をport番号とみなして解析.
		 * また、portが16進数で記述されているサイトの場合はis_hex_portをtrueに指定する.
		 */
		ZnkStr_addf( ermsg, "Report : Now, we will analysis downloaded file=[%s].\n", ZnkStr_cstr(result_filename) );
		ProxyFinder_analysis( ZnkStr_cstr(result_filename), parent_proxy_filename,
				ip_ptn_begin,   ip_ptn_end,
				port_ptn_begin, port_ptn_end,
				is_hex_port, ermsg );
	}
	ZnkStr_delete( result_filename );
	return result;
}

static void
makeTargetSelectBarUI( ZnkStr ans, ZnkMyf conf_myf, const char* target_current_val )
{
	const size_t  size = ZnkMyf_numOfSection( conf_myf );
	size_t        idx;
	ZnkMyfSection sec  = NULL;
	const char*   sec_name = NULL;
	ZnkVarpAry    vars = NULL;
	ZnkVarp       varp = NULL;
	const char*   hostname = NULL;
	ZnkStrAry     val_list  = ZnkStrAry_create( true );
	ZnkStrAry     name_list = ZnkStrAry_create( true );

	for( idx=0; idx<size; ++idx ){
		sec = ZnkMyf_atSection( conf_myf, idx );
		if( ZnkMyfSection_type( sec ) == ZnkMyfSection_e_Vars ){
			vars = ZnkMyfSection_vars( sec );
			sec_name = ZnkMyfSection_name( sec );
			varp = ZnkVarpAry_findObj_byName_literal( vars, "hostname", false );
			if( varp ){
				hostname = ZnkVar_cstr( varp );
				ZnkStrAry_push_bk_cstr( name_list, hostname, Znk_NPOS );
				ZnkStrAry_push_bk_cstr( val_list,  sec_name, Znk_NPOS );
				if( target_current_val == NULL ){
					target_current_val = sec_name;
				}
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

static void
parseMainPostVars( RanoCGIEVar* evar, const ZnkVarpAry post_vars )
{
	const char* moai_dir = NULL;
	const char* authentic_key = NULL;
	bool        is_authenticated = false;

	const char* template_html_file = "templates/proxy_finder.html";
	ZnkBird     bird     = ZnkBird_create( "#[", "]#" );
	ZnkStr      ermsg    = ZnkStr_new( "" );
	ZnkMyf      conf_myf = ZnkMyf_create();
	ZnkVarp     command  = NULL;

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
	 * ProxyFinder設定ファイルのロード.
	 * 今回は認証されていない場合でもロードを許可してあります.
	 */
	if( !ZnkMyf_load( conf_myf, "proxy_finder.myf" ) ){
		/* error */
		ZnkStr_addf( ermsg, "Error : proxy_finder.myf does not exist.\n" );
		goto FUNC_END;
	}

	/***
	 * コマンドの処理.
	 * post_varsにcommandが指定されている場合、その値に応じて実行するコマンド処理を行います.
	 */
	command = ZnkVarpAry_findObj_byName_literal( post_vars, "command", false );
	if( command ){
		if( ZnkS_eq( ZnkVar_cstr(command), "get_proxy" ) && is_authenticated ){
			/***
			 * このコマンドに関しては認証が必要であるとします.
			 */
			ZnkStr_addf( ermsg, "Report : OK. This request is authenticated.\n" );
			getProxy( post_vars, conf_myf, moai_dir, ermsg );
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
		ZnkStr  ui   = ZnkStr_new( "" );
		ZnkVarp varp = ZnkVarpAry_findObj_byName_literal( post_vars, "target", false );
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
		printf( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
		printf( "<html><body>\n" );
		printf( "Cannot open template HTML file. [%s]\n", template_html_file );
		printf( "</body></html>\n" );
		fflush( stdout );
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

	ZnkVarpAry_destroy( post_vars );
	RanoCGIEVar_destroy( evar );
	return 0;
}
