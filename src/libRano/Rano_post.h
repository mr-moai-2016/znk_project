#ifndef INCLUDE_GUARD__Rano_post_h__
#define INCLUDE_GUARD__Rano_post_h__

#include <Znk_bfr.h>
#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

void
RanoPost_extendPostVarsToStream_forMPFD( const ZnkVarpAry hpvs, ZnkBfr bfr, const char* boundary );

void
RanoPost_extendPostVarsToStream_forURLE( const ZnkVarpAry hpvs, ZnkBfr bfr );

#if 0
// RanoCGIUtil_splitQueryString ‚ðŽg‚¤‚±‚Æ.
void
RanoPost_registPostVars_byHttpBodyURLE( ZnkVarpAry post_vars, const uint8_t* body, size_t body_leng );
#endif

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
