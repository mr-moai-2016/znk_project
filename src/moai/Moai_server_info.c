#include "Moai_server_info.h"

#include <Rano_myf.h>
#include <Rano_log.h>
#include <Znk_net_ip.h>
#include <Znk_rand.h>
#include <Znk_def_util.h>
#include <Znk_missing_libc.h>

static char          st_server_name[ 256 ] = "";
static uint16_t      st_moai_port = 8124;
static uint16_t      st_xhr_dmz_port = 8125;
static bool          st_enable_log_file = true;
static bool          st_enable_log_verbose = true;
static uint32_t      st_private_ip = 0;
static char          st_xhr_dmz[ 256 ] = "";
static char          st_xhr_authhost[ 256 ] = "";
static char          st_acceptable_host[ 256 ] = "";
static bool          st_you_need_to_restart_moai = false;
static char          st_filters_dir[ 256 ] = "filters";
static char          st_profile_dir[ 256 ] = "../moai_profile";
static RanoModuleAry st_mod_ary = NULL;
static bool          st_is_all_replace_cookie = true;
static ZnkVarpAry    st_cookie_extra_vars = NULL;
static char          st_authentic_key[ 256 ] = "";

const char*
MoaiServerInfo_server_name( void )
{
	return st_server_name;
}
void
MoaiServerInfo_set_server_name( const char* server_name )
{
	ZnkS_copy( st_server_name, sizeof(st_server_name), server_name, Znk_NPOS );
}
uint16_t
MoaiServerInfo_port( void )
{
	return st_moai_port;
}
void
MoaiServerInfo_set_port( uint16_t port )
{
	st_moai_port = port;
}
bool
MoaiServerInfo_is_enable_log_file( void )
{
	return st_enable_log_file;
}
void
MoaiServerInfo_set_enable_log_file( bool enable )
{
	st_enable_log_file = enable;
}
bool
MoaiServerInfo_is_enable_log_verbose( void )
{
	return st_enable_log_verbose;
}
void
MoaiServerInfo_set_enable_log_verbose( bool enable )
{
	st_enable_log_verbose = enable;
}
uint32_t
MoaiServerInfo_private_ip( void )
{
	return st_private_ip;
}
void
MoaiServerInfo_set_private_ip( uint32_t private_ip )
{
	st_private_ip = private_ip;
}
const char*
MoaiServerInfo_XhrDMZ( void )
{
	/***
	 * TODO:
	 * 環境変数HOST, あるいはDNSで設定された別名でも良いかもしれない.
	 * config.myf において指定も可であるようにしたい.
	 */
	if( ZnkS_empty( st_xhr_dmz ) ){
		if( ZnkS_eq( st_acceptable_host, "LOOPBACK" ) ){
			Znk_snprintf( st_xhr_dmz, sizeof(st_xhr_dmz), "127.0.0.1:%u", st_xhr_dmz_port );
		} else {
			char private_ipstr[ 64 ] = "";
			ZnkNetIP_getIPStr_fromU32( st_private_ip, private_ipstr, sizeof(private_ipstr) );
			Znk_snprintf( st_xhr_dmz, sizeof(st_xhr_dmz), "%s:%u", private_ipstr, st_xhr_dmz_port );
		}
	}
	return st_xhr_dmz;
}
uint16_t
MoaiServerInfo_XhrDMZPort( void )
{
	return st_xhr_dmz_port;
}
void
MoaiServerInfo_setXhrDMZPort( uint16_t port )
{
	st_xhr_dmz_port = port;
}
const char*
MoaiServerInfo_XhrAuthHost( void )
{
	/***
	 * とりあえず 127.0.0.1 固定.
	 */
	if( ZnkS_empty( st_xhr_authhost ) ){
		Znk_snprintf( st_xhr_authhost, sizeof(st_xhr_authhost), "127.0.0.1:%u", st_moai_port );
	}
	return st_xhr_authhost;
}
void
MoaiServerInfo_setXhrDMZ( const char* xhr_dmz )
{
	ZnkS_copy( st_xhr_dmz, sizeof(st_xhr_dmz), xhr_dmz, Znk_NPOS );
}

const char*
MoaiServerInfo_acceptable_host( void )
{
	return st_acceptable_host;
}
void
MoaiServerInfo_set_acceptable_host( const char* acceptable_host )
{
	ZnkS_copy( st_acceptable_host, sizeof(st_acceptable_host), acceptable_host, Znk_NPOS );
}
bool
MoaiServerInfo_you_need_to_restart_moai( void )
{
	return st_you_need_to_restart_moai;
}
void
MoaiServerInfo_set_you_need_to_restart_moai( bool you_need_to_restart_moai )
{
	st_you_need_to_restart_moai = you_need_to_restart_moai;
}

const char*
MoaiServerInfo_filters_dir( void )
{
	return st_filters_dir;
}
void
MoaiServerInfo_set_filters_dir( const char* filters_dir )
{
	ZnkS_copy( st_filters_dir, sizeof(st_filters_dir), filters_dir, Znk_NPOS );
}
const char*
MoaiServerInfo_profile_dir( void )
{
	return st_profile_dir;
}
void
MoaiServerInfo_set_profile_dir( const char* profile_dir )
{
	ZnkS_copy( st_profile_dir, sizeof(st_profile_dir), profile_dir, Znk_NPOS );
}

