#include <Est_assort.h>
#include <Est_config.h>
#include <Est_ui.h>
#include <Est_recentory.h>
#include <Est_base.h>
#include <Est_dir_util.h>
#include <Est_tag.h>

#include <Rano_sset.h>
#include <Rano_file_info.h>

#include <Znk_htp_util.h>
#include <Znk_dir.h>
#include <Znk_dir_recursive.h>
#include <Znk_missing_libc.h>
#include <Znk_str_ex.h>
#include <Znk_date.h>
#include <Znk_mem_find.h>
#include <Znk_str_fio.h>
#include <Znk_md5.h>
#include <Znk_varp_ary.h>

#define IS_OK( val ) (bool)( (val) != NULL )

static void
getInfoListMyfPath( char* myf_file_path, size_t myf_file_path_size,
		const char* dst_topdir, const char* ext, const char* subdir_xx )
{
	Znk_snprintf( myf_file_path, myf_file_path_size, "%s/%s/%s/info_list.myf",
			dst_topdir, ext, subdir_xx );
}
static ZnkMyf
openInfoListMyf( const char* myf_file_path, bool is_modesty )
{
	ZnkMyf myf = NULL;
	if( ZnkDir_getType( myf_file_path ) == ZnkDirType_e_File ){
		myf = ZnkMyf_create();
		ZnkMyf_load( myf, myf_file_path );
	} else {
		if( is_modesty ){
			return NULL;
		}
		myf = ZnkMyf_create();
		ZnkMyf_set_quote( myf, "['", "']" );
	}
	return myf;
}

/* Update info_list.myf */
static bool
registInfoListMyf( const char* dst_topdir, const char* ext, const char* subdir_xx,
		const char* md5_of_file, const char* dst_file_path, const char* file_tags, const char* comment,
		bool is_marge_tags, bool is_modesty, ZnkStr msg )
{
	bool       result = false;
	char       myf_file_path[ 256 ] = "";
	char       file_key[ 256 ] = "";
	ZnkMyf     myf = NULL;
	ZnkVarpAry vars = NULL;
	ZnkDate    date = { 0 };
	uint64_t   file_size = 0;
	ZnkStr     str_dst = NULL;

	getInfoListMyfPath( myf_file_path, sizeof(myf_file_path), dst_topdir, ext, subdir_xx );
	myf = openInfoListMyf( myf_file_path, is_modesty );
	if( myf == NULL ){
		ZnkStr_addf( msg, "registInfoListMyf : openInfoListMyf is failure.\n" );
		goto FUNC_END;
	}

	Znk_snprintf( file_key, sizeof(file_key), "%s.%s", md5_of_file, ext );

	RanoFileInfo_getFileSize( dst_file_path, &file_size );
	RanoFileInfo_getLastUpdatedTime( dst_file_path, &date );

	if( is_modesty ){
		vars = ZnkMyf_find_vars( myf, file_key );
		if( vars == NULL ){
			ZnkStr_addf( msg, "registInfoListMyf : file_key=[%s] is missing.\n", file_key );
			goto FUNC_END;
		}
	} else {
		vars = ZnkMyf_intern_vars( myf, file_key );
	}

	/* file_path : no needed in info_list.myf */

	str_dst = EstBase_registStrToVars( vars, "file_tags", NULL );
	if( is_marge_tags ){
		RanoSSet_addSeq( str_dst, file_tags );
	} else {
		ZnkStr_set( str_dst, file_tags );
	}

	str_dst = EstBase_registStrToVars( vars, "file_time", NULL );
	ZnkDate_getStr( str_dst, 0, &date, ZnkDateStr_e_Std );

	str_dst = EstBase_registStrToVars( vars, "file_size", NULL );
	ZnkStr_setf( str_dst, "%I64u", file_size );

	str_dst = EstBase_registStrToVars( vars, "comment", comment );
	EstBase_escapeToAmpForm( str_dst );

	result = ZnkMyf_save( myf, myf_file_path );

FUNC_END:
	ZnkMyf_destroy( myf );
	return result;
}

