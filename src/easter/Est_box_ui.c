#include <Est_box_ui.h>
#include <Est_config.h>
#include <Est_ui_base.h>
#include <Est_cinf_ui.h>
#include <Est_linf_list.h>

#include <Znk_s_base.h>
#include <Znk_date.h>

static void
renderOpen( ZnkStr ans, const char* style_class_name, const char* manager,
		const char* unesc_url, const char* authentic_key, const bool is_target_blank )
{
	ZnkStr_add( ans, "<a " );
	if( style_class_name ){
		ZnkStr_addf( ans, "class=%s ", style_class_name );
	}
	ZnkStr_addf( ans, "href=\"/easter?est_manager=%s&amp;command=open&amp;cache_path=%s&amp;Moai_AuthenticKey=%s\" %s>Open</a>",
			manager,
			unesc_url, authentic_key,
			is_target_blank ? "target=_blank" : "" );
}

static void
renderThumbA( ZnkStr ans, const char* unesc_url, const char* esc_url,
		const char* xhr_dmz, bool is_target_blank,
		size_t width, size_t height, bool is_thumbnail )
{
	const char* ext = ZnkS_get_extension( unesc_url,'.' );
	ZnkStr_addf( ans, "<a href='http://%s/cgis/easter/%s' %s>", xhr_dmz, esc_url,
			is_target_blank ? "target=_blank" : "" );

	if( ZnkS_eqCase( ext, "webm" ) ){
		ZnkStr_addf( ans, "<img src='http://%s/cgis/easter/publicbox/icons/webm.png' width=%zu height=%zu>",
				xhr_dmz, width, height );
	} else if( ZnkS_eqCase( ext, "mp4" ) ){
		ZnkStr_addf( ans, "<img src='http://%s/cgis/easter/publicbox/icons/mp4.png' width=%zu height=%zu>",
				xhr_dmz, width, height );
	} else if( ZnkS_eqCase( ext, "htm" ) || ZnkS_eqCase( ext, "html" ) ){
		ZnkStr_addf( ans, "<img src='http://%s/cgis/easter/publicbox/icons/html.png' width=%zu height=%zu>",
				xhr_dmz, width, height );
	} else if( is_thumbnail ){
		ZnkStr_addf( ans, "<img src='http://%s/cgis/easter/%s' width=%zu height=%zu>",
				xhr_dmz, esc_url, width, height );
	} else {
		ZnkStr_addf( ans, "<img src='http://%s/cgis/easter/publicbox/icons/file.png' width=%zu height=%zu>",
				xhr_dmz, width, height );
	}

	ZnkStr_add( ans, "</a>" );
}

static void
renderImgElem( ZnkStr ans, const char* style_class_name,
		const char* unesc_url, const char* esc_url, const char* elem_name,
		size_t width, size_t height,
		const char* xhr_dmz, const bool is_target_blank )
{
	const bool is_thumbnail = true;
	const size_t max_width  = EstConfig_getPreviewMaxWidth();
	const size_t max_height = EstConfig_getPreviewMaxHeight();

	ZnkStr_add( ans, "<br>" );
	ZnkStr_add( ans, "<a " );
	if( style_class_name ){
		ZnkStr_addf( ans, "class=%s ", style_class_name );
	}

	ZnkStr_add( ans, "name=ahref_previewable " );
	ZnkStr_addf( ans, "href=\"javascript:void(0);\" onclick=\"Easter_showPreview( this, 'http://%s/cgis/easter/%s', %zu, %zu );\">",
			xhr_dmz, esc_url, max_width, max_height );

	ZnkStr_add( ans, "<img src='/cgis/easter/publicbox/icons/file.png'>" );
	ZnkStr_addf( ans, " %s", elem_name );

	ZnkStr_add( ans, "</a>" );

	ZnkStr_add( ans, "<br>" );
	renderThumbA( ans, unesc_url, esc_url,
			xhr_dmz, is_target_blank,
			width, height, is_thumbnail );
}

