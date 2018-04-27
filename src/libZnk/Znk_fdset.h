#ifndef Znk_fdset_h__INCLUDED__
#define Znk_fdset_h__INCLUDED__

#include <Znk_socket.h>
#include <Znk_socket_ary.h>

Znk_EXTERN_C_BEGIN

struct ZnkTimeval {
	long tv_sec;
	long tv_usec;
};

typedef struct ZnkFdSetImpl* ZnkFdSet;

ZnkFdSet
ZnkFdSet_create( void );
void
ZnkFdSet_destroy( ZnkFdSet fdst );
/**
 * @brief
 *  sockをZnkFdSetにsetする.
 *  最大保持数のFD_SETSIZE個を超えた場合など、setに失敗する場合もあるので、
 *  特に途中新規に生成した acceptによる sock や connectによる sockを追加する場合で
 *  かつそれが大量に発生し得る用途では、必ずこの戻り値を確認し、適切な処理を講じること.
 */
bool
ZnkFdSet_set( ZnkFdSet fdst, const ZnkSocket sock );
void
ZnkFdSet_zero( ZnkFdSet fdst );
void
ZnkFdSet_clr( ZnkFdSet fdst, const ZnkSocket sock );
bool
ZnkFdSet_isset( const ZnkFdSet fdst, const ZnkSocket sock );
void
ZnkFdSet_print_e( const ZnkFdSet fdst );
void
ZnkFdSet_copy( ZnkFdSet dst, const ZnkFdSet src );
void
ZnkFdSet_getSocketAry( ZnkFdSet fdst, ZnkSocketAry sock_ary );
ZnkSocket
ZnkFdSet_getMaxOfSocket( const ZnkFdSet fdst );
size_t
ZnkFdSet_FD_SETSIZE( void );

int
ZnkFdSet_select( ZnkSocket sock,
		ZnkFdSet fdst_read, ZnkFdSet fdst_write, ZnkFdSet fdst_except,
		const struct ZnkTimeval* timeout );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
