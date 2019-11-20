#include "Moai_io_base.h"
#include "Moai_connection.h"

#include <Rano_log.h>
#include <Znk_net_base.h>
#include <Znk_socket.h>
#include <Znk_missing_libc.h>
#include <Znk_str.h>
#include <Znk_s_base.h>
#include <time.h>


static void
formatTimeString( char* buf, size_t buf_size, time_t clck )
{
	struct 	tm *timePtr;
	if( clck == 0 )
		clck = time(NULL);
	timePtr = gmtime( &clck );
	//strftime( buf, buf_size, "%a, %d %b %Y %T GMT", timePtr );
	strftime( buf, buf_size, "%a, %d %b %Y %H:%M:%S GMT", timePtr );
}
static void
makeResponseHdr_200( ZnkStr str, const char* content_type, int contentLength, int modTime )
{
	char timeBuf[ 128 ];
	formatTimeString( timeBuf, sizeof(timeBuf), 0 );

	ZnkStr_addf( str,
			"HTTP/1.1 200 OK\r\n"
			"Date: %s\r\n"
			"Content-Type: %s\r\n",
			timeBuf, content_type
	);

	if( contentLength > 0 ){
		ZnkStr_addf( str,
				"Content-Length: %d\r\n",
				contentLength
		);
	}
	ZnkStr_add( str, "\r\n" );
}

int
MoaiIO_sendTxtf( ZnkSocket sock, const char* content_type, const char* fmt, ... )
{
	int ret = 0;
	va_list ap;
	ZnkStr ans = ZnkStr_new("");
	ZnkStr msg = ZnkStr_new("");

	va_start(ap, fmt);

	ZnkStr_vsnprintf( msg, Znk_NPOS, Znk_NPOS, fmt, ap );
	makeResponseHdr_200( ans, content_type, ZnkStr_leng(msg), 0 );
	ZnkStr_add( ans, ZnkStr_cstr(msg) );
	ret = ZnkSocket_send_cstr( sock, ZnkStr_cstr(ans) );

	va_end(ap);

	ZnkStr_delete( msg );
	ZnkStr_delete( ans );
	return ret;
}
bool
MoaiIO_sendResponseFile( ZnkSocket sock, const char* filename, MoaiIOFilterFunc io_filter_func )
{
	bool result = false;
	ZnkFile fp = Znk_fopen( filename, "rb" );
	if( fp ){
		size_t read_size;
		uint8_t buf[ 4096 ];
		ZnkBfr bfr = ZnkBfr_create_null();
		ZnkStr str = ZnkStr_new( "" );
		const char* content_type = "text/html";
		const char* ext = ZnkS_get_extension( filename, '.' );

		if( ZnkS_eqCase( ext, "jpg" ) || ZnkS_eqCase( ext, "jpeg" ) ){
			content_type = "image/jpeg";
		} else if( ZnkS_eqCase( ext, "png" ) ){
			content_type = "image/png";
		} else if( ZnkS_eqCase( ext, "gif" ) ){
			content_type = "image/gif";
		} else if( ZnkS_eqCase( ext, "htm" ) || ZnkS_eqCase( ext, "html" ) ){
			content_type = "text/html";
		} else if( ZnkS_eqCase( ext, "css" ) ){
			content_type = "text/css";
		} else if( ZnkS_eqCase( ext, "txt" ) ){
			content_type = "text/plain";
		} else if( ZnkS_eqCase( ext, "xml" ) ){
			content_type = "text/xml";
		} else if( ZnkS_eqCase( ext, "js" ) ){
			content_type = "application/javascript";
		} else {
			content_type = "application/octet-stream";
		}

		while( true ){
			read_size = Znk_fread( buf, sizeof(buf), fp );
			if( read_size == 0 ){
				break; /* eof */
			}
			ZnkBfr_append_dfr( bfr, buf, read_size );
		}
		Znk_fclose( fp );
		if( io_filter_func ){
			io_filter_func( bfr, content_type );
		}
		makeResponseHdr_200( str, content_type, ZnkBfr_size(bfr), 0 );
		ZnkBfr_insert( bfr, 0, (uint8_t*)ZnkStr_cstr(str), ZnkStr_leng(str) );
		if( ZnkSocket_send( sock, ZnkBfr_data(bfr), ZnkBfr_size(bfr) ) < 0 ){
			result = false;
		}
		ZnkBfr_destroy( bfr );
		result = true;
	}
	return result;
}


