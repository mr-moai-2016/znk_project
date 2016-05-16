#ifndef INCLUDE_GUARD__Moai_plugin_dev_h__
#define INCLUDE_GUARD__Moai_plugin_dev_h__

#include <Znk_myf.h>

Znk_EXTERN_C_BEGIN

bool initiate( ZnkMyf ftr_send, const char* parent_proxy,
		char* result_msg_buf, size_t result_msg_buf_size );
bool on_post_before( ZnkMyf ftr_send );
bool on_response_hdr( ZnkMyf ftr_send, ZnkVarpAry hdr_vars );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
