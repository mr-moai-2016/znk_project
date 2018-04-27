#include <Est_tag_manager.h>
#include <Est_config.h>
#include <Est_ui.h>
#include <Est_unid.h>
#include <Est_hint_manager.h>
#include <Est_assort_ui.h>
#include <Est_assort.h>
#include <Est_tag.h>
#include <Est_base.h>

#include <Znk_htp_util.h>
#include <Znk_str_ex.h>
#include <Znk_missing_libc.h>
#include <Znk_nset.h>
#include <time.h>

#define SJIS_YO "\x97\x5c" /* 予 */

#define IS_OK( val ) (bool)( (val) != NULL )

static void
registNewGroup( ZnkVarpAry post_vars, ZnkStr msg )
{
	ZnkVarp varp;
	if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "EstCM_new_group", false ) )){
		if( ZnkS_empty( ZnkVar_cstr( varp ) ) ){
			ZnkStr_addf( msg, "無効なグループ名です.\n" );
		} else {
			const size_t pv_size = ZnkVarpAry_size( post_vars );
			size_t       pv_idx;
			ZnkMyf       tags_myf  = EstConfig_tags_myf();
			ZnkStr       unesc_val = ZnkStr_new( "" );
			ZnkHtpURL_unescape_toStr( unesc_val, ZnkVar_cstr(varp), ZnkVar_str_leng(varp) );

			if( ZnkS_eq( ZnkStr_cstr( unesc_val ), "未知のタグ" ) ){
				ZnkStr_addf( msg, "そのグループ名はシステムで" SJIS_YO "約されており登録できません.\n" );
			} else {
				ZnkStrAry id_list = ZnkStrAry_create( true );

				for( pv_idx=0; pv_idx<pv_size; ++pv_idx ){
					const char* id;
					varp = ZnkVarpAry_at( post_vars, pv_idx );
					id = ZnkVar_name_cstr( varp );
					if( ZnkS_isBegin_literal( id, "category_" ) ){
						ZnkStrAry_push_bk_cstr( id_list, id, Znk_NPOS );
					}
				}
				if( EstTag_registNewGroup( tags_myf, ZnkStr_cstr(unesc_val), msg, id_list ) ){
					EstConfig_saveTagsMyf();
				}

				ZnkStrAry_destroy( id_list );
			}
			ZnkStr_delete( unesc_val );
		}
	} else {
		ZnkStr_addf( msg, "EstCM_new_group doesn't exist.\n" );
	}
}


