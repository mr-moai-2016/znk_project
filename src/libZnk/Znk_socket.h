#ifndef INCLUDE_GUARD__Znk_socket_h__
#define INCLUDE_GUARD__Znk_socket_h__

#include <Znk_base.h>
#include <Znk_bfr.h>
#include <Znk_str.h>
#include <Znk_err.h>
#include <Znk_stdc.h>

Znk_EXTERN_C_BEGIN

typedef uintptr_t ZnkSocket;
typedef bool (*ZnkSocketCallback)( int rc, uint32_t sys_errno, size_t waiting_count, void* arg );

#define ZnkSocket_INVALID ((ZnkSocket)(-1))

ZnkSocket
ZnkSocket_open( void );
void
ZnkSocket_close( ZnkSocket sock );
ZnkSocket
ZnkSocket_accept( ZnkSocket listen_sock );

bool
ZnkSocket_setBlockingMode( ZnkSocket sock, bool is_blocking_mode );

bool
ZnkSocket_connectToServer( ZnkSocket sock, const char* hostname, uint16_t port, ZnkErr* zkerr, bool* is_inprogress );
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
ZnkSocket_send_ex( ZnkSocket sock, const uint8_t* data, size_t data_size,
		ZnkSocketCallback cb_func, void* cb_func_arg );
int
ZnkSocket_recv_ex( ZnkSocket sock, uint8_t* buf, size_t buf_size,
		ZnkSocketCallback cb_func, void* cb_func_arg );

bool
ZnkSocket_getPeerIPandPort( ZnkSocket sock, uint32_t* ipaddr, uint16_t* port );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
