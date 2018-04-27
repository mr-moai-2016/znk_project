#include <Est_tag.h>
#include <Est_unid.h>

#include <Znk_missing_libc.h>
#include <Znk_myf.h>
#include <Znk_varp_ary.h>
#include <Znk_htp_util.h>

#define SJIS_YO "\x97\x5c" /* 予 */

bool
EstTag_registNew( ZnkMyf tags_myf, const char* tag, ZnkStr tagid, ZnkStr msg, const char* select_group )
{
	bool found  = false;
	bool result = false;
	const char*  group_key  = NULL;
	ZnkVarp      group_var  = NULL;
	ZnkVarpAry   group_list = ZnkMyf_find_vars( tags_myf, "category_all" );
	ZnkVarpAry   tag_list      = NULL;
	const size_t group_size = ZnkVarpAry_size( group_list );
	size_t       group_idx;

	for( group_idx=0; group_idx<group_size; ++group_idx ){
		group_var = ZnkVarpAry_at( group_list, group_idx );
		group_key = ZnkVar_name_cstr( group_var );

		tag_list = ZnkMyf_find_vars( tags_myf, group_key );
		if( tag_list ){
			size_t idx = ZnkVarpAry_findIdx_byStrVal( tag_list, tag, Znk_NPOS );
			if( idx != Znk_NPOS ){
				found = true;
				break;
			}
		}
	}

	if( found ){
		ZnkStr_addf( msg, "タグ[%s]は既にグループ[%s]内に存在しています.\n", tag, ZnkVar_cstr(group_var) );
	} else {
		tag_list = ZnkMyf_find_vars( tags_myf, select_group );
		if( tag_list ){
			char buf[ 256 ] = "";
			char new_id[ 256 ] = "";
			ZnkVarp new_elem = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
			Znk_snprintf( new_id, sizeof(new_id), "tagid_%s", EstUNID_issue( buf, sizeof(buf) ) );
			ZnkStr_assign( new_elem->name_, 0, new_id, Znk_strlen(new_id) );
			ZnkVar_set_val_Str( new_elem, tag, Znk_strlen(tag) );
			ZnkVarpAry_push_bk( tag_list, new_elem );

			if( tagid ){
				ZnkStr_set( tagid, new_id );
			}
			ZnkStr_addf( msg, "新しいタグ[%s]を登録しました.\n", tag );
			result = true;
		} else {
			ZnkStr_addf( msg, "Invalid select_group.\n" );
		}
	}
	return result;
}
bool
EstTag_remove( ZnkMyf tags_myf, const char* tag, ZnkStr msg )
{
	bool found  = false;
	bool result = false;
	const char*  group_key  = NULL;
	ZnkVarp      group_var  = NULL;
	ZnkVarpAry   group_list = ZnkMyf_find_vars( tags_myf, "category_all" );
	ZnkVarpAry   tag_list      = NULL;
	const size_t group_size = ZnkVarpAry_size( group_list );
	size_t       group_idx;

	for( group_idx=0; group_idx<group_size; ++group_idx ){
		group_var = ZnkVarpAry_at( group_list, group_idx );
		group_key = ZnkVar_name_cstr( group_var );

		tag_list = ZnkMyf_find_vars( tags_myf, group_key );
		if( tag_list ){
			size_t idx = ZnkVarpAry_findIdx_byStrVal( tag_list, tag, Znk_NPOS );
			if( idx != Znk_NPOS ){
				found = true;
				result = ZnkVarpAry_erase_byIdx( tag_list, idx );
				break;
			}
		}
	}

	if( !found ){
		ZnkStr_addf( msg, "タグ[%s]がみつかりません.\n", tag );
	} else {
		if( result ){
			ZnkStr_addf( msg, "タグ[%s]を削除しました.<br>\n", tag );
		} else {
			ZnkStr_addf( msg, "タグ[%s]を削除しようとしましたが失敗しました.\n", tag );
		}
	}
	return result;
}

