#ifndef INCLUDE_GUARD__Moai_context_h__
#define INCLUDE_GUARD__Moai_context_h__

#include "Moai_info.h"
#include "Moai_module_ary.h"
#include <Znk_str.h>
#include <Znk_htp_hdrs.h>

Znk_EXTERN_C_BEGIN

typedef struct MoaiContextImpl* MoaiContext;

struct MoaiContextImpl {
	const char*     hostname_;
	uint16_t        port_;
	char            hostname_buf_[ 4096 ];
	uint8_t         buf_[ 4096 ];
	ZnkStr          msgs_;
	ZnkStr          text_;
	int             result_size_;
	bool            recv_executed_;
	ZnkStr          req_urp_;
	ZnkHtpReqMethod req_method_;
	MoaiBodyInfo    body_info_;
	MoaiInfo        hi_;
	ZnkMyf          target_myf_;
	MoaiModuleAry   mod_ary_;
	ZnkMyf          config_;
	ZnkMyf          analysis_;
};

MoaiContext
MoaiContext_create( void );
void
MoaiContext_destroy( MoaiContext ctx );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
