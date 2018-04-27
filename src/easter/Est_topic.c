#include <Est_topic.h>
#include <Est_config.h>
#include <Est_ui.h>
#include <Est_ui_base.h>
#include <Est_box.h>
#include <Est_box_ui.h>
#include <Est_base.h>
#include <Est_assort.h>
#include <Est_assort_ui.h>
#include <Est_search_manager.h>
#include <Est_recentory.h>
#include <Est_finf.h>
#include <Est_assort_list.h>
#include <Est_hint_manager.h>

#include <Znk_htp_util.h>
#include <Znk_str_ex.h>
#include <Znk_missing_libc.h>
#include <Znk_dir.h>
#include <Znk_mem_find.h>
#include <Znk_str_fio.h>
#include <time.h>
#include <stdio.h>

#define IS_OK( val ) (bool)( (val) != NULL )

static void
makeEstTopicViewLink( ZnkStr ans,
		const char* searched_key, const char* topic_name, bool is_search_result_mode,
		size_t begin_idx, size_t end_idx, const char* authentic_key, bool with_checkbox )
{
	const char* style_class_name = "MstyElemLink";
	ZnkStr uxs_topic_name = ZnkStr_new( topic_name );
	ZnkStr query_string_base = is_search_result_mode ?
		ZnkStr_newf( "est_manager=search&amp;command=result&amp;searched_key=%s", searched_key ) :
		ZnkStr_newf( "est_manager=topic&amp;command=view&amp;searched_key=%s",    searched_key );

	ZnkHtpURL_negateHtmlTagEffection( uxs_topic_name ); /* for XSS */
	ZnkStr_add( ans, "<br>" );
	if( with_checkbox ){
		ZnkStr_addf( ans, "<input type=checkbox name=searched_%s style='margin-top:6px;'>", searched_key );
	}
	ZnkStr_addf( ans, "<a class=%s href=/easter?%s&amp;est_cache_begin=%zu&amp;est_cache_end=%zu&amp;Moai_AuthenticKey=%s#TopicViewer>%s</a>\n",
			style_class_name, ZnkStr_cstr(query_string_base), begin_idx, end_idx, authentic_key,
			ZnkStr_cstr(uxs_topic_name) );

	ZnkStr_delete( query_string_base );
	ZnkStr_delete( uxs_topic_name );
}

static bool
makeEstTopicView( ZnkStr EstTP_view, const char* authentic_key, ZnkStr msg )
{
	size_t show_file_num = EstConfig_getShowFileNum();
	ZnkMyf topic_list_myf = ZnkMyf_create();
	bool result = false;
	char topic_list_myf_path[ 256 ] = "";
	const char* topics_dir = EstConfig_topics_dir();

	Znk_snprintf( topic_list_myf_path, sizeof(topic_list_myf_path), "%s/topic_list.myf", topics_dir );
	if( ZnkMyf_load( topic_list_myf, topic_list_myf_path ) ){
		const size_t  num_of_sec = ZnkMyf_numOfSection( topic_list_myf );
		char          searched_key[ 256 ] = "";
		ZnkMyfSection sec = NULL;
		size_t        sec_idx;

		for( sec_idx=0; sec_idx<num_of_sec; ++sec_idx ){
			sec = ZnkMyf_atSection( topic_list_myf, sec_idx );
			if( ZnkMyfSection_type( sec ) == ZnkMyfSection_e_Vars ){
				ZnkVarpAry  vars  = ZnkMyfSection_vars( sec );
				ZnkVarp     var   = NULL;
				const char* type  = NULL;
				const char* name  = NULL;
				const char* file  = NULL;
				bool        is_ok = false;

				var = ZnkVarpAry_findObj_byName_literal( vars, "type", false );
				if( var == NULL ){ continue; }
				type = ZnkVar_cstr( var );

				var = ZnkVarpAry_findObj_byName_literal( vars, "name", false );
				if( var == NULL ){ continue; }
				name = ZnkVar_cstr( var );

				var = ZnkVarpAry_findObj_byName_literal( vars, "file", false );
				if( var == NULL ){ continue; }
				file = ZnkVar_cstr( var );

				if( ZnkS_eq( type, "searched" ) ){
					if( ZnkS_isBegin( file, "searched_" ) ){
						size_t searched_num = 0;
						const char* p   = file + Znk_strlen_literal( "searched_" );
						const char* ext = ZnkS_get_extension( file, '.' );
						if( ext ){
							ZnkS_copy( searched_key, sizeof(searched_key), p, ext - 1 - p );
							if( ZnkS_getSzU( &searched_num, searched_key ) ){
								is_ok = true;
							}
						}
					}
				}
				if( is_ok ){
					makeEstTopicViewLink( EstTP_view,
							searched_key, name, false,
							0, show_file_num, authentic_key, true );
				}
			}
		}
		result = true;
	} else {
		ZnkStr_addf( msg, "Cannot load [%s]\n", topic_list_myf_path );
		result = false;
	}

	ZnkMyf_destroy( topic_list_myf );
	return result;
}