static bool
eraseInfoListMyf( const char* dst_dir_path, const char* md5filename, ZnkStr msg )
{
	bool       result = false;
	ZnkMyf     myf = NULL;
	char       myf_file_path[ 256 ] = "";
	size_t     sec_idx = Znk_NPOS;
	bool       is_modesty = true;

	Znk_snprintf( myf_file_path, sizeof(myf_file_path), "%s/info_list.myf", dst_dir_path );
	myf = openInfoListMyf( myf_file_path, is_modesty );
	if( myf == NULL ){
		ZnkStr_addf( msg, "eraseInfoListMyf : openInfoListMyf is failure.\n" );
		goto FUNC_END;
	}

	sec_idx = ZnkMyf_findIdx_withSecType( myf, md5filename, ZnkMyfSection_e_Vars );
	if( sec_idx == Znk_NPOS ){
		/* not exist */
		ZnkStr_addf( msg, "eraseInfoListMyf : ZnkMyf_findIdx is failure.\n" );
		goto FUNC_END;
	}

	ZnkMyf_erase_byIdx( myf, sec_idx );
	result = ZnkMyf_save( myf, myf_file_path );

FUNC_END:
	ZnkMyf_destroy( myf );
	return result;
}

static bool
eraseFInf( const char* myf_file_path, const char* md5filename, ZnkStr msg )
{
	bool       result = false;
	size_t     idx = Znk_NPOS;
	ZnkVarpAry finf_list = EstFInfList_create();
	ZnkVarpAry sqi_vars  = ZnkVarpAry_create( true );

	if( !EstFInfList_load( finf_list, sqi_vars, myf_file_path ) ){
		ZnkStr_addf( msg, "eraseFInf : EstFInfList_load is failure.\n" );
		goto FUNC_END;
	}

	idx = EstFInfList_findIdx_byName( finf_list, md5filename );
	if( idx == Znk_NPOS ){
		/* not exist */
		ZnkStr_addf( msg, "eraseFInf : EstFInfList_findIdx_byName is failure.\n" );
		goto FUNC_END;
	}

	EstFInfList_erase_byIdx( finf_list, idx );

	{
		EstSQI sqi = EstSQI_create( sqi_vars );
		result = EstFInfList_save( finf_list, sqi, myf_file_path, false, false );
		EstSQI_destroy( sqi );
	}

FUNC_END:
	EstFInfList_destroy( finf_list );
	ZnkVarpAry_destroy( sqi_vars );
	return result;
}

static bool
addTags_toInfoListMyf( const char* dst_topdir, const char* ext, const char* subdir_xx,
		const char* md5_of_file, const char* dst_file_path, const char* file_tags, const char* comment,
		bool is_modesty, size_t* processed_count )
{
	bool       result = false;
	char       myf_file_path[ 256 ] = "";
	char       file_key[ 256 ] = "";
	ZnkMyf     myf = ZnkMyf_create();
	ZnkVarpAry vars = NULL;
	ZnkDate    date = { 0 };
	uint64_t   file_size = 0;
	ZnkStr     str_dst = NULL;
	bool is_marge_tags = true;

	Znk_snprintf( myf_file_path, sizeof(myf_file_path), "%s/%s/%s/info_list.myf",
			dst_topdir, ext, subdir_xx );

	Znk_snprintf( file_key, sizeof(file_key), "%s.%s", md5_of_file, ext );

	RanoFileInfo_getFileSize( dst_file_path, &file_size );
	RanoFileInfo_getLastUpdatedTime( dst_file_path, &date );

	if( ZnkDir_getType( myf_file_path ) == ZnkDirType_e_File ){
		ZnkMyf_load( myf, myf_file_path );
	} else {
		if( is_modesty ){
			goto FUNC_END;
		}
		ZnkMyf_set_quote( myf, "['", "']" );
	}

	if( is_modesty ){
		vars = ZnkMyf_find_vars( myf, file_key );
		if( vars == NULL ){
			goto FUNC_END;
		}
	} else {
		vars = ZnkMyf_intern_vars( myf, file_key );
	}

	str_dst = EstBase_registStrToVars( vars, "file_tags", NULL );
	if( is_marge_tags ){
		*processed_count = RanoSSet_addSeq( str_dst, file_tags );
	} else {
		ZnkStr_set( str_dst, file_tags );
	}

	str_dst = EstBase_registStrToVars( vars, "file_time", NULL );
	ZnkDate_getStr( str_dst, 0, &date, ZnkDateStr_e_Std );

	str_dst = EstBase_registStrToVars( vars, "file_size", NULL );
	ZnkStr_setf( str_dst, "%I64u", file_size );

	str_dst = EstBase_registStrToVars( vars, "comment", comment );
	EstBase_escapeToAmpForm( str_dst );

	result = ZnkMyf_save( myf, myf_file_path );

FUNC_END:
	ZnkMyf_destroy( myf );
	return result;
}

static void
getMD5CheckSum( ZnkMD5CheckSum* checksum, const char* file_path )
{
	ZnkMD5_CTX md5ctx = {{ 0 }};
	ZnkMD5_getDigest_byFile( &md5ctx, file_path );
	ZnkMD5CheckSum_get_byContext( checksum, &md5ctx );
}

