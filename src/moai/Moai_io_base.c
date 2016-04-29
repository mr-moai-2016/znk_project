#include "Moai_io_base.h"
#include "Moai_connection.h"
#include "Moai_log.h"
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
	strftime( buf, buf_size, "%a, %d %b %Y %T GMT", timePtr );
}
static void
makeResponseHdr_200( ZnkStr str, const char* content_type, int contentLength, int modTime )
{
	char timeBuf[ 128 ];
	formatTimeString( timeBuf, sizeof(timeBuf), 0 );

	ZnkStr_addf( str,
			"HTTP/1.0 200 OK\r\n"
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
MoaiIO_sendResponseFile( ZnkSocket sock, const char* filename )
{
	bool result = false;
	ZnkFile fp = ZnkF_fopen( filename, "rb" );
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
			read_size = ZnkF_fread( buf, 1, sizeof(buf), fp );
			if( read_size == 0 ){
				break; /* eof */
			}
			ZnkBfr_append_dfr( bfr, buf, read_size );
		}
		makeResponseHdr_200( str, content_type, ZnkBfr_size(bfr), 0 );
		ZnkBfr_insert( bfr, 0, (uint8_t*)ZnkStr_cstr(str), ZnkStr_leng(str) );
		if( ZnkSocket_send( sock, ZnkBfr_data(bfr), ZnkBfr_size(bfr) ) < 0 ){
			result = false;
		}
		ZnkBfr_destroy( bfr );
		ZnkF_fclose( fp );
		result = true;
	}
	return result;
}


Znk_INLINE double
getCurrentSec( void ){ return ( 1.0 / CLOCKS_PER_SEC ) * clock(); }

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
	MoaiConnection cinfo  = MoaiConnection_find( sock );
	bool broken_link = false;
	if( cinfo ){
		if( detail
		  && cinfo->sock_type_ != MoaiSockType_e_Listen
		  && cinfo->sock_type_ != MoaiSockType_e_None
		  && !ZnkS_empty(cinfo->hostname_) )
		{
			MoaiConnection I_cnct = NULL;
			MoaiConnection O_cnct = NULL;
			ZnkSocket I_sock = ZnkSocket_INVALID;
			ZnkSocket O_sock = ZnkSocket_INVALID;
			char I_waiting = '?';
			char O_waiting = '?';
			switch( cinfo->sock_type_ ){
			case MoaiSockType_e_Inner:
			{
				I_cnct    = cinfo;
				I_sock    = I_cnct->src_sock_;
				I_waiting = I_cnct->waiting_ ? 'w' : ' ';
				O_cnct    = MoaiConnection_find( I_cnct->dst_sock_ );
				if( O_cnct ){
					broken_link = (bool)( O_cnct->src_sock_ != I_cnct->dst_sock_ );
					O_sock    = O_cnct->src_sock_;
					O_waiting = O_cnct->waiting_ ? 'w' : ' ';
				}
				break;
			}
			case MoaiSockType_e_Outer:
			{
				O_cnct    = cinfo;
				O_sock    = O_cnct->src_sock_;
				O_waiting = O_cnct->waiting_ ? 'w' : ' ';
				I_cnct    = MoaiConnection_find( O_cnct->dst_sock_ );
				if( I_cnct ){
					broken_link = (bool)( O_cnct->src_sock_ != I_cnct->dst_sock_ );
					I_sock    = I_cnct->src_sock_;
					I_waiting = I_cnct->waiting_ ? 'w' : ' ';
				}
				break;
			}
			default:
				assert( 0 );
				break;
			}

			if( cinfo->port_ == 80 ){
				Znk_snprintf( buf, buf_size, "%s(%s%d%c=>%d%c)", cinfo->hostname_,
						broken_link ? "Broken:" : "",
						I_sock, I_waiting, O_sock, O_waiting );
			} else {
				Znk_snprintf( buf, buf_size, "%s:%d(%s%d%c=>%d%c)", cinfo->hostname_, cinfo->port_,
						broken_link ? "Broken:" : "",
						I_sock, I_waiting, O_sock, O_waiting );
			}
		} else {
			Znk_snprintf( buf, buf_size, "%d(%c)", sock, getSockTypeCh( cinfo->sock_type_ ) );
		}
	} else {
		Znk_snprintf( buf, buf_size, "%d(N)", sock );
	}
	return buf;
}
void
MoaiIO_addAnalyzeLabel( ZnkStr msgs, ZnkSocket sock, int result_size )
{
	char scbuf[ 1024 ];
	ZnkStr_addf( msgs, "  AnalyzeRecv : sock=[%s] result_size=[%d] : ",
			MoaiIO_makeSockStr( scbuf, sizeof(scbuf), sock, true ),result_size );
}

