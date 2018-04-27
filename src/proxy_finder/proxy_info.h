#ifndef INCLUDE_GUARD__proxy_list_h__
#define INCLUDE_GUARD__proxy_list_h__

#include <Znk_obj_ary.h>
#include <Znk_stdc.h>

Znk_EXTERN_C_BEGIN

typedef struct ProxyInfoImpl* ProxyInfo;

/**
 * BREK : proxyとして機能していない(BREaK)
 * RFUS : サーバが接続を拒絶する(ReFUSed)
 * NTJP : Not jpと表示される
 * ACNG : アクセス規制中
 * ACOK : BBS等へ書込み可能
 * UNKN : 未調査(UnKnown)
 * UCNT : サーバが起動していない(UnConnect)
 * MBON : Mobile(携帯/スマホ)のみ書込み可
 * PCON : Computer(PC)のみ書込み可
 */
typedef enum {
	 ProxyState_e_UNKN = 0
	,ProxyState_e_ACOK
	,ProxyState_e_ACNG
	,ProxyState_e_RFUS
	,ProxyState_e_NTJP
	,ProxyState_e_BREK
	,ProxyState_e_UCNT
	,ProxyState_e_MBON
	,ProxyState_e_PCON
}ProxyStateCode;

Znk_INLINE const char*
ProxyStateCode_toStr( const ProxyStateCode query_code )
{
	const char* str = "ProxyState_e_UNKN";
	switch( query_code ){
	case Znk_BIND_STR( :str=, ProxyState_e_UNKN ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_ACOK ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_ACNG ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_RFUS ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_NTJP ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_BREK ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_UCNT ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_MBON ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_PCON ); break;
	default: break;
	}
	return str + Znk_strlen_literal( "ProxyState_e_" );
}

ProxyStateCode
ProxyStateCode_getCode_fromStr( const char* query_str );


Znk_DECLARE_HANDLE( ProxyInfoAry );

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
ProxyInfoAry
ProxyInfoAry_create( bool elem_responsibility );

/**
 * @brief Aryを破棄する.
 */
Znk_INLINE void
ProxyInfoAry_destroy( ProxyInfoAry dary ){
	ZnkObjAry_M_DESTROY( dary );
}

Znk_INLINE size_t
ProxyInfoAry_size( const ProxyInfoAry dary ){
	return ZnkObjAry_M_SIZE( dary );
}
Znk_INLINE ProxyInfo
ProxyInfoAry_at( ProxyInfoAry dary, size_t idx ){
	return ZnkObjAry_M_AT( dary, idx, ProxyInfo );
}
Znk_INLINE ProxyInfo*
ProxyInfoAry_dary_ptr( ProxyInfoAry dary ){
	return ZnkObjAry_M_ARY_PTR( dary, ProxyInfo );
}

Znk_INLINE bool
ProxyInfoAry_erase( ProxyInfoAry dary, ProxyInfo obj ){
	return ZnkObjAry_M_ERASE( dary, obj );
}
Znk_INLINE bool
ProxyInfoAry_erase_byIdx( ProxyInfoAry dary, size_t idx ){
	return ZnkObjAry_M_ERASE_BY_IDX( dary, idx );
}
Znk_INLINE void
ProxyInfoAry_clear( ProxyInfoAry dary ){
	ZnkObjAry_M_CLEAR( dary );
}
Znk_INLINE void
ProxyInfoAry_resize( ProxyInfoAry dary, size_t size ){
	ZnkObjAry_M_RESIZE( dary, size );
}

Znk_INLINE void
ProxyInfoAry_push_bk( ProxyInfoAry dary, ProxyInfo obj ){
	ZnkObjAry_M_PUSH_BK( dary, obj );
}
Znk_INLINE void
ProxyInfoAry_set( ProxyInfoAry dary, size_t idx, ProxyInfo obj ){
	ZnkObjAry_M_SET( dary, idx, obj );
}


void
ProxyInfoAry_regist( ProxyInfoAry list,
		const char* ip, uint16_t port, ProxyStateCode state_code, const char* hostname );

bool
ProxyInfoAry_load( ProxyInfoAry list, const char* filename );

bool
ProxyInfoAry_save( const ProxyInfoAry list, const char* filename, bool with_statecode );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