/***
 * favorite/ext/xx/md5_of_file.ext という形式でリネームしてファイルを移動する.
 * ここでextは拡張子、xx は md5_of_file(該当ファイルの内容のMD5CheckSumをとったもの)
 * における最初の2文字で16進数である.
 */
bool
EstAssort_renameFile_toMD5ofFile( const char* src_file_path, const char* dst_topdir, ZnkStr msg, size_t* processed_count,
		const char* file_tags, const char* comment, bool is_marge_tags,
		ZnkStr renamed_filename, bool* ans_is_moved_file )
{
	ZnkMD5CheckSum checksum = {{ 0 }};
	const char* md5_of_file = NULL;
	const char* ext = ZnkS_get_extension( src_file_path, '.' );
	char subdir_xx[ 16 ] = "";
	bool result = false;
	ZnkStr dst_file_path = ZnkStr_new( "" );
	bool is_modesty = false;
	bool is_src_delete = true;
	bool is_moved_file = false;

	getMD5CheckSum( &checksum, src_file_path );
	md5_of_file = ZnkMD5CheckSum_cstr( &checksum );

	if( ZnkS_empty( ext ) ){ ext = "dat"; }
	ZnkS_copy( subdir_xx, sizeof(subdir_xx), md5_of_file, 2 );
	ZnkStr_setf( renamed_filename, "%s/%s/%s.%s",
			ext, subdir_xx,
			md5_of_file, ext );

	is_moved_file = EstDirUtil_moveFile( src_file_path, dst_topdir, ZnkStr_cstr(renamed_filename), msg, dst_file_path, is_src_delete );

	/***
	 * ファイル移動に失敗した場合は、info_list.myf内に該当項目が既に存在する場合のみ
	 * その情報を更新する(存在しない場合は何もしない).
	 */
	is_modesty = !is_moved_file;

	/***
	 * ファイル移動に成功したかどうかを関数外から知りたい場合がある.
	 * (例えば別途移動元の情報を保持しているメタファイルがあったとして、
	 * ファイル移動に失敗している場合は、そのメタファイルの移動元の情報を削除してはならない.)
	 */
	if( ans_is_moved_file ){
		*ans_is_moved_file = is_moved_file;
	}

	result = registInfoListMyf( dst_topdir, ext, subdir_xx,
			md5_of_file, ZnkStr_cstr(dst_file_path), file_tags, comment, is_marge_tags, is_modesty, msg );
	if( result && processed_count ){
		++(*processed_count);
	}

	ZnkStr_delete( dst_file_path );
	return result;
}

static bool
getComment( const char* myf_file_path, const char* file_key, ZnkStr comment )
{
	bool       result = false;
	ZnkMyf     myf  = NULL;
	ZnkVarpAry vars = NULL;
	ZnkVarp    var  = NULL;
	ZnkStr     str  = NULL;
	bool is_modesty = true;

	myf = openInfoListMyf( myf_file_path, is_modesty );
	if( myf == NULL ){
		goto FUNC_END;
	}
	vars = ZnkMyf_find_vars( myf, file_key );
	if( vars == NULL ){
		goto FUNC_END;
	}

	var = ZnkVarpAry_find_byName_literal( vars, "comment", false );
	if( var ){
		str = ZnkVar_str( var );
		ZnkStr_copy( comment, str );
		EstBase_unescapeToAmpForm( comment );
		result = true;
	}

FUNC_END:
	ZnkMyf_destroy( myf );
	return result;
}

