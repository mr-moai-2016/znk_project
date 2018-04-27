#ifndef INCLUDE_GUARD__Rano_html_ui_h__
#define INCLUDE_GUARD__Rano_html_ui_h__

#include <Znk_str_ary.h>

Znk_EXTERN_C_BEGIN

void
RanoHtmlUI_makeSelectBox( ZnkStr html,
		const char* varname, const char* current_val, bool is_enable,
		ZnkStrAry val_list, ZnkStrAry name_list );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
