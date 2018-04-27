#include <Est_box.h>
#include <Est_finf.h>
#include <Est_recentory.h>
#include <Est_bat_operator.h>
#include <Est_assort.h>
#include <Est_dir_util.h>
#include <Est_tag.h>
#include <Est_config.h>
#include <Est_sqi.h>
#include <Est_search_manager.h>

#include <Znk_dir.h>
#include <Znk_md5.h>
#include <Znk_missing_libc.h>
#include <Znk_htp_util.h>

#define IS_OK( val ) (bool)( (val) != NULL )

typedef struct BoxContext_tag {
	const char*   box_fsys_dir_;
	const char*   box_vname_;
	size_t        processed_count_;
	ZnkStrAry     dst_vpath_list_ref_;
	ZnkStr        file_tags_;
	bool          is_marge_tags_;
	ZnkStr        comment_;
	ZnkStr        renamed_filename_;
	ZnkStr        dst_fsys_path_;
	ZnkVarpAry    finf_list_;
	EstRecentory recent_;
} *BoxContext;

static void
BoxContext_compose( BoxContext info, const char* box_fsys_dir, const char* box_vname )
{
	info->box_fsys_dir_    = box_fsys_dir;
	info->box_vname_       = box_vname;
	info->processed_count_ = 0;
	info->dst_vpath_list_ref_ = NULL;

	info->file_tags_        = ZnkStr_new( "" );
	info->is_marge_tags_    = false;
	info->comment_          = ZnkStr_new( "" );
	info->renamed_filename_ = ZnkStr_new( "" );
	info->dst_fsys_path_    = ZnkStr_new( "" );
	info->finf_list_ = EstFInfList_create();
	info->recent_    = EstRecentory_create();
}
static void
BoxContext_dispose( BoxContext info )
{
	ZnkStr_delete( info->file_tags_ );
	ZnkStr_delete( info->comment_ );
	ZnkStr_delete( info->renamed_filename_ );
	ZnkStr_delete( info->dst_fsys_path_ );
	EstFInfList_destroy( info->finf_list_ );
	EstRecentory_destroy( info->recent_ );
}

/***
 * remove
 * (完全削除)
 */
static bool
remove_dir_func( EstBoxDirType box_dir_type, const char* fsys_path, const char* unesc_vpath, size_t box_path_offset, ZnkStr msg, void* param )
{
	static const bool is_err_ignore = false;
	return ZnkDir_rmdirAll_force( fsys_path, is_err_ignore, msg );
}
static bool
remove_file_func( EstBoxDirType box_dir_type, const char* fsys_path, const char* unesc_vpath, size_t box_path_offset, ZnkStr msg, void* param )
{
	bool result = false;
	if( ZnkDir_deleteFile_byForce( fsys_path ) ){
		EstAssort_remove( box_dir_type, fsys_path, msg );
		ZnkStr_addf( msg, "  deleteFile : [%s] OK.\n", fsys_path );
		result = true;
	} else {
		ZnkStr_addf( msg, "  deleteFile : [%s] failure.\n", fsys_path );
	}
	return result;
}

size_t
EstBox_remove( ZnkVarpAry post_vars, ZnkStr msg )
{
	return EstBatOperator_processBox( post_vars, msg,
			remove_dir_func, 
			remove_file_func, 
			NULL );
}


/***
 * dustize
 * (dustboxへの移動)
 */
static bool
dustize_dir_func( EstBoxDirType box_dir_type, const char* fsys_path, const char* unesc_vpath, size_t box_path_offset, ZnkStr msg, void* param )
{
	EstDirUtil_moveDir( fsys_path, "dustbox",
			"EstBox_dustize", msg,
			NULL, NULL );
	return true;
}
static bool
dustize_file_func( EstBoxDirType box_dir_type, const char* fsys_path, const char* unesc_vpath, size_t box_path_offset, ZnkStr msg, void* param )
{
	bool result = false;
	const char* box_fsys_dir             = EstBoxBase_getBoxFsysDir( box_dir_type );
	const char* dst_additional_under_dir = EstBoxBase_getBoxVName( box_dir_type );
	if( EstAssort_moveSubdirFile( box_fsys_dir, fsys_path, "dustbox", dst_additional_under_dir, "EstBox_dustize", msg ) ){
		EstAssort_remove( box_dir_type, fsys_path, msg );
		ZnkStr_addf( msg, "  dustizeFile : [%s] OK.\n", fsys_path );
		result = true;
	} else {
		ZnkStr_addf( msg, "  dustizeFile : [%s] failure.\n", fsys_path );
	}
	return result;
}

