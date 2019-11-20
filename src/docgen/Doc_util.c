#include "Doc_util.h"

#include <Znk_myf.h>
#include <Znk_s_base.h>
#include <Znk_str_ex.h>

static void
drawMenuBar2( ZnkStr ans, const char* query_urp, ZnkPrimpAry tbl, const char* query_category )
{
	const size_t size = ZnkPrimpAry_size( tbl );
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		ZnkPrimp  prim = ZnkPrimpAry_at( tbl, idx );
		ZnkStrAry sary = ZnkPrim_strAry( prim );
		if( ZnkStrAry_size(sary) >= 2 ){
			const char* href     = ZnkStrAry_at_cstr( sary, 0 );
			const char* title    = ZnkStrAry_at_cstr( sary, 1 );
			const char* category = ( ZnkStrAry_size(sary) >= 3 ) ? ZnkStrAry_at_cstr( sary, 2 ) : NULL;
			bool selected = query_category ?
				( category ? ZnkS_eq( category, query_category ) : false )
				: (bool)( query_urp && ZnkS_eq( href, query_urp ) );
			const char* style = selected ? "MstyNowSelectedLink" : "MstyElemLink";
			ZnkStr_addf( ans, "<a class=%s href=\"%s\">%s</a> &nbsp;\n",
					style, href, title );
		}
	}
}

void
DocUtil_drawHeader( ZnkStr ans, ZnkStrAry category_path, const char* urp, ZnkMyf menu_myf, const char* doc_title )
{
	ZnkStr_addf( ans, 
			//"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n"
			"<!DOCTYPE html>"
			"<html>\n"
			"<head>\n"
			"\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
			"\t<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n"
			"\t<meta http-equiv=\"Content-Style-Type\" content=\"text/css\">\n"
			"\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
			"\t<link href=\"/msty.css\" rel=\"stylesheet\" type=\"text/css\">\n"
			"\t<link href=\"/bulma.css\" rel=\"stylesheet\" type=\"text/css\">\n"
	);
	/* html_head */
	{
		ZnkStrAry html_head = ZnkMyf_find_lines( menu_myf, "html_head" );
		if( html_head ){
			ZnkStrEx_addJoin( ans, html_head,
					0, Znk_NPOS,
					"\n", 1, 16 );
			ZnkStr_add_c( ans, '\n' );
		}
	}
	ZnkStr_add( ans, "</head>\n" );

	ZnkStr_addf( ans,
			"<body>\n"
			"<div class=\"section\">\n"
			"<div class=\"container\">\n"
			"<div class=\"content\">\n"

			"<a name=TopBar></a>\n"
			"<p><b><img src=\"/imgs/here_landmark.png\"> %s</b></p>\n\n",
			doc_title ? doc_title : ""
	);

	/* root */
	{
		ZnkPrimpAry root = ZnkMyf_find_prims( menu_myf, "root" );
		drawMenuBar2( ans, urp, root,
				( category_path && ZnkStrAry_size(category_path) > 0 ) ? ZnkStrAry_at_cstr(category_path,0) : NULL );
	}
	if( category_path ){
		const size_t category_path_size = ZnkStrAry_size(category_path);
		size_t category_path_idx;
		for( category_path_idx=0; category_path_idx<category_path_size; ++category_path_idx ){
			const char* category = ZnkStrAry_at_cstr( category_path, category_path_idx );
			ZnkPrimpAry cat_tbl = ZnkMyf_find_prims( menu_myf, category );
			if( cat_tbl ){
				ZnkStr_addf( ans, "<br> <br>\n" );
				drawMenuBar2( ans, urp, cat_tbl,
						ZnkStrAry_size( category_path ) > category_path_idx + 1 ?
						ZnkStrAry_at_cstr( category_path, category_path_idx + 1 ) : NULL );
			}
		}
	}
	ZnkStr_addf( ans, "<br> <br>\n\n" );
}
	
void
DocUtil_drawEnd( ZnkStr ans )
{
	ZnkStr_add( ans, "</div>\n" );        /* content */
	ZnkStr_add( ans, "</div>\n</div>\n" ); /* section, container */
	ZnkStr_add( ans, "</body>\n</html>" );
}
