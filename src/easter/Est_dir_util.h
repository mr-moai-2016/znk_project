#ifndef INCLUDE_GUARD__Est_dir_util_h__
#define INCLUDE_GUARD__Est_dir_util_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

typedef bool (*EstDirUtilFuncT)( const char* file_path, void* arg );

/***
 * src_file_pathとdst_file_pathは(MD5の結果などから)同じ内容である可能性が濃厚であるとする.
 * この関数ではそれらがファイルシステム上は同一ではない(Duplicate)であることを確かめ、
 * さらにファイルサイズが同じであるかどうかを調べる.
 */
bool
EstDirUtil_checkDuplicateSameSizeFiles( const char* src_file_path, const char* dst_file_path, ZnkStr ermsg );

bool
EstDirUtil_moveFile( const char* src_file, const char* dst_dir, const char* renamed_filename, ZnkStr ermsg, ZnkStr dst_file_path,
		bool is_src_delete );

void
EstDirUtil_moveDir( const char* src_dir, const char* dst_dir,
		const char* title, ZnkStr ermsg,
		EstDirUtilFuncT is_processFile_func, void* is_processFile_arg );
void
EstDirUtil_removeDir( const char* topdir,
		const char* title, ZnkStr ermsg,
		EstDirUtilFuncT is_processFile_func, void* is_processFile_arg );

void
EstDirUtil_moveOldFile_toDustbox( const char* topdir, ZnkStr ermsg, size_t days_ago, size_t sec_ago );
void
EstDirUtil_removeOldFile( const char* topdir, ZnkStr ermsg, size_t days_ago, size_t sec_ago );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