static size_t
eraseEstTopicItem( const char* query_searched_key, ZnkStr msg )
{
	ZnkMyf topic_list_myf = ZnkMyf_create();
	size_t processed_count = 0;
	char topic_list_myf_path[ 256 ] = "";
	const char* topics_dir = EstConfig_topics_dir();

	Znk_snprintf( topic_list_myf_path, sizeof(topic_list_myf_path), "%s/topic_list.myf", topics_dir );
	if( ZnkMyf_load( topic_list_myf, topic_list_myf_path ) ){
		const size_t  num_of_sec = ZnkMyf_numOfSection( topic_list_myf );
		char          searched_key[ 256 ] = "";
		ZnkMyfSection sec = NULL;
		size_t        sec_idx;

		for( sec_idx=0; sec_idx<num_of_sec; ++sec_idx ){
			sec = ZnkMyf_atSection( topic_list_myf, sec_idx );
			if( ZnkMyfSection_type( sec ) == ZnkMyfSection_e_Vars ){
				ZnkVarpAry  vars  = ZnkMyfSection_vars( sec );
				ZnkVarp     var   = NULL;
				const char* type  = NULL;
				//const char* name  = NULL;
				const char* file  = NULL;

				var = ZnkVarpAry_findObj_byName_literal( vars, "type", false );
				if( var == NULL ){ continue; }
				type = ZnkVar_cstr( var );

#if 0
				var = ZnkVarpAry_findObj_byName_literal( vars, "name", false );
				if( var == NULL ){ continue; }
				name = ZnkVar_cstr( var );
#endif

				var = ZnkVarpAry_findObj_byName_literal( vars, "file", false );
				if( var == NULL ){ continue; }
				file = ZnkVar_cstr( var );

				if( ZnkS_eq( type, "searched" ) ){
					if( ZnkS_isBegin( file, "searched_" ) ){
						const char* p   = file + Znk_strlen_literal( "searched_" );
						const char* ext = ZnkS_get_extension( file, '.' );
						if( ext ){
							ZnkS_copy( searched_key, sizeof(searched_key), p, ext - 1 - p );
							if( ZnkS_eq( query_searched_key, searched_key ) ){
								ZnkMyf_erase_byIdx( topic_list_myf, sec_idx );
								++processed_count;
								break;
							}
						}
					}
				}
			}
		}
		if( processed_count ){
			ZnkMyf_save( topic_list_myf, topic_list_myf_path );
		}
	} else {
		ZnkStr_addf( msg, "Cannot load [%s]\n", topic_list_myf_path );
	}

	ZnkMyf_destroy( topic_list_myf );
	return processed_count;
}
static bool
findEstTopicName( ZnkStr ans_name, const char* query_searched_key, ZnkStr msg )
{
	ZnkMyf topic_list_myf = ZnkMyf_create();
	bool result = false;
	char topic_list_myf_path[ 256 ] = "";
	const char* topics_dir = EstConfig_topics_dir();

	Znk_snprintf( topic_list_myf_path, sizeof(topic_list_myf_path), "%s/topic_list.myf", topics_dir );
	if( ZnkMyf_load( topic_list_myf, topic_list_myf_path ) ){
		const size_t  num_of_sec = ZnkMyf_numOfSection( topic_list_myf );
		char          searched_key[ 256 ] = "";
		ZnkMyfSection sec = NULL;
		size_t        sec_idx;

		for( sec_idx=0; sec_idx<num_of_sec; ++sec_idx ){
			sec = ZnkMyf_atSection( topic_list_myf, sec_idx );
			if( ZnkMyfSection_type( sec ) == ZnkMyfSection_e_Vars ){
				ZnkVarpAry  vars  = ZnkMyfSection_vars( sec );
				ZnkVarp     var   = NULL;
				const char* type  = NULL;
				const char* name  = NULL;
				const char* file  = NULL;

				var = ZnkVarpAry_findObj_byName_literal( vars, "type", false );
				if( var == NULL ){ continue; }
				type = ZnkVar_cstr( var );

				var = ZnkVarpAry_findObj_byName_literal( vars, "name", false );
				if( var == NULL ){ continue; }
				name = ZnkVar_cstr( var );

				var = ZnkVarpAry_findObj_byName_literal( vars, "file", false );
				if( var == NULL ){ continue; }
				file = ZnkVar_cstr( var );

				if( ZnkS_eq( type, "searched" ) ){
					if( ZnkS_isBegin( file, "searched_" ) ){
						const char* p   = file + Znk_strlen_literal( "searched_" );
						const char* ext = ZnkS_get_extension( file, '.' );
						if( ext ){
							ZnkS_copy( searched_key, sizeof(searched_key), p, ext - 1 - p );
							if( ZnkS_eq( query_searched_key, searched_key ) ){
								ZnkStr_set( ans_name, name );
								ZnkHtpURL_negateHtmlTagEffection( ans_name ); /* for XSS */
								result = true;
								goto FUNC_END;
							}
						}
					}
				}
			}
		}
	} else {
		ZnkStr_addf( msg, "Cannot load [%s]\n", topic_list_myf_path );
	}

FUNC_END:
	ZnkMyf_destroy( topic_list_myf );
	return result;
}

