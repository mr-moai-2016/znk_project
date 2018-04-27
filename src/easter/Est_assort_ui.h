#ifndef INCLUDE_GUARD__Est_assort_ui_h__
#define INCLUDE_GUARD__Est_assort_ui_h__

#include <Znk_str_ary.h>

Znk_EXTERN_C_BEGIN

void
EstAssortUI_make( ZnkStr assort_ui, const char* tag_editor_msg, ZnkStrAry enable_tag_list, const char* comment );

void
EstAssortUI_makeCategorySelectBar( ZnkStr assort_ui, const char* current_category_id, ZnkStr current_category_name,
		const char* manager, const char* command, const char* arg, const char* anchor );

void
EstAssortUI_makeTagsView( ZnkStr assort_ui,
		ZnkStrAry enable_tag_list, const char* current_category_id, const char* tag_editor_msg, const char* comment, bool with_open_manager );
void
EstAssortUI_makeGroupsView( ZnkStr assort_ui,
		ZnkStrAry enable_group_list, const char* current_category_id );

void
EstAssortUI_makeTagEditor( ZnkStr editor_ui, const char* editor_msg, const char* regist_msg, const char* category_key );

void
EstAssortUI_makeGroupEditor( ZnkStr editor_ui, const char* editor_msg );

void
EstAssortUI_makeCategoryEditor( ZnkStr editor_ui, const char* editor_msg, const char* current_category_id );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
