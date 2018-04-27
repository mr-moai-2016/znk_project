#ifndef INCLUDE_GUARD__Znk_htp_post_h__
#define INCLUDE_GUARD__Znk_htp_post_h__

#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 ZnkHtpPostVar_e_None=0 /* text/plain‚Æ“¯“™‚Æ‚µ‚Äˆµ‚¤ */
	,ZnkHtpPostVar_e_BinaryData
}ZnkHtpPostVarType;

ZnkVarp
ZnkHtpPostVars_regist( ZnkVarpAry vars, const char* name, const char* filename,
		ZnkHtpPostVarType type, const uint8_t* data, size_t data_size );

bool
ZnkHtpPostVars_regist_byMPFDBody( ZnkVarpAry vars,
		const char* boundary, size_t boundary_leng,
		const uint8_t* body, size_t body_size,
		ZnkStr emsg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