bool
EstTag_removeGroup( ZnkMyf tags_myf, const char* query_group_key, ZnkStr msg )
{
	bool found  = false;
	bool result = false;
	const char*  group_key  = NULL;
	ZnkVarp      group_var  = NULL;
	ZnkVarpAry   group_list = ZnkMyf_find_vars( tags_myf, "category_all" );
	ZnkVarpAry   tag_list      = NULL;
	const size_t group_size = ZnkVarpAry_size( group_list );
	size_t       group_idx;

	ZnkVarp query_group_var = ZnkVarpAry_findObj_byName( group_list, query_group_key, Znk_NPOS, false );
	ZnkStr  query_group_val = ZnkStr_new( query_group_var ? ZnkVar_cstr( query_group_var ) : "" );

	if( ZnkS_eq( query_group_key, "group_0" ) ){
		ZnkStr_addf( msg, "グループ[種類]はシステムで" SJIS_YO "約された特殊なグループであるため消去できません.<br>\n" );
		result = false;
		goto FUNC_END;
	}

	for( group_idx=0; group_idx<group_size; ++group_idx ){
		group_var = ZnkVarpAry_at( group_list, group_idx );
		group_key = ZnkVar_name_cstr( group_var );
		if( ZnkS_eq(group_key, query_group_key) ){
			tag_list = ZnkMyf_find_vars( tags_myf, group_key );
			if( tag_list ){
				size_t size = ZnkVarpAry_size( tag_list );
				found = true;
				if( size ){
				} else {
					/* found */
					const size_t sec_size = ZnkMyf_numOfSection( tags_myf );
					size_t sec_idx;
					ZnkMyfSection sec;
					for( sec_idx=0; sec_idx<sec_size; ++sec_idx ){
						sec = ZnkMyf_atSection( tags_myf, sec_idx );
						if( ZnkS_eq( ZnkMyfSection_name(sec), group_key ) ){
							ZnkMyf_erase_byIdx( tags_myf, sec_idx );
							break;
						}
					}
					ZnkVarpAry_erase_byIdx( group_list, group_idx );
					result = true;
					break;
				}
			}
		}

	}

	if( !found ){
		ZnkStr_addf( msg, "グループ[%s]がみつかりません.\n", ZnkStr_cstr(query_group_val) );
	} else {
		if( result ){
			ZnkStr_addf( msg, "グループ[%s]を削除しました.<br>\n", ZnkStr_cstr(query_group_val) );
		} else {
			ZnkStr_addf( msg, "グループ[%s]を削除しようとしましたが失敗しました(空ではありません).\n", ZnkStr_cstr(query_group_val) );
		}
	}

FUNC_END:
	ZnkStr_delete(query_group_val);
	return result;
}


