#include "Znk_missing_libc.h"
#include "Znk_vsnprintf.h"
#include "Znk_s_base.h"
#include <stdio.h>
#include <string.h>

void*
Znk_memrchr( const void* buf, int val, size_t size )
{
	if( size ){
		register const uint8_t* r1;
		register const uint8_t* last = (uint8_t*)buf;
		r1 = last + size - 1;
		while( r1 != last && *r1 != val ) --r1;
		return ( *r1 == val ) ? (void*)r1 : NULL;
	}
	return NULL;
}

int
Znk_vsnprintf( char* dest, size_t count, const char* format, va_list argptr )
{
	return Znk_vsnprintf_C99( dest, count, format, argptr );
}
int
Znk_snprintf( char* dest, size_t count, const char* format, ... )
{
	int r;
	va_list args;
	va_start(args, format);
	r = Znk_vsnprintf( dest, count, format, args );
	va_end(args);
	return r;
}

void*
Znk_memmem( const void* mem, size_t mem_leng, const void* ptn, size_t ptn_leng )
{
	if( mem_leng && ptn_leng && mem_leng >= ptn_leng ){
		const register uint8_t* p;
		const register uint8_t* end;
		const uint8_t* uptn = (const uint8_t*) ptn;
		const uint8_t* umem = (const uint8_t*) mem;

		if( ptn_leng == 1 ){
			return memchr( mem, (int)*uptn, mem_leng );
		}

		end = umem + mem_leng - ptn_leng + 1;
		p = umem;
		while( p != end ){
			if( p[0] == uptn[0] && memcmp( p, uptn, ptn_leng ) == 0 ){
				return (void*)p; /* found */
			}
			++p;
		}
	}
	return NULL; /* not found */
}
