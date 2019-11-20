#include <Est_assort_list.h>
#include <Est_config.h>
#include <Est_ui.h>
#include <Est_box.h>
#include <Est_base.h>
#include <Est_assort.h>
#include <Est_assort_ui.h>

#include <Znk_missing_libc.h>
#include <Znk_htp_util.h>
#include <Znk_str_ex.h>
#include <stdio.h>

#define IS_OK( val ) (bool)( (val) != NULL )

void
EstAssortList_favoritize( ZnkBird bird,
		RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr backto, ZnkStr assort_msg, ZnkStr msg, const char* authentic_key,
		const char* bkt_manager, const char* bkt_vpath_key )
{
	static const char* style_class_name = "MstyElemLink";
	ZnkStr      result_view     = ZnkStr_new( "" );
	ZnkStr      assort_ui       = ZnkStr_new( "" );
	ZnkStr      tag_editor_ui   = ZnkStr_new( "" );
	ZnkStrAry   dst_vpath_list  = ZnkStrAry_create( true );
	size_t      processed_count = 0;
	const char* favorite_dir    = EstConfig_favorite_dir();
	const size_t show_file_num  = EstConfig_getShowFileNum();
	const char*  comment = "";

	EstAssort_addPostVars_ifNewTagRegisted( post_vars, assort_msg );

	processed_count = EstBox_favoritize( post_vars, msg, favorite_dir, "favorite", dst_vpath_list );
	EstAssortUI_makeTagsView( assort_ui, NULL, "category_all", NULL, comment, false );

	ZnkStr_addf( result_view, "%zu 件お気に入り登録しました.\n", processed_count );
	{
		EstUI_makeAssortedReportView( evar, dst_vpath_list,
				result_view, show_file_num, authentic_key,
				ZnkStr_cstr(assort_msg) );
	}
	/* 一括favoritizeなら、Box画面にbacktoする「戻る」を表示すべき */
	/* img_viewからのストックなら、favoriteをOpen表示すべき. */
	if( ZnkStr_leng(backto) ){
		char bkt_vpath[ 256 ] = "";
		unsigned int bkt_begin_idx = 0;
		unsigned int bkt_end_idx   = 0;
		sscanf( ZnkStr_cstr(backto), "%u,%u,%s", &bkt_begin_idx, &bkt_end_idx, bkt_vpath );
		ZnkStr_addf( result_view,
				"<br><a class=%s href=\"/easter?est_manager=%s&command=view&%s=%s&est_cache_begin=%u&est_cache_end=%u&Moai_AuthenticKey=%s#PageSwitcher\">戻る</a>",
				style_class_name,
				bkt_manager,
				bkt_vpath_key, bkt_vpath,
				bkt_begin_idx, bkt_end_idx, authentic_key );
	} else {
		ZnkStr_addf( result_view, "<br><br><font size=-1>このタブ画面はEasterではもはや使用されないため、不要ならブラウザのボタンでお閉じください.</font>" );
	}
	
	ZnkBird_regist( bird, "assort_ui",     ZnkStr_cstr(assort_ui) );
	ZnkBird_regist( bird, "tag_editor_ui", ZnkStr_cstr(tag_editor_ui) );
	ZnkBird_regist( bird, "result_view",   ZnkStr_cstr(result_view) );
	ZnkBird_regist( bird, "backto",        ZnkStr_cstr(backto) );
	ZnkStr_delete( result_view );
	ZnkStr_delete( assort_ui );
	ZnkStr_delete( tag_editor_ui );
	ZnkStrAry_destroy( dst_vpath_list );
}
void
EstAssortList_stock( ZnkBird bird,
		RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr backto, ZnkStr assort_msg, ZnkStr msg, const char* authentic_key,
		const char* bkt_manager, const char* bkt_vpath_key )
{
	static const char* style_class_name = "MstyElemLink";
	ZnkStr       result_view     = ZnkStr_new( "" );
	ZnkStr       assort_ui       = ZnkStr_new( "" );
	ZnkStr       tag_editor_ui   = ZnkStr_new( "" );
	ZnkStrAry    dst_vpath_list  = ZnkStrAry_create( true );
	size_t       processed_count = 0;
	const char*  stockbox_dir    = EstConfig_stockbox_dir();
	const size_t show_file_num   = EstConfig_getShowFileNum();
	const char*  comment = "";

	EstAssort_addPostVars_ifNewTagRegisted( post_vars, assort_msg );

	processed_count = EstBox_stock( post_vars, msg, stockbox_dir, "stockbox", dst_vpath_list );
	EstAssortUI_makeTagsView( assort_ui, NULL, "category_all", NULL, comment, false );

	ZnkStr_addf( result_view, "%zu 件ストックしました.\n", processed_count );
	{
		EstUI_makeAssortedReportView( evar, dst_vpath_list,
				result_view, show_file_num, authentic_key,
				ZnkStr_cstr(assort_msg) );
	}
	/* 一括ストックなら、Box画面にbacktoする「戻る」を表示すべき */
	/* img_viewからのストックなら、stockboxの最終へのリンクを表示すべき. */
	if( ZnkStr_leng(backto) ){
		char bkt_vpath[ 256 ] = "";
		unsigned int bkt_begin_idx = 0;
		unsigned int bkt_end_idx   = 0;
		sscanf( ZnkStr_cstr(backto), "%u,%u,%s", &bkt_begin_idx, &bkt_end_idx, bkt_vpath );
		ZnkStr_addf( result_view,
				"<br><a class=%s href=\"/easter?est_manager=%s&command=view&%s=%s&est_cache_begin=%u&est_cache_end=%u&Moai_AuthenticKey=%s#PageSwitcher\">戻る</a>",
				style_class_name,
				bkt_manager,
				bkt_vpath_key, bkt_vpath,
				bkt_begin_idx, bkt_end_idx, authentic_key );
	} else {
		ZnkStr_addf( result_view, "<br><br><font size=-1>このタブ画面はEasterではもはや使用されないため、不要ならブラウザのボタンでお閉じください.</font>" );
	}

	ZnkBird_regist( bird, "assort_ui",     ZnkStr_cstr(assort_ui) );
	ZnkBird_regist( bird, "tag_editor_ui", ZnkStr_cstr(tag_editor_ui) );
	ZnkBird_regist( bird, "result_view",   ZnkStr_cstr(result_view) );
	ZnkBird_regist( bird, "backto",        ZnkStr_cstr(backto) );
	ZnkStr_delete( result_view );
	ZnkStr_delete( assort_ui );
	ZnkStr_delete( tag_editor_ui );
	ZnkStrAry_destroy( dst_vpath_list );
}