size_t
EstBox_dustize( ZnkVarpAry post_vars, ZnkStr msg )
{
	return EstBatOperator_processBox( post_vars, msg,
			dustize_dir_func, 
			dustize_file_func, 
			NULL );
}

/***
 * favoritize
 * (favoriteへの移動)
 */
static bool
favoritize_dir_func( EstBoxDirType box_dir_type, const char* fsys_path, const char* unesc_vpath, size_t box_path_offset, ZnkStr msg, void* param )
{
	BoxContext info = Znk_force_ptr_cast( BoxContext, param );
	const char* box_fsys_dir = info->box_fsys_dir_;
	EstDirUtil_moveDir( fsys_path, box_fsys_dir, "EstBox_favoritize", msg, NULL, NULL );
	return true;
}
static bool
favoritize_file_func( EstBoxDirType box_dir_type, const char* fsys_path, const char* unesc_vpath, size_t box_path_offset, ZnkStr msg, void* param )
{
	BoxContext info = Znk_force_ptr_cast( BoxContext, param );
	const char* box_fsys_dir = info->box_fsys_dir_;
	const char* box_vname    = info->box_vname_;
	ZnkStr      renamed_filename = info->renamed_filename_;
	/* box_fsys_dir 直下 finf_list.myfへ追加. */
	const char* comment = ZnkStr_cstr( info->comment_ );
	bool is_moved_file = false;

	EstFInfList_registFileByMD5( info->finf_list_, box_vname, fsys_path,
			ZnkStr_cstr(info->file_tags_), comment, info->is_marge_tags_ );

	ZnkStr_addf( msg, "favoritize_file_func : fsys_path=[%s] unesc_vpath=[%s]\n", fsys_path, unesc_vpath );
	EstAssort_renameFile_toMD5ofFile( fsys_path, box_fsys_dir, msg, &info->processed_count_,
				ZnkStr_cstr(info->file_tags_), comment, info->is_marge_tags_, renamed_filename,
				&is_moved_file );
	if( is_moved_file ){
		/* ファイル本体の移動に成功している場合のみ、これを実行 */
		EstAssort_remove( box_dir_type, fsys_path, msg );
	}
	ZnkStr_setf( info->dst_fsys_path_, "%s/%s", box_fsys_dir, ZnkStr_cstr(renamed_filename) );
	EstRecentory_add2( info->recent_, ZnkStr_cstr(renamed_filename), box_vname, ZnkStr_cstr(info->dst_fsys_path_),
			unesc_vpath, msg );

	if( info->dst_vpath_list_ref_ ){
		ZnkStrAry_push_bk_snprintf( info->dst_vpath_list_ref_, Znk_NPOS, "favorite/%s", ZnkStr_cstr(renamed_filename) );
	}

	return true;
}

size_t
EstBox_favoritize( ZnkVarpAry post_vars, ZnkStr msg, const char* box_fsys_dir, const char* box_vname, ZnkStrAry dst_vpath_list )
{
	size_t count = 0;
	struct BoxContext_tag instance = { 0 }; BoxContext info = &instance;
	char finf_list_path[ 256 ] = "";
	ZnkVarpAry sqi_vars = ZnkVarpAry_create( true );
	bool add_tags = true;
	ZnkVarp varp = NULL;
	if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "add_tags", false ) )){
		add_tags = ZnkS_eq( ZnkVar_cstr( varp ), "true" );
	}

	BoxContext_compose( info, box_fsys_dir, box_vname );

	info->dst_vpath_list_ref_ = dst_vpath_list;
	EstTag_joinTagIDs( info->file_tags_, post_vars, ' ' );
	info->is_marge_tags_ = add_tags;

	EstRecentory_load( info->recent_, "recentory.myf" );

	ZnkDir_mkdirPath( box_fsys_dir, Znk_NPOS, '/', NULL );
	Znk_snprintf( finf_list_path, sizeof(finf_list_path), "%s/finf_list.myf", box_fsys_dir );
	EstFInfList_load( info->finf_list_, sqi_vars, finf_list_path );

	count = EstBatOperator_processBox( post_vars, msg,
			favoritize_dir_func, 
			favoritize_file_func, 
			info );
	if( count ){
		const bool is_marge_tags = add_tags;
		const bool is_modesty = false;
		EstSQI sqi = EstSQI_create( sqi_vars );
		EstRecentory_save( info->recent_, "recentory.myf", is_marge_tags );
		EstFInfList_save( info->finf_list_, sqi, finf_list_path, is_marge_tags, is_modesty );
		EstSQI_destroy( sqi );
	}

	BoxContext_dispose( info );
	ZnkVarpAry_destroy( sqi_vars );
	return count;
}