static void
moveTag( ZnkVarpAry post_vars, ZnkStr msg, const char* dst_group_id )
{
	const size_t pv_size = ZnkVarpAry_size( post_vars );
	ZnkStr  unesc_val = ZnkStr_new( "" );
	size_t  pv_idx;
	size_t  count = 0;
	ZnkMyf  tags_myf = EstConfig_tags_myf();
	ZnkVarp varp;

	for( pv_idx=0; pv_idx<pv_size; ++pv_idx ){
		const char* id;
		varp = ZnkVarpAry_at( post_vars, pv_idx );
		id = ZnkVar_name_cstr( varp );
		if( ZnkS_isBegin_literal( id, "tagid_" ) ){
			ZnkStr_clear( unesc_val );
			ZnkHtpURL_unescape_toStr( unesc_val, ZnkVar_cstr(varp), ZnkVar_str_leng(varp) );
			if( EstTag_remove( tags_myf, ZnkStr_cstr(unesc_val), msg ) ){
				if( EstTag_registNew( tags_myf, ZnkStr_cstr(unesc_val), NULL, msg, dst_group_id ) ){
					++count;
				}
			}
		}
	}
	if( count ){
		EstConfig_saveTagsMyf();
	}
	ZnkStr_delete( unesc_val );
}
static void
removeTag( ZnkVarpAry post_vars, ZnkStr msg )
{
	const size_t pv_size = ZnkVarpAry_size( post_vars );
	ZnkStr  unesc_val = ZnkStr_new( "" );
	size_t  pv_idx;
	size_t  count = 0;
	ZnkMyf  tags_myf = EstConfig_tags_myf();
	ZnkVarp varp;

	for( pv_idx=0; pv_idx<pv_size; ++pv_idx ){
		const char* id;
		varp = ZnkVarpAry_at( post_vars, pv_idx );
		id = ZnkVar_name_cstr( varp );
		if( ZnkS_isBegin_literal( id, "tagid_" ) ){
			ZnkStr_clear( unesc_val );
			ZnkHtpURL_unescape_toStr( unesc_val, ZnkVar_cstr(varp), ZnkVar_str_leng(varp) );
			if( EstTag_remove( tags_myf, ZnkStr_cstr(unesc_val), msg ) ){
				++count;
			}
		}
	}
	if( count ){
		EstConfig_saveTagsMyf();
	}
	ZnkStr_delete( unesc_val );
}
static void
ageTags( ZnkVarpAry post_vars, ZnkStr msg )
{
	const size_t pv_size = ZnkVarpAry_size( post_vars );
	ZnkStr  unesc_val = ZnkStr_new( "" );
	size_t  pv_idx;
	size_t  count = 0;
	ZnkMyf  tags_myf = EstConfig_tags_myf();
	ZnkVarp varp;
	ZnkVarpAry   tag_list;

	ZnkVarpAry   category_all = ZnkMyf_find_vars( tags_myf, "category_all" );
	const size_t group_size = ZnkVarpAry_size( category_all );
	size_t       group_idx;
	const char*  group_key = NULL;
	ZnkVarp      group_var = NULL;

	for( pv_idx=0; pv_idx<pv_size; ++pv_idx ){
		const char* id;
		varp = ZnkVarpAry_at( post_vars, pv_idx );
		id = ZnkVar_name_cstr( varp );
		if( ZnkS_isBegin_literal( id, "tagid_" ) ){
			ZnkStr_clear( unesc_val );
			ZnkHtpURL_unescape_toStr( unesc_val, ZnkVar_cstr(varp), ZnkVar_str_leng(varp) );

			for( group_idx=0; group_idx<group_size; ++group_idx ){
				group_var = ZnkVarpAry_at( category_all, group_idx );
				group_key = ZnkVar_name_cstr( group_var );
		
				tag_list = ZnkMyf_find_vars( tags_myf, group_key );
				if( tag_list ){
					count += EstBase_agePtrObjs( tag_list, post_vars );
				}
			}
		}
	}
	if( count ){
		EstConfig_saveTagsMyf();
	}
	ZnkStr_delete( unesc_val );
}

/***
 * myf内に記述されたVarsの配列から、指定された要素を削除する.
 * これは具体的な例としては以下のような形式であって
 * 削除対象要素としてelem_3を指定した場合、vars_1 と vars_3 内にあるelem_3が削除される.
 *
 * @@V ary_of_vars 
 * vars_1 = ['...']
 * vars_2 = ['...']
 * vars_3 = ['...']
 * @@.
 *
 * @@V vars_1
 * elem_1 = ['...']
 * elem_2 = ['...']
 * elem_3 = ['...']
 * @@.
 *
 * @@V vars_2
 * elem_1 = ['...']
 * elem_2 = ['...']
 * @@.
 *
 * @@V vars_3
 * elem_1 = ['...']
 * elem_3 = ['...']
 * @@.
 */
static size_t
eraseElem_inAryOfVars( ZnkMyf myf, const char* ary_of_vars_key, const char* elem_name )
{
	ZnkVarpAry   ary_of_vars = ZnkMyf_find_vars( myf, ary_of_vars_key );
	const size_t ary_size = ZnkVarpAry_size( ary_of_vars );
	size_t       vars_idx;
	ZnkVarp      vars_var = NULL;
	const char*  vars_key = NULL;
	ZnkVarpAry   vars;
	size_t       count = 0;
	for( vars_idx=0; vars_idx<ary_size; ++vars_idx ){
		vars_var = ZnkVarpAry_at( ary_of_vars, vars_idx );
		vars_key = ZnkVar_name_cstr( vars_var );
		vars     = ZnkMyf_find_vars( myf, vars_key );
		if( vars ){
			size_t erase_idx = ZnkVarpAry_findIdx_byName( vars, elem_name, Znk_NPOS, false );
			if( erase_idx != Znk_NPOS ){
				ZnkVarpAry_erase_byIdx( vars, erase_idx );
				++count;
			}
		}
	}
	return count;
}