bool
EstAssort_getEssential( ZnkStr comment,
		EstBoxDirType ast_dir_type, const char* fsys_path, const char* assort_path,
		const char* box_fsys_dir, ZnkStr msg, ZnkStr renamed_filename )
{
	bool result = false;

	if( ZnkDir_getType( box_fsys_dir ) != ZnkDirType_e_Directory ){
		ZnkStr_addf( msg, "EstAssort_getEssential : [%s] directory does not exist.\n", box_fsys_dir );
		goto FUNC_END;
	} else {

		switch( ast_dir_type ){
		case EstBoxDir_e_Unknown:
			ZnkStr_addf( msg, "EstAssort_getEssential : ast_dir_type is Unknown.\n" );
			break;
		case EstBoxDir_e_Favorite:
		case EstBoxDir_e_Stockbox:
		{
			/* 仕分け先が与えられている場合は仕分け先そのものをチェックする */
			const char* p = Znk_strchr( assort_path, '/');
			if( p ){
				char md5_of_file[ 64 ] = "";
				const char* ext = ZnkS_get_extension( fsys_path, '.' );
				char subdir_xx[ 16 ] = "";
				ZnkStr dst_file_path = ZnkStr_new( "" );
				ZnkStr dst_info_path  = ZnkStr_new( "" );
				const char* q = Znk_strrchr( assort_path, '/');
				const char* r = Znk_strrchr( q, '.');

				ZnkS_copy( md5_of_file, sizeof(md5_of_file), q+1, r-q-1 );
				ZnkS_copy( subdir_xx, sizeof(subdir_xx), md5_of_file, 2 );
				if( ZnkS_empty( ext ) ){ ext = "dat"; }

				ZnkStr_set( renamed_filename, p+1 );
				ZnkStr_setf( dst_file_path, "%s/%s", box_fsys_dir, ZnkStr_cstr(renamed_filename) );
				ZnkStr_setf( dst_info_path, "%s/%s/%s/info_list.myf", box_fsys_dir, ext, subdir_xx );
				if( ZnkDir_getType( ZnkStr_cstr(dst_file_path) ) == ZnkDirType_e_File ){
					ZnkStr_addf( msg, "EstAssort_getEssential : dst_file_path[%s] is file.\n", ZnkStr_cstr(dst_file_path) );
					result = true;
				} else {
					ZnkStr_addf( msg, "EstAssort_getEssential : dst_file_path[%s] is not file.\n", ZnkStr_cstr(dst_file_path) );
				}
				if( ZnkDir_getType( ZnkStr_cstr(dst_info_path) ) == ZnkDirType_e_File ){
					char file_key[ 256 ] = "";
					Znk_snprintf( file_key, sizeof(file_key), "%s.%s", md5_of_file, ext );
					getComment( ZnkStr_cstr(dst_info_path), file_key, comment );
				} else {
					ZnkStr_addf( msg, "EstAssort_getEssential : dst_info_path[%s] is not file.\n", ZnkStr_cstr(dst_info_path) );
				}
				ZnkStr_delete( dst_file_path );
				ZnkStr_delete( dst_info_path );
			}
			break;
		}
		default:
			/* 仕分け元が与えられている場合は仕分け元のMD5を取得し、その名前で仕分け先をチェックする */
			if( ZnkDir_getType( fsys_path ) == ZnkDirType_e_File ){
				ZnkMD5CheckSum checksum = {{ 0 }};
				const char* md5_of_file = NULL;
				const char* ext = ZnkS_get_extension( fsys_path, '.' );
				char subdir_xx[ 16 ] = "";
				ZnkStr dst_file_path = ZnkStr_new( "" );
				ZnkStr dst_info_path  = ZnkStr_new( "" );
			
				getMD5CheckSum( &checksum, fsys_path );
				md5_of_file = ZnkMD5CheckSum_cstr( &checksum );
			
				if( ZnkS_empty( ext ) ){ ext = "dat"; }
				ZnkS_copy( subdir_xx, sizeof(subdir_xx), md5_of_file, 2 );
				ZnkStr_setf( renamed_filename, "%s/%s/%s.%s",
						ext, subdir_xx,
						md5_of_file, ext );
				ZnkStr_setf( dst_file_path, "%s/%s", box_fsys_dir, ZnkStr_cstr(renamed_filename) );
				ZnkStr_setf( dst_info_path, "%s/%s/%s/info_list.myf", box_fsys_dir, ext, subdir_xx );

				if( ZnkDir_getType( ZnkStr_cstr(dst_file_path) ) == ZnkDirType_e_File ){
					ZnkStr_addf( msg, "EstAssort_getEssential : dst_file_path[%s] is file.\n", ZnkStr_cstr(dst_file_path) );
					result = true;
				} else {
					ZnkStr_addf( msg, "EstAssort_getEssential : dst_file_path[%s] is not file.\n", ZnkStr_cstr(dst_file_path) );
				}
				if( ZnkDir_getType( ZnkStr_cstr(dst_info_path) ) == ZnkDirType_e_File ){
					char file_key[ 256 ] = "";
					Znk_snprintf( file_key, sizeof(file_key), "%s.%s", md5_of_file, ext );
					getComment( ZnkStr_cstr(dst_info_path), file_key, comment );
				}
				ZnkStr_delete( dst_file_path );
				ZnkStr_delete( dst_info_path );
			} else {
				ZnkStr_addf( msg, "EstAssort_getEssential : verify : fsys_path=[%s] is not file.\n", fsys_path );
			}
			break;
		}
	}

FUNC_END:
	return result;

}

