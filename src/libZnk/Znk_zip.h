#ifndef INCLUDE_GUARD__Znk_zip_h__
#define INCLUDE_GUARD__Znk_zip_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

typedef bool (*ZnkZipProgressFuncT)( const char* msg, size_t num_of_entry, size_t idx_of_entry, void* param );

bool
ZnkZip_extract( const char* zipfilename, ZnkStr ermsg, ZnkZipProgressFuncT progress_func, void* param, const char* dst_dir );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
