#ifndef INCLUDE_GUARD__Moai_module_ary_h__
#define INCLUDE_GUARD__Moai_module_ary_h__

#include <Znk_obj_ary.h>
#include <Moai_module.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( MoaiModuleAry );


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
MoaiModuleAry
MoaiModuleAry_create( bool elem_responsibility );

/**
 * @brief Aryを破棄する.
 */
Znk_INLINE void
MoaiModuleAry_destroy( MoaiModuleAry ary ){
	ZnkObjAry_M_DESTROY( ary );
}

Znk_INLINE size_t
MoaiModuleAry_size( const MoaiModuleAry ary ){
	return ZnkObjAry_M_SIZE( ary );
}
Znk_INLINE MoaiModule
MoaiModuleAry_at( MoaiModuleAry ary, size_t idx ){
	return ZnkObjAry_M_AT( ary, idx, MoaiModule );
}
Znk_INLINE MoaiModule*
MoaiModuleAry_ary_ptr( MoaiModuleAry ary ){
	return ZnkObjAry_M_ARY_PTR( ary, MoaiModule );
}

Znk_INLINE bool
MoaiModuleAry_erase( MoaiModuleAry ary, MoaiModule obj ){
	return ZnkObjAry_M_ERASE( ary, obj );
}
Znk_INLINE bool
MoaiModuleAry_erase_byIdx( MoaiModuleAry ary, size_t idx ){
	return ZnkObjAry_M_ERASE_BY_IDX( ary, idx );
}
Znk_INLINE void
MoaiModuleAry_clear( MoaiModuleAry ary ){
	ZnkObjAry_M_CLEAR( ary );
}
Znk_INLINE void
MoaiModuleAry_resize( MoaiModuleAry ary, size_t size ){
	ZnkObjAry_M_RESIZE( ary, size );
}

Znk_INLINE void
MoaiModuleAry_push_bk( MoaiModuleAry ary, MoaiModule obj ){
	ZnkObjAry_M_PUSH_BK( ary, obj );
}
Znk_INLINE void
MoaiModuleAry_set( MoaiModuleAry ary, size_t idx, MoaiModule obj ){
	ZnkObjAry_M_SET( ary, idx, obj );
}

void
MoaiModuleAry_loadAllModules( MoaiModuleAry mod_ary, const ZnkMyf mtgt );

MoaiModule
MoaiModuleAry_find_byHostname( const MoaiModuleAry mod_ary, const char* hostname );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