void
MoaiIO_reportFdst( const char* label, ZnkSocketDAry sock_dary, ZnkFdSet fdst, bool detail )
{
	size_t    i;
	size_t    sock_dary_size;
	ZnkSocket sock;
	char      buf[ 1024 ] = "";
	ZnkSocketDAry repoted = ZnkSocketDAry_create();
	MoaiConnection cinfo = NULL;
	size_t line_leng = 0;
	bool first_printed = false;

	ZnkSocketDAry_clear( sock_dary );
	ZnkFdSet_getSocketDAry( fdst, sock_dary );
	sock_dary_size = ZnkSocketDAry_size( sock_dary );

	MoaiLog_printf("%s : sock=[", label );
	line_leng = Znk_strlen( label ) + 9;

	for( i=0; i<sock_dary_size; ++i ){
		sock  = ZnkSocketDAry_at( sock_dary, i );
		if( ZnkSocketDAry_find( repoted, sock ) != Znk_NPOS ){
			continue;
		}
		ZnkSocketDAry_push_bk( repoted, sock );

		cinfo = MoaiConnection_find( sock );
		if( cinfo ){
			if( ZnkSocketDAry_find( repoted, cinfo->dst_sock_ ) == Znk_NPOS ){
				if( ZnkSocketDAry_find( sock_dary, cinfo->dst_sock_ ) != Znk_NPOS ){
					/* sock_daryにも確かに存在する */
					ZnkSocketDAry_push_bk( repoted, cinfo->dst_sock_ );
				} else {
					/***
					 * cinfo->src_sock_がObserveされているにも関わらず
					 * cinfo->dst_sock_がObserveされていない.
					 * この場合はcinfo->dst_sock_は予約されている可能性がある.
					 */
				}
			} else {
				/***
				 * 異常:
				 * cinfo->dst_sock_がreported済みなら、本来cinfo->src_sock_もreportedされており、
				 * その処理はもう終了しているはず.
				 * 従って、ここに来た場合、dst_sock_とsrc_sock_の相互参照が壊れている.
				 * MoaiIO_makeSockStrはこの場合特別な表示をするようになっているので、
				 * とりあえず表示はそれに任せる.
				 */
			}
		}

		if( first_printed ){
			MoaiLog_printf(", ");
			line_leng += 2;
		}
		MoaiIO_makeSockStr( buf, sizeof(buf), sock, detail );
		if( line_leng + Znk_strlen( buf ) > 80 ){
			MoaiLog_printf( "\n    " );
			line_leng = 4;
		}
		MoaiLog_printf( "%s", buf );
		line_leng += Znk_strlen( buf );
		first_printed = true;
	}
	MoaiLog_printf("]\n" );

	ZnkSocketDAry_destroy( repoted );
}

