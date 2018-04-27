#include <Est_filter.h>
#include <Est_link.h>
#include <Est_config.h>
#include <Est_parser.h>
#include <Est_base.h>
#include <Est_hint_manager.h>

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
preloadTextAndInfos( ZnkStr text, ZnkStr unesc_src, const char* src, struct EstLinkInfo* link_info, ZnkFile fp, bool save_img_cache )
{
	ZnkStr line = ZnkStr_new( "" );

	ZnkHtpURL_unescape_toStr( unesc_src, src, Znk_strlen(src) );

	{
		const char* s = ZnkStr_cstr( unesc_src );
		const char* q = NULL;
		s = RanoCGIUtil_skipProtocolPrefix( s );
		q = strchr( s, '/' );
		if( q ){
			link_info->hostname_ = ZnkStr_create( s, q-s );
		} else {
			link_info->hostname_ = ZnkStr_new( s );
		}
	}
	link_info->hyperpost_url_ = ZnkStr_new( "" );

	{
		const char* s = ZnkStr_cstr( unesc_src );
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

	while( true ){
		if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			break;
		}
		ZnkStr_add( text, ZnkStr_cstr(line) );
	}

	ZnkStr_delete( line );
}

static void
setBaseHRef( ZnkStr base_href, const char* val, const char* hostname )
{
	if( val[ 0 ] == '/' ){
		if( val[ 1 ] == '/' ){
			/***
			 * <base href= の指定が //で始まる場合は何も加工せずそのままセット.
			 */
			ZnkStr_set( base_href, val );
		} else {
			/***
			 * <base href= の指定が /で始まる場合はhostnameを前に追加.
			 */
			ZnkStr_set( base_href, "http://" );
			ZnkStr_add( base_href, hostname );
			ZnkStr_add( base_href, val );
		}
	} else {
		/***
		 * <base href= の指定が /で始まらない場合は何も加工せずそのままセット.
		 */
		ZnkStr_set( base_href, val );
	}
}

