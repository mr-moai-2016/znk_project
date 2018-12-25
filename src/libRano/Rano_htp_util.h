#ifndef INCLUDE_GUARD__Rano_htp_util_h__
#define INCLUDE_GUARD__Rano_htp_util_h__

#include <Znk_varp_ary.h>
#include <Rano_module.h>

Znk_EXTERN_C_BEGIN

bool
RanoHtpUtil_download( const char* hostname, const char* unesc_req_urp, const char* target,
		const char* ua, ZnkVarpAry cookie,
		const char* parent_proxy,
		ZnkStr result_filename, ZnkStr msg, RanoModule mod, int* status_code, bool is_https,
		const char* tmpdir, const char* explicit_referer, const char* default_ua );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
