#include <Est_img_viewer.h>
#include <Est_config.h>
#include <Est_ui.h>
#include <Est_ui_base.h>
#include <Est_base.h>
#include <Est_dir_util.h>
#include <Est_assort.h>
#include <Est_assort_ui.h>
#include <Est_tag.h>
#include <Est_finf.h>
#include <Est_hint_manager.h>

#include <Rano_file_info.h>
#include <Rano_html_ui.h>

#include <Znk_htp_util.h>
#include <Znk_dir.h>
#include <Znk_dir_recursive.h>
#include <Znk_missing_libc.h>
#include <Znk_str_ex.h>
#include <Znk_date.h>
#include <Znk_mem_find.h>
#include <Znk_str_fio.h>
#include <Znk_md5.h>

#define IS_OK( val ) (bool)( (val) != NULL )

#define SJIS_HYO "\x95\x5c" /* 表 */

static void
makeTopbarHint( ZnkStr topbar_ui )
{
	ZnkStr_addf( topbar_ui,
			"<font size=-1>"
			"<a href=\"javascript:void(0);\" onclick=\"displayQandA( 'aboutTopbar' );\"><img style=vertical-align:text-bottom src='/cgis/easter/publicbox/icons/question_16.png'></a>"
			"<div id=aboutTopbar></div>"
			"</font>" );
}
static void
makeTopbarUIHelp( ZnkStr topbar_ui )
{
	ZnkStr_addf( topbar_ui,
			"<a class=MstyElemLink href=/moai2.0/easter_collection.html target=_blank>ヘルプ</a>\n"
			"<font size=-1>"
			"<a href=\"javascript:void(0);\" onclick=\"displayQandA( 'aboutTopbar' );\"><img style=vertical-align:text-bottom src='/cgis/easter/publicbox/icons/question_16.png'></a>"
			"<div id=aboutTopbar></div>"
			"</font>" );
}
static void
makeTopbarUIHelp_forLoading( ZnkStr topbar_ui )
{
	ZnkStr_addf( topbar_ui,
			"<span class=MstyElemLink><font color=#888888>ヘルプ</font></span>\n"
			"<font size=-1>"
			"<img style=vertical-align:text-bottom src='/cgis/easter/publicbox/icons/question_16.png'>"
			"<div id=aboutTopbar></div>"
			"</font>" );
}

