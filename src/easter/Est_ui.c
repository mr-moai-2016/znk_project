#include <Est_ui.h>
#include <Est_ui_base.h>
#include <Est_config.h>
#include <Est_assort.h>
#include <Est_base.h>
#include <Est_box_ui.h>

#include <Znk_htp_util.h>
#include <Znk_dir.h>
#include <Znk_missing_libc.h>
#include <Znk_mem_find.h>
#include <Znk_str_ex.h>
#include <Znk_str_fio.h>


static bool
getHtmlBetweenPtn( ZnkStr ans, const char* filename, size_t max_count,
		const char* ptn_beg, size_t ptn_beg_leng,
		const char* ptn_end, size_t ptn_end_leng )
{
	bool result = false;
	ZnkStr fsys_path = ZnkStr_new( "" );
	EstBoxBase_convertBoxDir_toFSysDir( fsys_path, filename, NULL, NULL );
	{
		ZnkFile fp = Znk_fopen( ZnkStr_cstr(fsys_path), "rb" );
	
		size_t  occ_ptn_beg[ 256 ] = { 0 };
		size_t  occ_ptn_end[ 256 ] = { 0 };
	
		ZnkMem_getLOccTable_forBMS( occ_ptn_beg, (uint8_t*)ptn_beg,  ptn_beg_leng );
		ZnkMem_getLOccTable_forBMS( occ_ptn_end, (uint8_t*)ptn_end,  ptn_end_leng );
	
		if( fp ){
			ZnkStr line = ZnkStr_new( "" );
			size_t begp = Znk_NPOS;
			size_t sizp = Znk_NPOS;
			size_t count = 0;
	
			while( true ){
				if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
					/* eof */
					break;
				}
				if( count > max_count ){
					break;
				}
				ZnkStr_chompNL( line );
	
				begp = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(line), ZnkStr_leng(line),
						(uint8_t*)ptn_beg, ptn_beg_leng, 1, occ_ptn_beg );
				if( begp != Znk_NPOS ){
					begp += ptn_beg_leng;
					sizp = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(line)+begp, ZnkStr_leng(line)-begp,
							(uint8_t*)ptn_end, ptn_end_leng, 1, occ_ptn_end );
					ZnkStr_append( ans, ZnkStr_cstr(line)+begp, sizp );
					break;
				}
	
				++count;
			}
	
			Znk_fclose( fp );
	
			if( begp == Znk_NPOS ){
				result = false;
			} else {
				result = true;
			}
			ZnkHtpURL_negateHtmlTagEffection( ans ); /* for XSS */
			ZnkStr_delete( line );
		}
	}
	ZnkStr_delete( fsys_path );
	return result;
}

