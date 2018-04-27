#include <Est_assort_ui.h>
#include <Est_config.h>
#include <Est_ui_base.h>
#include <Est_unid.h>

#include <Rano_html_ui.h>

#include <Znk_myf.h>
#include <Znk_varp_ary.h>

void
EstAssortUI_makeCategorySelectBar( ZnkStr assort_ui, const char* current_category_id, ZnkStr current_category_name,
		const char* manager, const char* command, const char* arg, const char* anchor )
{
	ZnkMyf       tags_myf = EstConfig_tags_myf();
	ZnkStr       additional_arg = ZnkStr_newf( !ZnkS_empty(arg) ? "&%s" : "", arg );
	ZnkVarpAry   category_list      = ZnkMyf_find_vars( tags_myf, "category_list" );
	const size_t category_list_size = ZnkVarpAry_size( category_list );
	size_t       category_idx;
	for( category_idx=0; category_idx<category_list_size; ++category_idx ){
		ZnkVarp      category_var  = ZnkVarpAry_at( category_list, category_idx );
		const char*  category_key  = ZnkVar_name_cstr( category_var );
		const char*  category_name = ZnkVar_cstr( category_var );
		bool         now_selected = ZnkS_eq( category_key, current_category_id );

		if( now_selected && current_category_name ){
			ZnkStr_set( current_category_name, category_name );
		}

		ZnkStr_addf( assort_ui,
				"<a class=%s onclick=\"EjsAssort_submitCommandEx( document.fm_main, '%s', '%s', 'category_key=%s%s', '%s' )\">",
				now_selected ? "MstyNowSelectedLink" : "MstyElemLink",
				manager, command, category_key, ZnkStr_cstr(additional_arg),
				anchor );
		ZnkStr_addf( assort_ui, "%s</a> \n", category_name );
	}
	ZnkStr_add( assort_ui, "<br>\n" );

	ZnkStr_delete( additional_arg );
}