static void
makeTopbarUI_forCache( ZnkStr topbar_ui,
		bool mode_assort, const char* vpath, const char* cache_dir_path, const char* authentic_key, bool is_video, bool is_confirm )
{
	const char* content_type_name = is_video ? "動画" : "画像";
	ZnkStr_clear( topbar_ui );
	ZnkStr_addf( topbar_ui,
			"<a class=\"MstyElemLinkRed\" "
			"href=\"javascript:EjsAssort_submitCommandAndArg( document.fm_main, 'img_viewer', 'stock', '' )\">この%sをストック</a>\n", content_type_name );
	ZnkStr_addf( topbar_ui,
			"<a class=\"MstyElemLinkRed\" "
			"href=\"javascript:EjsAssort_submitCommandAndArg( document.fm_main, 'img_viewer', 'favoritize', '' )\">この%sをお気に入りへ</a>\n", content_type_name );
	if( is_confirm ){
		ZnkStr_addf( topbar_ui,
				"<a class=\"MstyElemLinkRed\" "
				"href=\"/easter?est_manager=cache&amp;command=remove&amp;cache_path=%s&amp;Moai_AuthenticKey=%s&amp;confirm=on\">"
				"このファイルを削除</a>\n",
				vpath,
				authentic_key );
	} else {
		ZnkStr_addf( topbar_ui,
				"<a class=\"MstyElemLinkRed\" "
				"href=\"/easter?est_manager=cache&amp;command=remove&amp;cache_path=%s&amp;Moai_AuthenticKey=%s&amp;confirm=off\">"
				"キャッシュから即消去</a>\n",
				vpath,
				authentic_key );
	}
	makeTopbarUIHelp( topbar_ui );
}
void
EstImgViewer_makeTopbarUI_forLoading( ZnkStr topbar_ui, bool is_video )
{
	const char* content_type_name = is_video ? "動画" : "画像";
	ZnkStr_addf( topbar_ui,
			"<span class=\"MstyElemLink\"><font color=#888888>この%sをストック</font></span>\n", content_type_name );
	ZnkStr_addf( topbar_ui,
			"<span class=\"MstyElemLink\"><font color=#888888>この%sをお気に入りへ</font></span>\n", content_type_name );
	ZnkStr_addf( topbar_ui,
			"<span class=\"MstyElemLink\"><font color=#888888>キャッシュから即消去</font></span>\n", content_type_name );
	makeTopbarUIHelp_forLoading( topbar_ui );
}
void
EstImgViewer_makeNowLoading( ZnkStr ans, RanoTextType txt_type, const char* result_filename )
{
	const char* xhr_auth_host = EstConfig_XhrAuthHost();
	switch( txt_type ){
	case RanoText_Image:
		ZnkStr_addf( ans, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
		ZnkStr_addf( ans, "<html>\n" );
		ZnkStr_addf( ans, "<head>\n" );
		ZnkStr_addf( ans, "<META HTTP-EQUIV=\"refresh\" content=\"0;URL=http://%s/easter?est_manager=img_viewer&amp;cache_path=%s\">\n",
				xhr_auth_host, result_filename );
		ZnkStr_addf( ans, "<link href=\"http://%s/msty.css\" rel=\"stylesheet\" type=\"text/css\" />\n", xhr_auth_host );
		ZnkStr_addf( ans, "</head>\n" );
		ZnkStr_addf( ans, "<body>\n" );
	
		ZnkStr_addf( ans, "<font size=\"-1\" color=\"#808000\">Easter Image Viewer</font> <br>" );

		EstImgViewer_makeTopbarUI_forLoading( ans, false );
		
		ZnkStr_addf( ans, "<br>\n" );
		ZnkStr_addf( ans, "<img src=\"/cgis/easter/%s\"><br>\n", result_filename );
		break;
	case RanoText_Video:
		ZnkStr_addf( ans, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
		ZnkStr_addf( ans, "<html>\n" );
		ZnkStr_addf( ans, "<head>\n" );
		ZnkStr_addf( ans, "<META HTTP-EQUIV=\"refresh\" content=\"0;URL=http://%s/easter?est_manager=video_viewer&amp;cache_path=%s\">\n",
				xhr_auth_host, result_filename );
		ZnkStr_addf( ans, "<link href=\"http://%s/msty.css\" rel=\"stylesheet\" type=\"text/css\" />\n", xhr_auth_host );
		ZnkStr_addf( ans, "</head>\n" );
		ZnkStr_addf( ans, "<body>\n" );

		/***
		 * Easterでは動画ファイルもキャッシュされる.
		 * サムネなどを表示すれば良いが現段階ではそこまでせず、単にキャッシュへのリンクを示すだけとする.
		 */
		ZnkStr_addf( ans, "<font size=\"-1\" color=\"#808000\">Easter Video Cache</font> <br>" );
		ZnkStr_addf( ans, "This video is cached at <a class=MstyElemLink href=\"/cgis/easter/%s\">/cgis/easter/%s</a> by Easter.<br>\n",
				result_filename, result_filename );

		ZnkStr_addf( ans, "<span class=\"MstyElemLink\"><font color=#888888>この動画をCacheから削除</font></span>\n" );
		{
			const char* p = result_filename;
			const char* q = Znk_strrchr( p, '/' );
			if( q ){
				char dir_path[ 256 ] = "";
				ZnkS_copy( dir_path, sizeof(dir_path), p, q-p );
				ZnkStr_addf( ans, "<span class=\"MstyElemLink\"><font color=#888888>動画が格納されたキャッシュディレクトリを開く</font></span>\n" );
			}
		}
		break;
	default:
		break;
	}
}




static void
makeTopbarUI_forFavorite( ZnkStr topbar_ui, bool mode_assort, const char* vpath, const char* cache_dir_path, const char* authentic_key, bool is_video, bool is_confirm )
{
	ZnkStr_clear( topbar_ui );
	ZnkStr_addf( topbar_ui,
			"<span class=MstyAutoLinkOther>Opening : %s</span>", vpath );
	ZnkStr_addf( topbar_ui,
			"<a class=\"MstyElemLink\" "
			"href=\"/easter?est_manager=img_viewer&amp;command=open&amp;vpath=%s&amp;Moai_AuthenticKey=%s\">"
			"Reopen</a>\n",
			vpath,
			authentic_key );
	ZnkStr_addf( topbar_ui,
			"<a class=\"MstyElemLink\" "
			"href=\"/easter?est_manager=topic&amp;Moai_AuthenticKey=%s\" target=_blank>"
			"コレクション</a>\n",
			authentic_key );
	makeTopbarHint( topbar_ui );
}
static void
makeTopbarUI_forStockbox( ZnkStr topbar_ui, bool mode_assort, const char* vpath, const char* cache_dir_path, const char* authentic_key, bool is_video, bool is_confirm )
{
	ZnkStr_clear( topbar_ui );
	ZnkStr_addf( topbar_ui,
			"<span class=MstyAutoLinkOther>Opening : %s</span>", vpath );
	ZnkStr_addf( topbar_ui,
			"<a class=\"MstyElemLink\" "
			"href=\"/easter?est_manager=img_viewer&amp;command=open&amp;vpath=%s&amp;Moai_AuthenticKey=%s\">"
			"Reopen</a>\n",
			vpath,
			authentic_key );
	ZnkStr_addf( topbar_ui,
			"<a class=\"MstyElemLink\" "
			"href=\"/easter?est_manager=topic&amp;Moai_AuthenticKey=%s\" target=_blank>"
			"コレクション</a>\n",
			authentic_key );
	makeTopbarHint( topbar_ui );
}

static bool
loadEnableTagList( ZnkStrAry tags_list, const char* dir_path, const char* file_path )
{
	bool result = false;
	char info_list_path[ 256 ] = "";
	ZnkMyf myf = ZnkMyf_create();

	Znk_snprintf( info_list_path, sizeof(info_list_path), "%s/info_list.myf", dir_path );
	if( !ZnkMyf_load( myf, info_list_path ) ){
		goto FUNC_END;
	} else {
		const char* p = Znk_strrchr( file_path, '/' );
		if( p ){
			const char* filename = p + 1;
			if( !ZnkS_empty( filename ) ){
				ZnkVarpAry vars = ZnkMyf_find_vars( myf, filename );
				if( vars ){
					ZnkVarp    var  = ZnkVarpAry_find_byName_literal( vars, "file_tags", false );
					if( var ){
						const char* file_tags = ZnkVar_cstr( var );
						ZnkStrAry_clear( tags_list );
						ZnkStrEx_addSplitC( tags_list,
								file_tags, Znk_NPOS,
								' ', false, 4 );
						result = true;
					}
				}
			}
		}
	}
FUNC_END:
	ZnkMyf_destroy( myf );
	return result;
}

typedef enum {
	 ExistMsgType_e_AlreadyExist=0
	,ExistMsgType_e_NewFavoritize
	,ExistMsgType_e_ReFavoritize
	,ExistMsgType_e_NewStock
	,ExistMsgType_e_ReStock
} ExistMsgType;

static bool
checkExistMsg( ZnkStr exist_msg, ZnkStr play_msg, ZnkStr comment,
		EstBoxDirType ast_dir_type, const char* fsys_path, const char* vpath, const char* box_fsys_dir,
		ZnkStr msg, ZnkStr renamed_filename, bool is_video, const char* authentic_key, const char* xhr_dmz,
		const char* box_vname, ExistMsgType exist_msg_type )
{
	bool is_exist_vbox = false;
	if( EstAssort_getEssential( comment, ast_dir_type, fsys_path, vpath, box_fsys_dir, msg, renamed_filename ) ){
		bool is_target_blank = true;
		is_exist_vbox = true;

		switch( exist_msg_type ){
		case ExistMsgType_e_NewFavoritize:
			ZnkStr_add( exist_msg, "このファイルはお気に入りに登録されました(tagを追加).<br>\n" );
			break;
		case ExistMsgType_e_ReFavoritize:
			ZnkStr_add( exist_msg, "このファイルはお気に入りに登録されました(tagを完全更新).<br>\n" );
			break;
		case ExistMsgType_e_NewStock:
			ZnkStr_add( exist_msg, "このファイルはストックボックスに登録されました(tagを追加).<br>\n" );
			break;
		case ExistMsgType_e_ReStock:
			ZnkStr_add( exist_msg, "このファイルはストックボックスに登録されました(tagを完全更新).<br>\n" );
			break;
		default:
		{
			EstBoxDirType exist_ast_dir_type = EstBoxBase_getDirType( box_vname );
			switch( exist_ast_dir_type ){
			case EstBoxDir_e_Favorite:
				ZnkStr_add( exist_msg, "このファイルは既にお気に入りに存在します.<br>\n" );
				break;
			case EstBoxDir_e_Stockbox:
				ZnkStr_add( exist_msg, "このファイルは既にストックボックスに存在します.<br>\n" );
				break;
			default:
				ZnkStr_addf( exist_msg, "このファイルはBox[%s]に存在します.<br>\n", box_vname );
				break;
			}
			break;
		}
		}

		if( is_video ){
			ZnkStr_addf( exist_msg, "<a class=MstyElemLink href=/easter?est_manager=video_viewer&amp;command=open&amp;cache_path=%s/%s&amp;Moai_AuthenticKey=%s %s>Open</a>",
					box_vname,
					ZnkStr_cstr(renamed_filename), authentic_key,
				is_target_blank ? "target=_blank" : "" );
			ZnkStr_addf( exist_msg, "<span class=MstyAutoLinkOther>/%s/%s</span>\n",
					box_vname,
					ZnkStr_cstr(renamed_filename) );

			ZnkStr_addf( play_msg, "<a class=MstyElemLink href=\"http://%s/cgis/easter/%s/%s\">Play</a>",
					xhr_dmz,
					box_vname,
					ZnkStr_cstr(renamed_filename) );
			ZnkStr_addf( play_msg, "<span class=MstyAutoLinkOther>/cgis/easter/%s/%s</span>\n",
					box_vname,
					ZnkStr_cstr(renamed_filename) );
		}
	}
	return is_exist_vbox;
}

static void
openImg( ZnkStr assort_msg, ZnkStr file_path, const char* fsys_path, const char* xhr_dmz, ZnkStr msg )
{
	ZnkStr_set( file_path, fsys_path );
	ZnkStr_addf( msg, "fsys_path=[%s].\n", fsys_path );
	{
		ZnkFile fp = Znk_fopen( fsys_path, "rb" );
		if( fp ){
			uint8_t buf[ 64 ] = { 0 };
			size_t readed_size = Znk_fread( buf, sizeof(buf), fp );
			bool   is_html = false;
			const char* ptn = "";
			const char* type_name = "";

			ZnkStr dump = ZnkStr_new( "" );
			ZnkStr_addf( msg, "readed_size=[%zu].\n", readed_size );

			/* 検査用(改行を含めない) */
			EstBase_safeDumpBuf( dump, buf, readed_size, 0 );
			ZnkHtpURL_negateHtmlTagEffection( dump ); /* for XSS */

			if( ZnkStrEx_strstr( dump, "HTML" ) || ZnkStrEx_strstr( dump, "html" ) ){
				ptn       = "HTML";
				type_name = "HTML";
				is_html   = true;
			} else if( ZnkStrEx_strstr( dump, "JFIF" ) ){
				ptn       = "JFIF";
				type_name = "JPEG";
			} else if( ZnkStrEx_strstr( dump, "ffd8ffe1" ) && ZnkStrEx_strstr( dump, "Exif" ) ){
				ptn       = "Exif";
				type_name = "JPEG";
			} else if( ZnkStrEx_strstr( dump, "89PNG" ) ){
				ptn       = "89PNG";
				type_name = "PNG";
			} else if( ZnkStrEx_strstr( dump, "GIF89" ) ){
				ptn       = "GIF89";
				type_name = "GIF";
			} else if( ZnkStrEx_strstr( dump, "webmB8" ) ){
				ptn       = "webmB8";
				type_name = "WEBM";
			} 

			/* 表示用 */
			ZnkStr_clear( dump );
			EstBase_safeDumpBuf( dump, buf, readed_size, 16 );
			ZnkHtpURL_negateHtmlTagEffection( dump ); /* for XSS */
			ZnkStrEx_replace_BF( dump, 0,
					"\n", 1,
					"<br>", 4,
					Znk_NPOS, Znk_NPOS );

			ZnkStr_add( assort_msg, "<div class=MstyElemLinkOther>" );
			ZnkStr_add( assort_msg, "<div style=text-align:left>" );

			ZnkStr_add( assort_msg, "<font size=-1>" );
			ZnkStr_add( assort_msg, "<u><b>結果:</b></u><br>" );

			if( !ZnkS_empty(type_name) ){
				ZnkStr_addf( assort_msg,
						"%sファイルであると思われます.\n", type_name );
				if( is_html && ZnkS_isBegin( fsys_path, "cachebox/" ) ){
					const char* p = fsys_path + Znk_strlen_literal("cachebox/");
					const char* q = Znk_strchr( p, '/' );
					if( q ){
						ZnkStr_addf( assort_msg,
								"<br><a class=MstyElemLink href=http://%s/easter?est_reload=%s&amp;est_content_type=html >[HTMLとして開く]</a>\n",
								xhr_dmz, q+1 );
					}
				}
			} else {
				ZnkStr_addf( assort_msg,
						"Easter Img Viewerはこのファイルの種別を判別できませんでした.\n" );
			}
			ZnkStr_add( assort_msg, "<br><br>" );

			ZnkStr_add( assort_msg, "<u><b>根拠:</b></u><br>" );
			if( ZnkS_empty( ptn ) ){
				ZnkStr_addf( assort_msg,
						"このファイルの最初の64byteを検証しました.<br>"
						"Easterで認識できる既知のパターンが含まれていないため、判定ができませんでした." );
			} else {
				ZnkStr_addf( assort_msg,
						"このファイルの最初の64byteを検証しました.<br>"
						"%sファイル固有のパターンが含まれていることより判定しました.",
						type_name );
			}
			ZnkStr_add( assort_msg, "<br><br>" );

			ZnkStr_add( assort_msg, "<u><b>ダンプ" SJIS_HYO "示:</b></u><br>" );
			ZnkStr_addf( assort_msg, "以下にこのファイルの最初の64byteを示します.<br>" );
			ZnkStr_add( assort_msg, "<ul class=GeneralCode>" );
			ZnkStr_add( assort_msg, ZnkStr_cstr(dump) );
			ZnkStr_add( assort_msg, "</ul>" );
			ZnkStr_addf( assort_msg, "尚、このダンプ" SJIS_HYO "示は如何なるバイト列が含まれようとも安全なものとなるよう<br>" );
			ZnkStr_addf( assort_msg, "処理済みのものを" SJIS_HYO "示しています.<br>" );

			ZnkStr_add( assort_msg, "</font>" );
			ZnkStr_add( assort_msg, "</div>" );
			ZnkStr_add( assort_msg, "</div>" );

			ZnkStr_delete( dump );
			Znk_fclose( fp );
		}
	}
}

static bool
updateExistMsg( ZnkStr msg, ZnkStr exist_msg, ZnkStr play_msg, ZnkStr comment,
		ZnkStr existed_filename,
		EstBoxDirType box_dir_type, EstBoxDirType* existed_box_dir_type,
		const char* fsys_path, const char* vpath,
		bool is_video, ExistMsgType exist_msg_type, const char* authentic_key )
{
	bool is_exist_vbox = false;
	const char* favorite_dir = EstConfig_favorite_dir();
	const char* stockbox_dir = EstConfig_stockbox_dir();
	const char* xhr_dmz = EstConfig_XhrDMZ();

	ZnkStr_addf( msg,  "check_exist fsys_path=[%s]\n", fsys_path );
	if( checkExistMsg( exist_msg, play_msg, comment, box_dir_type, fsys_path, vpath,
				favorite_dir, msg, existed_filename, is_video, authentic_key, xhr_dmz, "favorite", exist_msg_type ) )
	{
		is_exist_vbox = true;
		if( existed_box_dir_type ){
			*existed_box_dir_type = EstBoxDir_e_Favorite;
		}
	}
	else if( checkExistMsg( exist_msg, play_msg, comment, box_dir_type, fsys_path, vpath,
				stockbox_dir, msg, existed_filename, is_video, authentic_key, xhr_dmz, "stockbox", exist_msg_type ) )
	{
		is_exist_vbox = true;
		if( existed_box_dir_type ){
			*existed_box_dir_type = EstBoxDir_e_Stockbox;
		}
	}
	else {
		if( is_video ){
			ZnkStr_addf( play_msg, "<a class=MstyElemLink href=\"http://%s/cgis/easter/%s\">Play</a><br>",
					xhr_dmz,
					vpath );
			ZnkStr_addf( play_msg, "<span class=MstyAutoLinkOther>/cgis/easter/%s</span>\n", vpath );
		}
		ZnkStr_addf( exist_msg, "このファイルはまだお気に入りに存在しません.<br>" );
		ZnkStr_addf( exist_msg, "分類された場合のURL上のパスは[favorite/%s]となります.\n", ZnkStr_cstr(existed_filename) );
	}
	return is_exist_vbox;
}

static size_t
assortToBox( ZnkVarpAry post_vars, const char* src_fsys_path,
		const char* dst_box_fsys_dir, const char* dst_box_vname,
		ZnkStr msg, ZnkStr tag_editor_msg, ZnkStr assort_msg, const char* action_name,
		EstBoxDirType dst_box_dir_type, EstBoxDirType src_box_dir_type,
		ZnkStr dst_fsys_path, ZnkStr vpath, ZnkStr renamed_filename, ZnkStr comment,
		bool add_tags_if_exist, EstBoxDirType existed_ast_dir_type )
{
	size_t      processed_count = 0;
	char        finf_list_path[ 256 ] = "";
	ZnkVarpAry  finf_list = EstFInfList_create();
	ZnkVarpAry  sqi_vars  = ZnkVarpAry_create( true );
	ZnkStr file_tags = ZnkStr_new( "" );
	const bool existed_in_box = (bool)( existed_ast_dir_type == EstBoxDir_e_Favorite || existed_ast_dir_type == EstBoxDir_e_Stockbox );

	ZnkStr_addf( msg,  "src_fsys_path=[%s]\n", src_fsys_path );
	EstAssort_addPostVars_ifNewTagRegisted( post_vars, tag_editor_msg );
	EstTag_joinTagIDs( file_tags, post_vars, ' ' );
	{
		ZnkVarp varp = ZnkVarpAry_findObj_byName_literal( post_vars, "ast_comment", false );
		if( varp ){
			ZnkStr_clear( comment );
			ZnkHtpURL_unescape_toStr( comment, ZnkVar_cstr(varp), ZnkVar_str_leng(varp) );
			ZnkStr_addf( msg,  "ast_comment==[%s]\n", ZnkStr_cstr(comment) );
		}
	}

	Znk_snprintf( finf_list_path, sizeof(finf_list_path), "%s/finf_list.myf", dst_box_fsys_dir );
	EstFInfList_load( finf_list, sqi_vars, finf_list_path );

	if( add_tags_if_exist && existed_in_box ){
		/* finf_list.myfを更新 */
		EstFInfList_registFileByMD5( finf_list, dst_box_vname, src_fsys_path, ZnkStr_cstr(file_tags), ZnkStr_cstr(comment), add_tags_if_exist );

		/* info_list.myfを更新 */
		processed_count = EstAssort_addTags( ZnkStr_cstr(file_tags), ZnkStr_cstr(comment),
				src_fsys_path, ZnkStr_cstr(vpath),
				dst_box_fsys_dir, dst_box_vname, msg, renamed_filename );

		ZnkStr_setf( dst_fsys_path, "%s/%s", dst_box_fsys_dir, ZnkStr_cstr(renamed_filename) );
		ZnkStr_setf( vpath,         "%s/%s", dst_box_vname,    ZnkStr_cstr(renamed_filename) );
		ZnkStr_addf( assort_msg, "%sしました(タグを%zu個追加).\n", action_name, processed_count );
	} else {
		/* finf_list.myfを更新 */
		if( existed_in_box ){
			EstFInfList_registFileByMD5( finf_list, dst_box_vname, src_fsys_path, ZnkStr_cstr(file_tags), ZnkStr_cstr(comment), add_tags_if_exist );
		} else {
			EstFInfList_addFileByMD5( finf_list, dst_box_vname, src_fsys_path, ZnkStr_cstr(file_tags), ZnkStr_cstr(comment) );
		}

		/* info_list.myfを更新 */
		processed_count = EstAssort_doOneNewly( ZnkStr_cstr(file_tags), ZnkStr_cstr(comment), add_tags_if_exist,
				src_fsys_path, ZnkStr_cstr(vpath),
				dst_box_fsys_dir, dst_box_vname, msg, renamed_filename );

		ZnkStr_setf( dst_fsys_path, "%s/%s", dst_box_fsys_dir, ZnkStr_cstr(renamed_filename) );
		ZnkStr_setf( vpath,         "%s/%s", dst_box_vname,    ZnkStr_cstr(renamed_filename) );
		ZnkStr_addf( assort_msg, "%zu 件%sしました.\n", processed_count, action_name );
	}

	ZnkStr_addf( msg,  "processed_count==[%zu]\n", processed_count );

	if( processed_count && src_box_dir_type != dst_box_dir_type ){
		EstAssort_remove( src_box_dir_type, src_fsys_path, msg );
	}
	if( processed_count ){
		bool is_modesty = false;
		EstSQI sqi = EstSQI_create( sqi_vars );
		EstFInfList_save( finf_list, sqi, finf_list_path, add_tags_if_exist, is_modesty );
		EstSQI_destroy( sqi );
	}
	EstFInfList_destroy( finf_list );
	ZnkVarpAry_destroy( sqi_vars );
	ZnkStr_delete( file_tags );

	return processed_count;
}

void
EstImgViewer_main( RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr msg, const char* authentic_key, bool is_video )
{
	ZnkBird bird = ZnkBird_create( "#[", "]#" );
	ZnkVarp var_cache_path = NULL;
	ZnkVarp command = NULL;
	ZnkStr cache_path = ZnkStr_new( "" );
	ZnkStr cache_path_unesc = ZnkStr_new( "" );
	ZnkStr file_path = ZnkStr_new( "" );
	ZnkStr fsys_path = ZnkStr_new( "" );
	ZnkStr renamed_filename = ZnkStr_new( "" );
	ZnkStr existed_filename = ZnkStr_new( "" );
	const char* xhr_dmz = EstConfig_XhrDMZ();
	ZnkStr topbar_ui = ZnkStr_new( "" );
	ZnkStr tags_view = ZnkStr_new( "" );
	ZnkStr assort_ui = ZnkStr_new( "" );
	ZnkStr tag_editor_msg = ZnkStr_new( "" );
	ZnkStr assort_msg = ZnkStr_new( "" );
	ZnkStr exist_msg  = ZnkStr_new( "" );
	ZnkStr play_msg   = ZnkStr_new( "" );
	ZnkStr comment    = ZnkStr_new( "" );
	ZnkStr security_verifier = ZnkStr_new( "このコンテンツに関してはまだ検査を行っていません." );
	char dir_path[ 256 ] = "cachebox";
	char cache_dir_path[ 256 ] = "cachebox";
	const char* template_html_file = is_video ? "templates/video_viewer.html" :"templates/img_viewer.html";
	bool is_exist_vbox = false;

	struct ZnkHtpHdrs_tag htp_hdrs = { 0 };
	RanoModule mod = NULL;
	ZnkStr     EstCM_img_url_list = ZnkStr_new( "" );
	ZnkStr     pst_str    = ZnkStr_new( "" );
	bool   is_authenticated = false;
	bool   is_unescape_val = false;
	ZnkVarp varp;
	const char* favorite_dir = EstConfig_favorite_dir();
	const char* stockbox_dir = EstConfig_stockbox_dir();
	EstBoxDirType ast_dir_type         = EstBoxDir_e_Unknown;
	EstBoxDirType existed_ast_dir_type = EstBoxDir_e_Unknown;
	size_t box_path_offset = 0;
	ZnkStr backto = ZnkStr_new( "" );
	ZnkStr category_select_bar = ZnkStr_new( "" );
	ZnkStr current_category_name = ZnkStr_new( "すべて" );
	ZnkVarp backto_varp = NULL;
	ExistMsgType exist_msg_type = ExistMsgType_e_AlreadyExist;
	const char* current_category_id = "category_all";

	RanoCGIUtil_getPostedFormData( evar, post_vars, mod, &htp_hdrs, pst_str, NULL, is_unescape_val );
	command = ZnkVarpAry_find_byName_literal( post_vars, "command", false );

	if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "Moai_AuthenticKey", false ) )
	  && ZnkS_eq( authentic_key, ZnkVar_cstr(varp) ) ){
		is_authenticated = true;
	} else {
	}
	ZnkStr_addf( msg, "is_authenticated=[%d]\n", is_authenticated );
	ZnkStr_addf( msg, "pst_str=[%s]\n", ZnkStr_cstr(pst_str) );

	var_cache_path = ZnkVarpAry_find_byName_literal( post_vars, "vpath", false );
	if( var_cache_path == NULL ){
		var_cache_path = ZnkVarpAry_find_byName_literal( post_vars, "cache_path", false );
		if( var_cache_path == NULL ){
			ZnkStr_addf( msg, "cache_path does not exist.\n" );
			goto FUNC_END;
		}
	}

	ZnkStr_set( cache_path_unesc, ZnkVar_cstr( var_cache_path ) );
	ZnkStr_set( cache_path,       ZnkVar_cstr( var_cache_path ) );
	EstBase_unescape_forMbcFSysPath( cache_path_unesc );
	EstBase_escape_forURL( cache_path, 1 );

	ast_dir_type = EstBoxBase_convertBoxDir_toFSysDir( fsys_path, ZnkStr_cstr(cache_path), &box_path_offset, msg );
	ZnkStr_copy( file_path, cache_path );

	if( ZnkS_eqCase( ZnkS_get_extension(ZnkStr_cstr(cache_path),'.'), "webm" ) ){
		is_video = true;
		template_html_file = "templates/video_viewer.html";
	}

	if( IS_OK( backto_varp = ZnkVarpAry_find_byName_literal( post_vars, "backto", false ) )){
		ZnkHtpURL_unescape_toStr( backto, ZnkVar_cstr(backto_varp), ZnkVar_str_leng(backto_varp) );
	}

	/* check_exist: 常に実行. */
	is_exist_vbox = updateExistMsg( msg, exist_msg, play_msg, comment,
			existed_filename,
			ast_dir_type, &existed_ast_dir_type,
			ZnkStr_cstr(fsys_path), ZnkStr_cstr(cache_path),
			is_video, exist_msg_type, authentic_key );

	if( command ){
		ZnkVarp varp;
		ZnkStr_addf( msg, "command=[%s]\n", ZnkVar_cstr(command) );

		if( ZnkS_eq( ZnkVar_cstr( command ), "favoritize" ) ){
			bool add_tags = true;
			if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "add_tags", false ) )){
				add_tags = ZnkS_eq( ZnkVar_cstr( varp ), "true" );
			}
			assortToBox( post_vars, ZnkStr_cstr(fsys_path),
					favorite_dir, "favorite",
					msg, tag_editor_msg, assort_msg, "お気に入り登録",
					EstBoxDir_e_Favorite, ast_dir_type,
					file_path, cache_path, renamed_filename, comment, add_tags, existed_ast_dir_type );

			/* 再実行 */
			ZnkStr_clear( exist_msg );
			ZnkStr_clear( play_msg );
			ZnkStr_clear( existed_filename );
			is_exist_vbox = updateExistMsg( msg, exist_msg, play_msg, comment,
					existed_filename,
					ast_dir_type, &existed_ast_dir_type,
					ZnkStr_cstr(file_path), ZnkStr_cstr(cache_path),
					is_video, add_tags ? ExistMsgType_e_NewFavoritize : ExistMsgType_e_ReFavoritize, authentic_key );

		} else if( ZnkS_eq( ZnkVar_cstr( command ), "stock" ) ){
			bool add_tags = true;
			if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "add_tags", false ) )){
				add_tags = ZnkS_eq( ZnkVar_cstr( varp ), "true" );
			}
			assortToBox( post_vars, ZnkStr_cstr(fsys_path),
					stockbox_dir, "stockbox",
					msg, tag_editor_msg, assort_msg, "ストック",
					EstBoxDir_e_Stockbox, ast_dir_type,
					file_path, cache_path, renamed_filename, comment, add_tags, existed_ast_dir_type );

			/* 再実行 */
			ZnkStr_clear( exist_msg );
			ZnkStr_clear( play_msg );
			ZnkStr_clear( existed_filename );
			is_exist_vbox = updateExistMsg( msg, exist_msg, play_msg, comment,
					existed_filename,
					ast_dir_type, &existed_ast_dir_type,
					ZnkStr_cstr(file_path), ZnkStr_cstr(cache_path),
					is_video, add_tags ? ExistMsgType_e_NewStock : ExistMsgType_e_ReStock, authentic_key );

		} else if( ZnkS_eq( ZnkVar_cstr( command ), "check_exist" ) ){
			ZnkStr_addf( msg,  "check_exist fsys_path=[%s]\n", ZnkStr_cstr(fsys_path) );
			if( EstAssort_isExist( ast_dir_type, ZnkStr_cstr(fsys_path), ZnkStr_cstr(cache_path), favorite_dir, msg, renamed_filename ) ){
				bool is_target_blank = true;
				//is_exist_favorite = true;
				is_exist_vbox = true;

				ZnkStr_addf( assort_msg, "このファイルは既にお気に入りに存在します.<br>\n" );
				if( is_video ){
					ZnkStr_addf( assort_msg, "<a class=MstyElemLink href=/easter?est_manager=video_viewer&amp;command=open&amp;cache_path=favorite/%s&amp;Moai_AuthenticKey=%s %s>Open</a>",
							ZnkStr_cstr(renamed_filename), authentic_key,
							is_target_blank ? "target=_blank" : "" );
				} else {
					ZnkStr_addf( assort_msg, "<a class=MstyElemLink href=/easter?est_manager=img_viewer&amp;command=open&amp;cache_path=favorite/%s&amp;Moai_AuthenticKey=%s %s>Open</a>",
							ZnkStr_cstr(renamed_filename), authentic_key,
							is_target_blank ? "target=_blank" : "" );
				}
				ZnkStr_addf( assort_msg, "<span class=MstyAutoLinkOther>favorite/%s</span>\n", ZnkStr_cstr(renamed_filename) );
			} else {

				ZnkStr_addf( assort_msg, "このファイルはまだお気に入りに存在しません.<br>" );
				ZnkStr_addf( assort_msg, "分類された場合のURL上のパスは[favorite/%s]となります.\n", ZnkStr_cstr(renamed_filename) );
			}

		} else if( is_authenticated && ZnkS_eq( ZnkVar_cstr(command), "remove" ) ){
			/***
			 * TODO:
			 * 以下を削除します.
			 * 本当によろしいですか？
			 * 画面を出し、「削除」ボタンを押すとカレントのEstCacheManager画面に戻る.
			 */
			ZnkVarp confirm;
			ZnkStr  result_view = ZnkStr_new( "" );
			size_t  processed_count = 0;
			bool    is_confirm = false;
			const char* alternate_dir = "unknown";
			const size_t show_file_num = EstConfig_getShowFileNum();

			if( IS_OK( confirm = ZnkVarpAry_find_byName_literal( post_vars, "confirm", false ) )){
				if( ZnkS_eq( ZnkVar_cstr(confirm), "on" ) ){
					is_confirm = true;
				}
			}

			if( is_confirm ){
				template_html_file = "templates/remove_confirm.html";
				EstUI_makeCheckedConfirmView( evar, post_vars,
						result_view, show_file_num, authentic_key,
						ZnkStr_cstr(assort_msg) );
			} else {
				switch( ast_dir_type ){
				case EstBoxDir_e_Favorite:
					alternate_dir = "favorite";
					break;
				case EstBoxDir_e_Cachebox:
					alternate_dir = "cachebox";
					break;
				case EstBoxDir_e_Dustbox:
					alternate_dir = "dustbox";
					break;
				case EstBoxDir_e_Stockbox:
					alternate_dir = "stockbox";
					break;
				case EstBoxDir_e_Userbox:
					alternate_dir = "userbox";
					break;
				default:
					alternate_dir = "unknown";
					break;
				}
				ZnkStr_copy( file_path, fsys_path );
				{
					const char* box_fsys_dir = EstBoxBase_getBoxFsysDir( ast_dir_type );
					processed_count = EstAssort_moveSubdirFile( box_fsys_dir, ZnkStr_cstr(fsys_path), "dustbox", alternate_dir, "img_viewer", msg );
					if( processed_count ){
						EstAssort_remove( ast_dir_type, ZnkStr_cstr(fsys_path), msg );
						ZnkStr_addf( tag_editor_msg, "ファイル[%s] を削除しました.\n", ZnkStr_cstr(fsys_path) );
					} else {
						ZnkStr_addf( tag_editor_msg, "ファイル[%s] の削除に失敗しました.\n", ZnkStr_cstr(fsys_path) );
					}
				}
			}
			if( is_confirm ){
				/* C4819を回避するため、念のためShiftJIS部とASCII部を分けて文字列リテラルを記述してある部分がある. */
				ZnkStr_addf( result_view, "削除しますがよろしいですか？" "<br>\n" );
				ZnkStr_addf( result_view, "<a class=MstyElemLink href=\"javascript:EjsAssort_submitCommandAndArg( document.fm_main, 'cache', 'remove', '' )\">" );
				ZnkStr_addf( result_view, "はい、チェックの入ったものを一括削除します.</a><br>\n" );

				ZnkStr_addf( result_view, "<a class=MstyElemLink href=\"javascript:EjsAssort_submitCommandAndArg( document.fm_main, 'img_viewer', 'open', '' )\">" );
				ZnkStr_addf( result_view, "いいえ、何もせず元に戻ります.</a><br>\n" );
			} else {
				ZnkStr_addf( result_view, "%zu 件削除しました.\n", processed_count );
			}

			ZnkBird_regist( bird, "manager", "img_view" );
			ZnkBird_regist( bird, "result_view",   ZnkStr_cstr(result_view) );
			ZnkStr_delete( result_view );

		} else if( ZnkS_eq( ZnkVar_cstr( command ), "open" ) ){
			ZnkStr_clear( security_verifier );
			openImg( security_verifier, file_path, ZnkStr_cstr(fsys_path), xhr_dmz, msg );
			ZnkStr_addf( msg, "comment(open)=[%s].\n", ZnkStr_cstr(comment) );
		} else {
			ZnkStr_addf( msg, "Invalid command=[%s].\n", ZnkVar_cstr(command) );
		}
	} else {
		if( is_exist_vbox ){
			ZnkStr_clear( security_verifier );
			openImg( security_verifier, file_path, ZnkStr_cstr(fsys_path), xhr_dmz, msg );
		} else {
			ZnkStr_addf( msg, "command does not exist.\n" );
		}
	}

	ZnkStr_addf( msg, "file_path=[%s]\n", ZnkStr_cstr(file_path) );

	{
		EstBoxDirType dir_type = is_exist_vbox ? existed_ast_dir_type : ast_dir_type;
		ZnkStr vpath = ZnkStr_new( "" );
		ZnkStrAry tags_list = ZnkStrAry_create( true );
		const char* content_type_name = is_video ? "動画" : "画像";

		switch( dir_type ){
		case EstBoxDir_e_Favorite:
			ZnkStr_setf( vpath, "favorite/%s", ZnkStr_cstr(existed_filename) );
			ZnkStr_setf( file_path, "%s/%s", favorite_dir, ZnkStr_cstr(existed_filename) );
			makeTopbarUI_forFavorite( topbar_ui, true, ZnkStr_cstr(vpath), cache_dir_path, authentic_key, is_video, true );
			if( ast_dir_type != EstBoxDir_e_Favorite ){
				ZnkStr_addf( topbar_ui, "%s", ZnkStr_cstr(exist_msg) );
			}
			break;
		case EstBoxDir_e_Stockbox:
			ZnkStr_setf( vpath, "stockbox/%s", ZnkStr_cstr(existed_filename) );
			ZnkStr_setf( file_path, "%s/%s", stockbox_dir, ZnkStr_cstr(existed_filename) );
			makeTopbarUI_forStockbox( topbar_ui, true, ZnkStr_cstr(vpath), cache_dir_path, authentic_key, is_video, true );
			if( ast_dir_type != EstBoxDir_e_Stockbox ){
				ZnkStr_addf( topbar_ui, "%s", ZnkStr_cstr(exist_msg) );
			}
			break;
		case EstBoxDir_e_Userbox:
		{
			const bool is_confirm = true;
			makeTopbarUI_forCache( topbar_ui, false, ZnkStr_cstr(cache_path), cache_dir_path, authentic_key, is_video, is_confirm );
			if( is_exist_vbox ){
				ZnkStr_addf( topbar_ui, "%s", ZnkStr_cstr(exist_msg) );
			}
			break;
		}
		default:
		{
			const bool is_confirm = false;
			makeTopbarUI_forCache( topbar_ui, false, ZnkStr_cstr(cache_path), cache_dir_path, authentic_key, is_video, is_confirm );
			if( is_exist_vbox ){
				ZnkStr_addf( topbar_ui, "%s", ZnkStr_cstr(exist_msg) );
			}
			break;
		}
		}

		{
			const char* p = ZnkStr_cstr(file_path);
			const char* q = Znk_strrchr( p, '/' );
			if( q ){
				ZnkS_copy( dir_path, sizeof(dir_path), p, q-p );
			}
			ZnkBird_regist( bird, "dir_path", dir_path );
		}
		{
			const char* p = ZnkStr_cstr(cache_path);
			const char* q = Znk_strrchr( p, '/' );
			if( q ){
				ZnkS_copy( cache_dir_path, sizeof(cache_dir_path), p, q-p );
			}
			ZnkBird_regist( bird, "cache_dir_path", cache_dir_path );
		}

		if( !loadEnableTagList( tags_list, dir_path, ZnkStr_cstr(file_path) ) ){
			ZnkStr_addf( msg, "Cannot load enable tag list.\n" );
			ZnkStr_addf( msg, "  dir_path=[%s] file_path=[%s]\n", dir_path, ZnkStr_cstr(file_path) );
		}

		{
			ZnkVarp current_category     = ZnkVarpAry_find_byName_literal( post_vars, "category_key", false );
			if( current_category ){
				current_category_id = ZnkVar_cstr( current_category );
			} else {
				/***
				 * current_category が明示的に指定されていない場合は一番先頭のものが選ばれているものとする.
				 */
				ZnkMyf tags_myf = EstConfig_tags_myf();
				current_category_id = EstTag_getCategoryKey( tags_myf, 0 );
			}
			EstAssortUI_makeCategorySelectBar( category_select_bar, current_category_id, current_category_name,
					"img_viewer", "view", "", "#TagsView" );
		}

		EstAssortUI_makeTagsView( tags_view, tags_list, current_category_id, ZnkStr_cstr(assort_msg), ZnkStr_cstr(comment), false );

		/* 簡易新規追加UI */
		{
			ZnkMyf tags_myf = EstConfig_tags_myf();
			ZnkVarpAry category_vars = ZnkMyf_find_vars( tags_myf, current_category_id );

			ZnkStr_addf( assort_ui, "<fieldset class=MstyStdFieldSet><legend>新規タグを同時に追加</legend>\n" );
			ZnkStr_addf( assort_ui,
					"<input class=EasterInputField type=text name=EstCM_new_tag placeholder=\"タグ文字列の指定\" value=\"\" size=20> \n" );
			EstUIBase_makeSelectBox_byVarpAry( assort_ui, category_vars,
					"グループ: ", "EstCM_select_group", "group_0" );
			ZnkStr_addf( assort_ui, "</fieldset><br>" );
		}

		switch( dir_type ){
		case EstBoxDir_e_Favorite:
			ZnkStr_addf( assort_ui,
					"<a class=\"MstyElemLinkRed\" "
					"href=\"javascript:EjsAssort_submitCommandEx( document.fm_main, 'img_viewer', 'favoritize', 'add_tags=false&category_key=%s', '' )\">"
					"タグを更新</a>\n", current_category_id );
			ZnkStr_add( assort_ui, "<br><br>" );
			ZnkStr_addf( assort_ui,
					"<a class=\"MstyElemLinkRed\" "
					"href=\"javascript:EjsAssort_submitCommandEx( document.fm_main, 'img_viewer', 'stock', 'add_tags=false&category_key=%s', '' )\">"
					"この%sをストック状態へ降格</a>\n", current_category_id, content_type_name );
			ZnkStr_add( assort_ui, "<br><br>" );
			ZnkStr_addf( assort_ui,
					"<a class=\"MstyElemLinkRed\" "
					"href=\"/easter?est_manager=img_viewer&amp;command=remove&amp;cache_path=%s&amp;Moai_AuthenticKey=%s&amp;confirm=on\">"
					"この%sを削除</a>\n",
					ZnkStr_cstr(vpath),
					authentic_key,
					content_type_name );
			break;
		case EstBoxDir_e_Stockbox:
			ZnkStr_addf( assort_ui,
					"<a class=\"MstyElemLinkRed\" "
					"href=\"javascript:EjsAssort_submitCommandEx( document.fm_main, 'img_viewer', 'stock', 'category_key=%s', '' )\">"
					"タグを更新</a>\n", current_category_id );
			ZnkStr_add( assort_ui, "<br><br>" );
			ZnkStr_addf( assort_ui,
					"<a class=\"MstyElemLinkRed\" "
					"href=\"javascript:EjsAssort_submitCommandEx( document.fm_main, 'img_viewer', 'favoritize', 'add_tags=false&category_key=%s', '' )\">"
					"この%sをお気に入りへ昇格</a>\n", current_category_id, content_type_name );
			ZnkStr_add( assort_ui, "<br><br>" );
			ZnkStr_addf( assort_ui,
					"<a class=\"MstyElemLinkRed\" "
					"href=\"/easter?est_manager=img_viewer&amp;command=remove&amp;cache_path=%s&amp;Moai_AuthenticKey=%s&amp;confirm=on\">"
					"この%sを削除</a>\n",
					ZnkStr_cstr(vpath),
					authentic_key,
					content_type_name );
			break;
		default:
			ZnkStr_addf( assort_ui,
					"<a class=\"MstyElemLinkRed\" "
					"href=\"javascript:EjsAssort_submitCommandEx( document.fm_main, 'img_viewer', 'stock', 'category_key=%s', '' )\">"
					"この%sをストック</a>\n", current_category_id, content_type_name );
			ZnkStr_addf( assort_ui,
					"<a class=\"MstyElemLinkRed\" "
					"href=\"javascript:EjsAssort_submitCommandEx( document.fm_main, 'img_viewer', 'favoritize', 'category_key=%s', '' )\">"
					"この%sをお気に入りへ</a>\n", current_category_id, content_type_name );
			break;
		}
		ZnkStrAry_destroy( tags_list );

		if( is_video ){
			ZnkStr_addf( topbar_ui, "<br>%s", ZnkStr_cstr(play_msg) );
		}
		ZnkStr_delete( vpath );
	}

