#ifndef INCLUDE_GUARD__Moai_server_info_h__
#define INCLUDE_GUARD__Moai_server_info_h__

#include <Rano_module_ary.h>
#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

const char*
MoaiServerInfo_server_name( void );
void
MoaiServerInfo_set_server_name( const char* server_name );

const char*
MoaiServerInfo_filters_dir( void );
void
MoaiServerInfo_set_filters_dir( const char* filters_dir );
const char*
MoaiServerInfo_profile_dir( void );
void
MoaiServerInfo_set_profile_dir( const char* profile_dir );

uint16_t
MoaiServerInfo_port( void );
void
MoaiServerInfo_set_port( uint16_t port );

bool
MoaiServerInfo_is_enable_log_file( void );
void
MoaiServerInfo_set_enable_log_file( bool enable );

bool
MoaiServerInfo_is_enable_log_verbose( void );
void
MoaiServerInfo_set_enable_log_verbose( bool enable );

uint32_t
MoaiServerInfo_private_ip( void );
void
MoaiServerInfo_set_private_ip( uint32_t private_ip );

const char*
MoaiServerInfo_XhrDMZ( void );
uint16_t
MoaiServerInfo_XhrDMZPort( void );
void
MoaiServerInfo_setXhrDMZPort( uint16_t port );
const char*
MoaiServerInfo_XhrAuthHost( void );
void
MoaiServerInfo_setXhrDMZ( const char* xhr_dmz );

const char*
MoaiServerInfo_acceptable_host( void );
void
MoaiServerInfo_set_acceptable_host( const char* acceptable_host );

bool
MoaiServerInfo_you_need_to_restart_moai( void );
void
MoaiServerInfo_set_you_need_to_restart_moai( bool you_need_to_restart_moai );

RanoModuleAry 
MoaiServerInfo_mod_ary( void );

void
MoaiServerInfo_initiate_mod_ary( void );
void
MoaiServerInfo_finalize_mod_ary( void );

void
MoaiServerInfo_autosetServerName( void );

void
MoaiServerInfo_reloadFilters( RanoModule mod, const char* hostname );

bool
MoaiServerInfo_isAllReplaceCookie( void );

ZnkVarpAry
MoaiServerInfo_refCookieExtraVars( void );

const char*
MoaiServerInfo_authenticKey( void );
void
MoaiServerInfo_set_authenticKey( const char* authentic_key );

const char*
MoaiServerInfo_version( bool is_load_vtag );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
