#include <Est_dir_util.h>

#include <Rano_file_info.h>

#include <Znk_s_base.h>
#include <Znk_str_ex.h>
#include <Znk_missing_libc.h>
#include <Znk_date.h>
#include <Znk_dir.h>
#include <Znk_dir_recursive.h>

bool
EstDirUtil_checkDuplicateSameSizeFiles( const char* src_file_path, const char* dst_file_path, ZnkStr ermsg )
{
	bool result = false;
	if(  ZnkDir_getType( src_file_path ) == ZnkDirType_e_File
	  && ZnkDir_getType( dst_file_path ) == ZnkDirType_e_File
	  && ZnkDir_getIdentity( src_file_path, dst_file_path ) != ZnkDirIdentity_e_Same )
	{
		uint64_t src_file_size = 0;
		uint64_t dst_file_size = 0;
		RanoFileInfo_getFileSize( src_file_path, &src_file_size );
		RanoFileInfo_getFileSize( dst_file_path, &dst_file_size );
		if( src_file_size == dst_file_size ){
			if( ermsg ){
				ZnkStr_addf( ermsg, "EstDirUtil_checkDuplicateSameSizeFiles : dst_file [%s] is already exist (and file size is same).\n", dst_file_path );
			}
			result = true;
		} else {
			if( ermsg ){
				ZnkStr_addf( ermsg, "EstDirUtil_checkDuplicateSameSizeFiles : dst_file [%s] is already exist (and file size is different).\n", dst_file_path );
			}
			result = false;
		}
	}
	return result;
}

bool
EstDirUtil_moveFile( const char* src_file, const char* dst_dir, const char* renamed_filename, ZnkStr ermsg, ZnkStr dst_file_path,
		bool is_src_delete )
{
	bool result = false;
	const char* p = NULL;

	if( ZnkS_empty(renamed_filename) ){
		/* src_file のtailを取得 */
		p = Znk_strrchr( src_file, '/' );
		if( p ){
			++p;
		} else {
			p = src_file;
		}
		ZnkStr_setf( dst_file_path, "%s/%s", dst_dir, p );

		/* dst_file_path の directory部は、この場合dst_dirに等しい */
		ZnkDir_mkdirPath( dst_dir, Znk_NPOS, '/', NULL );
	} else {
		ZnkStr_setf( dst_file_path, "%s/%s", dst_dir, renamed_filename );

		/* dst_file_path の directory部を取得 */
		p = ZnkStrEx_strrchr( dst_file_path, '/' );
		if( p ){
			ZnkDir_mkdirPath( ZnkStr_cstr(dst_file_path), p - ZnkStr_cstr(dst_file_path), '/', ermsg );
		}
	}

	if( ZnkS_eq( src_file, ZnkStr_cstr(dst_file_path) ) ){
		if( ermsg ){
			ZnkStr_addf( ermsg, "EstDirUtil_moveFile : [%s] to [%s] failure(same path).\n", src_file, ZnkStr_cstr(dst_file_path) );
		}
		return false;
	}

	/***
	 * ここで与えられる src_file, dst_file_path は、同名のファイルである.
	 * さらにこれらがファイルシステム上においては同一ではなく、ファイルサイズが一致する場合は
	 * 内容が同じファイルが重複しているとみなす.
	 */
	if( EstDirUtil_checkDuplicateSameSizeFiles( src_file, ZnkStr_cstr(dst_file_path), ermsg ) ){
		if( is_src_delete ){
			if( ZnkDir_deleteFile_byForce( src_file ) ){
				if( ermsg ){
					ZnkStr_addf( ermsg, "EstDirUtil_moveFile : duplicated src_file [%s] is removed.\n", src_file );
				}
				return true;
			}
		}
		/* conflict */
		return false;
	}

	if( ZnkDir_copyFile_byForce( src_file, ZnkStr_cstr(dst_file_path), ermsg ) ){
		if( ZnkDir_deleteFile_byForce( src_file ) ){
			if( ermsg ){
				ZnkStr_addf( ermsg, "EstDirUtil_moveFile : [%s] to [%s] OK.\n", src_file, ZnkStr_cstr(dst_file_path) );
			}
			result = true;
		} else {
			if( ermsg ){
				ZnkStr_addf( ermsg, "EstDirUtil_moveFile : [%s] to [%s] failure(delete).\n", src_file, ZnkStr_cstr(dst_file_path) );
			}
			result = false;
		}
	} else {
		if( ermsg ){
			ZnkStr_addf( ermsg, "EstDirUtil_moveFile : [%s] to [%s] failure(copy).\n", src_file, ZnkStr_cstr(dst_file_path) );
		}
		result = false;
	}
	return result;
}

