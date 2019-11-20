#ifndef INCLUDE_GUARD__Moai_io_base_h__
#define INCLUDE_GUARD__Moai_io_base_h__

#include <Znk_socket.h>
#include <Znk_fdset.h>
#include <Znk_bfr.h>
#include <Moai_fdset.h>

Znk_EXTERN_C_BEGIN

typedef void (*MoaiIOFilterFunc)( ZnkBfr bfr, const char* content_type );

int
MoaiIO_sendTxtf( ZnkSocket sock, const char* content_type, const char* fmt, ... );
bool
MoaiIO_sendResponseFile( ZnkSocket sock, const char* filename, MoaiIOFilterFunc io_filter_func );
const char*
MoaiIO_makeSockStr( char* buf, size_t buf_size, ZnkSocket sock, bool detail );
void
MoaiIO_addAnalyzeLabel( ZnkStr msgs, ZnkSocket sock, int result_size, const char* label );

void
MoaiIO_reportFdst( const char* label, ZnkSocketAry sock_ary, ZnkFdSet fdst_observe, bool detail );
bool
MoaiIO_procExile( ZnkSocketAry sock_ary, ZnkFdSet fdst, MoaiFdSet mfds );

void
MoaiIO_close_ISock( const char* label, ZnkSocket sock, MoaiFdSet mfds );
void
MoaiIO_close_OSock( const char* label, ZnkSocket sock, MoaiFdSet mfds );

void
MoaiIO_closeSocketAll( const char* label, ZnkFdSet fdst_observe, MoaiFdSet mfds );

bool
MoaiIO_recvInPtn( ZnkBfr stream, ZnkSocket sock, MoaiFdSet mfds, const char* ptn, size_t* result_size );
bool
MoaiIO_recvByPtn( ZnkBfr stream, ZnkSocket sock, MoaiFdSet mfds, const char* ptn, size_t* result_size );
bool
MoaiIO_recvBySize( ZnkBfr stream, ZnkSocket sock, MoaiFdSet mfds, size_t size, size_t* result_size );
bool
MoaiIO_recvByZero( ZnkBfr stream, ZnkSocket sock, MoaiFdSet mfds, size_t* result_size );

size_t
MoaiIO_recvByPtn2( ZnkBfr stream, ZnkSocket sock, MoaiFdSet mfds, const char* ptn );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
