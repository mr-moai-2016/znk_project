#ifndef INCLUDE_GUARD__Rano_module_ary_h__
#define INCLUDE_GUARD__Rano_module_ary_h__

#include <Znk_obj_ary.h>
#include <Znk_str_ary.h>
#include <Rano_module.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( RanoModuleAry );


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
RanoModuleAry
RanoModuleAry_create( bool elem_responsibility );

/**
 * @brief Aryを破棄する.
 */
Znk_INLINE void
RanoModuleAry_destroy( RanoModuleAry ary ){
	ZnkObjAry_M_DESTROY( ary );
}

Znk_INLINE size_t
RanoModuleAry_size( const RanoModuleAry ary ){
	return ZnkObjAry_M_SIZE( ary );
}
Znk_INLINE RanoModule
RanoModuleAry_at( RanoModuleAry ary, size_t idx ){
	return ZnkObjAry_M_AT( ary, idx, RanoModule );
}
Znk_INLINE RanoModule*
RanoModuleAry_ary_ptr( RanoModuleAry ary ){
	return ZnkObjAry_M_ARY_PTR( ary, RanoModule );
}

Znk_INLINE bool
RanoModuleAry_erase( RanoModuleAry ary, RanoModule obj ){
	return ZnkObjAry_M_ERASE( ary, obj );
}
Znk_INLINE bool
RanoModuleAry_erase_byIdx( RanoModuleAry ary, size_t idx ){
	return ZnkObjAry_M_ERASE_BY_IDX( ary, idx );
}
Znk_INLINE void
RanoModuleAry_clear( RanoModuleAry ary ){
	ZnkObjAry_M_CLEAR( ary );
}
Znk_INLINE void
RanoModuleAry_resize( RanoModuleAry ary, size_t size ){
	ZnkObjAry_M_RESIZE( ary, size );
}

Znk_INLINE void
RanoModuleAry_push_bk( RanoModuleAry ary, RanoModule obj ){
	ZnkObjAry_M_PUSH_BK( ary, obj );
}
Znk_INLINE void
RanoModuleAry_set( RanoModuleAry ary, size_t idx, RanoModule obj ){
	ZnkObjAry_M_SET( ary, idx, obj );
}

void
RanoModuleAry_loadAllModules( RanoModuleAry mod_ary, const ZnkMyf mtgt,
		const char* filters_dir, const char* plugins_dir );

RanoModule
RanoModuleAry_find_byHostname( const RanoModuleAry mod_ary, const char* hostname );

void
RanoModuleAry_initiateFilters( RanoModuleAry mod_ary, ZnkStrAry result_msgs );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