bool
EstAssort_isExist( EstBoxDirType ast_dir_type, const char* fsys_path, const char* assort_path,
		const char* box_fsys_dir, ZnkStr msg, ZnkStr renamed_filename )
{
	bool result = false;

	if( ZnkDir_getType( box_fsys_dir ) != ZnkDirType_e_Directory ){
		ZnkStr_addf( msg, "EstAssort_isExist : [%s] directory does not exist.\n", box_fsys_dir );
		goto FUNC_END;
	} else {

		switch( ast_dir_type ){
		case EstBoxDir_e_Unknown:
			ZnkStr_addf( msg, "EstAssort_isExist : ast_dir_type is Unknown.\n" );
			break;
		case EstBoxDir_e_Favorite:
		{
			/* 仕分け先が与えられている場合は仕分け先そのものをチェックする */
			const char* p = Znk_strchr( assort_path, '/');
			if( p ){
				ZnkStr dst_file_path = ZnkStr_new( "" );
				ZnkStr_set( renamed_filename, p+1 );
				ZnkStr_setf( dst_file_path, "%s/%s", box_fsys_dir, ZnkStr_cstr(renamed_filename) );
				if( ZnkDir_getType( ZnkStr_cstr(dst_file_path) ) == ZnkDirType_e_File ){
					ZnkStr_addf( msg, "EstAssort_isExist : dst_file_path[%s] is file.\n", ZnkStr_cstr(dst_file_path) );
					result = true;
				} else {
					ZnkStr_addf( msg, "EstAssort_isExist : dst_file_path[%s] is not file.\n", ZnkStr_cstr(dst_file_path) );
				}
				ZnkStr_delete( dst_file_path );
			}
			break;
		}
		default:
			/* 仕分け元が与えられている場合は仕分け元のMD5を取得し、その名前で仕分け先をチェックする */
			if( ZnkDir_getType( fsys_path ) == ZnkDirType_e_File ){
				ZnkMD5CheckSum checksum = {{ 0 }};
				const char* md5_of_file = NULL;
				const char* ext = ZnkS_get_extension( fsys_path, '.' );
				char subdir_xx[ 16 ] = "";
				ZnkStr dst_file_path = ZnkStr_new( "" );
			
				getMD5CheckSum( &checksum, fsys_path );
				md5_of_file = ZnkMD5CheckSum_cstr( &checksum );
			
				if( ZnkS_empty( ext ) ){ ext = "dat"; }
				ZnkS_copy( subdir_xx, sizeof(subdir_xx), md5_of_file, 2 );
				ZnkStr_setf( renamed_filename, "%s/%s/%s.%s",
						ext, subdir_xx,
						md5_of_file, ext );
				ZnkStr_setf( dst_file_path, "%s/%s", box_fsys_dir, ZnkStr_cstr(renamed_filename) );

				if( ZnkDir_getType( ZnkStr_cstr(dst_file_path) ) == ZnkDirType_e_File ){
					ZnkStr_addf( msg, "EstAssort_isExist : dst_file_path[%s] is file.\n", ZnkStr_cstr(dst_file_path) );
					result = true;
				} else {
					ZnkStr_addf( msg, "EstAssort_isExist : dst_file_path[%s] is not file.\n", ZnkStr_cstr(dst_file_path) );
				}
				ZnkStr_delete( dst_file_path );
			} else {
				ZnkStr_addf( msg, "EstAssort_isExist : verify : fsys_path=[%s] is not file.\n", fsys_path );
			}
			break;
		}
	}

FUNC_END:
	return result;
}

size_t
EstAssort_doOneNewly( const char* file_tags, const char* comment, bool is_marge_tags,
		const char* src_fpath, const char* src_vpath,
		const char* box_fsys_dir, const char* box_vname, ZnkStr msg, ZnkStr renamed_filename )
{
	size_t processed_count = 0;

	ZnkDir_mkdirPath( box_fsys_dir, Znk_NPOS, '/', NULL );

	if( src_fpath ){
		ZnkStr_addf( msg, "EstAssort_doOneNewly : src_fpath=[%s]\n", src_fpath );
		if( ZnkDir_getType( src_fpath ) == ZnkDirType_e_File ){
			ZnkStr dst_fsys_path = ZnkStr_new( "" );
			EstRecentory recent = EstRecentory_create();
			EstRecentory_load( recent, "recentory.myf" );

			ZnkStr_addf( msg, "  verify : src_fpath=[%s] is file.\n", src_fpath );
			EstAssort_renameFile_toMD5ofFile( src_fpath, box_fsys_dir, msg, &processed_count,
					file_tags, comment, is_marge_tags,
					renamed_filename, NULL );

			ZnkStr_setf( dst_fsys_path, "%s/%s", box_fsys_dir, ZnkStr_cstr(renamed_filename) );
			EstRecentory_add2( recent, ZnkStr_cstr(renamed_filename), box_vname, ZnkStr_cstr(dst_fsys_path), src_vpath, msg );
			ZnkStr_delete( dst_fsys_path );

			if( processed_count ){
				EstRecentory_save( recent, "recentory.myf", is_marge_tags );
			}
			EstRecentory_destroy( recent );
		} else {
			ZnkStr_addf( msg, "  verify : ZnkStr_cstr(src_fpath)=[%s] is not file.\n", src_fpath );
		}
	}

	return processed_count;
}

