#include <Est_filter.h>
#include <Est_link.h>
#include <Est_config.h>
#include <Est_parser.h>
#include <Est_base.h>
#include <Est_hint_manager.h>
#include <Est_filter_default.h>
#include <Est_filter_futaba.h>
#include <Est_filter_5ch.h>

#include <Rano_cgi_util.h>
#include <Rano_log.h>

#include <Znk_htp_util.h>
#include <Znk_str_fio.h>
#include <Znk_str_ex.h>
#include <Znk_str_ptn.h>
#include <Znk_mem_find.h>

#include <string.h>
#include <ctype.h>

static void
preloadTextAndInfos( ZnkStr text, const char* src, struct EstLinkInfo* link_info, ZnkFile fp, bool save_img_cache, bool is_https )
{
	ZnkStr line = ZnkStr_new( "" );

	link_info->unesc_src_ = ZnkStr_new( "" );
	ZnkHtpURL_unescape_toStr( link_info->unesc_src_, src, Znk_strlen(src) );

	{
		const char* s = ZnkStr_cstr( link_info->unesc_src_ );
		const char* q = NULL;
		s = RanoCGIUtil_skipProtocolPrefix( s );
		q = strchr( s, '/' );
		if( q ){
			link_info->hostname_ = ZnkStr_create( s, q-s );
		} else {
			link_info->hostname_ = ZnkStr_new( s );
		}
		{
			const char* target = EstConfig_findTargetName( ZnkStr_cstr(link_info->hostname_) );
			if( ZnkS_eq( target, "futaba" ) ){
				link_info->filter_module_ = EstFilter_futaba_getModule();
			} else if( ZnkS_eq( target, "5ch" ) ){
				link_info->filter_module_ = EstFilter_5ch_getModule();
			} else {
				link_info->filter_module_ = EstFilter_default_getModule();
			}
		}
	}
	link_info->hyperpost_url_ = ZnkStr_new( "" );

	{
		const char* s = ZnkStr_cstr( link_info->unesc_src_ );
		const char* p = NULL;
		const char* q = NULL;
		s = RanoCGIUtil_skipProtocolPrefix( s );
		q = strchr( s, '?' );
		if( q ){
			p = Znk_memrchr( (uint8_t*)s, '/', q-s );
		} else {
			p = strrchr( s, '/' );
		}
		if( p ){
			link_info->base_href_ = ZnkStr_create( s, p+1-s );
		} else {
			link_info->base_href_ = ZnkStr_new( "" );
		}
	}

	link_info->est_req_   = EstRequest_e_get;
	link_info->mtgt_      = EstConfig_target_myf();
	link_info->img_link_direct_ = EstConfig_isDirectImgLink();
	link_info->css_link_done_ = false;
	if( save_img_cache ){
		link_info->img_link_direct_ = false;
	}
	link_info->is_https_parent_ = is_https;

	while( true ){
		if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			break;
		}
		ZnkStr_add( text, ZnkStr_cstr(line) );
	}

	ZnkStr_delete( line );
}


