#ifndef INCLUDE_GUARD__Znk_vpod_h__
#define INCLUDE_GUARD__Znk_vpod_h__

#include <Znk_bfr.h>

Znk_EXTERN_C_BEGIN


/*****************************************************************************/
/***
 * ZnkVUInt16
 */

Znk_INLINE size_t  ZnkVUInt16_size( const ZnkBfr vpt ){
	return ZnkBfr_size( vpt ) / sizeof( uint16_t );
}
Znk_INLINE void    ZnkVUInt16_clear( ZnkBfr vpt ){
	ZnkBfr_clear( vpt );
}
Znk_INLINE void    ZnkVUInt16_resize( ZnkBfr vpt, size_t size ){
	ZnkBfr_resize( vpt, size * sizeof( uint16_t ) );
}
Znk_INLINE void    ZnkVUInt16_resize_fill( ZnkBfr vpt, size_t size, uint16_t val )
{
	const size_t old_size = ZnkVUInt16_size( vpt );
	if( size < old_size ){
		ZnkVUInt16_resize( vpt, size );
	} else if( size > old_size ){
		const size_t delta_size = size - old_size;
		ZnkVUInt16_resize( vpt, size );
		ZnkBfr_fill_16( vpt, old_size * sizeof( uint16_t ), val, delta_size, Znk_isLE() );
	}
}
Znk_INLINE uint16_t ZnkVUInt16_at( const ZnkBfr vpt, size_t idx ){
	const uint16_t* ary = (uint16_t*)ZnkBfr_data(vpt);
	return (uint16_t)( ary[ idx ] );
}
Znk_INLINE void    ZnkVUInt16_set( ZnkBfr vpt, size_t idx, uint16_t val ){
	ZnkBfr_set_aryval_16( vpt, idx, val, Znk_isLE() );
}
Znk_INLINE void    ZnkVUInt16_push_bk( ZnkBfr vpt, uint16_t val ){
	ZnkBfr_push_bk_16( vpt, val, Znk_isLE() );
}
Znk_INLINE void    ZnkVUInt16_pop_bk( ZnkBfr vpt ){
	ZnkBfr_pop_bk_ex( vpt, NULL, sizeof(uint16_t) );
}


/*****************************************************************************/
/***
 * ZnkVUInt32
 */

Znk_INLINE size_t  ZnkVUInt32_size( const ZnkBfr vpt ){
	return ZnkBfr_size( vpt ) / sizeof( uint32_t );
}
Znk_INLINE void    ZnkVUInt32_clear( ZnkBfr vpt ){
	ZnkBfr_clear( vpt );
}
Znk_INLINE void    ZnkVUInt32_resize( ZnkBfr vpt, size_t size ){
	ZnkBfr_resize( vpt, size * sizeof( uint32_t ) );
}
Znk_INLINE void    ZnkVUInt32_resize_fill( ZnkBfr vpt, size_t size, uint32_t val )
{
	const size_t old_size = ZnkVUInt32_size( vpt );
	if( size < old_size ){
		ZnkVUInt32_resize( vpt, size );
	} else if( size > old_size ){
		const size_t delta_size = size - old_size;
		ZnkVUInt32_resize( vpt, size );
		ZnkBfr_fill_32( vpt, old_size * sizeof( uint32_t ), val, delta_size, Znk_isLE() );
	}
}
Znk_INLINE uint32_t ZnkVUInt32_at( const ZnkBfr vpt, size_t idx ){
	const uint32_t* ary = (uint32_t*)ZnkBfr_data(vpt);
	return (uint32_t)( ary[ idx ] );
}
Znk_INLINE void    ZnkVUInt32_set( ZnkBfr vpt, size_t idx, uint32_t val ){
	ZnkBfr_set_aryval_32( vpt, idx, val, Znk_isLE() );
}
Znk_INLINE void    ZnkVUInt32_push_bk( ZnkBfr vpt, uint32_t val ){
	ZnkBfr_push_bk_32( vpt, val, Znk_isLE() );
}
Znk_INLINE void    ZnkVUInt32_pop_bk( ZnkBfr vpt ){
	ZnkBfr_pop_bk_ex( vpt, NULL, sizeof(uint32_t) );
}



