#include <Est_boxmap_viewer.h>
#include <Est_config.h>
#include <Est_ui.h>
#include <Est_base.h>
#include <Est_dir_util.h>
#include <Est_assort.h>
#include <Est_assort_ui.h>
#include <Est_hint_manager.h>
#include <Est_tag.h>
#include <Est_bat_operator.h>
#include <Est_box.h>
#include <Est_box_ui.h>
#include <Est_assort_list.h>

#include <Rano_file_info.h>
#include <Rano_html_ui.h>
#include <Rano_dir_util.h>

#include <Znk_htp_util.h>
#include <Znk_dir.h>
#include <Znk_dir_recursive.h>
#include <Znk_missing_libc.h>
#include <Znk_str_ex.h>
#include <Znk_date.h>
#include <Znk_mem_find.h>
#include <Znk_str_fio.h>
#include <Znk_md5.h>

#include <stdio.h>

#define IS_OK( val ) (bool)( (val) != NULL )

static EstBoxDirType
makeCacheView( RanoCGIEVar* evar,
		ZnkStr caches, const char* elem_name_src, size_t begin_idx, size_t end_idx, size_t show_file_num, const char* authentic_key,
		ZnkStr EstCM_img_url_list, const char* favorite_dir, const char* assort_msg )
{
	char path[ 256 ] = "";
	char vpath[ 256 ] = "";
	const char* elem_name = NULL;
	ZnkStr uxs_elem_name = ZnkStr_new( elem_name_src );
	ZnkStr fsys_path = ZnkStr_new( "" );
	EstBoxDirType dir_type = EstBoxDir_e_Unknown;
	size_t box_path_offset = 0;
	//bool   is_dirlist_first_style = false;
	bool   is_dirlist_first_style = true;

	/***
	 * Security check
	 */
	ZnkHtpURL_negateHtmlTagEffection( uxs_elem_name ); /* for XSS */
	ZnkStr_chompC( uxs_elem_name, Znk_NPOS, '/' );
	elem_name = ZnkStr_cstr( uxs_elem_name );

	dir_type = EstBoxBase_convertBoxDir_toFSysDir( fsys_path, elem_name, &box_path_offset, caches );

	if( ZnkDir_getType( ZnkStr_cstr(fsys_path) ) == ZnkDirType_e_Directory ){
		ZnkVarpAry cache_dir_list  = ZnkVarpAry_create( true );
		ZnkVarpAry cache_file_list = ZnkVarpAry_create( true );
		ZnkDirId id = ZnkDir_openDir( ZnkStr_cstr(fsys_path) );
		char   new_id[ 256 ] = "";
		size_t file_count = 0;
		ZnkStr elem_name_mbc_esc = ZnkStr_new( elem_name );

		if( id ){
			const char* name = "";
			/***
			 * new_idxはwhileで高速で回る中ではtime()の値でもまずいし勿論以下のように単にインクリメントするだけでもまずい.
			 * 絶対にかぶらないかつ常再現性のあるissue方法を検討せねばならない.
			 */
			size_t new_file_idx = 0;
			size_t new_dir_idx = 0;
			const char* p = Znk_strrchr( elem_name, '/' );
			ZnkStr elem_name_mbc     = ZnkStr_new( elem_name );
			ZnkStr name_esc = ZnkStr_new( "" );
			EstBase_unescape_forMbcFSysPath( elem_name_mbc );
			EstBase_escape_forURL( elem_name_mbc_esc, 1 );

			//ZnkStr_addf( caches, "<b>カレントディレクトリ : <span class=MstyAutoLinkOther>%s</span></b>\n", ZnkStr_cstr(elem_name_mbc) );
			ZnkStr_addf( caches, "<input type=hidden name=est_cache_pwd   value=\"%s\">\n",  ZnkStr_cstr(elem_name_mbc) );
			ZnkStr_addf( caches, "<input type=hidden name=est_cache_begin value=\"%zu\">\n", begin_idx );
			ZnkStr_addf( caches, "<input type=hidden name=est_cache_end   value=\"%zu\">\n", end_idx );

			ZnkStr_addf( caches, " <a class=MstyAppLink href='/easter?"
					"est_manager=cache&amp;command=view&amp;cache_path=%s"
					"&amp;est_cache_begin=%zu"
					"&amp;est_cache_end=%zu"
					"&amp;Moai_AuthenticKey=%s'><b>%s</b></a>\n",
					ZnkStr_cstr(elem_name_mbc), begin_idx, end_idx, authentic_key, ZnkStr_cstr(elem_name_mbc) );
			if( p ){
				ZnkS_copy( path, sizeof(path), elem_name, p-elem_name );
				ZnkStr_addf( caches,
						" &nbsp;<a class=MstyAppLink href='/easter?"
						"est_manager=cache&amp;command=view&amp;cache_path=%s"
						"&amp;est_cache_end=%zu"
						"&amp;Moai_AuthenticKey=%s'>上のフォルダへ移動</a>\n",
						path, show_file_num, authentic_key );
			}

			//ZnkStr_addf( caches, " <span id=now_selected_path></span>\n" );
			//ZnkStr_addf( caches, "<hr>" );
			ZnkStr_addf( caches, "<br>" );

			while( true ){
				name = ZnkDir_readDir( id );
				if( name == NULL ){
					break;
				}
				ZnkStr_clear( name_esc );
				ZnkHtpURL_escape( name_esc, (uint8_t*)name, Znk_strlen(name) );
				Znk_snprintf( path, sizeof(path), "%s/%s", ZnkStr_cstr(fsys_path), name );
				if( ZnkDir_getType( path ) == ZnkDirType_e_Directory ){
					ZnkVarp new_elem = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
					Znk_snprintf( new_id, sizeof(new_id), "%s%zu", "cache_dir", new_dir_idx );
					ZnkStr_assign( new_elem->name_, 0, new_id, Znk_strlen(new_id) );
					Znk_snprintf( vpath, sizeof(vpath), "%s/%s", ZnkStr_cstr(elem_name_mbc_esc), ZnkStr_cstr(name_esc) );
					ZnkVar_set_val_Str( new_elem, vpath, Znk_strlen(vpath) );
					ZnkVarpAry_push_bk( cache_dir_list, new_elem );
					++new_dir_idx;
				} else if( ZnkDir_getType( path ) == ZnkDirType_e_File ){
					ZnkVarp new_elem = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
					Znk_snprintf( new_id, sizeof(new_id), "%s%zu", "cache_file", new_file_idx );
					ZnkStr_assign( new_elem->name_, 0, new_id, Znk_strlen(new_id) );
					Znk_snprintf( vpath, sizeof(vpath), "%s/%s", ZnkStr_cstr(elem_name_mbc_esc), ZnkStr_cstr(name_esc) );
					ZnkVar_set_val_Str( new_elem, vpath, Znk_strlen(vpath) );
					ZnkVarpAry_push_bk( cache_file_list, new_elem );
					++new_file_idx;
					EstAssort_addImgURLList( EstCM_img_url_list, file_count, begin_idx, end_idx, vpath );
					++file_count;
				}
			}
			ZnkDir_closeDir( id );

			if( is_dirlist_first_style ){
				/* dir_list */
				if( ZnkVarpAry_size( cache_dir_list ) ){
					ZnkStr_addf( caches, "<div class=MstyAutoLinkOther>\n" );
					ZnkStr_addf( caches, "<b><u>フォルダ一覧</u></b><br> <br>\n" );
					EstBoxUI_make_forFSysView( caches, cache_dir_list,
							0, Znk_NPOS, authentic_key, false, false );
					ZnkStr_addf( caches, "</div>\n" );
					ZnkStr_addf( caches, "<br>" );
				}
			}

#if 0
			ZnkStr_addf( caches, "<a class=MstyElemLink onclick=\"EjsAssort_checkExist( '%s', %zu, %zu, '%s', previewing_img_url )\">CheckExist</a> &nbsp;",
					ZnkStr_cstr(elem_name_mbc_esc), begin_idx, end_idx, authentic_key );
#endif

			ZnkStr_delete( elem_name_mbc );
			ZnkStr_delete( name_esc );
		}
		if( is_dirlist_first_style ){
#if 0
			/* dir_list */
			EstBoxUI_make_forFSysView( caches, cache_dir_list,
					0, Znk_NPOS, authentic_key, false, false );
			if( ZnkVarpAry_size( cache_dir_list ) && ZnkVarpAry_size( cache_file_list ) ){
				ZnkStr_addf( caches, "<hr>" );
			}
#endif

			/* file_list */
			if( ZnkVarpAry_size( cache_file_list ) ){
				ZnkStr_addf( caches, "<div class=MstyAutoLinkOther>\n" );
				ZnkStr_addf( caches, "<b><u>ファイル一覧</u></b><br> <br>\n" );
				{
					ZnkStr query_string_base = ZnkStr_newf( "est_manager=cache&amp;command=view&amp;cache_path=%s", ZnkStr_cstr(elem_name_mbc_esc) );
					EstUI_showPageSwitcher( caches, ZnkStr_cstr(query_string_base),
							ZnkVarpAry_size( cache_file_list ),
							show_file_num, begin_idx, authentic_key, "PageSwitcher" );
					ZnkStr_delete( query_string_base );
				}
				ZnkStr_add( caches, "<table><tr><td valign=top>\n" );
				EstBoxUI_make_forFSysView( caches, cache_file_list,
						begin_idx, end_idx, authentic_key, true, false );
				ZnkStr_addf( caches, "</td><td valign=top>%s<span id=preview></span></td></tr></table>\n", assort_msg );
				ZnkStr_addf( caches, "</div>" );
			}
		} else {
			/* file_list */
			if( ZnkVarpAry_size( cache_file_list ) ){
				ZnkStr_addf( caches, "<div class=MstyAutoLinkOther>\n" );
				{
					ZnkStr query_string_base = ZnkStr_newf( "est_manager=cache&amp;command=view&amp;cache_path=%s", ZnkStr_cstr(elem_name_mbc_esc) );
					EstUI_showPageSwitcher( caches, ZnkStr_cstr(query_string_base),
							ZnkVarpAry_size( cache_file_list ),
							show_file_num, begin_idx, authentic_key, "PageSwitcher" );
					ZnkStr_delete( query_string_base );
				}
				ZnkStr_add( caches, "<table><tr><td valign=top>\n" );
				EstBoxUI_make_forFSysView( caches, cache_file_list,
						begin_idx, end_idx, authentic_key, true, false );
				ZnkStr_addf( caches, "</td><td valign=top>%s<span id=preview></span></td></tr></table>\n", assort_msg );
				ZnkStr_addf( caches, "</div>" );
			}

			/* dir_list */
			if( ZnkVarpAry_size( cache_dir_list ) && ZnkVarpAry_size( cache_file_list ) ){
				ZnkStr_addf( caches, "<hr>" );
			}
			EstBoxUI_make_forFSysView( caches, cache_dir_list,
					0, Znk_NPOS, authentic_key, false, false );
		}

		ZnkStr_delete( elem_name_mbc_esc );
		ZnkVarpAry_destroy( cache_dir_list );
		ZnkVarpAry_destroy( cache_file_list );

	} else if( ZnkDir_getType( ZnkStr_cstr(fsys_path) ) == ZnkDirType_e_File ){
		const char* ext = ZnkS_get_extension( elem_name, '.' );
		const char* p = Znk_strrchr( elem_name, '/' );
		if( p ){
			ZnkS_copy( path, sizeof(path), elem_name, p-elem_name );
			ZnkStr_addf( caches, "カレントディレクトリ : <a class=MstyElemLink href='/easter?"
					"est_manager=cache&amp;command=view&amp;cache_path=%s"
					"&amp;Moai_AuthenticKey=%s'>%s</a><hr>\n",
					path, authentic_key,
					path );
		}
		if( ZnkS_eqCase( ext, "jpg" ) 
		 || ZnkS_eqCase( ext, "png" )
		 || ZnkS_eqCase( ext, "gif" ) )
		{
			ZnkStr_addf( caches, "<a href='/cgis/easter/%s'>%s</a><br>\n", elem_name, elem_name );
			ZnkStr_addf( caches, "画像サイズ自重モード: width=512<br>\n" );
			ZnkStr_addf( caches, "<a href='/cgis/easter/%s'><img src='/cgis/easter/%s' width=512></a>\n", elem_name, elem_name );
		} else {
			ZnkStr_addf( caches, "<a href='/cgis/easter/%s'>%s</a><br>\n", elem_name, elem_name );
		}
	} else {
		ZnkStr_addf( caches, "none cache in current directory=[%s] (fsys_path=[%s]) dir_type=[%d]\n", elem_name, ZnkStr_cstr(fsys_path), dir_type );
	}

	//ZnkStr_add( caches, "<hr>\n" );
	ZnkStr_add( caches, "<br> <br>\n" );
	ZnkStr_addf( caches, "<a class=MstyElemLink onclick=\"EjsAssort_checkAll( 'cache_file', %zu, %zu, true )\">全部にCheckを入れる</a> &nbsp;", begin_idx, end_idx );
	ZnkStr_addf( caches, "<a class=MstyElemLink onclick=\"EjsAssort_checkAll( 'cache_file', %zu, %zu, false )\">全部のCheckを外す</a>", begin_idx, end_idx );
	//ZnkStr_add( caches, "<br>" );
	ZnkStr_add( caches, "<br>\n" );

	ZnkStr_delete( uxs_elem_name );
	ZnkStr_delete( fsys_path );
	return dir_type;
}