void
EstAssortUI_makeTagsView( ZnkStr assort_ui,
		ZnkStrAry enable_tag_list, const char* current_category_id, const char* tag_editor_msg, const char* comment, bool in_tag_editor )
{
	ZnkMyf tags_myf = EstConfig_tags_myf();
	ZnkVarpAry category_all = ZnkMyf_find_vars( tags_myf, "category_all" );
	ZnkVarpAry tag_list;
	const size_t line_tag_num = EstConfig_getLineTagNum();
	const size_t group_size = ZnkVarpAry_size( category_all );
	size_t       tag_size;
	size_t       tag_idx;
	size_t       group_idx;
	const char*  group_key = NULL;
	ZnkVarp      group_var = NULL;

	ZnkVarp var = NULL;
	const char* id;
	const char* val;
	size_t width = 120;
	bool on = false;
	ZnkVarpAry current_category = ZnkMyf_find_vars( tags_myf, current_category_id );
	ZnkBfr unknown_list  = ZnkBfr_create_null();
	size_t enable_idx  = Znk_NPOS;

	if( enable_tag_list ){
		ZnkBfr_resize_fill( unknown_list, ZnkStrAry_size(enable_tag_list), 0 );
	}

	for( group_idx=0; group_idx<group_size; ++group_idx ){
		group_var = ZnkVarpAry_at( category_all, group_idx );
		group_key = ZnkVar_name_cstr( group_var );
		if( current_category && ZnkVarpAry_findIdx_byName( current_category, group_key, Znk_NPOS, false ) == Znk_NPOS ){
			continue;
		}

		tag_list = ZnkMyf_find_vars( tags_myf, group_key );
		if( tag_list ){

			ZnkStr_addf( assort_ui, "<div id=%s_list style=display:block>\n", group_key );

			ZnkStr_addf( assort_ui, "<b>%s</b>", ZnkVar_cstr( group_var ) );

			ZnkStr_add( assort_ui, "<table border=0>\n" );
			ZnkStr_add( assort_ui, "<tr>\n" );
			tag_size = ZnkVarpAry_size( tag_list );
			for( tag_idx=0; tag_idx<tag_size; ++tag_idx ){
				ZnkStr_addf( assort_ui, "<td class=MstyNoneWordBreak width=\"%zu\" valign=top>", width );
				var = ZnkVarpAry_at( tag_list, tag_idx );
				id  = ZnkVar_name_cstr( var );
				val = ZnkVar_cstr( var );
				on  = false;

				if( enable_tag_list ){
					enable_idx = ZnkStrAry_find( enable_tag_list, 0, val, Znk_NPOS );
					if( enable_idx != Znk_NPOS ){
						on = true;
						ZnkBfr_set_aryval_8( unknown_list, enable_idx, 1 );
					}
				}
				ZnkStr_addf( assort_ui, "<input type=checkbox id=%s name=%s size=\"10\" value=\"%s\" %s>%s<br>", id, id, val,
						on ? "checked" : "",
						val );

				ZnkStr_add( assort_ui, "</td>\n" );
				if( (tag_idx+1) % line_tag_num == 0 ){
					ZnkStr_add( assort_ui, "</tr>\n<tr>\n" );
				}
			}
			ZnkStr_add( assort_ui, "</tr>\n" );
			ZnkStr_add( assort_ui, "</table>\n" );
			ZnkStr_add( assort_ui, "</div><br>\n" );
		}
	}

	/* Unknown Tags */
	if( enable_tag_list ){
		const size_t   unknown_size = ZnkBfr_size( unknown_list );
		const uint8_t* unknown_data = ZnkBfr_data( unknown_list );
		size_t         unknown_idx;
		bool           unknown_exist = false;

		for( unknown_idx=0; unknown_idx<unknown_size; ++unknown_idx ){
			if( unknown_data[ unknown_idx ] == 0 ){
				unknown_exist = true;
				break;
			}
		}

		if( unknown_exist ){
			ZnkStr_addf( assort_ui, "<b>未知のタグ</b><br>" );
			ZnkStr_addf( assort_ui, "<font size=-1 color=#555555>以下のタグがこの画像に付加されていますが、未知のタグです.</font>" );
			ZnkStr_add( assort_ui, "<table border=0>\n" );
			ZnkStr_add( assort_ui, "<tr>\n" );

			tag_idx = 0;
			for( unknown_idx=0; unknown_idx<unknown_size; ++unknown_idx ){
				if( unknown_data[ unknown_idx ] == 0 ){
					char buf[ 256 ] = "";
					char new_id[ 256 ] = "";
					ZnkStr_addf( assort_ui, "<td class=MstyNoneWordBreak width=\"%zu\" valign=top>", width );
					val = ZnkStrAry_at_cstr( enable_tag_list, unknown_idx );
					Znk_snprintf( new_id, sizeof(new_id), "tagid_%s", EstUNID_issue( buf, sizeof(buf) ) );
					ZnkStr_addf( assort_ui, "<input type=checkbox id=%s name=%s size=\"10\" value=\"%s\" checked>%s<br>",
							new_id, new_id, val, val );

					ZnkStr_add( assort_ui, "</td>\n" );
					if( (tag_idx+1) % 4 == 0 ){
						ZnkStr_add( assort_ui, "</tr>\n<tr>\n" );
					}
					++tag_idx;
				}
			}
			ZnkStr_add( assort_ui, "</tr>\n" );
			ZnkStr_add( assort_ui, "</table>\n" );
		}
	}

	if( tag_editor_msg ){
		ZnkStr_addf( assort_ui, "<br>%s<br>\n", tag_editor_msg );
	}

	if( enable_tag_list ){
		const size_t enable_size = ZnkStrAry_size( enable_tag_list );
		if( enable_size ){
			ZnkStr_addf( assort_ui, "<br>現在この画像に付加されている全タグ<br>" );
			ZnkStr_addf( assort_ui, "<div class=MstyComment>" );
			for( enable_idx=0; enable_idx<enable_size; ++enable_idx ){
				const char* enable_tag = ZnkStrAry_at_cstr( enable_tag_list, enable_idx );
				ZnkStr_addf( assort_ui, "%s ", enable_tag );
				if( (enable_idx+1) % 4 == 0 ){
					ZnkStr_add( assort_ui, "<br>\n" );
				}
			}
			ZnkStr_addf( assort_ui, "</div>\n" );
		}
	}

	if( comment ){
		ZnkStr_addf( assort_ui, "<br>この分類に関するコメント<br>" );
		//ZnkStr_addf( assort_ui, "<textarea class=MstyInputField name=ast_comment cols=48 rows=4>%s</textarea>\n", comment );
		ZnkStr_addf( assort_ui, "<textarea class=MstyInputField name=ast_comment cols=35 rows=4>%s</textarea>\n", comment );
	}
	ZnkStr_add( assort_ui, "<br>\n" );

	ZnkBfr_destroy( unknown_list );

}

