#ifndef INCLUDE_GUARD__Znk_s_atom_h__
#define INCLUDE_GUARD__Znk_s_atom_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkSAtomDBImpl* ZnkSAtomDB;

typedef uintptr_t ZnkSAtomId;

ZnkSAtomDB ZnkSAtomDB_create( void );
void       ZnkSAtomDB_destroy( ZnkSAtomDB atomdb );

/**
 * @brief
 *   C文字列 key の「内容」に対応する一意な id (ZnkSAtomId)を返す.
 *   このとき、key の内容は、atomdbが示す文字列DBへ保存される.
 *   戻り値である id の実体は、このDBにおける key の内容が保存されたインデックスを返す数値であり、
 *   次に同じ「内容」の key が指定された場合は、前回登録されたときと同じ値の id が返されるだけで
 *   新たな登録は行われない. つまり、この関数では指定されたkeyと同じ「内容」のものが既に存在して
 *   いるかどうかを毎回内部シークしているためあまり高速ではない.
 *   keyのポインタ値としての比較ではなく、あくまでそれが示す文字列の内容が同じであるか否かで
 *   判断している点に注意する.
 *
 * @return
 *   intern処理した結果得られた、keyの内容に対応するid(ZnkSAtomId)を返す.
 *
 * @note
 *   この処理はX Window SystemのAtomにおけるintern処理を真似たものである.
 */
ZnkSAtomId    ZnkSAtomDB_intern( ZnkSAtomDB atomdb, const char* key );

/**
 * @brief
 *   id が示す文字列を atomdb から参照して返す.
 *   id が示す文字列とは、ZnkSAtomDB_intern 呼び出し時に登録した文字列である.
 *
 * @return
 *   id が示す文字列のポインタであり、NULL終端したC文字列である.
 *   これが示す文字列ポインタは、id が対応する atomdb が、ZnkSAtomDB_destroyによって破棄されない限り
 *   無効になることはない.
 *
 * @note
 *   戻り値となる文字列のポインタは内部ZnkVStrの各要素であるZnkStrがさらに内部で保持するポインタである.
 *   従って、単純にこのZnkVStrがリサイズなどされた場合に、このポインタへの影響を心配する必要はない.
 */
const char* ZnkSAtomDB_cstr( ZnkSAtomDB atomdb, ZnkSAtomId id );

void
ZnkSAtomDB_swap( ZnkSAtomDB atomdb1, ZnkSAtomDB atomdb2 );

/***
 * ZnkSAtomId と ZnkSAtomDBのセット.
 * 以下はユーザヘルパーデータと関数である.
 */
typedef struct {
	ZnkSAtomId id_;
	ZnkSAtomDB db_;
} ZnkSAtomPr;

Znk_INLINE void
ZnkSAtomPr_intern( ZnkSAtomPr* ans_atompr, ZnkSAtomDB atomdb, const char* key )
{
	ans_atompr->id_ = ZnkSAtomDB_intern( atomdb, key );
	ans_atompr->db_ = atomdb;
}
Znk_INLINE const char*
ZnkSAtomPr_cstr( const ZnkSAtomPr* atompr )
{
	return ZnkSAtomDB_cstr( atompr->db_, atompr->id_ );
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
