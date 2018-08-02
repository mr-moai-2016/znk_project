#ifndef INCLUDE_GUARD__Bdm_base_h__
#define INCLUDE_GUARD__Bdm_base_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

ZnkStr
BdmBase_getMoaiDir( void );
bool
BdmBase_loadAuthenticKey( char* authentic_key_buf, size_t authentic_key_buf_size, const char* moai_dir );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
