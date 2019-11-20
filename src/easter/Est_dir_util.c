#include <Est_dir_util.h>
#include <Rano_file_info.h>
#include <Rano_dir_util.h>

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
	RanoDirUtil_moveDir( topdir, "dustbox",
			"EstDirUtil_moveOldFile_toDustbox", ermsg,
			isOldFile, &ago );
}
void
EstDirUtil_removeOldFile( const char* topdir, ZnkStr ermsg, size_t days_ago, size_t sec_ago )
{
	AgoInfo ago = { 0 };
	ago.days_ago_ = days_ago;
	ago.sec_ago_  = sec_ago;
	RanoDirUtil_removeDir( topdir,
			"EstDirUtil_removeOldFile", ermsg,
			isOldFile, &ago );
}