typedef bool (*BatOperator_ProcessKeyFunc)( const char* key_name, ZnkStr msg, void* param );

static size_t
processKeys( const ZnkVarpAry given_vars, ZnkStr msg, BatOperator_ProcessKeyFunc proc_func, void* param )
{
	const size_t   given_vars_size = ZnkVarpAry_size( given_vars );
	const ZnkVarp* given_vars_ptr  = ZnkVarpAry_ary_ptr( given_vars );
	ZnkVarp        given_varp      = NULL;
	size_t         processed_count = 0;
	size_t         idx;

	for( idx=0; idx<given_vars_size; ++idx ){
		const char* var_name = NULL;
		given_varp = given_vars_ptr[ idx ];
		var_name = ZnkVar_name_cstr( given_varp );
		if( ZnkS_isBegin( var_name, "searched_" ) ){
			ZnkStr_addf( msg, "processKeys : var_name=[%s]\n", var_name );
			if( proc_func( var_name, msg, param ) ){
				++processed_count;
			}
		}
	}
	return processed_count;
}

static bool
procKeyRemove( const char* key_name, ZnkStr msg, void* param )
{
	if( key_name ){
		const char*  query_searched_key = key_name + Znk_strlen_literal( "searched_" );
		const size_t processed_count    = eraseEstTopicItem( query_searched_key, msg );
		return (bool)( processed_count != 0 );
	}
	return false;
}
static bool
procKeyRemoveConfirm( const char* key_name, ZnkStr msg, void* param )
{
	bool result = false;
	ZnkStr result_view = Znk_force_ptr_cast( ZnkStr, param );
	if( key_name ){
		ZnkStr topic_name = ZnkStr_new( "" );
		const char* query_searched_key = key_name + Znk_strlen_literal( "searched_" );
		if( findEstTopicName( topic_name, query_searched_key, msg ) ){
			ZnkStr_addf( result_view, "<span class=MstyAutoLinkOther>トピックス[%s]</span><br>\n", ZnkStr_cstr(topic_name) );
			ZnkStr_addf( result_view, "<input type=hidden name=searched_%s value=''>\n", query_searched_key );
			result = true;
		}
		ZnkStr_delete( topic_name );
	}
	return result;
}

