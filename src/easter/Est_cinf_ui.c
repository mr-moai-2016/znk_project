#include <Est_cinf_ui.h>
#include <Est_base.h>

#include <Znk_htp_util.h>
#include <Znk_mem_find.h>

void
EstCInfListUI_make( ZnkStr ans, ZnkVarpAry cinf_list,
		EstCInfViewFuncT view_func, EstCInfGetElemFuncT get_elem_func,
		size_t begin_idx, size_t end_idx,
		const char* style_class_name, const char* new_linkid, const char* url_prefix, const char* id_prefix,
		bool is_target_blank, size_t show_file_num, size_t line_file_num, size_t elem_pix_width,
		const char* authentic_key, bool is_checked )
{
	const size_t size = ZnkVarpAry_size( cinf_list );
	if( end_idx > size ){
		end_idx = size;
	}
	if( begin_idx < end_idx ){
		size_t idx;
		ZnkStr esc_url = ZnkStr_new( "" );
		ZnkStr unesc_url = ZnkStr_new( "" );
		ZnkStr title = ZnkStr_new( "Unknown" );
		ZnkStr line = ZnkStr_new( "" );

		ZnkSRef sref_title_beg = { 0 };
		ZnkSRef sref_title_end = { 0 };
		size_t  occ_title_beg[ 256 ] = { 0 };
		size_t  occ_title_end[ 256 ] = { 0 };
		ZnkSRef_set_literal( &sref_title_beg, "<title>" );
		ZnkSRef_set_literal( &sref_title_end, "</title>" );
		ZnkMem_getLOccTable_forBMS( occ_title_beg, (uint8_t*)sref_title_beg.cstr_,  sref_title_beg.leng_ );
		ZnkMem_getLOccTable_forBMS( occ_title_end, (uint8_t*)sref_title_end.cstr_,  sref_title_end.leng_ );
	
		ZnkStr_add( ans, "\n" );

		ZnkStr_add( ans, "<table border=0>\n" );
		ZnkStr_add( ans, "<tr>\n" );

		for( idx=begin_idx; idx<end_idx; ++idx ){
			void* elem_ptr  = NULL;
			const char* id  = NULL;
			const char* url = NULL;

			elem_ptr = get_elem_func( cinf_list, idx, &id, &url );

			if( url == NULL ){
				continue;
			}

			ZnkStr_set( esc_url,   url );
			ZnkStr_set( unesc_url, url );
			EstBase_unescape_forMbcFSysPath( unesc_url );
			EstBase_escape_forURL( esc_url, 1 );
			ZnkHtpURL_negateHtmlTagEffection( unesc_url ); /* for XSS */
			{
				const char* val = url;
				const char* checked = is_checked ? "checked" : "";
				ZnkStr_add( ans, "<td style=vertical-align:top>" );
				ZnkStr_addf( ans, "<input type=checkbox id=%s%s name=%s%s value=\"%s\" %s>", id_prefix, id, id_prefix, id, val, checked );
				ZnkStr_add( ans, "</td>" );
				/* width‚ğ +32 ‚È‚Ç‚Æ‰ÁZ‚µ‚È‚¢•û‚ª‚æ‚¢(Chrome‚ÅƒuƒŒŒ»Û‚ª”­¶‚·‚é) */
				ZnkStr_addf( ans, "<td width=\"%zu\" valign=top style=word-break:break-all>", elem_pix_width );

				view_func( ans, elem_ptr,
						url_prefix, url, ZnkStr_cstr(unesc_url), ZnkStr_cstr(esc_url),
						id, title,
						style_class_name, is_target_blank,
						authentic_key );

				if( ZnkS_eq( id, new_linkid ) ){
					ZnkStr_addf( ans, "<font color=red> New!</font>" );
				}
			}

			ZnkStr_add( ans, "</td>\n" );
			if( idx != end_idx-1 ){
				if( ((idx+1)-begin_idx) % line_file_num == 0 ){
					ZnkStr_add( ans, "</tr>\n<tr>\n" );
				}
			}
			ZnkStr_add( ans, "\n" );
		}

		ZnkStr_add( ans, "</tr>\n" );
		ZnkStr_add( ans, "</table>\n" );

		ZnkStr_add( ans, "\n" );
		ZnkStr_delete( esc_url );
		ZnkStr_delete( unesc_url );
		ZnkStr_delete( line );
		ZnkStr_delete( title );
	}
}

