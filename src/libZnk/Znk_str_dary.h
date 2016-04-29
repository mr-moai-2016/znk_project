#ifndef INCLUDE_GUARD__Znk_str_dary_h__
#define INCLUDE_GUARD__Znk_str_dary_h__

#include <Znk_obj_dary.h>
#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkStrDAry );

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
ZnkStrDAry
ZnkStrDAry_create( bool elem_responsibility );

/**
 * @brief DAryを破棄する.
 */
Znk_INLINE void
ZnkStrDAry_destroy( ZnkStrDAry dary ){
	ZnkObjDAry_M_DESTROY( dary );
}

Znk_INLINE size_t
ZnkStrDAry_size( const ZnkStrDAry dary ){
	return ZnkObjDAry_M_SIZE( dary );
}
Znk_INLINE ZnkStr
ZnkStrDAry_at( ZnkStrDAry dary, size_t idx ){
	return ZnkObjDAry_M_AT( dary, idx, ZnkStr );
}
Znk_INLINE ZnkStr*
ZnkStrDAry_dary_ptr( ZnkStrDAry dary ){
	return ZnkObjDAry_M_ARY_PTR( dary, ZnkStr );
}

Znk_INLINE bool
ZnkStrDAry_erase( ZnkStrDAry dary, ZnkStr obj ){
	return ZnkObjDAry_M_ERASE( dary, obj );
}
Znk_INLINE bool
ZnkStrDAry_erase_byIdx( ZnkStrDAry dary, size_t idx ){
	return ZnkObjDAry_M_ERASE_BY_IDX( dary, idx );
}
Znk_INLINE void
ZnkStrDAry_clear( ZnkStrDAry dary ){
	ZnkObjDAry_M_CLEAR( dary );
}
Znk_INLINE void
ZnkStrDAry_resize( ZnkStrDAry dary, size_t size ){
	ZnkObjDAry_M_RESIZE( dary, size );
}

Znk_INLINE void
ZnkStrDAry_push_bk( ZnkStrDAry dary, ZnkStr obj ){
	ZnkObjDAry_M_PUSH_BK( dary, obj );
}
Znk_INLINE void
ZnkStrDAry_set( ZnkStrDAry dary, size_t idx, ZnkStr obj ){
	ZnkObjDAry_M_SET( dary, idx, obj );
}
Znk_INLINE void
ZnkStrDAry_swap( ZnkStrDAry dary1, ZnkStrDAry dary2 ){
	ZnkObjDAry_M_SWAP( dary1, dary2 );
}

int
ZnkStrDAry_push_bk_snprintf( ZnkStrDAry dary, size_t size, const char* fmt, ... );

void
ZnkStrDAry_push_bk_cstr( ZnkStrDAry dary, const char* cstr, size_t cstr_leng );

size_t
ZnkStrDAry_find( ZnkStrDAry dary, size_t pos, const char* ptn, size_t ptn_leng );

Znk_INLINE const char*
ZnkStrDAry_at_cstr( ZnkStrDAry dary, size_t idx ){
	return ZnkStr_cstr( ZnkStrDAry_at( dary, idx ) );
}
Znk_INLINE size_t
ZnkStrDAry_at_leng( ZnkStrDAry dary, size_t idx ){
	return ZnkStr_leng( ZnkStrDAry_at( dary, idx ) );
}

Znk_INLINE void
ZnkStrDAry_reserve( ZnkStrDAry dary, size_t size ){
	const size_t size_save = ZnkStrDAry_size( dary );
	ZnkObjDAry_M_RESIZE( dary, size );
	ZnkObjDAry_M_RESIZE( dary, size_save );
}

Znk_INLINE void
ZnkStrDAry_copy( ZnkStrDAry dst, const ZnkStrDAry src )
{
	const size_t size = ZnkStrDAry_size( src );
	size_t idx;
	ZnkStr str;
	ZnkStrDAry_clear( dst );
	for( idx=0; idx<size; ++idx ){
		str = ZnkStrDAry_at( src, idx );
		ZnkStrDAry_push_bk_cstr( dst, ZnkStr_cstr( str ), ZnkStr_leng( str ) );
	}
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
