#include "Znk_bfr.h"
#include "Znk_stdc.h"

#if defined(NDEBUG)
struct ZnkBfrImpl {
	void*  data_;
	size_t size_;
	size_t capacity_;
	ZnkCapacityType type_;
};
#endif

#define GET_PAD_E2( size, pad ) ( ((size) + (pad)-1 ) & ~((pad)-1) )

ZnkBfr
ZnkBfr_create( const uint8_t* init_data, size_t size, bool with_zero, ZnkCapacityType type )
{
	ZnkBfr zkbfr = Znk_malloc( sizeof(struct ZnkBfrImpl) );
	if( zkbfr ){
		if( size > 0 ){
			const size_t capacity = ZnkCapacity_calc( size, type );
			void* data = with_zero ? Znk_alloc0( capacity ) : Znk_malloc( capacity );
			if( data ){
				zkbfr->data_ = data;
				zkbfr->size_ = size;
				zkbfr->capacity_ = capacity;
				if( init_data ){
					Znk_memmove( (uint8_t*)zkbfr->data_, init_data, size );
				}
				return zkbfr;
			}
			/***
			 * data �m�ێ��s��.
			 */
			Znk_free( zkbfr );
			return NULL;
		}
		/* ���̏ꍇ�͐����Ƃ݂Ȃ� */
		zkbfr->data_     = NULL;
		zkbfr->size_     = 0;
		zkbfr->capacity_ = 0;
	}
	return zkbfr;
}

void
ZnkBfr_destroy( ZnkBfr zkbfr )
{
	if( zkbfr ){
		ZnkBfr_clear_and_minimize( zkbfr );
		Znk_free( zkbfr );
	}
}

void
ZnkBfr_clear_and_minimize( ZnkBfr zkbfr )
{
	if( zkbfr->data_ ){
		Znk_free( zkbfr->data_ );
	}
	zkbfr->data_     = NULL;
	zkbfr->size_     = 0;
	zkbfr->capacity_ = 0;
}

Znk_INLINE bool
I_reserve( ZnkBfr zkbfr, size_t req_capacity )
{
	if( zkbfr->capacity_ < req_capacity ){
		const size_t new_capacity = ZnkCapacity_calc( req_capacity, zkbfr->type_ );
		void* new_data = Znk_realloc( zkbfr->data_, new_capacity );
		if( new_data ){
			/* ���������ꍇ�̂ݏ����X�V���� */
			zkbfr->data_     = new_data;
			zkbfr->capacity_ = new_capacity;
			return true;
		}
		/* reallocate���s�� */
		return false;
	}
	/* ���̏ꍇ�͏�ɐ����Ƃ݂Ȃ� */
	return true;
}
Znk_INLINE bool
I_resize( ZnkBfr zkbfr, size_t size )
{
	if( I_reserve( zkbfr, size ) ){
		zkbfr->size_ = size;
		return true;
	}
	return false;
}

bool
ZnkBfr_reserve( ZnkBfr zkbfr, size_t req_capacity )
{
	return I_reserve( zkbfr, req_capacity );
}

bool
ZnkBfr_resize( ZnkBfr zkbfr, size_t size )
{
	return I_resize( zkbfr, size );
}

bool
ZnkBfr_resize_fill( ZnkBfr zkbfr, size_t size, uint8_t val )
{
	const size_t old_size = zkbfr->size_;
	if( size < old_size ){
		return I_resize( zkbfr, size );
	} else if( size > old_size ){
		const size_t delta_size = size - old_size;
		if( !I_resize( zkbfr, size ) ){
			return false;
		}
		Znk_memset( (uint8_t*)zkbfr->data_ + old_size, val, delta_size );
	}
	return true;
}

