#ifndef INCLUDE_GUARD__Znk_rgx_dfa_h__
#define INCLUDE_GUARD__Znk_rgx_dfa_h__

#include <Znk_rgx_base.h>
#include "Znk_rgx_nfa.h"

Znk_EXTERN_C_BEGIN

typedef struct ZnkRgxDFAState_tag* ZnkRgxDFAState;

Znk_DECLARE_HANDLE( ZnkRgxDFAStateAry );

ZnkRgxDFAStateAry
ZnkRgxDFAStateAry_create( void );
void
ZnkRgxDFAStateAry_destroy( ZnkRgxDFAStateAry ary );
void
ZnkRgxDFAStateAry_clear( ZnkRgxDFAStateAry ary );
size_t
ZnkRgxDFAStateAry_size( const ZnkRgxDFAStateAry ary );
ZnkRgxDFAState
ZnkRgxDFAStateAry_at( const ZnkRgxDFAStateAry ary, size_t idx );

bool
ZnkRgxDFAState_isAccepted( ZnkRgxDFAState dfas );

/* 状態dfasから文字cによって遷移して、遷移後の状態を返す。
   文字cによって遷移できなければNULLを返す */
size_t
ZnkRgxDFAState_findNextState( ZnkRgxDFAState dfas, ZnkUChar64 c );

/* NFAを等価なDFAへと変換する */
ZnkRgxDFAState
ZnkRgxDFAState_convertFromNFA( ZnkRgxDFAStateAry dfa_ary, ZnkRgxNFATable nfa_ary, int nfa_entry, int nfa_exit );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */

