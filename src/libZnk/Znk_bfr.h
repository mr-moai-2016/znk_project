#ifndef INCLUDE_GUARD__Znk_bfr_h__
#define INCLUDE_GUARD__Znk_bfr_h__

/***
 * ZnkBfr
 * 汎用のbinary streamを保持する uint8_t の動的配列である.
 */

#include <Znk_capacity.h>

Znk_EXTERN_C_BEGIN

#if !defined(NDEBUG)
/* for VC debugger */
struct ZnkBfrImpl {
	char*  data_;
	size_t size_;
	size_t capacity_;
	ZnkCapacityType type_;
};
#endif

typedef struct ZnkBfrImpl* ZnkBfr;

ZnkBfr
ZnkBfr_create( const uint8_t* init_data, size_t size, bool with_zero, ZnkCapacityType type );

Znk_INLINE ZnkBfr
ZnkBfr_create_null( void ){
	return ZnkBfr_create( NULL, 0, false, ZnkCapacity_Exp2 );
}

void
ZnkBfr_destroy( ZnkBfr bfr );

void
ZnkBfr_clear_and_minimize( ZnkBfr bfr );

bool
ZnkBfr_reserve( ZnkBfr bfr, size_t req_capacity );

bool
ZnkBfr_resize( ZnkBfr bfr, size_t size );

Znk_INLINE void ZnkBfr_clear( ZnkBfr bfr ){ ZnkBfr_resize( bfr, 0 ); }

bool
ZnkBfr_resize_fill( ZnkBfr bfr, size_t size, uint8_t val );

bool
ZnkBfr_shrink_to_fit( ZnkBfr bfr );

uint8_t*
ZnkBfr_data( ZnkBfr bfr );

Znk_INLINE void*
ZnkBfr_at_ptr( ZnkBfr bfr, size_t idx ){
	const uintptr_t* ary = (uintptr_t*)ZnkBfr_data(bfr);
	return (void*)( ary[ idx ] );
}
#define ZnkBfr_at_type( type, bfr, idx )  ( (type*)( ZnkBfr_data(bfr) + (idx) * sizeof(type) ) )

size_t
ZnkBfr_size( const ZnkBfr bfr );

size_t
ZnkBfr_capacity( const ZnkBfr bfr );

void
ZnkBfr_swap( ZnkBfr zkbfr1, ZnkBfr zkbfr2 );


/**
 * @brief
 *   bfr におけるdst_posからdst_lengバイト分の領域(以下ではholeと呼ぶ)をnew_lengで指定された
 *   バイトサイズに拡大/縮小する. この操作でこの範囲以外の部分の値が変化することはないが、
 *   hole より後ろにある部分は new_leng の大きさに応じて適切にスライドされる.
 *   このスライド操作はreplace処理等で行うメモリ移動操作と同等である.
 *
 * @return
 *   この関数が成功した場合はtrueを返す.
 *   内部でreallocに失敗した場合はこの関数は失敗し、falseを返す.
 *   このとき元のbfrは破壊されず維持され、内容も一切変更されない.
 */
bool
ZnkBfr_slide_hole( ZnkBfr bfr, const size_t dst_pos, const size_t dst_leng, const size_t new_leng );

void
ZnkBfr_replace( ZnkBfr bfr, size_t dst_pos, size_t dst_leng, const uint8_t* src, size_t src_leng );

Znk_INLINE void
ZnkBfr_insert( ZnkBfr bfr, size_t dst_pos, const uint8_t* src, size_t src_leng ){
	ZnkBfr_replace( bfr, dst_pos, 0, src, src_leng );
}

Znk_INLINE void
ZnkBfr_copy( ZnkBfr dst, const ZnkBfr src ){
	ZnkBfr_replace( dst, 0, ZnkBfr_size(dst), ZnkBfr_data(src), ZnkBfr_size(src) );
}

