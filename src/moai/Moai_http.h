#ifndef INCLUDE_GUARD__Moai_http_h__
#define INCLUDE_GUARD__Moai_http_h__

#include <Moai_context.h>
#include <Moai_fdset.h>
#include <Rano_module.h>

Znk_EXTERN_C_BEGIN

bool
MoaiHttp_scanHeaderVar( const char* var_name, const uint8_t* buf,
		size_t result_size, char* val_buf, size_t val_buf_size );

void
MoaiHttp_processResponse_forText( ZnkSocket O_sock, MoaiContext ctx, MoaiFdSet mfds,
		size_t* content_length_remain, RanoModule mod );

void
MoaiHttp_updateCookieFilter_bySetCookie( const ZnkVarpAry hdr_vars, ZnkMyf ftr_send );

void
MoaiHttp_debugHdrVars( const ZnkVarpAry vars, const char* prefix_tab );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
