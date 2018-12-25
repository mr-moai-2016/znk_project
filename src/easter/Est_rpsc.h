#ifndef INCLUDE_GUARD__Est_replacer_h__
#define INCLUDE_GUARD__Est_replacer_h__

#include <Rano_txt_filter.h>

#include <Znk_obj_ary.h>
#include <Znk_str.h>
#include <Znk_str_ary.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 EstRpsc_e_Replace=0
	,EstRpsc_e_ReplaceEx
	,EstRpsc_e_ClearT
	,EstRpsc_e_ClearF
	,EstRpsc_e_ClearSrcT
	,EstRpsc_e_ClearSrcF
} EstRpscType;

bool
EstRpsc_exec( RanoTxtFilterAry fltr_ary, ZnkStr text, void* arg );

bool
EstRpsc_compile( RanoTxtFilterAry fltr_ary, const ZnkStrAry cmds );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