size_t
EstAssort_addTags( const char* file_tags, const char* comment, const char* src_fpath, const char* src_vpath,
		const char* box_fsys_dir, const char* box_vname, ZnkStr msg, ZnkStr renamed_filename )
{
	/**
	 * srcとdstが同じ場合はprocessed_countを0にすべき.
	 */
	size_t processed_count = 0;

	ZnkDir_mkdirPath( box_fsys_dir, Znk_NPOS, '/', NULL );

	if( ZnkDir_getType( src_fpath ) == ZnkDirType_e_File ){
		EstRecentory recent = EstRecentory_create();

		ZnkMD5CheckSum checksum = {{ 0 }};
		const char* md5_of_file = NULL;
		const char* ext = ZnkS_get_extension( src_fpath, '.' );
		char subdir_xx[ 16 ] = "";
		ZnkStr dst_fsys_path = ZnkStr_new( "" );
		EstRecentory_load( recent, "recentory.myf" );
	
		getMD5CheckSum( &checksum, src_fpath );
		md5_of_file = ZnkMD5CheckSum_cstr( &checksum );
	
		if( ZnkS_empty( ext ) ){ ext = "dat"; }
		ZnkS_copy( subdir_xx, sizeof(subdir_xx), md5_of_file, 2 );
		ZnkStr_setf( renamed_filename, "%s/%s/%s.%s",
				ext, subdir_xx,
				md5_of_file, ext );

		ZnkStr_setf( dst_fsys_path, "%s/%s", box_fsys_dir, ZnkStr_cstr(renamed_filename) );

		if( EstDirUtil_checkDuplicateSameSizeFiles( src_fpath, ZnkStr_cstr(dst_fsys_path), msg ) ){
			if( ZnkDir_deleteFile_byForce( src_fpath ) ){
				if( msg ){
					ZnkStr_addf( msg, "EstAssort_addTags : duplicated src_file [%s] is removed.\n", src_fpath );
				}
			}
		}

		addTags_toInfoListMyf( box_fsys_dir, ext, subdir_xx,
				md5_of_file, ZnkStr_cstr(dst_fsys_path), file_tags, comment,
				true, &processed_count );

		EstRecentory_add2( recent, ZnkStr_cstr(renamed_filename), box_vname, ZnkStr_cstr(dst_fsys_path), src_vpath, msg );

		ZnkStr_delete( dst_fsys_path );
		if( processed_count ){
			bool is_marge_tags = true;
			EstRecentory_save( recent, "recentory.myf", is_marge_tags );
		}
		EstRecentory_destroy( recent );
	} else {
		ZnkStr_addf( msg, "  verify : ZnkStr_cstr(src_fpath)=[%s] is not file.\n", src_fpath );
	}

	return processed_count;
}

void
EstAssort_convertCachePath_toFilePath( ZnkStr cache_path, const char* cache_top_dir, const char* file_top_dir )
{
	ZnkStr_replace( cache_path, 0, Znk_strlen(cache_top_dir), file_top_dir, Znk_NPOS );
}

/***
 * 例1.
 * src_top_dir = ../
 * src_file_path = ../data.dat
 * dst_top_dir = ../dst_dir
 * dst_path = ../dst_dir/data.dat
 *
 * src_top_dir = ../src_dir
 * src_file_path = ../src_dir/data.dat
 * dst_top_dir = ../dst_dir
 * dst_path = ../dst_dir/data.dat
 *
 * src_top_dir = ../src_dir
 * src_file_path = ../src_dir/dat/data.dat
 * dst_top_dir = ../dst_dir
 * dst_path = ../dst_dir/dat/data.dat
 */
