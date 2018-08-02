#ifndef INCLUDE_GUARD__Mkf_sset_h__
#define INCLUDE_GUARD__Mkf_sset_h__

#include <Znk_str_ary.h>

Znk_EXTERN_C_BEGIN

/***
 * 元が文字列の(数学の意味での)集合を取り扱う.
 * データ構造的にはとりあえず元に重複のない配列とする.
 */

bool
MkfSSet_add( ZnkStrAry sset, const char* str );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
