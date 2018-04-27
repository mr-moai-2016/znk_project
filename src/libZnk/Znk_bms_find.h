#ifndef INCLUDE_GUARD__Znk_bms_find_h__
#define INCLUDE_GUARD__Znk_bms_find_h__

#include <Znk_err.h>
#include <Znk_str.h>
#include <Znk_s_base.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkBmsFinderImpl* ZnkBmsFinder;

ZnkBmsFinder
ZnkBmsFinder_create( const char* begin_ptn, const char* end_ptn );

void
ZnkBmsFinder_destroy( ZnkBmsFinder info );

const char*
ZnkBmsFinder_begin_ptn( const ZnkBmsFinder info );
const char*
ZnkBmsFinder_end_ptn( const ZnkBmsFinder info );
size_t
ZnkBmsFinder_begin_ptn_leng( const ZnkBmsFinder info );
size_t
ZnkBmsFinder_end_ptn_leng( const ZnkBmsFinder info );

/***
 * src_lengはsrc文字列のアクセス可能な範囲を示すものとする.
 * scanされるバイトサイズではないことに注意.
 * つまり、実際にscanされる範囲は [src_begin, src_leng) であり、
 * src_leng-src_beginバイト分のみがscanされるということである.
 */
bool
ZnkBmsFinder_getBetweenRange( const ZnkBmsFinder info,
		const char* src, size_t src_leng, size_t src_begin,
		size_t* ans_btwn_pos, size_t* ans_btwn_leng );

Znk_INLINE bool
ZnkBmsFinder_getBetweenCStr( const ZnkBmsFinder info,
		const char* src, size_t src_leng, char* ansbuf, size_t ansbuf_size )
{
	size_t btwn_pos;
	size_t btwn_leng;
	if( ZnkBmsFinder_getBetweenRange( info, src, src_leng, 0, &btwn_pos, &btwn_leng ) ){
		ZnkS_copy( ansbuf, ansbuf_size, src+btwn_pos, btwn_leng );
		return true;
	}
	return false;
}
Znk_INLINE bool
ZnkBmsFinder_addBetweenStr( const ZnkBmsFinder info,
		const char* src, size_t src_leng, ZnkStr ans )
{
	size_t btwn_pos;
	size_t btwn_leng;
	if( ZnkBmsFinder_getBetweenRange( info, src, src_leng, 0, &btwn_pos, &btwn_leng ) ){
		ZnkStr_append( ans, src+btwn_pos, btwn_leng );
		return true;
	}
	return false;
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
