#include <Znk_zlib.h>
#include <Znk_stdc.h>
#include <Znk_net_base.h>
#include <Znk_htp_rar.h>
#include <Znk_htp_hdrs.h>
#include <Znk_auto_ptr.hpp>
#include <Znk_s_base.h>
#include <Znk_str.h>
#include <Znk_str_fio.h>
#include <Znk_cookie.h>
#include <stdio.h>


/***
 * HTTPリクエスト送信
 * HTTP1.1では「Host:」は唯一の必須ヘッダとなっている(1.0ではそうではないかもしれないが、
 * 付けておいた方がよい)
 * これを付け忘れると、(特にreq_uriが空でない場合) Bad Request 400 が帰ってくる場合がある.
 */
Znk_INLINE void
makeSendData_forGet( ZnkBfr ans,
		const char* hostname,
		const char* req_uri,
		ZnkStr wkstr )
{
	ZnkStr send_hdr = wkstr;
	bool enable_gzip = true;

	ZnkStr_clear( send_hdr );
    ZnkStr_sprintf( send_hdr, Znk_NPOS, "Accept: */*\r\n" );
    ZnkStr_sprintf( send_hdr, Znk_NPOS, "Accept-Language: ja,en-US;q=0.7,en;q=0.3\r\n" );
    ZnkStr_sprintf( send_hdr, Znk_NPOS, "Accept-Encoding: gzip, deflate\r\n" );
    ZnkStr_sprintf( send_hdr, Znk_NPOS, "Connection: close\r\n" );

	if( enable_gzip ){
		ZnkStr_add( send_hdr, "Accept-Encoding: gzip,deflate\r\n" );
	}

	ZnkStr_add( send_hdr, "\r\n" );
	ZnkBfr_append_dfr( ans, (uint8_t*)ZnkStr_cstr(send_hdr), ZnkStr_leng(send_hdr) );
}

/***
 * RequestMethodより基本ヘッダの典型を生成する.
 */
Znk_INLINE bool
makeHdr1st( ZnkStrDAry hdr1st, ZnkHtpReqMethod req_method, const char* req_uri )
{
	switch( req_method ){
	case ZnkHtpReqMethod_e_GET:
	case ZnkHtpReqMethod_e_POST:
	case ZnkHtpReqMethod_e_HEAD:
	case ZnkHtpReqMethod_e_PUT:
	case ZnkHtpReqMethod_e_DELETE:
	case ZnkHtpReqMethod_e_CONNECT:
	case ZnkHtpReqMethod_e_OPTIONS:
	case ZnkHtpReqMethod_e_TRACE:
		ZnkStrDAry_resize( hdr1st, 3 );
		ZnkStrDAry_set( hdr1st, 0, ZnkStr_new( ZnkHtpReqMethod_getCStr( req_method ) ) );
		ZnkStrDAry_set( hdr1st, 1, ZnkStr_new( req_uri ) );
		ZnkStrDAry_set( hdr1st, 2, ZnkStr_new( "HTTP/1.1" ) );
		break;
	default:
		return false;
	}
	return true;
}

/***
 * User-Agent, Referer, CookieなどでHTTPヘッダを装飾する.
 */
static bool
loadHdrs( ZnkHtpHdrs hdrs, const char* http_hdr_filename )
{
	Znk_AUTO_PTR( ZnkFile, fp, ZnkF_fopen( http_hdr_filename, "rb" ), ZnkF_fclose );
	if( !fp ){
		ZnkF_printf_e( "Cannot open hdr file [%s]\n", http_hdr_filename );
		return false;
	} else {
		Znk_AUTO_PTR( ZnkStr, line, ZnkStr_new( "" ), ZnkStr_delete );
		ZnkStrFIO_fgets( line, 0, 4096, fp );
		ZnkStr_chompNL( line );
		ZnkHtpHdrs_registHdr1st( hdrs->hdr1st_, ZnkStr_cstr(line), ZnkStr_leng(line) );
		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				/* eof */
				break;
			}
			ZnkStr_chompNL( line );
			ZnkHtpHdrs_regist_byLine( hdrs->vars_, ZnkStr_cstr(line), ZnkStr_leng(line) );
		}
	}
	return true;
}
static bool
loadBody( ZnkBfr ans, const char* body_img_filename )
{
	if( body_img_filename ){
		uint8_t buf[ 1024 ];
		ZnkFile fp = ZnkF_fopen( body_img_filename, "rb" );
		if( fp ){
			size_t readed_size = 0;
			while( !ZnkF_feof( fp ) ){
				readed_size = ZnkF_fread( buf, 1, 1024, fp );
				ZnkBfr_append_dfr( ans, buf, readed_size );
			}
			ZnkF_fclose( fp );
			return true;
		}
	}
	return false;
}

