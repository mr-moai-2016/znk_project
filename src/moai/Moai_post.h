#ifndef INCLUDE_GUARD__Moai_post_h__
#define INCLUDE_GUARD__Moai_post_h__

#include <Znk_socket.h>
#include <Znk_fdset.h>
#include "Moai_info.h"
#include "Moai_module_ary.h"

Znk_EXTERN_C_BEGIN

void
MoaiPost_initiate( void );

void
MoaiPost_proc( ZnkSocket sock, ZnkFdSet fdst_observe,
		const char* dst_hostname, MoaiInfo* info, MoaiBodyInfo* body_info,
		bool as_local_proxy, bool master_confirm,
		MoaiModuleAry mod_ary, ZnkMyf mtgt, ZnkMyf analysis, const char* req_urp );

MoaiInfo*
MoaiPost_parsePostVars( ZnkSocket sock, ZnkFdSet fdst_observe,
		ZnkStr str, const char* dst_hostname, MoaiInfo* info, MoaiBodyInfo* body_info,
		MoaiModuleAry mod_ary, ZnkMyf mtgt );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
