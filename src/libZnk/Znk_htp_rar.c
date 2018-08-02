#include <Znk_htp_rar.h>
#include <Znk_htp_sbio.h>
#include <Znk_socket.h>
#include <Znk_cookie.h>
#include <Znk_s_base.h>
#include <Znk_str_ary.h>
#include <Znk_stdc.h>
#include <Znk_sys_errno.h>
#include <Znk_stock_bio.h>

struct SARInfo {
	ZnkHtpOnRecvFuncArg prog_fnca_;
	ZnkSocket           sock_;
};

static int
recvSocket( void* arg, uint8_t* buf, size_t buf_size )
{
	struct SARInfo* sar_info = Znk_force_ptr_cast( struct SARInfo*, arg );
	ZnkSocket sock = sar_info->sock_;
	int result = ZnkSocket_recv( sock, buf, buf_size );
	if( sar_info->prog_fnca_.func_ ){
		sar_info->prog_fnca_.func_( sar_info->prog_fnca_.arg_, buf, buf_size );
	}
	return result;
}

static bool
sendAndRecv_bySocket( ZnkSocket sock,
		ZnkHtpHdrs send_hdrs, ZnkBfr send_body,
		ZnkHtpHdrs recv_hdrs, ZnkHtpOnRecvFuncArg recv_fnca, ZnkHtpOnRecvFuncArg prog_fnca,
		ZnkVarpAry cookie, ZnkBfr wk_bfr, ZnkStr ermsg )
{
	ZnkErr_D( err );
	bool result = false;
	bool internal_wk_bfr = false;
	struct SARInfo sar_info = { { 0 } };
	ZnkStockBIO sbio = NULL;
	ZnkHtpReqMethod req_method = ZnkHtpReqMethod_getType_fromCStr(
			ZnkStrAry_at_cstr( send_hdrs->hdr1st_, 0 ) );

	sar_info.prog_fnca_ = prog_fnca;
	sar_info.sock_      = sock;
	sbio = ZnkStockBIO_create( 256, recvSocket, (void*)&sar_info );

	if( wk_bfr == NULL ){
		internal_wk_bfr = true;
		wk_bfr = ZnkBfr_create_null();
	}

	ZnkBfr_clear( wk_bfr );
	ZnkHtpHdrs_extendToStream( send_hdrs->hdr1st_, send_hdrs->vars_, wk_bfr, true );

	/* BodyImage‚ð’Ç‰Á */
	if( send_body ){
		ZnkBfr_append_dfr( wk_bfr, ZnkBfr_data(send_body), ZnkBfr_size(send_body) );
	}

	if( ZnkSocket_send( sock, ZnkBfr_data( wk_bfr ), ZnkBfr_size( wk_bfr ) ) == -1 ){
		ZnkSysErrnoInfo* err_info = ZnkSysErrno_getInfo( ZnkSysErrno_errno() );
		ZnkErr_internf( &err,
				"ZnkSocket_send : Failure : SysErr=[%s:%s]",
				err_info->sys_errno_key_, err_info->sys_errno_msg_ );
		result = false;
		goto FUNC_END;
	}

	result = ZnkHtpSBIO_recv( sbio, req_method,
			recv_hdrs, recv_fnca, cookie, wk_bfr, ermsg );

	
FUNC_END:
	if( internal_wk_bfr ){
		ZnkBfr_destroy( wk_bfr );
	}
	ZnkStockBIO_destroy( sbio );
	return result;
}

const char*
ZnkHtpRAR_getHostnameAndUnderpath_fromURL( const char* url, char* hostname_buf, size_t hostname_buf_size )
{
	size_t slash_pos;
	const char* underpath = "";
	if( ZnkS_isBegin( url, "http://" ) ){
		url = url + 7;
	}
	slash_pos = ZnkS_lfind_one_of( url, 0, Znk_NPOS, "/", 1 );
	ZnkS_copy( hostname_buf, hostname_buf_size, url, slash_pos );
	if( slash_pos != Znk_NPOS ){
		underpath = url + slash_pos + 1;
	}
	return underpath;
}

