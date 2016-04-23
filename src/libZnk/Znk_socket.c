#include "Znk_socket.h"
#include "Znk_net_base.h"
#include "Znk_sys_errno.h"
#include "Znk_stdc.h"

#if defined(Znk_TARGET_WINDOWS)
#  include <winsock2.h>

#elif defined(Znk_TARGET_UNIX)
#  include <sys/socket.h> /* for socket,connect,bind,listen,accept,shutdown,recv,send etc. */
#  include <unistd.h>     /* for close */
#  include <arpa/inet.h>  /* for inet_* function (inet_addr etc) */
#  include <netdb.h>      /* for gethostbyname, getservbyname */
#  include <netinet/in.h> /* for struct sockaddr_in, htons etc. */

#else
#endif



ZnkSocket
ZnkSocket_open( void )
{
	/* Win32/X11 で記述方法は全く同じ */
	return socket(AF_INET, SOCK_STREAM, 0);
}

void ZnkSocket_close( ZnkSocket sock )
{
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

void
ZnkSocket_set_blocking_mode( ZnkSocket sock, bool is_blocking_mode )
{
	/***
	 * ソケットのデフォルトはブロッキングモードである.
	 * val = 1 で下記を呼び出すことにより非ブロッキングモードに変更できる.
	 * val = 0 でブロッキングモードに設定しなおすことも可能.
	 *
	 * これは試験的実装である.
	 * WinSockならばこれでよいがLinuxではまだ未確認でこれではうまくいかないかもしれない.
	 */
#if defined(Znk_TARGET_WINDOWS)
	unsigned long val = is_blocking_mode ? 0 : 1;
	ioctlsocket( sock, FIONBIO, &val );
#else
	/* 調査中 */
#endif
}


/***
 * serverには sin_familyとsin_portは設定済みであるとする.
 */
Znk_INLINE bool
connectCore( ZnkSocket sock, struct sockaddr_in* server, const char* hostname, ZnkErr* zkerr )
{
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
			if( connect(sock, (struct sockaddr *)server, sizeof(struct sockaddr_in)) == 0 ){
				/* 成功. これを採用 */
				break;
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
		if( connect(sock, (struct sockaddr *)server, sizeof(struct sockaddr_in)) != 0 ){
			char errmsg_buf[ 4096 ];
			ZnkNetBase_getErrMsg( errmsg_buf, sizeof(errmsg_buf), WSAGetLastError() );
			ZnkErr_internf( zkerr, "ZnkSocket_connectToServer : Failure : connect IP=[%08x] WSAErr=[%s]",
					server->sin_addr.S_un.S_addr, errmsg_buf );
			return false;
		}
	}
	return true;

#elif defined(Znk_TARGET_UNIX)

	/* ホスト名とIP アドレスを扱うための構造体のポインタ(gethostbynameの戻り値) */
	struct hostent* servhost = gethostbyname( hostname );
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
ZnkSocket_connectToServer( ZnkSocket sock, const char* hostname, uint16_t port, ZnkErr* zkerr )
{
	struct sockaddr_in server;   /* ソケットを扱うための構造体 */

	/* ゼロクリア */
	Znk_memset( &server, 0, sizeof(server) );

	/* sin_family */
	server.sin_family = AF_INET;

	/* port */
	server.sin_port = htons(port);

	return connectCore( sock, &server, hostname, zkerr );
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

int
ZnkSocket_send( ZnkSocket sock, const uint8_t* data, size_t data_size )
{
#if defined(Znk_TARGET_WINDOWS)
	return send( sock, (const char*)data, (int)data_size, 0 );
#else
	return send( sock, (const char*)data, data_size, 0 );
#endif
}
int
ZnkSocket_recv( ZnkSocket sock, uint8_t* buf, size_t buf_size )
{
#if defined(Znk_TARGET_WINDOWS)
	return recv( sock, (char*)buf, (int)buf_size, 0 );
#else
	return recv( sock, (char*)buf, buf_size, 0 );
#endif
}
int
ZnkSocket_forward( ZnkSocket src_sock, ZnkSocket dst_sock, uint8_t* buf, size_t buf_size )
{
	int recv_size = ZnkSocket_recv( src_sock, buf, buf_size );
	if( recv_size > 0 ){
		int forward_size = 0;
		int send_size = 0;
		while( forward_size < recv_size ){
			send_size = ZnkSocket_send( dst_sock, buf + forward_size, recv_size - forward_size );
			if( send_size > 0 ){
				forward_size += send_size;
			} else if( send_size < 0 ){
				return send_size; /* send error */
			} else {
				/* 最大試行回数を設定すべきか? */
			}
		}
		return forward_size;
	}
	return recv_size; /* recv error */
}