FUNC_END:
	ZnkBird_regist( bird, "backto", ZnkStr_cstr(backto) );
	ZnkBird_regist( bird, "category_select_bar", ZnkStr_cstr(category_select_bar) );
	ZnkBird_regist( bird, "current_category_name", ZnkStr_cstr(current_category_name) );
	ZnkBird_regist( bird, "xhr_dmz", xhr_dmz );
	ZnkBird_regist( bird, "topbar_ui", ZnkStr_cstr(topbar_ui) );
	ZnkBird_regist( bird, "file_path", ZnkStr_cstr(file_path) );
	ZnkBird_regist( bird, "cache_path", ZnkStr_cstr(cache_path) );
	ZnkBird_regist( bird, "tags_view", ZnkStr_cstr(tags_view) );
	ZnkBird_regist( bird, "assort_ui", ZnkStr_cstr(assort_ui) );
	ZnkBird_regist( bird, "security_verifier", ZnkStr_cstr(security_verifier) );

	ZnkStr_delete( topbar_ui );
	ZnkStr_delete( tags_view );
	ZnkStr_delete( assort_ui );
	ZnkStr_delete( tag_editor_msg );
	ZnkStr_delete( assort_msg );
	ZnkStr_delete( exist_msg );
	ZnkStr_delete( play_msg );
	ZnkStr_delete( comment );
	ZnkStr_delete( file_path );
	ZnkStr_delete( fsys_path );
	ZnkStr_delete( cache_path );
	ZnkStr_delete( renamed_filename );
	ZnkStr_delete( existed_filename );
	ZnkStr_delete( backto );
	ZnkStr_delete( category_select_bar );
	ZnkStr_delete( current_category_name );
	ZnkStr_delete( security_verifier );

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
		ZnkStr hint_table = EstHint_getHintTable( "img_view" );
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
}

