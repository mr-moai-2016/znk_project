#ifndef INCLUDE_GUARD__Rano_file_info_h__
#define INCLUDE_GUARD__Rano_file_info_h__

#include <Znk_date.h>

Znk_EXTERN_C_BEGIN

bool
RanoFileInfo_getLastUpdatedTime( const char* filepath, ZnkDate* date );

bool
RanoFileInfo_getFileSize( const char* filepath, uint64_t* ans_size );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
