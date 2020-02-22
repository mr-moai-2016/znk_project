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
ZnkSocket_getInvalid( void )
{
#if defined(Znk_TARGET_WINDOWS)
	return (ZnkSocket)INVALID_SOCKET;
#else
	return (ZnkSocket)(-1);
#endif
}

ZnkSocket
ZnkSocket_open( void )
{
#if defined(Znk_TARGET_WINDOWS)
	return socket(AF_INET, SOCK_STREAM, 0);
#else
	//return socket(AF_INET, SOCK_STREAM, 0);
	const int sock = socket(AF_INET, SOCK_STREAM, 0);
	return sock < 0 ? ZnkSocket_getInvalid() : (ZnkSocket)sock;
#endif
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
	 * �ȉ���addr_leng�l�̏������͕K�{�ł���.
	 * accept�J�n���A�܂��͓��͂Ƃ��Ă��̒l���Q�Ƃ���.
	 * �܂�accept�I�����A�o�͂Ƃ��Ď��ۂɎ擾���ꂽaddr�̃o�b�t�@�T�C�Y�������ɏ㏑������.
	 * ���̏o�͒l�́A�ꍇ�ɂ���Ă�sizeof(struct sockaddr)�����傫�����Ȃ蓾��.
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
	 * �\�P�b�g�̃f�t�H���g�̓u���b�L���O���[�h�ł���.
	 * val = 1 �ŉ��L���Ăяo�����Ƃɂ���u���b�L���O���[�h�ɕύX�ł���.
	 * val = 0 �Ńu���b�L���O���[�h�ɐݒ肵�Ȃ������Ƃ��\.
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
 * server�ɂ� sin_family��sin_port�͐ݒ�ς݂ł���Ƃ���.
 */
Znk_INLINE bool
connectCore( ZnkSocket sock,
		struct sockaddr_in* server, const char* hostname, ZnkErr* zkerr, bool* is_inprogress )
{
	int rc;
	*is_inprogress = false;
#if defined(Znk_TARGET_WINDOWS)

	/***
	 * hostname��IP(v4)�̏ꍇ�Ainet_addr�͒ʏ퐬������͂��ł���.
	 * (�P��8byte�����S�̑g�ݍ��킹����Ȃ镶�����32bit�l�֕ϊ����Ă���ɂ����Ȃ�)
	 * �������Ahostname��������IP�ȊO��URL�ł���ꍇ�A����͒ʏ편�s����.
	 * ���̂Ƃ��A���̖߂�l�� 0xffffffff �ƂȂ�.
	 * ���̏ꍇ�́Agethostbyname �ɂ��A���O���������݂�.
	 */
	server->sin_addr.S_un.S_addr = inet_addr( hostname );
	if( server->sin_addr.S_un.S_addr == 0xffffffff ){
		unsigned int** addrptr;
		/* inet_addr()�����s�����Ƃ�(hostname�ɂ�URL���w�肳��Ă������̂Ǝv����) */
		struct hostent* hostent_ptr = gethostbyname(hostname);
		if( hostent_ptr == NULL ){
			char errmsg_buf[ 4096 ];
			ZnkNetBase_getErrMsg( errmsg_buf, sizeof(errmsg_buf), WSAGetLastError() );
			ZnkErr_internf( zkerr,
					"ZnkSocket_connectToServer : Failure : gethostbyname(DNS) hostname=[%s] WSAErr=[%s]",
					hostname, errmsg_buf );
			return false;
		}

		/* �����ł͂�茵���� h_addr_list �ɂ��邷�ׂĂ̌��ʂ���������A
		 * �ŏ��ɐ����������̂��̗p����.
		 */
		addrptr = (unsigned int **)hostent_ptr->h_addr_list;
		while( *addrptr != NULL ){
			server->sin_addr.S_un.S_addr = *(*addrptr);
			/* connect()������������loop�𔲂��� */
			rc = connect(sock, (struct sockaddr *)server, sizeof(struct sockaddr_in));
			if( rc == 0 ){
				/* ����. ������̗p */
				break;
			}
			/***
			 * ��u���b�L���OIO�̏ꍇ.
			 * ���̏ꍇ��WSAEWOULDBLOCK�ƂȂ��Ă�����is_inprogress�Ƃ݂Ȃ�.
			 * �߂�l��true�Ƃ���.
			 */
			if( WSAGetLastError() == WSAEWOULDBLOCK ){
				*is_inprogress = true;
				return true;
			}
			++addrptr;
		}
		/* connect���S�Ď��s�����ꍇ */
		if( *addrptr == NULL ){
			/* �Ƃ肠�����G���[���b�Z�[�W�ł͈�ԍŌ��IP�ŕ񍐂��� */
			char errmsg_buf[ 4096 ];
			ZnkNetBase_getErrMsg( errmsg_buf, sizeof(errmsg_buf), WSAGetLastError() );
			ZnkErr_internf( zkerr, "ZnkSocket_connectToServer : Failure : connect IP=[%08x] WSAErr=[%s]",
					server->sin_addr.S_un.S_addr, errmsg_buf );
			return false;
		}
		
	} else {
		/* inet_addr()�����������Ƃ�(hostname�ɂ�IP�A�h���X�����Ɏw�肳��Ă������̂Ǝv����) */
		rc = connect( sock, (struct sockaddr *)server, sizeof(struct sockaddr_in) );
		if( rc != 0 ){
			/***
			 * ��u���b�L���OIO�̏ꍇ.
			 * ���̏ꍇ��WSAEWOULDBLOCK�ƂȂ��Ă�����is_inprogress�Ƃ݂Ȃ�.
			 * �߂�l��true�Ƃ���.
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

	/* �z�X�g����IP �A�h���X���������߂̍\���̂̃|�C���^(gethostbyname�̖߂�l) */
	struct hostent* servhost = gethostbyname( hostname );
	Znk_UNUSED( rc );
	if( servhost == NULL ){
		ZnkSysErrnoInfo* err_info = ZnkSysErrno_getInfo( ZnkSysErrno_errno() );
		ZnkErr_internf( zkerr,
				"ZnkSocket_connectToServer : Failure : gethostbyname hostname=[%s] SysErr=[%s:%s]",
				hostname, err_info->sys_errno_key_, err_info->sys_errno_msg_ );
		return false;
	}

	/* sin_addr(IP�A�h���X) */
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
	struct sockaddr_in server;   /* �\�P�b�g���������߂̍\���� */

	/* �[���N���A */
	Znk_memset( &server, 0, sizeof(server) );

	/* sin_family */
	server.sin_family = AF_INET;

	/* port */
	server.sin_port = htons(port);

	return connectCore( sock, &server, hostname, zkerr, is_inprogress );
}

/***
 * �T�[�r�X�����port�ԍ��𓾂�.
 * ���������s�����ꍇ��0��Ԃ�.
 */
uint16_t
ZnkSocket_getPort_byTCPName( const char* service_name ) 
{
	/* �T�[�r�X (http �Ȃ�) ���������߂̍\���� */
	struct servent* service = getservbyname( service_name, "tcp" );
	if( service == NULL ){
		/* ���s */
		return 0;
	}
	/* ����.
	 * �����炭network byteorder �Ŏ擾�����悤�Ȃ̂ŁA
	 * Net=>Host(short) �ŕϊ��������Ă���. 
	 */
	return ntohs( service->s_port );
}

#if defined(Znk_TARGET_WINDOWS)
/***
 * WSAGetLastError�̒l�����errno�́A�}���`�X���b�h�Ń��C�u�����ł�
 * �X���b�h���Ɏ��̂�����(�܂�thread-safe�ł���).
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
 * �ȉ��̂悤�ɂ��Ă������������������ǂ����ŏ��̐����wait�Ԋu��
 * ���܂�ɒZ�������EAGAIN���A������s�����������悤�ł���.
 * �Ԋu���኱���߂ɂƂ�Ƃ��ꂪ�N����ɂ����Ȃ�.
 * �܂�waiting_count�͖����ɋ��e�����A�g�[�^����15�b���x�ɂȂ�񐔂ɂȂ�����
 * ���߂� rc < 0 ���u���E�U�ɕԂ��������ǂ͑S�̓I�ɑ��x���啝�ɑ����悤�ł���.
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
			 * ���񂩎��s���ă_���Ȃ������.
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
	struct sockaddr_in addr;   // �\�P�b�g�A�h���X
#if defined(Znk_TARGET_WINDOWS)
	int addr_leng = sizeof(struct sockaddr);
#else
	socklen_t addr_leng = sizeof(struct sockaddr);
#endif
	int  result;
	
	// �A�h���X���擾
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
