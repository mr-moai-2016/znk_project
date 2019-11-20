/**
 * CustomBoy Engine (introduced by Ver2.0 2017.3)
 *
 * @brief
 *  CustomBoy is the next-generation of USERS(User-Agent Screen-size and Everything Randomizer System)
 *  based on Moai Ver2.0 and build up your virtual browser information instantly.
 *  You can also create a custom detailed information about each element of browser,
 *  such as Fingerprint and result values of Javascript snippet code on Moai CGI user-interface.
 *
 * @auther
 *  Main Programmer : K.Yakisoba.H
 *  Library Support : Zenkaku
 *  Supervisor      : Mr.Moai
 *
 * @licence
 *  Copyright (c) Zen-nippon Network Kenkyujo(ZNK)
 *  Licensed under the NYSL( see http://www.kmonos.net/nysl/index.en.html for detail ).
 *
 * @disclaimer
 *  This software is provided 'as-is', without any express or implied warranty.
 *  In no event will the authors be held liable for any damages arising
 *  from the use of this software.
 */
#include <CB_virtualizer.h>
#include <CB_fgp_info.h>
#include <CB_ua_info.h>
#include <CB_config.h>
#include <CB_custom_automatic.h>
#include <CB_custom_postvars.h>
#include <CB_custom_feature.h>
#include <CB_custom_cookie.h>

#include <Rano_cgi_util.h>
#include <Rano_html_ui.h>
#include <Rano_log.h>
#include <Rano_htp_boy.h>
#include <Rano_conf_util.h>

#include <Znk_str.h>
#include <Znk_str_path.h>
#include <Znk_stdc.h>
#include <Znk_htp_util.h>
#include <Znk_dir.h>
#include <Znk_zlib.h>
#include <Znk_net_base.h>

#include <stdlib.h>
#include <ctype.h>


static char st_moai_authentic_key[ 256 ] = "";


typedef enum {
	 Form_e_Get
	,Form_e_Main
	,Form_e_Cookie
	,Form_e_Finger
	,Form_e_Virtual
	,Form_e_Unknown
} FormType;


static FormType
getFormType( ZnkVarpAry cb_vars )
{
	FormType form_type = Form_e_Main;
	ZnkVarp varp = ZnkVarpAry_find_byName_literal( cb_vars, "cb_type", false );
	if( varp ){
		if( ZnkS_eq(ZnkVar_cstr(varp),"postvars") ){
			form_type = Form_e_Main;
		} else if( ZnkS_eq(ZnkVar_cstr(varp),"cookie") ){
			form_type = Form_e_Cookie;
		} else if( ZnkS_eq(ZnkVar_cstr(varp),"feature") ){
			form_type = Form_e_Finger;
		} else if( ZnkS_eq(ZnkVar_cstr(varp),"automatic") ){
			form_type = Form_e_Virtual;
		} else {
			form_type = Form_e_Unknown;
		}
	} else {
		form_type = Form_e_Virtual;
	}
	return form_type;
}


struct CategoryInfo_tag{
	const char* id_;
	const char* name_;
};

static void
getCategory_select( ZnkStr selui, const char* select_name, const char* select_id )
{
	static struct CategoryInfo_tag id_list[] = {
		{ "pc"     , "PC(Windows/Linux/Mac)" },
		{ "mobile" , "Mobile(Android/iPhone)" },
		{ "random" , "All(PC/Mobile)" },
		{ "firefox", "Firefox" },
		{ "chrome" , "Chrome" },
		{ "opera"  , "Opera" },
		{ "safari" , "Safari" },
		{ "ie"     , "IE" },
		{ "android", "Android" },
		{ "iphone" , "iPhone" },
	};
	ZnkStrAry val_list  = ZnkStrAry_create( true );
	ZnkStrAry name_list = ZnkStrAry_create( true );
	size_t idx;

	for( idx=0; idx<Znk_NARY(id_list); ++idx ){
		ZnkStrAry_push_bk_cstr( val_list,  id_list[idx].id_,   Znk_NPOS );
		ZnkStrAry_push_bk_cstr( name_list, id_list[idx].name_, Znk_NPOS );
	}
	RanoHtmlUI_makeSelectBox( selui,
			select_name, select_id, true,
			val_list, name_list );

	ZnkStrAry_destroy( val_list );
	ZnkStrAry_destroy( name_list );

}

