#ifndef INCLUDE_GUARD__Znk_rgx_base_h__
#define INCLUDE_GUARD__Znk_rgx_base_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef union ZnkUChar64_tag {
	uint64_t u64_; /* 初期化子で64bit整数での指定ができるようにこれを第１番目のメンバーにする */
	char     s_[ 8 ];
} ZnkUChar64;

#define ZnkRgx_DOT   UINT64_C(0xffffffffffffffff)
/* ZnkRgx_EMPTYはε遷移を表す */
#define ZnkRgx_EMPTY UINT64_C(0xfffffffffffffffe)

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */

