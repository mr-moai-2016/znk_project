#include "Moai_parent_proxy.h"
#include <Znk_str_dary.h>
#include <Znk_s_base.h>

static const char* st_proxy_hostname = NULL;
static uint16_t    st_proxy_port = 0;
static int         st_proxy_indicating_mode = -1;

bool
MoaiParentProxy_isUse( const ZnkMyf config, const char* goal_hostname, size_t goal_hostname_leng )
{
	bool use_proxy = false;
	Znk_setStrLen_ifNPos( &goal_hostname_leng, goal_hostname );
	if( st_proxy_indicating_mode < 0 ){
		ZnkStrDAry proxy_minus = ZnkMyf_find_lines( config, "proxy_minus" );
		if( proxy_minus && ZnkStrDAry_find( proxy_minus, 0, goal_hostname, goal_hostname_leng ) != Znk_NPOS ){
			/* リストされているもののみ外部proxyを使用しない */
			use_proxy = false;
		} else {
			use_proxy = (bool)(st_proxy_hostname != NULL);
		}
	} else if( st_proxy_indicating_mode > 0 ){
		ZnkStrDAry proxy_plus = ZnkMyf_find_lines( config, "proxy_plus" );
		if( proxy_plus && ZnkStrDAry_find( proxy_plus, 0, goal_hostname, goal_hostname_leng ) != Znk_NPOS ){
			/* リストされているもののみ外部proxyを使用する */
			use_proxy = (bool)(st_proxy_hostname != NULL);
		} else {
			use_proxy = false;
		}
	} else {
		/* st_proxy_indicating_mode が 0 の場合は 外部proxyを無効とする */
		use_proxy = false;
	}
	return use_proxy;
}

void
MoaiParentProxy_set( const char* parent_proxy_hostname, uint16_t parent_proxy_port )
{
	st_proxy_hostname = parent_proxy_hostname;
	st_proxy_port     = parent_proxy_port;
}
const char*
MoaiParentProxy_getHostname( void )
{
	return st_proxy_hostname;
}
uint16_t
MoaiParentProxy_getPort( void )
{
	return st_proxy_port;
}
void
MoaiParentProxy_setIndicatingMode( int indicating_mode )
{
	st_proxy_indicating_mode = indicating_mode;
}