static void
viewSearchFileItem( ZnkStr ans, EstFInf finf,
		const char* url, const char* esc_url, const char* id, ZnkStr title,
		const char* style_class_name, bool is_target_blank,
		const char* authentic_key )
{
	bool is_thumbnail = true;
	const char* xhr_dmz = EstConfig_XhrDMZ();
	const char* p = Znk_strrchr( url, '/' );
	const char* elem_name = url;
	size_t width  = EstConfig_getThumbSize();
	size_t height = EstConfig_getThumbSize();
	size_t max_width  = EstConfig_getPreviewMaxWidth();
	size_t max_height = EstConfig_getPreviewMaxHeight();

	renderOpen( ans, style_class_name, "img_viewer",
			url, authentic_key, is_target_blank );

	ZnkStr_add( ans, "<a " );
	if( style_class_name ){
		ZnkStr_addf( ans, "class=%s ", style_class_name );
	}

	if( p ){ elem_name = p+1; }

	/***
	 * HTMLの場合、DMZにおいて開く.
	 */
	if( ZnkS_eqCase( ZnkS_get_extension(url,'.'), "htm" ) || ZnkS_eqCase( ZnkS_get_extension(url,'.'), "html" ) ){
		const char* url_for_dmz = NULL;
		if( ZnkS_isBegin_literal( url, "cachebox/" ) ){
			url_for_dmz = url + Znk_strlen_literal( "cachebox/" );
			url_for_dmz = Znk_strchr( url_for_dmz, '/' );
			if( url_for_dmz ){
				++url_for_dmz;
			}
			ZnkStr_addf( ans, "href=\"http://%s/easter?est_get=%s\" %s>",
					xhr_dmz,
					url_for_dmz, is_target_blank ? "target=_blank" : "" );
		} else {
			ZnkStr_addf( ans, "href=\"http://%s/cgis/easter/%s\" %s>",
					xhr_dmz,
					url, is_target_blank ? "target=_blank" : "" );
		}
		ZnkStr_addf( ans, "<img src='/cgis/easter/publicbox/icons/file.png'>" );
		ZnkStr_addf( ans, " %s", elem_name );
		ZnkStr_add( ans, "</a>" );

		EstUIBase_getHtmlTitle( title, url, 10 );
		ZnkStr_addf( ans, " : [%s]", ZnkStr_cstr( title ) );
		is_thumbnail = false;

	} else if( ZnkS_eqCase( ZnkS_get_extension(url,'.'), "jpg" )
	        || ZnkS_eqCase( ZnkS_get_extension(url,'.'), "png" )
	        || ZnkS_eqCase( ZnkS_get_extension(url,'.'), "gif" )
	){
		ZnkStr_add( ans, "name=ahref_previewable " );
		ZnkStr_addf( ans, "href=\"javascript:void(0);\" onclick=\"Easter_showPreview( this, 'http://%s/cgis/easter/%s', %zu, %zu );\">",
				xhr_dmz, esc_url, max_width, max_height );

		ZnkStr_add( ans, "<img src='/cgis/easter/publicbox/icons/file.png'>" );
		ZnkStr_addf( ans, " %s", elem_name );
		ZnkStr_add( ans, "</a>" );
	} else {
		ZnkStr_addf( ans, "href=\"http://%s/cgis/easter/%s\" %s>",
				xhr_dmz, esc_url, is_target_blank ? "target=_blank" : "" );

		ZnkStr_addf( ans, "<img src='/cgis/easter/publicbox/icons/file.png'>" );
		ZnkStr_addf( ans, " %s", elem_name );
		ZnkStr_add( ans, "</a>" );
	}

	if( finf ){
		const char* file_path = EstFInf_file_path( finf );
		const char* file_tags = EstFInf_file_tags( finf );
		ZnkDate access_time = { 0 };
		size_t file_size = EstFInf_file_size( finf );
		ZnkStr access_time_str = ZnkStr_new( "" );

		EstFInf_access_time( finf, &access_time );
		ZnkDate_getStr( access_time_str, 0, &access_time, ZnkDateStr_e_Std );

		ZnkStr_add( ans, "<br>" );
		renderThumbA( ans, url, esc_url,
				xhr_dmz, is_target_blank,
				width, height, is_thumbnail );

		ZnkStr_add( ans, "<span style=display:inline-block;vertical-align:top;padding:4px>" );

		/* file_path */
		ZnkStr_addf( ans, "<font size=-1>%s</font><br>", file_path );

		/* file_tags */
		ZnkStr_addf( ans, "<font size=-1>%s</font><br>", file_tags );

		/* access_time */
		ZnkStr_addf( ans, "<font size=-1>%s</font> &nbsp;", ZnkStr_cstr( access_time_str ) );

		/* file_size */
		ZnkStr_addf( ans, "<font size=-1>%zuB</font>", file_size );

		ZnkStr_add( ans, "</span>" );
	}
}