static void
removeGroup( ZnkVarpAry post_vars, ZnkStr msg )
{
	const size_t pv_size = ZnkVarpAry_size( post_vars );
	size_t  pv_idx;
	size_t  count = 0;
	ZnkMyf  tags_myf = EstConfig_tags_myf();
	ZnkVarp varp;

	for( pv_idx=0; pv_idx<pv_size; ++pv_idx ){
		const char* id;
		varp = ZnkVarpAry_at( post_vars, pv_idx );
		id = ZnkVar_name_cstr( varp );
		if( ZnkS_isBegin_literal( id, "group_" ) ){
			if( EstTag_removeGroup( tags_myf, id, msg ) ){
				++count;
				eraseElem_inAryOfVars( tags_myf, "category_list", id );
			}
		}
	}
	if( count ){
		EstConfig_saveTagsMyf();
	}
}
static void
ageGroup( ZnkVarpAry post_vars, ZnkStr tag_editor_msg )
{
	const size_t pv_size = ZnkVarpAry_size( post_vars );
	ZnkStr  unesc_val = ZnkStr_new( "" );
	size_t  pv_idx;
	size_t  count = 0;
	ZnkMyf  tags_myf = EstConfig_tags_myf();
	ZnkVarp varp;
	ZnkVarpAry   category_N;

	ZnkVarpAry   category_list = ZnkMyf_find_vars( tags_myf, "category_list" );
	const size_t category_size = ZnkVarpAry_size( category_list );
	size_t       category_idx;
	const char*  category_key = NULL;
	ZnkVarp      category_var = NULL;

	for( pv_idx=0; pv_idx<pv_size; ++pv_idx ){
		const char* id;
		varp = ZnkVarpAry_at( post_vars, pv_idx );
		id = ZnkVar_name_cstr( varp );
		if( ZnkS_isBegin_literal( id, "group_" ) ){
			ZnkStr_clear( unesc_val );
			ZnkHtpURL_unescape_toStr( unesc_val, ZnkVar_cstr(varp), ZnkVar_str_leng(varp) );

			for( category_idx=0; category_idx<category_size; ++category_idx ){
				category_var = ZnkVarpAry_at( category_list, category_idx );
				category_key = ZnkVar_name_cstr( category_var );
		
				category_N = ZnkMyf_find_vars( tags_myf, category_key );
				if( category_N ){
					count += EstBase_agePtrObjs( category_N, post_vars );
				}
			}
		}
	}
	if( count ){
		EstConfig_saveTagsMyf();
	}
	ZnkStr_delete( unesc_val );
}