Znk_INLINE double
getCurrentSec( void ){ return ( 1.0 / CLOCKS_PER_SEC ) * clock() + 1.0; }

static char
getSockTypeCh( MoaiSockType sock_type )
{
	switch( sock_type ){
	case MoaiSockType_e_Listen: return 'L';
	case MoaiSockType_e_Outer:  return 'O';
	case MoaiSockType_e_Inner:  return 'I';
	default: break;
	}
	return 'N';
}

const char*
MoaiIO_makeSockStr( char* buf, size_t buf_size, ZnkSocket sock, bool detail )
{
	MoaiConnection mcn = NULL;
	MoaiSockType sock_type = MoaiSockType_e_None;

	if( MoaiConnection_isListeningSock( sock ) ){
		sock_type = MoaiSockType_e_Listen;
	} else if( (mcn = MoaiConnection_find_byISock( sock )) != NULL ){
		sock_type = MoaiSockType_e_Inner;
	} else if( (mcn = MoaiConnection_find_byOSock( sock )) != NULL ){
		sock_type = MoaiSockType_e_Outer;
	}

	if( mcn ){
		const char* hostname = MoaiConnection_hostname( mcn );
		const uint16_t port  = MoaiConnection_port( mcn );
		if( detail
		  && sock_type != MoaiSockType_e_Listen
		  && sock_type != MoaiSockType_e_None
		  && !ZnkS_empty(hostname) )
		{
			ZnkSocket I_sock = MoaiConnection_I_sock( mcn );
			ZnkSocket O_sock = MoaiConnection_O_sock( mcn );
			const char* arrow = sock_type == MoaiSockType_e_Inner ? "=>" :
				sock_type == MoaiSockType_e_Outer ? "<=" : "<=>";

			if( port == 80 ){
				Znk_snprintf( buf, buf_size, "%s(%d%s%d)",
						hostname, I_sock, arrow, O_sock );
			} else {
				Znk_snprintf( buf, buf_size, "%s:%d(%d%s%d)",
						hostname, port, I_sock, arrow, O_sock );
			}
		} else {
			Znk_snprintf( buf, buf_size, "%d(%c)", sock, getSockTypeCh( sock_type ) );
		}
	} else {
		Znk_snprintf( buf, buf_size, "%d(%c)", sock, getSockTypeCh( sock_type ) );
	}
	return buf;
}
void
MoaiIO_addAnalyzeLabel( ZnkStr msgs, ZnkSocket sock, int result_size, const char* label )
{
	char scbuf[ 1024 ];
	ZnkStr_addf( msgs, "  %s : sock=[%s] result_size=[%d] : ",
			label, MoaiIO_makeSockStr( scbuf, sizeof(scbuf), sock, true ),result_size );
}

static bool
isReported( ZnkObjAry repoted, MoaiConnection query_mcn )
{
	const size_t size = ZnkObjAry_size( repoted );
	size_t idx;
	MoaiConnection mcn = NULL;
	for( idx=0; idx<size; ++idx ){
		mcn = (MoaiConnection)ZnkObjAry_at( repoted, idx );
		if( query_mcn == mcn ){
			return true;
		}
	}
	return false;
}
static MoaiConnection
findReportingConnection( ZnkObjAry repoted, ZnkSocket sock )
{
	MoaiConnection mcn = NULL;
	mcn  = MoaiConnection_find_byISock( sock );
	if( mcn ){
		return isReported( repoted, mcn ) ? NULL : mcn;
	}
	mcn  = MoaiConnection_find_byOSock( sock );
	if( mcn ){
		return isReported( repoted, mcn ) ? NULL : mcn;
	}
	return NULL;
}

