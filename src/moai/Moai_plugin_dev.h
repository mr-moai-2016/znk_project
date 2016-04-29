#ifndef INCLUDE_GUARD__Moai_plugin_dev_h__
#define INCLUDE_GUARD__Moai_plugin_dev_h__

#include <Znk_myf.h>
#include <Znk_htp_hdrs.h>

Znk_EXTERN_C_BEGIN

bool on_init( ZnkMyf myf, const char* proxy_hostname, const char* proxy_port );
bool on_post_before( ZnkMyf myf );
bool on_response_hdr( ZnkMyf myf, ZnkVarpDAry hdr_vars );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
