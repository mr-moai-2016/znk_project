#include "Znk_str_ptn.h"
#include "Znk_missing_libc.h"
#include "Znk_stdc.h"
#include <string.h>

#define DECIDE_LENG( leng, str ) do if( (leng) == Znk_NPOS ){ (leng) = Znk_strlen(str); } while(0)

const uint8_t*
ZnkStrPtn_getBetween( ZnkStr ans, const uint8_t* data, const size_t data_size,
		const char* ptn_begin, const char* ptn_end )
{
	const size_t ptn_begin_leng = Znk_strlen( ptn_begin );
	const size_t ptn_end_leng   = Znk_strlen( ptn_end );
	const uint8_t* end = data + data_size;
	const uint8_t* p = Znk_memmem( data, data_size, ptn_begin, ptn_begin_leng );
	const uint8_t* q = NULL;
	if( p ){
		p += ptn_begin_leng;
		q = Znk_memmem( p, end-p, ptn_end, ptn_end_leng );
		ZnkStr_assign( ans, 0, (const char*)p, q-p );
		q += ptn_end_leng;
		return q;
	}
	return NULL;
}

bool
ZnkStrPtn_replaceBetween( ZnkStr ans,
		const uint8_t* data, size_t data_size,
		const char* ptn_begin, const char* ptn_end,
		const char* replace_str, bool replaced_with_ptn )
{
	const size_t ptn_begin_leng = Znk_strlen( ptn_begin );
	const size_t ptn_end_leng   = Znk_strlen( ptn_end );
	const uint8_t* p = data;
	const uint8_t* end = data + data_size;
	const uint8_t* b = NULL;
	const uint8_t* e = NULL;
	b = Znk_memmem( p, end-p, ptn_begin, ptn_begin_leng );
	if( b ){
		p = b + ptn_begin_leng;
		e = Znk_memmem( p, end-p, ptn_end, ptn_end_leng );
		if( e ){
			/* found */
			ZnkStr_assign( ans, 0, (const char*)data, b-data );
			if( replaced_with_ptn ){ ZnkStr_add( ans, ptn_begin ); }
			ZnkStr_add( ans, replace_str );
			if( replaced_with_ptn ){ ZnkStr_add( ans, ptn_end ); }
			p = e + ptn_end_leng;
			ZnkStr_add( ans, (const char*)p );
			return true;
		}
	}
	return false;
}


size_t
ZnkStrPtn_replace( ZnkStr str, size_t begin,
		const uint8_t* old_ptn, size_t old_ptn_leng,
		const uint8_t* new_ptn, size_t new_ptn_leng,
		size_t seek_depth )
{
	if( seek_depth ){
		size_t count=0;
		size_t str_leng = ZnkStr_leng(str);
		uint8_t* p;

		DECIDE_LENG( old_ptn_leng, (const char*)old_ptn );
		DECIDE_LENG( new_ptn_leng, (const char*)new_ptn );

		while( begin < str_leng ){
			p = (uint8_t*)Znk_memmem( ZnkStr_cstr(str)+begin, str_leng-begin, old_ptn, old_ptn_leng );
			if( p == NULL ){ break; }
			begin = p - (uint8_t*)ZnkStr_cstr(str);
			++count;

			ZnkBfr_replace( str, begin, old_ptn_leng, (uint8_t*)new_ptn, new_ptn_leng );
			begin += new_ptn_leng;

			if( count == seek_depth ){ break; }
			str_leng = ZnkStr_leng( str );
		}
		return count;
	}
	return 0;
}
