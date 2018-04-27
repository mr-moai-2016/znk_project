#include "Znk_socket.h"
#include "Znk_net_base.h"
#include "Znk_sys_errno.h"
#include "Znk_stdc.h"
#include "Znk_thread.h"
#include "Znk_missing_libc.h"

#if defined(Znk_TARGET_WINDOWS)
#  include <winsock2.h>

#elif defined(Znk_TARGET_UNIX)
#  include <sys/socket.h> /* for socket,connect,bind,listen,accept,shutdown,recv,send etc. */
#  include <unistd.h>     /* for close */
#  include <fcntl.h>      /* for fcntl etc. */
#  include <arpa/inet.h>  /* for inet_* function (inet_addr etc) */
#  include <netdb.h>      /* for gethostbyname, getservbyname */
#  include <netinet/in.h> /* for struct sockaddr_in, htons etc. */

#else
#endif

#include <errno.h>
#include <assert.h>


ZnkSocket
ZnkSocket_open( void )
{
	/* Win32/X11 で記述方法は全く同じ */
	return socket(AF_INET, SOCK_STREAM, 0);
}

void ZnkSocket_close( ZnkSocket sock )
{
	assert( sock != 0 );
#if defined(Znk_TARGET_WINDOWS)
	closesocket( sock );
#else
	close( sock );
#endif
}

ZnkSocket
ZnkSocket_accept( ZnkSocket listen_sock )
{
	struct sockaddr addr = { 0 };
	/***
	 * 以下のaddr_leng値の初期化は必須である.
	 * accept開始時、まずは入力としてこの値を参照する.
	 * またaccept終了時、出力として実際に取得されたaddrのバッファサイズをここに上書きする.
	 * この出力値は、場合によってはsizeof(struct sockaddr)よりも大きくもなり得る.
	 */
#if defined(Znk_TARGET_WINDOWS)
	int addr_leng = sizeof(struct sockaddr);
#else
	socklen_t addr_leng = sizeof(struct sockaddr);
#endif
	return accept( listen_sock, &addr, &addr_leng );
}

bool
ZnkSocket_setBlockingMode( ZnkSocket sock, bool is_blocking_mode )
{
#if defined(Znk_TARGET_WINDOWS)
	/***
	 * ソケットのデフォルトはブロッキングモードである.
	 * val = 1 で下記を呼び出すことにより非ブロッキングモードに変更できる.
	 * val = 0 でブロッキングモードに設定しなおすことも可能.
	 */
	unsigned long val = is_blocking_mode ? 0 : 1;
	int rc = ioctlsocket( sock, FIONBIO, &val );
	return (bool)( rc == 0 );
#else
    int rc = fcntl( sock, F_GETFL, 0 );
    if( rc < 0 ) return false;

    rc = fcntl( sock, F_SETFL, is_blocking_mode ? (rc & ~O_NONBLOCK) : (rc | O_NONBLOCK) );
    if( rc < 0 ) return false;
    return true;
#endif
}


/***
 * serverには sin_familyとsin_portは設定済みであるとする.
 */
Znk_INLINE bool
connectCore( ZnkSocket sock,
		struct sockaddr_in* server, const char* hostname, ZnkErr* zkerr, bool* is_inprogress )
{
	int rc;
	*is_inprogress = false;
#if defined(Znk_TARGET_WINDOWS)

