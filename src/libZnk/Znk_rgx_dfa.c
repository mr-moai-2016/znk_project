#include "Znk_rgx_dfa.h"
#include <Znk_base.h>
#include <Znk_stdc.h>
#include <Znk_str.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG   1

/* DFAPathは遷移先のリストを表す型。
   文字cによって状態toDFASへ遷移する */
typedef struct DFAPath_tag {
	ZnkUChar64          c_;
	int                 toDFAS;
	struct DFAPath_tag* nextP; /* 次のデータへのポインタ */
} DFAPath;

/* ZnkRgxDFAStateはDFA状態を表す型 */
struct ZnkRgxDFAState_tag {
	ZnkNSet32  nset_;    /* このDFA状態が表すNFA状態集合 */
	int        visited;  /* 処理済みなら1 */
	int        accepted; /* 終了状態を含むなら1 */
	DFAPath*   nextP;    /* 遷移先を示すパスの集合(リストで実装) */
};



static void
deleteZnkRgxDFAState( ZnkRgxDFAState ds )
{
	if( ds ){
		DFAPath* p = ds->nextP;
		DFAPath* nextP;
		while( p ){
			nextP = p->nextP;
			Znk_free( p );
			p = nextP;
		}
		Znk_free( ds );
	}
}
static void
clearZnkRgxDFAStateAry( ZnkRgxDFAStateAry ary )
{
	size_t idx = 0;
	const size_t size = ZnkRgxDFAStateAry_size( ary );
	ZnkRgxDFAState ds;
	for( idx=0; idx<size; ++idx ){
		ds = (ZnkRgxDFAState)ZnkObjAry_at( (ZnkObjAry)ary, idx );
		deleteZnkRgxDFAState( ds );
	}
	ZnkObjAry_clear( (ZnkObjAry)ary );
}
ZnkRgxDFAStateAry
ZnkRgxDFAStateAry_create( void )
{
	ZnkObjAry ary = ZnkObjAry_create( NULL );
	return (ZnkRgxDFAStateAry)ary;
}
void
ZnkRgxDFAStateAry_destroy( ZnkRgxDFAStateAry ary )
{
	if( ary ){
		clearZnkRgxDFAStateAry( ary );
		ZnkObjAry_destroy( (ZnkObjAry)ary );
	}
}
void
ZnkRgxDFAStateAry_clear( ZnkRgxDFAStateAry ary )
{
	if( ary ){
		clearZnkRgxDFAStateAry( ary );
	}
}
size_t
ZnkRgxDFAStateAry_size( const ZnkRgxDFAStateAry ary )
{
	return ZnkObjAry_size( (ZnkObjAry)ary );
}
ZnkRgxDFAState
ZnkRgxDFAStateAry_at( const ZnkRgxDFAStateAry ary, size_t idx )
{
	return (ZnkRgxDFAState)ZnkObjAry_at( (ZnkObjAry)ary, idx );
}
static void
ZnkRgxDFAStateAry_push_bk( ZnkRgxDFAStateAry ary, ZnkRgxDFAState ds )
{
	ZnkObjAry_push_bk( (ZnkObjAry)ary, (ZnkObj)ds );
}



bool
ZnkRgxDFAState_isAccepted( ZnkRgxDFAState dfas )
{
	return (bool)( dfas->accepted != 0 );
}


/* DFAを表示する（デバッグ用） */
static void
dumpDFA( ZnkRgxDFAStateAry dfa_ary, ZnkRgxNFATable nfa_ary, ZnkStr msg )
{
	DFAPath *path;
	size_t i;
	const size_t dfa_size = ZnkRgxDFAStateAry_size( dfa_ary );
	ZnkRgxDFAState ds;

	ZnkStr_add( msg, "--- DFA -----\n" );
	for( i=0; i<dfa_size; ++i ){
		ds = ZnkRgxDFAStateAry_at( dfa_ary, i );
		ZnkStr_addf2( msg, "%2zu%c: ", i, ZnkRgxDFAState_isAccepted( ds ) ? 'A' : ' ' );
		for( path = ds->nextP; path != NULL; path = path->nextP ){
			if( path->c_.u64_ == ZnkRgx_DOT ){
				ZnkStr_addf2( msg, "DOT=>%d ", path->toDFAS );
			} else {
				ZnkStr_addf2( msg, "%c=>%d ", path->c_, path->toDFAS );
			}
		}
		ZnkStr_add( msg, "\n" );
	}
	
	for( i=0; i<dfa_size; ++i ){
		ds = ZnkRgxDFAStateAry_at( dfa_ary, i );
		ZnkStr_addf2( msg, "state %2zu%c = {", i, ZnkRgxDFAState_isAccepted( ds ) ? 'A' : ' ' );
		//ZnkNSet32_print_e( ds->nset_ );
		ZnkStr_add( msg, "}\n" );
	}
	ZnkStr_add( msg, "--- DFA end-----\n" );
}

/***
 * NFA状態集合nsetをDFAに登録して、DFA状態へのポインタを返す。
 * nsetが終了状態を含んでいれば、acceptedフラグをセットする。
 * すでにnsetがDFAに登録されていたら何もしない.
 */
