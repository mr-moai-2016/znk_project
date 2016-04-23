#ifndef INCLUDE_GUARD__Moai_parent_proxy_h__
#define INCLUDE_GUARD__Moai_parent_proxy_h__

#include <Znk_myf.h>

Znk_EXTERN_C_BEGIN

bool
MoaiParentProxy_isUse( const ZnkMyf config, const char* goal_hostname, size_t goal_hostname_leng );
void
MoaiParentProxy_set( const char* parent_proxy_hostname, uint16_t parent_proxy_port );
const char*
MoaiParentProxy_getHostname( void );
uint16_t
MoaiParentProxy_getPort( void );
void
MoaiParentProxy_setIndicatingMode( int indicating_mode );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
