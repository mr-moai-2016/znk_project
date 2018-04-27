#ifndef INCLUDE_GUARD__Est_cinf_ui_h__
#define INCLUDE_GUARD__Est_cinf_ui_h__

#include <Znk_varp_ary.h>
#include <Znk_bird.h>

Znk_EXTERN_C_BEGIN

typedef void (*EstCInfViewFuncT)( ZnkStr ans, void* inf_ptr,
		const char* url_prefix, const char* url, const char* unesc_url, const char* esc_url,
		const char* id, ZnkStr title,
		const char* style_class_name, bool is_target_blank,
		const char* authentic_key );

typedef void* (*EstCInfGetElemFuncT)( ZnkVarpAry cinf_list, size_t idx,
		const char** id, const char** url );

void
EstCInfListUI_make( ZnkStr ans, ZnkVarpAry cinf_list,
		EstCInfViewFuncT view_func, EstCInfGetElemFuncT get_elem_func,
		size_t begin_idx, size_t end_idx,
		const char* style_class_name, const char* new_linkid, const char* url_prefix, const char* id_prefix,
		bool is_target_blank, size_t show_file_num, size_t line_file_num, size_t elem_pix_width,
		const char* authentic_key, bool is_checked );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