static size_t
ZnkRgxDFAState_regist( ZnkRgxDFAStateAry dfa_ary, const ZnkNSet32 nset, int nfa_exit )
{
	size_t i;
	const size_t dfa_size = ZnkRgxDFAStateAry_size( dfa_ary );
	ZnkRgxDFAState ds;

	/* NFA状態nsetがすでにDFAに登録されていたら、何もしないでリターンする */
	for( i=0; i<dfa_size; ++i ){
		ds = ZnkRgxDFAStateAry_at( dfa_ary, i );
		if( ds && ZnkNSet32_eq( ds->nset_, nset) ){
			return i;
		}
	}
	
	/* ZnkRgxDFAState を新規作成 */
	ds = Znk_alloc0(sizeof(struct ZnkRgxDFAState_tag));
	/* DFAに必要な情報をセットする */
	ds->nset_    = ZnkNSet32_create();
	ZnkNSet32_copy( ds->nset_, nset );
	ds->visited  = 0;
	ds->accepted = ZnkNSet32_exist(nset, nfa_exit) ? 1 : 0;
	ds->nextP    = NULL;
	ZnkRgxDFAStateAry_push_bk( dfa_ary, ds );
	return dfa_size;
}

/* 処理済みの印がついていないDFA状態を探す。
   見つからなければNULLを返す */
static ZnkRgxDFAState
ZnkRgxDFAState_fetch_unvisited( ZnkRgxDFAStateAry dfa_ary )
{
	const size_t dfa_size = ZnkRgxDFAStateAry_size( dfa_ary );
	size_t i;
	ZnkRgxDFAState ds;
	for( i=0; i<dfa_size; ++i ){
		ds = ZnkRgxDFAStateAry_at( dfa_ary, i );
		if( ds && ds->visited == 0 ){
			return ds;
		}
	}
	return NULL;
}

/* 状態dfasから文字cによって遷移して、遷移後の状態を返す。
   文字cによって遷移できなければNULLを返す */
size_t
ZnkRgxDFAState_findNextState( ZnkRgxDFAState dfas, ZnkUChar64 c )
{
	DFAPath* p;
	for( p = dfas->nextP; p != NULL; p = p->nextP ){
		if( c.u64_ == p->c_.u64_ || p->c_.u64_ == ZnkRgx_DOT ){
			return p->toDFAS;
		}
	}
	return Znk_NPOS;
}


/* NFAを等価なDFAへと変換する */
ZnkRgxDFAState
ZnkRgxDFAState_convertFromNFA( ZnkRgxDFAStateAry dfa_ary, ZnkRgxNFATable nfa_ary, int nfa_entry, int nfa_exit )
{
	ZnkNSet32          initial_nset;
	ZnkRgxDFAState     ds;
	ZnkRgxNFAReachable rns;
	DFAPath*           dpath;
	size_t initial_dfa_state = 0;
#if DEBUG
	ZnkStr msg = ZnkStr_new( "" );
#else
	ZnkStr msg = NULL;
#endif
	
	/* DFAの初期状態を登録する */
	initial_nset = ZnkNSet32_create();
	ZnkNSet32_push( initial_nset, nfa_entry );
	ZnkRgxNFATable_collect_empty_transition( nfa_ary, initial_nset );
	initial_dfa_state = ZnkRgxDFAState_regist( dfa_ary, initial_nset, nfa_exit );
	
	/* 未処理のDFA状態があれば、それを取り出して処理する。 
	   （注目しているDFA状態をdsとする） */
	while( (ds = ZnkRgxDFAState_fetch_unvisited(dfa_ary)) != NULL ){
		/* 処理済みの印をつける */
		ds->visited = 1;

		/* 状態dsから遷移可能なDFA状態をすべてDFAに登録する。
		   rnsは(文字, NFA状態集合)の対になっている */
		rns = ZnkRgxNFAReachable_create( nfa_ary, ds->nset_ );

		for( ; rns != NULL; rns = ZnkRgxNFAReachable_next(rns) ){
			/* NFA状態集合のε-closureを求める */
			ZnkRgxNFATable_collect_empty_transition( nfa_ary, ZnkRgxNFAReachable_nset(rns) );
			/* dpathを新規作成 */
			dpath = Znk_alloc0(sizeof(DFAPath));
			dpath->c_ = ZnkRgxNFAReachable_c( rns );
			/* NFA状態集合をDFAに登録する */
			dpath->toDFAS = ZnkRgxDFAState_regist( dfa_ary, ZnkRgxNFAReachable_nset(rns), nfa_exit );
			/* dsにdpathを追加 */
			dpath->nextP = ds->nextP;
			ds->nextP = dpath;
		}
		ZnkRgxNFAReachable_destroy( rns );
	}

	if( msg ){
		dumpDFA( dfa_ary, nfa_ary, msg );
		ZnkStr_delete( msg );
	}

	return ZnkRgxDFAStateAry_at( dfa_ary, initial_dfa_state );
}


