#include "pat_diff.h"
#include <Znk_dir.h>
#include <Znk_dir_recursive.h>
#include <Znk_missing_libc.h>
#include <Znk_s_base.h>
#include <Znk_str_path.h>
#include <Znk_str_ary.h>
#include <Znk_md5.h>
#include <Rano_file_info.h>

typedef struct {
	const char*     title_;
	const char*     src_topdir_;
	const char*     dst_topdir_;
	ZnkStr          ermsg_;
	ZnkStr          ans_;
	ZnkStrAry       ignore_list_;
	EstDirUtilFuncT is_processFile_func_;
	void*           is_processFile_arg_;
} MoveInfo;

static bool isIgnoreDir_S2D( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	ZnkStrAry ignore_list = info->ignore_list_;
	size_t src_topdir_leng = Znk_strlen( info->src_topdir_ );
	const char* under_path = top_dir + src_topdir_leng;

	if( under_path[ 0 ] == '/' ){ ++under_path; }
	if( ignore_list && ZnkStrAry_find_isMatch( ignore_list, 0, under_path, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
		/* skip */
		return true;
	}
	return false;
}
static bool isIgnoreDir_D2S( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	ZnkStrAry ignore_list = info->ignore_list_;
	size_t dst_topdir_leng = Znk_strlen( info->dst_topdir_ );
	const char* under_path = top_dir + dst_topdir_leng;

	if( under_path[ 0 ] == '/' ){ ++under_path; }
	if( ignore_list && ZnkStrAry_find_isMatch( ignore_list, 0, under_path, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
		/* skip */
		return true;
	}
	return false;
}

static bool report_onEnterDir_S2D( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
#if 0
	MoveInfo* info  = Znk_force_ptr_cast( MoveInfo*, arg );
	ZnkStr    ermsg = info->ermsg_;
	ZnkStr    ans   = info->ans_;
	size_t src_topdir_leng = Znk_strlen( info->src_topdir_ );
	const char* under_path = top_dir + src_topdir_leng;
	char dst_path[ 256 ] = "";
	bool deleted = false;
	Znk_UNUSED( ermsg );

	if( under_path[ 0 ] == '/' ){ ++under_path; }
	Znk_snprintf( dst_path, sizeof(dst_path), "%s/%s", info->dst_topdir_, under_path );
	if( ZnkDir_getType(dst_path) == ZnkDirType_e_File ){
		/* TODO : この場合はdirectoryをdeletedし更にファイルを追加する必要がある */
		deleted = true;
	} else if( ZnkDir_getType(dst_path) != ZnkDirType_e_Directory ){
		deleted = true;
	} 

	if( ans ){
		if( deleted ){
			ZnkStr_addf( ans, "rmdir  ['%s']\n", under_path );
		}
	}
#endif
	return true;
}
static bool report_onEnterDir_D2S( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	ZnkStr    ermsg = info->ermsg_;
	ZnkStr    ans   = info->ans_;
	size_t dst_topdir_leng = Znk_strlen( info->dst_topdir_ );
	const char* under_path = top_dir + dst_topdir_leng;
	char src_path[ 256 ] = "";
	bool added = false;
	Znk_UNUSED( ermsg );

	if( under_path[ 0 ] == '/' ){ ++under_path; }
	Znk_snprintf( src_path, sizeof(src_path), "%s/%s", info->src_topdir_, under_path );
	if( ZnkDir_getType(src_path) == ZnkDirType_e_File ){
		/* TODO : この場合はdirectoryをaddedし更にファイルをdeleteする必要がある */
		added = true;
	} else if( ZnkDir_getType(src_path) != ZnkDirType_e_Directory ){
		added = true;
	}

	if( ans ){
		//Znk_printf( "%s : onEnterDir_D2S : [%s] under_path=[%s]\n", title, top_dir, under_path );
		if( added ){
			ZnkStr_addf( ans, "mkdir  ['%s']\n", under_path );
		}
	}
	return true;
}
static bool
isEqDigest( const ZnkMD5_CTX* md5ctx1, const ZnkMD5_CTX* md5ctx2 )
{
	const uint8_t* digest1 = ZnkMD5_digest( md5ctx1 );
	const uint8_t* digest2 = ZnkMD5_digest( md5ctx2 );
	size_t idx;
	for( idx=0; idx<ZnkMD5_DIGEST_SIZE; ++idx ){
		if( digest1[ idx ] != digest2[ idx ] ){
			return false;
		}
	}
	return true;
}
static bool processFile_S2D( ZnkDirRecursive recur, const char* file_path, void* arg, size_t local_err_num )
{
	bool result = true;
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	ZnkStr    ermsg = info->ermsg_;
	ZnkStr    ans   = info->ans_;
	ZnkStrAry ignore_list = info->ignore_list_;
	Znk_UNUSED( ermsg );

	if( info->is_processFile_func_ == NULL || info->is_processFile_func_( file_path, info->is_processFile_arg_ ) ){
		size_t src_topdir_leng = Znk_strlen( info->src_topdir_ );
		const char* under_path = file_path + src_topdir_leng;
		char src_path[ 256 ] = "";
		char dst_path[ 256 ] = "";
		ZnkDate  src_date = { 0 };
		uint64_t src_size = 0;
		ZnkDate  dst_date = { 0 };
		uint64_t dst_size = 0;
		bool     updated = false;
		bool     deleted = false;

		if( under_path[ 0 ] == '/' ){ ++under_path; }
		if( ignore_list && ZnkStrAry_find_isMatch( ignore_list, 0, under_path, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
			/* skip */
			return true;
		}

		{
			Znk_snprintf( src_path, sizeof(src_path), "%s/%s", info->src_topdir_, under_path );
			RanoFileInfo_getLastUpdatedTime( src_path, &src_date );
			RanoFileInfo_getFileSize( src_path, &src_size );
		}
		{
			Znk_snprintf( dst_path, sizeof(dst_path), "%s/%s", info->dst_topdir_, under_path );
			if( ZnkDir_getType(dst_path) == ZnkDirType_e_File ){
				RanoFileInfo_getLastUpdatedTime( dst_path, &dst_date );
				RanoFileInfo_getFileSize( dst_path, &dst_size );
			} else if( ZnkDir_getType(dst_path) == ZnkDirType_e_Directory ){
				/* TODO : 同名のディレクトリが存在する場合 */
			} else {
				/* srcにあってdstにはない. */
				deleted = true;
			}
		}

#if 0
		ZnkStr_addf( ermsg, "src_date : [" );
		ZnkDate_getStr( ermsg, Znk_NPOS, &src_date, ZnkDateStr_e_Std );
		ZnkStr_addf( ermsg, "] " );
		ZnkStr_addf( ermsg, "src_size : [%zu]\n", src_size );

		ZnkStr_addf( ermsg, "dst_date : [" );
		ZnkDate_getStr( ermsg, Znk_NPOS, &dst_date, ZnkDateStr_e_Std );
		ZnkStr_addf( ermsg, "] " );
		ZnkStr_addf( ermsg, "dst_size : [%zu]\n", dst_size );
#endif

		if( src_size != dst_size ){
			/* 更新ありとみなす. */
			updated = true;
		} else if( ZnkDate_eq( &src_date, &dst_date ) ){
			/* 簡易判定 : 更新のない同じファイルとみなす */
			updated = false;
		} else {
			/* 更新日時は異なるが、sizeは同じ場合. MD5判定も行う. */
			ZnkMD5_CTX md5ctx_src = {{ 0 }};
			ZnkMD5_CTX md5ctx_dst = {{ 0 }};
			ZnkMD5_getDigest_byFile( &md5ctx_src, src_path );
			ZnkMD5_getDigest_byFile( &md5ctx_dst, dst_path );
			updated = !isEqDigest( &md5ctx_src, &md5ctx_dst );
		}

		if( deleted ){
			ZnkStr_addf( ans, "delete ['%s']\n", under_path );
		} else if( updated ){
			ZnkStr_addf( ans, "update ['%s']\n", under_path );
		}
	}

	return result;
}
static bool processFile_D2S( ZnkDirRecursive recur, const char* file_path, void* arg, size_t local_err_num )
{
	bool result = true;
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	ZnkStr    ermsg = info->ermsg_;
	ZnkStr    ans   = info->ans_;
	ZnkStrAry ignore_list = info->ignore_list_;
	Znk_UNUSED( ermsg );

	if( info->is_processFile_func_ == NULL || info->is_processFile_func_( file_path, info->is_processFile_arg_ ) ){
		size_t dst_topdir_leng = Znk_strlen( info->dst_topdir_ );
		const char* under_path = file_path + dst_topdir_leng;
		char src_path[ 256 ] = "";
		char dst_path[ 256 ] = "";
		ZnkDate  dst_date = { 0 };
		uint64_t dst_size = 0;
		bool     added = false;

		if( under_path[ 0 ] == '/' ){ ++under_path; }
		if( ignore_list && ZnkStrAry_find_isMatch( ignore_list, 0, under_path, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
			/* skip */
			return true;
		}

		{
			Znk_snprintf( dst_path, sizeof(dst_path), "%s/%s", info->dst_topdir_, under_path );
			RanoFileInfo_getLastUpdatedTime( dst_path, &dst_date );
			RanoFileInfo_getFileSize( dst_path, &dst_size );
		}
		{
			Znk_snprintf( src_path, sizeof(src_path), "%s/%s", info->src_topdir_, under_path );
			if( ZnkDir_getType(src_path) == ZnkDirType_e_File ){
			} else if( ZnkDir_getType(src_path) == ZnkDirType_e_Directory ){
				/* TODO : 同名のディレクトリが存在する場合 */
			} else {
				/* dstにあってsrcにはない. */
				added = true;
			}
		}

		if( added ){
			ZnkStr_addf( ans, "add    ['%s']\n", under_path );
		}
	}

	return result;
}
static bool onExitDir( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	bool result = true;
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	ZnkStr    ermsg = info->ermsg_;
	ZnkStr    ans   = info->ans_;
	const char* title = info->title_;
	Znk_UNUSED( ans );

	if( local_err_num == 0 ){
		if( ermsg ){
			//Znk_printf_e( "onExitDir : [%s] OK.\n", top_dir );
		}
	} else {
		if( ermsg ){
			ZnkStr_addf( ermsg, "%s : onExitDir : local_err_num = [%zu].\n", title, local_err_num );
		}
		result = false;
	}
	return result;
}
static bool onExitDir_S2D( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	MoveInfo* info  = Znk_force_ptr_cast( MoveInfo*, arg );
	ZnkStr    ermsg = info->ermsg_;
	ZnkStr    ans   = info->ans_;
	size_t src_topdir_leng = Znk_strlen( info->src_topdir_ );
	const char* under_path = top_dir + src_topdir_leng;
	char dst_path[ 256 ] = "";
	bool deleted = false;
	Znk_UNUSED( ermsg );

	if( under_path[ 0 ] == '/' ){ ++under_path; }
	Znk_snprintf( dst_path, sizeof(dst_path), "%s/%s", info->dst_topdir_, under_path );
	if( ZnkDir_getType(dst_path) == ZnkDirType_e_File ){
		/* TODO : この場合はdirectoryをdeletedし更にファイルを追加する必要がある */
		deleted = true;
	} else if( ZnkDir_getType(dst_path) != ZnkDirType_e_Directory ){
		deleted = true;
	} 

	if( ans ){
		if( deleted ){
			ZnkStr_addf( ans, "rmdir  ['%s']\n", under_path );
		}
	}
	return true;
}

void
Pat_diff( const char* src_dir, const char* dst_dir,
		const char* title, ZnkMyf config, ZnkStr ermsg,
		EstDirUtilFuncT is_processFile_func, void* is_processFile_arg )
{
	bool is_err_ignore = true;
	MoveInfo info = { 0 };
	ZnkDirRecursive recur = NULL;
	ZnkStr ans = ZnkStr_new( "" );

	if( ZnkDir_getType( src_dir ) != ZnkDirType_e_Directory ){
		goto FUNC_END;
	}
	info.title_       = title;
	info.src_topdir_  = src_dir;
	info.dst_topdir_  = dst_dir;
	info.ermsg_       = ermsg;
	info.ans_         = ans;
	info.ignore_list_ = ZnkMyf_find_lines( config, "ignore_list" );
	info.is_processFile_func_ = is_processFile_func;
	info.is_processFile_arg_  = is_processFile_arg;

	Znk_printf( "# src_dir=[%s]\n", src_dir );
	Znk_printf( "set_new_dir ['%s']\n", dst_dir );
	
	/***
	 * srcからのscan
	 * updateとdeleted の検出.
	 */
	recur = ZnkDirRecursive_create( is_err_ignore,
			report_onEnterDir_S2D, &info,
			processFile_S2D, &info,
			onExitDir_S2D,  &info );
	ZnkDirRecursive_config_isIgnoreDir( recur, isIgnoreDir_S2D, &info );
	ZnkDirRecursive_traverse( recur, src_dir, NULL );
	ZnkDirRecursive_destroy( recur );

	/***
	 * dstからのscan
	 * added の検出.
	 */
	recur = ZnkDirRecursive_create( is_err_ignore,
			report_onEnterDir_D2S, &info,
			processFile_D2S, &info,
			onExitDir,  &info );
	ZnkDirRecursive_config_isIgnoreDir( recur, isIgnoreDir_D2S, &info );
	ZnkDirRecursive_traverse( recur, dst_dir, NULL );
	ZnkDirRecursive_destroy( recur );

FUNC_END:
	Znk_fputs( ZnkStr_cstr(ans), Znk_stdout() );
	ZnkStr_delete( ans );
}