static bool
getEnableTagList_fromTagsStr( ZnkStrAry tags_list, const char* tags_str )
{
	ZnkStrAry_clear( tags_list );
	ZnkStrEx_addSplitC( tags_list,
			tags_str, Znk_NPOS,
			' ', false, 4 );
	return true;
}

static void
makeTagsView( ZnkStr assort_ui, ZnkVarpAry post_vars, const char* current_category_id )
{
	ZnkStrAry enable_tag_list = ZnkStrAry_create( true );
	ZnkVarp pv = ZnkVarpAry_find_byName_literal( post_vars, "EstSM_tags", false );
	const char* tags = NULL;
	static const char* comment = NULL;
	if( pv ){
		tags = ZnkVar_cstr( pv );
	}
	if( tags ){
		getEnableTagList_fromTagsStr( enable_tag_list, tags );
	}
	EstAssortUI_makeTagsView( assort_ui, enable_tag_list, current_category_id, "", comment, false );
	ZnkStrAry_destroy( enable_tag_list );
}

static void
viewTopic( ZnkBird bird, ZnkVarpAry post_vars, ZnkStr backto, ZnkStr msg, const char* authentic_key )
{
	ZnkVarpAry  finf_list = NULL;
	ZnkStr      EstTP_view = NULL;
	bool        is_ref = false;
	EstRecentory recent = EstRecentory_create();
	size_t show_file_num = EstConfig_getShowFileNum();
	size_t begin_idx = 0;
	size_t end_idx   = show_file_num;
	ZnkStr query_string_base = ZnkStr_new( "" );
	ZnkStr EstCM_img_url_list = ZnkStr_new( "" );
	ZnkStr tags_view = ZnkStr_new( "" );
	ZnkStr tag_editor_ui = ZnkStr_new( "" );
	ZnkStr category_select_bar = ZnkStr_new( "" );
	ZnkStr current_category_name = ZnkStr_new( "すべて" );
	ZnkVarp varp = NULL;
	const char* topics_dir = EstConfig_topics_dir();
	ZnkVarp searched_key = ZnkVarpAry_find_byName_literal( post_vars, "searched_key", false );

	if( searched_key == NULL ){
		/* error */
		EstTP_view = ZnkStr_new( "searched_key is NULL" );
		ZnkStr_addf( msg, "searched_key is NULL\n" );
	} else {
		char finf_list_path[ 256 ] = "";
		const char* current_category_id = "category_all";
		ZnkVarp current_category     = ZnkVarpAry_find_byName_literal( post_vars, "category_key", false );

		if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_cache_begin", false ) )){
			ZnkS_getSzU( &begin_idx, ZnkVar_cstr(varp) );
			ZnkStr_addf( msg, "est_cache_begin=[%zu]\n", begin_idx );
		}
		if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_cache_end", false ) )){
			ZnkS_getSzU( &end_idx, ZnkVar_cstr(varp) );
			ZnkStr_addf( msg, "est_cache_end=[%zu]\n", end_idx );
		}

		ZnkStr_addf( msg, "searched_key=[%s]\n", ZnkVar_cstr(searched_key) );
		if( ZnkS_eq( ZnkVar_cstr(searched_key), "recentory" ) ){
			EstRecentory_load( recent, "recentory.myf" );
			EstTP_view = EstRecentory_title( recent );
			finf_list  = EstRecentory_finf_list( recent );
			is_ref = true;
		} else if( ZnkS_eq( ZnkVar_cstr(searched_key), "stockbox" ) ){
			const char* stockbox_dir = EstConfig_stockbox_dir();
			Znk_snprintf( finf_list_path, sizeof(finf_list_path), "%s/finf_list.myf", stockbox_dir );
			finf_list = EstFInfList_create();
			EstFInfList_load( finf_list, NULL, finf_list_path );
			EstTP_view = ZnkStr_new( "" );
		} else if( ZnkS_eq( ZnkVar_cstr(searched_key), "favorite" ) ){
			const char* favorite_dir = EstConfig_favorite_dir();
			Znk_snprintf( finf_list_path, sizeof(finf_list_path), "%s/finf_list.myf", favorite_dir );
			finf_list = EstFInfList_create();
			EstFInfList_load( finf_list, NULL, finf_list_path );
			EstTP_view = ZnkStr_new( "" );
		} else {
			Znk_snprintf( finf_list_path, sizeof(finf_list_path), "%s/searched_%s.myf", topics_dir, ZnkVar_cstr(searched_key) );
			finf_list = EstFInfList_create();
			if( !EstFInfList_load( finf_list, NULL, finf_list_path ) ){
				ZnkStr_addf( msg, "Cannot load [%s]\n", finf_list_path );
			}
			EstTP_view = ZnkStr_new( "" );
		}

		if( finf_list ){
			size_t finf_list_size = EstFInfList_size( finf_list );
			size_t finf_idx;
			for( finf_idx=0; finf_idx<finf_list_size; ++finf_idx ){
				EstFInf finf = EstFInfList_at( finf_list, finf_idx );
				const char* vpath = EstFInf_file_path( finf );
				EstAssort_addImgURLList( EstCM_img_url_list, finf_idx, begin_idx, end_idx, vpath );
			}
			ZnkStr_setf( query_string_base, "est_manager=topic&amp;command=view&amp;searched_key=%s", ZnkVar_cstr(searched_key) );
			EstUI_showPageSwitcher( EstTP_view, ZnkStr_cstr(query_string_base),
					EstFInfList_size( finf_list ),
					show_file_num, begin_idx, authentic_key, "PageSwitcher" );

			ZnkStr_add( EstTP_view, "<table><tr><td valign=top>\n" );
			EstBoxUI_make_forSearchResult( EstTP_view, finf_list,
					begin_idx, end_idx, authentic_key );
			ZnkStr_add( EstTP_view, "</td><td valign=top><span id=preview></span></td></tr></table>\n" );
		} else {
			ZnkStr_addf( EstTP_view, "searched_key=[%s] does not exist.", ZnkVar_cstr(searched_key) );
		}
		{
			ZnkStr  arg = searched_key ?
				ZnkStr_newf( "searched_key=%s", ZnkVar_cstr(searched_key) ) :
				ZnkStr_new( "searched_key=0" );
			if( current_category ){
				current_category_id = ZnkVar_cstr( current_category );
			}
			EstAssortUI_makeCategorySelectBar( category_select_bar, current_category_id, current_category_name,
					"topic", "view", ZnkStr_cstr(arg), "#TagsView" );
			ZnkStr_delete( arg );
		}

		makeTagsView( tags_view, post_vars, current_category_id );
	}

	if( searched_key ){
		ZnkStr_setf( backto, "%zu,%zu,%s", begin_idx, end_idx, ZnkVar_cstr(searched_key) );
	}

	ZnkBird_regist( bird, "searched_key", searched_key ? ZnkVar_cstr(searched_key) : "" );
	ZnkBird_regist( bird, "Moai_AuthenticKey",  authentic_key );
	ZnkBird_regist( bird, "EstTP_view",   ZnkStr_cstr(EstTP_view) );
	ZnkBird_regist( bird, "EstCM_img_url_list", ZnkStr_cstr(EstCM_img_url_list) );
	ZnkBird_regist( bird, "category_select_bar", ZnkStr_cstr(category_select_bar) );
	ZnkBird_regist( bird, "current_category_name", ZnkStr_cstr(current_category_name) );
	ZnkBird_regist( bird, "tags_view",     ZnkStr_cstr(tags_view) );
	ZnkBird_regist( bird, "tag_editor_ui", ZnkStr_cstr(tag_editor_ui) );
	//ZnkBird_regist( bird, "hint_table", "" );
	ZnkStr_delete( query_string_base );
	ZnkStr_delete( EstCM_img_url_list );
	EstRecentory_destroy( recent );
	ZnkStr_delete( tags_view );
	ZnkStr_delete( tag_editor_ui );
	ZnkStr_delete( category_select_bar );
	ZnkStr_delete( current_category_name );
	if( !is_ref ){
		ZnkStr_delete( EstTP_view );
		EstFInfList_destroy( finf_list );
	}
}