static void
viewCache( ZnkBird bird, RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr msg, const char* authentic_key,
		ZnkStr EstCM_img_url_list, const char* assort_msg, const char* comment )
{
	ZnkStr caches = ZnkStr_new( "" );
	ZnkStr assort_ui = ZnkStr_new( "" );
	ZnkStr tag_editor_ui = ZnkStr_new( "" );
	ZnkStr backto = ZnkStr_new( "" );
	ZnkStr current_category_name = ZnkStr_new( "" );
	ZnkStr category_select_bar = ZnkStr_new( "" );
	ZnkVarp varp = NULL;

	/* cache_path は URL内で与えられているのでunescapeは不要 */
	size_t begin_idx = 0;
	size_t end_idx   = Znk_NPOS;
	const size_t show_file_num = EstConfig_getShowFileNum();
	const char* favorite_dir = EstConfig_favorite_dir();
	const char* current_category_id = "category_all";
	ZnkVarp current_category = ZnkVarpAry_find_byName_literal( post_vars, "category_key", false );

	if( current_category ){
		current_category_id = ZnkVar_cstr( current_category );
	} else {
		/***
		 * current_category が明示的に指定されていない場合は一番先頭のものが選ばれているものとする.
		 */
		ZnkMyf tags_myf = EstConfig_tags_myf();
		current_category_id = EstTag_getCategoryKey( tags_myf, 0 );
	}

	if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "cache_path", false ) )){
		EstBoxDirType dir_type = EstBoxDir_e_Unknown;
		const char* cache_path = ZnkVar_cstr(varp);
		ZnkStr elem_name_mbc   = ZnkStr_new( cache_path );
		EstBase_unescape_forMbcFSysPath( elem_name_mbc );
		ZnkHtpURL_sanitizeReqUrpDir( elem_name_mbc, false );

		ZnkStr_addf( msg, "EstCacheManager : view : elem_name=[%s]\n", ZnkStr_cstr(elem_name_mbc) );
		if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_cache_begin", false ) )){
			ZnkS_getSzU( &begin_idx, ZnkVar_cstr(varp) );
			ZnkStr_addf( msg, "est_cache_begin=[%zu]\n", begin_idx );
		}
		if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_cache_end", false ) )){
			ZnkS_getSzU( &end_idx, ZnkVar_cstr(varp) );
			ZnkStr_addf( msg, "est_cache_end=[%zu]\n", end_idx );
		}
		ZnkStr_setf( backto, "%zu,%zu,%s", begin_idx, end_idx, ZnkStr_cstr( elem_name_mbc ) );

		dir_type = makeCacheView( evar, caches, ZnkStr_cstr(elem_name_mbc), begin_idx, end_idx, show_file_num, authentic_key,
				EstCM_img_url_list, favorite_dir, assort_msg );

		//if( dir_type != EstBoxDir_e_Favorite ){
		Znk_UNUSED( dir_type );
		{
			ZnkStr arg = ZnkStr_newf( "cache_path=%s&est_cache_begin=%zu&est_cache_end=%zu", cache_path, begin_idx, end_idx );
			EstAssortUI_makeCategorySelectBar( category_select_bar, current_category_id, current_category_name,
					"cache", "view", ZnkStr_cstr(arg), "#TagsView" );
			EstAssortUI_makeTagsView( assort_ui, NULL, current_category_id, NULL, comment, false );
			ZnkStr_delete( arg );
		}

		ZnkStr_delete( elem_name_mbc );
	} else {
		ZnkStr_add( msg, "cache_path is not found.\n" );
	}

	ZnkBird_regist( bird, "assort_ui", ZnkStr_cstr(assort_ui) );
	ZnkBird_regist( bird, "tag_editor_ui", ZnkStr_cstr(tag_editor_ui) );
	ZnkBird_regist( bird, "caches",    ZnkStr_cstr(caches) );
	ZnkBird_regist( bird, "backto", ZnkStr_cstr(backto) );
	ZnkBird_regist( bird, "category_select_bar", ZnkStr_cstr(category_select_bar) );
	ZnkBird_regist( bird, "current_category_name", ZnkStr_cstr(current_category_name) );
	ZnkStr_delete( caches );
	ZnkStr_delete( assort_ui );
	ZnkStr_delete( tag_editor_ui );
	ZnkStr_delete( backto );
	ZnkStr_delete( current_category_name );
	ZnkStr_delete( category_select_bar );
}


