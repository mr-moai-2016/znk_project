#ifndef INCLUDE_GUARD__Moai_post_h__
#define INCLUDE_GUARD__Moai_post_h__

#include <Znk_socket.h>
#include <Znk_fdset.h>
#include <Rano_module_ary.h>
#include "Moai_info.h"
#include "Moai_fdset.h"

Znk_EXTERN_C_BEGIN

typedef enum {
	 MoaiPostMode_e_Confirm=0
	,MoaiPostMode_e_SendAfterConfirm
	,MoaiPostMode_e_SendDirect
} MoaiPostMode;


MoaiPostMode
MoaiPost_decidePostMode2( ZnkMyf config, const char* dst_hostname );
bool
MoaiPost_isPostConfirm( void );
void
MoaiPost_setPostConfirm( bool post_confirm );

bool
MoaiPost_parsePostAndCookieVars( ZnkSocket sock, MoaiFdSet mfds,
		ZnkStr str,
		const size_t hdr_size, ZnkHtpHdrs htp_hdrs,
		size_t content_length, ZnkBfr stream,
		ZnkVarpAry post_vars,
		const RanoModule mod, bool is_authenticated );

bool
MoaiPost_sendRealy( MoaiInfoID info_id,
		bool is_realy_send, ZnkSocket O_sock, MoaiFdSet mfds );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