size_t
EstAssort_moveSubdirFile( const char* src_top_dir, const char* src_file_path, const char* dst_top_dir, const char* dst_additional_under_dir,
		const char* title, ZnkStr ermsg )
{
	static const char* func_name = "EstAssort_moveSubdirFile";
	size_t result = 0;
	char src_under_dir[ 256 ] = "";
	char dst_dir[ 256 ] = "";
	char dst_file_path[ 256 ] = "";
	const size_t src_top_dir_leng = Znk_strlen( src_top_dir );
	const char*  p = NULL;
	const char*  src_under_begin = NULL;

	if( !ZnkS_isBegin( src_file_path, src_top_dir ) ){
		if( ermsg ){
			ZnkStr_addf( ermsg, "%s : %s : src_file_path=[%s] is not begun [%s].\n", title, func_name, src_file_path, src_top_dir );
		}
		return 0;
	}

	/***
	 * src_under_begin とは src_file_path のうち src_top_dir 以降の部分でfileを示す.
	 * src_under_dir と dst_file_path を作成するために一時的に生成する.
	 */
	src_under_begin = src_file_path + src_top_dir_leng;
	while( *src_under_begin == '/' ){
		++src_under_begin;
	}

	/***
	 * src_under_dir とは src_file_path のうち src_top_dir 以降の部分でdirectoryを示す.
	 * dst_dir を作成するために一時的に生成する.
	 */
	p = Znk_strrchr( src_under_begin, '/' );
	if( p ){
		while( *p == '/' ){ --p; }
		ZnkS_copy( src_under_dir, sizeof(src_under_dir), src_under_begin, p+1-src_under_begin );
	}
	if( Znk_strlen(src_under_dir) ){
		if( ZnkS_empty( dst_additional_under_dir ) ){
			Znk_snprintf( dst_dir, sizeof(dst_dir),             "%s/%s",    dst_top_dir, src_under_dir );
			Znk_snprintf( dst_file_path, sizeof(dst_file_path), "%s/%s",    dst_top_dir, src_under_begin );
		} else {
			Znk_snprintf( dst_dir, sizeof(dst_dir),             "%s/%s/%s", dst_top_dir, dst_additional_under_dir, src_under_dir );
			Znk_snprintf( dst_file_path, sizeof(dst_file_path), "%s/%s/%s", dst_top_dir, dst_additional_under_dir, src_under_begin );
		}
	} else {
		/* dst_additional_under_dir が設定されている場合はそれをdst側へ挿入する */
		if( ZnkS_empty( dst_additional_under_dir ) ){
			ZnkS_copy(    dst_dir, sizeof(dst_dir), dst_top_dir, Znk_NPOS );
			Znk_snprintf( dst_file_path, sizeof(dst_file_path), "%s/%s",    dst_top_dir, src_under_begin );
		} else {
			Znk_snprintf( dst_dir, sizeof(dst_dir),             "%s/%s",    dst_top_dir, dst_additional_under_dir );
			Znk_snprintf( dst_file_path, sizeof(dst_file_path), "%s/%s/%s", dst_top_dir, dst_additional_under_dir, src_under_begin );
		}
	}

	if( !ZnkDir_mkdirPath( dst_dir, Znk_NPOS, '/', ermsg ) ){
		if( ermsg ){
			ZnkStr_addf( ermsg, "%s : %s : ZnkDir_mkdirPath [%s] is failure.\n", title, func_name, dst_dir );
		}
		result = 0;
	} else {
		if( ZnkDir_copyFile_byForce( src_file_path, dst_file_path, ermsg ) ){
			if( ZnkDir_deleteFile_byForce( src_file_path ) ){
				if( ermsg ){
					ZnkStr_addf( ermsg, "%s : %s : [%s] to [%s] OK.\n", title, func_name, src_file_path, dst_file_path );
				}
				result = 1;
			} else {
				if( ermsg ){
					ZnkStr_addf( ermsg, "%s : %s : [%s] to [%s] failure(delete).\n", title, func_name, src_file_path, dst_file_path );
				}
				result = 0;
			}
		} else {
			if( ermsg ){
				ZnkStr_addf( ermsg, "%s : %s : [%s] to [%s] failure(copy).\n", title, func_name, src_file_path, dst_file_path );
			}
			result = 0;
		}
	}

	return result;
}

bool
EstAssort_remove( EstBoxDirType box_dir_type, const char* fsys_path, ZnkStr msg )
{
	bool result = false;

	switch( box_dir_type ){
	case EstBoxDir_e_Unknown:
		ZnkStr_addf( msg, "EstAssort_remove : box_dir_type is Unknown.\n" );
		break;
	case EstBoxDir_e_Favorite:
	case EstBoxDir_e_Stockbox:
	{
		const char* p = Znk_strrchr( fsys_path, '/' );
		if( p ){
			char fsys_dir[ 256 ] = "";
			const char* md5filename = p + 1;
			ZnkS_copy( fsys_dir, sizeof(fsys_dir), fsys_path, p-fsys_path );
			if( eraseInfoListMyf( fsys_dir, md5filename, msg ) ){
				ZnkStr_addf( msg, "EstAssort_remove : eraseInfoListMyf done. : fsys_dir=[%s] md5filename=[%s].\n", fsys_dir, md5filename );
			} else {
				ZnkStr_addf( msg, "EstAssort_remove : eraseInfoListMyf not done. : fsys_dir=[%s] md5filename=[%s].\n", fsys_dir, md5filename );
			}
			EstAssort_remove_fromFInfList( box_dir_type, md5filename, msg );
		}
		break;
	}
	default:
		break;
	}

	return result;
}
bool
EstAssort_remove_fromFInfList( EstBoxDirType box_dir_type, const char* md5filename, ZnkStr msg )
{
	char myf_file_path[ 256 ] = "";
	const char* box_fsys_dir = EstBoxBase_getBoxFsysDir( box_dir_type );
	Znk_snprintf( myf_file_path, sizeof(myf_file_path), "%s/finf_list.myf", box_fsys_dir );
	return eraseFInf( myf_file_path, md5filename, msg );
}

void
EstAssort_addImgURLList( ZnkStr EstCM_img_url_list, size_t file_count, size_t begin_idx, size_t end_idx, const char* path )
{
	if( EstCM_img_url_list && file_count >= begin_idx && file_count < end_idx ){
		const char* xhr_dmz = EstConfig_XhrDMZ();
		const char* ext = ZnkS_get_extension( path, '.' );
		if( ZnkS_eqCase( ext, "jpg" ) 
		 || ZnkS_eqCase( ext, "png" )
		 || ZnkS_eqCase( ext, "gif" ) )
		{
			if( ZnkStr_empty( EstCM_img_url_list ) ){
				ZnkStr_add( EstCM_img_url_list, "\n\t " );
			} else {
				ZnkStr_add( EstCM_img_url_list, "\t," );
			}
			ZnkStr_addf( EstCM_img_url_list, "'http://%s/cgis/easter/%s'\n", xhr_dmz, path );
		}
	}
}

bool
EstAssort_registNewTag( ZnkVarpAry post_vars, ZnkStr tag_editor_msg, ZnkStr tagid )
{
	bool result = false;
	ZnkVarp varp = NULL;
	if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "EstCM_new_tag", false ) )){
		if( ZnkS_empty( ZnkVar_cstr( varp ) ) ){
			ZnkStr_addf( tag_editor_msg, "無効なタグです.\n" );
		} else {
			ZnkStr unesc_new_tag = ZnkStr_new( "" );
			ZnkHtpURL_unescape_toStr( unesc_new_tag, ZnkVar_cstr(varp), ZnkVar_str_leng(varp) );
			if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "EstCM_select_group", false ) )){
				ZnkMyf tags_myf = EstConfig_tags_myf();
				const char* EstCM_select_group = ZnkVar_cstr( varp );
				if( EstTag_registNew( tags_myf, ZnkStr_cstr(unesc_new_tag), tagid, tag_editor_msg, EstCM_select_group ) ){
					EstConfig_saveTagsMyf();
					result = true;
				}
			}
			ZnkStr_delete( unesc_new_tag );
		}
	} else {
		ZnkStr_addf( tag_editor_msg, "EstCM_new_tag doesn't exist.\n" );
	}
	return result;
}
void
EstAssort_addPostVars_ifNewTagRegisted( ZnkVarpAry post_vars, ZnkStr tag_editor_msg )
{
	ZnkStr tagid = ZnkStr_new( "" );
	if( EstAssort_registNewTag( post_vars, tag_editor_msg, tagid ) ){
		ZnkVarp new_tag = NULL;
		ZnkVarp new_var = ZnkVarp_create( ZnkStr_cstr(tagid), "", 0, ZnkPrim_e_Str, NULL );
		if( IS_OK( new_tag = ZnkVarpAry_find_byName_literal( post_vars, "EstCM_new_tag", false ) )){
			ZnkVar_set_val_Str( new_var, ZnkVar_cstr( new_tag ), ZnkVar_str_leng( new_tag ) );
		}
		ZnkVarpAry_push_bk( post_vars, new_var );
	}
	ZnkStr_delete( tagid );
}