static void
removeCaches( ZnkBird bird,
		RanoCGIEVar* evar, ZnkVarpAry post_vars,
		const char** template_html_file, ZnkStr EstCM_img_url_list,
		ZnkStr backto, ZnkStr assort_msg, ZnkStr msg, const char* authentic_key,
		bool this_is_dustbox )
{
	/***
	 * TODO:
	 * 以下を削除します.
	 * 本当によろしいですか？
	 * 画面を出し、「削除」ボタンを押すとカレントのEstCacheManager画面に戻る.
	 */
	ZnkVarp confirm;
	ZnkVarp varp;
	ZnkStr  result_view = ZnkStr_new( "" );
	ZnkStr  assort_ui = ZnkStr_new( "" );
	ZnkStr  tag_editor_ui = ZnkStr_new( "" );
	size_t  processed_count = 0;
	bool    is_confirm = false;
	const size_t show_file_num = EstConfig_getShowFileNum();
	const char* style_class_name = "MstyElemLink";
	const char* favorite_dir = EstConfig_favorite_dir();

	*template_html_file = "templates/boxmap_viewer.html";

	if( IS_OK( confirm = ZnkVarpAry_find_byName_literal( post_vars, "confirm", false ) )){
		if( ZnkS_eq( ZnkVar_cstr(confirm), "on" ) ){
			is_confirm = true;
		}
	}

	if( is_confirm ){
		const char* est_manager = "cache";
		*template_html_file = "templates/remove_confirm.html";
		EstUI_makeCheckedConfirmView( evar, post_vars,
				result_view, show_file_num, authentic_key,
				ZnkStr_cstr(assort_msg) );

		ZnkStr_add( result_view, "<br>\n" );
		ZnkStr_add( result_view, "<font size=-1>\n" );
		ZnkStr_add( result_view, "上記のものを削除しようとしています.<br>\n" );
		ZnkStr_add( result_view, "よろしいですか?<br>\n" );
		ZnkStr_add( result_view, "</font>\n" );
		ZnkStr_add( result_view, "<br>\n" );

		ZnkStr_add( result_view, "<br>\n" );
		ZnkStr_addf( result_view, "<a class=%s href=\"javascript:EjsAssort_submitCommandAndArg( document.fm_main, 'cache', 'remove', 'this_is_dustbox=%s' )\">",
				style_class_name, this_is_dustbox ? "true" : "false" );
		ZnkStr_addf( result_view, "はい、実際に削除します.</a><br>\n" );

		if( ZnkStr_leng(backto) ){
			char bkt_vpath[ 256 ] = "";
			unsigned int bkt_begin_idx = 0;
			unsigned int bkt_end_idx   = 0;
			sscanf( ZnkStr_cstr(backto), "%u,%u,%s", &bkt_begin_idx, &bkt_end_idx, bkt_vpath );
			ZnkStr_addf( result_view,
					"<a class=%s href=\"/easter?est_manager=%s&command=view&"
					"cache_path=%s&est_cache_begin=%u&est_cache_end=%u&Moai_AuthenticKey=%s#PageSwitcher\">いいえ、何もせずに戻ります</a><br>",
					style_class_name, est_manager,
					bkt_vpath, bkt_begin_idx, bkt_end_idx, authentic_key );
		} else {
			ZnkStr_addf( result_view,
					"<a class=%s href=\"/easter?est_manager=%s&command=view&"
					"Moai_AuthenticKey=%s#PageSwitcher\">いいえ、何もせず戻ります.</a><br>",
					style_class_name, est_manager,
					authentic_key );
		}
	} else {
		ZnkVarp pwd;
		if( this_is_dustbox ){
			processed_count = EstBox_remove(  post_vars, msg );
		} else {
			processed_count = EstBox_dustize( post_vars, msg );
		}
		if( IS_OK( pwd = ZnkVarpAry_find_byName_literal( post_vars, "est_cache_pwd", false ) )){
			size_t begin_idx = 0;
			size_t end_idx   = Znk_NPOS;

			ZnkStr unesc_pwd = ZnkStr_new( "" );
			ZnkHtpURL_unescape_toStr( unesc_pwd, ZnkVar_cstr(pwd), ZnkVar_str_leng(pwd) );
			ZnkHtpURL_sanitizeReqUrpDir( unesc_pwd, true );

			if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_cache_begin", false ) )){
				ZnkS_getSzU( &begin_idx, ZnkVar_cstr(varp) );
				ZnkStr_addf( msg, "est_cache_begin=[%zu]\n", begin_idx );
			}
			if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_cache_end", false ) )){
				ZnkS_getSzU( &end_idx, ZnkVar_cstr(varp) );
				ZnkStr_addf( msg, "est_cache_end=[%zu]\n", end_idx );
			}

			makeCacheView( evar, result_view, ZnkStr_cstr(unesc_pwd), begin_idx, end_idx, show_file_num, authentic_key,
					EstCM_img_url_list, favorite_dir, ZnkStr_cstr(assort_msg) );
			ZnkStr_delete( unesc_pwd );
		} else {
			*template_html_file = "templates/command_complete.html";
		}

		ZnkStr_addf( result_view, "%zu 件削除しました.\n", processed_count );
		if( ZnkStr_leng(backto) ){
			char bkt_vpath[ 256 ] = "";
			unsigned int bkt_begin_idx = 0;
			unsigned int bkt_end_idx   = 0;
			sscanf( ZnkStr_cstr(backto), "%u,%u,%s", &bkt_begin_idx, &bkt_end_idx, bkt_vpath );
			ZnkStr_addf( result_view, "<br><a class=%s href=\"/easter?est_manager=cache&command=view&cache_path=%s&est_cache_begin=%u&est_cache_end=%u&Moai_AuthenticKey=%s#PageSwitcher\">戻る</a>",
					style_class_name,
					bkt_vpath, bkt_begin_idx, bkt_end_idx, authentic_key );
		} else {
			ZnkStr_addf( result_view, "<br><br><font size=-1>このタブ画面はEasterではもはや使用されないため、不要ならブラウザのボタンでお閉じください.</font>" );
		}
	}

	ZnkBird_regist( bird, "assort_ui",     ZnkStr_cstr(assort_ui) );
	ZnkBird_regist( bird, "tag_editor_ui", ZnkStr_cstr(tag_editor_ui) );
	ZnkBird_regist( bird, "result_view",   ZnkStr_cstr(result_view) );
	ZnkBird_regist( bird, "backto",        ZnkStr_cstr(backto) );
	ZnkBird_regist( bird, "manager",       "cache" );
	ZnkStr_delete( result_view );
	ZnkStr_delete( assort_ui );
	ZnkStr_delete( tag_editor_ui );
}