static void
removeTopicList( ZnkBird bird, ZnkVarpAry post_vars, ZnkStr backto, const char** template_html_file, ZnkStr msg, const char* authentic_key )
{
	static const char* style_class_name = "MstyElemLink";

	ZnkVarp confirm = NULL;
	ZnkStr  result_view   = ZnkStr_new( "" );
	ZnkStr  removing_objs = ZnkStr_new( "" );
	bool    is_confirm = false;
	if( IS_OK( confirm = ZnkVarpAry_find_byName_literal( post_vars, "confirm", false ) )){
		if( ZnkS_eq( ZnkVar_cstr(confirm), "on" ) ){
			is_confirm = true;
		}
	}
	*template_html_file = "templates/remove_confirm.html";

	if( is_confirm ){
		ZnkStr_add( result_view, "<font size=-1>\n" );
		ZnkStr_add( result_view, "下記のトピックスを削除しようとしています.<br>\n" );
		ZnkStr_add( result_view, "よろしいですか?<br>\n" );
		ZnkStr_add( result_view, "</font>\n" );
		ZnkStr_add( result_view, "<br>\n" );

		processKeys( post_vars, msg, procKeyRemoveConfirm, result_view );

		ZnkStr_add( result_view, "<br>\n" );
		ZnkStr_addf( result_view, "<a class=MstyElemLink href=\"javascript:EjsAssort_submitCommand( document.fm_main, 'topic', 'remove_topic_list' )\">" );
		ZnkStr_addf( result_view, "はい、実際に削除します.</a><br>\n" );

		ZnkStr_addf( result_view,
				"<a class=%s href=\"/easter?est_manager=topic&Moai_AuthenticKey=%s#PageSwitcher\">いいえ、何もせず戻ります.</a><br>",
				style_class_name, authentic_key );
	} else {
		size_t processed_count = processKeys( post_vars, msg, procKeyRemove, result_view );
		ZnkStr_addf( result_view, "%zu 件削除しました.\n", processed_count );
		ZnkStr_add( result_view, "<br> <br>\n" );
		ZnkStr_addf( result_view,
				"<a class=%s href=\"/easter?est_manager=topic&Moai_AuthenticKey=%s#PageSwitcher\">戻る</a>",
				style_class_name, authentic_key );
		*template_html_file = "templates/command_complete.html";
	}

	ZnkBird_regist( bird, "manager", "topic" );
	ZnkBird_regist( bird, "removing_objs", ZnkStr_cstr(removing_objs) );
	ZnkBird_regist( bird, "Moai_AuthenticKey",  authentic_key );
	ZnkBird_regist( bird, "result_view",   ZnkStr_cstr(result_view) );
	ZnkStr_delete( result_view );
	ZnkStr_delete( removing_objs );
}


