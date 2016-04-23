#ifndef INCLUDE_GUARD__Znk_socket_h__
#define INCLUDE_GUARD__Znk_socket_h__

#include <Znk_base.h>
#include <Znk_bfr.h>
#include <Znk_str.h>
#include <Znk_err.h>
#include <Znk_stdc.h>

Znk_EXTERN_C_BEGIN

typedef uintptr_t ZnkSocket;

#define ZnkSocket_INVALID ((ZnkSocket)(-1))

ZnkSocket
ZnkSocket_open( void );
void
ZnkSocket_close( ZnkSocket sock );
ZnkSocket
ZnkSocket_accept( ZnkSocket listen_sock );

void
ZnkSocket_set_blocking_mode( ZnkSocket sock, bool is_blocking_mode );

bool
ZnkSocket_connectToServer( ZnkSocket sock, const char* hostname, uint16_t port, ZnkErr* zkerr );
uint16_t
ZnkSocket_getPort_byTCPName( const char* service_name );

int
ZnkSocket_send( ZnkSocket sock, const uint8_t* data, size_t data_size );
Znk_INLINE int
ZnkSocket_send_cstr( ZnkSocket sock, const char* cstr ){
	return ZnkSocket_send( sock, (const uint8_t*)cstr, Znk_strlen(cstr) );
}
Znk_INLINE int
ZnkSocket_send_ZnkStr( ZnkSocket sock, const ZnkStr zkstr ){
	return ZnkSocket_send( sock, ZnkBfr_data(zkstr), ZnkStr_leng(zkstr) );
}
int
ZnkSocket_recv( ZnkSocket sock, uint8_t* buf, size_t buf_size );

int
ZnkSocket_forward( ZnkSocket src_sock, ZnkSocket dst_sock, uint8_t* buf, size_t buf_size );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