Znk_INLINE void
ZnkBfr_erase_ptr( ZnkBfr bfr, size_t pos, size_t num ){
	ZnkBfr_slide_hole( bfr, pos, sizeof(void*)*num, 0 );
}
Znk_INLINE void
ZnkBfr_erase( ZnkBfr bfr, size_t pos, size_t leng ){
	ZnkBfr_slide_hole( bfr, pos, leng, 0 );
}

/**
 * @brief
 *   src と src_leng で示される領域のデータを bfr の後ろへ連結する.
 *   bfr->data_とsrcは異なるAggregationでなければならない.
 */
bool
ZnkBfr_append_dfr( ZnkBfr bfr, const uint8_t* src, size_t src_leng );
Znk_INLINE void
ZnkBfr_set_dfr( ZnkBfr bfr, const uint8_t* src, size_t src_leng ){
	ZnkBfr_clear( bfr );
	ZnkBfr_append_dfr( bfr, src, src_leng );
}

bool
ZnkBfr_push_bk( ZnkBfr bfr, uint8_t val );
bool
ZnkBfr_push_bk_16( ZnkBfr bfr, uint16_t val, bool is_LE );
bool
ZnkBfr_push_bk_32( ZnkBfr bfr, uint32_t val, bool is_LE );
bool
ZnkBfr_push_bk_64( ZnkBfr bfr, uint64_t val, bool is_LE );
bool
ZnkBfr_push_bk_ptr( ZnkBfr bfr, void* ptr );

/**
 * @brief
 *   これから delta byte だけのデータをpush_bk するのに先立って事前にresizeしておく.
 *   増加した delta byte 分の領域はこの時点では不定値となっているが、
 *   戻り値のポインタがその増分領域を示すので、これを利用して値をセットすればよい.
 *
 * @note
 *   この関数はpush_bk処理を高速化するためのものであり、
 *   これによりZnkBfr関数の呼び出し回数を最小限にできる.
 */
uint8_t*
ZnkBfr_push_bk_previous( ZnkBfr zkbfr, size_t delta );

size_t
ZnkBfr_pop_bk_ex( ZnkBfr bfr, uint8_t* data, size_t data_size );

Znk_INLINE void
ZnkBfr_pop_bk_ptr( ZnkBfr bfr ){
	ZnkBfr_pop_bk_ex( bfr, NULL, sizeof(void*) );
}

void
ZnkBfr_fill( ZnkBfr bfr, size_t pos, uint8_t val, size_t size );
void
ZnkBfr_fill_16( ZnkBfr bfr, size_t pos, uint16_t val, size_t num, bool is_LE );
void
ZnkBfr_fill_32( ZnkBfr bfr, size_t pos, uint32_t val, size_t num, bool is_LE );
void
ZnkBfr_fill_64( ZnkBfr bfr, size_t pos, uint64_t val, size_t num, bool is_LE );
void
ZnkBfr_fill_ptr( ZnkBfr bfr, size_t pos, void* ptr, size_t num );
void
ZnkBfr_fill_block( ZnkBfr bfr, size_t pos,
		uint8_t* block, size_t block_size, size_t num );

size_t
ZnkBfr_transfer( ZnkBfr zkbfr_dst, size_t dst_pos, const uint8_t* src, size_t src_leng );

bool
ZnkBfr_set_aryval_8( ZnkBfr bfr, size_t idx, uint8_t val );
bool
ZnkBfr_set_aryval_16( ZnkBfr bfr, size_t idx, uint16_t val, bool is_LE );
bool
ZnkBfr_set_aryval_32( ZnkBfr bfr, size_t idx, uint32_t val, bool is_LE );
bool
ZnkBfr_set_aryval_64( ZnkBfr bfr, size_t idx, uint64_t val, bool is_LE );
bool
ZnkBfr_set_aryval_ptr( ZnkBfr bfr, size_t idx, void* ptr );
bool
ZnkBfr_set_aryval_block( ZnkBfr bfr, size_t idx, uint8_t* block, size_t block_bytesize );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