/***
 * stock
 * (stockboxへの移動)
 */
/* favoritize_dir_func と全く同じ */
static bool
stock_dir_func( EstBoxDirType box_dir_type, const char* fsys_path, const char* unesc_vpath, size_t box_path_offset, ZnkStr msg, void* param )
{
	BoxContext info = Znk_force_ptr_cast( BoxContext, param );
	const char* box_fsys_dir = info->box_fsys_dir_;
	EstDirUtil_moveDir( fsys_path, box_fsys_dir, "EstBox_stock", msg, NULL, NULL );
	return true;
}
static bool
stock_file_func( EstBoxDirType box_dir_type, const char* fsys_path, const char* unesc_vpath, size_t box_path_offset, ZnkStr msg, void* param )
{
	BoxContext info = Znk_force_ptr_cast( BoxContext, param );
	const char* box_fsys_dir = info->box_fsys_dir_;
	const char* box_vname    = info->box_vname_;
	const char* favorite_dir = EstConfig_favorite_dir();
	ZnkStr      renamed_filename = info->renamed_filename_;

	if( EstAssort_isExist( box_dir_type, fsys_path, unesc_vpath, favorite_dir, msg, renamed_filename ) ){
		ZnkStr_setf( info->dst_fsys_path_, "%s/%s", favorite_dir, ZnkStr_cstr(renamed_filename) );
		EstRecentory_add2( info->recent_, ZnkStr_cstr(renamed_filename), "favorite", ZnkStr_cstr(info->dst_fsys_path_),
				unesc_vpath, msg );
		++info->processed_count_;
	} else {
		const char* fileid  = NULL;
		const char* comment = ZnkStr_cstr( info->comment_ );
		EstFInf     finf    = NULL;
		/* box_fsys_dir : /ext/subdir_xx/md5_of_file.extを調査. */
		if( EstAssort_isExist( box_dir_type, fsys_path, unesc_vpath, box_fsys_dir, msg, renamed_filename ) ){
			finf = EstFInfList_registFileByMD5( info->finf_list_, box_vname, fsys_path,
					ZnkStr_cstr(info->file_tags_), comment, info->is_marge_tags_ );
			EstAssort_addTags( ZnkStr_cstr(info->file_tags_), ZnkStr_cstr(info->comment_), fsys_path, unesc_vpath,
					box_fsys_dir, box_vname, msg, renamed_filename );
			ZnkStr_addf( msg, "再ストックしました(タグ追加).\n" );
			++info->processed_count_;
		} else {
			/* box_fsys_dir :  直下 finf_list.myfへ追加. */
			finf = EstFInfList_registFileByMD5( info->finf_list_, box_vname, fsys_path,
					ZnkStr_cstr(info->file_tags_), comment, info->is_marge_tags_ );
			if( finf ){
				fileid = EstFInf_id( finf );
			}
			EstAssort_doOneNewly( ZnkStr_cstr(info->file_tags_), comment, info->is_marge_tags_,
					fsys_path, unesc_vpath,
					box_fsys_dir, box_vname, msg, renamed_filename );

			ZnkStr_addf( msg, "ストックしました(新規MD5[%s]).\n", fileid );
			++info->processed_count_;
		}
		if( info->dst_vpath_list_ref_ ){
			ZnkStrAry_push_bk_snprintf( info->dst_vpath_list_ref_, Znk_NPOS, "stockbox/%s", ZnkStr_cstr(renamed_filename) );
		}
	}
	return true;
}

