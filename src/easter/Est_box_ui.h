#ifndef INCLUDE_GUARD__Est_box_ui_h__
#define INCLUDE_GUARD__Est_box_ui_h__

#include <Znk_str.h>
#include <Est_finf.h>

Znk_EXTERN_C_BEGIN

void
EstBoxUI_make_forSearchResult( ZnkStr ans, ZnkVarpAry finf_list,
		size_t begin_idx, size_t end_idx, const char* authentic_key );

void
EstBoxUI_make_forFSysView( ZnkStr ans, ZnkVarpAry cinf_list,
		size_t begin_idx, size_t end_idx, const char* authentic_key,
		bool is_target_blank, bool is_checked );

void
EstBoxUI_make_forLinks( ZnkStr ans, ZnkVarpAry link_list,
		size_t begin_idx, size_t end_idx, const char* authentic_key );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