bool
MoaiIO_procExile( ZnkSocketDAry sock_dary, ZnkFdSet fdst )
{
	size_t    i;
	size_t    sock_dary_size;
	ZnkSocket sock;
	MoaiConnection cinfo = NULL;
	bool deletion_done = false;

	ZnkSocketDAry_clear( sock_dary );
	ZnkFdSet_getSocketDAry( fdst, sock_dary );
	sock_dary_size = ZnkSocketDAry_size( sock_dary );

	for( i=0; i<sock_dary_size; ++i ){
		sock  = ZnkSocketDAry_at( sock_dary, i );
		cinfo = MoaiConnection_find( sock );
		if( cinfo ){
			/***
			 * TODO:
			 * Timeout処理として以下のような安易な実装ではまずい.
			 * これだと動画サイトなどで長時間のストリーミング再生をする時に
			 * 途中でTimeoutが働き、必要なコネクションがcloseされてしまう.
			 * もっとマシな方法を検討する必要がある.
			 */
#if 0
			if( cinfo->waiting_ ){
				static const int st_wait_sec_max = 480;
				if( (uint64_t)getCurrentSec() - cinfo->waiting_ > st_wait_sec_max ){
					/***
					 * いずれか一方で待ち時間の限界を向かえた場合
					 * 双方ともに閉じ、かつ完全クリアする.
					 */
					MoaiConnection dst_cnct = MoaiConnection_find( cinfo->dst_sock_ );
					ZnkSocket_close( cinfo->src_sock_ );
					ZnkSocket_close( dst_cnct->src_sock_ );
					ZnkFdSet_clr( fdst, cinfo->src_sock_ );
					ZnkFdSet_clr( fdst, dst_cnct->src_sock_ );
					MoaiConnection_clear( cinfo );
					MoaiConnection_clear( dst_cnct );
					MoaiLog_printf( "Moai : Waiting[%d] deletion by timeout.\n", sock );
					return deletion_done = true;
				}
			}
#endif
			if( cinfo->sock_type_ != MoaiSockType_e_Inner ){
				continue;
			}
			if( cinfo->dst_sock_ != ZnkSocket_INVALID ){
				continue;
			}
			/* This is Exile socket */
			if( (uint64_t)getCurrentSec() - cinfo->exile_time_ > 15 ){
				MoaiLog_printf( "Moai : Exile[%d] deletion by timeout.\n", sock );
				ZnkSocket_close( sock );
				MoaiConnection_clear( cinfo );
				ZnkFdSet_clr( fdst, sock );
				return deletion_done = true;
			}
		}
	}
	return deletion_done;
}

