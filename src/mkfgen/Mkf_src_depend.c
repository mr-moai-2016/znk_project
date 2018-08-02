#include "Mkf_src_depend.h"
#include "Mkf_seek.h"
#include <Znk_varp_ary.h>
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <Znk_str_fio.h>
#include <string.h>

static void
parseSrcInclude( ZnkVarpAry vary, const char* filename, ZnkStr line, ZnkStrAry list, const char* obj_sfx )
{
	ZnkFile fp = Znk_fopen( filename, "rb" );
	if( fp ){
		char  included_file[ 256 ];
		char* p;
		char* q;
		char  quote_end;
		char  obj_file[ 256 ];
		ZnkVarp varp = NULL;
		const char* ext = ZnkS_get_extension( filename, '.' );

		ZnkS_copy( obj_file, sizeof(obj_file), filename, ext-1-filename );
		ZnkS_concat( obj_file, sizeof(obj_file), "." );
		ZnkS_concat( obj_file, sizeof(obj_file), obj_sfx );

		varp = ZnkVarpAry_find_byName( vary, obj_file, Znk_strlen(obj_file), false );
		if( varp == NULL ){
			varp = ZnkVarp_create( obj_file, "", 0, ZnkPrim_e_StrAry, NULL );
			ZnkVarpAry_push_bk( vary, varp );
		}

		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				break;
			}
			if( ZnkStr_first(line) == '#' ){
				p = strstr( ZnkStr_cstr(line)+1, "include" );
				if( p ){
					p += 7;
					while( *p == ' ' || *p == '\t' ) ++p;
					if( *p == '\"' || *p == '\'' || *p == '<' ){
						quote_end = *p == '<' ? '>' : *p;
						q = p;
						p++;
						while( *p ){
							if( *p == quote_end ){
								size_t included_file_leng = 0;
								ZnkS_copy( included_file, sizeof(included_file), q+1, p-q-1 );
								included_file_leng = Znk_strlen(included_file);
								if( ZnkStrAry_find( list, 0, included_file, included_file_leng ) != Znk_NPOS ){
									ZnkStrAry_push_bk_cstr( varp->prim_.u_.sda_, included_file, included_file_leng );
								}
								break;
							}
							++p;
						}
					}
				}
			}
		}
		Znk_fclose( fp );

		//Znk_printf_e( "%s:\n", filename, included_file );
		//Znk_printf_e( "filename=[%s] included_file=[%s]\n", filename, included_file );
	}
}
void
MkfSrcDepend_get( ZnkStr text, ZnkStrAry list, const char* obj_sfx, const char* nl, MkfSeekFuncT_isInterestExt is_interest_ext )
{
	size_t size;
	size_t idx;
	ZnkStr line = ZnkStr_new( "" );
	ZnkVarpAry vary = ZnkVarpAry_create( true );

	size = ZnkStrAry_size( list );
	for( idx=0; idx<size; ++idx ){
		const char* name = ZnkStrAry_at_cstr( list, idx );
		const char* ext = ZnkS_get_extension( name, '.' );
		if( is_interest_ext( ext ) ){
			parseSrcInclude( vary, name, line, list, obj_sfx );
		}
	}
	ZnkStr_delete( line );

	size = ZnkVarpAry_size( vary );
	for( idx=0; idx<size; ++idx ){
		const ZnkVarp varp = ZnkVarpAry_at( vary, idx );
		if( varp ){
			size_t elem_size = ZnkStrAry_size( varp->prim_.u_.sda_ );
			if( elem_size ){
				/***
				 * âΩÇ…Ç‡àÀë∂ÇµÇƒÇ¢Ç»Ç¢èÍçáÇÕÅAÇ±ÇÍÇèoóÕÇ∑ÇÈÇÃÇÕñ≥ë Ç≈Ç†ÇÈÇΩÇﬂÅA
				 * elem_size == 0 ÇÃèÍçáÇÕÇ±ÇÍÇè»ó™Ç∑ÇÈ.
				 */
				size_t elem_idx;
				ZnkStr_addf( text, "%s:", ZnkVar_name_cstr( varp ) );
				for( elem_idx=0; elem_idx<elem_size; ++elem_idx ){
					const char* elem = ZnkStrAry_at_cstr( varp->prim_.u_.sda_, elem_idx );
					ZnkStr_addf( text, " %s", elem );
				}
				ZnkStr_add( text, nl );
			}
		}
	}

	ZnkVarpAry_destroy( vary );
}