	/***
	 * hostnameがIP(v4)の場合、inet_addrは通常成功するはずである.
	 * (単に8byte整数４つの組み合わせからなる文字列を32bit値へ変換しているにすぎない)
	 * しかし、hostnameがいわゆるIP以外のURLである場合、これは通常失敗する.
	 * このとき、この戻り値は 0xffffffff となる.
	 * この場合は、gethostbyname により、名前解決を試みる.
	 */
	server->sin_addr.S_un.S_addr = inet_addr( hostname );
	if( server->sin_addr.S_un.S_addr == 0xffffffff ){
		unsigned int** addrptr;
		/* inet_addr()が失敗したとき(hostnameにはURLが指定されていたものと思われる) */
		struct hostent* hostent_ptr = gethostbyname(hostname);
		if( hostent_ptr == NULL ){
			char errmsg_buf[ 4096 ];
			ZnkNetBase_getErrMsg( errmsg_buf, sizeof(errmsg_buf), WSAGetLastError() );
			ZnkErr_internf( zkerr,
					"ZnkSocket_connectToServer : Failure : gethostbyname(DNS) hostname=[%s] WSAErr=[%s]",
					hostname, errmsg_buf );
			return false;
		}

		/* ここではより厳密に h_addr_list にあるすべての結果を一つずつ試し、
		 * 最初に成功したものを採用する.
		 */
		addrptr = (unsigned int **)hostent_ptr->h_addr_list;
		while( *addrptr != NULL ){
			server->sin_addr.S_un.S_addr = *(*addrptr);
			/* connect()が成功したらloopを抜ける */
			rc = connect(sock, (struct sockaddr *)server, sizeof(struct sockaddr_in));
			if( rc == 0 ){
				/* 成功. これを採用 */
				break;
			}
			/***
			 * 非ブロッキングIOの場合.
			 * この場合はWSAEWOULDBLOCKとなっていたらis_inprogressとみなす.
			 * 戻り値はtrueとする.
			 */
			if( WSAGetLastError() == WSAEWOULDBLOCK ){
				*is_inprogress = true;
				return true;
			}
			++addrptr;
		}
		/* connectが全て失敗した場合 */
		if( *addrptr == NULL ){
			/* とりあえずエラーメッセージでは一番最後のIPで報告する */
			char errmsg_buf[ 4096 ];
			ZnkNetBase_getErrMsg( errmsg_buf, sizeof(errmsg_buf), WSAGetLastError() );
			ZnkErr_internf( zkerr, "ZnkSocket_connectToServer : Failure : connect IP=[%08x] WSAErr=[%s]",
					server->sin_addr.S_un.S_addr, errmsg_buf );
			return false;
		}
		
	} else {
		/* inet_addr()が成功したとき(hostnameにはIPアドレスが直に指定されていたものと思われる) */
		rc = connect( sock, (struct sockaddr *)server, sizeof(struct sockaddr_in) );
		if( rc != 0 ){
			/***
			 * 非ブロッキングIOの場合.
			 * この場合はWSAEWOULDBLOCKとなっていたらis_inprogressとみなす.
			 * 戻り値はtrueとする.
			 */
			if( WSAGetLastError() == WSAEWOULDBLOCK ){
				*is_inprogress = true;
			} else {
				char errmsg_buf[ 4096 ];
				ZnkNetBase_getErrMsg( errmsg_buf, sizeof(errmsg_buf), WSAGetLastError() );
				ZnkErr_internf( zkerr, "ZnkSocket_connectToServer : Failure : connect IP=[%08x] WSAErr=[%s]",
						server->sin_addr.S_un.S_addr, errmsg_buf );
				return false;
			}
		}
	}
	return true;

#elif defined(Znk_TARGET_UNIX)

	/* ホスト名とIP アドレスを扱うための構造体のポインタ(gethostbynameの戻り値) */
	struct hostent* servhost = gethostbyname( hostname );
	Znk_UNUSED( rc );
	if( servhost == NULL ){
		ZnkSysErrnoInfo* err_info = ZnkSysErrno_getInfo( ZnkSysErrno_errno() );
		ZnkErr_internf( zkerr,
				"ZnkSocket_connectToServer : Failure : gethostbyname hostname=[%s] SysErr=[%s:%s]",
				hostname, err_info->sys_errno_key_, err_info->sys_errno_msg_ );
		return false;
	}

	/* sin_addr(IPアドレス) */
	Znk_memcpy( &(server->sin_addr), servhost->h_addr, servhost->h_length ); 
	if( connect(sock, (struct sockaddr*)server, sizeof(struct sockaddr_in) ) != 0 ){
		if( errno == EINPROGRESS ){
			*is_inprogress = true;
			return true;
		}
		ZnkSysErrnoInfo* err_info = ZnkSysErrno_getInfo( ZnkSysErrno_errno() );
		ZnkErr_internf( zkerr,
				"ZnkSocket_connectToServer : Failure : connect IP=[%08x] SysErr=[%s:%s]",
				server->sin_addr.s_addr, err_info->sys_errno_key_, err_info->sys_errno_msg_ );
		return false;
	}
	return true;
#endif
}

bool
ZnkSocket_connectToServer( ZnkSocket sock, const char* hostname, uint16_t port, ZnkErr* zkerr, bool* is_inprogress )
{
	struct sockaddr_in server;   /* ソケットを扱うための構造体 */

	/* ゼロクリア */
	Znk_memset( &server, 0, sizeof(server) );

	/* sin_family */
	server.sin_family = AF_INET;

	/* port */
	server.sin_port = htons(port);

	return connectCore( sock, &server, hostname, zkerr, is_inprogress );
}

/***
 * サービス名よりport番号を得る.
 * ただし失敗した場合は0を返す.
 */
uint16_t
ZnkSocket_getPort_byTCPName( const char* service_name ) 
{
	/* サービス (http など) を扱うための構造体 */
	struct servent* service = getservbyname( service_name, "tcp" );
	if( service == NULL ){
		/* 失敗 */
		return 0;
	}
	/* 成功.
	 * おそらくnetwork byteorder で取得されるようなので、
	 * Net=>Host(short) で変換をかけておく. 
	 */
	return ntohs( service->s_port );
}

#if defined(Znk_TARGET_WINDOWS)
/***
 * WSAGetLastErrorの値およびerrnoは、マルチスレッド版ライブラリでは
 * スレッド毎に実体がある(つまりthread-safeである).
 */
static void
set_errno_forWin( int winsock_err )
{
	switch(winsock_err) {
	case WSAEWOULDBLOCK:
		errno = EAGAIN;
		break;
	default:
		errno = winsock_err;
		break;
	}
}
#endif