static bool
registNewCategory( ZnkVarpAry post_vars, ZnkStr msg )
{
	ZnkVarp varp;
	bool result = false;
	if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "EstCM_new_category", false ) )){
		if( ZnkS_empty( ZnkVar_cstr( varp ) ) ){
			ZnkStr_addf( msg, "無効なカテゴリー名です.\n" );
		} else {
			ZnkMyf tags_myf = EstConfig_tags_myf();
			ZnkStr unesc_val = ZnkStr_new( "" );
			ZnkHtpURL_unescape_toStr( unesc_val, ZnkVar_cstr(varp), ZnkVar_str_leng(varp) );

			if( EstTag_registNewCategory( tags_myf, ZnkStr_cstr(unesc_val), msg ) ){
				EstConfig_saveTagsMyf();
				result = true;
			}
			ZnkStr_delete( unesc_val );
		}
	} else {
		ZnkStr_addf( msg, "EstCM_new_category doesn't exist.\n" );
	}
	return result;
}
static void
updateCategory( ZnkVarpAry post_vars, const char* current_category_id, ZnkStr msg )
{
	const size_t pv_size = ZnkVarpAry_size( post_vars );
	size_t  pv_idx;
	ZnkMyf  tags_myf = EstConfig_tags_myf();
	ZnkVarp varp;
	ZnkVarp elem;
	ZnkVarpAry current_category = ZnkMyf_find_vars( tags_myf, current_category_id );
	ZnkStr unesc_val = ZnkStr_new( "" );

	if( ZnkS_eq( current_category_id, "category_all" ) ){
		ZnkStr_addf( msg, "このカテゴリは更新できません.\n" );
		goto FUNC_END;
	}

	ZnkVarpAry_clear( current_category );
	for( pv_idx=0; pv_idx<pv_size; ++pv_idx ){
		const char* id;
		varp = ZnkVarpAry_at( post_vars, pv_idx );
		id = ZnkVar_name_cstr( varp );
		if( ZnkS_isBegin_literal( id, "group_" ) ){
			elem = ZnkVarp_create( id, "", 0, ZnkPrim_e_Str, NULL );
			ZnkStr_clear( unesc_val );
			ZnkHtpURL_unescape_toStr( unesc_val, ZnkVar_cstr( varp ), ZnkVar_str_leng( varp ) );
			ZnkVar_set_val_Str( elem, ZnkStr_cstr(unesc_val), ZnkStr_leng(unesc_val) );
			ZnkVarpAry_push_bk( current_category, elem );
		}
	}
	EstConfig_saveTagsMyf();
	{
		ZnkVarpAry category_list = ZnkMyf_find_vars( tags_myf, "category_list" );
		ZnkVarp    category_varp = ZnkVarpAry_findObj_byName( category_list, current_category_id, Znk_NPOS, false );
		if( category_varp ){
			const char* category_name = ZnkVar_cstr( category_varp );
			ZnkStr_addf( msg, "カテゴリ[%s]は更新されました.\n", category_name );
		}
	}

FUNC_END:
	ZnkStr_delete( unesc_val );
}
static void
ageCategory( const char* current_category_id, ZnkStr msg )
{
	ZnkMyf tags_myf = EstConfig_tags_myf();
	ZnkVarpAry category_list = ZnkMyf_find_vars( tags_myf, "category_list" );
	ZnkVarp    first = ZnkVarpAry_at( category_list, 0 );
	if( first ){
		const char* first_id = ZnkVar_name_cstr( first ); 
		if( !ZnkS_eq( first_id, current_category_id ) ){
			ZnkVarpAry selected_list = ZnkVarpAry_create( true );
			ZnkVarp    elem = ZnkVarp_create( current_category_id, "", 0, ZnkPrim_e_Str, NULL );
			ZnkVarpAry_push_bk( selected_list, elem );
			EstBase_agePtrObjs( category_list, selected_list );
			EstConfig_saveTagsMyf();
			ZnkVarpAry_destroy( selected_list );
		}
	}
}
static bool
removeCategory( const char* current_category_id, ZnkStr msg )
{
	ZnkMyf tags_myf = EstConfig_tags_myf();
	if( EstTag_removeCategory( tags_myf, current_category_id, msg ) ){
		EstConfig_saveTagsMyf();
		return true;
	}
	return false;
}


static void
makeEnableGroupList( ZnkStrAry enable_group_list, const char* current_category_id )
{
	ZnkMyf tags_myf = EstConfig_tags_myf();
	ZnkVarpAry current_category = ZnkMyf_find_vars( tags_myf, current_category_id );
	if( current_category ){
		const size_t group_size = ZnkVarpAry_size( current_category );
		size_t group_idx;
		for( group_idx=0; group_idx<group_size; ++group_idx ){
			ZnkVarp     group    = ZnkVarpAry_at( current_category, group_idx );
			const char* group_id = ZnkVar_name_cstr( group );
			ZnkStrAry_push_bk_cstr( enable_group_list, group_id, Znk_NPOS );
		}
	}
}