RanoModuleAry 
MoaiServerInfo_mod_ary( void )
{
	return st_mod_ary;
}
void
MoaiServerInfo_initiate_mod_ary( void )
{
	ZnkMyf target_myf = RanoMyf_theTarget();
	Znk_DELETE_PTR( st_mod_ary, RanoModuleAry_destroy, NULL );
	st_mod_ary = RanoModuleAry_create( true );
	RanoModuleAry_loadAllModules( st_mod_ary, target_myf, st_filters_dir, "plugins" );
}
void
MoaiServerInfo_finalize_mod_ary( void )
{
	Znk_DELETE_PTR( st_mod_ary, RanoModuleAry_destroy, NULL );
}

/***
 * LOOPBACK以外もサポートする場合、即ち localhost以外のLAN上のマシンからの接続や、
 * 192.168.*.* などのPrivateIPの形式でlocalhostを指定した場合も受け付ける場合である.
 * (WANからの接続というのも理論上は有り得るが、セキュリティ上はシステムにおいて
 * 許可してはならない)
 *
 * この場合、server_nameをLAN上からアクセスできる名前としなければならないが
 * server_nameを空値に設定した場合はPrivateIPの自動取得を試み、それを設定するものとする.
 * この自動取得に失敗した場合は 127.0.0.1 がセットされる.
 * ( このとき、MoaiのWeb Configurationにおいて localhost以外からの設定は不可となる )
 */
void
MoaiServerInfo_autosetServerName( void )
{
	char private_ipstr[ 64 ] = "";
	const char* server_name     = MoaiServerInfo_server_name();
	const char* acceptable_host = MoaiServerInfo_acceptable_host();
	uint32_t private_ip = 0;
	/***
	 * 最新のPrivateIPを自動取得する.
	 */
	if( ZnkNetIP_getPrivateIP32( &private_ip ) ){
		ZnkNetIP_getIPStr_fromU32( private_ip, private_ipstr, sizeof(private_ipstr) );
		RanoLog_printf2( "Moai : AutoGet PrivateIP=[%s].\n", private_ipstr );
	}
	MoaiServerInfo_set_private_ip( private_ip );

	/* 試験的 */
	MoaiServerInfo_XhrDMZ();

	if(  ZnkS_empty( acceptable_host )
	  || ZnkS_eq( acceptable_host, "127.0.0.1" )
	  || ZnkS_eq( acceptable_host, "LOOPBACK" )
	){
		/* LOOPBACKの場合 */
		MoaiServerInfo_set_server_name( "127.0.0.1" );
		return;
	} else if( ZnkS_empty( server_name ) || ZnkS_eq( server_name, "127.0.0.1" ) ){
		/***
		 * ANYの場合かつserver_nameが空or127.0.0.1のとき、server_nameの値をPrivateIPで置き換える.
		 */
		if( private_ip == 0 ){
			/***
			 * PrivateIPの自動取得に失敗するなどでprivate_ipが空のとき.
			 * とりあえずLOOPBACK用の値を指定しておく.
			 */
			MoaiServerInfo_set_server_name( "127.0.0.1" );
		} else {
			MoaiServerInfo_set_server_name( private_ipstr );
		}
	}
}
void
MoaiServerInfo_reloadFilters( RanoModule mod, const char* hostname )
{
	if( mod ){
		ZnkMyf target_myf = RanoMyf_theTarget();
		const char* target_name = RanoTarget_findTargetName( target_myf, hostname );
		if( target_name ){
			RanoModule_reloadFilters_byDate( mod, target_name, st_filters_dir );
		}
	}
}

bool
MoaiServerInfo_isAllReplaceCookie( void )
{
	return st_is_all_replace_cookie;
}
ZnkVarpAry
MoaiServerInfo_refCookieExtraVars( void )
{
	return st_cookie_extra_vars;
}

const char*
MoaiServerInfo_authenticKey( void )
{
	if( ZnkS_empty( st_authentic_key ) ){
		union {
			uint64_t ptr64_;
			uint8_t  ary_[ 8 ];
		} u;
		uint32_t r_code_1 = 0;
		uint32_t r_code_2 = 0;
		uint32_t sum = 0;
		size_t i;
		u.ptr64_ = Znk_force_ptr_cast( uint64_t, MoaiServerInfo_authenticKey );
		for( i=0; i<Znk_NARY(u.ary_); ++i ){
			sum += u.ary_[ i ];
		}
		ZnkRand_shuffle( sum );
		r_code_1 = (uint32_t)ZnkRand_getRandUL( UINT_MAX );
		r_code_2 = (uint32_t)ZnkRand_getRandUL( UINT_MAX );
		if( r_code_1 < r_code_2 ){ Znk_SWAP( uint32_t, r_code_1, r_code_2 ); }
		Znk_snprintf( st_authentic_key, sizeof(st_authentic_key), 
				sum > 60 ? "%04X%04X" : "%04x%04x", r_code_1, r_code_2 );
	}
	return st_authentic_key;
}
