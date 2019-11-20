#ifndef INCLUDE_GUARD__Znk_vpod_h__
#define INCLUDE_GUARD__Znk_vpod_h__

#include <Znk_bfr.h>
#include <Znk_vpod_aux.h>

Znk_EXTERN_C_BEGIN


/*****************************************************************************/
/***
 * ZnkVUInt16
 */
Znk_INLINE ZnkBfr
ZnkVUInt16_create( size_t size ){
	return ZnkVPod_M_CREATE( uint16_t, size );
}
Znk_INLINE void
ZnkVUInt16_destroy( ZnkBfr bfr ){
	ZnkBfr_destroy( bfr );
}
Znk_INLINE size_t
ZnkVUInt16_size( const ZnkBfr bfr ){
	return ZnkVPod_M_SIZE( bfr, uint16_t );
}
Znk_INLINE void
ZnkVUInt16_clear( ZnkBfr bfr ){
	ZnkBfr_clear( bfr );
}
Znk_INLINE void
ZnkVUInt16_resize( ZnkBfr bfr, size_t size ){
	ZnkVPod_M_RESIZE( bfr, uint16_t, size );
}
Znk_INLINE void
ZnkVUInt16_resize_fill( ZnkBfr bfr, size_t size, uint16_t val ){
	ZnkVPod_M_FILL_RESIZE_BIT( bfr, uint16_t, size, val, 16 );
}
Znk_INLINE uint16_t
ZnkVUInt16_at( const ZnkBfr bfr, size_t idx ){
	return ZnkVPod_M_AT( bfr, uint16_t, idx );
}
Znk_INLINE void
ZnkVUInt16_set( ZnkBfr bfr, size_t idx, uint16_t val ){
	ZnkVPod_M_SET_BIT( bfr, idx, val, 16 );
}
Znk_INLINE void
ZnkVUInt16_push_bk( ZnkBfr bfr, uint16_t val ){
	ZnkVPod_M_PUSH_BK_BIT( bfr, val, 16 );
}
Znk_INLINE void
ZnkVUInt16_pop_bk( ZnkBfr bfr ){
	ZnkVPod_M_POP_BK( bfr, uint16_t );
}
Znk_INLINE size_t
ZnkVUInt16_regist( ZnkBfr bfr, uint16_t new_val, uint16_t null_val ){
	size_t idx;
	ZnkVPod_M_REGIST_BIT( bfr, uint16_t, idx, new_val, null_val, 16 );
	return idx;
}


/*****************************************************************************/
/***
 * ZnkVUInt32
 */
Znk_INLINE ZnkBfr
ZnkVUInt32_create( size_t size ){
	return ZnkVPod_M_CREATE( uint32_t, size );
}
Znk_INLINE void
ZnkVUInt32_destroy( ZnkBfr bfr ){
	ZnkBfr_destroy( bfr );
}
Znk_INLINE size_t
ZnkVUInt32_size( const ZnkBfr bfr ){
	return ZnkVPod_M_SIZE( bfr, uint32_t );
}
Znk_INLINE void
ZnkVUInt32_clear( ZnkBfr bfr ){
	ZnkBfr_clear( bfr );
}
Znk_INLINE void
ZnkVUInt32_resize( ZnkBfr bfr, size_t size ){
	ZnkVPod_M_RESIZE( bfr, uint32_t, size );
}
Znk_INLINE void
ZnkVUInt32_resize_fill( ZnkBfr bfr, size_t size, uint32_t val ){
	ZnkVPod_M_FILL_RESIZE_BIT( bfr, uint32_t, size, val, 32 );
}
Znk_INLINE uint32_t
ZnkVUInt32_at( const ZnkBfr bfr, size_t idx ){
	return ZnkVPod_M_AT( bfr, uint32_t, idx );
}
Znk_INLINE void
ZnkVUInt32_set( ZnkBfr bfr, size_t idx, uint32_t val ){
	ZnkVPod_M_SET_BIT( bfr, idx, val, 32 );
}
Znk_INLINE void
ZnkVUInt32_push_bk( ZnkBfr bfr, uint32_t val ){
	ZnkVPod_M_PUSH_BK_BIT( bfr, val, 32 );
}
Znk_INLINE void
ZnkVUInt32_pop_bk( ZnkBfr bfr ){
	ZnkVPod_M_POP_BK( bfr, uint32_t );
}
Znk_INLINE size_t
ZnkVUInt32_regist( ZnkBfr bfr, uint32_t new_val, uint32_t null_val ){
	size_t idx;
	ZnkVPod_M_REGIST_BIT( bfr, uint32_t, idx, new_val, null_val, 32 );
	return idx;
}


