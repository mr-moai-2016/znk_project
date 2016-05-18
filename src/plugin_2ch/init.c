#include <Moai_plugin_dev.h>
#include <Znk_stdc.h>
#include <Znk_def_util.h>
#include <Znk_zlib.h>
#include <Znk_missing_libc.h>
#include <Znk_net_base.h>
#include <Znk_s_base.h>
#include <Znk_str.h>
#include <Znk_str_fio.h>
#include <Znk_str_ptn.h>
#include <Znk_cookie.h>
#include <Znk_myf.h>
#include <Znk_dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>
#include <limits.h>
#include <time.h>

static bool st_is_dos_path = false;

static bool
decorateHeaderGET( const char* hostname, const char* request_uri,
		const char* ua, const char* referer, ZnkVarpAry cookie )
{
	const char* tmpdir = st_is_dos_path ? ".\\tmp\\" : "./tmp/";
	ZnkFile fp = NULL;
	char http_hdr_file[ 256 ] = "";
	Znk_snprintf( http_hdr_file, sizeof(http_hdr_file), "%shttp_hdr.dat", tmpdir );
	fp = ZnkF_fopen( http_hdr_file, "wb" );
	if( fp ){
		ZnkF_fprintf( fp, "GET %s HTTP/1.1\r\n", request_uri );
		ZnkF_fprintf( fp, "Host: %s\r\n", hostname );
		ZnkF_fprintf( fp, "User-Agent: %s\r\n", ua );
		//ZnkF_fprintf( fp, "Accept: */*\r\n" );
		ZnkF_fprintf( fp, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n" );
		ZnkF_fprintf( fp, "Accept-Language: ja,en-US;q=0.7,en;q=0.3\r\n" );
		ZnkF_fprintf( fp, "Accept-Encoding: gzip, deflate\r\n" );

		/* Referer */
		if( !ZnkS_empty(referer) ){
			ZnkF_fprintf( fp, "Referer: %s\r\n", referer );
		}
		/* Cookie */
		{
			const size_t size = ZnkVarpAry_size( cookie );
			if( size ){
				size_t idx;
				ZnkVarp varp;
				ZnkF_fprintf( fp, "Cookie: " );
				for( idx=0; idx<size; ++idx ){
					varp = ZnkVarpAry_at( cookie, idx );
					ZnkF_fprintf( fp, "%s=%s", ZnkStr_cstr(varp->name_), ZnkVar_cstr(varp) );
					if( idx < size-1 ){
						ZnkF_fprintf( fp, "; " );
					}
				}
				ZnkF_fprintf( fp, "\r\n" );
			}
		}

		ZnkF_fprintf( fp, "Connection: keep-alive\r\n" );
		ZnkF_fprintf( fp, "\r\n" );
		ZnkF_fclose( fp );
		return true;
	}
	return false;
}

bool
initiate( ZnkMyf flr_send, const char* parent_proxy, ZnkStr result_msg )
{
	/* ‚Æ‚è‚ ‚¦‚¸Œ»Žž“_‚Å‚Í‰½‚à‚µ‚È‚¢ */
	ZnkStr_set( result_msg, "Virtual USERS done. Your 2ch_send.myf is initiated successfully." );
	return true;
}

