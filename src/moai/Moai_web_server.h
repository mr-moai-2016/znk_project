#ifndef INCLUDE_GUARD__Moai_web_server_h__
#define INCLUDE_GUARD__Moai_web_server_h__

#include <Moai_type.h>
#include <Moai_context.h>
#include <Moai_connection.h>
#include <Moai_fdset.h>

#include <Znk_socket.h>

Znk_EXTERN_C_BEGIN

MoaiRASResult
MoaiWebServer_do( const MoaiContext ctx, ZnkSocket sock, MoaiConnection mcn, MoaiFdSet mfds );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
