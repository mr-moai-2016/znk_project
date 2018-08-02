#include <Rano_dir_util.h>

#include <Znk_myf.h>
#include <Znk_s_base.h>
#include <Znk_stdc.h>
#include <Znk_mbc_jp.h>
#include <Znk_bfr_bif.h>
#include <Znk_bif.h>
#include <Znk_str_ex.h>
#include <Znk_dir.h>
#include <Znk_missing_libc.h>

#include <stdio.h>

struct Info {
	char* href_;
	char* name_;
	char* category_;
};

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

static void
drawHeader( ZnkStr ans, ZnkStrAry category_path, const char* urp, ZnkMyf menu_myf, const char* doc_title )
{
	ZnkStr_addf( ans, 
			"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n"
			"<html>\n"
			"<head>\n"
			"\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
			"\t<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n"
			"\t<meta http-equiv=\"Content-Style-Type\" content=\"text/css\">\n"
			"\t<link href=\"/msty.css\" rel=\"stylesheet\" type=\"text/css\">\n"
			"</head>\n"
	);

	ZnkStr_addf( ans,
			"<body>\n"
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
	
static void
drawConfig( ZnkStr ans, ZnkMyf in_myf, ZnkMyf menu_myf )
{
	ZnkVarpAry  global_config = ZnkMyf_find_vars( menu_myf, "config" );
	ZnkVarp     title_author_varp = ZnkVarpAry_findObj_byName_literal( global_config, "title_author", false );
	const char* title_author      = title_author_varp ? ZnkVar_cstr( title_author_varp ) : "Author";
	ZnkVarp     title_update_varp = ZnkVarpAry_findObj_byName_literal( global_config, "title_update", false );
	const char* title_update      = title_update_varp ? ZnkVar_cstr( title_update_varp ) : "LastUpdate";
	ZnkVarpAry  config = ZnkMyf_find_vars( in_myf, "config" );
	ZnkVarp     author_varp = ZnkVarpAry_findObj_byName_literal( config, "author", false );
	const char* author      = author_varp ? ZnkVar_cstr( author_varp ) : NULL;
	ZnkVarp     update_varp = ZnkVarpAry_findObj_byName_literal( config, "update", false );
	const char* update      = update_varp ? ZnkVar_cstr( update_varp ) : NULL;

	if( author || update ){
		ZnkStr_addf( ans,
				"<div class=MstyComment>\n"
				"<u><b>%s</b></u> : %s<br>\n"
				"<br>\n"
				"<u><b>%s</b></u> : %s<br>\n"
				"<br>\n"
				"</div>\n"
				"<br>\n",
				title_author, author ? author : "",
				title_update, update ? update : "" );
	}
	ZnkStr_add( ans, "\n\n" );
}

static bool
findInStrs( const char* begin, const char* end, ZnkStrAry strs )
{
	const size_t size = ZnkStrAry_size( strs );
	size_t       idx;
	if( begin > end ){
		return false;
	}
	for( idx=0; idx<size; ++idx ){
		const size_t str_leng = ZnkStrAry_at_leng( strs, idx );
		const char*  str      = ZnkStrAry_at_cstr( strs, idx );
		if( (size_t)(end-begin) == str_leng && ZnkS_eqCaseEx( begin, str, str_leng ) ){
			/* found */
			return true;
		}
	}
	return false;
}

static bool
isInTags( const char* tag_begin, ZnkStrAry nb_tags )
{
	const char* p       = tag_begin+1;
	const char* q       = Znk_strchr( tag_begin+1, ' ' );
	const char* tag_end = Znk_strchr( tag_begin+1, '>' );
	if( tag_end == NULL ){
		/* this is broken tag */
		return false;
	}
	if( *p == '/' ){ ++p; }
	if( q == NULL || q > tag_end ){
		q = tag_end;
		if( *(q-1) == '/' ){
			/* <xxx/>というタイプのタグ.
			 * 尚、ここでの条件ではtag_beginからqまでの間にはスペースが存在しないから
			 * 最後の / が (hrefなどでの)パスのdspとなっている可能性は除外できる. */
			--q;
		}
		if( p > q ){
			/* 万が一 </> というパターンが来た場合はこのような状況が起き得る */
			return false;
		}
	}
	return findInStrs( p, q, nb_tags );
}
static void
drawLine( ZnkStr ans, ZnkStrAry lines, size_t idx, ZnkStrAry nb_tags )
{
	bool with_br = true;
	ZnkStr line = ZnkStrAry_at( lines, idx );
	if( ZnkStr_last( line ) == '>' ){
		if( ZnkStr_isEnd( line, "<\\>" ) ){
			ZnkStr_cut_back( line, 3 );
			with_br = false;
		} else if( nb_tags ){
			char* tag_begin = ZnkStrEx_strrchr( line, '<' );
			if( tag_begin ){
				if( isInTags( tag_begin, nb_tags ) ){
					with_br = false;
				}
			}
		}
	}
	if( with_br ){
		ZnkStr_addf( ans, "%s<br>\n", ZnkStr_cstr( line ) );
	} else {
		ZnkStr_addf( ans, "%s\n", ZnkStr_cstr( line ) );
	}
}

static void
drawAtFirst( ZnkStr ans, ZnkMyf in_myf, ZnkMyf menu_myf )
{
	ZnkStrAry atfirst = ZnkMyf_find_lines( in_myf, "AtFirst" );
	if( atfirst ){
		ZnkVarpAry   global_config  = ZnkMyf_find_vars( menu_myf, "config" );
		ZnkVarp      sec_title_varp = ZnkVarpAry_findObj_byName_literal( global_config, "title_atfirst", false );
		const char*  sec_title      = sec_title_varp ? ZnkVar_cstr( sec_title_varp ) : "At First";
		ZnkVarpAry   config       = ZnkMyf_find_vars( in_myf, "config" );
		ZnkVarp      nb_tags_varp = ZnkVarpAry_findObj_byName_literal( config, "nb_tags", false );
		ZnkStrAry    nb_tags      = nb_tags_varp ? ZnkVar_str_ary( nb_tags_varp ) : NULL;
		const size_t size         = ZnkStrAry_size( atfirst );
		size_t       idx;

		ZnkStr_add( ans, "<a name=AtFirst></a>\n" );
		ZnkStr_add( ans, "<u><b>\n" );
		ZnkStr_addf( ans, "%s\n", sec_title );
		ZnkStr_add( ans, "</b></u><br>\n" );
		ZnkStr_add( ans, "<br>\n" );
		ZnkStr_add( ans, "<div class=MstyIndent>\n" );
		for( idx=0; idx<size; ++idx ){
			drawLine( ans, atfirst, idx, nb_tags );
		}
		ZnkStr_add( ans, "</div><br>\n" );
		ZnkStr_add( ans, "<br>\n" );
		ZnkStr_add( ans, "\n\n" );
	}
}
static void
drawIndex( ZnkStr ans, ZnkMyf in_myf, ZnkMyf menu_myf )
{
	ZnkPrimpAry index = ZnkMyf_find_prims( in_myf, "Index" );
	if( index ){
		ZnkVarpAry   global_config  = ZnkMyf_find_vars( menu_myf, "config" );
		ZnkVarp      sec_title_varp = ZnkVarpAry_findObj_byName_literal( global_config, "title_index", false );
		const char*  sec_title      = sec_title_varp ? ZnkVar_cstr( sec_title_varp ) : "Index";
		const size_t size = ZnkPrimpAry_size( index );
		size_t idx;
		ZnkStr_add( ans, "<a name=Index></a>\n" );
		ZnkStr_add( ans, "<u><b>\n" );
		ZnkStr_addf( ans, "%s\n", sec_title );
		ZnkStr_add( ans, "</b></u><br>\n" );
		ZnkStr_add( ans, "<br>\n" );
		ZnkStr_add( ans, "<ul>\n" );
		for( idx=0; idx<size; ++idx ){
			ZnkPrimp  prim = ZnkPrimpAry_at( index, idx );
			ZnkStrAry sary = ZnkPrim_strAry( prim );
			if( ZnkStrAry_size(sary) >= 2 ){
				const char* href  = ZnkStrAry_at_cstr( sary, 0 );
				const char* title = ZnkStrAry_at_cstr( sary, 1 );
				ZnkStr_addf( ans, "\t<li><b><a class=MstyElemLink href=%s>%s</a></b></li>\n", href, title );
			}
		}
		ZnkStr_add( ans, "</ul><br>\n" );
		ZnkStr_add( ans, "\n\n" );
	}
}

static void
drawSection( ZnkStr ans, ZnkMyf in_myf, ZnkMyf menu_myf, const char* sec_name, const char* sec_title, const char* title_category, bool index_hidden )
{
	ZnkStrAry sec = ZnkMyf_find_lines( in_myf, sec_name );
	if( sec ){
		ZnkVarpAry   global_config  = ZnkMyf_find_vars( menu_myf, "config" );
		ZnkVarp      msg_backto_index_varp = ZnkVarpAry_findObj_byName_literal( global_config, "msg_backto_index", false );
		const char*  msg_backto_index      = msg_backto_index_varp ? ZnkVar_cstr( msg_backto_index_varp ) : "Back to Index";
		ZnkVarpAry   config       = ZnkMyf_find_vars( in_myf, "config" );
		ZnkVarp      nb_tags_varp = ZnkVarpAry_findObj_byName_literal( config, "nb_tags", false );
		ZnkStrAry    nb_tags      = nb_tags_varp ? ZnkVar_str_ary( nb_tags_varp ) : NULL;
		const size_t size         = ZnkStrAry_size( sec );
		size_t       idx;

		ZnkStr_addf( ans, "<a name=%s></a>\n", sec_name );
		ZnkStr_add(  ans, "<u><b>\n" );
		ZnkStr_addf( ans, "%s\n", sec_title );
		ZnkStr_add( ans, "</b></u>" );
		if( title_category ){
			ZnkVarp     title_attr_fmt_varp = ZnkVarpAry_findObj_byName_literal( config, "title_attr_fmt", false );
			const char* title_attr_fmt      = title_attr_fmt_varp ? ZnkVar_cstr( title_attr_fmt_varp ) : NULL;
			if( title_attr_fmt ){
				ZnkStr_addf( ans, title_attr_fmt, title_category );
			}
		}
		ZnkStr_add( ans, "<br>\n" );
		ZnkStr_add( ans, "<br>\n" );
		ZnkStr_add( ans, "<div class=MstyIndent>\n" );
		for( idx=0; idx<size; ++idx ){
			drawLine( ans, sec, idx, nb_tags );
		}
		if( !index_hidden ){
			ZnkStr_add( ans, "\t<br>\n" );
			ZnkStr_addf( ans, "\t<b><a class=MstyElemLink href=#Index>%s</a></b><br>\n", msg_backto_index );
		}
		ZnkStr_add( ans, "</div><br>\n" );
		ZnkStr_add( ans, "<br>\n" );
		ZnkStr_add( ans, "\n\n" );
	}
}

static void
drawLink( ZnkStr ans, ZnkMyf in_myf, ZnkMyf menu_myf )
{
	ZnkPrimpAry link = ZnkMyf_find_prims( in_myf, "Link" );
	if( link ){
		ZnkVarpAry   global_config  = ZnkMyf_find_vars( menu_myf, "config" );
		ZnkVarp      sec_title_varp = ZnkVarpAry_findObj_byName_literal( global_config, "title_link", false );
		const char*  sec_title      = sec_title_varp ? ZnkVar_cstr( sec_title_varp ) : "Link";
		const size_t size = ZnkPrimpAry_size( link );
		size_t idx;
		ZnkStr_add( ans, "<a name=Link></a>\n" );
		ZnkStr_add( ans, "<u><b>\n" );
		ZnkStr_addf( ans, "%s\n", sec_title );
		ZnkStr_add( ans, "</b></u><br>\n" );
		ZnkStr_add( ans, "<br>\n" );
		ZnkStr_add( ans, "<ul>\n" );
		for( idx=0; idx<size; ++idx ){
			ZnkPrimp  prim = ZnkPrimpAry_at( link, idx );
			ZnkStrAry sary = ZnkPrim_strAry( prim );
			if( ZnkStrAry_size(sary) >= 2 ){
				const char* href  = ZnkStrAry_at_cstr( sary, 0 );
				const char* title = ZnkStrAry_at_cstr( sary, 1 );
				ZnkStr_addf( ans, "\t<li><a class=MstyElemLink href=%s>%s</a></li>\n", href, title );
			}
		}
		ZnkStr_add( ans, "</ul><br>\n" );
		ZnkStr_add( ans, "\n\n" );
	}
}

static void
convertSJIStoUTF8( ZnkStr ans )
{
	struct ZnkBif_tag bif_instance = { 0 };
	struct ZnkBif_tag tmp_instance = { 0 };
	ZnkErr_D( err );
	ZnkBif bif = &bif_instance;
	ZnkBif tmp = &tmp_instance;
	ZnkBfrBif_create( bif );
	ZnkBfrBif_create( tmp );
	ZnkBif_append( bif, (uint8_t*)ZnkStr_cstr(ans), ZnkStr_leng(ans) );
	ZnkMbc_convert_self( bif, ZnkMbcType_SJIS, ZnkMbcType_UTF8, tmp, &err );
	ZnkStr_assign( ans, 0, (char*)ZnkBif_data(bif), ZnkBif_size(bif) );
	ZnkBfrBif_destroy( bif );
	ZnkBfrBif_destroy( tmp );
}

static void
makeOne( ZnkStr ans, ZnkMyf in_myf, ZnkMyf menu_myf, const char* in_file_path, const char* out_file_path )
{
	bool index_hidden = false;

	ZnkStr_clear( ans );
	ZnkMyf_clear( in_myf );
	
	ZnkMyf_load( in_myf, in_file_path );

	{
		ZnkVarpAry  config = ZnkMyf_find_vars( in_myf, "config" );
		ZnkVarp     category_path_varp = ZnkVarpAry_findObj_byName_literal( config, "category_path", false );
		ZnkStrAry   category_path      = category_path_varp ? ZnkVar_str_ary( category_path_varp ) : NULL;
		ZnkVarp     urp_varp           = ZnkVarpAry_findObj_byName_literal( config, "urp", false );
		const char* urp                = urp_varp           ? ZnkVar_cstr( urp_varp ) : NULL;
		ZnkVarp     doc_title_varp     = ZnkVarpAry_findObj_byName_literal( config, "doc_title", false );
		const char* doc_title          = doc_title_varp     ? ZnkVar_cstr( doc_title_varp ) : NULL;
		ZnkVarp     index_hidden_varp  = ZnkVarpAry_findObj_byName_literal( config, "index_hidden", false );
		index_hidden = index_hidden_varp ? ZnkS_eq( ZnkVar_cstr( index_hidden_varp ), "true" ) : false;
		drawHeader( ans, category_path, urp, menu_myf, doc_title );
	}

	drawConfig( ans, in_myf, menu_myf );
	drawAtFirst( ans, in_myf, menu_myf );

	if( !index_hidden ){
		drawIndex( ans, in_myf, menu_myf );
	}

	{
		ZnkPrimpAry index = ZnkMyf_find_prims( in_myf, "Index" );
		if( index ){
			const size_t size = ZnkPrimpAry_size( index );
			size_t idx;
			for( idx=0; idx<size; ++idx ){
				ZnkPrimp  prim = ZnkPrimpAry_at( index, idx );
				ZnkStrAry sary = ZnkPrim_strAry( prim );
				if( ZnkStrAry_size(sary) >= 2 ){
					const char* href  = ZnkStrAry_at_cstr( sary, 0 );
					const char* title = ZnkStrAry_at_cstr( sary, 1 );
					const char* title_category = NULL;
					if( ZnkStrAry_size(sary) >= 3 ){
						title_category = ZnkStrAry_at_cstr( sary, 2 );
					}
					if( href[ 0 ] == '#' ){
						const char* sec_name = href + 1;
						drawSection( ans, in_myf, menu_myf, sec_name, title, title_category, index_hidden );
					}
				}
			}
		}
	}
	drawLink( ans, in_myf, menu_myf );
	ZnkStr_add( ans, "</body>\n</html>" );

	//convertSJIStoUTF8( ans );
	{
		ZnkFile fp = Znk_fopen( out_file_path, "wb" );
		if( fp ){
			Znk_fputs( ZnkStr_cstr(ans), fp );
			Znk_fclose( fp );
		}
	}
}



struct BufferInfo {
	ZnkStr ans_;
	ZnkMyf in_myf_;
	ZnkMyf menu_myf_;
};

static bool
filterFile( const char* src_file_path, const char* dst_file_path, void* arg )
{
	if( ZnkS_eq( ZnkS_get_extension( dst_file_path, '.' ), "in" ) ){
		/* .in を除去 */
		char out_file_path[ 256 ] = "";
		struct BufferInfo* info = Znk_force_ptr_cast( struct BufferInfo*, arg );
		ZnkStr ans      = info->ans_;
		ZnkMyf in_myf   = info->in_myf_;
		ZnkMyf menu_myf = info->menu_myf_;
		ZnkS_copy( out_file_path, sizeof(out_file_path), dst_file_path, Znk_strlen( dst_file_path ) - 3 );
		makeOne( ans, in_myf, menu_myf, src_file_path, out_file_path );
		return true;
	}
	return false;
}

int main( int argc, char** argv )
{
	ZnkMyf in_myf = ZnkMyf_create();
	ZnkMyf menu_myf = ZnkMyf_create();
	ZnkStr ans = ZnkStr_new( "" );
	ZnkStr ermsg = ZnkStr_new( "" );
	const char* in_dir  = "doc_in";
	const char* out_dir = "doc_out";
	struct BufferInfo info = { 0 };

	if( argc >= 2 ){
		in_dir = argv[ 1 ];
	}
	if( argc >= 3 ){
		out_dir = argv[ 2 ];
	}

	info.ans_      = ans;
	info.in_myf_   = in_myf;
	info.menu_myf_ = menu_myf;

	ZnkDir_mkdirPath( out_dir, Znk_NPOS, '/', NULL );

	{
		char menu_myf_path[ 256 ];
		Znk_snprintf( menu_myf_path, sizeof(menu_myf_path), "%s/menu.myf", in_dir );
		if( !ZnkMyf_load( menu_myf, menu_myf_path ) ){
			Znk_printf_e( "docgen Error : Cannot load [%s].\n", menu_myf_path );
			Znk_getchar();
			goto FUNC_END;
		}
	}

	RanoDirUtil_filterDir( in_dir, out_dir,
			"docgen", ermsg,
			NULL, NULL,
			filterFile, &info );

FUNC_END:
	ZnkStr_delete( ermsg );
	ZnkStr_delete( ans );
	ZnkMyf_destroy( menu_myf );
	ZnkMyf_destroy( in_myf );
	return 0;
}