void
MoaiIO_reportFdst( const char* label, ZnkSocketAry sock_ary, ZnkFdSet fdst_observe, bool detail )
{
	size_t sock_ary_size;

	ZnkSocketAry_clear( sock_ary );
	ZnkFdSet_getSocketAry( fdst_observe, sock_ary );
	sock_ary_size = ZnkSocketAry_size( sock_ary );

	if( sock_ary_size ){
		bool   first_printed = false;
		MoaiConnection mcn = NULL;
		size_t line_leng = 0;
		ZnkSocket sock;
		size_t    i;
		char      buf[ 1024 ] = "";
		ZnkObjAry repoted = ZnkObjAry_create( NULL );
		RanoLog_printf("%s : sock=[", label );
		line_leng = Znk_strlen( label ) + 9;
	
		for( i=0; i<sock_ary_size; ++i ){
			sock = ZnkSocketAry_at( sock_ary, i );
			if( MoaiConnection_isListeningSock( sock ) ){
				mcn = NULL;
			} else {
				mcn = findReportingConnection( repoted, sock );
				if( mcn == NULL ){
					continue;
				}
				ZnkObjAry_push_bk( repoted, (ZnkObj)mcn );
			}
	
			if( first_printed ){
				RanoLog_printf(", ");
				line_leng += 2;
			}
			MoaiIO_makeSockStr( buf, sizeof(buf), sock, detail );
			if( line_leng + Znk_strlen( buf ) > 80 ){
				RanoLog_printf( "\n    " );
				line_leng = 4;
			}
			RanoLog_printf( "%s", buf );
			line_leng += Znk_strlen( buf );
			first_printed = true;
		}
		RanoLog_printf("]\n" );
		ZnkObjAry_destroy( repoted );
	}

}

bool
MoaiIO_procExile( ZnkSocketAry sock_ary, ZnkFdSet fdst, MoaiFdSet mfds )
{
	size_t    i;
	size_t    sock_ary_size;
	ZnkSocket sock;
	MoaiConnection mcn = NULL;
	bool deletion_done = false;

	ZnkSocketAry_clear( sock_ary );
	ZnkFdSet_getSocketAry( fdst, sock_ary );
	sock_ary_size = ZnkSocketAry_size( sock_ary );

	for( i=0; i<sock_ary_size; ++i ){
		sock  = ZnkSocketAry_at( sock_ary, i );
		mcn = MoaiConnection_find_byISock( sock );
		if( mcn ){
			ZnkSocket O_sock = MoaiConnection_O_sock( mcn );
			if( O_sock != ZnkSocket_INVALID ){
				continue;
			}
			/* This is Exile socket */
			if( (uint64_t)getCurrentSec() - mcn->exile_time_ > 15 ){
				RanoLog_printf( "Moai : Exile[%d] deletion by timeout.\n", sock );
				ZnkSocket_close( sock );
				MoaiConnection_erase( mcn, mfds );
				ZnkFdSet_clr( fdst, sock );
				return deletion_done = true;
			}
		}
	}
	return deletion_done;
}

