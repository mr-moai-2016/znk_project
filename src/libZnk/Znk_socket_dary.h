#ifndef Znk_socket_dary_h__INCLUDED__
#define Znk_socket_dary_h__INCLUDED__

#include <Znk_obj_dary.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkSocketDAry );


Znk_INLINE ZnkSocketDAry
ZnkSocketDAry_create( void )
{
	return (ZnkSocketDAry)ZnkObjDAry_create( NULL );
}

Znk_INLINE void
ZnkSocketDAry_destroy( ZnkSocketDAry dary ){
	ZnkObjDAry_M_DESTROY( dary );
}

Znk_INLINE size_t
ZnkSocketDAry_size( const ZnkSocketDAry dary ){
	return ZnkObjDAry_M_SIZE( dary );
}
Znk_INLINE ZnkSocket
ZnkSocketDAry_at( ZnkSocketDAry dary, size_t idx ){
	return ZnkObjDAry_M_AT( dary, idx, ZnkSocket );
}
Znk_INLINE ZnkSocket*
ZnkSocketDAry_dary_ptr( ZnkSocketDAry dary ){
	return ZnkObjDAry_M_ARY_PTR( dary, ZnkSocket );
}

Znk_INLINE bool
ZnkSocketDAry_erase( ZnkSocketDAry dary, ZnkSocket obj ){
	return ZnkObjDAry_M_ERASE( dary, obj );
}
Znk_INLINE bool
ZnkSocketDAry_erase_byIdx( ZnkSocketDAry dary, size_t idx ){
	return ZnkObjDAry_M_ERASE_BY_IDX( dary, idx );
}
Znk_INLINE void
ZnkSocketDAry_clear( ZnkSocketDAry dary ){
	ZnkObjDAry_M_CLEAR( dary );
}
Znk_INLINE void
ZnkSocketDAry_resize( ZnkSocketDAry dary, size_t size ){
	ZnkObjDAry_M_RESIZE( dary, size );
}

Znk_INLINE void
ZnkSocketDAry_push_bk( ZnkSocketDAry dary, ZnkSocket obj ){
	ZnkObjDAry_M_PUSH_BK( dary, obj );
}
Znk_INLINE void
ZnkSocketDAry_set( ZnkSocketDAry dary, size_t idx, ZnkSocket obj ){
	ZnkObjDAry_M_SET( dary, idx, obj );
}
Znk_INLINE void
ZnkSocketDAry_swap( ZnkSocketDAry dary1, ZnkSocketDAry dary2 ){
	ZnkObjDAry_M_SWAP( dary1, dary2 );
}
Znk_INLINE size_t
ZnkSocketDAry_find( ZnkSocketDAry sock_ary, ZnkSocket query_sock )
{
	const size_t size = ZnkSocketDAry_size( sock_ary );
	size_t       idx;
	ZnkSocket    sock;
	for( idx=0; idx<size; ++idx ){
		sock = ZnkSocketDAry_at( sock_ary, idx );
		if( sock == query_sock ){
			return idx;
		}
	}
	return Znk_NPOS;
}


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