void
EstUI_getSummary( ZnkBird bird, const char* file_path )
{
	ZnkStr summary = ZnkStr_new( "" );
	ZnkSRef old_ptn = { 0 };
	ZnkSRef new_ptn = { 0 };
	ZnkSRef_set_literal( &old_ptn, "&lt;br&gt;" );
	ZnkSRef_set_literal( &new_ptn, "<br>" );

	EstUIBase_getHtmlTitle( summary, file_path, 10 );

	ZnkStr_add( summary, "<br> <br>" );

	getHtmlBetweenPtn( summary, file_path, Znk_NPOS,
			"<blockquote>", 12,
			"</blockquote>", 13 );

	ZnkStrEx_replace_BF( summary, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 

	ZnkBird_regist( bird, "summary",  ZnkStr_cstr(summary) );
	ZnkStr_delete( summary );
}

static void
drawLinkButton2( ZnkStr caches, const char* query_string_base,
		size_t cache_begin_idx, size_t cache_end_idx,
		const char* authentic_key, const char* title,
		const char* anchor )
{
	ZnkStr_addf( caches, "<a class=MstyElemLink href='/easter?"
			"%s"
			"&amp;est_cache_begin=%zu"
			"&amp;est_cache_end=%zu"
			"&amp;Moai_AuthenticKey=%s"
			"%s'>%s</a>\n",
			query_string_base,
			cache_begin_idx, cache_end_idx,
			authentic_key,
			anchor, title );
}

static void
showPageSwitcher_ver2( ZnkStr result_ui, const char* query_string_base,
		size_t cache_file_list_size, size_t show_file_num, size_t begin_idx,
		const char* authentic_key, const char* page_switcher_name )
{
	char anchor[ 256 ] = "#CacheViewer";
	if( page_switcher_name ){
	 	Znk_snprintf( anchor, sizeof(anchor), "#%s", page_switcher_name );
	}
	if( cache_file_list_size ){
		char title[ 256 ] = "";
		size_t mid_idx = cache_file_list_size / 2;
		size_t idx;
		const size_t max_width  = EstConfig_getPreviewMaxWidth();
		const size_t max_height = EstConfig_getPreviewMaxHeight();

		if( page_switcher_name ){
			ZnkStr_addf( result_ui, "<a name=\"%s\"></a>\n", page_switcher_name );
		}
		ZnkStr_addf( result_ui, "<div class=MstyAutoLinkOther>\n" );
		if( page_switcher_name ){
			ZnkStr_addf( result_ui, "<a class=MstyElemLink href='#PageSwitcher'>&para;</a>\n" );
		}
		ZnkStr_addf( result_ui, "ページの移動 : 全%zu個のファイル\n", cache_file_list_size );
		mid_idx = mid_idx / show_file_num * show_file_num;
		ZnkStr_addf( result_ui, "<br> \n" );

	
		/* 前のN件 */
		Znk_snprintf( title, sizeof(title), "前<img src='/cgis/easter/publicbox/icons/prev_16.png'>" );
		if( begin_idx > 0 ){
			const size_t prev_idx = ( begin_idx < show_file_num ) ? 0 : begin_idx-show_file_num;
			drawLinkButton2( result_ui, query_string_base,
					prev_idx, prev_idx+show_file_num,
					authentic_key, title, anchor );
		} else {
			ZnkStr_addf( result_ui, "<span class=MstyElemLink><font color=#888888>%s</font></span>\n", title );
		}
		/* 次のN件 */
		Znk_snprintf( title, sizeof(title), "次<img src='/cgis/easter/publicbox/icons/next_16.png'>" );
		if( begin_idx+show_file_num < cache_file_list_size ){
			const size_t next_idx = begin_idx + show_file_num;
			drawLinkButton2( result_ui, query_string_base,
					next_idx, next_idx+show_file_num,
					authentic_key, title, anchor );
		} else {
			ZnkStr_addf( result_ui, "<span class=MstyElemLink><font color=#888888>%s</font></span>\n", title );
		}

		ZnkStr_addf( result_ui, "&nbsp;&nbsp;" );
		/* 最初のN件 */
		Znk_snprintf( title, sizeof(title), "最初" );
		if( begin_idx > 0 ){
			const size_t prev_idx = 0;
			drawLinkButton2( result_ui, query_string_base,
					prev_idx, prev_idx+show_file_num,
					authentic_key, title, anchor );
		} else {
			ZnkStr_addf( result_ui, "<span class=MstyElemLink><font color=#888888>%s</font></span>\n", title );
		}
#if 0
		/* 中央のN件 */
		Znk_snprintf( title, sizeof(title), "中央" );
		if( begin_idx < mid_idx || begin_idx > mid_idx + show_file_num ){
			drawLinkButton2( result_ui, query_string_base,
					mid_idx, mid_idx+show_file_num,
					authentic_key, title, anchor );
		} else {
			ZnkStr_addf( result_ui, "<span class=MstyElemLink><font color=#888888>%s</font></span>\n", title );
		}
#endif
		/* 最後のN件 */
		Znk_snprintf( title, sizeof(title), "最後" );
		if( begin_idx+show_file_num < cache_file_list_size ){
			const size_t next_idx = ( cache_file_list_size - 1 ) / show_file_num * show_file_num;
			drawLinkButton2( result_ui, query_string_base,
					next_idx, next_idx+show_file_num,
					authentic_key, title, anchor );
		} else {
			ZnkStr_addf( result_ui, "<span class=MstyElemLink><font color=#888888>%s</font></span>\n", title );
		}
		ZnkStr_addf( result_ui, "&nbsp;&nbsp;" );

		for( idx=4; idx>0; --idx ){
			if( begin_idx >= show_file_num*idx ){
				const size_t prev_idx = ( begin_idx < show_file_num*idx ) ? 0 : begin_idx-show_file_num*idx;
				Znk_snprintf( title, sizeof(title), "%zu", begin_idx-show_file_num*idx );
				drawLinkButton2( result_ui, query_string_base,
						prev_idx, prev_idx+show_file_num,
						authentic_key, title, anchor );
			}
		}
		ZnkStr_addf( result_ui, "<span class=MstyElemLink><font color=#888888>%zu</font></span>\n", begin_idx );
		for( idx=1; idx<1+4; ++idx ){
			if( begin_idx+show_file_num*idx < cache_file_list_size ){
				const size_t next_idx = begin_idx + show_file_num*idx;
				Znk_snprintf( title, sizeof(title), "%zu", begin_idx+show_file_num*idx );
				drawLinkButton2( result_ui, query_string_base,
						next_idx, next_idx+show_file_num,
						authentic_key, title, anchor );
			}
		}
		ZnkStr_addf( result_ui, "件目へ移動\n" );
		ZnkStr_addf( result_ui, "</div>\n" );

		ZnkStr_addf( result_ui, "<br>" );
		if( page_switcher_name ){
			ZnkStr_addf( result_ui, "<a name=\"%s_2\"></a>\n", page_switcher_name );
		}
		ZnkStr_addf( result_ui, "<div class=MstyAutoLinkOther>\n" );
		if( page_switcher_name ){
			ZnkStr_addf( result_ui, "<a class=MstyElemLink href='#PageSwitcher_2'>&para;</a>\n" );
		}
		{
			ZnkStr_addf( result_ui,
					"&nbsp;&nbsp;<a class=MstyElemLink href='javascript:void(0);' "
					"onclick=\"Easter_showPrev( this, %zu, %zu );\">"
					"プレビュー前<img src='/cgis/easter/publicbox/icons/prev_16.png'></a> \n",
					max_width, max_height );
			ZnkStr_addf( result_ui,
					"&nbsp;&nbsp;<a class=MstyElemLink href='javascript:void(0);' "
					"onclick=\"Easter_showNext( this, %zu, %zu );\">"
					"プレビュー次<img src='/cgis/easter/publicbox/icons/next_16.png'></a> ",
					max_width, max_height );
		}
		ZnkStr_addf( result_ui, "</div>\n" );
		ZnkStr_addf( result_ui, "<br>" );
	}
}

void
EstUI_showPageSwitcher( ZnkStr result_ui, const char* query_string_base,
		size_t cache_file_list_size, size_t show_file_num, size_t begin_idx,
		const char* authentic_key, const char* page_switcher_name )
{
	showPageSwitcher_ver2( result_ui, query_string_base,
			cache_file_list_size, show_file_num, begin_idx,
			authentic_key, page_switcher_name );
}

void
EstUI_makeCheckedConfirmView( RanoCGIEVar* evar, ZnkVarpAry post_vars,
		ZnkStr caches, size_t show_file_num, const char* authentic_key,
		const char* assort_msg )
{
	const size_t   post_vars_size = ZnkVarpAry_size( post_vars );
	const ZnkVarp* post_vars_ptr  = ZnkVarpAry_ary_ptr( post_vars );
	ZnkVarp        post_varp = NULL;
	const char*    path = NULL;
	size_t idx;
	ZnkStr unesc_path = ZnkStr_new( "" );
	ZnkVarpAry cache_dir_list  = ZnkVarpAry_create( true );
	ZnkVarpAry cache_file_list = ZnkVarpAry_create( true );
	bool is_checked = true;

	for( idx=0; idx<post_vars_size; ++idx ){
		post_varp = post_vars_ptr[ idx ];
		ZnkStr_clear( unesc_path );
		if( ZnkS_isBegin( ZnkVar_name_cstr( post_varp ), "cache_dir" )){
			/* dir : cache_dir */
			path = ZnkVar_cstr( post_varp );
			ZnkHtpURL_unescape_toStr( unesc_path, path, Znk_strlen(path) );
			ZnkHtpURL_sanitizeReqUrpDir( unesc_path, true );

			{
				ZnkVarp new_elem = ZnkVarp_create( ZnkVar_name_cstr( post_varp ), "", 0, ZnkPrim_e_Str, NULL );
				const char* vpath = ZnkStr_cstr( unesc_path );
				ZnkVar_set_val_Str( new_elem, vpath, Znk_strlen(vpath) );
				ZnkVarpAry_push_bk( cache_dir_list, new_elem );
			}

		} else if( ZnkS_isBegin( ZnkVar_name_cstr( post_varp ), "cache_file" )){
			/* file : cache_file */
			path = ZnkVar_cstr( post_varp );
			ZnkHtpURL_unescape_toStr( unesc_path, path, Znk_strlen(path) );
			ZnkHtpURL_sanitizeReqUrpDir( unesc_path, true );

			{
				ZnkVarp new_elem = ZnkVarp_create( ZnkVar_name_cstr( post_varp ), "", 0, ZnkPrim_e_Str, NULL );
				const char* vpath = ZnkStr_cstr( unesc_path );
				ZnkVar_set_val_Str( new_elem, vpath, Znk_strlen(vpath) );
				ZnkVarpAry_push_bk( cache_file_list, new_elem );
			}

		} else if( ZnkS_eq( ZnkVar_name_cstr( post_varp ), "cache_path" )){
			/* file : cache_path */
			path = ZnkVar_cstr( post_varp );
			ZnkHtpURL_unescape_toStr( unesc_path, path, Znk_strlen(path) );
			ZnkHtpURL_sanitizeReqUrpDir( unesc_path, true );

			{
				ZnkVarp new_elem = ZnkVarp_create( ZnkVar_name_cstr( post_varp ), "", 0, ZnkPrim_e_Str, NULL );
				const char* vpath = ZnkStr_cstr( unesc_path );
				ZnkVar_set_val_Str( new_elem, vpath, Znk_strlen(vpath) );
				ZnkVarpAry_push_bk( cache_file_list, new_elem );
			}

		}
	}

	/* file_list */
	ZnkStr_add( caches, "<table><tr><td valign=top>\n" );
	EstBoxUI_make_forFSysView( caches, cache_file_list,
			0, Znk_NPOS, authentic_key, true, is_checked );
	ZnkStr_addf( caches, "</td><td valign=top>%s<span id=preview></span></td></tr></table>\n", assort_msg );

	/* dir_list */
	if( ZnkVarpAry_size( cache_dir_list ) && ZnkVarpAry_size( cache_file_list ) ){
		ZnkStr_addf( caches, "<hr>" );
	}
	EstBoxUI_make_forFSysView( caches, cache_dir_list,
			0, Znk_NPOS, authentic_key, false, is_checked );

	ZnkStr_delete( unesc_path );
	ZnkVarpAry_destroy( cache_dir_list );
	ZnkVarpAry_destroy( cache_file_list );

}

void
EstUI_makeAssortedReportView( RanoCGIEVar* evar, ZnkStrAry dst_vpath_list,
		ZnkStr caches, size_t show_file_num, const char* authentic_key,
		const char* assort_msg )
{
	const size_t dst_vpath_list_size = ZnkStrAry_size( dst_vpath_list );
	size_t     idx;
	ZnkStr     unesc_path = ZnkStr_new( "" );
	ZnkVarpAry cache_dir_list  = ZnkVarpAry_create( true );
	ZnkVarpAry cache_file_list = ZnkVarpAry_create( true );
	bool       is_checked = true;

	for( idx=0; idx<dst_vpath_list_size; ++idx ){
		const char* vpath = ZnkStrAry_at_cstr( dst_vpath_list, idx );
		char fileid[ 256 ] = "";
		Znk_snprintf( fileid, sizeof(fileid), "cache_file_%zu", idx );
		{
			ZnkVarp new_elem = ZnkVarp_create( fileid, "", 0, ZnkPrim_e_Str, NULL );
			ZnkVar_set_val_Str( new_elem, vpath, Znk_strlen(vpath) );
			ZnkVarpAry_push_bk( cache_file_list, new_elem );
		}
	}

	/* file_list */
	ZnkStr_add( caches, "<table><tr><td valign=top>\n" );
	EstBoxUI_make_forFSysView( caches, cache_file_list,
			0, Znk_NPOS, authentic_key, true, is_checked );
	ZnkStr_addf( caches, "</td><td valign=top><span id=preview></span></td></tr></table>\n" );

	ZnkStr_addf( caches, "%s\n", assort_msg );

	/* dir_list */
	if( ZnkVarpAry_size( cache_dir_list ) && ZnkVarpAry_size( cache_file_list ) ){
		ZnkStr_addf( caches, "<hr>" );
	}
	EstBoxUI_make_forFSysView( caches, cache_dir_list,
			0, Znk_NPOS, authentic_key, false, is_checked );

	ZnkStr_delete( unesc_path );
	ZnkVarpAry_destroy( cache_dir_list );
	ZnkVarpAry_destroy( cache_file_list );

}
