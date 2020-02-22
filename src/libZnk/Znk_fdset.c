#include "Znk_fdset.h"
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

#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(Znk_TARGET_WINDOWS)
typedef SOCKET Fd_int_t;
#else
typedef int Fd_int_t;
#endif

struct ZnkFdSetImpl {
	fd_set fds_;
};

ZnkFdSet
ZnkFdSet_create( void )
{
	ZnkFdSet fdst = (ZnkFdSet)Znk_malloc( sizeof(struct ZnkFdSetImpl) );
	FD_ZERO( &fdst->fds_ );
	return fdst;
}
void
ZnkFdSet_destroy( ZnkFdSet fdst )
{
	if( fdst ){
		Znk_free( fdst );
	}
}
bool
ZnkFdSet_set( ZnkFdSet fdst, const ZnkSocket sock )
{
	Fd_int_t s = (Fd_int_t)sock;
#if defined(__CYGWIN__) || defined(__ANDROID__)
	if( !ZnkSocket_isInvalid(sock) ){ FD_SET( s, &fdst->fds_ ); }
#else
	FD_SET( s, &fdst->fds_ );
#endif
	/***
	 * fds�͍ő�FD_SETSIZE��sock�����ێ��ł��Ȃ�.
	 * FD_SET�ł͂���𒴂����ꍇ�͉����������̃G���[���o���Ȃ����߁A
	 * ���̂悤�ȏ󋵂����O�ŕߑ�����K�v������.
	 * ����Ɋւ��āA�ڐA���̖�肪�o��\��������悤�ȏ����n�ˑ��̃R�[�h����������
	 * isset�Ō��ʂ��m�F���������m���ł���.
	 */
	return ZnkFdSet_isset( fdst, sock );
}
void
ZnkFdSet_zero( ZnkFdSet fdst )
{
	FD_ZERO( &fdst->fds_ );
}
void
ZnkFdSet_clr( ZnkFdSet fdst, const ZnkSocket sock )
{
	Fd_int_t s = (Fd_int_t)sock;
#if defined(__CYGWIN__) || defined(__ANDROID__)
	if( !ZnkSocket_isInvalid(sock) ){ FD_CLR( s, &fdst->fds_ ); }
#else
	FD_CLR( s, &fdst->fds_ );
#endif
}
bool
ZnkFdSet_isset( const ZnkFdSet fdst, const ZnkSocket sock )
{
	Fd_int_t s = (Fd_int_t)sock;
#if defined(__CYGWIN__) || defined(__ANDROID__)
	if( !ZnkSocket_isInvalid(sock) ){
		return (bool)( FD_ISSET( s, &fdst->fds_ ) );
	}
	return false;
#else
	return (bool)( FD_ISSET( s, &fdst->fds_ ) );
#endif
}
void
ZnkFdSet_print_e( const ZnkFdSet fdst )
{
#if defined(Znk_TARGET_WINDOWS)
	size_t idx;
	for( idx=0; idx<fdst->fds_.fd_count; ++idx ){
		Znk_printf_e( "fds.fd_array[%zu]=[%d]\n", idx, fdst->fds_.fd_array[ idx ] );
	}
#else
	int s;
	Znk_printf_e( "fds={\n" );
	for( s=0; s<FD_SETSIZE; ++s ){
		if( FD_ISSET( s, &fdst->fds_ ) ){
			Znk_printf_e( "%d, ", s );
		}
	}
	Znk_printf_e( "}\n" );
#endif
}
void
ZnkFdSet_copy( ZnkFdSet dst, const ZnkFdSet src )
{
	if( dst != src ){
		*dst = *src;
	}
}
void
ZnkFdSet_getSocketAry( ZnkFdSet fdst, ZnkSocketAry sock_ary )
{
#if defined(Znk_TARGET_WINDOWS)
	size_t idx;
	for( idx=0; idx<fdst->fds_.fd_count; ++idx ){
		ZnkSocketAry_push_bk( sock_ary, fdst->fds_.fd_array[ idx ] );
	}
#else
	int s;
	for( s=0; s<FD_SETSIZE; ++s ){
		if( FD_ISSET( s, &fdst->fds_ ) ){
			ZnkSocketAry_push_bk( sock_ary, (ZnkSocket)s );
		}
	}
#endif
}
ZnkSocket
ZnkFdSet_getMaxOfSocket( const ZnkFdSet fdst )
{
#if defined(Znk_TARGET_WINDOWS)
	long max = -1;
	size_t idx;
	for( idx=0; idx<fdst->fds_.fd_count; ++idx ){
		if( (long)fdst->fds_.fd_array[ idx ] > max ){
			max = (long)fdst->fds_.fd_array[ idx ];
		}
	}
	return ( max == -1 ) ? ZnkSocket_getInvalid() : (ZnkSocket)max;
#else
	int s = FD_SETSIZE-1;
	while( s >= 0 ){
		if( FD_ISSET( s, &fdst->fds_ ) ){
			return s;
		}
		--s;
	}
	return ZnkSocket_getInvalid();
#endif
}
size_t
ZnkFdSet_FD_SETSIZE( void )
{
	return (size_t)FD_SETSIZE;
}

int
ZnkFdSet_select( ZnkSocket sock,
		ZnkFdSet fdst_read, ZnkFdSet fdst_write, ZnkFdSet fdst_except,
		const struct ZnkTimeval* timeout )
{
	struct timeval  timeout_internal = { 0 };
	struct timeval* timeout_ref = NULL;
	fd_set* readfds   = fdst_read   ? &fdst_read->fds_   : NULL;
	fd_set* writefds  = fdst_write  ? &fdst_write->fds_  : NULL;
	fd_set* exceptfds = fdst_except ? &fdst_except->fds_ : NULL;

	/***
	 * ������timeout�𒼐�select�֓n�����A��Utimeout_internal�ֈ����n���Ă���̂�
	 * �كR���p�C���Ԃł̃_�C�i�~�b�N���[�h�ł̈ڐA���̂��߂ł����邪�A���̑��ɂ��Ӗ�������.
	 * Linux �ł́A select() �� timeout ��ύX���A�c��̒�~���Ԃ𔽉f����悤�ɂȂ��Ă���.
	 * (���Ȃ݂ɑ��̂قƂ�ǂ̎����ł͂��̂悤�ɂȂ��Ă��Ȃ�. POSIX.1-2001 �͂ǂ���̓�����F�߂Ă���)
	 * ���̂��߁Aselect() ���畜�A������� timeout �͖���`�ł���ƍl����ׂ��ł���. 
	 * ���̕ύX�̉e��������邽�߂ɂ��A�����ł͈�Utimeout_internal�ɑ�����s���Ă���.
	 */
	if( timeout ){
		timeout_internal.tv_sec  = timeout->tv_sec;
		timeout_internal.tv_usec = timeout->tv_usec;
		timeout_ref = &timeout_internal;
	}
	return select( (int)sock, readfds, writefds, exceptfds, timeout_ref );
}

