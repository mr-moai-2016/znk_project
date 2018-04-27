#ifndef INCLUDE_GUARD__Rano_sset_h__
#define INCLUDE_GUARD__Rano_sset_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

/***
 * 元が文字列の(数学の意味での)集合を取り扱う.
 * データ構造的にはとりあえず元に重複のない配列とする.
 */

size_t
RanoSSet_addSeq( ZnkStr dst_seq, const char* src_seq );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
