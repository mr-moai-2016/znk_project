#ifndef INCLUDE_GUARD__Moai_post_vars_h__
#define INCLUDE_GUARD__Moai_post_vars_h__

#include <Znk_str.h>
#include <Znk_var.h>
#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 MoaiPostVar_e_None=0 /* text/plain‚Æ“¯“™‚Æ‚µ‚Äˆµ‚¤ */
	,MoaiPostVar_e_BinaryData
}MoaiPostVarType;

ZnkVarp
MoaiPostVars_regist( ZnkVarpAry vars, const char* name, const char* filename,
		MoaiPostVarType type, const uint8_t* data, size_t data_size );
bool
MoaiPostVars_regist_byHttpBody( ZnkVarpAry vars,
		const char* boundary, size_t boundary_leng,
		const uint8_t* body, size_t body_size );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