static void
cleanCacheBox( ZnkBird bird,
		RanoCGIEVar* evar, ZnkVarpAry post_vars,
		const char** template_html_file, ZnkStr EstCM_img_url_list,
		ZnkStr backto, ZnkStr assort_msg, ZnkStr msg, const char* authentic_key )
{
	/***
	 * TODO:
	 * 以下を削除します.
	 * 本当によろしいですか？
	 * 画面を出し、「削除」ボタンを押すとカレントのEstCacheManager画面に戻る.
	 */
	ZnkVarp confirm;
	ZnkVarp varp;
	ZnkStr  result_view = ZnkStr_new( "" );
	ZnkStr  assort_ui = ZnkStr_new( "" );
	ZnkStr  tag_editor_ui = ZnkStr_new( "" );
	size_t  processed_count = 0;
	bool    is_confirm = false;
	const size_t show_file_num = EstConfig_getShowFileNum();
	const char* style_class_name = "MstyElemLink";
	const char* favorite_dir = EstConfig_favorite_dir();

	*template_html_file = "templates/boxmap_viewer.html";

	if( IS_OK( confirm = ZnkVarpAry_find_byName_literal( post_vars, "confirm", false ) )){
		if( ZnkS_eq( ZnkVar_cstr(confirm), "on" ) ){
			is_confirm = true;
		}
	}

	if( is_confirm ){
		const char* est_manager = "cache";
		*template_html_file = "templates/remove_confirm.html";
		EstUI_makeCheckedConfirmView( evar, post_vars,
				result_view, show_file_num, authentic_key,
				ZnkStr_cstr(assort_msg) );

		ZnkStr_add( result_view, "<br>\n" );
		ZnkStr_add( result_view, "<font size=-1>\n" );
		ZnkStr_add( result_view, "cacheboxを全削除しようとしています.<br>\n" );
		ZnkStr_add( result_view, "よろしいですか?<br>\n" );
		ZnkStr_add( result_view, "</font>\n" );
		ZnkStr_add( result_view, "<br>\n" );

		ZnkStr_add( result_view, "<br>\n" );
		ZnkStr_addf( result_view, "<a class=%s href=\"javascript:EjsAssort_submitCommandAndArg( document.fm_main, 'cache', 'clean_cachebox', '' )\">", style_class_name );
		ZnkStr_addf( result_view, "はい、実際に削除します.</a><br>\n" );

		if( ZnkStr_leng(backto) ){
			char bkt_vpath[ 256 ] = "";
			unsigned int bkt_begin_idx = 0;
			unsigned int bkt_end_idx   = 0;
			sscanf( ZnkStr_cstr(backto), "%u,%u,%s", &bkt_begin_idx, &bkt_end_idx, bkt_vpath );
			ZnkStr_addf( result_view,
					"<a class=%s href=\"/easter?est_manager=%s&command=view&"
					"cache_path=%s&est_cache_begin=%u&est_cache_end=%u&Moai_AuthenticKey=%s#PageSwitcher\">いいえ、何もせずに戻ります</a><br>",
					style_class_name, est_manager,
					bkt_vpath, bkt_begin_idx, bkt_end_idx, authentic_key );
		} else {
			ZnkStr_addf( result_view,
					"<a class=%s href=\"/easter?est_manager=%s&command=view&"
					"Moai_AuthenticKey=%s#PageSwitcher\">いいえ、何もせず戻ります.</a><br>",
					style_class_name, est_manager,
					authentic_key );
		}
	} else {
		ZnkVarp pwd;

		{
			ZnkDirId dirid = ZnkDir_openDir( "cachebox" );
			if( dirid ){
				const char* name;
				char path[ 256 ];
				while( true ){
					name = ZnkDir_readDir( dirid );
					if( name == NULL ){
						break;
					}
					Znk_snprintf( path, sizeof(path), "cachebox/%s", name ); 
					RanoDirUtil_moveDir( path, "dustbox",
							"cleanCacheBox", msg,
							NULL, NULL );
				}
				ZnkDir_closeDir( dirid );
			}
		}

		*template_html_file = "templates/command_complete.html";

		ZnkStr_addf( result_view, "cacheboxを全削除しました.\n" );
		if( ZnkStr_leng(backto) ){
			char bkt_vpath[ 256 ] = "";
			unsigned int bkt_begin_idx = 0;
			unsigned int bkt_end_idx   = 0;
			sscanf( ZnkStr_cstr(backto), "%u,%u,%s", &bkt_begin_idx, &bkt_end_idx, bkt_vpath );
			ZnkStr_addf( result_view, "<br><a class=%s href=\"/easter?est_manager=cache&command=view&cache_path=%s&est_cache_begin=%u&est_cache_end=%u&Moai_AuthenticKey=%s#PageSwitcher\">戻る</a>",
					style_class_name,
					bkt_vpath, bkt_begin_idx, bkt_end_idx, authentic_key );
		} else {
			ZnkStr_addf( result_view, "<br><br><font size=-1>このタブ画面はEasterではもはや使用されないため、不要ならブラウザのボタンでお閉じください.</font>" );
		}
	}

	ZnkBird_regist( bird, "assort_ui",     ZnkStr_cstr(assort_ui) );
	ZnkBird_regist( bird, "tag_editor_ui", ZnkStr_cstr(tag_editor_ui) );
	ZnkBird_regist( bird, "result_view",   ZnkStr_cstr(result_view) );
	ZnkBird_regist( bird, "backto",        ZnkStr_cstr(backto) );
	ZnkBird_regist( bird, "manager",       "cache" );
	ZnkStr_delete( result_view );
	ZnkStr_delete( assort_ui );
	ZnkStr_delete( tag_editor_ui );
}


