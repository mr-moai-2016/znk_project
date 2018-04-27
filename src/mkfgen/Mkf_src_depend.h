#ifndef INCLUDE_GUARD__Mkf_src_depend_h__
#define INCLUDE_GUARD__Mkf_src_depend_h__

#include <Mkf_seek.h>
#include <Znk_str_ary.h>

Znk_EXTERN_C_BEGIN

void
MkfSrcDepend_get( ZnkStr text, ZnkStrAry list, const char* obj_sfx, const char* nl, MkfSeekFuncT_isInterestExt is_interest_ext );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
