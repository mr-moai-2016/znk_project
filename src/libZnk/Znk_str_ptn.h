#ifndef INCLUDE_GUARD__Znk_str_ptn_h__
#define INCLUDE_GUARD__Znk_str_ptn_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

const uint8_t*
ZnkStrPtn_getBetween( ZnkStr ans, const uint8_t* data, const size_t data_size,
		const char* ptn_begin, const char* ptn_end );

bool
ZnkStrPtn_replaceBetween( ZnkStr ans,
		const uint8_t* data, size_t data_size,
		const char* ptn_begin, const char* ptn_end,
		const char* replace_str, bool replaced_with_ptn );

size_t
ZnkStrPtn_replace( ZnkStr str, size_t begin,
		const uint8_t* old_ptn, size_t old_ptn_leng,
		const uint8_t* new_ptn, size_t new_ptn_leng,
		size_t seek_depth );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
