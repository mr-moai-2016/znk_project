#include <Est_filter_futaba.h>
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
		ZnkStrAry cmds = EstConfig_getCoreBehaviorLines( "filter_target_futaba" );
		st_rplc_ary = RanoTxtFilterAry_create();
		EstRpsc_compile( st_rplc_ary, cmds );
	}
	return st_rplc_ary;
}

static int
filterHeightZeroTrick( ZnkStr str, void* arg )
{
	if( ZnkStrEx_strstr( str, "iframe" ) || ZnkStrEx_strstr( str, "src=\"\"" ) ){
		ZnkSRef old_ptn = { 0 };
		ZnkSRef new_ptn = { 0 };
		ZnkSRef_set_literal( &old_ptn, "height:" );
		ZnkSRef_set_literal( &new_ptn, "height:0px;width:" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	return 1;
}


static int
addOpenHyperpostButtonEx( ZnkStr str, const char* unesc_src, const char* dst_thno )
{
	if( ZnkS_eq( dst_thno, "0" ) ){
		ZnkStr_addf( str, "<br><a style='margin-top: 10px' class=MstyElemLink "
				"onClick=\"document.Easter_fm_main.action = Easter_hyperpost_url; document.Easter_fm_main.submit(); \" "
				"target=_blank>Easter HyperPostでスレッドを立てる</a>" );
	} else {
		ZnkStr_addf( str, "<br><a style='margin-top: 10px' class=MstyElemLink "
				"onClick=\"document.Easter_fm_main.action = Easter_hyperpost_url + '&est_original_url=%s'; document.Easter_fm_main.submit(); \" "
				"target=_blank>Easter HyperPostで返信する</a>",
				unesc_src );
	}
	return 1;
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
	
		/* for futaba */
		attr = ZnkVarpAry_find_byName_literal( varp_ary, "onload", true );
		if( attr ){
			if( ZnkS_eq( EstHtmlAttr_val(attr), "new microadCompass.AdInitializer().initialize();" ) ){
				EstHtmlAttr_val_clear( attr );
			}
		}
		
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
			if( ZnkStrEx_strstr( str, "futaba.php" ) ){
				ZnkVarp enctype = ZnkVarpAry_find_byName_literal( varp_ary, "enctype", true );
				if( enctype && ZnkS_eq( ZnkVar_cstr(enctype), "multipart/form-data" ) ){
					link_info->est_req_ = EstRequest_e_post_ex;
					ZnkStr_set( link_info->hyperpost_url_, ZnkStr_cstr(str) ); 
					EstLink_filterLink( link_info->hyperpost_url_, link_info, EstLinkXhr_e_ExplicitAuth );
					link_info->est_req_ = EstRequest_e_post;
					{
						/***
						 * may 等ではメイン投稿formにおいて id="fm" などが指定されている.
						 * しかしidが指定されていない板も存在する.
						 * そのような板に関しては、とりあえず id が指定されていないmultipart/form-dataのformをメイン投稿formとみなす.
						 */
						ZnkVarp id = ZnkVarpAry_find_byName_literal( varp_ary, "id", true );
						if( id == NULL || ( id && ZnkS_eq( ZnkVar_cstr(id), "fm" ) ) ){
							ZnkVarp name = ZnkVarpAry_find_byName_literal( varp_ary, "name", true );
							if( name == NULL ){
								name = ZnkVarp_create( "name", "", 0, ZnkPrim_e_Str, NULL );
								EstHtmlAttr_set_quote_char( name, '"' );
								ZnkVarpAry_push_bk( varp_ary, name );
							}
							ZnkVar_set_val_Str( name, "Easter_fm_main", Znk_NPOS );
						}
					}
				}
			}
			return EstLink_filterLink( str, link_info, EstLinkXhr_e_ImplicitDMZ );
		}
	}
	return 0;
}

