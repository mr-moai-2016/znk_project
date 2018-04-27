#ifndef INCLUDE_GUARD__Mkf_seek_h__
#define INCLUDE_GUARD__Mkf_seek_h__

#include <Znk_str_ary.h>

Znk_EXTERN_C_BEGIN

typedef bool (*MkfSeekFuncT_isInterestExt)( const char* ext );

bool
MkfSeek_listDir( ZnkStrAry list, ZnkStrAry dir_list, const char* dir, ZnkStrAry ignore_list,
		MkfSeekFuncT_isInterestExt is_interest_ext );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