static int
filterHtmlTags( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend )
{
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );
	const char* xhr_auth_host = EstConfig_XhrAuthHost();

	if( ZnkS_eqCase( ZnkStr_cstr(tagname), "base" ) ){
		/* base hrefの値を取得 */
		ZnkVarp attr = ZnkVarpAry_find_byName_literal( varp_ary, "href", true );
		if( attr ){
			ZnkStr base_href = link_info->base_href_;
			const char* val = EstHtmlAttr_val( attr );
			setBaseHRef( base_href, val, ZnkStr_cstr(link_info->hostname_) );
		}
		/***
		 * <base href...>タグはeasterの動作を阻害するためとりあえず消去しておく.
		 * ここでは強制的にコメントアウト.
		 */
		ZnkStr_set( tagname, "!-- base" );
		ZnkStr_set( tagend, "-->" );

	} else if( ZnkS_eqCase( ZnkStr_cstr(tagname), "iframe" ) ){
		/***
		 * <iframe...>タグは強制的にコメントアウト.
		 */
		ZnkStr_set( tagname, "!-- iframe" );
		ZnkStr_set( tagend, "-->" );

	} else if( ZnkS_eqCase( ZnkStr_cstr(tagname), "/iframe" ) ){
		ZnkStr_set( tagname, "!-- /iframe" );
		ZnkStr_set( tagend, "-->" );

	} else if( ZnkS_eqCase( ZnkStr_cstr(tagname), "a" ) ){
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

	} else if( ZnkS_eqCase( ZnkStr_cstr(tagname), "img" ) ){
		/* img srcの値を取得 */
		ZnkVarp attr = ZnkVarpAry_find_byName_literal( varp_ary, "src", true );
		if( attr ){
			ZnkStr str = EstHtmlAttr_str( attr );

			/* ignore_hosts */
			const ZnkMyf hosts_myf = EstConfig_hosts_myf();
			ZnkStrAry ignore_hosts = hosts_myf ? ZnkMyf_find_lines( hosts_myf, "ignore_hosts" ) : NULL;
			char hostname[ 256 ] = "";
			ZnkStr req_urp = ZnkStr_new( "" );
			EstBase_getHostnameAndRequrp_fromEstVal( hostname, sizeof(hostname), req_urp, ZnkStr_cstr(str) );
			ZnkStr_delete( req_urp );
			if( ZnkStrAry_find_isMatch( ignore_hosts, 0, hostname, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
				ZnkStr_clear( str );
				return 0;
			}

			link_info->est_req_ = EstRequest_e_get; /* 直接取得 */
			return EstLink_filterImg( str, link_info );
		}

	} else if( ZnkS_eqCase( ZnkStr_cstr(tagname), "frame" ) ){
		/* frame srcの値を取得 */
		ZnkVarp attr = ZnkVarpAry_find_byName_literal( varp_ary, "src", true );
		if( attr ){
			ZnkStr str = EstHtmlAttr_str( attr );
			link_info->est_req_ = EstRequest_e_get; /* 直接取得 */
			return EstLink_filterLink( str, link_info, EstLinkXhr_e_ImplicitDMZ );
		}

	} else if( ZnkS_eqCase( ZnkStr_cstr(tagname), "link" ) ){
		/* link hrefの値を取得 */
		ZnkVarp href = ZnkVarpAry_find_byName_literal( varp_ary, "href", true );
		if( href ){
			ZnkStr str = EstHtmlAttr_str( href );
			if( !link_info->css_link_done_ ){
				ZnkVarp rel  = ZnkVarpAry_find_byName_literal( varp_ary, "rel",  true );
				ZnkVarp type = ZnkVarpAry_find_byName_literal( varp_ary, "type", true );
				char file_path[ 256 ] = "";
				Znk_snprintf( file_path, sizeof(file_path), "http://%s/msty.css", xhr_auth_host );
				if( rel && type 
				  && ZnkS_eq( EstHtmlAttr_val(rel),  "stylesheet" )
				  && ZnkS_eq( EstHtmlAttr_val(type), "text/css" )
				  && !ZnkS_eq( EstHtmlAttr_val(href), file_path ) )
				{
					ZnkStr_setf( tagend, " /> <link href=\"%s\" rel=\"stylesheet\" type=\"text/css\" />", file_path );
					link_info->css_link_done_ = true;
				}
			}
			link_info->est_req_ = EstRequest_e_get; /* 直接取得 */
			return EstLink_filterLink( str, link_info, EstLinkXhr_e_ImplicitDMZ );
		}

	} else if( ZnkS_eqCase( ZnkStr_cstr(tagname), "script" ) ){
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

			/* ignore_hosts */
			const ZnkMyf hosts_myf = EstConfig_hosts_myf();
			ZnkStrAry ignore_hosts = hosts_myf ? ZnkMyf_find_lines( hosts_myf, "ignore_hosts" ) : NULL;
			char hostname[ 256 ] = "";
			ZnkStr req_urp = ZnkStr_new( "" );
			EstBase_getHostnameAndRequrp_fromEstVal( hostname, sizeof(hostname), req_urp, ZnkStr_cstr(str) );
			ZnkStr_delete( req_urp );
			if( ZnkStrAry_find_isMatch( ignore_hosts, 0, hostname, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
				ZnkStr_clear( str );
				return 0;
			}

			link_info->est_req_ = EstRequest_e_get; /* 直接取得 */
			return EstLink_filterScript( str, link_info );
		}

	} else if( ZnkS_eqCase( ZnkStr_cstr(tagname), "form" ) ){
		/* form actionの値を取得 */
		ZnkVarp attr = ZnkVarpAry_find_byName_literal( varp_ary, "action", true );
		if( attr ){
			ZnkStr str = EstHtmlAttr_str( attr );
			const char* target = EstConfig_findTargetName( ZnkStr_cstr(link_info->hostname_) );
			link_info->est_req_ = EstRequest_e_post;
			if( ZnkS_eq( target, "futaba" ) ){
				if( ZnkStrEx_strstr( str, "futaba.php" ) ){
					ZnkVarp enctype = ZnkVarpAry_find_byName_literal( varp_ary, "enctype", true );
					if( enctype && ZnkS_eq( ZnkVar_cstr(enctype), "multipart/form-data" ) ){
						link_info->est_req_ = EstRequest_e_post_ex;
						ZnkStr_set( link_info->hyperpost_url_, ZnkStr_cstr(str) ); 
						EstLink_filterLink( link_info->hyperpost_url_, link_info, EstLinkXhr_e_ExplicitAuth );
						link_info->est_req_ = EstRequest_e_post;
						{
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

	} else if( ZnkS_eqCase( ZnkStr_cstr(tagname), "meta" ) ){

		/* <META HTTP-EQUIV="refresh" content="1;URL=..."> のURL=の値を解釈しフィルタリング */
		ZnkVarp attr = ZnkVarpAry_find_byName_literal( varp_ary, "content", true );
		if( attr ){
			ZnkStr str = EstHtmlAttr_str( attr );
			const char* p = ZnkStrEx_strstr( str, "URL=" );
			if( p ){
				int result = 0;
				ZnkStr url = ZnkStr_new( "" );
				p += Znk_strlen_literal("URL=");
				ZnkStr_set( url, p );
				link_info->est_req_ = EstRequest_e_get;
				result = EstLink_filterLink( url, link_info, EstLinkXhr_e_ImplicitDMZ );
				ZnkStr_add( url, "#EasterThreadOperation" );
				/* URL=以降を新しいものに置き換え */
				ZnkStr_replace( str, p - ZnkStr_cstr(str), Znk_NPOS, ZnkStr_cstr(url), ZnkStr_leng(url) );
				ZnkStr_delete( url );
				return result;
			}
		}
	} else if( ZnkS_eqCase( ZnkStr_cstr(tagname), "div" ) ){
		/* for futaba : 広告の無駄なスペース(高さ)を削除 */
		ZnkVarp attr = ZnkVarpAry_find_byName_literal( varp_ary, "style", true );
		if( attr ){
			ZnkStr str = ZnkVar_str( attr );
			ZnkSRef old_ptn = { 0 };
			ZnkSRef new_ptn = { 0 };
			ZnkSRef_set_literal( &old_ptn, "width:300px;height:250px;" );
			ZnkSRef_set_literal( &new_ptn, "width:300px;height:0px;" );
			ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
			ZnkSRef_set_literal( &old_ptn, "width:336px;height:280px;" );
			ZnkSRef_set_literal( &new_ptn, "width:336px;height:0px;" );
			ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
			ZnkSRef_set_literal( &old_ptn, "width:728px;height:90px;" );
			ZnkSRef_set_literal( &new_ptn, "width:728px;height:0px;" );
			ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
			return 0;
		}
	}
	return 0;
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

static int
filterPlaneTxt( ZnkStr planetxt, void* arg )
{
	ZnkStr unesc_src = Znk_force_ptr_cast( ZnkStr, arg );
	if( EstConfig_isAutoLink() && ZnkStrEx_strstr( unesc_src, "mode=cat" ) == NULL ){
		const char* cur = NULL;
		const char* p = NULL;
		const char* h = NULL;
		ZnkStr new_str = ZnkStr_new( "" );
		ZnkStr new_url = ZnkStr_new( "" );
		char hostname[ 256 ] = "";
		const char* target = NULL;
	
		cur = ZnkStr_cstr( planetxt );
		while( *cur ){
			p = Znk_strstr( cur, "http://" );
			if( p ){
				size_t pos  = 0;
				size_t leng = 0;
				const char* q = p;
				h = Znk_strchr( p + Znk_strlen_literal("http://"), '/' );
				if( h ){
					ZnkS_copy( hostname, sizeof(hostname), p + Znk_strlen_literal("http://"), h - p - Znk_strlen_literal("http://") );
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

				ZnkStr_set( new_str, "<a class=MstyOpenLink href=\"" );
				ZnkStr_add( new_str, ZnkStr_cstr(new_url) );
				ZnkStr_add( new_str, "\" target=_blank> <font color=\"#404000\"> Open </font>" );
				ZnkStr_add( new_str, "</a>" );
				if( target ){
					ZnkStr_addf( new_str, "<span class=MstyAutoLinkTarget>%s</span>", ZnkStr_cstr(new_url) );
				} else {
					ZnkStr_addf( new_str, "<span class=MstyAutoLinkOther>%s</span>", ZnkStr_cstr(new_url) );
				}
		
				ZnkStr_replace( planetxt, pos, leng, ZnkStr_cstr(new_str), ZnkStr_leng(new_str) );
				cur = ZnkStr_cstr( planetxt ) + pos + ZnkStr_leng(new_str);
			} else {
				break;
			}
		}
	
		cur = ZnkStr_cstr( planetxt );
		while( *cur ){
			p = Znk_strstr( cur, "https://" );
			if( p ){
				size_t pos  = 0;
				size_t leng = 0;
				const char* q = p;
				h = Znk_strchr( p + Znk_strlen_literal("https://"), '/' );
				if( h ){
					ZnkS_copy( hostname, sizeof(hostname), p + Znk_strlen_literal("https://"), h - p - Znk_strlen_literal("https://") );
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
					ZnkStr_set( new_url, "/easter?est_view=http://" ); /* httpへ変更 */
					ZnkStr_append( new_url, p + Znk_strlen_literal("https://"), leng - Znk_strlen_literal("https://") );
				} else {
					ZnkStr_assign( new_url, 0, p, leng );
				}

				ZnkStr_set( new_str, "<a class=MstyOpenLink href=\"" );
				ZnkStr_add( new_str, ZnkStr_cstr(new_url) );
				ZnkStr_add( new_str, "\" target=_blank> <font color=\"#404000\"> Open </font>" );
				ZnkStr_add( new_str, "</a>" );
				if( target ){
					ZnkStr_addf( new_str, "<span class=MstyAutoLinkTarget>%s</span>", ZnkStr_cstr(new_url) );
				} else {
					ZnkStr_addf( new_str, "<span class=MstyAutoLinkOther>%s</span>", ZnkStr_cstr(new_url) );
				}
		
				ZnkStr_replace( planetxt, pos, leng, ZnkStr_cstr(new_str), ZnkStr_leng(new_str) );
				cur = ZnkStr_cstr( planetxt ) + pos + ZnkStr_leng(new_str);
			} else {
				break;
			}
		}
	
		ZnkStr_delete( new_str );
		ZnkStr_delete( new_url );
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
		ZnkStr_insert( str, 0, ZnkStr_cstr(js), ZnkStr_leng(js) );
		ZnkStr_delete( js );
	}
	return 1;
}
static int
replaceEstViewToGet( ZnkStr str, void* arg )
{
	{
		ZnkSRef old_ptn = { 0 };
		ZnkSRef new_ptn = { 0 };
		ZnkSRef_set_literal( &old_ptn, "est_view=" );
		ZnkSRef_set_literal( &new_ptn, "est_get=" );
		ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	return 1;
}
static int
filterClear( ZnkStr str, void* arg )
{
	ZnkStr_clear( str );
	return 1;
}

static int
addOpenHyperpostButton( ZnkStr str, void* arg )
{
	ZnkStr unesc_src = Znk_force_ptr_cast( ZnkStr, arg );
	ZnkStr_addf( str, "<br><a style='margin-top: 10px' class=MstyElemLink "
			"onClick=\"document.Easter_fm_main.action = Easter_hyperpost_url + '&est_original_url=%s'; document.Easter_fm_main.submit(); \" "
			"target=_blank>Easter HyperPostで返信する</a>",
			ZnkStr_cstr(unesc_src) );
	return 1;
}
static int
addOpenHyperpostButtonThre( ZnkStr str, void* arg )
{
	ZnkStr_addf( str, "<br><a style='margin-top: 10px' class=MstyElemLink "
			"onClick=\"document.Easter_fm_main.action = Easter_hyperpost_url; document.Easter_fm_main.submit(); \" "
			"target=_blank>Easter HyperPostでスレッドを立てる</a>" );
	return 1;
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
static void
insertThreadOperation( ZnkStr text, 
		const char* result_filename, const char* landmark, const char* src, const char* bbs_id_name, ZnkStr console_msg )
{
	const char* xhr_auth_host = EstConfig_XhrAuthHost();
	ZnkStr dst = ZnkStr_new( "" );
	ZnkStr hint_table = EstHint_getHintTable( "bbs_operation" );

	ZnkStr_setf( dst, "%s\n", landmark );
	ZnkStr_add( dst, "<a name='EasterThreadOperation'></a>" );
	ZnkStr_add( dst, "<table><tr><td>" );
	ZnkStr_add( dst, "<fieldset class=MstyBBSOperation><legend><font color=\"#808000\">Easter BBS Operation</font></legend>\n" );

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
	//ZnkStr_addf( dst, "<a class=QandALink href=\"javascript:void(0);\" onclick=\"displayQandA( 'aboutBBSOperation' );\"><b>?</b></a>\n" );
	ZnkStr_addf( dst, "<a class=MstyQandALink href=\"javascript:void(0);\" onclick=\"displayQandA( 'Hint', 'aboutBBSOperation' );\"><img src='/cgis/easter/publicbox/icons/question_16.png'></a>\n" );
	ZnkStr_addf( dst, "<font size=-1><div id=startLink></div></font>\n" );
	ZnkStr_addf( dst, "<font size=-1><div id=aboutBBSOperation></div></font>\n" );
	ZnkStr_addf( dst, "<a class=MstyElemLink href=\"/easter?est_reload=%s\" onClick=\"Easter_reload('%s', '%s'); return false;\" >リロード</a>", src, src, bbs_id_name );

	//ZnkStr_add( dst, "<br>" );
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

	ZnkStr_addf( dst, "<a class=MstyElemLink href=\"http://%s/easter\" target=_blank>Easterトップページ</a><br>", xhr_auth_host );
	//ZnkStr_add( dst, "<a class=MstyElemLink href=\"javascript:void(0);\">JSによるCookieをクリアする</a>" );
	ZnkStr_add( dst, "<pre class=MstyMsgConsole>" );
	ZnkStr_add( dst, "<b>Easter BBS Console:</b>\n" );
	ZnkStr_add( dst, ZnkStr_cstr(console_msg) );
	ZnkStr_add( dst, "</pre> " );
	ZnkStr_add( dst, "</fieldset>\n" );
	ZnkStr_add( dst, "</td></tr></table>\n" );

	/***
	 * landmarkが複数ある場合もあり得る.
	 * それに対処するため、後方からの検索で最初に一致する場に挿入しなければならない.
	 */
	{
		size_t dst_pos = ZnkMem_rfind_data_BF( (uint8_t*)ZnkStr_cstr(text), ZnkStr_leng(text),
				(uint8_t*)landmark, Znk_strlen(landmark), 1 );
		if( dst_pos != Znk_NPOS ){
			ZnkStr_replace( text, dst_pos, Znk_strlen(landmark), ZnkStr_cstr(dst), ZnkStr_leng(dst) );
		}
	}

	ZnkStr_delete( dst );
}



void
EstFilter_main( const char* result_file, const char* src, const char* target, RanoTextType txt_type, ZnkStr console_msg, bool save_img_cache )
{
	ZnkFile fp = Znk_fopen( result_file, "rb" );
	if( fp ){
		ZnkStr text = ZnkStr_new( "" );
		ZnkStr unesc_src = ZnkStr_new( "" );
		struct EstLinkInfo link_info = { 0 };
		ZnkStr ermsg = ZnkStr_new( "" );

		preloadTextAndInfos( text, unesc_src, src, &link_info, fp, save_img_cache );

		if( !EstParser_invokeHtmlTagEx( text, filterHtmlTags, &link_info,
					filterPlaneTxt, unesc_src,
					ermsg ) )
		{
			RanoLog_printf( "EstParser_invokeHtmlTagEx : failure. ermsg=[%s]\n", ZnkStr_cstr(ermsg) );
		}

		/* Ester_hostname */
		{
			ZnkStrPtn_invokeInQuote( text,
					"<head>", "</head>",
					NULL, NULL,
					addVarEaster_hostname, &link_info, false );
		}

		if( ZnkS_eq( target, "futaba" ) ){
			ZnkSRef old_ptn = { 0 };
			ZnkSRef new_ptn = { 0 };

			/* かなりトリッキーだが現時点では仕方がない */
			ZnkSRef_set_literal( &old_ptn, ".webm</a>-(" );
			ZnkSRef_set_literal( &new_ptn, ".webm</a> <font size=-1 color=\"#808000\">by Easter Video Cache</font> -(" );
			ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 

			ZnkStrPtn_invokeInQuote( text,
					".webm\" target='_blank'>", ".webm\" target='_blank'><img src=",
					NULL, NULL,
					replaceEstViewToGet, &link_info, true );

			/* futaba : via Easter on XhrDMZ landmarking */
			ZnkSRef_set_literal( &old_ptn, "＠ふたば</span>" );
			ZnkSRef_set_literal( &new_ptn, "＠ふたば <font size=\"-1\" color=\"#808000\">via Easter on XhrDMZ</font></span>" );
			ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 

			/* futaba : via Easter on XhrDMZ landmarking(for i-mode) */
			ZnkStrPtn_invokeInQuote( text,
					"data-iconpos=\"left\">TOP</a> <h1>", "</h1>",
					NULL, NULL,
					landmarkEaster, &link_info, false );
			ZnkStrPtn_invokeInQuote( text,
					"data-iconpos=\"left\">戻る</a> <h1>", "</h1>",
					NULL, NULL,
					landmarkEaster, &link_info, false );

			/* futaba : ad */
			ZnkStrPtn_invokeInQuote( text,
					"microadCompass.queue.push({", "});",
					NULL, NULL,
					filterClear, &link_info, true );

			/* futaba : デフォルトの[リロード]のリンクは面倒なので一旦消去する */
			ZnkStrPtn_invokeInQuote( text,
					"<span id=\"contres\">", "<span id=\"contdisp\">",
					NULL, NULL,
					filterClear, &link_info, false );

			/* futaba : Easter_scroll */
			ZnkSRef_set_literal( &old_ptn, "scrll()" );
			ZnkSRef_set_literal( &new_ptn, "Easter_scroll( 'resto' )" );
			ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 

			/* futaba : open hyperpost button */
			ZnkStrPtn_invokeInQuote( text,
					"<input type=submit value=\"返信する\" onClick=\"ptfk(", ")\">",
					NULL, NULL,
					addOpenHyperpostButton, unesc_src, true );
			ZnkStrPtn_invokeInQuote( text,
					"<input type=submit value=\"スレッドを立てる\" onClick=\"ptfk(", ")\">",
					NULL, NULL,
					addOpenHyperpostButtonThre, NULL, true );

		} else if( ZnkS_eq( target, "5ch" ) ){
			ZnkSRef old_ptn = { 0 };
			ZnkSRef new_ptn = { 0 };

#if 0
			ZnkSRef_set_literal( &old_ptn, "if(document.cookie.indexOf('viewer=pc')!=-1){" );
			ZnkSRef_set_literal( &new_ptn, "if( false ){" );
			ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
#endif

			/* Easter URL変換 : subback-tmpl x-handlebars-template */
			//ZnkSRef_set_literal( &old_ptn, " href=\"<%- helper.subDomain(thread[2]) %>/" );
			ZnkSRef_set_literal( &old_ptn, " href=\"/<%- helper.subDomain(thread[2]) %>/" );
			ZnkSRef_set_literal( &new_ptn, " href=\"/easter?est_view=//itest.5ch.net/" );
			ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 

			ZnkSRef_set_literal( &old_ptn, "<% if(helper.checkl50()) { %>/l50<% } %>" );
			ZnkSRef_set_literal( &new_ptn, "<% if(helper.checkl50()) { %>/l50<% } %>/" );
			ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 

			/* 5ch : via Easter on XhrDMZ landmarking */
			ZnkStrPtn_invokeInQuote( text,
					"<h1 class=\"title\">", "\n",
					NULL, NULL,
					landmarkEaster, &link_info, false );

			/* 5ch smart phone : via Easter on XhrDMZ landmarking */
			ZnkSRef_set_literal( &old_ptn, "<div id=\"title\" class=\"threadview_response_title\"></div>" );
			ZnkSRef_set_literal( &new_ptn, "<span id=\"title\" class=\"threadview_response_title\"></span> <font size=\"-1\" color=\"#808000\">via Easter on XhrDMZ</font>" );
			ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 

			/* 5ch : ソースでの可読性向上のための改行挿入 */
			ZnkSRef_set_literal( &old_ptn, "</span></div></div><br>" );
			ZnkSRef_set_literal( &new_ptn, "</span></div></div><br>\n" );
			ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 

			/* 5ch : bbstable.html */
			ZnkStrPtn_invokeInQuote( text,
					"【<B>", "</B>】",
					NULL, NULL,
					filter5chBBSTable, &link_info, true );

			/* 5ch : Easter_scroll */
			ZnkSRef_set_literal( &old_ptn, "function standardize(node){" );
			ZnkSRef_set_literal( &new_ptn, "Easter_scroll( 'key' ); function standardize(node){" );
			ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
		}

		/* Easter スレッドオペレーション */
		{
			ZnkSRef landmark = { 0 };

			/* 5ch */
			ZnkSRef_set_literal( &landmark, "<span class=\"newpostbutton\">" );
			insertThreadOperation( text,
					result_file, landmark.cstr_, src, "key", console_msg );

			/* futaba */
			ZnkSRef_set_literal( &landmark, "</div><!--スレッド終了-->" );
			insertThreadOperation( text,
					result_file, landmark.cstr_, src, "resto", console_msg );

		}

		ZnkStr_delete( ermsg );

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


