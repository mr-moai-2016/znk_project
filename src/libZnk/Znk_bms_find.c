#include "Znk_bms_find.h"
#include "Znk_mem_find.h"
#include "Znk_s_base.h"
#include "Znk_stdc.h"
#include <assert.h>


struct ZnkBmsFinderImpl {
	ZnkStr  begin_ptn_;
	ZnkStr  end_ptn_;
	size_t begin_occ_tbl_[ 256 ];
	size_t end_occ_tbl_[ 256 ];
};

ZnkBmsFinder
ZnkBmsFinder_create( const char* begin_ptn, const char* end_ptn )
{
	ZnkBmsFinder info = Znk_malloc( sizeof( struct ZnkBmsFinderImpl ) );
	info->begin_ptn_ = ZnkStr_new( begin_ptn );
	info->end_ptn_   = ZnkStr_new( end_ptn );
	ZnkMem_getLOccTable_forBMS( info->begin_occ_tbl_, (uint8_t*)begin_ptn, ZnkStr_leng(info->begin_ptn_) );
	ZnkMem_getLOccTable_forBMS( info->end_occ_tbl_,   (uint8_t*)end_ptn,   ZnkStr_leng(info->end_ptn_) );
	return info;
}
void
ZnkBmsFinder_destroy( ZnkBmsFinder info )
{
	if( info ){
		ZnkStr_delete( info->begin_ptn_ );
		ZnkStr_delete( info->end_ptn_ );
		Znk_free( info );
	}
}

const char*
ZnkBmsFinder_begin_ptn( const ZnkBmsFinder info )
{
	return ZnkStr_cstr( info->begin_ptn_ );
}
const char*
ZnkBmsFinder_end_ptn( const ZnkBmsFinder info )
{
	return ZnkStr_cstr( info->end_ptn_ );
}
size_t
ZnkBmsFinder_begin_ptn_leng( const ZnkBmsFinder info )
{
	return ZnkStr_leng( info->begin_ptn_ );
}
size_t
ZnkBmsFinder_end_ptn_leng( const ZnkBmsFinder info )
{
	return ZnkStr_leng( info->end_ptn_ );
}

Znk_INLINE bool
I_getBetweenRange( const char* src, size_t src_leng, size_t src_begin,
		size_t* ans_btwn_pos, size_t* ans_btwn_leng,
		const char* begin_ptn, size_t begin_ptn_leng, const size_t* begin_occ_tbl,
		const char* end_ptn,   size_t end_ptn_leng,   const size_t* end_occ_tbl )
{
	size_t cur_pos;
	assert( src_leng >= src_begin );
	cur_pos = ZnkMem_lfind_data_BMS( (uint8_t*)src+src_begin, src_leng-src_begin,
			(uint8_t*)begin_ptn, begin_ptn_leng, 1, begin_occ_tbl );
	if( cur_pos != Znk_NPOS ){
		size_t btwn_leng;
		/* found */
		cur_pos += src_begin + begin_ptn_leng;
		btwn_leng = ZnkMem_lfind_data_BMS( (uint8_t*)src+cur_pos, src_leng-cur_pos,
				(uint8_t*)end_ptn, end_ptn_leng, 1, end_occ_tbl );
		*ans_btwn_pos  = cur_pos;
		*ans_btwn_leng = btwn_leng;
		return true;
	}
	return false;
}

bool
ZnkBmsFinder_getBetweenRange( const ZnkBmsFinder info,
		const char* src, size_t src_leng, size_t src_begin,
		size_t* ans_btwn_pos, size_t* ans_btwn_leng )
{
	return I_getBetweenRange( src, src_leng, src_begin,
			ans_btwn_pos, ans_btwn_leng,
			ZnkStr_cstr( info->begin_ptn_ ), ZnkStr_leng( info->begin_ptn_ ), info->begin_occ_tbl_,
			ZnkStr_cstr( info->end_ptn_ ),   ZnkStr_leng( info->end_ptn_ ),   info->end_occ_tbl_ );
}