size_t
EstBox_stock( ZnkVarpAry post_vars, ZnkStr msg, const char* box_fsys_dir, const char* box_vname, ZnkStrAry dst_vpath_list )
{
	size_t count = 0;
	struct BoxContext_tag instance = { 0 }; BoxContext info = &instance;
	char finf_list_path[ 256 ] = "";
	ZnkVarpAry sqi_vars = ZnkVarpAry_create( true );
	bool add_tags = true;
	ZnkVarp varp = NULL;
	if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "add_tags", false ) )){
		add_tags = ZnkS_eq( ZnkVar_cstr( varp ), "true" );
	}

	BoxContext_compose( info, box_fsys_dir, box_vname );

	info->dst_vpath_list_ref_ = dst_vpath_list;
	EstTag_joinTagIDs( info->file_tags_, post_vars, ' ' );
	info->is_marge_tags_ = add_tags;

	{
		ZnkVarp varp = ZnkVarpAry_findObj_byName_literal( post_vars, "ast_comment", false );
		if( varp ){
			ZnkHtpURL_unescape_toStr( info->comment_, ZnkVar_cstr(varp), ZnkVar_str_leng(varp) );
		}
	}

	EstRecentory_load( info->recent_, "recentory.myf" );

	ZnkDir_mkdirPath( box_fsys_dir, Znk_NPOS, '/', NULL );
	Znk_snprintf( finf_list_path, sizeof(finf_list_path), "%s/finf_list.myf", box_fsys_dir );
	EstFInfList_load( info->finf_list_, sqi_vars, finf_list_path );

	count = EstBatOperator_processBox( post_vars, msg,
			stock_dir_func, 
			stock_file_func, 
			info );
	if( count ){
		const bool is_marge_tags = add_tags;
		const bool is_modesty = false;
		EstSQI sqi = EstSQI_create( sqi_vars );
		EstRecentory_save( info->recent_, "recentory.myf", is_marge_tags );
		EstFInfList_save( info->finf_list_, sqi, finf_list_path, is_marge_tags, is_modesty );
		EstSQI_destroy( sqi );
	}

	BoxContext_dispose( info );
	ZnkVarpAry_destroy( sqi_vars );
	return count;
}


/***
 * recentize
 * (recentoryへの追加)
 */
static bool
recent_file_func( EstBoxDirType box_dir_type, const char* fsys_path, const char* unesc_vpath, size_t box_path_offset, ZnkStr msg, void* param )
{
	BoxContext info = Znk_force_ptr_cast( BoxContext, param );
	const char* favorite_dir = EstConfig_favorite_dir();
	ZnkStr      renamed_filename = info->renamed_filename_;

	if( EstAssort_isExist( box_dir_type, fsys_path, unesc_vpath, favorite_dir, msg, renamed_filename ) ){
		ZnkStr_setf( info->dst_fsys_path_, "%s/%s", favorite_dir, ZnkStr_cstr(renamed_filename) );
		EstRecentory_add2( info->recent_, ZnkStr_cstr(renamed_filename), "favorite", ZnkStr_cstr(info->dst_fsys_path_),
				unesc_vpath, msg );
		++info->processed_count_;
		ZnkStr_addf( msg, "Recentoryへ追加しました(by favorite).\n" );
	} else {
		const char* stockbox_dir = EstConfig_stockbox_dir();
		if( EstAssort_isExist( box_dir_type, fsys_path, unesc_vpath, stockbox_dir, msg, renamed_filename ) ){
			ZnkStr_setf( info->dst_fsys_path_, "%s/%s", stockbox_dir, ZnkStr_cstr(renamed_filename) );
			EstRecentory_add2( info->recent_, ZnkStr_cstr(renamed_filename), "stockbox", ZnkStr_cstr(info->dst_fsys_path_),
					unesc_vpath, msg );
			++info->processed_count_;
			ZnkStr_addf( msg, "Recentoryへ追加しました(by stockbox).\n" );
		} else {
			EstRecentory_add2( info->recent_, unesc_vpath+box_path_offset, "cachebox", fsys_path,
					unesc_vpath, msg );
			++info->processed_count_;
			ZnkStr_addf( msg, "Recentoryへ追加しました(by cachebox).\n" );
		}
	}
	return true;
}

