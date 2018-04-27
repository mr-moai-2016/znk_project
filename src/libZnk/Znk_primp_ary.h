#ifndef Znk_primp_ary_h__INCLUDED__
#define Znk_primp_ary_h__INCLUDED__

#include <Znk_obj_ary.h>
#include <Znk_prim.h>

Znk_EXTERN_C_BEGIN

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
ZnkPrimpAry
ZnkPrimpAry_create( bool elem_responsibility );

/**
 * @brief Aryを破棄する.
 */
Znk_INLINE void
ZnkPrimpAry_destroy( ZnkPrimpAry ary ){
	ZnkObjAry_M_DESTROY( ary );
}

Znk_INLINE size_t
ZnkPrimpAry_size( const ZnkPrimpAry ary ){
	return ZnkObjAry_M_SIZE( ary );
}
Znk_INLINE ZnkPrimp
ZnkPrimpAry_at( ZnkPrimpAry ary, size_t idx ){
	return ZnkObjAry_M_AT( ary, idx, ZnkPrimp );
}
Znk_INLINE ZnkPrimp*
ZnkPrimpAry_ary_ptr( ZnkPrimpAry ary ){
	return ZnkObjAry_M_ARY_PTR( ary, ZnkPrimp );
}

Znk_INLINE bool
ZnkPrimpAry_erase( ZnkPrimpAry ary, ZnkPrimp obj ){
	return ZnkObjAry_M_ERASE( ary, obj );
}
Znk_INLINE bool
ZnkPrimpAry_erase_byIdx( ZnkPrimpAry ary, size_t idx ){
	return ZnkObjAry_M_ERASE_BY_IDX( ary, idx );
}
Znk_INLINE void
ZnkPrimpAry_clear( ZnkPrimpAry ary ){
	ZnkObjAry_M_CLEAR( ary );
}
Znk_INLINE void
ZnkPrimpAry_resize( ZnkPrimpAry ary, size_t size ){
	ZnkObjAry_M_RESIZE( ary, size );
}

Znk_INLINE void
ZnkPrimpAry_push_bk( ZnkPrimpAry ary, ZnkPrimp obj ){
	ZnkObjAry_M_PUSH_BK( ary, obj );
}
Znk_INLINE void
ZnkPrimpAry_set( ZnkPrimpAry ary, size_t idx, ZnkPrimp obj ){
	ZnkObjAry_M_SET( ary, idx, obj );
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