void
MoaiIO_close_ISock( const char* label, ZnkSocket sock, MoaiFdSet mfds )
{
	char buf[ 1024 ];
	RanoLog_printf( "  MoaiIO_close_ISock : %s sock=[%s]\n",
			label, MoaiIO_makeSockStr(buf,sizeof(buf),sock,true) );

	if( !MoaiConnection_isListeningSock( sock ) ){
		MoaiConnection mcn = NULL;
		if( (mcn = MoaiConnection_find_byISock( sock )) != NULL ){
			ZnkFdSet fdst_observe_r = MoaiFdSet_fdst_observe_r( mfds );
			ZnkFdSet_clr( fdst_observe_r,sock );
			ZnkSocket_close( sock );
			mcn->I_sock_ = ZnkSocket_INVALID;
		}
	}
}
void
MoaiIO_close_OSock( const char* label, ZnkSocket sock, MoaiFdSet mfds )
{
	char buf[ 1024 ];
	RanoLog_printf( "  MoaiIO_close_OSock : %s sock=[%s]\n",
			label, MoaiIO_makeSockStr(buf,sizeof(buf),sock,true) );

	if( !MoaiConnection_isListeningSock( sock ) ){
		MoaiConnection mcn = NULL;
		if( (mcn = MoaiConnection_find_byOSock( sock )) != NULL ){
			ZnkFdSet fdst_observe_r = MoaiFdSet_fdst_observe_r( mfds );
			MoaiFdSet_removeConnectingSock( mfds, sock );
			ZnkFdSet_clr( fdst_observe_r, sock );
			ZnkSocket_close( sock );
			mcn->O_sock_ = ZnkSocket_INVALID;
		}
	}
}

/***
 * listening sockを除き、fdst_observe内の全sockをクローズ.
 */
void
MoaiIO_closeSocketAll( const char* label, ZnkFdSet fdst_observe, MoaiFdSet mfds )
{
	size_t    i;
	size_t    sock_ary_size;
	ZnkSocket sock;
	ZnkSocketAry sock_ary = ZnkSocketAry_create();

	ZnkFdSet_getSocketAry( fdst_observe, sock_ary );
	sock_ary_size = ZnkSocketAry_size( sock_ary );

	for( i=0; i<sock_ary_size; ++i ){
		sock = ZnkSocketAry_at( sock_ary, i );
		if( !MoaiConnection_isListeningSock( sock ) ){
			MoaiIO_close_OSock( label, sock, mfds );
			MoaiIO_close_ISock( label, sock, mfds );
		}
	}
	ZnkSocketAry_destroy( sock_ary );
}