bool
EstFilter_ignoreHosts( ZnkStr str )
{
	/* ignore_hosts */
	const ZnkMyf hosts_myf = EstConfig_hosts_myf();
	ZnkStrAry ignore_hosts = hosts_myf ? ZnkMyf_find_lines( hosts_myf, "ignore_hosts" ) : NULL;
	char hostname[ 256 ] = "";
	ZnkStr req_urp = ZnkStr_new( "" );
	EstBase_getHostnameAndRequrp_fromEstVal( hostname, sizeof(hostname), req_urp, ZnkStr_cstr(str), NULL );
	ZnkStr_delete( req_urp );
	if( ZnkStrAry_find_isMatch( ignore_hosts, 0, hostname, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
		ZnkStr_clear( str );
		return true;
	}
	return false;
}

#define GET_ON_KIND( tag_kind ) ( filter_module && filter_module->on_##tag_kind##_ ) ? \
	filter_module->on_##tag_kind##_ : filter_default->on_##tag_kind##_

static int
filterHtmlTags( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend )
{
	int result = 0;
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );
	const EstFilterModule* filter_module  = link_info->filter_module_;
	const EstFilterModule* filter_default = EstFilter_default_getModule();
	const bool  end_kind = (bool)( ZnkStr_first(tagname) == '/' );
	const char* tag_kind = end_kind ? ZnkStr_cstr(tagname) + 1 : ZnkStr_cstr(tagname);
	EstFilterTagKind1Func_T on_kind1 = NULL;
	EstFilterTagKind2Func_T on_kind2 = NULL;

	if( ZnkS_eqCase( tag_kind, "base" ) ){
		on_kind1 = GET_ON_KIND( base );

	} else if( ZnkS_eqCase( tag_kind, "frame" ) ){
		on_kind2 = GET_ON_KIND(        frame );

	} else if( ZnkS_eqCase( tag_kind, "iframe" ) ){
		on_kind2 = GET_ON_KIND(        iframe );

	} else if( ZnkS_eqCase( tag_kind, "a" ) ){
		on_kind2 = GET_ON_KIND(        a );

	} else if( ZnkS_eqCase( tag_kind, "img" ) ){
		on_kind1 = GET_ON_KIND(        img );

	} else if( ZnkS_eqCase( tag_kind, "link" ) ){
		on_kind1 = GET_ON_KIND(        link );

	} else if( ZnkS_eqCase( tag_kind, "script" ) ){
		on_kind2 = GET_ON_KIND(        script );

	} else if( ZnkS_eqCase( tag_kind, "form" ) ){
		on_kind2 = GET_ON_KIND(        form );

	} else if( ZnkS_eqCase( tag_kind, "meta" ) ){
		on_kind1 = GET_ON_KIND(        meta );

	} else if( ZnkS_eqCase( tag_kind, "div" ) ){
		on_kind2 = GET_ON_KIND(        div );

	} else if( ZnkS_eqCase( tag_kind, "input" ) ){
		on_kind1 = GET_ON_KIND(        input );

	}

	if( on_kind1 ){
		result = on_kind1( tagname, varp_ary, arg, tagend );
	} else if( on_kind2 ){
		result = on_kind2( tagname, varp_ary, arg, tagend, end_kind );
	}

	//return 0;
	return result;
}

static bool
isMajorURLChar( uint8_t ch )
{
	switch( ch ){
	case ':': case '/': case '?': case '#':
	case '[': case ']': case '@':
	case '!': case '$': case '&':
	case '(': case ')': case '+':
	case ',': case ';': case '=':
	case '-': case '.': case '_': case '~':
	case '%':
		return true;
	default:
		break;
	}
	return (bool)( isalnum( ch ) != 0 );
}

static size_t
replaceByScheme( ZnkStr planetxt, const char* scheme, size_t scheme_leng, bool is_append_a_tag )
{
	const char* p = NULL;
	const char* h = NULL;
	char hostname[ 256 ] = "";
	const char* target = NULL;
	size_t replace_count = 0;
	ZnkStr new_str = ZnkStr_new( "" );
	ZnkStr new_url = ZnkStr_new( "" );
	p = ZnkStr_cstr( planetxt );

	while( *p ){
		if( *p == '<' ){
			/* skip when planetxt contains other html tags */
			++p;
			while( *p ){
				if( *p == '>' ){
					++p;
					break;
				}
				++p;
			}
		} else if( ZnkS_isBegin( p, scheme ) ){
			size_t pos  = 0;
			size_t leng = 0;
			const char* q = p;
			h = Znk_strchr( p + scheme_leng, '/' );
			if( h ){
				ZnkS_copy( hostname, sizeof(hostname), p + scheme_leng, h - p - scheme_leng );
			}
			target = EstBase_findTargetName( EstConfig_target_myf(), hostname );
			while( true ){
				if( !isMajorURLChar( *q ) ){
					break;
				}
				++q;
			}
			pos  = p - ZnkStr_cstr( planetxt );
			leng = q - p;
		
			if( target ){
				ZnkStr_set( new_url, "/easter?est_view=" );
				ZnkStr_append( new_url, p, leng );
			} else {
				ZnkStr_assign( new_url, 0, p, leng );
			}

			ZnkStr_clear( new_str );
			if( is_append_a_tag ){
				ZnkStr_add( new_str, "<a class=MstyOpenLink href=\"" );
				ZnkStr_add( new_str, ZnkStr_cstr(new_url) );
				ZnkStr_add( new_str, "\" target=_blank>" );
				ZnkStr_add( new_str, "<font color=\"#404000\"> Open </font>" );
				ZnkStr_add( new_str, "</a>" );
			}
			if( target ){
				ZnkStr_addf( new_str, "<!-- a --><span class=MstyAutoLinkTarget>%s</span>", ZnkStr_cstr(new_url) );
			} else {
				ZnkStr_addf( new_str, "<span class=MstyAutoLinkOther>%s</span>", ZnkStr_cstr(new_url) );
			}
		
			ZnkStr_replace( planetxt, pos, leng, ZnkStr_cstr(new_str), ZnkStr_leng(new_str) );
			p = ZnkStr_cstr( planetxt ) + pos + ZnkStr_leng(new_str);
			++replace_count;
		} else {
			++p;
		}
	}

	ZnkStr_delete( new_str );
	ZnkStr_delete( new_url );
	return replace_count;
}