static void
viewSearchFInf( ZnkStr ans, void* inf_ptr,
		const char* url_prefix, const char* url, const char* unesc_url, const char* esc_url,
		const char* id, ZnkStr title,
		const char* style_class_name, bool is_target_blank,
		const char* authentic_key )
{
	EstFInf finf = Znk_force_ptr_cast( EstFInf, inf_ptr );
	viewSearchFileItem( ans, finf,
			url, esc_url, id, title,
			style_class_name, is_target_blank,
			authentic_key );
}

static void*
getSearchFInf( ZnkVarpAry list, size_t idx, const char** id, const char** url )
{
	EstFInf finf = EstFInfList_at( list, idx );
	*id  = EstFInf_id( finf );
	*url = EstFInf_file_path( finf );
	return finf;
}


void
EstBoxUI_make_forSearchResult( ZnkStr ans, ZnkVarpAry finf_list,
		size_t begin_idx, size_t end_idx, const char* authentic_key )
{
	static const EstCInfViewFuncT    view_func     = viewSearchFInf;
	static const EstCInfGetElemFuncT get_elem_func = getSearchFInf;
	static const char* style_class_name = "MstyFInfElemLink";
	static const char* new_linkid       = "";
	static const char* url_prefix       = "/easter?est_manager=cache&amp;command=view&amp;cache_path=";
	static const char* id_prefix        = "md5ext_";
	static const bool  is_target_blank  = true;
	static const bool  is_checked       = false;
	const size_t show_file_num = EstConfig_getShowFileNum();
	const size_t line_file_num = EstConfig_getLineFileNum();
	const size_t thumb_size    = EstConfig_getThumbSize();

	EstCInfListUI_make( ans,
			finf_list, view_func, get_elem_func,
			begin_idx, end_idx,
			style_class_name, new_linkid, url_prefix, id_prefix,
			is_target_blank, show_file_num, line_file_num, thumb_size,
			authentic_key, is_checked );
}

