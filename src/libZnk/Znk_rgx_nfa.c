#include "Znk_rgx_nfa.h"
#include <Znk_base.h>
#include <Znk_stdc.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG   1

/***
 * 構文木からNFAを生成する
 */

/* NFAStateはNFAにおける遷移を表す型。
   文字cによって状態toNFAへと遷移する  */
typedef struct NFAState_tag {
	ZnkUChar64           c_;
	int                  toNFA;
	struct NFAState_tag* next;   /* 次のデータへのポインタ */
} NFAState;

/***
 * NFAの状態遷移表
 * from0 => (to1,c1) (to2,c2), (to3,c3), ...
 * from1 => (to1,c1) (to2,c2), (to3,c3), ...
 * from2 => (to1,c1) (to2,c2), (to3,c3), ...
 * というような構造をとる. (ハッシュテーブルを実装する場合のリストの配列に似ている)
 */

static void deleteNFAState( NFAState* ns )
{
	NFAState* next;
	while( ns ){
		next = ns->next;
		Znk_free( ns );
		ns = next;
	}
}
Znk_INLINE void
clearZnkRgxNFATable( ZnkRgxNFATable ary )
{
	size_t idx = 0;
	const size_t size = ZnkRgxNFATable_size( ary );
	for( idx=0; idx<size; ++idx ){
		NFAState* ns = (NFAState*)ZnkObjAry_at( (ZnkObjAry)ary, idx );
		deleteNFAState( ns );
	}
	ZnkObjAry_clear( (ZnkObjAry)ary );
}
ZnkRgxNFATable
ZnkRgxNFATable_create( void )
{
	/***
	 * このポインタの配列では、各要素はZnkObjAry_setにより頻繁に書き換わるにも関わらず、
	 * そのタイミングで旧要素値はnextに移動され、その実態もまだ解放せずに残しておかなければならない.
	 * このような特殊な構造のため、ZnkObjAry_setなどで自動的な解放がなされてはならない.
	 * 従ってdeleteElemにはNULLを指定しておく.
	 * 各要素の解放はdestroyのタイミングのみ一括してnextのものまで踏まえて行う.
	 */
	ZnkObjAry ary = ZnkObjAry_create( NULL );
	return (ZnkRgxNFATable)ary;
}
void
ZnkRgxNFATable_destroy( ZnkRgxNFATable ary )
{
	if( ary ){
		clearZnkRgxNFATable( ary );
		ZnkObjAry_destroy( (ZnkObjAry)ary );
	}
}
void
ZnkRgxNFATable_clear( ZnkRgxNFATable ary )
{
	if( ary ){
		clearZnkRgxNFATable( ary );
	}
}
size_t
ZnkRgxNFATable_size( const ZnkRgxNFATable ary )
{
	return ZnkObjAry_size( (ZnkObjAry)ary );
}
NFAState*
ZnkRgxNFATable_at( const ZnkRgxNFATable ary, size_t idx )
{
	return (NFAState*)ZnkObjAry_at( (ZnkObjAry)ary, idx );
}
void
ZnkRgxNFATable_set( ZnkRgxNFATable ary, size_t idx, NFAState* ns )
{
	ZnkObjAry_set( (ZnkObjAry)ary, idx, (ZnkObj)ns );
}


/* ノードに番号を割り当てる */
int ZnkRgxNFATable_gen_node( ZnkRgxNFATable nfa_ary )
{
	const size_t size = ZnkObjAry_size( (ZnkObjAry)nfa_ary );
	ZnkObjAry_push_bk( (ZnkObjAry)nfa_ary, NULL );
    return (int)size;
}

/* NFAを表示する（デバッグ用） */
void ZnkRgxNFATable_dump( ZnkRgxNFATable nfa_ary, int nfa_entry, int nfa_exit )
{
	size_t i;
	NFAState *p;
	const size_t size = ZnkRgxNFATable_size( nfa_ary );

	Znk_printf("--- NFA -----\n");
	Znk_printf("state entry=%3d exit=%3d\n", nfa_entry, nfa_exit);
	for( i = 0; i <size; ++i ) {
		p = ZnkRgxNFATable_at( nfa_ary, i );
		if( p ){
			Znk_printf("state %3zu: ", i);
			for( ; p != NULL; p = p->next) {
				if( p->c_.u64_ == ZnkRgx_DOT ){
					Znk_printf("(DOT :toNFA=%d) ", p->toNFA);
				} else if( p->c_.u64_ == ZnkRgx_EMPTY ){
					Znk_printf("(EMPTY :toNFA=%d) ", p->toNFA);
				} else {
					Znk_printf("(%s(%08I64x) :toNFA=%d) ", p->c_.s_, p->c_.u64_, p->toNFA);
				}
			}
			Znk_printf("\n");
		}
	}
	Znk_printf("--- NFA end-----\n");
}



/* NFA状態集合nsetにNFA状態sを追加する。 
   同時にNFA状態sからε遷移で移動できるNFA状態も追加する */
static void mark_empty_transition( ZnkRgxNFATable nfa_ary, ZnkNSet32 nset, int s )
{
	NFAState* p;
	ZnkNSet32_push(nset, s);
	p = ZnkRgxNFATable_at( nfa_ary, s );
	for( ; p != NULL; p = p->next ){
		if( p->c_.u64_ == ZnkRgx_EMPTY && !ZnkNSet32_exist(nset, p->toNFA) ){
			mark_empty_transition( nfa_ary, nset, p->toNFA );
		}
	}
}

