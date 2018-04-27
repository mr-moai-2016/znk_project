#include <Est_ui_base.h>
#include <Est_box_base.h>

#include <Rano_html_ui.h>

#include <Znk_s_base.h>
#include <Znk_mem_find.h>
#include <Znk_str_fio.h>
#include <Znk_htp_util.h>

bool
EstUIBase_getHtmlTitle( ZnkStr title, const char* filename, size_t max_count )
{
	bool result = false;
	ZnkStr fsys_path = ZnkStr_new( "" );
	EstBoxBase_convertBoxDir_toFSysDir( fsys_path, filename, NULL, NULL );
	{
		ZnkFile fp = Znk_fopen( ZnkStr_cstr(fsys_path), "rb" );
		if( fp == NULL ){
			ZnkStr_setf( title, "Cannot open file %s", filename );
			goto FUNC_END;
		} else {
			ZnkSRef sref_title_beg = { 0 };
			ZnkSRef sref_title_end = { 0 };
			size_t  occ_title_beg[ 256 ] = { 0 };
			size_t  occ_title_end[ 256 ] = { 0 };
		
			ZnkSRef sref_TITLE_beg = { 0 };
			ZnkSRef sref_TITLE_end = { 0 };
			size_t  occ_TITLE_beg[ 256 ] = { 0 };
			size_t  occ_TITLE_end[ 256 ] = { 0 };
		
			ZnkSRef_set_literal( &sref_title_beg, "<title>" );
			ZnkSRef_set_literal( &sref_title_end, "</title>" );
			ZnkMem_getLOccTable_forBMS( occ_title_beg, (uint8_t*)sref_title_beg.cstr_,  sref_title_beg.leng_ );
			ZnkMem_getLOccTable_forBMS( occ_title_end, (uint8_t*)sref_title_end.cstr_,  sref_title_end.leng_ );
		
			ZnkSRef_set_literal( &sref_TITLE_beg, "<TITLE>" );
			ZnkSRef_set_literal( &sref_TITLE_end, "</TITLE>" );
			ZnkMem_getLOccTable_forBMS( occ_TITLE_beg, (uint8_t*)sref_TITLE_beg.cstr_,  sref_TITLE_beg.leng_ );
			ZnkMem_getLOccTable_forBMS( occ_TITLE_end, (uint8_t*)sref_TITLE_end.cstr_,  sref_TITLE_end.leng_ );
		
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
							(uint8_t*)sref_title_beg.cstr_, sref_title_beg.leng_, 1, occ_title_beg );
					if( begp != Znk_NPOS ){
						begp += sref_title_beg.leng_;
						sizp = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(line)+begp, ZnkStr_leng(line)-begp,
								(uint8_t*)sref_title_end.cstr_, sref_title_end.leng_, 1, occ_title_end );
						ZnkStr_assign( title, 0, ZnkStr_cstr(line)+begp, sizp );
						break;
					}
		
					begp = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(line), ZnkStr_leng(line),
							(uint8_t*)sref_TITLE_beg.cstr_, sref_TITLE_beg.leng_, 1, occ_TITLE_beg );
					if( begp != Znk_NPOS ){
						begp += sref_TITLE_beg.leng_;
						sizp = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(line)+begp, ZnkStr_leng(line)-begp,
								(uint8_t*)sref_TITLE_end.cstr_, sref_TITLE_end.leng_, 1, occ_TITLE_end );
						ZnkStr_assign( title, 0, ZnkStr_cstr(line)+begp, sizp );
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
				ZnkHtpURL_negateHtmlTagEffection( title ); /* for XSS */
				ZnkStr_delete( line );
			}
		}
		if( !result ){
			ZnkStr_set( title, "Title not exist." );
		}
	}
FUNC_END:
	ZnkStr_delete( fsys_path );
	return result;
}

void
EstUIBase_makeSelectBox_byVarpAry( ZnkStr ui, ZnkVarpAry list,
		const char* label, const char* varname, const char* current_val )
{
	const size_t size = ZnkVarpAry_size( list );
	size_t       idx;
	const char*  key = NULL;
	ZnkVarp      var = NULL;
	ZnkStrAry    val_list  = ZnkStrAry_create( true );
	ZnkStrAry    name_list = ZnkStrAry_create( true );
	for( idx=0; idx<size; ++idx ){
		var = ZnkVarpAry_at( list, idx );
		key = ZnkVar_name_cstr( var );
		ZnkStrAry_push_bk_cstr( val_list,  key, Znk_NPOS );
		ZnkStrAry_push_bk_cstr( name_list, ZnkVar_cstr(var), Znk_NPOS );
	}
	ZnkStr_add( ui, label );
	RanoHtmlUI_makeSelectBox( ui,
			varname, current_val, true,
			val_list, name_list );

	ZnkStrAry_destroy( val_list );
	ZnkStrAry_destroy( name_list );
}

