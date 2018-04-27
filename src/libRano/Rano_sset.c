#include "Rano_sset.h"
#include <Znk_str_ary.h>
#include <Znk_str_ex.h>

size_t
RanoSSet_addSeq( ZnkStr dst_seq, const char* src_seq )
{
	ZnkStrAry dst_list = ZnkStrAry_create( true );
	ZnkStrAry src_list = ZnkStrAry_create( true );
	size_t idx;
	size_t size = 0;
	size_t count = 0;

	ZnkStrEx_addSplitC( dst_list,
			ZnkStr_cstr(dst_seq), Znk_NPOS,
			' ', false, 4 );
	ZnkStrEx_addSplitC( src_list,
			src_seq, Znk_NPOS,
			' ', false, 4 );
	size = ZnkStrAry_size( src_list );

	for( idx=0; idx<size; ++idx ){
		const char* src_str = ZnkStrAry_at_cstr( src_list, idx );
		if( ZnkStrAry_find( dst_list, 0, src_str, Znk_NPOS ) == Znk_NPOS ){
			ZnkStrAry_push_bk_cstr( dst_list, src_str, Znk_NPOS );
			++count;
		}
	}

	ZnkStr_clear( dst_seq );
	ZnkStrEx_addJoin( dst_seq, dst_list, 0, Znk_NPOS, " ", 1, ZnkStrAry_size(dst_list) );

	ZnkStrAry_destroy( dst_list );
	ZnkStrAry_destroy( src_list );
	return count;
}
