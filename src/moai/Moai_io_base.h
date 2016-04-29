#ifndef INCLUDE_GUARD__Moai_io_base_h__
#define INCLUDE_GUARD__Moai_io_base_h__

#include <Znk_socket.h>
#include <Znk_fdset.h>
#include <Znk_bfr.h>

Znk_EXTERN_C_BEGIN

int
MoaiIO_sendTxtf( ZnkSocket sock, const char* content_type, const char* fmt, ... );
bool
MoaiIO_sendResponseFile( ZnkSocket sock, const char* filename );
const char*
MoaiIO_makeSockStr( char* buf, size_t buf_size, ZnkSocket sock, bool detail );
void
MoaiIO_addAnalyzeLabel( ZnkStr msgs, ZnkSocket sock, int result_size );

void
MoaiIO_reportFdst( const char* label, ZnkSocketDAry sock_dary, ZnkFdSet fdst, bool detail );
bool
MoaiIO_procExile( ZnkSocketDAry sock_dary, ZnkFdSet fdst );

void
MoaiIO_closeSocket( const char* label, ZnkSocket sock, ZnkFdSet fdst_observe );
void
MoaiIO_closeSocketAll( const char* label, ZnkFdSet fdst_observe );

bool
MoaiIO_recvInPtn( ZnkBfr stream, ZnkSocket sock, ZnkFdSet fdst_observe, const char* ptn, size_t* result_size );
bool
MoaiIO_recvByPtn( ZnkBfr stream, ZnkSocket sock, ZnkFdSet fdst_observe, const char* ptn, size_t* result_size );
bool
MoaiIO_recvBySize( ZnkBfr stream, ZnkSocket sock, ZnkFdSet fdst_observe, size_t size, size_t* result_size );
bool
MoaiIO_recvByZero( ZnkBfr stream, ZnkSocket sock, ZnkFdSet fdst_observe, size_t* result_size );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
