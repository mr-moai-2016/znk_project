#ifndef INCLUDE_GUARD__Rano_dir_util_h__
#define INCLUDE_GUARD__Rano_dir_util_h__

#include <Znk_str.h>
#include <Znk_dir_recursive.h>

Znk_EXTERN_C_BEGIN

typedef bool (*RanoDirUtilFuncT)( const char* file_path, void* arg );

typedef bool (*RanoDirUtilFilterFuncT)( const char* in_file_path, const char* out_file_path, void* arg );

/**
 * @brief
 *   src_dirディレクトリ配下を再帰的にdst_dirへ移動する.
 */
void
RanoDirUtil_moveDir( const char* src_dir, const char* dst_dir,
		const char* title, ZnkStr ermsg,
		RanoDirUtilFuncT is_processFile_func, void* is_processFile_arg );

/**
 * @brief
 *   top_dirディレクトリ配下をすべて削除する.
 */
void
RanoDirUtil_removeDir( const char* topdir,
		const char* title, ZnkStr ermsg,
		RanoDirUtilFuncT is_processFile_func, void* is_processFile_arg );

/**
 * @brief
 *   src_dirディレクトリ配下を再帰的にfilt処理し、
 *   filt処理済みの結果をdst_dirへ再帰的に出力する.
 *   (src_dirディレクトリ配下は何も変更されない)
 *
 * @param is_processFile_func:
 *   filt処理対象のファイルかどうかを判定する関数を与える.
 *   NULLを指定した場合、すべてのファイルを対象とする.
 *
 * @param is_processFile_arg:
 *   is_processFile_func へ渡す引数.
 *
 * @param filterFile_func:
 *   filt処理を実装した関数を与える.
 *   ここにNULLを指定することはできない.
 *
 * @param filterFile_arg:
 *   filterFile_func へ渡す引数.
 *
 * @param isIgnoreDir_func:
 *   filt処理の対象としたくないディレクトリ(IgnoreDir)を判定する関数を与える.
 *   NULLを指定した場合、そのようなディレクトリはないものとする.
 *   すなわちすべてのディレクトリを対象とする.
 *
 * @param isIgnoreDir_arg:
 *   isIgnoreDir_func へ渡す引数.
 */
void
RanoDirUtil_filterDir( const char* src_dir, const char* dst_dir,
		const char* title, ZnkStr ermsg,
		RanoDirUtilFuncT       is_processFile_func, void* is_processFile_arg,
		RanoDirUtilFilterFuncT filterFile_func,     void* filterFile_arg,
		ZnkDirRecursiveFuncT   isIgnoreDir_func,    void* isIgnoreDir_arg );

/**
 * @brief
 *   src_dirディレクトリ配下を再帰的にdst_dirへインストールする.
 *
 * @param is_processFile_func:
 *   copy処理対象のファイルかどうかを判定する関数を与える.
 *   NULLを指定した場合、すべてのファイルを対象とする.
 *
 * @param is_processFile_arg:
 *   is_processFile_func へ渡す引数.
 *
 * @param isIgnoreDir_func:
 *   copy処理の対象としたくないディレクトリ(IgnoreDir)を判定する関数を与える.
 *   NULLを指定した場合、そのようなディレクトリはないものとする.
 *   すなわちすべてのディレクトリを対象とする.
 *
 * @param isIgnoreDir_arg:
 *   isIgnoreDir_func へ渡す引数.
 */
void
RanoDirUtil_installDir( const char* src_dir, const char* dst_dir,
		const char* title, ZnkStr ermsg,
		RanoDirUtilFuncT     is_processFile_func, void* is_processFile_arg,
		ZnkDirRecursiveFuncT isIgnoreDir_func,    void* isIgnoreDir_arg );


/**
 * @brief
 *   pathのtail部分を包含するディレクトリ(要はtailより左側)をすべてmkdirする.
 */
bool
RanoDirUtil_mkdirOfTailContain( const char* path, ZnkStr ermsg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
