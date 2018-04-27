#include <Est_assort_list.h>
#include <Est_config.h>
#include <Est_ui.h>
#include <Est_box.h>
#include <Est_base.h>
#include <Est_assort.h>
#include <Est_assort_ui.h>

#include <Znk_missing_libc.h>
#include <stdio.h>

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