bool
ZnkBfr_shrink_to_fit( ZnkBfr zkbfr )
{
	size_t new_capacity = GET_PAD_E2( zkbfr->size_, 8 );
	if( new_capacity != zkbfr->capacity_ ){
		void* new_data = Znk_realloc( zkbfr->data_, new_capacity );
		if( new_data ){
			/* ���������ꍇ�̂ݏ����X�V���� */
			zkbfr->data_     = new_data;
			zkbfr->capacity_ = new_capacity;
			return true;
		}
		/* reallocate���s�� */
		return false;
	}
	/* ���̏ꍇ�͏�ɐ����Ƃ݂Ȃ� */
	return true;
}

uint8_t*
ZnkBfr_data( ZnkBfr zkbfr ){
#if !defined(NDEBUG)
	return (uint8_t*)zkbfr->data_;
#else
	return zkbfr->data_;
#endif
}
size_t
ZnkBfr_size( const ZnkBfr zkbfr ){
	return zkbfr->size_;
}
size_t
ZnkBfr_capacity( const ZnkBfr zkbfr ){
	return zkbfr->capacity_;
}
void
ZnkBfr_swap( ZnkBfr zkbfr1, ZnkBfr zkbfr2 ){
	Znk_SWAP( struct ZnkBfrImpl, *zkbfr1, *zkbfr2 );
}


/**
 * @brief
 *   zkbfr �ɂ�����dst_pos����dst_leng�o�C�g���̗̈�(�ȉ��ł�hole�ƌĂ�)��new_leng�Ŏw�肳�ꂽ
 *   �o�C�g�T�C�Y�Ɋg��/�k������. ���̑���ł��͈͈̔ȊO�̕����̒l���ω����邱�Ƃ͂Ȃ����A
 *   hole �����ɂ��镔���� new_leng �̑傫���ɉ����ēK�؂ɃX���C�h�����.
 *   ���̃X���C�h�����replace�������ōs���������ړ�����Ɠ����ł���.
 *
 * @return
 *   ���̊֐������������ꍇ��true��Ԃ�.
 *   ������realloc�Ɏ��s�����ꍇ�͂��̊֐��͎��s���Afalse��Ԃ�.
 *   ���̂Ƃ�����zkbfr�͔j�󂳂ꂸ�ێ�����A���e����ؕύX����Ȃ�.
 */
bool
ZnkBfr_slide_hole( ZnkBfr zkbfr, const size_t dst_pos, const size_t dst_leng, const size_t new_leng )
{
	/* slide buffer remain part */
	const size_t dst_end     = Znk_clampSize( dst_pos + dst_leng, zkbfr->size_ );
	const size_t remain_size = zkbfr->size_ - dst_end;
	const size_t new_size    = dst_pos + new_leng + remain_size;
	if( I_resize( zkbfr, new_size ) ){
		Znk_memmove( (uint8_t*)zkbfr->data_ + new_size - remain_size, (uint8_t*)zkbfr->data_ + dst_end, remain_size );
		return true;
	}
	return false;
}

void
ZnkBfr_replace( ZnkBfr zkbfr, size_t dst_pos, size_t dst_leng, const uint8_t* src, size_t src_leng )
{
	ZnkBfr_slide_hole( zkbfr, dst_pos, dst_leng, src_leng );
	Znk_memmove( (uint8_t*)zkbfr->data_ + dst_pos, src, src_leng );
}

/**
 * @brief
 *   src �� src_leng �Ŏ������̈�̃f�[�^�� zkbfr �̌��֘A������.
 *   zkbfr->data_��src�͈قȂ�Aggregation�łȂ���΂Ȃ�Ȃ�.
 */
bool
ZnkBfr_append_dfr( ZnkBfr zkbfr, const uint8_t* src, size_t src_leng )
{
	const size_t old_size = zkbfr->size_;
	const size_t new_size = zkbfr->size_ + src_leng;
	if( I_resize( zkbfr, new_size ) ){
		Znk_memmove( (uint8_t*)zkbfr->data_ + old_size, src, src_leng );
		return true;
	}
	return false;
}

#define SET_VAL16( dst, val, is_LE ) \
		if( Znk_isLE() != is_LE ){ Znk_swapU16( &val ); } \
		/* �������̂���Loop�͎g��Ȃ� */ \
		dst[0] = ( (const uint8_t*)(&val) )[0]; \
		dst[1] = ( (const uint8_t*)(&val) )[1];