size_t
EstFilter_replaceToAutolink( ZnkStr planetxt, bool is_append_a_tag )
{
	size_t replace_count = 0;

	/* 一旦すべてttp系へ変換 */
	{
		ZnkSRef old_ptn = { 0 };
		ZnkSRef new_ptn = { 0 };
		ZnkSRef_set_literal( &old_ptn, "http://" );
		ZnkSRef_set_literal( &new_ptn, "ttp://" );
		ZnkStrEx_replace_BF( planetxt, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef old_ptn = { 0 };
		ZnkSRef new_ptn = { 0 };
		ZnkSRef_set_literal( &old_ptn, "https://" );
		ZnkSRef_set_literal( &new_ptn, "ttps://" );
		ZnkStrEx_replace_BF( planetxt, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}

	/* ttp系で処理 */
	{
		ZnkSRef scheme = { 0 };
		ZnkSRef_set_literal( &scheme, "ttp://" );
		replace_count += replaceByScheme( planetxt, scheme.cstr_, scheme.leng_, is_append_a_tag );
	}
	{
		ZnkSRef scheme = { 0 };
		ZnkSRef_set_literal( &scheme, "ttps://" );
		replace_count += replaceByScheme( planetxt, scheme.cstr_, scheme.leng_, is_append_a_tag );
	}

	/* すべてhttp系へ変換 */
	{
		ZnkSRef old_ptn = { 0 };
		ZnkSRef new_ptn = { 0 };
		ZnkSRef_set_literal( &old_ptn, "ttp://" );
		ZnkSRef_set_literal( &new_ptn, "http://" );
		ZnkStrEx_replace_BF( planetxt, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkSRef old_ptn = { 0 };
		ZnkSRef new_ptn = { 0 };
		ZnkSRef_set_literal( &old_ptn, "ttps://" );
		ZnkSRef_set_literal( &new_ptn, "https://" );
		ZnkStrEx_replace_BF( planetxt, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}

	return replace_count;
}

static int
filterPlaneTxt( ZnkStr planetxt, void* arg )
{
	int result = 1;
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );
	if( link_info && link_info->filter_module_ && link_info->filter_module_->on_plane_text_ ){
		result = link_info->filter_module_->on_plane_text_( planetxt, arg );
	}
	return result;
}
static int
filterPreModify( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend, ZnkStr text, size_t cur )
{
	int result = 1;
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );
	const char* p = ZnkStr_cstr(text) + cur;
	if( ZnkS_eqCase( ZnkStr_cstr(tagname), "a" ) ){
		const bool is_append_a_tag = false;
		const char* end_ptn = ZnkStr_first(tagname) == 'a' ? "</a>" : "<A/>";
		const char* q = Znk_strstr( p, end_ptn );
		if( q > p ){
			size_t replace_count = 0;
			ZnkStr wk_str  = ZnkStr_new( "" );
			ZnkStr_assign( wk_str, 0, p, q-p ); 
			replace_count = EstFilter_replaceToAutolink( wk_str, is_append_a_tag );
			if( replace_count ){
				ZnkStr_replace( text, cur, q-p, ZnkStr_cstr(wk_str), ZnkStr_leng(wk_str) );
			}
			ZnkStr_delete( wk_str );
		}
	}
	return result;
}

static int
changeHEAD2head( ZnkStr str, void* arg )
{
	if( ZnkStrEx_chompFirstStr( str, "<HEAD>", Znk_NPOS ) ){
		ZnkStr_insert( str, 0, "<head>", Znk_NPOS );
	}
	if( ZnkStrEx_chompLastStr( str, "</HEAD>", Znk_NPOS ) ){
		ZnkStr_add( str, "</head>" );
	}
	return 1;
}

static int
addVarEaster_hostname( ZnkStr str, void* arg )
{
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );
	{
		ZnkStr js = ZnkStr_new ( "" );
		ZnkStr_addf( js, "<script type='text/javascript'><!--\n"
				"var Easter_hostname=\"%s\";"
				"var Easter_hyperpost_url=\"%s\";"
				"//--></script>\n",
				ZnkStr_cstr(link_info->hostname_),
				ZnkStr_cstr(link_info->hyperpost_url_) );

		{
			const char* xhr_auth_host = EstConfig_XhrAuthHost();
			ZnkStr_addf( js, "<link href=\"http://%s/msty.css\" rel=\"stylesheet\" type=\"text/css\" />\n", xhr_auth_host );
			ZnkStr_addf( js, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n" );
			ZnkStr_addf( js, "<link href=\"http://%s/bulma.css\" rel=\"stylesheet\" type=\"text/css\" />\n", xhr_auth_host );
		}

		ZnkStr_insert( str, 0, ZnkStr_cstr(js), ZnkStr_leng(js) );
		ZnkStr_delete( js );
	}
	return 1;
}

bool
EstFilter_insertBBSOperation( ZnkStr text, 
		const char* result_filename, const char* landmark, const char* src, const char* bbs_id_name, ZnkStr console_msg )
{
	bool result = false;
	const char* xhr_auth_host = EstConfig_XhrAuthHost();
	ZnkStr dst = ZnkStr_new( "" );
	ZnkStr hint_table = EstHint_getHintTable( "bbs_operation" );

	ZnkStr_setf( dst, "%s\n", landmark );
	ZnkStr_add( dst, "<a name='EasterThreadOperation'></a>" );
	ZnkStr_add( dst, "<br><article class=\"message is-dark\">\n" );
	ZnkStr_add( dst, "<div class=\"message-header\">Easter BBS Operation</div><div class=\"message-body\">\n" );

	ZnkStr_addf( dst, "<script type=\"text/javascript\" src=\"http://%s/cgis/easter/publicbox/easter.js\"></script>\n",
			xhr_auth_host );
	ZnkStr_addf( dst, "<script type=\"text/javascript\"><!--\n" );
	ZnkStr_addf( dst, "var st_func_tbls = [];\n" );
	ZnkStr_addf( dst, "%s\n", ZnkStr_cstr( hint_table ) );
	ZnkStr_addf( dst, "//--></script>\n" );
	ZnkStr_addf( dst, "<script type=\"text/javascript\" src=\"http://%s/cgis/easter/publicbox/hint.js\"></script>\n",
			xhr_auth_host );

	/***
	 * XMLHttpRequestは、セキュリティーの関係からそのページと同じドメインからしかドキュメントを
	 * 取得できない(Same Origin Policy と呼ぶ）.
	 * 別ドメインの URL を指定しても、リクエストは送信されず、statusとして常に 0 が返る.
	 * ローカルにある HTML ファイルからはいかなるドメインにもリクエストを投げられない.
	 * 但し、Opera Widgets ならこの制限がかからないらしい.
	 *
	 * とりあえずここでは、この部分が既に est_get= に置換されているはずなので
	 * これをさらに est_reload=に置換しておき、処理はすべてest_reloadに委託する形にしておく.
	 * またonClickで何も行わないようにしておく.
	 */
	ZnkStr_addf( dst,
			"<a class=MstyQandALink href=\"javascript:void(0);\" onclick=\"displayQandA( 'Hint', 'aboutBBSOperation' );\">"
			"<img src='/cgis/easter/publicbox/icons/question_16.png'></a><br>\n" );
	ZnkStr_addf( dst, "<font size=-1><div id=startLink></div></font>\n" );
	ZnkStr_addf( dst, "<font size=-1><div id=aboutBBSOperation></div></font>\n" );
	ZnkStr_addf( dst, "<a class=MstyElemLink href=\"/easter?est_reload=%s\" onClick=\"Easter_reload('%s', '%s'); return false;\" >リロード</a>",
			src, src, bbs_id_name );

	ZnkStr_add( dst, "&nbsp;" );

	{
		const char* cache_path = result_filename;
		if( ZnkS_isBegin( cache_path, "./cachebox/" ) ){
			cache_path += 2;
		}
		ZnkStr_addf( dst, "<a class=MstyElemLink href=\"http://%s/easter?est_manager=cache&amp;command=bbsop&amp;cache_path=%s&backto=%s\">"
				"詳細モード</a> \n",
				xhr_auth_host, cache_path, src );
	}


	ZnkStr_add( dst, "<br>" );


	ZnkHtpURL_negateHtmlTagEffection( console_msg ); /* for XSS */

	ZnkStr_addf( dst, "<a class=MstyElemLink href=\"http://%s/easter\" target=_blank>Easter</a>&nbsp;", xhr_auth_host );
	ZnkStr_addf( dst, "<a class=MstyElemLink href=\"http://%s/cgis/custom_boy/custom_boy.cgi?cb_target=futaba&cb_type=automatic\">VirtualUSERS</a>&nbsp;", xhr_auth_host );
	ZnkStr_addf( dst, "<a class=MstyElemLink href=\"http://%s/\">Moai</a><br>", xhr_auth_host );
	ZnkStr_add( dst, "<span id=EasterReloadState></span>" );
	ZnkStr_add( dst, "<pre class=MstyMsgConsole>" );
	ZnkStr_add( dst, "<b>Easter BBS Console:</b>\n" );
	ZnkStr_add( dst, ZnkStr_cstr(console_msg) );
	ZnkStr_add( dst, "</pre> " );
	ZnkStr_add( dst, "</div></article>\n" );

	/***
	 * landmarkが複数ある場合もあり得る.
	 * それに対処するため、後方からの検索で最初に一致する場に挿入しなければならない.
	 */
	{
		size_t dst_pos = ZnkMem_rfind_data_BF( (uint8_t*)ZnkStr_cstr(text), ZnkStr_leng(text),
				(uint8_t*)landmark, Znk_strlen(landmark), 1 );
		if( dst_pos != Znk_NPOS ){
			ZnkStr_replace( text, dst_pos, Znk_strlen(landmark), ZnkStr_cstr(dst), ZnkStr_leng(dst) );
			result = true;
		}
	}

	ZnkStr_delete( dst );
	return result;
}


void
EstFilter_main( const char* result_file,
		const char* src, const char* target, RanoTextType txt_type, ZnkStr console_msg, bool save_img_cache, bool is_https )
{
	ZnkFile fp = Znk_fopen( result_file, "rb" );
	if( fp ){
		ZnkStr text = ZnkStr_new( "" );
		struct EstLinkInfo link_info = { 0 };
		ZnkStr ermsg = ZnkStr_new( "" );
		const EstFilterModule* filter_module = NULL;
		const char* easter_landmarking = is_https ? "via Easter on XhrDMZ(https)" : "via Easter on XhrDMZ";

		preloadTextAndInfos( text, src, &link_info, fp, save_img_cache, is_https );
		filter_module = link_info.filter_module_;

		/* HtmlTagフィルター及びHtmlプレーンテキストフィルター */
		if( !EstParser_invokeHtmlTagEx( text, filterHtmlTags, &link_info,
					filterPlaneTxt, &link_info,
					filterPreModify, &link_info,
					ermsg ) )
		{
			RanoLog_printf( "EstParser_invokeHtmlTagEx : failure. ermsg=[%s]\n", ZnkStr_cstr(ermsg) );
		}

		/* Easter_hostname */
		ZnkStrPtn_invokeInQuote( text,
				"<HEAD>", "</HEAD>",
				NULL, NULL,
				changeHEAD2head, NULL, true );
		ZnkStrPtn_invokeInQuote( text,
				"<head>", "</head>",
				NULL, NULL,
				addVarEaster_hostname, &link_info, false );

		/* その他の文字列置換処理 */
		if( filter_module && filter_module->on_other_ ){
			filter_module->on_other_( text, &link_info, easter_landmarking );
		}

		/* Easter BBSオペレーション */
		if( filter_module && filter_module->on_bbs_operation_ ){
			filter_module->on_bbs_operation_( text, result_file, src, console_msg );
		}

		ZnkStr_delete( ermsg );

		/* final process */
		{
			RanoModule mod = EstConfig_findModule( target );
			if( mod ){
				RanoModule_invokeOnResponse( mod, NULL, text, ZnkStr_cstr(link_info.unesc_src_) );
				RanoModule_filtTxt( mod, text, txt_type );
			}
		}

		ZnkStr_delete( link_info.unesc_src_ );
		ZnkStr_delete( link_info.base_href_ );
		ZnkStr_delete( link_info.hostname_ );
		ZnkStr_delete( link_info.hyperpost_url_ );
		Znk_fclose( fp );

		fp = Znk_fopen( result_file, "wb" );
		if( fp ){
			Znk_fputs( ZnkStr_cstr(text), fp );
			Znk_fclose( fp );
		}

		ZnkStr_delete( text );
	}
}