static const char* st_hint_base_PV  = "/moai2.0/customboy_hints_PostVars.html#";
static const char* st_hint_base_Fgp = "/moai2.0/customboy_hints_Fingerprint.html#";
static const char* st_hint_base_Snp = "/moai2.0/customboy_hints_Snippet.html#";


static void
parseQueryString( RanoCGIEVar* evar )
{
	ZnkStr str = ZnkStr_new( evar->query_string_ );
	ZnkStr msg = ZnkStr_new( "" );
#if defined(__ANDROID__)
	ZnkStr category = ZnkStr_new( "mobile" );
#else
	ZnkStr category = ZnkStr_new( "pc" );
#endif
	ZnkVarpAry cb_vars = ZnkVarpAry_create( true );
	CBFgpInfo fgp_info = CBFgpInfo_create();
	ZnkVarpAry main_vars = ZnkVarpAry_create( true );
	struct CBUAInfo_tag ua_info = {
		CBUABrowser_e_Unknown,
		0,0,
		CBUAOS_e_Unknown,
		CBUAMachine_e_Unknown,
		0,0,0,
	};
	FormType form_type = Form_e_Get;
	CBConfigInfo* info = NULL;
	const char* template_html_file = NULL;
	bool is_unescape_val  = false;
	bool all_cookie_clear = false;

	RanoCGIUtil_splitQueryString( cb_vars, ZnkStr_cstr(str), ZnkStr_leng(str), is_unescape_val );

	{
		ZnkVarp varp = ZnkVarpAry_find_byName_literal( cb_vars, "cb_target", false );
		if( varp ){
			CBConfig_setNegotiatingTarget( ZnkVar_cstr( varp ) );
		}
	}
	info = CBConfig_theInfo2(); 
	if( info ){
		template_html_file = "templates/automatic.html";
	}

	{
		uint64_t ptua64 = 0;
		ZnkVarp varp;
		RanoModule mod = NULL;
		struct ZnkHtpHdrs_tag htp_hdrs = { 0 };
		ZnkStr pst_str = ZnkStr_new( "" );
		ZnkStr lacking_var = ZnkStr_new( "" );
		ZnkStr cb_src  = ZnkStr_new( CBConfig_theCBSrc() );
		ZnkBird bird = ZnkBird_create( "#[", "]#" );
		ZnkStr RE_key = ZnkStr_new( "webkit" );
		CBStatus cb_status = CBStatus_e_Editing;
		bool is_authenticated = false;

		ZnkBird_regist( bird, "hint_base_PV",   st_hint_base_PV );
		ZnkBird_regist( bird, "hint_base_Fgp",  st_hint_base_Fgp );
		ZnkBird_regist( bird, "hint_base_Snp",  st_hint_base_Snp );
		ZnkBird_regist( bird, "user_agent", "" );
		ZnkBird_regist( bird, "pthb",       "" );
		ZnkBird_regist( bird, "pthc",       "" );
		ZnkBird_regist( bird, "pthd",       "" );
		ZnkBird_regist( bird, "ptua",       "" );
		ZnkBird_regist( bird, "scsz",       "" );
		ZnkBird_regist( bird, "pwd",        "" );
		ZnkBird_regist( bird, "flrv",       "" );
		ZnkBird_regist( bird, "flvv",       "" );
		ZnkBird_regist( bird, "Fgp_RE_key", "webkit" );

		/* URLから:clear前のタイミングで拾う */
		form_type = getFormType( cb_vars );
		ZnkVarpAry_clear( cb_vars );

		RanoCGIUtil_getPostedFormData( evar, cb_vars, mod, &htp_hdrs, pst_str, NULL, is_unescape_val );

		varp = ZnkVarpAry_find_byName_literal( cb_vars, "Fgp_RE_key", false );
		if( varp ){
			ZnkStr_set( RE_key, ZnkVar_cstr( varp ) );
		}
		CBFgpInfo_init_byCBVars( fgp_info, cb_vars );

		varp = ZnkVarpAry_find_byName_literal( cb_vars, "cb_category", false );
		if( varp ){
			ZnkStr_set( category, ZnkVar_cstr( varp ) );
		}

		varp = ZnkVarpAry_find_byName_literal( cb_vars, "cb_all_clear_cookie", false );
		if( varp ){
			all_cookie_clear = true;
		}

		varp = ZnkVarpAry_find_byName_literal( cb_vars, "Moai_AuthenticKey", false );
		if( varp && ZnkS_eq( st_moai_authentic_key, ZnkVar_cstr(varp) ) ){
			is_authenticated = true;
		}
		
		if( !is_authenticated ){
			ZnkStr_addf( msg, "Not authenticated action.\n" );
		}

		//ZnkStr_addf( msg, "Debug:form_type=[%d] cmd_type=[%d]<br>", form_type, cmd_type );
		switch( form_type ){
		case Form_e_Virtual:
			cb_status = CBCustomAutomatic_main( evar, cb_vars, ZnkStr_cstr(cb_src),
					bird, RE_key, fgp_info, &ua_info,
					main_vars, &ptua64, msg, category, lacking_var,
					info, is_authenticated, all_cookie_clear );
			template_html_file = "templates/automatic.html";
			break;
		case Form_e_Main:
			cb_status = CBCustomPostVars_main( evar, cb_vars, ZnkStr_cstr(cb_src),
					bird, RE_key, fgp_info, &ua_info,
					main_vars, &ptua64, msg, category, lacking_var,
					info, is_authenticated, all_cookie_clear );
			template_html_file = "templates/postvars.html";
			break;
		case Form_e_Cookie:
			cb_status = CBCustomCookie_main( cb_vars,
					bird, RE_key, fgp_info,
					main_vars, &ptua64,
					info, is_authenticated );
			template_html_file = "templates/cookie.html";
			break;
		case Form_e_Finger:
			cb_status = CBCustomFeature_main( evar, cb_vars, ZnkStr_cstr(cb_src),
					bird, RE_key, fgp_info, &ua_info,
					main_vars, &ptua64, msg, category, lacking_var,
					info, is_authenticated, all_cookie_clear );
			template_html_file = "templates/feature.html";
			break;
		case Form_e_Get:
		default:
			break;
		}

		{
			ZnkStr selui = ZnkStr_new( "" );
			getCategory_select( selui, "cb_category", ZnkStr_cstr(category) );
			ZnkBird_regist( bird, "Category_ui", ZnkStr_cstr(selui) );
			ZnkStr_delete( selui );
		}
		switch( cb_status ){
		case CBStatus_e_Sealed:
			ZnkBird_regist( bird, "cb_status", "<b>現在の確定値</b>" );
			break;
		case CBStatus_e_InputLacking:
		{
			char buf[ 1024 ];
			Znk_snprintf( buf, sizeof(buf),
					"<b><font color=\"#ff0000\">編集モード: %s の値が空です. 値を入力してください</font></b>",
					ZnkStr_cstr(lacking_var) );
			ZnkBird_regist( bird, "cb_status", buf );
			break;
		}
		case CBStatus_e_Editing:
		default:
			ZnkBird_regist( bird, "cb_status",
					"<b><font color=\"#7733ff\">編集モード: 必要な値が揃いました. Step2ボタンで確定してください</font></b>" );
			break;
		}

		ZnkBird_regist( bird, "Moai_AuthenticKey",  st_moai_authentic_key );
		ZnkBird_regist( bird, "msg",  ZnkStr_cstr(msg) );
		ZnkBird_regist( bird, "nego_target",  CBConfig_theNegotiatingTarget() );
		RanoCGIUtil_printTemplateHTML( evar, bird, template_html_file );

		ZnkStr_delete( pst_str );
		ZnkStr_delete( lacking_var );
		ZnkStr_delete( cb_src );
		ZnkStr_delete( RE_key );
		ZnkBird_destroy( bird );
	}

	ZnkVarpAry_destroy( cb_vars );
	ZnkVarpAry_destroy( main_vars );
	ZnkStr_delete( str );
	ZnkStr_delete( msg );
	ZnkStr_delete( category );
	CBFgpInfo_destroy( fgp_info );
}

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

