#ifndef INCLUDE_GUARD__Est_ui_h__
#define INCLUDE_GUARD__Est_ui_h__

#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>
#include <Znk_bird.h>

Znk_EXTERN_C_BEGIN

#if 0
void
EstUI_registBird_LinkListUIEx( RanoCGIEVar* evar,
		ZnkStr ans, ZnkVarpAry list,
		size_t begin_idx, size_t end_idx,
		const char* style_class_name, const char* new_linkid, const char* url_prefix,
		bool is_target_blank, size_t show_file_num, const char* authentic_key, bool is_checked );
#endif

void
EstUI_getSummary( ZnkBird bird, const char* file_path );

void
EstUI_showPageSwitcher( ZnkStr result_ui, const char* query_string_base,
		size_t cache_file_list_size, size_t show_file_num, size_t begin_idx,
		const char* authentic_key, const char* page_switcher_name );

void
EstUI_makeCheckedConfirmView( RanoCGIEVar* evar, ZnkVarpAry post_vars,
		ZnkStr caches, size_t show_file_num, const char* authentic_key,
		const char* assort_msg );

void
EstUI_makeAssortedReportView( RanoCGIEVar* evar, ZnkStrAry dst_vpath_list,
		ZnkStr caches, size_t show_file_num, const char* authentic_key,
		const char* assort_msg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
