#ifndef INCLUDE_GUARD__Moai_module_ary_h__
#define INCLUDE_GUARD__Moai_module_ary_h__

#include <Znk_obj_dary.h>
#include <Moai_module.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( MoaiModuleAry );


/**
 * @brief DAryを生成する.
 *
 * @param elem_responsibility:
 *  これがtrueの場合、各要素の寿命についてこのDAryは責任を持つ.
 *  つまりdestroy, clear, resize, erase, pop_bk時において、各要素に応じたdeleterとしてこれが呼ばれる.
 *  このとき、ユーザは外部で寿命を割り当てたobjectをset, push_bkなどで渡してそのまま放置しておく
 *  形になる(言い換えれば外部でこれをdeleteしてはならない). あるいは別途regist系の関数などを
 *  用意すべきである.
 *
 *  これがfalseの場合は、単にこのDAryをポインタ値の一時的なコンテナとして使用し、
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
MoaiModuleAry
MoaiModuleAry_create( bool elem_responsibility );

/**
 * @brief DAryを破棄する.
 */
Znk_INLINE void
MoaiModuleAry_destroy( MoaiModuleAry dary ){
	ZnkObjDAry_M_DESTROY( dary );
}

Znk_INLINE size_t
MoaiModuleAry_size( const MoaiModuleAry dary ){
	return ZnkObjDAry_M_SIZE( dary );
}
Znk_INLINE MoaiModule
MoaiModuleAry_at( MoaiModuleAry dary, size_t idx ){
	return ZnkObjDAry_M_AT( dary, idx, MoaiModule );
}
Znk_INLINE MoaiModule*
MoaiModuleAry_dary_ptr( MoaiModuleAry dary ){
	return ZnkObjDAry_M_ARY_PTR( dary, MoaiModule );
}

Znk_INLINE bool
MoaiModuleAry_erase( MoaiModuleAry dary, MoaiModule obj ){
	return ZnkObjDAry_M_ERASE( dary, obj );
}
Znk_INLINE bool
MoaiModuleAry_erase_byIdx( MoaiModuleAry dary, size_t idx ){
	return ZnkObjDAry_M_ERASE_BY_IDX( dary, idx );
}
Znk_INLINE void
MoaiModuleAry_clear( MoaiModuleAry dary ){
	ZnkObjDAry_M_CLEAR( dary );
}
Znk_INLINE void
MoaiModuleAry_resize( MoaiModuleAry dary, size_t size ){
	ZnkObjDAry_M_RESIZE( dary, size );
}

Znk_INLINE void
MoaiModuleAry_push_bk( MoaiModuleAry dary, MoaiModule obj ){
	ZnkObjDAry_M_PUSH_BK( dary, obj );
}
Znk_INLINE void
MoaiModuleAry_set( MoaiModuleAry dary, size_t idx, MoaiModule obj ){
	ZnkObjDAry_M_SET( dary, idx, obj );
}

void
MoaiModuleAry_loadAllModules( MoaiModuleAry mod_ary, const ZnkMyf mtgt );

MoaiModule
MoaiModuleAry_find_byHostname( const MoaiModuleAry mod_ary, const ZnkMyf mtgt, const char* hostname );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
