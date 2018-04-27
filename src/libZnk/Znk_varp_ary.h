#ifndef Znk_varp_ary_h__INCLUDED__
#define Znk_varp_ary_h__INCLUDED__

#include <Znk_obj_ary.h>
#include <Znk_var.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkVarpAry );


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
ZnkVarpAry
ZnkVarpAry_create( bool elem_responsibility );

/**
 * @brief Aryを破棄する.
 */
Znk_INLINE void
ZnkVarpAry_destroy( ZnkVarpAry ary ){
	ZnkObjAry_M_DESTROY( ary );
}

Znk_INLINE size_t
ZnkVarpAry_size( const ZnkVarpAry ary ){
	return ZnkObjAry_M_SIZE( ary );
}
Znk_INLINE ZnkVarp
ZnkVarpAry_at( ZnkVarpAry ary, size_t idx ){
	return ZnkObjAry_M_AT( ary, idx, ZnkVarp );
}
Znk_INLINE ZnkVarp*
ZnkVarpAry_ary_ptr( ZnkVarpAry ary ){
	return ZnkObjAry_M_ARY_PTR( ary, ZnkVarp );
}

Znk_INLINE bool
ZnkVarpAry_erase( ZnkVarpAry ary, ZnkVarp obj ){
	return ZnkObjAry_M_ERASE( ary, obj );
}
Znk_INLINE bool
ZnkVarpAry_erase_byIdx( ZnkVarpAry ary, size_t idx ){
	return ZnkObjAry_M_ERASE_BY_IDX( ary, idx );
}
Znk_INLINE void
ZnkVarpAry_clear( ZnkVarpAry ary ){
	ZnkObjAry_M_CLEAR( ary );
}
Znk_INLINE void
ZnkVarpAry_resize( ZnkVarpAry ary, size_t size ){
	ZnkObjAry_M_RESIZE( ary, size );
}

Znk_INLINE void
ZnkVarpAry_push_bk( ZnkVarpAry ary, ZnkVarp obj ){
	ZnkObjAry_M_PUSH_BK( ary, obj );
}
Znk_INLINE void
ZnkVarpAry_set( ZnkVarpAry ary, size_t idx, ZnkVarp obj ){
	ZnkObjAry_M_SET( ary, idx, obj );
}
size_t
ZnkVarpAry_findIdx_byName( ZnkVarpAry ary,
		const char* query_name, size_t query_name_leng, bool use_eqCase );
Znk_INLINE ZnkVarp
ZnkVarpAry_findObj_byName( ZnkVarpAry ary,
		const char* query_name, size_t query_name_leng, bool use_eqCase )
{
	const size_t idx = ZnkVarpAry_findIdx_byName( ary, query_name, query_name_leng, use_eqCase );
	return ( idx == Znk_NPOS ) ? NULL : ZnkVarpAry_at( ary, idx );
}
size_t
ZnkVarpAry_findIdx_byStrVal( ZnkVarpAry ary,
		const char* query_val, size_t query_val_leng );

/* 他の同系統のAryモジュールに仕様にあわせるなら本来findはidxを返すようにすべきであった.
 * 移行に時間がかかるため、しばらくこのマクロをかぶせる */
#define ZnkVarpAry_find_byName ZnkVarpAry_findObj_byName
#define ZnkVarpAry_find_byName_literal( ary, query_name, use_eqCase ) \
	ZnkVarpAry_find_byName( ary, query_name, Znk_strlen_literal(query_name), use_eqCase )

/* 新しく記述する場合はこちらを使うこと.
 */
#define ZnkVarpAry_findIdx_byName_literal( ary, query_name, use_eqCase ) \
	ZnkVarpAry_findIdx_byName( ary, query_name, Znk_strlen_literal(query_name), use_eqCase )
#define ZnkVarpAry_findObj_byName_literal( ary, query_name, use_eqCase ) \
	ZnkVarpAry_findObj_byName( ary, query_name, Znk_strlen_literal(query_name), use_eqCase )

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
