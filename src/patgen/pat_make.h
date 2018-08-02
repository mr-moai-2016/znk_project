#ifndef INCLUDE_GUARD__pat_make_h__
#define INCLUDE_GUARD__pat_make_h__

#include <Znk_myf.h>

Znk_EXTERN_C_BEGIN

size_t
Pat_make( const char* pmk_file, const char* common_dir, const char* platform_dir, const char* platform_id, ZnkMyf config, ZnkStr ermsg );

void
Pat_marge( const char* old_file, const char* new_file, const char* save_file );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
