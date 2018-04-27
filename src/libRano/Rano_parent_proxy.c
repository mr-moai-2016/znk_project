#include "Rano_parent_proxy.h"
#include "Rano_log.h"
#include <Znk_str_ary.h>
#include <Znk_s_base.h>
#include <Znk_str_fio.h>
#include <Znk_str_ex.h>
#include <string.h>

static char      st_proxy_hostname[ 256 ] = "";
static uint16_t  st_proxy_port = 0;
static int       st_proxy_indicating_mode = -1;
static ZnkStrAry st_parent_proxys = NULL; 
static ZnkDate   st_parent_proxys_date = { 0 }; 

bool
RanoParentProxy_isAppliedHost2( const ZnkMyf hosts, const char* goal_hostname, size_t goal_hostname_leng )
{
	ZnkStrAry proxy_except = NULL;
	ZnkStrAry proxy_apply  = NULL;

	Znk_setStrLen_ifNPos( &goal_hostname_leng, goal_hostname );

	proxy_except = ZnkMyf_find_lines( hosts, "proxy_except" );
	if( proxy_except && ZnkStrAry_find_isMatch( proxy_except,
				0, goal_hostname, goal_hostname_leng, ZnkS_isMatchSWC ) != Znk_NPOS )
	{
		/* リストされているものは外部proxyを使用しない */
		return false;
	}

	proxy_apply  = ZnkMyf_find_lines( hosts, "proxy_apply" );
	if( proxy_apply && ZnkStrAry_find_isMatch( proxy_apply,
				0, goal_hostname, goal_hostname_leng, ZnkS_isMatchSWC ) != Znk_NPOS )
	{
		/* リストされているものについて外部proxyを使用 */
		return !ZnkS_empty( st_proxy_hostname );
	}
	return false;
}

void
RanoParentProxy_set( const char* parent_proxy_hostname, uint16_t parent_proxy_port )
{
	ZnkS_copy( st_proxy_hostname, sizeof(st_proxy_hostname), parent_proxy_hostname, Znk_NPOS );
	st_proxy_port     = parent_proxy_port;

	/* 最終更新時間を記録 */
	ZnkDate_getCurrent( &st_parent_proxys_date );
}
bool
RanoParentProxy_set_byAuthority( const char* parent_proxy_autority )
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
	RanoLog_printf( "Rano : Setting New Proxy=[%s:%u]\n", st_proxy_hostname, st_proxy_port );

	/* 最終更新時間を記録 */
	ZnkDate_getCurrent( &st_parent_proxys_date );
	return true;
}
const char*
RanoParentProxy_getHostname( void )
{
	return st_proxy_hostname;
}
uint16_t
RanoParentProxy_getPort( void )
{
	return st_proxy_port;
}
void
RanoParentProxy_setIndicatingMode( int indicating_mode )
{
	st_proxy_indicating_mode = indicating_mode;
}
ZnkStrAry
RanoParentProxy_getParentProxys( void )
{
	return st_parent_proxys;
}
ZnkDate
RanoParentProxy_getDate( void )
{
	return st_parent_proxys_date;
}

static bool
loadLineAryData( ZnkStrAry line_ary, const char* filename )
{
	ZnkFile fp = Znk_fopen( filename, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		ZnkStrAry_clear( line_ary );

		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				break; /* eof : 読み込み完了 */
			}

			ZnkStr_chompNL( line ); /* 改行を除去 */
			if( ZnkStr_first( line ) == '#' ){
				continue; /* 行頭が#で始まる場合はskip */
			}
			ZnkStrEx_removeSideCSet( line, " \t", 2 );
			if( ZnkStr_empty( line ) ){
				continue; /* 空行をskip */
			}

			/* この行を一つの値として文字列配列へと追加 */
			ZnkStrAry_push_bk_cstr( line_ary, ZnkStr_cstr(line), ZnkStr_leng(line) );
		}
		ZnkStr_delete( line );
		Znk_fclose( fp );
		return true;
	}
	return false;
}

bool
RanoParentProxy_loadCandidate( const char* filename )
{
	bool result;
	if( st_parent_proxys == NULL ){
		st_parent_proxys = ZnkStrAry_create( true );
	}
	result = loadLineAryData( st_parent_proxys, filename );
	ZnkStrAry_push_bk_cstr( st_parent_proxys, "NONE", Znk_NPOS );
	return result;
}
