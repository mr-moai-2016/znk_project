#include "Znk_cookie.h"
#include "Znk_s_base.h"
#include "Znk_stdc.h"
#include "Znk_str_fio.h"
#include <string.h>

ZnkVarp
ZnkCookie_regist_byAssignmentStatement( ZnkVarpDAry cookie, const char* stmt, size_t stmt_leng )
{
	size_t key_begin = Znk_NPOS;
	size_t key_end   = Znk_NPOS;
	size_t val_begin = Znk_NPOS;
	size_t val_end   = Znk_NPOS;
	const char* key;
	const char* val;

	ZnkS_find_key_and_val( stmt, 0, stmt_leng,
			&key_begin, &key_end,
			&val_begin, &val_end,
			"=", 1,
			" \t", 2 );
	if( key_begin != Znk_NPOS && val_begin != Znk_NPOS ){
		ZnkVarp varp;
		key = stmt + key_begin;
		val = stmt + val_begin;

		/* regist */
		varp = ZnkVarpDAry_find_byName( cookie,
				key, key_end - key_begin, false );
		if( varp == NULL ){
			/* V‹K’Ç‰Á */
			varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str );
			ZnkStr_assign( varp->name_, 0, key, key_end - key_begin );
			ZnkVarpDAry_push_bk( cookie, varp );
		}
		ZnkVar_set_val_Str( varp, val, val_end - val_begin );
		return varp;
	}
	return NULL;
}
void
ZnkCookie_regist_byCookieStatement( ZnkVarpDAry cok_vars, const char* cok_stmt, size_t cok_stmt_leng )
{
	const char* p = cok_stmt;
	const char* e = p + cok_stmt_leng;
	const char* q = memchr( p, ';', (size_t)(e-p) );
	if( q == NULL ){
		q = e;
	}
	while( true ){
		ZnkCookie_regist_byAssignmentStatement( cok_vars, p, (size_t)(q-p) );
		if( q == e ){ break; }
		p = q + 1;
		while( p < e && ( *p == ' ' || *p == '\t' ) ) ++p;
		if( p >= e ){ break; }
		q = memchr( p, ';', (size_t)(e-p) );
		if( q == NULL ){
			q = e;
		}
	}
}

void
ZnkCookie_extend_toCookieStatement( const ZnkVarpDAry cok_vars, ZnkStr cok_stmt )
{
	const size_t cok_size = ZnkVarpDAry_size( cok_vars );
	size_t       cok_idx;
	ZnkVarp      cok_var;
	size_t       cok_val_leng;

	for( cok_idx=0; cok_idx<cok_size; ++cok_idx ){
		cok_var = ZnkVarpDAry_at( cok_vars, cok_idx );
		cok_val_leng = ZnkVar_str_leng( cok_var );
		if( cok_val_leng ){
			ZnkStr_addf( cok_stmt, "%s=%s",
					ZnkVar_name_cstr(cok_var), ZnkVar_cstr(cok_var) );
			if( cok_idx != cok_size-1 ){
				ZnkStr_add( cok_stmt, "; " );
			}
		}
	}
}

bool
ZnkCookie_load( ZnkVarpDAry cookie, const char* cookie_filename )
{
	ZnkFile fp = ZnkF_fopen( cookie_filename, "rb" );
	if( fp == NULL ){
		return false;
	} else {
		ZnkStr line = ZnkStr_new( "" );
		while( ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			if( ZnkStr_at( line, 0 ) == '#' ){
				/* comment skip */
				continue;
			}
			ZnkStr_chompNL( line );
			ZnkCookie_regist_byAssignmentStatement( cookie, ZnkStr_cstr(line), ZnkStr_leng(line) );
		}
		ZnkStr_delete( line );
		ZnkF_fclose( fp );
	}
	return true;
}

bool
ZnkCookie_save( const ZnkVarpDAry cookie, const char* cookie_filename )
{
	ZnkFile fp = ZnkF_fopen( cookie_filename, "wb" );
	if( fp == NULL ){
		return false;
	} else {
		const size_t size = ZnkVarpDAry_size( cookie );
		ZnkVarp varp;
		size_t idx;
		for( idx=0; idx<size; ++idx ){
			varp = ZnkVarpDAry_at( cookie, idx );
			if( varp ){
				ZnkF_fprintf( fp, "%s=%s\n",
						ZnkStr_cstr( varp->name_ ),
						ZnkVar_cstr( varp ) );
			}
		}
		ZnkF_fclose( fp );
	}
	return true;
}