void
EstTopicManager_main( RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr msg, const char* authentic_key )
{
	ZnkBird bird = ZnkBird_create( "#[", "]#" );
	const char* template_html_file = "templates/topic_viewer.html";
	ZnkVarp cmd = NULL;
	struct ZnkHtpHdrs_tag htp_hdrs = { 0 };
	RanoModule mod = NULL;
	ZnkStr     pst_str    = ZnkStr_new( "" );
	bool   is_authenticated = false;
	bool   is_unescape_val = false;
	ZnkVarp varp;

	RanoCGIUtil_getPostedFormData( evar, post_vars, mod, &htp_hdrs, pst_str, NULL, is_unescape_val );

	if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "Moai_AuthenticKey", false ) )
	  && ZnkS_eq( authentic_key, ZnkVar_cstr(varp) ) ){
		is_authenticated = true;
	} else {
	}
	ZnkStr_addf( msg, "is_authenticated=[%d]\n", is_authenticated );

	ZnkBird_regist( bird, "EstCM_img_url_list", "" );
	if( IS_OK( cmd = ZnkVarpAry_find_byName_literal( post_vars, "command", false ) )){
		ZnkStr assort_msg = ZnkStr_new( "" );
		ZnkStr  backto = ZnkStr_new( "" );
		ZnkVarp backto_varp = NULL;
		if( IS_OK( backto_varp = ZnkVarpAry_find_byName_literal( post_vars, "backto", false ) )){
			ZnkHtpURL_unescape_toStr( backto, ZnkVar_cstr(backto_varp), ZnkVar_str_leng(backto_varp) );
		}
		if( ZnkS_eq( ZnkVar_cstr(cmd), "view" ) ){
			/* backtoの値はviewTopicで得られるものを優先して上書き */
			viewTopic( bird, post_vars, backto, msg, authentic_key );

		} else if( is_authenticated && ZnkS_eq( ZnkVar_cstr(cmd), "favoritize" ) ){
			/* この関数内ではadd_tags に応じた処理も内部で行う */
			EstAssortList_favoritize( bird, evar, post_vars, backto, assort_msg, msg, authentic_key, "topic", "searched_key" );
			template_html_file = "templates/command_complete.html";
		} else if( is_authenticated && ZnkS_eq( ZnkVar_cstr(cmd), "stock" ) ){
			/* この関数内ではadd_tags に応じた処理も内部で行う */
			EstAssortList_stock( bird, evar, post_vars, backto, assort_msg, msg, authentic_key, "topic", "searched_key" );
			template_html_file = "templates/command_complete.html";
		} else if( ZnkS_eq( ZnkVar_cstr(cmd), "remove_topic_list" ) ){
			removeTopicList( bird, post_vars, backto, &template_html_file, msg, authentic_key );
		}
		ZnkBird_regist( bird, "backto", ZnkStr_cstr(backto) );
		ZnkStr_delete( backto );
		ZnkStr_delete( assort_msg );
	} else {
		ZnkStr EstTP_view  = ZnkStr_new( "" );
		ZnkStr EstTP_entry = ZnkStr_new( "" );
		template_html_file = "templates/topic_list.html";

		{
			size_t show_file_num = EstConfig_getShowFileNum();
			makeEstTopicViewLink( EstTP_entry,
					"favorite", "お気に入りボックス", false,
					0, show_file_num, authentic_key, false );
			makeEstTopicViewLink( EstTP_entry,
					"stockbox", "ストックボックス", false,
					0, show_file_num, authentic_key, false );
			makeEstTopicViewLink( EstTP_entry,
					"recentory", "最近処理したもの", false,
					0, show_file_num, authentic_key, false );
		}

		makeEstTopicView( EstTP_view, authentic_key, msg );

		ZnkStr_addf( EstTP_view, "<br> <br>" );
		ZnkStr_addf( EstTP_view, "<a class=MstyElemLinkRed href=\"javascript:EjsAssort_submitCommand( document.fm_main, 'topic', 'remove_topic_list' )\">チェックが入ったトピックスを削除</a>\n" );
		ZnkStr_addf( EstTP_view, "<input type=hidden name=confirm value='on'>\n" );

		ZnkBird_regist( bird, "Moai_AuthenticKey",  authentic_key );
		ZnkBird_regist( bird, "EstTP_entry",  ZnkStr_cstr(EstTP_entry) );
		ZnkBird_regist( bird, "EstTP_view",   ZnkStr_cstr(EstTP_view) );
		ZnkStr_delete( EstTP_entry );
		ZnkStr_delete( EstTP_view );
	}

	{
		ZnkStr hint_table = EstHint_getHintTable( "topic" );
		if( hint_table ){
			ZnkBird_regist( bird, "hint_table", ZnkStr_cstr(hint_table) );
		} else {
			ZnkBird_regist( bird, "hint_table", "" );
		}
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
}