static int
on_input( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend )
{
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );
	/* ptfkを呼び出すinputタグを検出 */
	ZnkVarp attr = ZnkVarpAry_find_byName_literal( varp_ary, "onClick", true );
	if( attr ){
		static const bool old_compatible_mode = false;
		const char* val = EstHtmlAttr_val( attr );
		const char* p   = Znk_strstr( val, "ptfk" );
		if( p ){
			p = Znk_strchr( p, '(' );
			if( p ){
				++p;
				while( *p == ' ' ){ ++p; }
				if( *p ){
					const char* q = p;
					while( *q && *q != ' ' && *q != ')' ){ ++q; }
					if( *q ){
						char thno[ 256 ] = "";
						ZnkS_copy( thno, sizeof(thno), p, q-p );
						addOpenHyperpostButtonEx( tagend, ZnkStr_cstr(link_info->unesc_src_), thno );
					}
				}
			}
			/* onClick 内の return を削れば従来通りの挙動となるようである */
			if( old_compatible_mode ){
				ZnkStr onClick = EstHtmlAttr_str( attr );
				ZnkStrEx_chompFirstStr( onClick, "return", 6 );
			}
		}
	}
	return 0;
}

static int
replaceEstViewToGet( ZnkStr str, void* arg )
{
	ZnkSRef old_ptn = { 0 };
	ZnkSRef new_ptn = { 0 };
	ZnkSRef_set_literal( &old_ptn, "est_view=" );
	ZnkSRef_set_literal( &new_ptn, "est_get=" );
	ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	return 1;
}
static int
landmarkEaster( ZnkStr str, void* arg )
{
	ZnkStr_add( str, " <font size=\"-1\" color=\"#808000\">via Easter on XhrDMZ</font>" );
	return 1;
}

static int
on_other( ZnkStr text, void* arg, const char* landmarking )
{
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );

	ZnkStrPtn_invokeInQuote( text,
			".webm\" target='_blank'>", ".webm\" target='_blank'><img src=",
			NULL, NULL,
			replaceEstViewToGet, link_info, true );

	/* futaba : via Easter on XhrDMZ landmarking */
	{
		ZnkSRef old_ptn = { 0 };
		ZnkStr  new_ptn = ZnkStr_newf( "＠ふたば <font size=\"-1\" color=\"#808000\">%s</font></span>", landmarking );
		ZnkSRef_set_literal( &old_ptn, "＠ふたば</span>" );
		ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, ZnkStr_cstr(new_ptn), ZnkStr_leng(new_ptn), Znk_NPOS, Znk_NPOS ); 
		ZnkStr_delete( new_ptn );
	}

	/* futaba : via Easter on XhrDMZ landmarking(for i-mode) */
	ZnkStrPtn_invokeInQuote( text,
			"data-iconpos=\"left\">TOP</a> <h1>", "</h1>",
			NULL, NULL,
			landmarkEaster, link_info, false );
	ZnkStrPtn_invokeInQuote( text,
			"data-iconpos=\"left\">戻る</a> <h1>", "</h1>",
			NULL, NULL,
			landmarkEaster, link_info, false );

	/* Rpscスクリプトの実行 */
	{
		RanoTxtFilterAry ary = theRpsc();
		EstRpsc_exec( ary, text, link_info );
	}

	/* futaba : 除去オブジェクトを覆う div が無駄な空間を開ける場合、そのstyleの高さを0にする */
	ZnkStrPtn_invokeInQuote( text,
			"<div ", "</div>",
			NULL, NULL,
			filterHeightZeroTrick, link_info, true );
	return 0;
}

static int
on_plane_text( ZnkStr planetxt, void* arg )
{
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );
	if( EstConfig_isAutoLink() && ZnkStrEx_strstr( link_info->unesc_src_, "mode=cat" ) == NULL ){
		EstFilter_replaceToAutolink( planetxt );
	}
	return 1;
}

static int
on_bbs_operation( ZnkStr txt, const char* result_filename, const char* src, ZnkStr console_msg )
{
	ZnkSRef landmark = { 0 };
	ZnkSRef_set_literal( &landmark, "</div><!--スレッド終了-->" );
	EstFilter_insertBBSOperation( txt,
			result_filename, landmark.cstr_, src, "resto", console_msg );
	return 0;
}

EstFilterModule*
EstFilter_futaba_getModule( void )
{
	static EstFilterModule* st_module = NULL;
	if( st_module == NULL ){
		static EstFilterModule st_module_instance = { 0 };
		st_module_instance.on_a_      = on_a;
		st_module_instance.on_script_ = on_script;
		st_module_instance.on_form_   = on_form;
		st_module_instance.on_input_  = on_input;
		st_module_instance.on_other_         = on_other;
		st_module_instance.on_plane_text_    = on_plane_text;
		st_module_instance.on_bbs_operation_ = on_bbs_operation;
		st_module = &st_module_instance;
	}
	return st_module;
}
