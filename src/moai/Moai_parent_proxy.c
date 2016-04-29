#include "Moai_parent_proxy.h"
#include <Znk_str_dary.h>
#include <Znk_s_base.h>
#include <string.h>

static char       st_proxy_hostname[ 256 ] = "";
static uint16_t   st_proxy_port = 0;
static int        st_proxy_indicating_mode = -1;
static ZnkStrDAry st_parent_proxys = NULL; 

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
			use_proxy = !ZnkS_empty( st_proxy_hostname );
		}
	} else if( st_proxy_indicating_mode > 0 ){
		ZnkStrDAry proxy_plus = ZnkMyf_find_lines( config, "proxy_plus" );
		if( proxy_plus && ZnkStrDAry_find( proxy_plus, 0, goal_hostname, goal_hostname_leng ) != Znk_NPOS ){
			/* リストされているもののみ外部proxyを使用する */
			use_proxy = !ZnkS_empty( st_proxy_hostname );
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
	ZnkS_copy( st_proxy_hostname, sizeof(st_proxy_hostname), parent_proxy_hostname, Znk_NPOS );
	st_proxy_port     = parent_proxy_port;
}
bool
MoaiParentProxy_set_byAuthority( const char* parent_proxy_autority )
{
	const char* begin = parent_proxy_autority;
	char     proxy_hostname[ 256 ] = "";
	uint16_t proxy_port = 0;

	if( ZnkS_empty( begin ) || ZnkS_eq( begin, "NONE" ) ){
		proxy_hostname[ 0 ] = '\0';
		proxy_port = 0;
	} else {
		const char* p = strchr( begin, ':' );
		size_t hostname_leng = 0;
		if( p ){
			ZnkS_getU16U( &proxy_port, p+1 );
			hostname_leng = p - begin;
		} else {
			proxy_port = 80;
			hostname_leng = Znk_strlen( begin );
		}
		ZnkS_copy( proxy_hostname, sizeof(proxy_hostname), begin, hostname_leng );
	}

	if( ZnkS_eq( proxy_hostname, st_proxy_hostname ) && proxy_port == st_proxy_port ){
		/* 変更なし */
		return false;
	}
	/* 変更あり */
	ZnkS_copy( st_proxy_hostname, sizeof(st_proxy_hostname), proxy_hostname, Znk_NPOS );
	st_proxy_port = proxy_port;
	return true;
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
void
MoaiParentProxy_setParentProxys( ZnkStrDAry parent_proxys )
{
	if( st_parent_proxys == NULL ){
		st_parent_proxys = ZnkStrDAry_create( true );
	}
	ZnkStrDAry_copy( st_parent_proxys, parent_proxys );
	ZnkStrDAry_push_bk_cstr( st_parent_proxys, "NONE", Znk_NPOS );
}
ZnkStrDAry
MoaiParentProxy_getParentProxys( void )
{
	return st_parent_proxys;
}