void
EstAssortUI_makeGroupsView( ZnkStr assort_ui,
		ZnkStrAry enable_group_list, const char* current_category_id )
{
	ZnkMyf tags_myf = EstConfig_tags_myf();
	ZnkVarpAry category_all = ZnkMyf_find_vars( tags_myf, "category_all" );
	const size_t group_size = ZnkVarpAry_size( category_all );
	size_t       group_idx;
	ZnkVarp      group_var = NULL;
	const char*  group_key = NULL;
	const char*  group_val = NULL;

	bool on = false;
	ZnkVarpAry current_category = ZnkMyf_find_vars( tags_myf, current_category_id );

	for( group_idx=0; group_idx<group_size; ++group_idx ){
		group_var = ZnkVarpAry_at( category_all, group_idx );
		group_key = ZnkVar_name_cstr( group_var );
		group_val = ZnkVar_cstr( group_var );
		if( current_category && ZnkVarpAry_findIdx_byName( current_category, group_key, Znk_NPOS, false ) == Znk_NPOS ){
			continue;
		}

		if( enable_group_list ){
			on  = (bool)( ZnkStrAry_find( enable_group_list, 0, group_key, Znk_NPOS ) != Znk_NPOS );
		} else {
			on = false;
		}
		ZnkStr_addf( assort_ui, "<input type=checkbox id=%s name=%s value=\"%s\" %s>%s<br>\n",
				group_key, group_key, group_val,
				on ? "checked" : "",
				group_val );
	}
}

void
EstAssortUI_makeTagEditor( ZnkStr editor_ui, const char* editor_msg, const char* regist_msg, const char* category_key )
{
	ZnkStr_addf( editor_ui, "<fieldset class=MstyStdFieldSet><legend>タグの新規登録</legend>\n" );

	ZnkStr_addf( editor_ui,
			"<input class=MstyInputField type=text name=EstCM_new_tag placeholder=\"タグ文字列の指定\" value=\"\" size=20> \n" );

	{
		ZnkMyf tags_myf = EstConfig_tags_myf();
		//ZnkVarpAry category_all = ZnkMyf_find_vars( tags_myf, "category_all" );
		ZnkVarpAry category_vars = ZnkMyf_find_vars( tags_myf, category_key );
		EstUIBase_makeSelectBox_byVarpAry( editor_ui, category_vars,
				"グループ: ", "EstCM_select_group", "group_0" );
	}

	ZnkStr_addf( editor_ui,
			"<a class=\"MstyElemLinkRed\" "
			"href=\"javascript:EjsAssort_submitCommandEx( document.fm_main, 'tag_manager', 'regist_new_tag', 'edit_mode=tag&category_key=%s', '' )\">"
			"新規登録</a>\n", category_key );
	if( !ZnkS_empty( regist_msg ) ){
		ZnkStr_addf( editor_ui, "<br>%s\n", regist_msg );
	}

	ZnkStr_addf( editor_ui, "</fieldset>\n" );

	ZnkStr_addf( editor_ui, "<br> \n" );

	ZnkStr_addf( editor_ui, "<fieldset class=MstyStdFieldSet><legend>タグの移動と削除</legend>\n" );

	ZnkStr_addf( editor_ui,
			"<a class=\"MstyElemLinkRed\" "
			"href=\"javascript:EjsAssort_submitCommandEx( document.fm_main, 'tag_manager', 'move_tags', 'edit_mode=tag&category_key=%s', '' )\">"
			"選択したタグを別のグループへ移動</a>\n", category_key );

	{
		ZnkMyf tags_myf = EstConfig_tags_myf();
		//ZnkVarpAry category_all = ZnkMyf_find_vars( tags_myf, "category_all" );
		ZnkVarpAry category_vars = ZnkMyf_find_vars( tags_myf, category_key );
		EstUIBase_makeSelectBox_byVarpAry( editor_ui, category_vars,
				"移動先グループ: ", "EstTM_dst_group_id", "group_0" );
	}

	ZnkStr_addf( editor_ui,
			"<br> <br>\n" );

	ZnkStr_addf( editor_ui,
			"<a class=\"MstyElemLinkRed\" "
			"href=\"javascript:EjsAssort_submitCommandEx( document.fm_main, 'tag_manager', 'age_tag', 'edit_mode=tag&category_key=%s', '' )\">"
			"選択したタグを先頭へ移動</a>\n", category_key );

	ZnkStr_addf( editor_ui,
			"<br> <br>\n" );

	ZnkStr_addf( editor_ui,
			"<a class=\"MstyElemLinkRed\" "
			"href=\"javascript:EjsAssort_submitCommandEx( document.fm_main, 'tag_manager', 'remove_tag', 'edit_mode=tag&category_key=%s', '' )\">"
			"選択したタグを削除</a>\n", category_key );

	if( !ZnkS_empty( editor_msg ) ){
		ZnkStr_addf( editor_ui, "<br>%s\n", editor_msg );
	}

	ZnkStr_addf( editor_ui, "</fieldset>\n" );

}

