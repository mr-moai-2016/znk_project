#ifndef INCLUDE_GUARD__Znk_vpod_h__
#define INCLUDE_GUARD__Znk_vpod_h__

#include <Znk_bfr.h>

Znk_EXTERN_C_BEGIN


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
