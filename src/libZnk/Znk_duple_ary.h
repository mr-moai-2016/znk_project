#ifndef INCLUDE_GUARD__Znk_duple_ary_h__
#define INCLUDE_GUARD__Znk_duple_ary_h__

#include <Znk_prim.h>
#include <Znk_obj_ary.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkDupleAryImpl* ZnkDupleAry;

/**
 * @brief
 * val_ に関しては読みはもちろん、書き換えてもよい.
 * (これを構造体のメンバで直接行ってもよいし、下記アクセサを使ってもよい).
 * この書き換えはset処理に該当する. 例えば既にfindElemによりZnkDupleが
 * 得られている場合、ZnkDupleAry_setVal(*)のような関数よりはるかに高速である.
 * ここでは高速化のため、ZnkDupleの本当の実体(ZnkDuplePrivateImpl)内部を
 * 一部晒す形をとっている.
 */
struct ZnkDupleImpl {
	const void* val_;
};
typedef struct ZnkDupleImpl* ZnkDuple;

Znk_INLINE const void*
ZnkDuple_val( const ZnkDuple elem ){
	return elem->val_;
}
Znk_INLINE void
ZnkDuple_set_val( ZnkDuple elem, const void* val ){
	elem->val_ = val;
}

/**
 * @brief
 * この関数は使用方法によってはZnkDupleAryのデータ整合性を破壊する恐れがある.
 * できるだけ使用は自重し、使用する場合でも細心の注意を払うこと.
 * (特にこの値がZnkStrなどの文字列であった場合、その内容を変更してはならない)
 */
const void*
ZnkDuple_ikey_unsafe( const ZnkDuple elem );


/**
 * @brief
 * ZnkDupleAryの動作を規定するための関数群
 *
 * ZnkDupleKeyFuncT_eq:
 *   第1引数にはikey(ZnkDuplePrivate内で保持するキー)が与えられる.
 *   第2引数にはkey(ユーザ指定によるキー)が与えられる.
 *   この二つのキーが等しいかどうかを比較するための関数であり、
 *   等しい場合にはtrue、そうでない場合にはfalseを返すものとする.
 *
 * ZnkDupleKeyFuncT_makeIKey:
 *   引数にはkey(ユーザ指定によるキー)が与えられる.
 *   戻り値としてkeyより生成されたikey(ZnkDuplePrivate内で保持するキー)を
 *   返すものとする.
 *
 * ZnkDupleKeyFuncT_copyIKey:
 *   引数にはikey(ZnkDuplePrivate内で保持するキー)が与えられる.
 *   戻り値としてikeyよりクローン生成されたikeyを返すものとする.
 *
 * ZnkDupleKeyFuncT_deleteIKey:
 *   引数にはikey(ZnkDuplePrivate内で保持するキー)が与えられる.
 *   与えられたikeyを解放する方法を規定するための関数である.
 *
 *
 * ZnkDupleValFuncT_copyVal:
 *   引数にはvalが与えられる.
 *   戻り値としてvalよりクローン生成されたval値を返すものとする.
 *
 * ZnkDupleValFuncT_deleteVal:
 *   引数にはvalが与えられる.
 *   与えられたvalを解放する方法を規定するための関数である.
 */
typedef bool        (*ZnkDupleKeyFuncT_eq)( const void* ikey, const void* key );
typedef const void* (*ZnkDupleKeyFuncT_makeIKey)( const void* );
typedef const void* (*ZnkDupleKeyFuncT_copyIKey)( const void* );
typedef void        (*ZnkDupleKeyFuncT_deleteIKey)( void* );

typedef struct ZnkDupleKeyFuncs_tag {
	ZnkDupleKeyFuncT_eq         eq_;
	ZnkDupleKeyFuncT_makeIKey   make_ikey_;
	ZnkDupleKeyFuncT_copyIKey   copy_ikey_;
	ZnkDupleKeyFuncT_deleteIKey delete_ikey_;
}ZnkDupleKeyFuncs;

typedef const void* (*ZnkDupleValFuncT_copyVal)( const void* );
typedef void        (*ZnkDupleValFuncT_deleteVal)( void* );

typedef struct ZnkDupleValFuncs_tag {
	ZnkDupleValFuncT_copyVal   copy_val_;
	ZnkDupleValFuncT_deleteVal delete_val_;
}ZnkDupleValFuncs;


ZnkDupleAry
ZnkDupleAry_create(
		const ZnkDupleKeyFuncs* key_funcs,
		const ZnkDupleValFuncs* val_funcs );
void
ZnkDupleAry_destroy( ZnkDupleAry dupa );

size_t
ZnkDupleAry_size( const ZnkDupleAry dupa );

ZnkDuple
ZnkDupleAry_atElem( ZnkDupleAry dupa, size_t idx );

ZnkDuple
ZnkDupleAry_findElem( ZnkDupleAry dupa, const void* key );

ZnkDuple
ZnkDupleAry_regist( ZnkDupleAry dupa, const void* key, const void* val, bool is_update, bool* is_newly );

void
ZnkDupleAry_swap( ZnkDupleAry ht1, ZnkDupleAry ht2 );
void
ZnkDupleAry_copy( ZnkDupleAry dst, const ZnkDupleAry src );

void
ZnkDupleAry_clear( ZnkDupleAry dupa );

bool
ZnkDupleAry_erase( ZnkDupleAry dupa, const void* key );


/**
 * @brief
 * ZnkDupleAry_createの引数として使える典型的なZnkDupleKeyFuncsを集めたもの.
 * 以下でkeyとはユーザが指定する方のkeyを意味する.
 *
 * uval : 
 *   keyとikeyがともにunsignedな整数である実装
 * cstr : 
 *   keyとikeyがともにconst char* である実装
 *   keyとikeyは完全に同一かつ同じ場所を示したポインタであり、ikeyへのstrdupは行われない.
 *   少なくともこのHashTableの寿命が続く間はkeyの寿命が続く必要がある.
 * strdup : 
 *   keyとikeyがともにconst char* である実装
 *   keyとは別にikeyとしてstrdupによるコピーとメモリ確保が行われる.
 *   指定したユーザkeyは、ユーザが直ちに破棄しても問題はない.
 * ZnkStr : 
 *   keyがconst char*であり、ikeyがZnkStr である実装
 *   指定したユーザkeyは、ユーザが直ちに破棄しても問題はない.
 */
const ZnkDupleKeyFuncs* ZnkDupleKeyFuncs_get_uval( void );
const ZnkDupleKeyFuncs* ZnkDupleKeyFuncs_get_cstr( void );
const ZnkDupleKeyFuncs* ZnkDupleKeyFuncs_get_strdup( void );
const ZnkDupleKeyFuncs* ZnkDupleKeyFuncs_get_ZnkStr( void );

/**
 * @brief
 * ZnkDupleAry_createの引数として使える典型的なZnkDupleValFuncsを集めたもの.
 *
 * uval : 
 *   valがunsignedな整数である実装.
 *   ただし、uvalという名前ではあるが、実際には unsigned intに限らず
 *   C言語的に = 演算子で代入可能かつ解放に特別な関数を必要としないスカラー量ならば
 *   すべてこれでうまくいく.
 */
const ZnkDupleValFuncs* ZnkDupleValFuncs_get_uval( void );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