void
EstTagManager_main( RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr msg, const char* authentic_key )
{
	ZnkBird bird = ZnkBird_create( "#[", "]#" );
	const char* template_html_file = "templates/tag_manager.html";
	struct ZnkHtpHdrs_tag htp_hdrs = { 0 };
	RanoModule  mod = NULL;
	bool        is_authenticated = false;
	bool        is_unescape_val  = false;
	ZnkVarp     varp;
	ZnkStr      pst_str    = ZnkStr_new( "" );
	ZnkStr      editor_ui  = ZnkStr_new( "" );
	ZnkStr      editor_msg = ZnkStr_new( "" );
	const char* current_category_id = "category_all";
	const char* edit_mode = "tag";

	RanoCGIUtil_getPostedFormData( evar, post_vars, mod, &htp_hdrs, pst_str, NULL, is_unescape_val );

	if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "Moai_AuthenticKey", false ) )
	  && ZnkS_eq( authentic_key, ZnkVar_cstr(varp) ) ){
		is_authenticated = true;
	}
	ZnkStr_addf( msg, "is_authenticated=[%d]\n", is_authenticated );

	if( is_authenticated ){
		ZnkVarp command = ZnkVarpAry_find_byName_literal( post_vars, "command", false );
		ZnkStr_addf( msg, "query_string=[%s]\n", evar->query_string_ );
		if( command ){
			ZnkVarp current_category = ZnkVarpAry_find_byName_literal( post_vars, "category_key", false );
			ZnkVarp edit_mode_var    = ZnkVarpAry_find_byName_literal( post_vars, "edit_mode", false );

			template_html_file = "templates/tag_manager.html";
			if( edit_mode_var ){
				edit_mode = ZnkVar_cstr( edit_mode_var );
				if( ZnkS_eq( edit_mode, "group" ) ){
					template_html_file = "templates/group_editor.html";
				} else if( ZnkS_eq( edit_mode, "category" ) ){
					template_html_file = "templates/category_editor.html";
				}
			}
			if( current_category ){
				current_category_id = ZnkVar_cstr( current_category );
			} else {
				/***
				 * current_category が明示的に指定されていない場合は一番先頭のものが選ばれているものとする.
				 */
				ZnkMyf tags_myf = EstConfig_tags_myf();
				current_category_id = EstTag_getCategoryKey( tags_myf, 0 );
			}

			if( ZnkS_eq( ZnkVar_cstr( command ), "view" ) ){
				/* none */
			} else if( ZnkS_eq( ZnkVar_cstr( command ), "regist_new_tag" ) ){
				EstAssort_registNewTag( post_vars, editor_msg, NULL );
			} else if( ZnkS_eq( ZnkVar_cstr( command ), "move_tags" ) ){
				ZnkVarp dst_varp = ZnkVarpAry_find_byName_literal( post_vars, "EstTM_dst_group_id", false );
				if( dst_varp ){
					const char* dst_group_id = ZnkVar_cstr(dst_varp);
					moveTag( post_vars, editor_msg, dst_group_id );
				}
			} else if( ZnkS_eq( ZnkVar_cstr( command ), "remove_tag" ) ){
				removeTag( post_vars, editor_msg );
			} else if( ZnkS_eq( ZnkVar_cstr( command ), "age_tag" ) ){
				ageTags( post_vars, editor_msg );

			} else if( ZnkS_eq( ZnkVar_cstr( command ), "regist_new_group" ) ){
				registNewGroup( post_vars, editor_msg );

			} else if( ZnkS_eq( ZnkVar_cstr( command ), "remove_group" ) ){
				removeGroup( post_vars, editor_msg );

			} else if( ZnkS_eq( ZnkVar_cstr( command ), "age_group" ) ){
				ageGroup( post_vars, editor_msg );

			} else if( ZnkS_eq( ZnkVar_cstr(command), "regist_new_category" ) ){
				registNewCategory( post_vars, editor_msg );

			} else if( ZnkS_eq( ZnkVar_cstr(command), "update_category" ) ){
				updateCategory( post_vars, current_category_id, editor_msg );

			} else if( ZnkS_eq( ZnkVar_cstr(command), "age_category" ) ){
				ageCategory( current_category_id, editor_msg );

			} else if( ZnkS_eq( ZnkVar_cstr(command), "remove_category" ) ){
				if( removeCategory( current_category_id, editor_msg ) ){
					/* 今消去したcurrent_category_idは既に存在しないので替わりに */
					current_category_id = "category_all";
				}

			} else {
				ZnkStr_addf( msg, "unknown command [%s].", ZnkVar_cstr(command) );
			}

		} else {
			ZnkStr_add( msg, "command not found.\n" );
		}

		if( ZnkS_eq( edit_mode, "tag" ) ){
			ZnkStr tags_list = ZnkStr_new( "" );
			ZnkStr category_select_bar = ZnkStr_new( "" );
			ZnkStr current_category_name = ZnkStr_new( "" );
			const char* editor_msg_cstr = ZnkStr_cstr(editor_msg);
			const char* regist_msg_cstr = "";
	
			if( ZnkS_eq( ZnkVar_cstr(command), "regist_new_tag" ) ){
				editor_msg_cstr = "";
				regist_msg_cstr = ZnkStr_cstr(editor_msg);
			}
	
			EstAssortUI_makeCategorySelectBar( category_select_bar, current_category_id, current_category_name,
					"tag_manager", "view", "edit_mode=tag", "" );
			EstAssortUI_makeTagsView( tags_list, NULL, current_category_id, NULL, NULL, true );
			EstAssortUI_makeTagEditor( editor_ui, editor_msg_cstr, regist_msg_cstr, current_category_id );
	
			ZnkBird_regist( bird, "category_select_bar", ZnkStr_cstr(category_select_bar) );
			ZnkBird_regist( bird, "current_category_name", ZnkStr_cstr(current_category_name) );
			ZnkBird_regist( bird, "tags_list", ZnkStr_cstr(tags_list) );
			ZnkStr_delete( category_select_bar );
			ZnkStr_delete( current_category_name );
			ZnkStr_delete( tags_list );
	
		} else if( ZnkS_eq( edit_mode, "group" ) ){
			const char* editor_msg_cstr = ZnkStr_cstr(editor_msg);
			const char* regist_msg_cstr = "";
	
			if( ZnkS_eq( ZnkVar_cstr(command), "regist_new_group" ) ){
				editor_msg_cstr = "";
				regist_msg_cstr = ZnkStr_cstr(editor_msg);
			}
			EstAssortUI_makeGroupEditor( editor_ui, editor_msg_cstr );
			ZnkBird_regist( bird, "regist_msg_cstr", regist_msg_cstr );
	
		} else if( ZnkS_eq( edit_mode, "category" ) ){
			ZnkStr groups_list = ZnkStr_new( "" );
			ZnkStr category_select_bar = ZnkStr_new( "" );
			ZnkStr current_category_name = ZnkStr_new( "" );
			ZnkStrAry enable_group_list = ZnkStrAry_create( true );
			const char* editor_msg_cstr = ZnkStr_cstr(editor_msg);
			const char* regist_msg_cstr = "";
	
			if( ZnkS_eq( ZnkVar_cstr(command), "regist_new_category" ) ){
				editor_msg_cstr = "";
				regist_msg_cstr = ZnkStr_cstr(editor_msg);
			}
	
			EstAssortUI_makeCategorySelectBar( category_select_bar, current_category_id, current_category_name,
					"tag_manager", "view", "edit_mode=category", "" );
			makeEnableGroupList( enable_group_list, current_category_id );
			EstAssortUI_makeGroupsView( groups_list, enable_group_list, "category_all" );
	
			ZnkBird_regist( bird, "category_select_bar", ZnkStr_cstr(category_select_bar) );
			ZnkBird_regist( bird, "current_category_name", ZnkStr_cstr(current_category_name) );
			ZnkBird_regist( bird, "regist_msg_cstr", regist_msg_cstr );
			ZnkBird_regist( bird, "groups_list", ZnkStr_cstr(groups_list) );
			ZnkStr_delete( category_select_bar );
			ZnkStr_delete( current_category_name );
			ZnkStr_delete( groups_list );
			ZnkStrAry_destroy( enable_group_list );
	
			EstAssortUI_makeCategoryEditor( editor_ui, editor_msg_cstr, current_category_id );
		} else {
		}
	
		ZnkBird_regist( bird, "editor_ui", ZnkStr_cstr(editor_ui) );
		ZnkBird_regist( bird, "Moai_AuthenticKey", authentic_key );
		{
			ZnkStr hint_table = EstHint_getHintTable( "tag_manager" );
			if( hint_table ){
				ZnkBird_regist( bird, "hint_table", ZnkStr_cstr(hint_table) );
			} else {
				ZnkBird_regist( bird, "hint_table", "" );
			}
		}
	} else {
		const char* xhr_auth_host = EstConfig_XhrAuthHost();
		template_html_file = "templates/unauthorized.html";
		ZnkBird_regist( bird, "xhr_auth_host", xhr_auth_host );
	}
	
	ZnkHtpURL_negateHtmlTagEffection( msg ); /* for XSS */
	{
		ZnkSRef old_ptn = { 0 };
		ZnkSRef new_ptn = { 0 };
		ZnkSRef_set_literal( &old_ptn, "\n" );
		ZnkSRef_set_literal( &new_ptn, "<br>\n" );
		ZnkStrEx_replace_BF( msg, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	ZnkBird_regist( bird, "msg",   ZnkStr_cstr(msg) );
	RanoCGIUtil_printTemplateHTML( evar, bird, template_html_file );

	ZnkBird_destroy( bird );
	ZnkStr_delete( pst_str );
	ZnkStr_delete( editor_ui );
	ZnkStr_delete( editor_msg );
}