/***
 * 以下のようにしていた時期があったがどうやら最初の数回のwait間隔を
 * あまりに短くするとEAGAINが連発する不具合が発生するようである.
 * 間隔を若干長めにとるとこれが起こりにくくなる.
 * またwaiting_countは無限に許容せず、トータルで15秒程度になる回数になったら
 * 諦めて rc < 0 をブラウザに返す方が結局は全体的に速度が大幅に増すようである.
 *
 * if( waiting_count < 13 ){
 *   ZnkThread_sleep( 200 + waiting_count*100 );
 * } else {
 *   ZnkThread_sleep( 1500 );
 * }
 */
static void
wait_depending_on_count( size_t waiting_count )
{
	if( waiting_count < 9 ){
		ZnkThread_sleep( 200 + waiting_count*200 );
	} else {
		ZnkThread_sleep( 2000 );
	}
}

static bool
report( int rc, uint32_t sys_errno, size_t waiting_count, void* arg )
{
	bool is_again = false;

	if( rc < 0 ){
		if( sys_errno == EAGAIN ){
			/***
			 * 何回か試行してダメなら取り消し.
			 */
			if( waiting_count > 12 ){
				is_again = false;
			} else {
				wait_depending_on_count( waiting_count );
				if( waiting_count == 0 ){
					const char* msg_from = Znk_force_ptr_cast( const char*, arg );
					Znk_printf_e( "  %s : EAGAIN count=", msg_from );
				} else {
					Znk_printf_e( "\b\b\b\b" );
				}
				Znk_printf_e( "%04zu", waiting_count );
				++waiting_count;
				is_again = true;
				return is_again;
			}
		}
	}
	if( waiting_count ){
		Znk_printf_e( "\n" );
	}
	return is_again;
}

int
ZnkSocket_send( ZnkSocket sock, const uint8_t* data, size_t data_size )
{
	char msg_from[ 256 ];
	Znk_snprintf( msg_from, sizeof(msg_from), "ZnkSocket_send sock=[%d]", sock );
	return ZnkSocket_send_ex( sock, data, data_size, report, (void*)msg_from );
}

int
ZnkSocket_recv( ZnkSocket sock, uint8_t* buf, size_t buf_size )
{
	char msg_from[ 256 ];
	Znk_snprintf( msg_from, sizeof(msg_from), "ZnkSocket_recv sock=[%d]", sock );
	return ZnkSocket_recv_ex( sock, buf, buf_size, report, (void*)msg_from );
}

int
ZnkSocket_send_ex( ZnkSocket sock, const uint8_t* data, size_t data_size,
		ZnkSocketCallback cb_func, void* cb_func_arg )
{
	int      rc;
	uint32_t sys_errno = 0;
	size_t   waiting_count = 0;
AGAIN:
#if defined(Znk_TARGET_WINDOWS)
	rc = send( sock, (const char*)data, (int)data_size, 0 );
	if( rc == SOCKET_ERROR ){
		int winsock_err = WSAGetLastError();
		set_errno_forWin( winsock_err );
	}
#else
	rc = send( sock, (const char*)data, data_size, 0 );
#endif

	if( cb_func ){
		sys_errno = ZnkSysErrno_errno();
		if( (*cb_func)( rc, sys_errno, waiting_count, cb_func_arg ) ){
			++waiting_count;
			goto AGAIN;
		}
	}
	return rc;
}

int
ZnkSocket_recv_ex( ZnkSocket sock, uint8_t* buf, size_t buf_size,
		ZnkSocketCallback cb_func, void* cb_func_arg )
{
	int      rc;
	uint32_t sys_errno = 0;
	size_t   waiting_count = 0;
AGAIN:
#if defined(Znk_TARGET_WINDOWS)
	rc = recv( sock, (char*)buf, (int)buf_size, 0 );
	if( rc == SOCKET_ERROR ){
		int winsock_err = WSAGetLastError();
		set_errno_forWin( winsock_err );
	}
#else
	rc = recv( sock, (char*)buf, buf_size, 0 );
#endif

	if( cb_func ){
		sys_errno = ZnkSysErrno_errno();
		if( (*cb_func)( rc, sys_errno, waiting_count, cb_func_arg ) ){
			++waiting_count;
			goto AGAIN;
		}
	}
	return rc;
}

bool
ZnkSocket_getPeerIPandPort( ZnkSocket sock, uint32_t* ipaddr, uint16_t* port )
{
	struct sockaddr_in addr;   // ソケットアドレス
#if defined(Znk_TARGET_WINDOWS)
	int addr_leng = sizeof(struct sockaddr);
#else
	socklen_t addr_leng = sizeof(struct sockaddr);
#endif
	int  result;
	
	// アドレス情報取得
	result = getpeername( sock, (struct sockaddr*)&addr, &addr_leng );
	if( result < 0 ){
		Znk_printf_e( "ZnkSocket_getPeerIPandPort : getpeername error.\n" );
		return false;
	}
	
	if( ipaddr ){
#if defined(Znk_TARGET_WINDOWS)
		*ipaddr = (uint32_t)addr.sin_addr.S_un.S_addr;
#else
		*ipaddr = (uint32_t)addr.sin_addr.s_addr;
#endif
	}
	if( port ){
		*port = ntohs( addr.sin_port);
	}
	return true;
}