#define SET_VAL32( dst, val, is_LE ) \
		if( Znk_isLE() != is_LE ){ Znk_swapU32( &val ); } \
		/* �������̂���Loop�͎g��Ȃ� */ \
		dst[0] = ( (const uint8_t*)(&val) )[0]; \
		dst[1] = ( (const uint8_t*)(&val) )[1]; \
		dst[2] = ( (const uint8_t*)(&val) )[2]; \
		dst[3] = ( (const uint8_t*)(&val) )[3];

#define SET_VAL64( dst, val, is_LE ) \
		if( Znk_isLE() != is_LE ){ Znk_swapU64( &val ); } \
		/* �������̂���Loop�͎g��Ȃ� */ \
		dst[0] = ( (const uint8_t*)(&val) )[0]; \
		dst[1] = ( (const uint8_t*)(&val) )[1]; \
		dst[2] = ( (const uint8_t*)(&val) )[2]; \
		dst[3] = ( (const uint8_t*)(&val) )[3]; \
		dst[4] = ( (const uint8_t*)(&val) )[4]; \
		dst[5] = ( (const uint8_t*)(&val) )[5]; \
		dst[6] = ( (const uint8_t*)(&val) )[6]; \
		dst[7] = ( (const uint8_t*)(&val) )[7];

bool
ZnkBfr_push_bk( ZnkBfr zkbfr, uint8_t val )
{
	if( I_reserve( zkbfr, zkbfr->size_ + 1 ) ){
		((uint8_t*)zkbfr->data_)[ zkbfr->size_ ] = val; 
		++zkbfr->size_;
		return true;
	}
	return false;
}
bool
ZnkBfr_push_bk_16( ZnkBfr zkbfr, uint16_t val, bool is_LE )
{
	const size_t old_size = zkbfr->size_;
	if( I_resize( zkbfr, old_size + sizeof(uint16_t) ) ){
		uint8_t* dst = (uint8_t*)( zkbfr->data_ ) + old_size;
		SET_VAL16( dst, val, is_LE )
		return true;
	}
	return false;
}
bool
ZnkBfr_push_bk_32( ZnkBfr zkbfr, uint32_t val, bool is_LE )
{
	const size_t old_size = zkbfr->size_;
	if( I_resize( zkbfr, old_size + sizeof(uint32_t) ) ){
		uint8_t* dst = (uint8_t*)( zkbfr->data_ ) + old_size;
		SET_VAL32( dst, val, is_LE )
		return true;
	}
	return false;
}
bool
ZnkBfr_push_bk_64( ZnkBfr zkbfr, uint64_t val, bool is_LE )
{
	const size_t old_size = zkbfr->size_;
	if( I_resize( zkbfr, old_size + sizeof(uint64_t) ) ){
		uint8_t* dst = (uint8_t*)( zkbfr->data_ ) + old_size;
		SET_VAL64( dst, val, is_LE )
		return true;
	}
	return false;
}
bool
ZnkBfr_push_bk_ptr( ZnkBfr zkbfr, void* ptr )
{
	const bool is_LE = Znk_isLE(); /* �ʏ킱��͌����ɍ��킳�Ȃ���΂Ȃ�Ȃ� */
#if   Znk_CPU_BIT == 64
	return ZnkBfr_push_bk_64( zkbfr, (uint64_t)(uintptr_t)ptr, is_LE );
#elif Znk_CPU_BIT == 32
	return ZnkBfr_push_bk_32( zkbfr, (uint32_t)(uintptr_t)ptr, is_LE );
#endif
}
uint8_t*
ZnkBfr_push_bk_previous( ZnkBfr zkbfr, size_t delta )
{
	uint8_t* last_p = NULL;
	if( I_reserve( zkbfr, zkbfr->size_ + delta ) ){
		last_p = ((uint8_t*)zkbfr->data_) + zkbfr->size_;
		zkbfr->size_ += delta;
	}
	return last_p;
}