void
ZnkHtpRAR_getHostnameAndPort( const char* url,
		char* hostname_buf, size_t hostname_buf_size, uint16_t* port )
{
	size_t colon_pos;
	size_t slash_pos;
	char host_and_port[ 512 ];

	if( ZnkS_isBegin( url, "http://" ) ){
		url = url + 7;
	} else if( ZnkS_isBegin( url, "https://" ) ){
		url = url + 8;
	}

	slash_pos = ZnkS_lfind_one_of( url, 0, Znk_NPOS, "/", 1 );
	ZnkS_copy( host_and_port, sizeof(host_and_port), url, slash_pos );

	colon_pos = ZnkS_lfind_one_of( host_and_port, 0, Znk_NPOS, ":", 1 );
	ZnkS_copy( hostname_buf, hostname_buf_size, host_and_port, colon_pos );

	if( colon_pos == Znk_NPOS ){
		if( port ){ *port = 80; }
	} else {
		if( port ){
			const char* port_str = url + colon_pos + 1;
			ZnkS_getU16U( port, port_str );
		}
	}
}

bool
ZnkHtpRAR_sendAndRecv( const char* cnct_hostname, uint16_t cnct_port,
		ZnkHtpHdrs send_hdrs, ZnkBfr send_body,
		ZnkHtpHdrs recv_hdrs, ZnkHtpOnRecvFuncArg recv_fnca, ZnkHtpOnRecvFuncArg prog_fnca,
		ZnkVarpAry cookie,
		size_t try_connect_num, bool is_proxy, ZnkBfr wk_bfr, ZnkStr ermsg )
{
	bool        result = false;
	const char* hostname = cnct_hostname;
	bool        is_need_modify_req_uri = false;
	ZnkVarp     varp   = ZnkHtpHdrs_find_literal( send_hdrs->vars_, "Host" );
	ZnkStrAry   hdr1st = send_hdrs->hdr1st_;

	if( varp ){
		hostname = ZnkVar_name_cstr( varp );
		if( is_proxy ){
			ZnkStr req_uri = ZnkStrAry_at( hdr1st, 1 );
			/* connect via proxy. */
			switch( ZnkStr_first(req_uri) ){
			case '/':
				/* req_uri begin from under-path */
				is_need_modify_req_uri = true;
				break;
			case '*':
			default:
				/* req_uri begin from hostname or * */
				break;
			}
		}
	}

	if( is_need_modify_req_uri ){
		ZnkStr tmp = ZnkStr_new( "" );
		ZnkStr req_uri = ZnkStrAry_at( hdr1st, 1 );
		ZnkStr_addf( tmp, "%s%s", hostname, ZnkStr_cstr( req_uri ) );
		ZnkStr_swap( tmp, req_uri );
		ZnkStr_delete( tmp );
	}

	{
		ZnkErr_D( err );
		ZnkSocket sock = ZnkSocket_open();
		bool is_inprogress = false;
	
		while( try_connect_num ){
			if( !ZnkSocket_connectToServer( sock, cnct_hostname, cnct_port, &err, &is_inprogress ) ){
				if( ermsg ){
					ZnkStr_addf( ermsg, "%s (try=%zu).\n", ZnkErr_cstr(err),try_connect_num );
				}
				--try_connect_num;
				if( try_connect_num == 0 ){
					result = false;
					goto FUNC_END;
				}
			} else {
				break;
			}
		}
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkHtpRAR : connectTo[%s:%hu] : [Success].\n", cnct_hostname, cnct_port );
		}
	
		result = sendAndRecv_bySocket( sock,
				send_hdrs, send_body,
				recv_hdrs, recv_fnca, prog_fnca,
				cookie, wk_bfr, ermsg );

		ZnkSocket_close( sock );
	}

FUNC_END:
	return result;
}
