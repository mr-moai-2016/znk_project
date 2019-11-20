#include <Est_filter_default.h>
#include <Est_parser.h>
#include <Est_link.h>
#include <Est_config.h>

#include <Znk_str_ex.h>
#include <Znk_str_ptn.h>

static void
setBaseHRef( ZnkStr base_href, const char* val, const char* hostname, bool is_https_parent )
{
	if( val[ 0 ] == '/' ){
		if( val[ 1 ] == '/' ){
			/***
			 * <base href= の指定が //で始まる場合.
			 */
			//ZnkStr_set( base_href, val );
			if( is_https_parent ){
				ZnkStr_set( base_href, "https://" );
			} else {
				ZnkStr_set( base_href, "http://" );
			}
			ZnkStr_add( base_href, val+2 );
		} else {
			/***
			 * <base href= の指定が /で始まる場合はhostnameを前に追加.
			 */
			if( is_https_parent ){
				ZnkStr_set( base_href, "https://" );
			} else {
				ZnkStr_set( base_href, "http://" );
			}
			ZnkStr_add( base_href, hostname );
			ZnkStr_add( base_href, val );
		}
	} else {
		/***
		 * <base href= の指定が /で始まらない場合は何も加工せずそのままセット.
		 * (ただしhttp:// と https:// から始まる場合はそれは除去.)
		 */
		if( ZnkS_isBegin( val, "http://" ) ){
			ZnkStr_set( base_href, val+7 );
		} else if( ZnkS_isBegin( val, "https://" ) ){
			ZnkStr_set( base_href, val+8 );
		} else {
			ZnkStr_set( base_href, val );
		}
	}
}

static int
on_base( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend )
{
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );

	/* base hrefの値を取得 */
	ZnkVarp attr = ZnkVarpAry_find_byName_literal( varp_ary, "href", true );
	if( attr ){
		ZnkStr base_href = link_info->base_href_;
		const char* val = EstHtmlAttr_val( attr );
		setBaseHRef( base_href, val, ZnkStr_cstr(link_info->hostname_), link_info->is_https_parent_ );
	}
	/***
	 * <base href...>タグはeasterの動作を阻害するためとりあえず消去しておく.
	 * ここでは強制的にコメントアウト.
	 */
	ZnkStr_set( tagname, "!-- base" );
	ZnkStr_set( tagend, "-->" );
	return 0;
}

static int
on_iframe( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend, bool end_kind )
{
	/***
	 * <iframe...>タグは強制的にコメントアウト.
	 */
	if( end_kind ){
		ZnkStr_set( tagname, "!-- /iframe" );
		ZnkStr_set( tagend, "-->" );
	} else {
		ZnkStr_set( tagname, "!-- iframe" );
		ZnkStr_set( tagend, "-->" );
	}
	return 0;
}

static int
on_frame( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend, bool end_kind )
{
	if( !end_kind ){
		struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );
		/* frame srcの値を取得 */
		ZnkVarp attr = ZnkVarpAry_find_byName_literal( varp_ary, "src", true );
		if( attr ){
			ZnkStr str = EstHtmlAttr_str( attr );
			link_info->est_req_ = EstRequest_e_get; /* 直接取得 */
			return EstLink_filterLink( str, link_info, EstLinkXhr_e_ImplicitDMZ );
		}
	}
	return 0;
}

static int
on_img( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend )
{
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );

	/* img srcの値を取得 */
	ZnkVarp attr = ZnkVarpAry_find_byName_literal( varp_ary, "src", true );
	if( attr ){
		ZnkStr str = EstHtmlAttr_str( attr );
		if( !EstFilter_ignoreHosts( str ) ){
			link_info->est_req_ = EstRequest_e_get; /* 直接取得 */
			return EstLink_filterImg( str, link_info );
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
#if 0
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
				ZnkStr_set( tagend, " /> " );
				ZnkStr_addf( tagend, "<link href=\"%s\" rel=\"stylesheet\" type=\"text/css\" />", file_path );
				ZnkStr_addf( tagend, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" );
				ZnkStr_addf( tagend, "<link href=\"http://%s/bulma.css\" rel=\"stylesheet\" type=\"text/css\" />", xhr_auth_host );
				link_info->css_link_done_ = true;
			}
		}
#endif
		link_info->est_req_ = EstRequest_e_get; /* 直接取得 */
		return EstLink_filterLink( str, link_info, EstLinkXhr_e_ImplicitDMZ );
	}
	return 0;
}

static int
on_meta( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend )
{
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );

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

	return 0;
}

EstFilterModule*
EstFilter_default_getModule( void )
{
	static EstFilterModule* st_module = NULL;
	if( st_module == NULL ){
		static EstFilterModule st_module_instance = { 0 };
		st_module_instance.on_base_   = on_base;
		st_module_instance.on_iframe_ = on_iframe;
		st_module_instance.on_frame_  = on_frame;
		st_module_instance.on_img_    = on_img;
		st_module_instance.on_link_   = on_link;
		st_module_instance.on_meta_   = on_meta;
		st_module = &st_module_instance;
	}
	return st_module;
}