void
EstAssortUI_makeGroupEditor( ZnkStr editor_ui, const char* editor_msg )
{
	ZnkStr_addf( editor_ui, "<br>\n" );

	EstAssortUI_makeGroupsView( editor_ui, NULL, "category_all" );

	ZnkStr_addf( editor_ui,
			"<br>\n" );


	ZnkStr_addf( editor_ui,
			"<a class=\"MstyElemLinkRed\" "
			"href=\"javascript:EjsAssort_submitCommandAndArg( document.fm_main, 'tag_manager', 'age_group', 'edit_mode=group' )\">選択したグループを先頭へ移動</a>\n" );

	ZnkStr_addf( editor_ui,
			"<br> <br>\n" );

	ZnkStr_addf( editor_ui,
			"<a class=\"MstyElemLinkRed\" "
			"href=\"javascript:EjsAssort_submitCommandAndArg( document.fm_main, 'tag_manager', 'remove_group', 'edit_mode=group' )\">選択したグループを削除</a>\n" );

	if( !ZnkS_empty( editor_msg ) ){
		ZnkStr_addf( editor_ui, "<br>%s\n", editor_msg );
	}
}

void
EstAssortUI_makeCategoryEditor( ZnkStr editor_ui, const char* editor_msg, const char* current_category_id )
{
	ZnkStr_addf( editor_ui,
			"<br>\n" );

	if( ZnkS_eq( current_category_id, "category_all" ) ){
		ZnkStr_addf( editor_ui, "<span class=\"MstyElemLink\"><font color=#888888>グループ一覧を更新</font></span>\n" );
	} else {
		ZnkStr_addf( editor_ui,
				"<a class=\"MstyElemLinkRed\" "
				"href=\"javascript:EjsAssort_submitCommandAndArg( document.fm_main, 'tag_manager', 'update_category', 'edit_mode=category&category_key=%s' )\">グループ一覧を更新</a>\n",
				current_category_id );
	}

	ZnkStr_addf( editor_ui,
			"<br> <br>\n" );

	ZnkStr_addf( editor_ui,
			"<a class=\"MstyElemLinkRed\" "
			"href=\"javascript:EjsAssort_submitCommandAndArg( document.fm_main, 'tag_manager', 'age_category', 'edit_mode=category&category_key=%s' )\">このカテゴリを先頭へ移動</a>\n",
			current_category_id );

	ZnkStr_addf( editor_ui,
			"<br> <br>\n" );

	if( ZnkS_eq( current_category_id, "category_all" ) ){
		ZnkStr_addf( editor_ui, "<span class=\"MstyElemLink\"><font color=#888888>このカテゴリーを削除</font></span>\n" );
	} else {
		ZnkStr_addf( editor_ui,
				"<a class=\"MstyElemLinkRed\" "
				"href=\"javascript:EjsAssort_submitCommandAndArg( document.fm_main, 'tag_manager', 'remove_category', 'edit_mode=category&category_key=%s' )\">このカテゴリーを削除</a>\n",
				current_category_id );
	}

	if( !ZnkS_empty(editor_msg) ){
		ZnkStr_addf( editor_ui,
				"<br> <br>%s<br>\n", editor_msg );
	}
}

