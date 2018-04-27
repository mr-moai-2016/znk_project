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
#if defined(__CYGWIN__)
	if( sock != ZnkSocket_INVALID ){ FD_SET( s, &fdst->fds_ ); }
#else
	FD_SET( s, &fdst->fds_ );
#endif
	/***
	 * fdsは最大FD_SETSIZE個のsockしか保持できない.
	 * FD_SETではこれを超えた場合は何もせず何のエラーも出さないため、
	 * そのような状況を自前で捕捉する必要がある.
	 * これに関して、移植性の問題が出る可能性があるような処理系依存のコードを書くよりも
	 * issetで結果を確認した方が確実である.
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
#if defined(__CYGWIN__)
	if( sock != ZnkSocket_INVALID ){ FD_CLR( s, &fdst->fds_ ); }
#else
	FD_CLR( s, &fdst->fds_ );
#endif
}
bool
ZnkFdSet_isset( const ZnkFdSet fdst, const ZnkSocket sock )
{
	Fd_int_t s = (Fd_int_t)sock;
#if defined(__CYGWIN__)
	if( sock != ZnkSocket_INVALID ){
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
	return ( max == -1 ) ? ZnkSocket_INVALID : (ZnkSocket)max;
#else
	int s = FD_SETSIZE-1;
	while( s >= 0 ){
		if( FD_ISSET( s, &fdst->fds_ ) ){
			return s;
		}
		--s;
	}
	return ZnkSocket_INVALID;
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
	 * ここでtimeoutを直接selectへ渡さず、一旦timeout_internalへ引き渡しているのは
	 * 異コンパイラ間でのダイナミックロードでの移植性のためでもあるが、その他にも意味がある.
	 * Linux では、 select() は timeout を変更し、残りの停止時間を反映するようになっている.
	 * (ちなみに他のほとんどの実装ではこのようになっていない. POSIX.1-2001 はどちらの動作も認めている)
	 * そのため、select() から復帰した後は timeout は未定義であると考えるべきである. 
	 * この変更の影響を避けるためにも、ここでは一旦timeout_internalに代入を行っている.
	 */
	if( timeout ){
		timeout_internal.tv_sec  = timeout->tv_sec;
		timeout_internal.tv_usec = timeout->tv_usec;
		timeout_ref = &timeout_internal;
	}
	return select( (int)sock, readfds, writefds, exceptfds, timeout_ref );
}