static bool
downloadExternalURL( ZnkVarpAry post_vars, const char* new_filename,
		const char* ua, RanoModule mod, ZnkStr result_filename, ZnkStr ermsg )
{
	bool   is_result = false;
	bool   is_url   = false;

	is_url = ZnkS_isBegin( new_filename, "http://" ) ||
		ZnkS_isBegin( new_filename, "https://" ) ||
		ZnkS_isBegin( new_filename, "//" );

	/***
	 * http:// または https:// または // 以外は認めない.
	 * 即ちfile:// や ローカルファイルへの相対およびフルパスの指定は認めない.
	 * これらは「添付ファイル」より指定すべきであり、その方がよりセキュリティ的に堅牢である.
	 */
	if( is_url ){
		bool        is_https = false;
		int         status_code = -1;
		char        hostname[ 1024 ] = "";
		ZnkStr      req_urp = ZnkStr_new( "" );
		const char* src     = EstBase_getHostnameAndRequrp_fromEstVal( hostname, sizeof(hostname), req_urp, new_filename, &is_https );
		const char* unesc_req_urp = ZnkStr_cstr( req_urp );
		ZnkVarpAry  cookie  = ZnkVarpAry_create( true );
		const char* parent_proxy  = EstConfig_parent_proxy();

		Znk_UNUSED( src );
		is_result = EstBase_download( hostname, unesc_req_urp, "upload",
				ua, cookie,
				parent_proxy,
				result_filename, ermsg, mod, &status_code, is_https );

		ZnkVarpAry_destroy( cookie );
		ZnkStr_delete( req_urp );
	}

	return is_result;
}
void
EstAssortList_favoritize_from_external( ZnkBird bird,
		RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr backto, ZnkStr assort_msg, ZnkStr msg, const char* authentic_key,
		const char* bkt_manager, const char* bkt_vpath_key )
{
	static const char* style_class_name = "MstyElemLink";
	ZnkStr      result_view     = ZnkStr_new( "" );
	ZnkStr      assort_ui       = ZnkStr_new( "" );
	ZnkStr      tag_editor_ui   = ZnkStr_new( "" );
	ZnkStrAry   dst_vpath_list  = ZnkStrAry_create( true );
	size_t      processed_count = 0;
	const char* favorite_dir    = EstConfig_favorite_dir();
	const size_t show_file_num  = EstConfig_getShowFileNum();
	const char*  comment = "";

	EstAssort_addPostVars_ifNewTagRegisted( post_vars, assort_msg );

	/* URL cache_path */
	{
		ZnkVarp hyper_upload_path = ZnkVarpAry_find_byName_literal( post_vars, "est_hyper_upload_path", false );
		ZnkStr url_cache_path = ZnkStr_new( "" );
		ZnkStr url_cache_path_esc = ZnkStr_new( "" );
		ZnkVarp cache_path = NULL;
		const char*  easter_default_ua = EstConfig_getEasterDefaultUA();
		ZnkStr new_filename = ZnkStr_new( "" );
		ZnkHtpURL_unescape_toStr( new_filename, ZnkVar_cstr(hyper_upload_path), ZnkVar_str_leng(hyper_upload_path) );

		ZnkBird_regist( bird, "est_hyper_upload_path", ZnkStr_cstr(new_filename) );
		if( downloadExternalURL( post_vars,
				ZnkStr_cstr(new_filename), easter_default_ua, NULL, url_cache_path, NULL ) ){

			if( !IS_OK( cache_path = ZnkVarpAry_find_byName_literal( post_vars, "cache_path", false ) )){
				cache_path = ZnkVarp_create( "cache_path", "", 0, ZnkPrim_e_Str, NULL );
			}
			ZnkStrEx_chompFirstStr( url_cache_path, "./", 2 );
			ZnkHtpURL_escape( url_cache_path_esc, (uint8_t*)ZnkStr_cstr(url_cache_path), ZnkStr_leng(url_cache_path) );
			ZnkVar_set_val_Str( cache_path, ZnkStr_cstr(url_cache_path_esc), ZnkStr_leng(url_cache_path_esc) );
			ZnkVarpAry_push_bk( post_vars, cache_path );
			processed_count = EstBox_favoritize( post_vars, msg, favorite_dir, "favorite", dst_vpath_list );
			ZnkStr_addf( result_view, "%zu 件お気に入り登録しました.\n", processed_count );
		}
		//ZnkStr_addf( result_view, "[%s][%s]お気に入り登録よてい.\n", ZnkStr_cstr(new_filename), ZnkStr_cstr(url_cache_path) );
		ZnkStr_delete( url_cache_path );
		ZnkStr_delete( url_cache_path_esc );
	}

	EstAssortUI_makeTagsView( assort_ui, NULL, "category_all", NULL, comment, false );

	{
		EstUI_makeAssortedReportView( evar, dst_vpath_list,
				result_view, show_file_num, authentic_key,
				ZnkStr_cstr(assort_msg) );
	}
	/* 一括favoritizeなら、Box画面にbacktoする「戻る」を表示すべき */
	/* img_viewからのストックなら、favoriteをOpen表示すべき. */
	if( ZnkStr_leng(backto) ){
		char bkt_vpath[ 256 ] = "";
		unsigned int bkt_begin_idx = 0;
		unsigned int bkt_end_idx   = 0;
		sscanf( ZnkStr_cstr(backto), "%u,%u,%s", &bkt_begin_idx, &bkt_end_idx, bkt_vpath );
		ZnkStr_addf( result_view,
				"<br><a class=%s href=\"/easter?est_manager=%s&command=view&%s=%s&est_cache_begin=%u&est_cache_end=%u&Moai_AuthenticKey=%s#PageSwitcher\">戻る</a>",
				style_class_name,
				bkt_manager,
				bkt_vpath_key, bkt_vpath,
				bkt_begin_idx, bkt_end_idx, authentic_key );
	} else {
		ZnkStr_addf( result_view, "<br><br><font size=-1>このタブ画面はEasterではもはや使用されないため、不要ならブラウザのボタンでお閉じください.</font>" );
	}
	
	ZnkBird_regist( bird, "assort_ui",     ZnkStr_cstr(assort_ui) );
	ZnkBird_regist( bird, "tag_editor_ui", ZnkStr_cstr(tag_editor_ui) );
	ZnkBird_regist( bird, "result_view",   ZnkStr_cstr(result_view) );
	ZnkBird_regist( bird, "backto",        ZnkStr_cstr(backto) );
	ZnkStr_delete( result_view );
	ZnkStr_delete( assort_ui );
	ZnkStr_delete( tag_editor_ui );
	ZnkStrAry_destroy( dst_vpath_list );
}
