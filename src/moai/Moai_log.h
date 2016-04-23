#ifndef INCLUDE_GUARD__Moai_log_h__
#define INCLUDE_GUARD__Moai_log_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

void
MoaiLog_open( const char* filename, bool keep_open );
void
MoaiLog_close( void );
void
MoaiLog_printf( const char* fmt, ... );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