typedef struct {
	const char*     title_;
	const char*     dst_topdir_;
	ZnkStr          ermsg_;
	EstDirUtilFuncT is_processFile_func_;
	void*           is_processFile_arg_;
} MoveInfo;

static bool report_onEnterDir( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	const char* title    = info->title_;
	ZnkStr      ermsg    = info->ermsg_;

	if( ermsg ){
		ZnkStr_addf( ermsg, "%s : onEnterDir : [%s]\n", title, top_dir );
	}
	return true;
}
static bool move_processFile( ZnkDirRecursive recur, const char* file_path, void* arg, size_t local_err_num )
{
	bool result = false;
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	const char* title    = info->title_;
	ZnkStr      ermsg    = info->ermsg_;

	if( info->is_processFile_func_ == NULL || info->is_processFile_func_( file_path, info->is_processFile_arg_ ) ){
		char dst_file_path[ 256 ] = "";
		const char* q = NULL;
		Znk_snprintf( dst_file_path, sizeof(dst_file_path), "%s/%s", info->dst_topdir_, file_path );
		q = Znk_strrchr( dst_file_path, '/' );
		if( q ){
			char dst_dir_path[ 256 ] = "";
			ZnkS_copy( dst_dir_path, sizeof(dst_dir_path), dst_file_path, q-dst_file_path );
			ZnkDir_mkdirPath( dst_dir_path, Znk_NPOS, '/', NULL );
			if( ZnkDir_copyFile_byForce( file_path, dst_file_path, ermsg ) ){
				if( ZnkDir_deleteFile_byForce( file_path ) ){
					if( ermsg ){
						ZnkStr_addf( ermsg, "%s : moveFile : [%s] to [%s] OK.\n", title, file_path, dst_dir_path );
					}
					result = true;
				} else {
					if( ermsg ){
						ZnkStr_addf( ermsg, "%s : moveFile : [%s] to [%s] failure.\n", title, file_path, dst_dir_path );
					}
					result = false;
				}
			}
		}
	}

	return result;
}
static bool remove_processFile( ZnkDirRecursive recur, const char* file_path, void* arg, size_t local_err_num )
{
	bool result = false;
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	const char* title = info->title_;
	ZnkStr      ermsg = info->ermsg_;

	if( info->is_processFile_func_ == NULL || info->is_processFile_func_( file_path, info->is_processFile_arg_ ) ){
		if( ZnkDir_deleteFile_byForce( file_path ) ){
			if( ermsg ){
				ZnkStr_addf( ermsg, "%s : deleteFile : [%s] OK.\n", title, file_path );
			}
			result = true;
		} else {
			if( ermsg ){
				ZnkStr_addf( ermsg, "%s : deleteFile : [%s] failure.\n", title, file_path );
			}
			result = false;
		}
	}

	return result;
}
static bool rmdir_onExitDir( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	bool result = false;
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	ZnkStr      ermsg = info->ermsg_;
	const char* title = info->title_;

	if( local_err_num == 0 ){
		if( ZnkDir_rmdir( top_dir ) ){
			if( ermsg ){
				ZnkStr_addf( ermsg, "%s : rmdir : [%s] OK.\n", title, top_dir );
			}
			result = true;
		} else {
			if( ermsg ){
				ZnkStr_addf( ermsg, "%s : rmdir : [%s] failure.\n", title, top_dir );
			}
			result = false;
		}
	} else {
		if( ermsg ){
			ZnkStr_addf( ermsg, "%s : rmdir_onExitDir : local_err_num = [%zu].\n", title, local_err_num );
		}
		result = false;
	}
	return result;
}

