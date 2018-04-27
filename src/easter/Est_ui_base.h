#ifndef INCLUDE_GUARD__Est_ui_base_h__
#define INCLUDE_GUARD__Est_ui_base_h__

#include <Znk_str.h>
#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

bool
EstUIBase_getHtmlTitle( ZnkStr title, const char* filename, size_t max_count );

void
EstUIBase_makeSelectBox_byVarpAry( ZnkStr ui, ZnkVarpAry list,
		const char* label, const char* varname, const char* current_val );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */

