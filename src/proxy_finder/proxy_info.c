#include "proxy_info.h"

#include <Znk_mem_find.h>
#include <Znk_s_base.h>
#include <Znk_stdc.h>
#include <Znk_str_ex.h>
#include <Znk_str_fio.h>
#include <Znk_vpod.h>
#include <Znk_obj_ary.h>
#include <Znk_missing_libc.h>

#include <string.h>

ProxyStateCode
ProxyStateCode_getCode_fromStr( const char* query_str )
{
	static struct {
		ProxyStateCode state_code_; uint32_t id32_;
	} st_table [] = {
		{ ProxyState_e_UNKN, },
		{ ProxyState_e_ACOK, },
		{ ProxyState_e_ACNG, },
		{ ProxyState_e_RFUS, },
		{ ProxyState_e_NTJP, },
		{ ProxyState_e_BREK, },
		{ ProxyState_e_UCNT, },
		{ ProxyState_e_MBON, },
		{ ProxyState_e_PCON, },
	};
	static bool st_initialized = false;

	size_t   idx;
	uint32_t query_id32 = ZnkS_get_id32( query_str, Znk_NPOS );

	/* st_table ÇÃèâä˙âª */
	if( !st_initialized ){
		const char* str;
		for( idx=0; idx<Znk_NARY( st_table ); ++idx ){
			str = ProxyStateCode_toStr( st_table[ idx ].state_code_ );
			st_table[ idx ].id32_ = ZnkS_get_id32( str, Znk_NPOS );
		}
		st_initialized = true;
	}

	/* state_codeÇÃåüçı */
	for( idx=0; idx<Znk_NARY( st_table ); ++idx ){
		if( st_table[ idx ].id32_ == query_id32 ){
			return st_table[ idx ].state_code_;
		}
	}
	return ProxyState_e_UNKN;
}

struct ProxyInfoImpl {
	char     ip_[ 192 ];
	uint16_t port_;
	ProxyStateCode state_code_;
	char     hostname_[ 192 ];
};

static void
deleteElem( void* elem ){
	Znk_free( elem );
}
ProxyInfoAry
ProxyInfoAry_create( bool elem_responsibility )
{
	ZnkElemDeleterFunc deleter = elem_responsibility ? deleteElem : NULL;
	return (ProxyInfoAry)ZnkObjAry_create( deleter );
}


static bool
queryProxy( ZnkObj obj, void* arg )
{
	ProxyInfo info = (ProxyInfo)obj;
	const char* ip = (const char*)arg;
	return (bool)ZnkS_eq( info->ip_, ip );
}
static ZnkObj
allocProxy( void* arg )
{
	return (ZnkObj)Znk_malloc( sizeof( struct ProxyInfoImpl ) );
}
void
ProxyInfoAry_regist( ProxyInfoAry list,
		const char* ip, uint16_t port, ProxyStateCode state_code, const char* hostname )
{
	bool is_newly = false;
	ProxyInfo info = (ProxyInfo)ZnkObjAry_intern( (ZnkObjAry)list, NULL,
			queryProxy, (void*)ip, allocProxy, NULL,
			&is_newly );

	/***
	 * Ç±Ç±Ç≈ÇÃèàóùÇÕêVãKìoò^ÇæÇØÇ∆Ç∑ÇÈ.
	 * (ä˘ë∂Ç÷ÇÃè„èëÇ´èàóùÇÕçsÇÌÇ»Ç¢)
	 */
	if( info && is_newly ){
		ZnkS_copy( info->ip_, sizeof(info->ip_), ip, Znk_NPOS );
		info->port_       = port;
		info->state_code_ = state_code;
		ZnkS_copy( info->hostname_, sizeof(info->hostname_), hostname, Znk_NPOS );
	}
}


bool
ProxyInfoAry_load( ProxyInfoAry list, const char* filename )
{
	bool    result = false;
	ZnkFile fp     = NULL;

	fp = Znk_fopen( filename, "rb" );
	if( fp == NULL ){
		goto FUNC_END;
	} else {
		ZnkStr    line = ZnkStr_new( "" );
		ZnkStrAry tkns = ZnkStrAry_create( true ); 
		char      ip[ 192 ];

		while( ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			ZnkStr_chompNL( line );
			if( ZnkStr_empty( line ) ){ continue; }
			if( ZnkStr_first( line ) == '#' ){ continue; }

			ZnkStrAry_clear( tkns );
			ZnkStrEx_addSplitCSet( tkns,
					ZnkStr_cstr(line), ZnkStr_leng(line),
					" ", 1,
					3 );

			if( ZnkStrAry_size( tkns ) > 0 ){
				const char*    ip_and_port = ZnkStrAry_at_cstr( tkns, 0 );
				uint16_t       port        = 80;
				ProxyStateCode state_code  = ProxyState_e_UNKN;
				const char*    state_str   = "UNKN";
				const char*    hostname    = "?";
				const size_t   pos         = ZnkMem_lfind_8( (uint8_t*)ip_and_port, strlen(ip_and_port), (uint8_t)':', 1 );

				if( ZnkStrAry_size( tkns ) >= 2 ){
					state_str = ZnkStrAry_at_cstr( tkns, 1 );
				}
				if( ZnkStrAry_size( tkns ) >= 3 ){
					hostname = ZnkStrAry_at_cstr( tkns, 2 );
				}

				state_code = ProxyStateCode_getCode_fromStr( state_str );
				ZnkS_copy( ip, sizeof(ip), ip_and_port, pos );
				if( pos == Znk_NPOS ){
					port = 80;
				} else {
					ZnkS_getU16U( &port, ip_and_port+pos+1 );
				}
				ProxyInfoAry_regist( list, ip, port, state_code, hostname );
			}
		}

		ZnkStrAry_destroy( tkns );
		ZnkStr_delete( line );
	}

	result = true;
FUNC_END:
	Znk_fclose( fp );

	return result;
}

bool
ProxyInfoAry_save( const ProxyInfoAry list, const char* filename, bool with_additional_recode )
{
	bool    result = false;
	ZnkFile fp     = NULL;

	fp = Znk_fopen( filename, "wb" );
	if( fp == NULL ){
		goto FUNC_END;
	} else {
		ProxyInfo    info;
		const size_t size = ProxyInfoAry_size( list );
		size_t       idx;
		char         ip_and_port[ 256 ];
		ZnkStr       line = ZnkStr_new( "" );

		for( idx=0; idx<size; ++idx ){
			info = ProxyInfoAry_at( list, idx );
			Znk_snprintf( ip_and_port, sizeof(ip_and_port), "%s:%u", info->ip_, (unsigned int)info->port_ );

			ZnkStr_clear( line );
			ZnkStrEx_addEmbededStr( line,
					' ', 22,
					ip_and_port, strlen(ip_and_port), 'L' );
			if( with_additional_recode ){
				ZnkStr_addf( line, "%s %s", ProxyStateCode_toStr( info->state_code_ ), info->hostname_ );
			}
			ZnkStr_add( line, "\n" );
			Znk_fputs( ZnkStr_cstr(line), fp );
		}

		ZnkStr_delete( line );
	}

	result = true;
FUNC_END:
	Znk_fclose( fp );

	return result;
}

