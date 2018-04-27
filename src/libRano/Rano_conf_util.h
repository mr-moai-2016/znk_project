#ifndef INCLUDE_GUARD__Rano_conf_util_h__
#define INCLUDE_GUARD__Rano_conf_util_h__

#include <Znk_str_ary.h>

Znk_EXTERN_C_BEGIN

bool
RanoConfUtil_initByStrAry_withEnvVar( ZnkStr ans, ZnkStrAry sda, size_t default_idx, size_t envvar_idx,
		size_t dst_begin, size_t dst_size );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
