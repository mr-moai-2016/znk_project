#ifndef Znk_server_h__INCLUDED__
#define Znk_server_h__INCLUDED__

#include <Znk_socket.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkServerImpl* ZnkServer;

ZnkServer
ZnkServer_create( const char* acceptable_host, int port );

void
ZnkServer_destroy( ZnkServer server );

ZnkSocket
ZnkServer_getListenSocket( const ZnkServer server );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
