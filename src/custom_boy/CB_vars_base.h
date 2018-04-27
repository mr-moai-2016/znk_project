#ifndef INCLUDE_GUARD__CB_vars_base_h__
#define INCLUDE_GUARD__CB_vars_base_h__

#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

void
CBVarsBase_addNewStr_toVars( ZnkVarpAry vars, const char* var_name, const char* val, size_t val_leng );
void
CBVarsBase_registStr_toVars( ZnkVarpAry vars, const char* var_name, const char* val, size_t val_leng );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
