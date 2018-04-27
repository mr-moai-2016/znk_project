#ifndef INCLUDE_GUARD__Rano_log_h__
#define INCLUDE_GUARD__Rano_log_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

void
RanoLog_open( const char* filename, bool keep_open, bool additional );
void
RanoLog_close( void );
void
RanoLog_printf( const char* fmt, ... );
/* ñºèÃïúãAçÏã∆ÇÃÇΩÇﬂÇµÇŒÇÁÇ≠écÇ∑ */
#define RanoLog_printf2 RanoLog_printf

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
