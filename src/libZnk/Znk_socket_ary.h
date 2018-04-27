#ifndef Znk_socket_ary_h__INCLUDED__
#define Znk_socket_ary_h__INCLUDED__

#include <Znk_obj_ary.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkSocketAry );


Znk_INLINE ZnkSocketAry
ZnkSocketAry_create( void )
{
	return (ZnkSocketAry)ZnkObjAry_create( NULL );
}

Znk_INLINE void
ZnkSocketAry_destroy( ZnkSocketAry ary ){
	ZnkObjAry_M_DESTROY( ary );
}

Znk_INLINE size_t
ZnkSocketAry_size( const ZnkSocketAry ary ){
	return ZnkObjAry_M_SIZE( ary );
}
Znk_INLINE ZnkSocket
ZnkSocketAry_at( ZnkSocketAry ary, size_t idx ){
	return ZnkObjAry_M_AT( ary, idx, ZnkSocket );
}
Znk_INLINE ZnkSocket*
ZnkSocketAry_ary_ptr( ZnkSocketAry ary ){
	return ZnkObjAry_M_ARY_PTR( ary, ZnkSocket );
}

Znk_INLINE bool
ZnkSocketAry_erase( ZnkSocketAry ary, ZnkSocket obj ){
	return ZnkObjAry_M_ERASE( ary, obj );
}
Znk_INLINE bool
ZnkSocketAry_erase_byIdx( ZnkSocketAry ary, size_t idx ){
	return ZnkObjAry_M_ERASE_BY_IDX( ary, idx );
}
Znk_INLINE void
ZnkSocketAry_clear( ZnkSocketAry ary ){
	ZnkObjAry_M_CLEAR( ary );
}
Znk_INLINE void
ZnkSocketAry_resize( ZnkSocketAry ary, size_t size ){
	ZnkObjAry_M_RESIZE( ary, size );
}

Znk_INLINE void
ZnkSocketAry_push_bk( ZnkSocketAry ary, ZnkSocket obj ){
	ZnkObjAry_M_PUSH_BK( ary, obj );
}
Znk_INLINE void
ZnkSocketAry_set( ZnkSocketAry ary, size_t idx, ZnkSocket obj ){
	ZnkObjAry_M_SET( ary, idx, obj );
}
Znk_INLINE void
ZnkSocketAry_swap( ZnkSocketAry ary1, ZnkSocketAry ary2 ){
	ZnkObjAry_M_SWAP( ary1, ary2 );
}
Znk_INLINE size_t
ZnkSocketAry_find( ZnkSocketAry sock_ary, ZnkSocket query_sock )
{
	const size_t size = ZnkSocketAry_size( sock_ary );
	size_t       idx;
	ZnkSocket    sock;
	for( idx=0; idx<size; ++idx ){
		sock = ZnkSocketAry_at( sock_ary, idx );
		if( sock == query_sock ){
			return idx;
		}
	}
	return Znk_NPOS;
}
Znk_INLINE void
ZnkSocketAry_copy( ZnkSocketAry dst, const ZnkSocketAry src )
{
	const size_t size = ZnkSocketAry_size( src );
	size_t       idx;
	ZnkSocket    sock;
	ZnkSocketAry_clear( dst );
	for( idx=0; idx<size; ++idx ){
		sock = ZnkSocketAry_at( src, idx );
		ZnkSocketAry_push_bk( dst, sock );
	}
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