/* NFA状態集合nsetに対してε-follow操作を実行する。
   ε遷移で遷移可能なすべてのNFA状態を追加する */
void ZnkRgxNFATable_collect_empty_transition( ZnkRgxNFATable nfa_ary, ZnkNSet32 nset )
{
	/* TODO: nsetのイテレートを設け、それをイテレートさせればよいだけではないか ? */
	size_t i;
	const size_t size = ZnkRgxNFATable_size( nfa_ary );
	for ( i = 0; i < size; ++i ) {
		if( ZnkNSet32_exist(nset, i) ){
			mark_empty_transition( nfa_ary, nset, i);
		}
	}
}

void
ZnkRgxNFATable_add_transition( ZnkRgxNFATable nfa_ary, int from, int toNFA, ZnkUChar64 c )
{
	NFAState* new;
	new = (NFAState *)Znk_alloc0(sizeof(NFAState));
	new->c_    = c;
	new->toNFA = toNFA;
	new->next  = ZnkRgxNFATable_at( nfa_ary, from );
	ZnkRgxNFATable_set( nfa_ary, from, new );
}



/***
 * NFA2DFA 部分集合構成法に関する補助ルーチン.
 */

struct ZnkRgxNFAReachable_tag {
	ZnkUChar64                  c_;
	ZnkNSet32                   nset_;
	struct ZnkRgxNFAReachable_tag* nextRNS; /* 次のデータへのポインタ */
};

/***
 * query_cによってtoNFAへ遷移し得るNFA(ZnkRgxNFAReachable)を一つ追加する.
 * ZnkRgxNFAReachableはcをキーとするようなリストとなっている.
 * すなわちZnkRgxNFAReachableのノードをrns(N)とすると
 * rns(0) c0 : nset(toNFA0, toNFA1, toNFA2...)
 * rns(1) c1 : nset(toNFA0, toNFA1, toNFA2...)
 * rns(2) c2 : nset(toNFA0, toNFA1, toNFA2...)
 * といったような構造を持つ.
 */
static ZnkRgxNFAReachable
internZnkRgxNFAReachable( ZnkRgxNFAReachable rns, ZnkUChar64 query_c, int toNFA )
{
	ZnkRgxNFAReachable new;
	ZnkRgxNFAReachable a = rns;
	bool found = false;

	if( query_c.u64_ == ZnkRgx_DOT ){
		/***
		 * すべての文字についてtoNFAへの遷移を追加する.
		 */
		for( ; a != NULL; a=a->nextRNS ){
			if( a->c_.u64_ == ZnkRgx_DOT ){
				/* found */
				found = true;
			}
			ZnkNSet32_push(a->nset_, toNFA);
		}
	} else {
		/* 文字cに関する ZnkRgxNFAReachableを探索し、既存のものが見つかった場合はそのnsetへtoNFAを一つ追加 */
		for( ; a != NULL; a=a->nextRNS ){
			if( a->c_.u64_ == query_c.u64_ ){
				/* found */
				ZnkNSet32_push(a->nset_, toNFA);
				found = true;
				break;
			}
		}
	}

	if( found ){
		return rns;
	}

	/* not found */
	
	/* 文字cに関する ZnkRgxNFAReachableを新規に割り当て、リストに追加 */
	new = Znk_alloc0( sizeof(struct ZnkRgxNFAReachable_tag) );
	new->c_ = query_c;
	new->nset_ = ZnkNSet32_create();
	new->nextRNS = rns;
	
	/* nsetへtoNFAを一つ追加 */
	ZnkNSet32_push(new->nset_, toNFA);
	
	return new;
}


/***
 * 与えられたZnkNSet32 nsetから遷移可能なNFA状態の集合を求める.
 * 結果はリスト構造として作成される.
 */
ZnkRgxNFAReachable
ZnkRgxNFAReachable_create( ZnkRgxNFATable nfa_ary, const ZnkNSet32 nset )
{
	size_t i;
	NFAState* p;
	ZnkRgxNFAReachable result = NULL;
	const size_t size = ZnkRgxNFATable_size( nfa_ary );
	
	/* TODO: nsetのイテレートを設け、それをイテレートさせればよいだけではないか ? */
	
	/* NFA状態テーブルを順に調べる */
	for( i=0; i<size; ++i ){
		/* NFA状態iがnsetに含まれていれば、以下の処理を行う */
		if( ZnkNSet32_exist(nset, i) ){
			/* NFA状態iから遷移可能なNFA状態をすべてイテレート.
			 * ただしε遷移は除外する */
			p = ZnkRgxNFATable_at( nfa_ary, i );
			for( ; p != NULL; p = p->next ){
				if( p->c_.u64_ != ZnkRgx_EMPTY ){
					result = internZnkRgxNFAReachable( result, p->c_, p->toNFA );
				}
			}
		}
	}
	
	/* 作成したリストを返す */
	return result;
}

void
ZnkRgxNFAReachable_destroy( ZnkRgxNFAReachable rns )
{
	if( rns ){
		ZnkNSet32_destroy( rns->nset_ );
		Znk_free( rns );
	}
}

ZnkRgxNFAReachable
ZnkRgxNFAReachable_next( const ZnkRgxNFAReachable rns )
{
	return rns->nextRNS;
}

ZnkUChar64
ZnkRgxNFAReachable_c( const ZnkRgxNFAReachable rns )
{
	return rns->c_;
}

ZnkNSet32
ZnkRgxNFAReachable_nset( const ZnkRgxNFAReachable rns )
{
	return rns->nset_;
}