/*****************************************************************************/
/***
 * ZnkVUInt64
 */
Znk_INLINE ZnkBfr
ZnkVUInt64_create( size_t size ){
	return ZnkVPod_M_CREATE( uint64_t, size );
}
Znk_INLINE void
ZnkVUInt64_destroy( ZnkBfr bfr ){
	ZnkBfr_destroy( bfr );
}
Znk_INLINE size_t
ZnkVUInt64_size( const ZnkBfr bfr ){
	return ZnkVPod_M_SIZE( bfr, uint64_t );
}
Znk_INLINE void
ZnkVUInt64_clear( ZnkBfr bfr ){
	ZnkBfr_clear( bfr );
}
Znk_INLINE void
ZnkVUInt64_resize( ZnkBfr bfr, size_t size ){
	ZnkVPod_M_RESIZE( bfr, uint64_t, size );
}
Znk_INLINE void
ZnkVUInt64_resize_fill( ZnkBfr bfr, size_t size, uint64_t val ){
	ZnkVPod_M_FILL_RESIZE_BIT( bfr, uint64_t, size, val, 64 );
}
Znk_INLINE uint64_t
ZnkVUInt64_at( const ZnkBfr bfr, size_t idx ){
	return ZnkVPod_M_AT( bfr, uint64_t, idx );
}
Znk_INLINE void
ZnkVUInt64_set( ZnkBfr bfr, size_t idx, uint64_t val ){
	ZnkVPod_M_SET_BIT( bfr, idx, val, 64 );
}
Znk_INLINE void
ZnkVUInt64_push_bk( ZnkBfr bfr, uint64_t val ){
	ZnkVPod_M_PUSH_BK_BIT( bfr, val, 64 );
}
Znk_INLINE void
ZnkVUInt64_pop_bk( ZnkBfr bfr ){
	ZnkVPod_M_POP_BK( bfr, uint64_t );
}
Znk_INLINE size_t
ZnkVUInt64_regist( ZnkBfr bfr, uint64_t new_val, uint64_t null_val ){
	size_t idx;
	ZnkVPod_M_REGIST_BIT( bfr, uint64_t, idx, new_val, null_val, 64 );
	return idx;
}


/*****************************************************************************/
/***
 * ZnkVDouble
 */
Znk_INLINE ZnkBfr
ZnkVDouble_create( size_t size ){
	return ZnkVPod_M_CREATE( double, size );
}
Znk_INLINE void
ZnkVDouble_destroy( ZnkBfr bfr ){
	ZnkBfr_destroy( bfr );
}
Znk_INLINE size_t
ZnkVDouble_size( const ZnkBfr bfr ){
	return ZnkVPod_M_SIZE( bfr, double );
}
Znk_INLINE void
ZnkVDouble_clear( ZnkBfr bfr ){
	ZnkBfr_clear( bfr );
}
Znk_INLINE void
ZnkVDouble_resize( ZnkBfr bfr, size_t size ){
	ZnkVPod_M_RESIZE( bfr, double, size );
}
Znk_INLINE void
ZnkVDouble_resize_fill( ZnkBfr bfr, size_t size, double val ){
	ZnkVPod_M_FILL_RESIZE_BLOCK( bfr, double, size, val );
}
Znk_INLINE double
ZnkVDouble_at( const ZnkBfr bfr, size_t idx ){
	return ZnkVPod_M_AT( bfr, double, idx );
}
Znk_INLINE void
ZnkVDouble_set( ZnkBfr bfr, size_t idx, double val ){
	ZnkVPod_M_SET_BLOCK( bfr, idx, val );
}
Znk_INLINE void
ZnkVDouble_push_bk( ZnkBfr bfr, double val ){
	ZnkVPod_M_PUSH_BK_BLOCK( bfr, val );
}
Znk_INLINE void
ZnkVDouble_pop_bk( ZnkBfr bfr ){
	ZnkVPod_M_POP_BK( bfr, double );
}
Znk_INLINE size_t
ZnkVDouble_regist( ZnkBfr bfr, double new_val, double null_val ){
	size_t idx;
	ZnkVPod_M_REGIST_BLOCK( bfr, double, idx, new_val, null_val );
	return idx;
}


/*****************************************************************************/
/***
 * ZnkVSizeT
 */
Znk_INLINE ZnkBfr
ZnkVSizeT_create( size_t size ){
	return ZnkVPod_M_CREATE( size_t, size );
}
Znk_INLINE void
ZnkVSizeT_destroy( ZnkBfr bfr ){
	ZnkBfr_destroy( bfr );
}
Znk_INLINE size_t
ZnkVSizeT_size( const ZnkBfr bfr ){
	return ZnkVPod_M_SIZE( bfr, size_t );
}
Znk_INLINE void
ZnkVSizeT_clear( ZnkBfr bfr ){
	ZnkBfr_clear( bfr );
}
Znk_INLINE void
ZnkVSizeT_resize( ZnkBfr bfr, size_t size ){
	ZnkVPod_M_RESIZE( bfr, size_t, size );
}
Znk_INLINE void
ZnkVSizeT_resize_fill( ZnkBfr bfr, size_t size, size_t val ){
	ZnkVPod_M_FILL_RESIZE_BLOCK( bfr, size_t, size, val );
}
Znk_INLINE size_t
ZnkVSizeT_at( const ZnkBfr bfr, size_t idx ){
	return ZnkVPod_M_AT( bfr, size_t, idx );
}
Znk_INLINE void
ZnkVSizeT_set( ZnkBfr bfr, size_t idx, size_t val ){
	ZnkVPod_M_SET_BLOCK( bfr, idx, val );
}
Znk_INLINE void
ZnkVSizeT_push_bk( ZnkBfr bfr, size_t val ){
	ZnkVPod_M_PUSH_BK_BLOCK( bfr, val );
}
Znk_INLINE void
ZnkVSizeT_pop_bk( ZnkBfr bfr ){
	ZnkVPod_M_POP_BK( bfr, size_t );
}
Znk_INLINE size_t
ZnkVSizeT_regist( ZnkBfr bfr, size_t new_val, size_t null_val ){
	size_t idx;
	ZnkVPod_M_REGIST_BLOCK( bfr, size_t, idx, new_val, null_val );
	return idx;
}


/*****************************************************************************/
/***
 * ZnkVPtr
 */
Znk_INLINE ZnkBfr
ZnkVPtr_create( size_t size ){
	return ZnkVPod_M_CREATE( void*, size );
}
Znk_INLINE void
ZnkVPtr_destroy( ZnkBfr bfr ){
	ZnkBfr_destroy( bfr );
}
Znk_INLINE size_t
ZnkVPtr_size( const ZnkBfr bfr ){
	return ZnkVPod_M_SIZE( bfr, void* );
}
Znk_INLINE void
ZnkVPtr_clear( ZnkBfr bfr ){
	ZnkBfr_clear( bfr );
}
Znk_INLINE void
ZnkVPtr_resize( ZnkBfr bfr, size_t size ){
	ZnkVPod_M_RESIZE( bfr, void*, size );
}
Znk_INLINE void
ZnkVPtr_resize_fill( ZnkBfr bfr, size_t size, void* val ){
	ZnkVPod_M_FILL_RESIZE_BLOCK( bfr, void*, size, val );
}
Znk_INLINE void*
ZnkVPtr_at( const ZnkBfr bfr, size_t idx ){
	return ZnkVPod_M_AT( bfr, void*, idx );
}
Znk_INLINE void
ZnkVPtr_set( ZnkBfr bfr, size_t idx, void* val ){
	ZnkVPod_M_SET_BLOCK( bfr, idx, val );
}
Znk_INLINE void
ZnkVPtr_push_bk( ZnkBfr bfr, void* val ){
	ZnkVPod_M_PUSH_BK_BLOCK( bfr, val );
}
Znk_INLINE void
ZnkVPtr_pop_bk( ZnkBfr bfr ){
	ZnkVPod_M_POP_BK( bfr, void* );
}
Znk_INLINE size_t
ZnkVPtr_regist( ZnkBfr bfr, void* new_val, void* null_val ){
	size_t idx;
	ZnkVPod_M_REGIST_BLOCK( bfr, void*, idx, new_val, null_val );
	return idx;
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