static void
viewFSysElem( ZnkStr ans, void* inf_ptr,
		const char* url_prefix, const char* url, const char* unesc_url, const char* esc_url,
		const char* id, ZnkStr title,
		const char* style_class_name, bool is_target_blank,
		const char* authentic_key )
{
	//bool is_thumbnail = true;
	size_t width  = EstConfig_getThumbSize();
	size_t height = EstConfig_getThumbSize();
	const size_t show_file_num = EstConfig_getShowFileNum();
	const char* xhr_dmz = EstConfig_XhrDMZ();

	if( ZnkS_isBegin_literal( id, "cache_dir" ) ){
		const char* p   = Znk_strrchr( unesc_url, '/' );
		const char* elem_name = unesc_url;
		if( p ){ elem_name = p+1; }

		ZnkStr_add( ans, "<a " );
		if( style_class_name ){
			ZnkStr_addf( ans, "class=%s ", style_class_name );
		}

		ZnkStr_addf( ans, "href=\"%s%s"
				"&amp;est_cache_end=%zu"
				"&amp;Moai_AuthenticKey=%s\""
				" %s>"
				"<img src='/cgis/easter/publicbox/icons/folder.png'> %s</a>",
				url_prefix, esc_url, show_file_num, authentic_key,
				is_target_blank ? "target=_blank" : "",
				elem_name );
	} else if( ZnkS_isBegin_literal( id, "cache_file" ) || ZnkS_isBegin_literal( id, "cache_path" ) ){
		const char* p   = Znk_strrchr( unesc_url, '/' );
		const char* elem_name = unesc_url;
		if( p ){ elem_name = p+1; }

		/***
		 * HTMLの場合、DMZにおいて開く.
		 */
		if( ZnkS_eqCase( ZnkS_get_extension(unesc_url,'.'), "htm" ) || ZnkS_eqCase( ZnkS_get_extension(unesc_url,'.'), "html" ) ){
			const char* url_for_dmz = NULL;

			ZnkStr_add( ans, "<a " );
			if( style_class_name ){
				ZnkStr_addf( ans, "class=%s ", style_class_name );
			}

			if( ZnkS_isBegin_literal( unesc_url, "cachebox/" ) ){
				url_for_dmz = unesc_url + Znk_strlen_literal( "cachebox/" );
				url_for_dmz = Znk_strchr( url_for_dmz, '/' );
				if( url_for_dmz ){
					++url_for_dmz;
				}
				if( url_for_dmz ){
					ZnkStr_addf( ans, "href=\"http://%s/easter?est_get=%s\" %s>",
							xhr_dmz,
							url_for_dmz, is_target_blank ? "target=_blank" : "" );
				} else {
					ZnkStr_addf( ans, "href=\"/cgis/easter/%s\" %s>",
							unesc_url, is_target_blank ? "target=_blank" : "" );
				}
			} else {
				ZnkStr_addf( ans, "href=\"http://%s/cgis/easter/%s\" %s>",
						xhr_dmz,
						unesc_url, is_target_blank ? "target=_blank" : "" );
			}

			ZnkStr_addf( ans, "<img src='/cgis/easter/publicbox/icons/file.png'> %s</a>", elem_name );

			EstUIBase_getHtmlTitle( title, unesc_url, 10 );
			ZnkStr_addf( ans, " : [%s]", ZnkStr_cstr( title ) );

		} else if( ZnkS_eqCase( ZnkS_get_extension(unesc_url,'.'), "jpg" )
		        || ZnkS_eqCase( ZnkS_get_extension(unesc_url,'.'), "png" )
		        || ZnkS_eqCase( ZnkS_get_extension(unesc_url,'.'), "gif" )
		){
			renderOpen( ans, style_class_name, "img_viewer",
					unesc_url, authentic_key, is_target_blank );

			renderImgElem( ans, style_class_name,
					unesc_url, esc_url, elem_name,
					width, height,
					xhr_dmz, is_target_blank );

		} else if( ZnkS_eqCase( ZnkS_get_extension(unesc_url,'.'), "webm" )
		){
			renderOpen( ans, style_class_name, "video_viewer",
					unesc_url, authentic_key, is_target_blank );

			ZnkStr_add( ans, "<a " );

			if( style_class_name ){
				ZnkStr_addf( ans, "class=%s ", style_class_name );
			}
			ZnkStr_addf( ans, "href=\"http://%s/cgis/easter/%s\" %s>",
					xhr_dmz, esc_url, is_target_blank ? "target=_blank" : "" );

			ZnkStr_addf( ans, "<img src='/cgis/easter/publicbox/icons/file.png'> %s</a>", elem_name );


		} else {
			renderOpen( ans, style_class_name, "img_viewer",
					unesc_url, authentic_key, is_target_blank );

			ZnkStr_add( ans, "<a " );

			if( style_class_name ){
				ZnkStr_addf( ans, "class=%s ", style_class_name );
			}
			ZnkStr_addf( ans, "href=\"http://%s/cgis/easter/%s\" %s>",
					xhr_dmz, esc_url, is_target_blank ? "target=_blank" : "" );

			ZnkStr_addf( ans, "<img src='/cgis/easter/publicbox/icons/file.png'> %s</a>", elem_name );
		}

	}

}

static void*
getFSysElem( ZnkVarpAry list, size_t idx, const char** id, const char** url )
{
	ZnkVarp varp = ZnkVarpAry_at( list, idx );
	*id  = ZnkVar_name_cstr( varp );
	*url = ZnkVar_cstr( varp );
	return varp;
}

void
EstBoxUI_make_forFSysView( ZnkStr ans, ZnkVarpAry cinf_list,
		size_t begin_idx, size_t end_idx, const char* authentic_key,
		bool is_target_blank, bool is_checked )
{
	static const EstCInfViewFuncT    view_func     = viewFSysElem;
	static const EstCInfGetElemFuncT get_elem_func = getFSysElem;
	static const char* style_class_name = "MstyFInfElemLink";
	static const char* new_linkid       = "";
	static const char* url_prefix       = "/easter?est_manager=cache&amp;command=view&amp;cache_path=";
	static const char* id_prefix        = "";
	const size_t show_file_num = EstConfig_getShowFileNum();
	const size_t line_file_num = EstConfig_getLineFileNum();
	const size_t thumb_size    = EstConfig_getThumbSize();

	EstCInfListUI_make( ans,
			cinf_list, view_func, get_elem_func,
			begin_idx, end_idx,
			style_class_name, new_linkid, url_prefix, id_prefix,
			is_target_blank, show_file_num, line_file_num, thumb_size,
			authentic_key, is_checked );
}

