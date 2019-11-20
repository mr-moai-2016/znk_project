#ifndef INCLUDE_GUARD__Moai_context_h__
#define INCLUDE_GUARD__Moai_context_h__

#include "Moai_info.h"

#include <Rano_module_ary.h>
#include <Znk_str.h>
#include <Znk_htp_hdrs.h>

Znk_EXTERN_C_BEGIN

typedef struct MoaiContextImpl* MoaiContext;

struct MoaiContextImpl {
	uint8_t         buf_[ 4096 ];
	ZnkStr          msgs_;
	ZnkStr          text_;
	int             result_size_;
	bool            recv_executed_;
	ZnkHtpReqMethod req_method_;
	MoaiBodyInfo    body_info_;
	MoaiInfoID      draft_info_id_;
	MoaiInfo*       draft_info_;
	bool            as_local_proxy_;
	uint32_t        peer_ipaddr_;
};

MoaiContext
MoaiContext_create( void );
void
MoaiContext_destroy( MoaiContext ctx );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