size_t
ZnkBfr_pop_bk_ex( ZnkBfr zkbfr, uint8_t* data, size_t data_size )
{
	data_size = Znk_clampSize( data_size, zkbfr->size_ );
	if( data ){
		Znk_memmove( data, (uint8_t*)(zkbfr->data_) - data_size, data_size );
	}
	/* �T�C�Y�̌����ł��邽�߁Arealloc���Ă΂�邱�Ƃ͂Ȃ����A
	 * �ꉞ��ԍŌ�ɂ�������s���� */
	I_resize( zkbfr, zkbfr->size_ - data_size );
	return data_size;
}

/*****************************************************************************/
/* fill */

#define IS_EQ_ALLBYTE_16( val ) \
	( (((val) >> 8)  & 0xff) == ((val) & 0xff) )

#define IS_EQ_ALLBYTE_32( val ) \
	( (((val) >> 8)  & 0xff)   == ((val) & 0xff) && \
	  (((val) >> 16) & 0xffff) == ((val) & 0xffff) )

#define IS_EQ_ALLBYTE_64( val ) \
	( (((val) >> 8)  & 0xff)   == ((val) & 0xff) && \
	  (((val) >> 16) & 0xffff) == ((val) & 0xffff) && \
	  (((uint64_t)(val) >> 32) & 0xffffffff) == ((val) & 0xffffffff) )

#define M_MEM_FILL( bit, unit_byte, pos ) \
	if( num ){ \
		uint8_t* dst = (uint8_t*)zkbfr->data_ + pos; \
		if( val == 0 || IS_EQ_ALLBYTE_##bit( val ) ){ \
			const size_t buf_size = num * unit_byte; \
			Znk_memset( dst, (uint8_t)(val&0xff), buf_size ); \
		} else { \
			size_t i; \
			for( i=0; i<num; ++i ){ \
				SET_VAL##bit( dst, val, is_LE ) \
				dst += unit_byte; \
			} \
		} \
	}

#define M_DIVIDE_1( val ) (val)
#define M_DIVIDE_2( val ) ((val)>>1)
#define M_DIVIDE_4( val ) ((val)>>2)
#define M_DIVIDE_8( val ) ((val)>>3)