bool
EstTag_registNewGroup( ZnkMyf tags_myf, const char* group, ZnkStr msg, ZnkStrAry category_id_list )
{
	bool result = false;
	ZnkVarp      group_var  = NULL;
	ZnkVarpAry   category_all = ZnkMyf_find_vars( tags_myf, "category_all" );
	const size_t group_size = ZnkVarpAry_size( category_all );
	size_t       group_idx;

	for( group_idx=0; group_idx<group_size; ++group_idx ){
		group_var = ZnkVarpAry_at( category_all, group_idx );
		if( ZnkS_eq( group, ZnkVar_cstr(group_var) ) ){
			ZnkStr_addf( msg, "グループ[%s]は既に存在しています.\n", ZnkVar_cstr(group_var) );
			return false;
		}
	}

	{
		char new_id[ 256 ] = "";
		ZnkVarp new_elem = NULL;
		const size_t ct_size = ZnkStrAry_size( category_id_list );
		size_t       ct_idx;

		/* issue new_id */
		{
			char buf[ 256 ] = "";
			Znk_snprintf( new_id, sizeof(new_id), "group_%s", EstUNID_issue( buf, sizeof(buf) ) );
		}

		/* push_bk to category_all */
		{
			new_elem = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
			ZnkStr_assign( new_elem->name_, 0, new_id, Znk_strlen(new_id) );
			ZnkVar_set_val_Str( new_elem, group, Znk_strlen(group) );
			ZnkVarpAry_push_bk( category_all, new_elem );
		}

		for( ct_idx=0; ct_idx<ct_size; ++ct_idx ){
			/* push_bk to category_<N> */
			const char* category_id = ZnkStrAry_at_cstr( category_id_list, ct_idx );
			ZnkStr_addf( msg, "category_id=[%s].", category_id );
			if( ZnkS_isBegin( category_id, "category_" ) 
				&& !ZnkS_eq( category_id, "category_all" ) && !ZnkS_eq( category_id, "category_list" ) )
			{
				ZnkVarpAry category_N = ZnkMyf_find_vars( tags_myf, category_id );
				if( category_N ){
					ZnkVarp new_elem = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
					ZnkStr_assign( new_elem->name_, 0, new_id, Znk_strlen(new_id) );
					ZnkVar_set_val_Str( new_elem, group, Znk_strlen(group) );
					ZnkVarpAry_push_bk( category_N, new_elem );
				} else {
					ZnkStr_addf( msg, "category_id=[%s] is not found.", category_id );
				}
			}
		}

		ZnkMyf_intern_vars( tags_myf, new_id );

		result = true;
		ZnkStr_addf( msg, "新しいグループ[%s]を登録しました.\n", group );
	}

	return result;
}
bool
EstTag_registNewCategory( ZnkMyf tags_myf, const char* category_name, ZnkStr msg )
{
	bool result = false;
	ZnkVarp      category_var  = NULL;
	ZnkVarpAry   category_list = ZnkMyf_find_vars( tags_myf, "category_list" );
	const size_t category_size = ZnkVarpAry_size( category_list );
	size_t       category_idx;

	if( ZnkS_eq( category_name, "すべて" ) ){
		ZnkStr_addf( msg, "この名前をカテゴリー名として登録することはできません.\n" );
		return false;
	}
	for( category_idx=0; category_idx<category_size; ++category_idx ){
		category_var = ZnkVarpAry_at( category_list, category_idx );
		if( ZnkS_eq( category_name, ZnkVar_cstr(category_var) ) ){
			ZnkStr_addf( msg, "カテゴリー[%s]は既に存在しています.\n", ZnkVar_cstr(category_var) );
			return false;
		}
	}

	{
		char new_id[ 256 ] = "";
		ZnkVarp new_elem = NULL;

		/* issue new_id */
		{
			char buf[ 256 ] = "";
			Znk_snprintf( new_id, sizeof(new_id), "category_%s", EstUNID_issue( buf, sizeof(buf) ) );
		}

		/* push_bk to category_list */
		{
			new_elem = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
			ZnkStr_assign( new_elem->name_, 0, new_id, Znk_strlen(new_id) );
			ZnkVar_set_val_Str( new_elem, category_name, Znk_strlen(category_name) );
			ZnkVarpAry_push_bk( category_list, new_elem );
		}

		ZnkMyf_intern_vars( tags_myf, new_id );

		result = true;
		ZnkStr_addf( msg, "新しいカテゴリー[%s]を登録しました.\n", category_name );
	}

	return result;
}
bool
EstTag_removeCategory( ZnkMyf tags_myf, const char* query_category_key, ZnkStr msg )
{
	ZnkVarpAry category_list = ZnkMyf_find_vars( tags_myf, "category_list" );
	size_t     erase_idx;

	if( ZnkS_eq( query_category_key, "category_all" ) ){
		ZnkStr_addf( msg, "このカテゴリーは削除できません.\n" );
		return false;
	}
	erase_idx = ZnkVarpAry_findIdx_byName( category_list, query_category_key, Znk_NPOS, false );
	if( erase_idx != Znk_NPOS ){
		ZnkVarp varp = ZnkVarpAry_at( category_list, erase_idx );
		ZnkStr_addf( msg, "カテゴリ[%s]を削除しました.\n", ZnkVar_cstr(varp) );
		ZnkVarpAry_erase_byIdx( category_list, erase_idx );
	}

	erase_idx = ZnkMyf_findIdx_withSecType( tags_myf, query_category_key, ZnkMyfSection_e_Vars );
	if( erase_idx != Znk_NPOS ){
		ZnkMyf_erase_byIdx( tags_myf, erase_idx );
	}
	return true;
}


void
EstTag_joinTagIDs( ZnkStr ans, ZnkVarpAry post_vars, char connector_ch )
{
	const size_t   post_vars_size = ZnkVarpAry_size( post_vars );
	const ZnkVarp* post_vars_ptr  = ZnkVarpAry_ary_ptr( post_vars );
	ZnkVarp        post_varp      = NULL;
	size_t idx;
	for( idx=0; idx<post_vars_size; ++idx ){
		post_varp = post_vars_ptr[ idx ];
		if( ZnkS_isBegin( ZnkVar_name_cstr( post_varp ), "tagid" )){
			const char* tag_val = ZnkVar_cstr( post_varp );
			if( ZnkStr_leng( ans ) ){
				ZnkStr_add_c( ans, connector_ch );
			}
			ZnkHtpURL_unescape_toStr( ans, tag_val, Znk_strlen(tag_val) );
		}
	}
}

const char*
EstTag_getCategoryKey( ZnkMyf tags_myf, size_t idx )
{
	ZnkVarpAry   category_list      = ZnkMyf_find_vars( tags_myf, "category_list" );
	const size_t category_list_size = ZnkVarpAry_size( category_list );
	if( category_list_size ){
		ZnkVarp      category_var = ZnkVarpAry_at( category_list, idx );
		const char*  category_key = ZnkVar_name_cstr( category_var );
		return category_key;
	}
	return "category_all";
}
