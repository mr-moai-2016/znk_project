#include <Rano_dir_util.h>
#include <Rano_file_info.h>

#include <Znk_s_base.h>
#include <Znk_str_ex.h>
#include <Znk_missing_libc.h>
#include <Znk_date.h>
#include <Znk_dir.h>
#include <Znk_dir_recursive.h>
#include <Znk_str_path.h>

typedef struct {
	const char* title_;
	const char* src_topdir_;
	const char* dst_topdir_;
	ZnkStr      ermsg_;

	RanoDirUtilFuncT       is_processFile_func_;
	void*                  is_processFile_arg_;
	RanoDirUtilFilterFuncT filterFile_func_;
	void*                  filterFile_arg_;
} FilterInfo;

static bool onEnterDir_report( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	FilterInfo* info = Znk_force_ptr_cast( FilterInfo*, arg );
	const char* title    = info->title_;
	ZnkStr      ermsg    = info->ermsg_;

	if( ermsg ){
		ZnkStr_addf( ermsg, "%s : onEnterDir : [%s]\n", title, top_dir );
	}
	return true;
}
static bool processFile_move( ZnkDirRecursive recur, const char* file_path, void* arg, size_t local_err_num )
{
	bool result = false;
	FilterInfo* info = Znk_force_ptr_cast( FilterInfo*, arg );
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
static bool processFile_remove( ZnkDirRecursive recur, const char* file_path, void* arg, size_t local_err_num )
{
	bool result = false;
	FilterInfo* info = Znk_force_ptr_cast( FilterInfo*, arg );
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
static bool onExitDir( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	bool result = false;
	FilterInfo* info = Znk_force_ptr_cast( FilterInfo*, arg );
	ZnkStr      ermsg = info->ermsg_;
	const char* title = info->title_;

	if( local_err_num == 0 ){
		result = true;
	} else {
		if( ermsg ){
			ZnkStr_addf( ermsg, "%s : onExitDir : local_err_num = [%zu].\n", title, local_err_num );
		}
		result = false;
	}
	return result;
}
static bool onExitDir_rmdir( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	bool result = false;
	FilterInfo* info = Znk_force_ptr_cast( FilterInfo*, arg );
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
			ZnkStr_addf( ermsg, "%s : onExitDir_rmdir : local_err_num = [%zu].\n", title, local_err_num );
		}
		result = false;
	}
	return result;
}

void
RanoDirUtil_moveDir( const char* src_dir, const char* dst_dir,
		const char* title, ZnkStr ermsg,
		RanoDirUtilFuncT is_processFile_func, void* is_processFile_arg )
{
	bool is_err_ignore = true;
	FilterInfo info = { 0 };
	ZnkDirRecursive recur = NULL;

	info.title_      = title;
	info.dst_topdir_ = dst_dir;
	info.ermsg_      = ermsg;
	info.is_processFile_func_ = is_processFile_func;
	info.is_processFile_arg_  = is_processFile_arg;
	
	recur = ZnkDirRecursive_create( is_err_ignore,
			onEnterDir_report,&info,
			processFile_move, &info,
			onExitDir_rmdir,  &info );
	ZnkDirRecursive_traverse( recur, src_dir, NULL );
	ZnkDirRecursive_destroy( recur );
}

void
RanoDirUtil_removeDir( const char* topdir,
		const char* title, ZnkStr ermsg,
		RanoDirUtilFuncT is_processFile_func, void* is_processFile_arg )
{
	bool is_err_ignore = true;
	FilterInfo info = { 0 };
	ZnkDirRecursive recur = NULL;

	info.title_      = title;
	info.dst_topdir_ = topdir;
	info.ermsg_      = ermsg;
	info.is_processFile_func_ = is_processFile_func;
	info.is_processFile_arg_  = is_processFile_arg;
	
	recur = ZnkDirRecursive_create( is_err_ignore,
			onEnterDir_report,  &info,
			processFile_remove, &info,
			onExitDir_rmdir,    &info );
	ZnkDirRecursive_traverse( recur, topdir, NULL );
	ZnkDirRecursive_destroy( recur );
}

static bool processFile_filter( ZnkDirRecursive recur, const char* file_path, void* arg, size_t local_err_num )
{
	bool result = false;
	FilterInfo* info = Znk_force_ptr_cast( FilterInfo*, arg );
	const char* title    = info->title_;
	ZnkStr      ermsg    = info->ermsg_;

	if( info->is_processFile_func_ == NULL || info->is_processFile_func_( file_path, info->is_processFile_arg_ ) ){
		char dst_file_path[ 256 ] = "";
		const char* q = NULL;
		const char* r = file_path + Znk_strlen(info->src_topdir_);
		while( *r == '/' ){ ++r; }
		Znk_snprintf( dst_file_path, sizeof(dst_file_path), "%s/%s", info->dst_topdir_, r );
		q = Znk_strrchr( dst_file_path, '/' );
		if( q ){
			char dst_dir_path[ 256 ] = "";
			ZnkS_copy( dst_dir_path, sizeof(dst_dir_path), dst_file_path, q-dst_file_path );
			ZnkDir_mkdirPath( dst_dir_path, Znk_NPOS, '/', NULL );
			if( info->filterFile_func_( file_path, dst_file_path, info->filterFile_arg_ ) ){
				if( ermsg ){
					ZnkStr_addf( ermsg, "%s : filterFile : [%s] to [%s] OK.\n", title, file_path, dst_dir_path );
				}
				result = true;
			}
		}
	}
	return result;
}

void
RanoDirUtil_filterDir( const char* src_dir, const char* dst_dir,
		const char* title, ZnkStr ermsg,
		RanoDirUtilFuncT is_processFile_func, void* is_processFile_arg,
		RanoDirUtilFilterFuncT filterFile_func, void* filterFile_arg )
{
	bool is_err_ignore = true;
	FilterInfo info = { 0 };
	ZnkDirRecursive recur = NULL;

	info.title_      = title;
	info.src_topdir_ = src_dir;
	info.dst_topdir_ = dst_dir;
	info.ermsg_      = ermsg;
	info.is_processFile_func_ = is_processFile_func;
	info.is_processFile_arg_  = is_processFile_arg;
	info.filterFile_func_     = filterFile_func;
	info.filterFile_arg_      = filterFile_arg;
	
	recur = ZnkDirRecursive_create( is_err_ignore,
			onEnterDir_report,&info,
			processFile_filter, &info,
			onExitDir,  &info );
	ZnkDirRecursive_traverse( recur, src_dir, NULL );
	ZnkDirRecursive_destroy( recur );
}

bool
RanoDirUtil_mkdirOfTailContain( const char* path, ZnkStr ermsg )
{
	bool result = false;
	size_t tail_pos = ZnkStrPath_getTailPos( path );
	ZnkStr head_dir = ZnkStr_new( "" );
	ZnkDirType dir_type = ZnkDirType_e_NotFound;

	ZnkStr_assign( head_dir, 0, path, tail_pos );
	dir_type = ZnkDir_getType( ZnkStr_cstr(head_dir) );
	switch( dir_type ){
	case ZnkDirType_e_File:
		result = false;
		break;
	case ZnkDirType_e_Directory:
		result = true;
		break;
	default:
		result = ZnkDir_mkdirPath( ZnkStr_cstr(head_dir), ZnkStr_leng(head_dir), '/', ermsg );
		break;
	}
	ZnkStr_delete( head_dir );
	return result;
}