int main( int argc, char** argv ) 
{
	int          exit_result = EXIT_FAILURE;
	RanoCGIEVar* evar        = RanoCGIEVar_create();
	size_t       count       = 0;
	const char*  moai_dir    = NULL;
	ZnkStr       ermsg       = ZnkStr_new( "" );
	ZnkMyf       custom_boy_myf = NULL;

	/* rano app setup */

	if( RanoConfUtil_rano_app_initiate( ".", true, ermsg ) ){
		static const bool is_multi_dir = false;
		count = RanoCGIUtil_rano_app_init_log( "custom_boy", is_multi_dir );
	} else {
		RanoCGIUtil_rano_app_print_error( ZnkStr_cstr(ermsg) );
		goto FUNC_END;
	}

	if( !ZnkZlib_initiate() ){
		RanoCGIUtil_rano_app_print_error( "ZnkZlib_initiate is failure.\n" );
		return EXIT_FAILURE;
	}
	if( !ZnkNetBase_initiate( false ) ){
		RanoCGIUtil_rano_app_print_error( "ZnkNetBase_initiate is failure.\n" );
		return EXIT_FAILURE;
	}

	moai_dir = RanoConfUtil_moai_dir( NULL );
	if( moai_dir == NULL ){
		RanoCGIUtil_rano_app_print_error( "moai_dir is not found." );
		goto FUNC_END;
	}

	/* Application setup */

	Znk_UNUSED( count );
	custom_boy_myf = CBConfig_custom_boy_myf();
	if( custom_boy_myf ){
		double RE_random_factor = 0.7;
		ZnkVarpAry vars = ZnkMyf_find_vars( custom_boy_myf, "config" );
		if( vars ){
			ZnkVarp var;

			var = ZnkVarpAry_find_byName_literal( vars, "RE_random_factor", false );
			if( var ){
				ZnkS_getReal( &RE_random_factor, ZnkVar_cstr(var) );
				CBVirtualizer_setRERandomFactor( RE_random_factor );
			}
		}
	} else {
		RanoCGIUtil_rano_app_print_error( "custom_boy.myf is not found." );
		goto FUNC_END;
	}

	{
		char path[ 256 ] = "";
		ZnkFile fp = NULL;
		Znk_snprintf( path, sizeof(path), "%sauthentic_key.dat", moai_dir );
		fp = Znk_fopen( path, "rb" );
		if( fp ){
			Znk_fgets( st_moai_authentic_key, sizeof(st_moai_authentic_key), fp );
			Znk_fclose( fp );
		}
	}

	/***
	 * filters_dir setting ( from moai_dir/config.myf ).
	 */
	{
		char filename[ 1024 ] = "";
		ZnkMyf config = ZnkMyf_create();
		Znk_snprintf( filename, sizeof(filename), "%sconfig.myf", moai_dir );
		if( ZnkMyf_load( config, filename ) ){
			ZnkVarpAry vars = ZnkMyf_find_vars( config, "config" );
			if( vars ){
				ZnkStr dir_str = ZnkStr_new( "" );
				ZnkVarp var;

				var = ZnkVarpAry_find_byName_literal( vars, "filters_dir", false );
				if( var ){
					setupConfigDir( dir_str, ZnkVar_cstr(var), moai_dir, "filters" );
					CBVirtualizer_setFiltersDir( ZnkStr_cstr(dir_str) );
				}

				ZnkStr_delete( dir_str );
			}
		}
		ZnkMyf_destroy( config );
	}

	parseQueryString( evar );

	exit_result = EXIT_SUCCESS;
FUNC_END:
	RanoCGIEVar_destroy( evar );
	CBConfig_custom_boy_myf_destroy();

	RanoConfUtil_rano_app_finalize();
	RanoConfUtil_moai_dir_finalize();
	ZnkStr_delete( ermsg );

	return exit_result;
}

