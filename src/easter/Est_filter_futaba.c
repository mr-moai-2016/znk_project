#include <Est_filter_futaba.h>
#include <Est_parser.h>
#include <Est_link.h>
#include <Est_config.h>
#include <Est_rpsc.h>

#include <Znk_str_ex.h>
#include <Znk_str_ptn.h>
#include <Znk_str_fio.h>
#include <Znk_mem_find.h>

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
filterSetStr( ZnkStr str, void* arg )
{
	const char* val = Znk_force_ptr_cast( char*, arg );
	ZnkStr_set( str, val );
	return 1;
}

static int
filterHeightZeroTrick( ZnkStr str, void* arg )
{
	if( ZnkStrEx_strstr( str, "iframe" ) || ZnkStrEx_strstr( str, "src=\"\"" ) ){
		ZnkStrPtn_invokeInQuote( str,
				"width:", "px",
				NULL, NULL,
				filterSetStr, "0", false );
		ZnkStrPtn_invokeInQuote( str,
				"height:", "px",
				NULL, NULL,
				filterSetStr, "0", false );
		ZnkStrPtn_invokeInQuote( str,
				"margin:", "px",
				NULL, NULL,
				filterSetStr, "0", false );
	}
	return 1;
}
static int
filterPaginationBtn( ZnkStr str, void* arg )
{
	ZnkStr href = ZnkStr_new( "" );
	ZnkStr name = ZnkStr_new( "" );
	const char* p = NULL;
	const char* q = NULL;

	p = ZnkStrEx_strchr( str, '"' );
	if( p == NULL ){
		goto FUNC_END;
	}
	q = Znk_strchr( p+1, '"' );
	if( q == NULL ){
		goto FUNC_END;
	}
	ZnkStr_assign( href, 0, p+1, q-p-1 ); 

	p = q+1;

	p = Znk_strchr( p, '"' );
	if( p == NULL ){
		goto FUNC_END;
	}
	q = Znk_strchr( p+1, '"' );
	if( q == NULL ){
		goto FUNC_END;
	}
	ZnkStr_assign( name, 0, p+1, q-p-1 ); 

	p = q+1;

	p = Znk_strstr( p, "accesskey=" );
	if( p ){
		p += Znk_strlen( "accesskey=" );
		if( *p == 'x' ){
			ZnkStr_setf( str, "<a class=\"pagination-next\" href=\"%s\" accesskey=%c>%s</a>\n", ZnkStr_cstr(href), *p, ZnkStr_cstr(name) );
		} else {
			ZnkStr_setf( str, "<a class=\"pagination-previous\" href=\"%s\" accesskey=%c>%s</a>\n", ZnkStr_cstr(href), *p, ZnkStr_cstr(name) );
		}
	} else {
		ZnkStr_setf( str, "<a class=\"pagination-next\" href=\"%s\">%s</a>\n", ZnkStr_cstr(href), ZnkStr_cstr(name) );
	}

FUNC_END:
	ZnkStr_delete( href );
	ZnkStr_delete( name );
	return 1;
}
static int
filterPagination( ZnkStr str, void* arg )
{
	ZnkSRef old_ptn = { 0 };
	ZnkSRef new_ptn = { 0 };
	{
		ZnkSRef_set_literal( &old_ptn, "<table align=left border=1 class=\"psen\"" );
		ZnkSRef_set_literal( &new_ptn, "<div class=\"pagination\"" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef_set_literal( &old_ptn, "</table>" );
		ZnkSRef_set_literal( &new_ptn, "</div>" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef_set_literal( &old_ptn, "<tr><td>" );
		ZnkSRef_set_literal( &new_ptn, "" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef_set_literal( &old_ptn, "</td><td>" );
		ZnkSRef_set_literal( &new_ptn, "" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef_set_literal( &old_ptn, "</td></tr>" );
		ZnkSRef_set_literal( &new_ptn, "" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef_set_literal( &old_ptn, "]&nbsp;[" );
		ZnkSRef_set_literal( &new_ptn, "][" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef_set_literal( &old_ptn, "]<br>[" );
		ZnkSRef_set_literal( &new_ptn, "][" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef_set_literal( &old_ptn, "][" );
		ZnkSRef_set_literal( &new_ptn, "\n" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef_set_literal( &old_ptn, "[<" );
		ZnkSRef_set_literal( &new_ptn, "<div class=\"pagination-list\"><" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef_set_literal( &old_ptn, ">]" );
		ZnkSRef_set_literal( &new_ptn, ">\n</div>" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef_set_literal( &old_ptn, "<a " );
		ZnkSRef_set_literal( &new_ptn, "<a class=\"pagination-link\" " );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef_set_literal( &old_ptn, "<b>" );
		ZnkSRef_set_literal( &new_ptn, "<span class=\"pagination-link is-current\">" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef_set_literal( &old_ptn, "</b>" );
		ZnkSRef_set_literal( &new_ptn, "</span>" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef_set_literal( &old_ptn, "最初のページ" );
		ZnkSRef_set_literal( &new_ptn, "<a class=\"pagination-previous\">最初のページ</a>" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef_set_literal( &old_ptn, "最後のページ" );
		ZnkSRef_set_literal( &new_ptn, "<a class=\"pagination-next\">最後のページ</a>" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}

	ZnkStrPtn_invokeInQuote( str,
			"<form action=", "</form>",
			NULL, NULL,
			filterPaginationBtn, "", true );
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
on_link( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend )
{
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );

	/* link hrefの値を取得 */
	ZnkVarp href = ZnkVarpAry_find_byName_literal( varp_ary, "href", true );
	if( href ){
		ZnkStr str = EstHtmlAttr_str( href );
		if( !link_info->css_link_done_ ){
			const char* xhr_auth_host = EstConfig_XhrAuthHost();
			ZnkVarp rel  = ZnkVarpAry_find_byName_literal( varp_ary, "rel",  true );
			ZnkVarp type = ZnkVarpAry_find_byName_literal( varp_ary, "type", true );
			char file_path[ 256 ] = "";
			Znk_snprintf( file_path, sizeof(file_path), "http://%s/msty.css", xhr_auth_host );
			if( rel && type 
			  && ZnkS_eq( EstHtmlAttr_val(rel),  "stylesheet" )
			  && ZnkS_eq( EstHtmlAttr_val(type), "text/css" )
			  && !ZnkS_eq( EstHtmlAttr_val(href), file_path ) )
			{
				if( ZnkStrEx_strstr( str, "/bin/style4.css" ) ){
					/* ふたばのcssは無効化する */
					ZnkStr_set( tagname, "!-- link" );
					ZnkStr_set( tagend, "-->\n" );
				} else {
					ZnkStr_set( tagend, " />\n" );
				}
				ZnkStr_addf( tagend, "<link href=\"http://%s/cgis/easter/publicbox/bbs_futaba/futaba.css\" rel=\"stylesheet\" type=\"text/css\" />\n",
						xhr_auth_host );
				link_info->css_link_done_ = true;
			}
		} else if( ZnkStrEx_strstr( str, "/bin/style4.css" ) ){
			/* ふたばのcssは無効化する(二つ目以降のstyle4.cssがあった場合も消しておく) */
			ZnkStr_set( tagname, "!-- link" );
			ZnkStr_set( tagend, "-->\n" );
		}
		link_info->est_req_ = EstRequest_e_get; /* 直接取得 */
		return EstLink_filterLink( str, link_info, EstLinkXhr_e_ImplicitDMZ );
	}
	return 0;
}

static int
on_other( ZnkStr text, void* arg, const char* landmarking )
{
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );
	const char* xhr_auth_host = EstConfig_XhrAuthHost();

	/* futaba : via Easter on XhrDMZ landmarking */
	{
		ZnkSRef old_ptn = { 0 };
		ZnkStr  new_ptn = ZnkStr_newf( "＠ふたば <font size=\"-1\" color=\"#808000\"><a href=http://%s/easter>%s</a></font></span>", xhr_auth_host, landmarking );
		ZnkSRef_set_literal( &old_ptn, "＠ふたば</span>" );
		ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, ZnkStr_cstr(new_ptn), ZnkStr_leng(new_ptn), Znk_NPOS, Znk_NPOS ); 
		ZnkStr_delete( new_ptn );
	}

	/* Rpscスクリプトの実行 */
	{
		RanoTxtFilterAry ary = theRpsc();
		EstRpsc_exec( ary, text, link_info );
	}

	/* futaba : 除去オブジェクトを覆う div が無駄な空間を開ける場合、そのstyleの幅と高さを0にする */
	ZnkStrPtn_invokeInQuote( text,
			"<div ", "</div>",
			NULL, NULL,
			filterHeightZeroTrick, link_info, true );

	/* futaba : スレ立て用画面の最下にあるpagination */
	ZnkStrPtn_invokeInQuote( text,
			"<table align=left border=1 class=\"psen\">", "</td></tr></table><br clear=all>",
			NULL, NULL,
			filterPagination, link_info, true );
	return 0;
}

static int
on_plane_text( ZnkStr planetxt, void* arg )
{
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );
	if( EstConfig_isAutoLink() && ZnkStrEx_strstr( link_info->unesc_src_, "mode=cat" ) == NULL ){
		const bool is_append_a_tag = true;
		EstFilter_replaceToAutolink( planetxt, is_append_a_tag );
	}
	return 1;
}

static ZnkStr
makeText_fromFile( const char* filename )
{
	ZnkStr  text = NULL;
	ZnkFile fp   = Znk_fopen( filename, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		text = ZnkStr_new( "" );
		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				break;
			}
			ZnkStr_add( text, ZnkStr_cstr(line) );
		}
		ZnkStr_delete( line );
		Znk_fclose( fp );
	}
	return text;
}
static int
on_bbs_operation( ZnkStr txt, const char* result_filename, const char* src, ZnkStr console_msg )
{
	ZnkStr  require_js = makeText_fromFile( "publicbox/bbs_futaba/require_js.html" );
	ZnkSRef landmark = { 0 };

	if( require_js == NULL ){
		ZnkMyf myf = ZnkMyf_create();
		if( ZnkMyf_load( myf, "publicbox/bbs_futaba/require_js.myf" ) ){
			size_t numof_sec = ZnkMyf_numOfSection( myf );
			size_t sec_idx;
			ZnkMyfSection sec;
			ZnkVarpAry    vars;
			ZnkVarp       var;
			ZnkStr        pattern;
			size_t        occ_tbl[256];
			for( sec_idx=0; sec_idx<numof_sec; ++sec_idx ){
				sec     = ZnkMyf_atSection( myf, sec_idx );
				vars    = ZnkMyfSection_vars( sec );
				var     = ZnkVarpAry_findObj_byName_literal( vars, "pattern", false );
				pattern = ZnkVar_str( var );
				ZnkMem_getLOccTable_forBMS( occ_tbl, (uint8_t*)ZnkStr_cstr(pattern), ZnkStr_leng(pattern) );
				if( ZnkStr_empty(pattern) ||
						ZnkMem_lfind_data_BMS(
							(uint8_t*)ZnkStr_cstr(txt),     ZnkStr_leng(txt),
							(uint8_t*)ZnkStr_cstr(pattern), ZnkStr_leng(pattern), 1, occ_tbl ) != Znk_NPOS )
				{
					var  = ZnkVarpAry_findObj_byName_literal( vars, "str", false );
					require_js = ZnkStr_new( ZnkVar_cstr( var ) );
					break;
				}
			}
		}
		ZnkMyf_destroy( myf );
	}

	/* for スレ立て画面、画像掲示板一般 */
	ZnkSRef_set_literal( &landmark, "</div><!--スレッド終了-->" );
	if( EstFilter_insertBBSOperation( txt,
			result_filename, landmark.cstr_, src, "resto", console_msg, require_js ) ){
		goto FUNC_END;
	}

	/* for トップページ */
	ZnkSRef_set_literal( &landmark,  "<!-- top banner end-->" );
	if( EstFilter_insertBBSOperation( txt,
			result_filename, landmark.cstr_, src, "resto", console_msg, require_js ) ){
		goto FUNC_END;
	}

	/* その他 */
	ZnkSRef_set_literal( &landmark,  "<!-- GazouBBS v3.0 -->" );
	if( EstFilter_insertBBSOperation( txt,
			result_filename, landmark.cstr_, src, "resto", console_msg, require_js ) ){
		goto FUNC_END;
	}

FUNC_END:
	ZnkStr_delete( require_js );
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
		st_module_instance.on_link_   = on_link;
		st_module_instance.on_other_         = on_other;
		st_module_instance.on_plane_text_    = on_plane_text;
		st_module_instance.on_bbs_operation_ = on_bbs_operation;
		st_module = &st_module_instance;
	}
	return st_module;
}
