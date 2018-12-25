#include <Est_filter_5ch.h>
#include <Est_parser.h>
#include <Est_link.h>
#include <Est_config.h>
#include <Est_rpsc.h>

#include <Znk_str_ex.h>
#include <Znk_str_ptn.h>

static RanoTxtFilterAry
theRpsc( void )
{
	static RanoTxtFilterAry st_rplc_ary = NULL;
	if( st_rplc_ary == NULL ){
		ZnkStrAry cmds = EstConfig_getCoreBehaviorLines( "filter_target_5ch" );
		st_rplc_ary = RanoTxtFilterAry_create();
		EstRpsc_compile( st_rplc_ary, cmds );
	}
	return st_rplc_ary;
}

static int
on_a( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend, bool end_kind )
{
	if( !end_kind ){
		struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );
		/* a hrefの値を取得 */
		ZnkVarp attr = ZnkVarpAry_find_byName_literal( varp_ary, "href", true );
	
		{
			ZnkVarpAry onclick_jscall = EstConfig_filterOnclickJSCall();
			if( EstLink_filterOnclickJSCall( varp_ary, onclick_jscall ) ){
				return 0;
			}
		}
	
		if( attr ){
			ZnkStr str = EstHtmlAttr_str( attr );
	
			/* for jump.5ch.net */
			char* p = ZnkStrEx_strstr( str, "http://jump.5ch.net/?" );
			if( p ){
				const char* begin = ZnkStr_cstr( str );
				ZnkStr_erase( str, p-begin, Znk_strlen_literal("http://jump.5ch.net/?") );
				ZnkStr_set( tagend, " target=_blank >" );
			}
	
			link_info->est_req_ = EstRequest_e_view;
			return EstLink_filterLink( str, link_info, EstLinkXhr_e_ImplicitDMZ );
		}
	}
	return 0;
}

static int
on_script( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend, bool end_kind )
{
	if( !end_kind ){
		struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );
		ZnkVarp attr = NULL;
		
		/* script srcの値を取得 */
		attr = ZnkVarpAry_find_byName_literal( varp_ary, "src", true );
		if( attr ){
			ZnkStr str = EstHtmlAttr_str( attr );
			if( EstFilter_ignoreHosts( str ) ){
				ZnkStr_clear( str );
			} else {
				link_info->est_req_ = EstRequest_e_get; /* 直接取得 */
				return EstLink_filterScript( str, link_info );
			}
		}
	}
	return 0;
}

static int
on_form( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend, bool end_kind )
{
	if( !end_kind ){
		struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );
		/* form actionの値を取得 */
		ZnkVarp attr = ZnkVarpAry_find_byName_literal( varp_ary, "action", true );
		if( attr ){
			ZnkStr str = EstHtmlAttr_str( attr );
			link_info->est_req_ = EstRequest_e_post;
			return EstLink_filterLink( str, link_info, EstLinkXhr_e_ImplicitDMZ );
		}
	}
	return 0;
}

static int
landmarkEaster( ZnkStr str, void* arg )
{
	ZnkStr_add( str, " <font size=\"-1\" color=\"#808000\">via Easter on XhrDMZ</font>" );
	return 1;
}
static int
filter5chBBSTable( ZnkStr str, void* arg )
{
	ZnkStr_insert( str, 0, "<br> <br>", Znk_NPOS );
	ZnkStr_add( str, "<br>" );
	return 1;
}

static int
on_other( ZnkStr text, void* arg, const char* landmarking )
{
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );

	/* 5ch : via Easter on XhrDMZ landmarking */
	ZnkStrPtn_invokeInQuote( text,
			"<h1 class=\"title\">", "\n",
			NULL, NULL,
			landmarkEaster, link_info, false );

	/* 5ch smart phone : via Easter on XhrDMZ landmarking */
	{
		ZnkSRef old_ptn = { 0 };
		ZnkStr  new_ptn = ZnkStr_newf( "<span id=\"title\" class=\"threadview_response_title\"></span> "
				"<font size=\"-1\" color=\"#808000\">%s</font>", landmarking );
		ZnkSRef_set_literal( &old_ptn, "<div id=\"title\" class=\"threadview_response_title\"></div>" );
		ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, ZnkStr_cstr(new_ptn), ZnkStr_leng(new_ptn), Znk_NPOS, Znk_NPOS ); 
		ZnkStr_delete( new_ptn );
	}

	/* 5ch : bbstable.html */
	ZnkStrPtn_invokeInQuote( text,
			"【<B>", "</B>】",
			NULL, NULL,
			filter5chBBSTable, link_info, true );

	/* Rpscスクリプトの実行 */
	{
		RanoTxtFilterAry ary = theRpsc();
		EstRpsc_exec( ary, text, link_info );
	}

	return 0;
}

static int
on_plane_text( ZnkStr planetxt, void* arg )
{
	if( EstConfig_isAutoLink() ){
		EstFilter_replaceToAutolink( planetxt );
	}
	return 1;
}
static int
on_bbs_operation( ZnkStr txt, const char* result_filename, const char* src, ZnkStr console_msg )
{
	ZnkSRef landmark = { 0 };
	ZnkSRef_set_literal( &landmark, "<span class=\"newpostbutton\">" );
	EstFilter_insertBBSOperation( txt,
			result_filename, landmark.cstr_, src, "key", console_msg );
	return 0;
}

EstFilterModule*
EstFilter_5ch_getModule( void )
{
	static EstFilterModule* st_module = NULL;
	if( st_module == NULL ){
		static EstFilterModule st_module_instance = { 0 };
		st_module_instance.on_a_      = on_a;
		st_module_instance.on_script_ = on_script;
		st_module_instance.on_form_   = on_form;
		st_module_instance.on_other_         = on_other;
		st_module_instance.on_plane_text_    = on_plane_text;
		st_module_instance.on_bbs_operation_ = on_bbs_operation;
		st_module = &st_module_instance;
	}
	return st_module;
}
