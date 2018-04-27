#ifndef INCLUDE_GUARD__Znk_rgx_nfa_h__
#define INCLUDE_GUARD__Znk_rgx_nfa_h__

#include <Znk_base.h>
#include <Znk_obj_ary.h>
#include <Znk_nset.h>
#include "Znk_rgx_base.h"

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkRgxNFATable );

ZnkRgxNFATable
ZnkRgxNFATable_create( void );
void
ZnkRgxNFATable_destroy( ZnkRgxNFATable ary );
void
ZnkRgxNFATable_clear( ZnkRgxNFATable ary );
size_t
ZnkRgxNFATable_size( const ZnkRgxNFATable ary );
/**
 * 新しいノード番号を発行する.
 */
int
ZnkRgxNFATable_gen_node( ZnkRgxNFATable nfa_ary );

/**
 * NFAを表示する（デバッグ用）.
 */
void
ZnkRgxNFATable_dump( ZnkRgxNFATable nfa_ary, int nfa_entry, int nfa_exit );


/**
 * NFA状態集合nsetに対してε-follow操作を実行する.
 * ε遷移で遷移可能なすべてのNFA状態を追加する.
 */
void
ZnkRgxNFATable_collect_empty_transition( ZnkRgxNFATable nfa_ary, ZnkNSet32 nset );

/**
 * NFAに状態遷移を追加する.
 * 状態fromに対して、文字cのときに状態toNFAへの遷移を追加する.
 */
void
ZnkRgxNFATable_add_transition( ZnkRgxNFATable nfa_ary, int from, int toNFA, ZnkUChar64 c );


typedef struct ZnkRgxNFAReachable_tag* ZnkRgxNFAReachable;

/***
 * 与えられたZnkNSet32 nsetから遷移可能なNFA状態の集合を求める.
 * 結果は文字cを同値類としたリスト構造として作成される.
 */
ZnkRgxNFAReachable
ZnkRgxNFAReachable_create( ZnkRgxNFATable nfa_ary, const ZnkNSet32 nset );

void
ZnkRgxNFAReachable_destroy( ZnkRgxNFAReachable rns );

ZnkRgxNFAReachable
ZnkRgxNFAReachable_next( const ZnkRgxNFAReachable rns );

ZnkUChar64
ZnkRgxNFAReachable_c( const ZnkRgxNFAReachable rns );

ZnkNSet32
ZnkRgxNFAReachable_nset( const ZnkRgxNFAReachable rns );


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */

