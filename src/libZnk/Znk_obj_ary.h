#ifndef INCLUDE_GUARD__Znk_obj_ary_h__
#define INCLUDE_GUARD__Znk_obj_ary_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkObj );
typedef struct ZnkObjAryImpl* ZnkObjAry;

typedef bool (*ZnkObjQueryFunc)( ZnkObj, void* arg );
typedef ZnkObj (*ZnkObjAllocFunc)( void* arg );
typedef void (*ZnkElemDeleterFunc)( void* );

/**
 * @brief Aryを生成する.
 *
 * @param elem_deleter:
 *  これが非NULLの場合、各要素の寿命についてこのAryは責任を持つ.
 *  つまりdestroy, clear, resize, erase, pop_bk時において、各要素に応じたdeleterとしてこれが呼ばれる.
 *  このとき、ユーザは外部で寿命を割り当てたobjectをset, push_bkなどで渡してそのまま放置しておく
 *  形になる(言い換えれば外部でこれをdeleteしてはならない).
 *
 *  これがNULLの場合は、単にこのAryをポインタ値の一時的なコンテナとして使用し、
 *  各要素の寿命は外部の機構によって管理していることを意味する.
 *  つまりdestroy, clear, resize, erase, pop_bk時において、各要素に応じたdeleterは呼ばれない.
 */
ZnkObjAry
ZnkObjAry_create( ZnkElemDeleterFunc elem_deleter );

/**
 * @brief Aryを破棄する.
 *  Aryのelem_deleterが非NULLに設定されている場合は、
 *  全要素についてこのdeleterも呼ばれる.
 */
void
ZnkObjAry_destroy( ZnkObjAry obj_ary );

size_t
ZnkObjAry_size( const ZnkObjAry obj_ary );

ZnkObj
ZnkObjAry_at( const ZnkObjAry obj_ary, size_t idx );
ZnkObj*
ZnkObjAry_ary_ptr( ZnkObjAry obj_ary );

ZnkElemDeleterFunc
ZnkObjAry_getElemDeleter( const ZnkObjAry obj_ary );

/**
 * @brief Aryの要素を削除する.
 *  Aryのelem_deleterが非NULLに設定されている場合は、
 *  対象要素についてこのdeleterも呼ばれる.
 *
 * @note
 *  削除要素のidxが既知である場合はbyIdx版の方が探索を行わない分高速である.
 */
bool
ZnkObjAry_erase( ZnkObjAry obj_ary, ZnkObj obj );
bool
ZnkObjAry_erase_byIdx( ZnkObjAry obj_ary, size_t idx );
/**
 * @brief Aryの要素のうち、条件に合致するものをすべて削除する.
 *
 * @note
 *  この関数をコンパクション処理として利用することもできる.
 *  即ち、リアルタイム処理の途中などでeraseを呼ばずなんらかの印をつけるだけにしておき、
 *  後で余裕があるときにこの関数を呼び出して印のついたものを一括削除することで高速化と
 *  省メモリ化が図れる.
 */
size_t
ZnkObjAry_erase_withQuery( ZnkObjAry obj_ary, ZnkObjQueryFunc query_func, void* query_arg );
bool
ZnkObjAry_pop_bk( ZnkObjAry obj_ary );

/**
 * @brief Aryのサイズを0とする.
 *  Aryのelem_deleterが非NULLに設定されている場合は、
 *  すべての要素についてこのdeleterも呼ばれる.
 */
void
ZnkObjAry_clear( ZnkObjAry obj_ary );
/**
 * @brief Aryのサイズをsizeとする.
 *  Aryのelem_deleterが非NULLに設定されている場合は、
 *  減少によってeraseされるすべての要素についてこのdeleterも呼ばれる.
 *  一方、増大においては、新規領域にはNULLポインタがセットされる.
 */
void
ZnkObjAry_resize( ZnkObjAry obj_ary, size_t size );

void
ZnkObjAry_push_bk( ZnkObjAry obj_ary, ZnkObj obj );
void
ZnkObjAry_set( ZnkObjAry obj_ary, size_t idx, ZnkObj obj );

void
ZnkObjAry_swap( ZnkObjAry obj_ary1, ZnkObjAry obj_ary2 );

ZnkObj
ZnkObjAry_intern( ZnkObjAry obj_ary, size_t* ans_idx,
		ZnkObjQueryFunc query_func, void* query_arg,
		ZnkObjAllocFunc alloc_func, void* alloc_arg,
		bool* is_newly );

/***
 * helper macros.
 */
#define ZnkObjAry_M_DESTROY( ary )            ZnkObjAry_destroy( (ZnkObjAry)ary )
#define ZnkObjAry_M_SIZE( ary )               ZnkObjAry_size( (ZnkObjAry)ary )
#define ZnkObjAry_M_AT( ary, idx, elem_type ) (elem_type) ZnkObjAry_at( (ZnkObjAry)ary, idx )
#define ZnkObjAry_M_ARY_PTR( ary, elem_type ) (elem_type*)ZnkObjAry_ary_ptr( (ZnkObjAry)ary )
#define ZnkObjAry_M_ERASE( ary, obj )         ZnkObjAry_erase( (ZnkObjAry)ary, (ZnkObj)obj )
#define ZnkObjAry_M_ERASE_BY_IDX( ary, idx )  ZnkObjAry_erase_byIdx( (ZnkObjAry)ary, idx )
#define ZnkObjAry_M_POP_BK( ary )             ZnkObjAry_pop_bk( (ZnkObjAry)ary )
#define ZnkObjAry_M_CLEAR( ary )              ZnkObjAry_clear( (ZnkObjAry)ary )
#define ZnkObjAry_M_RESIZE( ary, size )       ZnkObjAry_resize( (ZnkObjAry)ary, size )
#define ZnkObjAry_M_PUSH_BK( ary, obj )       ZnkObjAry_push_bk( (ZnkObjAry)ary, (ZnkObj)obj )
#define ZnkObjAry_M_SET( ary, idx, obj )      ZnkObjAry_set( (ZnkObjAry)ary, idx, (ZnkObj)obj )
#define ZnkObjAry_M_SWAP( ary1, ary2 )        ZnkObjAry_swap( (ZnkObjAry)ary1, (ZnkObjAry)ary2 )


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
