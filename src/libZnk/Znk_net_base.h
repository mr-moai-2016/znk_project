#ifndef INCLUDE_GUARD__Znk_net_base_h__
#define INCLUDE_GUARD__Znk_net_base_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

bool
ZnkNetBase_initiate( bool verbose );

void
ZnkNetBase_finalize( void );

int
ZnkNetBase_getLastErrCode( void );

const char*
ZnkNetBase_getErrKey( int err_code );

void
ZnkNetBase_getErrMsg( char* errmsg_buf, size_t errmsg_buf_size, int err_code );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
