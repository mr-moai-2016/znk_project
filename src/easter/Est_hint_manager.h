#ifndef INCLUDE_GUARD__Est_hint_manager_h__
#define INCLUDE_GUARD__Est_hint_manager_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

bool
EstHint_initiate( void );
ZnkStr
EstHint_getHintTable( const char* cat_name );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
