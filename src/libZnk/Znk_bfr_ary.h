#ifndef Znk_bfr_ary_h__INCLUDED__
#define Znk_bfr_ary_h__INCLUDED__

#include <Znk_obj_ary.h>
#include <Znk_bfr.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkBfrAry );


/**
 * @brief Aryを生成する.
 *
 * @param elem_responsibility:
 *  これがtrueの場合、各要素の寿命についてこのAryは責任を持つ.
 *  つまりdestroy, clear, resize, erase, pop_bk時において、各要素に応じたdeleterとしてこれが呼ばれる.
 *  このとき、ユーザは外部で寿命を割り当てたobjectをset, push_bkなどで渡してそのまま放置しておく
 *  形になる(言い換えれば外部でこれをdeleteしてはならない).
 *
 *  これがfalseの場合は、単にこのAryをポインタ値の一時的なコンテナとして使用し、
 *  各要素の寿命は外部の機構によって管理していることを意味する.
 *  つまりdestroy, clear, resize, erase, pop_bk時において、各要素に応じたdeleterは呼ばれない.
 */
ZnkBfrAry
ZnkBfrAry_create( bool elem_responsibility );

/**
 * @brief Aryを破棄する.
 */
Znk_INLINE void
ZnkBfrAry_destroy( ZnkBfrAry ary ){
	ZnkObjAry_M_DESTROY( ary );
}

Znk_INLINE size_t
ZnkBfrAry_size( const ZnkBfrAry ary ){
	return ZnkObjAry_M_SIZE( ary );
}
Znk_INLINE ZnkBfr
ZnkBfrAry_at( ZnkBfrAry ary, size_t idx ){
	return ZnkObjAry_M_AT( ary, idx, ZnkBfr );
}
Znk_INLINE ZnkBfr*
ZnkBfrAry_ary_ptr( ZnkBfrAry ary ){
	return ZnkObjAry_M_ARY_PTR( ary, ZnkBfr );
}

Znk_INLINE bool
ZnkBfrAry_erase( ZnkBfrAry ary, ZnkBfr obj ){
	return ZnkObjAry_M_ERASE( ary, obj );
}
Znk_INLINE bool
ZnkBfrAry_erase_byIdx( ZnkBfrAry ary, size_t idx ){
	return ZnkObjAry_M_ERASE_BY_IDX( ary, idx );
}
Znk_INLINE void
ZnkBfrAry_clear( ZnkBfrAry ary ){
	ZnkObjAry_M_CLEAR( ary );
}
Znk_INLINE void
ZnkBfrAry_resize( ZnkBfrAry ary, size_t size ){
	ZnkObjAry_M_RESIZE( ary, size );
}

Znk_INLINE void
ZnkBfrAry_push_bk( ZnkBfrAry ary, ZnkBfr obj ){
	ZnkObjAry_M_PUSH_BK( ary, obj );
}
Znk_INLINE void
ZnkBfrAry_set( ZnkBfrAry ary, size_t idx, ZnkBfr obj ){
	ZnkObjAry_M_SET( ary, idx, obj );
}


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
