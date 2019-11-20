#ifndef INCLUDE_GUARD__Rano_conf_util_h__
#define INCLUDE_GUARD__Rano_conf_util_h__

#include <Znk_str_ary.h>
#include <Znk_myf.h>

Znk_EXTERN_C_BEGIN

bool
RanoConfUtil_initByStrAry_withEnvVar( ZnkStr ans, ZnkStrAry sda, size_t default_idx, size_t envvar_idx,
		size_t dst_begin, size_t dst_size );

void
RanoConfUtil_setupConfigDir( ZnkStr ans, const char* given_dir, const char* base_dir, const char* when_empty_dir );

ZnkMyf
RanoConfUtil_rano_app_myf( void );
bool
RanoConfUtil_rano_app_initiate( const char* base_dir, bool is_binary_mode, ZnkStr ermsg );
void
RanoConfUtil_rano_app_finalize( void );

const char*
RanoConfUtil_moai_dir( ZnkStr ermsg );
bool
RanoConfUtil_moai_dir_initiate( ZnkStr ermsg );
void
RanoConfUtil_moai_dir_finalize( void );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
