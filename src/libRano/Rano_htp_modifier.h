#ifndef INCLUDE_GUARD__Rano_htp_modifier_h__
#define INCLUDE_GUARD__Rano_htp_modifier_h__

#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

bool
RanoHtpModifier_modifySendHdrs( ZnkVarpAry hdr_vars, const char* ua, ZnkStr msg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
