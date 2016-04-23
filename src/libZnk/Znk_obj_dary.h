#ifndef INCLUDE_GUARD__Znk_obj_dary_h__
#define INCLUDE_GUARD__Znk_obj_dary_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkObj );
typedef struct ZnkObjDAryImpl* ZnkObjDAry;

typedef bool (*ZnkObjQueryFunc)( ZnkObj, void* arg );
typedef ZnkObj (*ZnkObjAllocFunc)( void* arg );
typedef void (*ZnkElemDeleterFunc)( void* );

/**
 * @brief DAry(Dynamic Array)を生成する.
 *
 * @param elem_deleter:
 *  これが非NULLの場合、各要素の寿命についてこのDAryは責任を持つ.
 *  つまりdestroy, clear, resize, erase, pop_bk時において、各要素に応じたdeleterとしてこれが呼ばれる.
 *  このとき、ユーザは外部で寿命を割り当てたobjectをset, push_bkなどで渡してそのまま放置しておく
 *  形になる(言い換えれば外部でこれをdeleteしてはならない). あるいは別途regist系の関数などを
 *  用意すべきである.
 *
 *  これがNULLの場合は、単にこのDAryをポインタ値の一時的なコンテナとして使用し、
 *  各要素の寿命は外部の機構によって管理していることを意味する.
 *  つまりdestroy, clear, resize, erase, pop_bk時において、各要素に応じたdeleterは呼ばれない.
 *  必然的に、ユーザは内部でオブジェクト割り当てが発生するようなregist系関数をこの場合に
 *  追加で用意すべきではない. これと clear, resize, erase, push_bk, set などを混ぜて呼び出すと
 *  寿命管理に不整合が生じるからである. どうしてもregist系関数が必要な場合は素直にdeleterを
 *  セットすべきである.
 */
ZnkObjDAry
ZnkObjDAry_create( ZnkElemDeleterFunc elem_deleter );

/**
 * @brief DAryを破棄する.
 *  DAryのelem_deleterが非NULLに設定されている場合は、
 *  全要素についてこのdeleterも呼ばれる.
 */
void
ZnkObjDAry_destroy( ZnkObjDAry obj_dary );

size_t
ZnkObjDAry_size( const ZnkObjDAry obj_dary );

ZnkObj
ZnkObjDAry_at( const ZnkObjDAry obj_dary, size_t idx );
ZnkObj*
ZnkObjDAry_dary_ptr( ZnkObjDAry obj_dary );

ZnkElemDeleterFunc
ZnkObjDAry_getElemDeleter( const ZnkObjDAry obj_dary );

/**
 * @brief DAryの要素を削除する.
 *  DAryのelem_deleterが非NULLに設定されている場合は、
 *  対象要素についてこのdeleterも呼ばれる.
 */
bool
ZnkObjDAry_erase( ZnkObjDAry obj_dary, ZnkObj obj );
bool
ZnkObjDAry_erase_byIdx( ZnkObjDAry obj_dary, size_t idx );
/**
 * @brief DAryの要素のうち、条件に合致するものをすべて削除する.
 *
 * @note
 *  この関数をコンパクション処理として利用することもできる.
 *  即ち、リアルタイム処理の途中などでeraseを呼ばずなんらかの印をつけるだけにしておき、
 *  後で余裕があるときにこの関数を呼び出して印のついたものを一括削除することで高速化と
 *  省メモリ化が図れる.
 */
size_t
ZnkObjDAry_erase_withQuery( ZnkObjDAry obj_dary, ZnkObjQueryFunc query_func, void* query_arg );
bool
ZnkObjDAry_pop_bk( ZnkObjDAry obj_dary );

/**
 * @brief DAryのサイズを0とする.
 *  DAryのelem_deleterが非NULLに設定されている場合は、
 *  すべての要素についてこのdeleterも呼ばれる.
 */
void
ZnkObjDAry_clear( ZnkObjDAry obj_dary );
/**
 * @brief DAryのサイズをsizeとする.
 *  DAryのelem_deleterが非NULLに設定されている場合は、
 *  減少によってeraseされるすべての要素についてこのdeleterも呼ばれる.
 *  一方、増大においては、新規領域にはNULLポインタがセットされる.
 */
void
ZnkObjDAry_resize( ZnkObjDAry obj_dary, size_t size );

void
ZnkObjDAry_push_bk( ZnkObjDAry obj_dary, ZnkObj obj );
void
ZnkObjDAry_set( ZnkObjDAry obj_dary, size_t idx, ZnkObj obj );

void
ZnkObjDAry_swap( ZnkObjDAry obj_dary1, ZnkObjDAry obj_dary2 );

ZnkObj
ZnkObjDAry_intern( ZnkObjDAry obj_dary, size_t* ans_idx,
		ZnkObjQueryFunc query_func, void* query_arg,
		ZnkObjAllocFunc alloc_func, void* alloc_arg,
		bool* is_newly );

/***
 * helper macros.
 */
#define ZnkObjDAry_M_DESTROY( dary )            ZnkObjDAry_destroy( (ZnkObjDAry)dary )
#define ZnkObjDAry_M_SIZE( dary )               ZnkObjDAry_size( (ZnkObjDAry)dary )
#define ZnkObjDAry_M_AT( dary, idx, elem_type ) (elem_type) ZnkObjDAry_at( (ZnkObjDAry)dary, idx )
#define ZnkObjDAry_M_ARY_PTR( dary, elem_type ) (elem_type*)ZnkObjDAry_dary_ptr( (ZnkObjDAry)dary )
#define ZnkObjDAry_M_ERASE( dary, obj )         ZnkObjDAry_erase( (ZnkObjDAry)dary, (ZnkObj)obj )
#define ZnkObjDAry_M_ERASE_BY_IDX( dary, idx )  ZnkObjDAry_erase_byIdx( (ZnkObjDAry)dary, idx )
#define ZnkObjDAry_M_POP_BK( dary )             ZnkObjDAry_pop_bk( (ZnkObjDAry)dary )
#define ZnkObjDAry_M_CLEAR( dary )              ZnkObjDAry_clear( (ZnkObjDAry)dary )
#define ZnkObjDAry_M_RESIZE( dary, size )       ZnkObjDAry_resize( (ZnkObjDAry)dary, size )
#define ZnkObjDAry_M_PUSH_BK( dary, obj )       ZnkObjDAry_push_bk( (ZnkObjDAry)dary, (ZnkObj)obj )
#define ZnkObjDAry_M_SET( dary, idx, obj )      ZnkObjDAry_set( (ZnkObjDAry)dary, idx, (ZnkObj)obj )
#define ZnkObjDAry_M_SWAP( dary1, dary2 )       ZnkObjDAry_swap( (ZnkObjDAry)dary1, (ZnkObjDAry)dary2 )


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