void
MoaiIO_closeSocket( const char* label, ZnkSocket sock, ZnkFdSet fdst_observe )
{
	/***
	 * ここでパートナーとなるsocketをどうするかという問題がある.
	 * 以下変数名との混同を避けるため、sockの値を具体的に1900として考える.
	 *
	 * 問題1.
	 * このとき、少なくともcinfo->dst_sock_が1900に等しいようなcinfoは
	 * そのcinfo->dst_sock_の値をZnkSocket_INVALIDに戻しておくべきである.
	 * さもないと次のシナリオが発生し得る.
	 * 1. まずx(I)<=>1900(O)という接続が存在したとする.
	 * 2. 1900(O)のRecvZeroを検知し、1900をclose 
	 * 3. このときxのcinfo->dst_sock_に1900の値を今仮にそのまま残していたとする.
	 * 4. I側からの新しい接続y(I)が発生. ZnkSocket_openによりソケットが新規作成されたとする.
	 *    この新規作成ソケットの値として直前にcloseした1900が再利用される状況は割と頻繁に発生し得る.
	 * 5. x(I)のcinfo->dst_sock_が想定する接続先と、y(I)のcinfo->dst_sock_が想定する接続先は
	 *    一般に異なる. にも関わらずこれらのdst_sock_は同じ1900という値となっている.
	 *    つまり二つのI側ソケットに対し、一つのO側ソケットが対応するという異常状態となる.
	 * 6. x(I) はO側との接続を失ったExile(浮浪)Socketであるが、まだI(Browser)側とcloseしていない.
	 *    Browserはこのソケットを新しい接続として再利用することがある.
	 *    そのとき、x(I)は1900と中継されるが、ほぼ同時にy(I)からの接続が発生した場合:
	 *    6-1. x(I)からの接続先Hostがy(I)からの接続先Hostと異なる場合、片方の通信先が
	 *         誤ったものとなってしまい明らかにまずい.
	 *    6-2. x(I)からの接続先Hostがy(I)からの接続先Hostと同じである場合、一見問題なさそうだが
	 *         同じ1900というsocketに例えば二つのHTTP GETリクエストなどがほぼ同時に送られる.
	 *         そうすると1900からrecvされるレスポンスがこれらのリクエストを連結したものとして
	 *         送り返されてしまう. つまり一回のrecvによって取得されるデータ隗の中にHTTPレスポンス
	 *         の開始位置が二つ含まれるといったような異常形式が受信され得る.
	 *         これはレスポンスの終了の検知の解析処理に混乱をもたらす.
	 *
	 * 問題2.
	 * では、cinfo->dst_sock_が1900に等しいようなcinfoに対応するsock(つまり相手となるsock)は
	 * closeもしておくべきか？
	 * これはもし相手がI側sockであれば、closeしてもよいし効率のため、接続を維持してもよい. 
	 * もし相手がO側sockであればそのO側sockはcloseすべきである. これはO側sockが単にネットワークの
	 * トラフィック上の理由などにより、受信に時間がかかって遅れていた場合、その間に新しいI側からの
	 * 接続が発生し、これがもしそのまだ接続が生きているO側sockを(接続先Hostが同じだからということで)
	 * closeせずにそのまま再利用したとすると、次の新しい接続のレスポンスの受信において
	 * 直前の(もはや何の意味もない)接続のレスポンスの最終部分が前部に連結された状態で受信される
	 * シナリオが考えられるからである. これはレスポンスの終了の検知の解析処理に混乱をもたらす.
	 *
	 * 特にO側へのconnectToServer処理は時間がかかることがある(Blocking Socketの場合は特に)
	 * O側でも明らかに全受信が完了したことが保障できる状況に限り、接続を維持してもよいのではないか?
	 * つまりそのような場合にそれをExile Socketとして、次のconnectToServerの機会にそれを
	 * 再利用する形にして高速化を図りたい.
	 */
	char buf[ 1024 ];
	MoaiLog_printf( "%s : close sock=[%s]\n", label, MoaiIO_makeSockStr(buf,sizeof(buf),sock,true) );
	ZnkSocket_close( sock );
	/***
	 * ここでまずsockに対するcinfoをfindし、cinfo->dst_sock_を取得し、
	 * cinfo->dst_sock_が(O)側であればこれをcloseする.
	 * cinfo->dst_sock_が(I)側であればこれの接続は維持したままにするが、
	 * cinfo->dst_sock_に対するrinfoをfindしてこれのrinfo->dst_sock_の値をZnkSocket_INVALIDにしておくべき.
	 */
	{
		MoaiConnection cinfo = MoaiConnection_find( sock );
		if( cinfo ){
			ZnkSocket dst_sock = cinfo->dst_sock_;
			if( dst_sock != ZnkSocket_INVALID ){
				MoaiConnection rinfo = MoaiConnection_find( dst_sock );
				if( rinfo->sock_type_ == MoaiSockType_e_Outer || rinfo->sock_type_ == MoaiSockType_e_None ){
					/* closeした上に完全抹消 */
					ZnkSocket_close( dst_sock );
					MoaiConnection_clear( rinfo );
					ZnkFdSet_clr( fdst_observe, dst_sock );
				} else if( rinfo->sock_type_ == MoaiSockType_e_Inner ){
					/* dst_sockをExile化する */
					rinfo->dst_sock_ = ZnkSocket_INVALID;
					//rinfo->is_tunneling_ = false;
				}
			}
			/***
			 * さらにcinfo->dst_sock_の値がsockと等しいようなcinfoを全検索して、
			 * それらのdst_sock_の値もZnkSocket_INVALIDに修正するような処理を
			 * 付け加えた方がよいのか...?
			 */
		}
	}
	MoaiConnection_clear_bySock( sock );
	ZnkFdSet_clr( fdst_observe, sock );
}

/***
 * listening sockを除き、fdst_observe内の全sockをクローズ.
 */
void
MoaiIO_closeSocketAll( const char* label, ZnkFdSet fdst_observe )
{
	size_t    i;
	size_t    sock_dary_size;
	ZnkSocket sock;
	ZnkSocketDAry sock_dary = ZnkSocketDAry_create();
	MoaiConnection cinfo;

	ZnkFdSet_getSocketDAry( fdst_observe, sock_dary );
	sock_dary_size = ZnkSocketDAry_size( sock_dary );

	for( i=0; i<sock_dary_size; ++i ){
		sock = ZnkSocketDAry_at( sock_dary, i );
		cinfo = MoaiConnection_find( sock );
		if( cinfo && cinfo->sock_type_ != MoaiSockType_e_Listen ){
			MoaiIO_closeSocket( label, sock, fdst_observe );
		}
	}
	ZnkSocketDAry_destroy( sock_dary );
}


