#ifndef INCLUDE_GUARD__Rano_parent_proxy_h__
#define INCLUDE_GUARD__Rano_parent_proxy_h__

#include <Znk_myf.h>
#include <Znk_date.h>

Znk_EXTERN_C_BEGIN

bool
RanoParentProxy_isAppliedHost2( const ZnkMyf hosts, const char* goal_hostname, size_t goal_hostname_leng );
void
RanoParentProxy_set( const char* parent_proxy_hostname, uint16_t parent_proxy_port );
bool
RanoParentProxy_set_byAuthority( const char* parent_proxy_autority );
const char*
RanoParentProxy_getHostname( void );
uint16_t
RanoParentProxy_getPort( void );
void
RanoParentProxy_setIndicatingMode( int indicating_mode );

ZnkStrAry
RanoParentProxy_getParentProxys( void );
ZnkDate
RanoParentProxy_getDate( void );
bool
RanoParentProxy_loadCandidate( const char* filename );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