static int
writeFP( void* arg, const uint8_t* buf, size_t buf_size )
{
	ZnkFile fp = (ZnkFile)arg;
	return ZnkF_fwrite( buf, buf_size, 1, fp );
}

static bool
process( const char* cnct_hostname, uint16_t cnct_port,
		const char* result_filename,
		const char* http_hdr_filename, const char* body_img_filename )
{
	bool result = false;

	Znk_AUTO_PTR( ZnkBfr, send_body, ZnkBfr_create_null(), ZnkBfr_destroy );
	Znk_AUTO_PTR( ZnkVarpDAry, cookie, ZnkVarpDAry_create(true), ZnkVarpDAry_destroy );
	Znk_AUTO_PTR( ZnkBfr, wk_bfr, ZnkBfr_create_null(), ZnkBfr_destroy );
	Znk_AUTO_PTR( ZnkFile, fp, ZnkF_fopen( result_filename, "wb" ), ZnkF_fclose );

	struct ZnkHtpHdrs_tag send_hdrs = { 0 };
	struct ZnkHtpHdrs_tag recv_hdrs = { 0 };
	ZnkHtpOnRecvFuncArg recv_fnca = { 0 };

	bool   is_proxy = false;
	size_t try_connect_num = 3;

	ZnkCookie_load( cookie, "cookie.txt" );

	recv_fnca.func_ = writeFP;
	recv_fnca.arg_  = fp;

	ZnkHtpHdrs_compose( &send_hdrs );
	ZnkHtpHdrs_compose( &recv_hdrs );

	loadHdrs( &send_hdrs, http_hdr_filename );
	loadBody( send_body, body_img_filename );

	result = ZnkHtpRAR_sendAndRecv( cnct_hostname, cnct_port,
			&send_hdrs, send_body,
			&recv_hdrs, recv_fnca,
			cookie,
			try_connect_num, is_proxy, wk_bfr );

	ZnkF_printf_e( "@@.\n" );
	{
		ZnkBfr_clear( wk_bfr );
		ZnkHtpHdrs_extendToStream( send_hdrs.hdr1st_, send_hdrs.vars_, wk_bfr, false );
		ZnkBfr_push_bk( wk_bfr, '\0' );
		ZnkF_printf_e( "@@L send_hdrs\n", (char*)ZnkBfr_data(wk_bfr) );
		ZnkF_printf_e( "%s", (char*)ZnkBfr_data(wk_bfr) );
		ZnkF_printf_e( "@@.\n" );
	}
	{
		ZnkBfr_clear( wk_bfr );
		ZnkHtpHdrs_extendToStream( recv_hdrs.hdr1st_, recv_hdrs.vars_, wk_bfr, false );
		ZnkBfr_push_bk( wk_bfr, '\0' );
		ZnkF_printf_e( "@@L recv_hdrs\n", (char*)ZnkBfr_data(wk_bfr) );
		ZnkF_printf_e( "%s", (char*)ZnkBfr_data(wk_bfr) );
		ZnkF_printf_e( "@@.\n" );
	}

	ZnkCookie_save( cookie, "cookie.txt" );

	ZnkHtpHdrs_dispose( &send_hdrs );
	ZnkHtpHdrs_dispose( &recv_hdrs );
	return result;
}


int main(int argc, char **argv)
{
	bool ret;
	const char* cnct_hostname = NULL;
	uint16_t    cnct_port = 80;
	const char* http_hdr_filename = NULL;
	const char* body_img_filename = NULL;
	const char* result_filename = NULL;

	ZnkF_printf( "@@L http_decorator_log\n" );

	ret = ZnkZlib_initiate();
	ZnkF_printf( "ZnkZlib_initiate : ret=[%d]\n", ret );

	ret = ZnkNetBase_initiate( false );
	ZnkF_printf( "ZnkNetBase_initiate : ret=[%d]\n", ret );

	if( argc < 5 ){
		ZnkF_printf( "Usage : http_decorator hostname port result_filename http_hdr_filename (body_img_filename)\n" );
		getchar();
		return 1;
	}

	cnct_hostname = argv[ 1 ];
	if( ZnkS_isBegin( cnct_hostname, "http://" ) ){
		cnct_hostname = cnct_hostname + 7;
	} else if( ZnkS_isBegin( cnct_hostname, "https://" ) ){
		cnct_hostname = cnct_hostname + 8;
	}
	sscanf( argv[ 2 ], "%hd", &cnct_port );
	result_filename   = argv[ 3 ];
	http_hdr_filename = argv[ 4 ];
	if( argc >= 6 ){
		body_img_filename = argv[ 5 ];
	}

	process( cnct_hostname, cnct_port, result_filename, http_hdr_filename, body_img_filename );

	return 0;
}