bool
MoaiIO_recvInPtn( ZnkBfr stream, ZnkSocket sock, ZnkFdSet fdst_observe, const char* ptn, size_t* result_size )
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
			if( err_code != 0 ){
				ZnkStr msgs = ZnkStr_new( "" );
				ZnkNetBase_getErrMsg( errmsg_buf, sizeof(errmsg_buf), err_code );
				MoaiIO_addAnalyzeLabel( msgs, sock, recv_size );
				ZnkStr_addf( msgs, "RecvError.\n" );
				ZnkStr_addf( msgs, "  errmsg=[%s]\n", errmsg_buf );
				MoaiLog_printf( "%s", ZnkStr_cstr( msgs ) );
				MoaiIO_closeSocket( "  RecvError", sock, fdst_observe );
				ZnkStr_delete( msgs );
			} else {
				MoaiLog_printf( "  recv_size=[%d]\n", recv_size );
				MoaiIO_closeSocket( "  UnexpectedRecvZero(recvInPtn)", sock, fdst_observe );
			}
			return false;
		}
		ZnkBfr_append_dfr( stream, buf, recv_size );
	}
	return true;
}
bool
MoaiIO_recvByPtn( ZnkBfr stream, ZnkSocket sock, ZnkFdSet fdst_observe, const char* ptn, size_t* result_size )
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
				MoaiIO_addAnalyzeLabel( msgs, sock, recv_size );
				ZnkStr_addf( msgs, "RecvError.\n" );
				ZnkStr_addf( msgs, "  errmsg=[%s]\n", errmsg_buf );
				MoaiLog_printf( "%s", ZnkStr_cstr( msgs ) );
				MoaiIO_closeSocket( "  RecvError", sock, fdst_observe );
				ZnkStr_delete( msgs );
			} else {
				MoaiIO_closeSocket( "  analyzeRecv : UnexpectedRecvZero(recvByPtn)", sock, fdst_observe );
			}
			return false;
		}
		ZnkBfr_append_dfr( stream, buf, recv_size );
	}
	return true;
}
bool
MoaiIO_recvBySize( ZnkBfr stream, ZnkSocket sock, ZnkFdSet fdst_observe, size_t size, size_t* result_size )
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
				MoaiIO_addAnalyzeLabel( msgs, sock, recv_size );
				ZnkStr_addf( msgs, "RecvError.\n" );
				ZnkStr_addf( msgs, "  errmsg=[%s]\n", errmsg_buf );
				MoaiLog_printf( "%s", ZnkStr_cstr( msgs ) );
				MoaiIO_closeSocket( "  RecvError", sock, fdst_observe );
				ZnkStr_delete( msgs );
			} else {
				MoaiIO_closeSocket( "  analyzeRecv : UnexpectedRecvZero(recvBySize)", sock, fdst_observe );
			}
			return false;
		}
		ZnkBfr_append_dfr( stream, buf, recv_size );
		if( result_size ){ *result_size += recv_size; }
	}
	return true;
}

bool
MoaiIO_recvByZero( ZnkBfr stream, ZnkSocket sock, ZnkFdSet fdst_observe, size_t* result_size )
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
				MoaiIO_addAnalyzeLabel( msgs, sock, recv_size );
				ZnkStr_addf( msgs, "RecvError.\n" );
				ZnkStr_addf( msgs, "  errmsg=[%s]\n", errmsg_buf );
				MoaiLog_printf( "%s", ZnkStr_cstr( msgs ) );
				MoaiIO_closeSocket( "  RecvError", sock, fdst_observe );
				ZnkStr_delete( msgs );
				return false;
			}
			MoaiLog_printf( "MoaiIO_recvByZero : RecvZero. OK.\n" );
			break; /* OK. */
		}
		ZnkBfr_append_dfr( stream, buf, recv_size );
		if( result_size ){ *result_size += recv_size; }
	}
	return true;
}

