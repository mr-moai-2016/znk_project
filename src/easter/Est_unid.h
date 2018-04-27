#ifndef INCLUDE_GUARD__Est_unid_h__
#define INCLUDE_GUARD__Est_unid_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

void
EstUNID_setSessionCode_byNumber( size_t number );
void
EstUNID_setSessionCode_byStr( const char* str );
const char*
EstUNID_getSessionCode( void );
char*
EstUNID_issue( char* buf, size_t buf_size );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
