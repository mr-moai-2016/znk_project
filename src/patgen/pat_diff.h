#ifndef INCLUDE_GUARD__pat_diff_h__
#define INCLUDE_GUARD__pat_diff_h__

#include <Znk_myf.h>

Znk_EXTERN_C_BEGIN

typedef bool (*EstDirUtilFuncT)( const char* file_path, void* arg );

void
Pat_diff( const char* src_dir, const char* dst_dir,
		const char* title, ZnkMyf config, ZnkStr ermsg,
		EstDirUtilFuncT is_processFile_func, void* is_processFile_arg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