#define M_DECIDE_NUM( num, unit_byte ) \
	if( pos >= zkbfr->size_ ){ return; } \
	(num) = Znk_clampSize( (num), M_DIVIDE_##unit_byte( zkbfr->size_ - pos ) );

void
ZnkBfr_fill( ZnkBfr zkbfr, size_t pos, uint8_t val, size_t size )
{
	M_DECIDE_NUM( size, 1 )
	Znk_memset( (uint8_t*)zkbfr->data_ + pos, val, size );
}
void
ZnkBfr_fill_16( ZnkBfr zkbfr, size_t pos, uint16_t val, size_t num, bool is_LE )
{
	M_DECIDE_NUM( num, 2 )
	M_MEM_FILL( 16, 2, pos )
}
void
ZnkBfr_fill_32( ZnkBfr zkbfr, size_t pos, uint32_t val, size_t num, bool is_LE )
{
	M_DECIDE_NUM( num, 4 )
	M_MEM_FILL( 32, 4, pos )
}
void
ZnkBfr_fill_64( ZnkBfr zkbfr, size_t pos, uint64_t val, size_t num, bool is_LE )
{
	M_DECIDE_NUM( num, 8 )
	M_MEM_FILL( 64, 8, pos )
}
void
ZnkBfr_fill_ptr( ZnkBfr zkbfr, size_t pos, void* ptr, size_t num )
{
	const bool is_LE = Znk_isLE(); /* �ʏ킱��͌����ɍ��킳�Ȃ���΂Ȃ�Ȃ� */
#if   Znk_CPU_BIT == 64
	ZnkBfr_fill_64( zkbfr, pos, (uint64_t)(uintptr_t)ptr, num, is_LE );
#elif Znk_CPU_BIT == 32
	ZnkBfr_fill_32( zkbfr, pos, (uint32_t)(uintptr_t)ptr, num, is_LE );
#endif
}
void
ZnkBfr_fill_block( ZnkBfr zkbfr, size_t pos,
		uint8_t* block, size_t block_size, size_t num )
{
	if( pos < zkbfr->size_ ){
		size_t i;
		uint8_t* dst = (uint8_t*)zkbfr->data_ + pos;
		num = Znk_clampSize( num, ( zkbfr->size_ - pos ) / block_size );
		for( i=0; i<num; ++i ){
			Znk_memmove( dst, block, block_size );
			dst += block_size;
		}
	}
}


Znk_INLINE size_t
I_transfer(
		uint8_t* dst_buf, size_t dst_buf_size,
		const uint8_t* src, size_t src_leng )
{
	src_leng = Znk_clampSize( src_leng, dst_buf_size );
	if( src_leng == 0 ){ return 0; }
	Znk_memmove( dst_buf, src, src_leng );
	return src_leng;
}
size_t
ZnkBfr_transfer( ZnkBfr zkbfr_dst, size_t dst_pos, const uint8_t* src, size_t src_leng )
{
	const size_t dst_buf_size = zkbfr_dst->size_;
	if( dst_pos < dst_buf_size ){
		return I_transfer(
				(uint8_t*)zkbfr_dst->data_+dst_pos, dst_buf_size-dst_pos,
				src, src_leng );
	}
	return 0;
}

#define M_CHECK_SERIALIZE_IDX( type_size ) \
	const size_t size = zkbfr->size_; \
	if( idx == Znk_NPOS ){ \
		if( type_size > size ){ return false; } \
		idx = size/type_size - 1; /* �ŏI�v�f */ \
	} else { \
		if( (idx+1)*type_size > size ){ return false; } \
	} \

#define M_SET_ARYVAL( bit ) \
	uint##bit##_t* ary; \
	M_CHECK_SERIALIZE_IDX( sizeof(uint##bit##_t) ) \
	if( Znk_isLE() != is_LE ){ Znk_swapU##bit( &val ); } \
	ary = (uint##bit##_t*)zkbfr->data_; \
	ary[ idx ] = val; \
	return true; \

bool
ZnkBfr_set_aryval_8( ZnkBfr zkbfr, size_t idx, uint8_t val )
{
	M_CHECK_SERIALIZE_IDX( sizeof(uint8_t) )
	((uint8_t*)(zkbfr->data_))[ idx ] = val;
	return true;
}
bool
ZnkBfr_set_aryval_16( ZnkBfr zkbfr, size_t idx, uint16_t val, bool is_LE )
{
	M_SET_ARYVAL( 16 )
}
bool
ZnkBfr_set_aryval_32( ZnkBfr zkbfr, size_t idx, uint32_t val, bool is_LE )
{
	M_SET_ARYVAL( 32 )
}
bool
ZnkBfr_set_aryval_64( ZnkBfr zkbfr, size_t idx, uint64_t val, bool is_LE )
{
	M_SET_ARYVAL( 64 )
}
bool
ZnkBfr_set_aryval_ptr( ZnkBfr zkbfr, size_t idx, void* ptr )
{
	const bool is_LE = Znk_isLE(); /* �ʏ킱��͌����ɍ��킳�Ȃ���΂Ȃ�Ȃ� */
#if   Znk_CPU_BIT == 64
	return ZnkBfr_set_aryval_64( zkbfr, idx, (uint64_t)(uintptr_t)ptr, is_LE );
#elif Znk_CPU_BIT == 32
	return ZnkBfr_set_aryval_32( zkbfr, idx, (uint32_t)(uintptr_t)ptr, is_LE );
#endif
}

bool
ZnkBfr_set_aryval_block( ZnkBfr zkbfr, size_t idx, uint8_t* block, size_t block_bytesize )
{
	M_CHECK_SERIALIZE_IDX( block_bytesize )
	ZnkBfr_transfer( zkbfr, idx*block_bytesize, block, block_bytesize );
	return true;
}

