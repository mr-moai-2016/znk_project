#ifndef INCLUDE_GUARD__CB_wgt_prim_h__
#define INCLUDE_GUARD__CB_wgt_prim_h__

#include <Znk_prim.h>
#include <Znk_obj_ary.h>

/**
 * Weighted Primitive for CustomBoy
 * Author : Zenkaku
 *
 * This implementation is the holder of ZnkPrim with probabilistic decision making.
 */

Znk_EXTERN_C_BEGIN

typedef struct CBWgtPrimImpl* CBWgtPrim;

void
CBWgtRand_init( void );
double
CBWgtRand_getRandomReal( double min_real, double max_real );

CBWgtPrim
CBWgtPrim_create( ZnkPrimType pm_type, double rate, ZnkElemDeleterFunc elem_deleter );

void
CBWgtPrim_destroy( CBWgtPrim wpm );

ZnkPrim*
CBWgtPrim_prim( CBWgtPrim wpm );
double
CBWgtPrim_rate( const CBWgtPrim wpm );

Znk_DECLARE_HANDLE( CBWgtPrimAry );

/**
 * @brief Aryを生成する.
 *
 * @param elem_responsibility:
 *  これがtrueの場合、各要素の寿命についてこのAryは責任を持つ.
 *  つまりdestroy, clear, resize, erase, pop_bk時において、各要素に応じたdeleterとしてこれが呼ばれる.
 *  このとき、ユーザは外部で寿命を割り当てたobjectをset, push_bkなどで渡してそのまま放置しておく
 *  形になる(言い換えれば外部でこれをdeleteしてはならない). あるいは別途regist系の関数などを
 *  用意すべきである.
 *
 *  これがfalseの場合は、単にこのAryをポインタ値の一時的なコンテナとして使用し、
 *  各要素の寿命は外部の機構によって管理していることを意味する.
 *  つまりdestroy, clear, resize, erase, pop_bk時において、各要素に応じたdeleterは呼ばれない.
 *  必然的に、ユーザは内部でオブジェクト割り当てが発生するようなregist系関数をこの場合に
 *  追加で用意すべきではない. これと clear, resize, erase, push_bk, set などを混ぜて呼び出すと
 *  寿命管理に不整合が生じるからである. どうしてもregist系関数が必要な場合は素直にdeleterを
 *  セットすべきである.
 *
 * @note
 *   上記regist系 APIとは regist を言う.
 */
CBWgtPrimAry
CBWgtPrimAry_create( bool elem_responsibility );

/**
 * @brief Aryを破棄する.
 */
Znk_INLINE void
CBWgtPrimAry_destroy( CBWgtPrimAry ary ){
	ZnkObjAry_M_DESTROY( ary );
}

Znk_INLINE size_t
CBWgtPrimAry_size( const CBWgtPrimAry ary ){
	return ZnkObjAry_M_SIZE( ary );
}
Znk_INLINE CBWgtPrim
CBWgtPrimAry_at( CBWgtPrimAry ary, size_t idx ){
	return ZnkObjAry_M_AT( ary, idx, CBWgtPrim );
}
Znk_INLINE CBWgtPrim*
CBWgtPrimAry_ary_ptr( CBWgtPrimAry ary ){
	return ZnkObjAry_M_ARY_PTR( ary, CBWgtPrim );
}

Znk_INLINE bool
CBWgtPrimAry_erase( CBWgtPrimAry ary, CBWgtPrim obj ){
	return ZnkObjAry_M_ERASE( ary, obj );
}
Znk_INLINE bool
CBWgtPrimAry_erase_byIdx( CBWgtPrimAry ary, size_t idx ){
	return ZnkObjAry_M_ERASE_BY_IDX( ary, idx );
}
Znk_INLINE void
CBWgtPrimAry_clear( CBWgtPrimAry ary ){
	ZnkObjAry_M_CLEAR( ary );
}
Znk_INLINE void
CBWgtPrimAry_resize( CBWgtPrimAry ary, size_t size ){
	ZnkObjAry_M_RESIZE( ary, size );
}

Znk_INLINE void
CBWgtPrimAry_push_bk( CBWgtPrimAry ary, CBWgtPrim obj ){
	ZnkObjAry_M_PUSH_BK( ary, obj );
}
Znk_INLINE void
CBWgtPrimAry_set( CBWgtPrimAry ary, size_t idx, CBWgtPrim obj ){
	ZnkObjAry_M_SET( ary, idx, obj );
}

CBWgtPrim
CBWgtPrimAry_registStr( CBWgtPrimAry ary, const char* str, double rate );

ZnkPrim*
CBWgtPrimAry_select( const CBWgtPrimAry ary );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