void
EstDirUtil_moveDir( const char* src_dir, const char* dst_dir,
		const char* title, ZnkStr ermsg,
		EstDirUtilFuncT is_processFile_func, void* is_processFile_arg )
{
	bool is_err_ignore = true;
	MoveInfo info = { 0 };
	ZnkDirRecursive recur = NULL;

	info.title_      = title;
	info.dst_topdir_ = dst_dir;
	info.ermsg_      = ermsg;
	info.is_processFile_func_ = is_processFile_func;
	info.is_processFile_arg_  = is_processFile_arg;
	
	recur = ZnkDirRecursive_create( is_err_ignore,
			report_onEnterDir,&info,
			move_processFile, &info,
			rmdir_onExitDir,  &info );
	ZnkDirRecursive_traverse( recur, src_dir, NULL );
	ZnkDirRecursive_destroy( recur );
}

void
EstDirUtil_removeDir( const char* topdir,
		const char* title, ZnkStr ermsg,
		EstDirUtilFuncT is_processFile_func, void* is_processFile_arg )
{
	bool is_err_ignore = true;
	MoveInfo info = { 0 };
	ZnkDirRecursive recur = NULL;

	info.title_      = title;
	info.dst_topdir_ = topdir;
	info.ermsg_      = ermsg;
	info.is_processFile_func_ = is_processFile_func;
	info.is_processFile_arg_  = is_processFile_arg;
	
	recur = ZnkDirRecursive_create( is_err_ignore,
			report_onEnterDir,&info,
			remove_processFile, &info,
			rmdir_onExitDir,  &info );
	ZnkDirRecursive_traverse( recur, topdir, NULL );
	ZnkDirRecursive_destroy( recur );
}



typedef struct {
	size_t days_ago_;
	size_t sec_ago_;
} AgoInfo;

static bool
isOldFile( const char* file_path, void* arg )
{
	AgoInfo* info = Znk_force_ptr_cast( AgoInfo*, arg );
	ZnkDate current = { 0 };
	ZnkDate date = { 0 };
	ZnkDate threshold = { 0 };
	long    diff_sec = 0;
	bool    is_old   = false;
	size_t  days_ago = info->days_ago_;
	size_t  sec_ago  = info->sec_ago_;

	ZnkDate_getCurrent( &current );
	RanoFileInfo_getLastUpdatedTime( file_path, &date );

	if( days_ago == 0 ){
		ZnkDate_getNDaysAgo( &threshold, &current, 1 );
	
		if( ZnkDate_compareDay( &date, &threshold ) >= 0 ){
			diff_sec = ZnkDate_diffSecond( &current, &date, 1 );
			if( diff_sec >= (long)sec_ago ){
				is_old = true;
			}
		} else {
			is_old = true;
		}
	} else {
		ZnkDate_getNDaysAgo( &threshold, &current, days_ago );
		is_old = (bool)( ZnkDate_compareDay( &date, &threshold ) < 0 );
	}

	return is_old;
}

void
EstDirUtil_moveOldFile_toDustbox( const char* topdir, ZnkStr ermsg, size_t days_ago, size_t sec_ago )
{
	AgoInfo ago = { 0 };
	ago.days_ago_ = days_ago;
	ago.sec_ago_  = sec_ago;
	EstDirUtil_moveDir( topdir, "dustbox",
			"EstDirUtil_moveOldFile_toDustbox", ermsg,
			isOldFile, &ago );
}
void
EstDirUtil_removeOldFile( const char* topdir, ZnkStr ermsg, size_t days_ago, size_t sec_ago )
{
	AgoInfo ago = { 0 };
	ago.days_ago_ = days_ago;
	ago.sec_ago_  = sec_ago;
	EstDirUtil_removeDir( topdir,
			"EstDirUtil_removeOldFile", ermsg,
			isOldFile, &ago );
}