/*****************************************************************************/
/***
 * ZnkVUInt64
 */

Znk_INLINE size_t  ZnkVUInt64_size( const ZnkBfr vpt ){
	return ZnkBfr_size( vpt ) / sizeof( uint64_t );
}
Znk_INLINE void    ZnkVUInt64_clear( ZnkBfr vpt ){
	ZnkBfr_clear( vpt );
}
Znk_INLINE void    ZnkVUInt64_resize( ZnkBfr vpt, size_t size ){
	ZnkBfr_resize( vpt, size * sizeof( uint64_t ) );
}
Znk_INLINE void    ZnkVUInt64_resize_fill( ZnkBfr vpt, size_t size, uint64_t val )
{
	const size_t old_size = ZnkVUInt64_size( vpt );
	if( size < old_size ){
		ZnkVUInt64_resize( vpt, size );
	} else if( size > old_size ){
		const size_t delta_size = size - old_size;
		ZnkVUInt64_resize( vpt, size );
		ZnkBfr_fill_64( vpt, old_size * sizeof( uint64_t ), val, delta_size, Znk_isLE() );
	}
}
Znk_INLINE uint64_t ZnkVUInt64_at( const ZnkBfr vpt, size_t idx ){
	const uint64_t* ary = (uint64_t*)ZnkBfr_data(vpt);
	return (uint64_t)( ary[ idx ] );
}
Znk_INLINE void    ZnkVUInt64_set( ZnkBfr vpt, size_t idx, uint64_t val ){
	ZnkBfr_set_aryval_64( vpt, idx, val, Znk_isLE() );
}
Znk_INLINE void    ZnkVUInt64_push_bk( ZnkBfr vpt, uint64_t val ){
	ZnkBfr_push_bk_64( vpt, val, Znk_isLE() );
}
Znk_INLINE void    ZnkVUInt64_pop_bk( ZnkBfr vpt ){
	ZnkBfr_pop_bk_ex( vpt, NULL, sizeof(uint64_t) );
}


/*****************************************************************************/
/***
 * ZnkVPtr
 */

Znk_INLINE size_t  ZnkVPtr_size( const ZnkBfr vpt ){
	return ZnkBfr_size( vpt ) / sizeof( void* );
}
Znk_INLINE void    ZnkVPtr_clear( ZnkBfr vpt ){
	ZnkBfr_clear( vpt );
}
Znk_INLINE void    ZnkVPtr_resize( ZnkBfr vpt, size_t size ){
	ZnkBfr_resize( vpt, size * sizeof( void* ) );
}
Znk_INLINE void    ZnkVPtr_resize_fill( ZnkBfr vpt, size_t size, void* val )
{
	const size_t old_size = ZnkVPtr_size( vpt );
	if( size < old_size ){
		ZnkVPtr_resize( vpt, size );
	} else if( size > old_size ){
		const size_t delta_size = size - old_size;
		ZnkVPtr_resize( vpt, size );
		ZnkBfr_fill_ptr( vpt, old_size * sizeof( void* ), val, delta_size );
	}
}
Znk_INLINE void*   ZnkVPtr_at( const ZnkBfr vpt, size_t idx ){
	return ZnkBfr_at_ptr( vpt, idx );
}
Znk_INLINE void    ZnkVPtr_set( ZnkBfr vpt, size_t idx, void* val ){
	ZnkBfr_set_aryval_ptr( vpt, idx, val );
}
Znk_INLINE void    ZnkVPtr_push_bk( ZnkBfr vpt, void* val ){
	ZnkBfr_push_bk_ptr( vpt, val );
}
Znk_INLINE void    ZnkVPtr_pop_bk( ZnkBfr vpt ){
	ZnkBfr_pop_bk_ptr( vpt );
}
Znk_INLINE size_t  ZnkVPtr_regist( ZnkBfr vpt, void* new_ptr )
{
	const size_t size = ZnkBfr_size( vpt ) / sizeof( void* );
	void* ptr;
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		ptr = ZnkBfr_at_ptr( vpt, idx );
		if( ptr == NULL ){
			ZnkBfr_set_aryval_ptr( vpt, idx, new_ptr );
			return idx;
		}
	}
	ZnkBfr_push_bk_ptr( vpt, new_ptr );
	return idx;
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