bool
MoaiIO_recvInPtn( ZnkBfr stream, ZnkSocket sock, MoaiFdSet mfds, const char* ptn, size_t* result_size )
{
	const char* p = NULL;
	size_t  ptn_leng = Znk_strlen( ptn );
	uint8_t buf[ 4096 ];
	int  recv_size;
	if( result_size ){ *result_size = 0; }
	while( true ){
		p = Znk_memmem( ZnkBfr_data(stream), ZnkBfr_size(stream), ptn, ptn_leng );
		if( p ){
			/* found */
			if( result_size ){
				uint8_t* begin = ZnkBfr_data( stream );
				uint8_t* end   = (uint8_t*)p + ptn_leng;
				*result_size = end - begin;
			}
			break;
		}
		recv_size = ZnkSocket_recv( sock, buf, sizeof(buf) );
		if( recv_size <= 0 ){
			char errmsg_buf[ 4096 ];
			int  err_code = ZnkNetBase_getLastErrCode();
			if( recv_size != 0 ){
				ZnkStr msgs = ZnkStr_new( "" );
				ZnkNetBase_getErrMsg( errmsg_buf, sizeof(errmsg_buf), err_code );
				MoaiIO_addAnalyzeLabel( msgs, sock, recv_size, "recvInPtn" );
				ZnkStr_add( msgs, "RecvError.\n" );
				ZnkStr_addf( msgs, "  errmsg=[%s]\n", errmsg_buf );
				RanoLog_printf( "%s", ZnkStr_cstr( msgs ) );
				MoaiIO_close_ISock( "  RecvError", sock, mfds );
				MoaiIO_close_OSock( "  RecvError", sock, mfds );
				ZnkStr_delete( msgs );
			} else {
				RanoLog_printf( "  recv_size=[%d]\n", recv_size );
				MoaiIO_close_ISock( "  UnexpectedRecvZero(recvInPtn)", sock, mfds );
				MoaiIO_close_OSock( "  UnexpectedRecvZero(recvInPtn)", sock, mfds );
			}
			return false;
		}
		ZnkBfr_append_dfr( stream, buf, recv_size );
	}
	return true;
}
bool
MoaiIO_recvByPtn( ZnkBfr stream, ZnkSocket sock, MoaiFdSet mfds, const char* ptn, size_t* result_size )
{
	const size_t old_bfr_size = ZnkBfr_size(stream);
	const char* p = NULL;
	size_t ptn_leng = Znk_strlen( ptn );
	uint8_t buf[ 4096 ];
	int  recv_size;
	if( result_size ){ *result_size = 0; }
	while( true ){
		p = Znk_memmem( ZnkBfr_data(stream)+old_bfr_size, ZnkBfr_size(stream)-old_bfr_size, ptn, ptn_leng );
		if( p ){
			/* found */
			if( result_size ){
				uint8_t* begin = ZnkBfr_data( stream )+old_bfr_size;
				uint8_t* end   = (uint8_t*)p + ptn_leng;
				*result_size = end - begin;
			}
			break;
		}
		recv_size = ZnkSocket_recv( sock, buf, sizeof(buf) );
		if( recv_size <= 0 ){
			char errmsg_buf[ 4096 ];
			int  err_code = ZnkNetBase_getLastErrCode();
			if( err_code != 0 ){
				ZnkStr msgs = ZnkStr_new( "" );
				ZnkNetBase_getErrMsg( errmsg_buf, sizeof(errmsg_buf), err_code );
				MoaiIO_addAnalyzeLabel( msgs, sock, recv_size, "recvByPtn" );
				ZnkStr_add( msgs, "RecvError.\n" );
				ZnkStr_addf( msgs, "  errmsg=[%s]\n", errmsg_buf );
				RanoLog_printf( "%s", ZnkStr_cstr( msgs ) );
				MoaiIO_close_ISock( "  RecvError", sock, mfds );
				MoaiIO_close_OSock( "  RecvError", sock, mfds );
				ZnkStr_delete( msgs );
			} else {
				MoaiIO_close_ISock( "  analyzeRecv : UnexpectedRecvZero(recvByPtn)", sock, mfds );
				MoaiIO_close_OSock( "  analyzeRecv : UnexpectedRecvZero(recvByPtn)", sock, mfds );
			}
			return false;
		}
		ZnkBfr_append_dfr( stream, buf, recv_size );
	}
	return true;
}
bool
MoaiIO_recvBySize( ZnkBfr stream, ZnkSocket sock, MoaiFdSet mfds, size_t size, size_t* result_size )
{
	const size_t old_bfr_size = ZnkBfr_size(stream);
	uint8_t buf[ 4096 ];
	int  recv_size;
	if( result_size ){ *result_size = 0; }
	while( true ){
		if( ZnkBfr_size(stream) - old_bfr_size >= size ){
			break;
		}
		recv_size = ZnkSocket_recv( sock, buf, sizeof(buf) );
		if( recv_size <= 0 ){
			char errmsg_buf[ 4096 ];
			int  err_code = ZnkNetBase_getLastErrCode();
			if( err_code != 0 ){
				ZnkStr msgs = ZnkStr_new( "" );
				ZnkNetBase_getErrMsg( errmsg_buf, sizeof(errmsg_buf), err_code );
				MoaiIO_addAnalyzeLabel( msgs, sock, recv_size, "recvBySize" );
				ZnkStr_add( msgs, "RecvError.\n" );
				ZnkStr_addf( msgs, "  errmsg=[%s]\n", errmsg_buf );
				RanoLog_printf( "%s", ZnkStr_cstr( msgs ) );
				MoaiIO_close_ISock( "  RecvError", sock, mfds );
				MoaiIO_close_OSock( "  RecvError", sock, mfds );
				ZnkStr_delete( msgs );
			} else {
				MoaiIO_close_ISock( "  analyzeRecv : UnexpectedRecvZero(recvBySize)", sock, mfds );
				MoaiIO_close_OSock( "  analyzeRecv : UnexpectedRecvZero(recvBySize)", sock, mfds );
			}
			return false;
		}
		ZnkBfr_append_dfr( stream, buf, recv_size );
		if( result_size ){ *result_size += recv_size; }
	}
	return true;
}