static void
viewLinks( ZnkStr ans, void* inf_ptr,
		const char* url_prefix, const char* url, const char* unesc_url, const char* esc_url,
		const char* id, ZnkStr title,
		const char* style_class_name, bool is_target_blank,
		const char* authentic_key )
{
	EstLInf    linf = Znk_force_ptr_cast( EstLInf, inf_ptr );
	char hostname[ 256 ] = "";
	const char* q = RanoCGIUtil_skipProtocolPrefix( url );
	const char* r = Znk_strchr( q, '/' );
	const char* target = NULL;
	const char* comment = EstLInf_comment( linf );

	ZnkStr_add( ans, "<a " );
	if( style_class_name ){
		ZnkStr_addf( ans, "class=%s ", style_class_name );
	}

	ZnkS_copy( hostname, sizeof(hostname), q, r ? r-q : Znk_NPOS );
	target = EstConfig_findTargetName( hostname );
	if( target ){
		ZnkStr_addf( ans, "href=\"%s%s\" %s>",
				url_prefix, esc_url,
				is_target_blank ? "target=_blank" : "" );
		ZnkStr_add(  ans, " <font color=\"#404000\"> Open </font></a>" );
		ZnkStr_add(  ans, "<span class=MstyAutoLinkOther>" );
		ZnkStr_addf( ans, "<img src='/cgis/easter/publicbox/icons/%s_16.png'> ", target );

		if( ZnkS_empty(comment) ){
			ZnkStr_addf( ans, "%s%s", q > url ? "" : "http://", url );
		} else {
			ZnkStr_add( ans, comment );
		}
		ZnkStr_add(  ans, "</span>\n" );
	} else {
		/* 基本的に targetにないURLはEasterで閲覧すべきでなく、従ってEaster-URL変換すべきではない. */
		ZnkStr_addf( ans, "href=\"%s%s\" %s>",
				q > url ? "" : "http://", url,
				is_target_blank ? "target=_blank" : "" );
		ZnkStr_add(  ans, " <font color=\"#404000\"> Open </font></a>" );
		ZnkStr_add(  ans, "<span class=MstyAutoLinkOther>" );
		ZnkStr_add(  ans, "<img src='/cgis/easter/publicbox/icons/others_16.png'> " );

		if( ZnkS_empty(comment) ){
			ZnkStr_addf( ans, "%s%s", q > url ? "" : "http://", url );
		} else {
			ZnkStr_add( ans, comment );
		}
		ZnkStr_add(  ans, "</span>\n" );
	}

	//ZnkStr_delete( url_prefix );
}

static void*
getLinkElem( ZnkVarpAry list, size_t idx, const char** id, const char** url )
{
	EstLInf linf = EstLInfList_at( list, idx );
	*id  = EstLInf_id( linf );
	*url = EstLInf_url( linf );
	return linf;
}

void
EstBoxUI_make_forLinks( ZnkStr ans, ZnkVarpAry linf_list,
		size_t begin_idx, size_t end_idx, const char* authentic_key )
{
	static const EstCInfViewFuncT    view_func     = viewLinks;
	static const EstCInfGetElemFuncT get_elem_func = getLinkElem;
	static const char* style_class_name = "MstyFInfElemLink";
	static const char* new_linkid       = "";
	static const char* id_prefix        = "";
	static const bool  is_target_blank  = true;
	static const bool  is_checked       = false;
	const size_t show_file_num = EstConfig_getShowFileNum();
	const size_t line_file_num = 1;
	ZnkStr url_prefix = ZnkStr_new( "" );
	const char* xhr_dmz = EstConfig_XhrDMZ();
	const size_t elem_pix_width = 512;

	ZnkStr_setf( url_prefix, "http://%s/easter?est_view=", xhr_dmz );

	EstCInfListUI_make( ans,
			linf_list, view_func, get_elem_func,
			begin_idx, end_idx,
			style_class_name, new_linkid, ZnkStr_cstr(url_prefix), id_prefix,
			is_target_blank, show_file_num, line_file_num, elem_pix_width,
			authentic_key, is_checked );
	ZnkStr_delete( url_prefix );
}