static void
getMappingStatus( ZnkStr ans )
{
	const char* favorite_dir = EstConfig_favorite_dir();
	const char* stockbox_dir = EstConfig_stockbox_dir();
	const char* userbox_dir  = EstConfig_userbox_dir();
	ZnkStr_addf( ans, "<fieldset class=MstyStdFieldSet><legend>Boxの現在のMapping状態</legend>" );
	ZnkStr_addf( ans, "<ul>\n" );
	ZnkStr_addf( ans, "<li><b>favorite</b></li><div class=MstyComment>%s</div>\n", favorite_dir );
	ZnkStr_addf( ans, "<li><b>stockbox</b></li><div class=MstyComment>%s</div>\n", stockbox_dir );
	ZnkStr_addf( ans, "<li><b>userbox</b></li><div class=MstyComment>%s</div>\n",  userbox_dir );
	ZnkStr_addf( ans, "</ul>\n" );
	ZnkStr_addf( ans, "</fieldset>" );
}

void
EstCacheManager_main( RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr msg, const char* authentic_key )
{
	ZnkBird bird = ZnkBird_create( "#[", "]#" );
	const char* template_html_file = "templates/boxmap_viewer.html";
	struct ZnkHtpHdrs_tag htp_hdrs = { 0 };
	RanoModule mod = NULL;
	ZnkStr     EstCM_img_url_list = ZnkStr_new( "" );
	ZnkStr     pst_str    = ZnkStr_new( "" );
	ZnkStr     new_linkid = ZnkStr_new( "" );
	ZnkStr     assort_msg = ZnkStr_new( "" );
	ZnkStr     backto     = ZnkStr_new( "" );
	/***
	 * だいたい１項目あたり縦に34pixel
	 * ブラウザのバーとタスクバーなどで7項目分くらい消費する.
	 * 1080の場合だいたい 34 * 31 : 31 - 7 = 24 項目
	 * 768の場合だいたい  34 * 22 : 22 - 7 = 15 項目
	 */
	const size_t show_file_num = EstConfig_getShowFileNum();
	bool   is_authenticated = false;
	bool   is_unescape_val = false;
	ZnkVarp varp;
	ZnkVarp cmd;
	ZnkVarp backto_varp;
	const char* favorite_dir = EstConfig_favorite_dir();
	const char* style_class_name = "MstyElemLink";
	const char* comment = "";

	RanoCGIUtil_getPostedFormData( evar, post_vars, mod, &htp_hdrs, pst_str, NULL, is_unescape_val );

	if( IS_OK( backto_varp = ZnkVarpAry_find_byName_literal( post_vars, "backto", false ) )){
		ZnkHtpURL_unescape_toStr( backto, ZnkVar_cstr(backto_varp), ZnkVar_str_leng(backto_varp) );
	}


	if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "Moai_AuthenticKey", false ) )
	  && ZnkS_eq( authentic_key, ZnkVar_cstr(varp) ) ){
		is_authenticated = true;
	} else {
	}
	ZnkStr_addf( msg, "is_authenticated=[%d]\n", is_authenticated );

	ZnkStr_addf( msg, "query_string=[%s]\n", evar->query_string_ );

	cmd = ZnkVarpAry_find_byName_literal( post_vars, "command", false );

	ZnkBird_regist( bird, "EstLM_url_val", "" );
	ZnkBird_regist( bird, "EstLM_comment_val", "" );
	ZnkBird_regist( bird, "EstLM_edit_ui", "" );
	ZnkBird_regist( bird, "this_is_dustbox", "false" );

	if( cmd == NULL ){
		ZnkStr_add( msg, "Est_boxmap_viewer : command is null.\n" );
	} else if( is_authenticated && ZnkS_eq( ZnkVar_cstr(cmd), "view" ) ){
		if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "cache_path", false ) )){
	  		const char* cache_path = ZnkVar_cstr(varp);
			bool this_is_dustbox = ZnkS_isBegin( cache_path, "dustbox" );
			template_html_file = "templates/boxmap_viewer.html";
			ZnkBird_regist( bird, "this_is_dustbox", this_is_dustbox ? "true" : "false" );
			viewCache( bird, evar, post_vars, msg, authentic_key,
					EstCM_img_url_list, ZnkStr_cstr(assort_msg), comment );
		} else {
			template_html_file = "templates/boxmap_manager.html";
			{
				ZnkStr mapping_status = ZnkStr_new( "" );
				char show_file_num_buf[ 256 ] = "10";
				getMappingStatus( mapping_status );
				Znk_snprintf( show_file_num_buf, sizeof(show_file_num_buf), "%zu", show_file_num );
				ZnkBird_regist( bird, "mapping_status", ZnkStr_cstr( mapping_status ) );
				ZnkBird_regist( bird, "show_file_num",  show_file_num_buf );
				ZnkStr_delete( mapping_status );
			}
		}

	} else if( is_authenticated && ZnkS_eq( ZnkVar_cstr(cmd), "remove" ) ){
		bool this_is_dustbox = false;
		varp = ZnkVarpAry_find_byName_literal( post_vars, "this_is_dustbox", false );
		if( varp && ZnkS_eq( ZnkVar_cstr( varp ), "true" ) ){
			this_is_dustbox = true;
		}
		removeCaches( bird,
				evar, post_vars,
				&template_html_file, EstCM_img_url_list,
				backto, assort_msg, msg, authentic_key, this_is_dustbox );

	} else if( is_authenticated && ZnkS_eq( ZnkVar_cstr(cmd), "clean_cachebox" ) ){
		cleanCacheBox( bird,
				evar, post_vars,
				&template_html_file, EstCM_img_url_list,
				backto, assort_msg, msg, authentic_key );

	} else if( is_authenticated && ZnkS_eq( ZnkVar_cstr(cmd), "favoritize" ) ){
		/* この関数内ではadd_tags に応じた処理も内部で行う */
		EstAssortList_favoritize( bird, evar,
				post_vars, backto, assort_msg, msg, authentic_key, "cache", "cache_path" );
		template_html_file = "templates/command_complete.html";

	} else if( is_authenticated && ZnkS_eq( ZnkVar_cstr(cmd), "stock" ) ){
		/* この関数内ではadd_tags に応じた処理も内部で行う */
		EstAssortList_stock( bird, evar,
				post_vars, backto, assort_msg, msg, authentic_key, "cache", "cache_path" );
		template_html_file = "templates/command_complete.html";

	} else if( is_authenticated && ZnkS_eq( ZnkVar_cstr(cmd), "recent" ) ){
		ZnkStr result_view = ZnkStr_new( "" );
		ZnkStrAry dst_vpath_list = ZnkStrAry_create( true );

		size_t processed_count = 0;
		template_html_file = "templates/command_complete.html";
		EstAssort_addPostVars_ifNewTagRegisted( post_vars, assort_msg );

		processed_count = EstBox_recentize( post_vars, msg );

		ZnkStr_addf( result_view, "%zu 件Recentizeしました.\n", processed_count );
		/* 一括ストックなら、Box画面にbacktoする「戻る」を表示すべき */
		/* img_viewからのストックなら、stockboxの最終へのリンクを表示すべき. */
		if( ZnkStr_leng(backto) ){
			char bkt_vpath[ 256 ] = "";
			unsigned int bkt_begin_idx = 0;
			unsigned int bkt_end_idx   = 0;
			sscanf( ZnkStr_cstr(backto), "%u,%u,%s", &bkt_begin_idx, &bkt_end_idx, bkt_vpath );
			ZnkStr_addf( result_view, "<br><a class=%s href=\"/easter?est_manager=cache&command=view&cache_path=%s&est_cache_begin=%u&est_cache_end=%u&Moai_AuthenticKey=%s#PageSwitcher\">戻る</a>",
					style_class_name,
					bkt_vpath, bkt_begin_idx, bkt_end_idx, authentic_key );
		} else {
			ZnkStr_addf( result_view, "<br><br><font size=-1>このタブ画面はEasterではもはや使用されないため、不要ならブラウザのボタンでお閉じください.</font>" );
		}

		ZnkBird_regist( bird, "result_view", ZnkStr_cstr(result_view) );
		ZnkBird_regist( bird, "backto", ZnkStr_cstr(backto) );
		ZnkStr_delete( result_view );
		ZnkStrAry_destroy( dst_vpath_list );

	} else if( ZnkS_eq( ZnkVar_cstr(cmd), "bbsop" ) ){
		ZnkVarp var_cache_path = ZnkVarpAry_find_byName_literal( post_vars, "cache_path", false );
		const char* file_path = ZnkVar_cstr( var_cache_path );
		const char* xhr_dmz = EstConfig_XhrDMZ();

		if( ZnkS_isBegin( file_path, "./cachebox/" ) ){
			file_path += 2;
		}
		if( ZnkS_isBegin( file_path, "cachebox/" ) ){
			const char* original_url = Znk_strchr( file_path + Znk_strlen_literal("cachebox/"), '/' );
			if( original_url ){
				/* TODO: file_path, original_url などに対するXSS対策. */
				original_url++;
				template_html_file = "templates/bbsop.html";
				ZnkBird_regist( bird, "xhr_dmz",      xhr_dmz );
				ZnkBird_regist( bird, "file_path",    file_path );
				ZnkBird_regist( bird, "original_url", original_url );
				EstUI_getSummary( bird, file_path );
				{
					const char* p = file_path;
					const char* q = Znk_strrchr( p, '/' );
					char dir_path[ 256 ] = "cachebox";
					if( q ){
						ZnkS_copy( dir_path, sizeof(dir_path), p, q-p );
					}
					ZnkBird_regist( bird, "dir_path", dir_path );
				}
			} else {
				ZnkStr_addf( msg, "bbsop : invalid original_url=[%s].\n", original_url );
			}
		} else {
			ZnkStr_addf( msg, "bbsop : invalid file_path=[%s].\n", file_path );
		}
		ZnkBird_regist( bird, "backto", ZnkStr_cstr(backto) );

	} else if( ZnkS_eq( ZnkVar_cstr( cmd ), "check_exist" ) ){
		const char* xhr_dmz = EstConfig_XhrDMZ();
		ZnkVarp previewing_img_url = ZnkVarpAry_find_byName_literal( post_vars, "previewing_img_url", false );
		if( previewing_img_url ){
			ZnkStr elem_name = ZnkStr_new( ZnkVar_cstr(previewing_img_url) );
			EstBoxDirType box_dir_type = EstBoxDir_e_Unknown;
			size_t box_path_offset = 0;
			const char* p = NULL;


			/***
			 * Security check
			 */
			ZnkHtpURL_negateHtmlTagEffection( elem_name ); /* for XSS */
			ZnkStr_chompC( elem_name, Znk_NPOS, '/' );

			p = ZnkStrEx_strstr( elem_name, xhr_dmz );
			if( p ){
				p += Znk_strlen( xhr_dmz );
				if( ZnkS_isBegin( p, "/cgis/easter/" ) ){
					ZnkStr fsys_path = ZnkStr_new( "" );
					p +=  Znk_strlen_literal( "/cgis/easter/" );
					box_dir_type = EstBoxBase_convertBoxDir_toFSysDir( fsys_path, p, &box_path_offset, msg );
					if( ZnkDir_getType( ZnkStr_cstr(fsys_path) ) == ZnkDirType_e_File ){
						ZnkStr renamed_filename = ZnkStr_new( "" );
						ZnkStr_addf( msg,  "check_exist fsys_path=[%s]\n", ZnkStr_cstr(fsys_path) );
						if( EstAssort_isExist( box_dir_type, ZnkStr_cstr(fsys_path), p, favorite_dir, msg, renamed_filename ) ){
							bool is_target_blank = true;

							ZnkStr_addf( assort_msg, "このファイルは既にお気に入りに存在します.<br>\n" );
							ZnkStr_addf( assort_msg, "<a class=MstyElemLink href=\"/easter?est_manager=img_viewer&amp;command=open&amp;cache_path=favorite/%s&amp;Moai_AuthenticKey=%s\" %s>Open</a>",
									ZnkStr_cstr(renamed_filename), authentic_key,
									is_target_blank ? "target=_blank" : "" );
							ZnkStr_addf( assort_msg, "<span class=MstyAutoLinkOther>favorite/%s</span>\n", ZnkStr_cstr(renamed_filename) );
						} else {

							ZnkStr_addf( assort_msg, "このファイルはまだお気に入りに存在しません.<br>" );
							ZnkStr_addf( assort_msg, "分類された場合のURL上のパスは[favorite/%s]となります.\n", ZnkStr_cstr(renamed_filename) );
						}
						ZnkStr_delete( renamed_filename );
					}
					ZnkStr_delete( fsys_path );
				}
			}
			ZnkStr_delete( elem_name );
		} else {
			ZnkStr_addf( assort_msg, "previewing_img_urlが取得できません." );
		}

		ZnkStr_add( assort_msg, "<br>\n" );
		template_html_file = "templates/boxmap_viewer.html";
		viewCache( bird, evar, post_vars, msg, authentic_key,
				EstCM_img_url_list, ZnkStr_cstr(assort_msg), comment );

	} else {
		ZnkStr_add( msg, "default:others.\n" );
	}

	ZnkBird_regist( bird, "Moai_AuthenticKey", authentic_key );
	ZnkHtpURL_negateHtmlTagEffection( msg ); /* for XSS */
	{
		ZnkSRef old_ptn = { 0 };
		ZnkSRef new_ptn = { 0 };
		ZnkSRef_set_literal( &old_ptn, "\n" );
		ZnkSRef_set_literal( &new_ptn, "<br>\n" );
		ZnkStrEx_replace_BF( msg, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	ZnkBird_regist( bird, "EstCM_img_url_list", ZnkStr_cstr(EstCM_img_url_list) );
	{
		ZnkStr hint_table = EstHint_getHintTable( "boxmap_viewer" );
		if( hint_table ){
			ZnkBird_regist( bird, "hint_table", ZnkStr_cstr(hint_table) );
		} else {
			ZnkBird_regist( bird, "hint_table", "" );
		}
	}
	ZnkBird_regist( bird, "msg",   ZnkStr_cstr(msg) );
	RanoCGIUtil_printTemplateHTML( evar, bird, template_html_file );
	ZnkBird_destroy( bird );
	ZnkStr_delete( EstCM_img_url_list );
	ZnkStr_delete( pst_str );
	ZnkStr_delete( new_linkid );
	ZnkStr_delete( assort_msg );
	ZnkStr_delete( backto );
}