bool
MoaiIO_recvByZero( ZnkBfr stream, ZnkSocket sock, MoaiFdSet mfds, size_t* result_size )
{
	uint8_t buf[ 4096 ];
	int  recv_size;
	if( result_size ){ *result_size = 0; }
	while( true ){
		recv_size = ZnkSocket_recv( sock, buf, sizeof(buf) );
		if( recv_size <= 0 ){
			char errmsg_buf[ 4096 ];
			int  err_code = ZnkNetBase_getLastErrCode();
			if( err_code != 0 ){
				ZnkStr msgs = ZnkStr_new( "" );
				ZnkNetBase_getErrMsg( errmsg_buf, sizeof(errmsg_buf), err_code );
				MoaiIO_addAnalyzeLabel( msgs, sock, recv_size, "recvByZero" );
				ZnkStr_add( msgs, "RecvError.\n" );
				ZnkStr_addf( msgs, "  errmsg=[%s]\n", errmsg_buf );
				RanoLog_printf( "%s", ZnkStr_cstr( msgs ) );
				MoaiIO_close_ISock( "  RecvError", sock, mfds );
				MoaiIO_close_OSock( "  RecvError", sock, mfds );
				ZnkStr_delete( msgs );
				return false;
			}
			RanoLog_printf( "MoaiIO_recvByZero : RecvZero. OK.\n" );
			break; /* OK. */
		}
		ZnkBfr_append_dfr( stream, buf, recv_size );
		if( result_size ){ *result_size += recv_size; }
	}
	return true;
}

size_t
MoaiIO_recvByPtn2( ZnkBfr stream, ZnkSocket sock, MoaiFdSet mfds, const char* ptn )
{
	const size_t old_bfr_size = ZnkBfr_size(stream);
	const char*  p            = NULL;
	size_t       ptn_leng     = Znk_strlen( ptn );
	uint8_t      buf[ 4096 ];
	int          recv_size;
	size_t       result_size = 0;
	while( true ){
		p = Znk_memmem( ZnkBfr_data(stream)+old_bfr_size, ZnkBfr_size(stream)-old_bfr_size, ptn, ptn_leng );
		if( p ){
			/* found */
			uint8_t* begin = ZnkBfr_data( stream )+old_bfr_size;
			uint8_t* end   = (uint8_t*)p + ptn_leng;
			result_size    = end - begin;
			break;
		}
		recv_size = ZnkSocket_recv( sock, buf, sizeof(buf) );
		if( recv_size == 0 ){
			/* RecvZero */
			RanoLog_printf( "  MoaiIO_recvByPtn2 : RecvZero\n" );
			/* OK : この関数ではこれは成功終了の一種であるとみなし. ここまでrecvしたサイズを返す */
			return ZnkBfr_size( stream ) - old_bfr_size;
		}
		if( recv_size < 0 ){
			char errmsg_buf[ 4096 ];
			int  err_code = ZnkNetBase_getLastErrCode();
			ZnkStr msgs = ZnkStr_new( "" );
			ZnkNetBase_getErrMsg( errmsg_buf, sizeof(errmsg_buf), err_code );
			MoaiIO_addAnalyzeLabel( msgs, sock, recv_size, "recvByPtn" );
			ZnkStr_add( msgs, "RecvError.\n" );
			ZnkStr_addf( msgs, "  errmsg=[%s]\n", errmsg_buf );
			RanoLog_printf( "  MoaiIO_recvByPtn2 : [%s]", ZnkStr_cstr( msgs ) );
			ZnkStr_delete( msgs );
			MoaiIO_close_ISock( "  MoaiIO_recvByPtn2 : Error", sock, mfds );
			MoaiIO_close_OSock( "  MoaiIO_recvByPtn2 : Error", sock, mfds );
			/* Error */
			return Znk_NPOS;
		}
		ZnkBfr_append_dfr( stream, buf, recv_size );
	}
	/* OK */
	return result_size;
}