size_t
EstBox_recentize( ZnkVarpAry post_vars, ZnkStr msg )
{
	size_t count = 0;
	struct BoxContext_tag instance = { 0 }; BoxContext info = &instance;
	const char* favorite_dir = EstConfig_favorite_dir();
	const char* stockbox_dir = EstConfig_stockbox_dir();
	bool add_tags = true;
	ZnkVarp varp = NULL;
	if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "add_tags", false ) )){
		add_tags = ZnkS_eq( ZnkVar_cstr( varp ), "true" );
	}

	BoxContext_compose( info, stockbox_dir, "stockbox" );
	info->is_marge_tags_ = add_tags;

	EstRecentory_load( info->recent_, "recentory.myf" );

	ZnkDir_mkdirPath( favorite_dir, Znk_NPOS, '/', NULL );
	ZnkDir_mkdirPath( stockbox_dir, Znk_NPOS, '/', NULL );

	count = EstBatOperator_processBox( post_vars, msg,
			NULL, /* φ */
			recent_file_func, 
			info );
	if( count ){
		const bool is_marge_tags = add_tags;
		EstRecentory_save( info->recent_, "recentory.myf", is_marge_tags );
	}

	BoxContext_dispose( info );
	return count;
}


/***
 * finf_list.myfの新規構築.
 */

static bool
isInterestExt( const char* ext )
{
	if(  ZnkS_eqCase( ext, "jpg" )
	  || ZnkS_eqCase( ext, "png" ) 
	  || ZnkS_eqCase( ext, "gif" ) 
	  || ZnkS_eqCase( ext, "html" ) 
	  || ZnkS_eqCase( ext, "htm" ) 
	  || ZnkS_eqCase( ext, "webm" ) 
	){
		return true;
	}
	return false;
}

void
EstBox_makeFInfList( const char* box_fsys_dir, const char* box_vname )
{
	char       finf_list_path[ 256 ] = "";
	EstSQI     sqi = NULL;
	ZnkVarpAry sqi_vars = ZnkVarpAry_create( true );
	const char* topics_dir = EstConfig_topics_dir();

	ZnkDirId dirid = ZnkDir_openDir( box_fsys_dir );
	if( dirid ){
		const char* name = NULL;
		char fsys_path[ 256 ] = "";
		char file_path[ 256 ] = "";
		const char* ext = NULL;
		ZnkStr renamed_filename = ZnkStr_new( "" );
		ZnkStr msg = NULL;
		size_t processed_count = 0;
		while( true ){
			name = ZnkDir_readDir( dirid );
			if( name == NULL ){
				break;
			}
			Znk_snprintf( fsys_path, sizeof(fsys_path), "%s/%s", box_fsys_dir, name );
			Znk_snprintf( file_path, sizeof(file_path), "%s/%s", box_vname, name );
			if( ZnkDir_getType( fsys_path ) == ZnkDirType_e_File ){
				ext = ZnkS_get_extension( name, '.' );
				if( isInterestExt( ext ) ){
					const char* file_tags = "";
					const char* comment   = "";
					const bool is_marge_tags = false;
					EstAssort_renameFile_toMD5ofFile( fsys_path, box_fsys_dir, msg, &processed_count,
							file_tags, comment, is_marge_tags, renamed_filename, NULL );
				}
			}
		}
		ZnkDir_closeDir( dirid );
		ZnkStr_delete( renamed_filename );
	}

	Znk_snprintf( finf_list_path, sizeof(finf_list_path), "%s/finf_list.myf", box_fsys_dir );
	{	
		static const bool force_fsys_scan = true;
		const char* searched_key = box_vname;
		char searched_dir[ 256 ] = "";

		Znk_snprintf( searched_dir, sizeof(searched_dir), "%s/tmp/searched", topics_dir );

		EstSQIVars_makeVars( sqi_vars,
				box_vname, "",
				"", "", "",
				"",
				"", "",
				"", "" );
		sqi = EstSQI_create( sqi_vars );
		EstSearchManager_searchInBox( sqi, NULL, searched_key, force_fsys_scan, searched_dir );
	}

	{
		const bool is_marge_tags = false;
		const bool is_modesty    = false;
		ZnkVarpAry finf_list = EstFInfList_create();
		char searched_myf_path[ 256 ] = "";
		Znk_snprintf( searched_myf_path, sizeof(searched_myf_path), "%s/tmp/searched/searched_%s.myf", topics_dir, box_vname );
		EstFInfList_load( finf_list, NULL, searched_myf_path );
		EstFInfList_sort( finf_list );
		EstFInfList_save( finf_list, sqi, finf_list_path, is_marge_tags, is_modesty );
		EstFInfList_destroy( finf_list );
	}

	ZnkVarpAry_destroy( sqi_vars );
	EstSQI_destroy( sqi );
}
