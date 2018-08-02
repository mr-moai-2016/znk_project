#ifndef INCLUDE_GUARD__Est_post_h__
#define INCLUDE_GUARD__Est_post_h__

#include <Rano_cgi_util.h>
#include <Rano_type.h>
#include <Rano_module.h>
#include <Znk_varp_ary.h>
#include <Znk_htp_hdrs.h>

Znk_EXTERN_C_BEGIN

typedef void (*EstPostFuncT_filterMain)( const char* result_file,
		const char* src, const char* target, RanoTextType txt_type, ZnkStr console_msg, bool save_img_cache, bool is_https );

void
EstPost_parsePostAndCookieVars( RanoCGIEVar* evar, ZnkVarpAry post_vars, const char* est_val, ZnkStr msg, ZnkHtpHdrs htp_hdrs,
		ZnkStr pst_str, const char* hostname, const char* req_urp, const char* target, RanoModule mod );

void
EstPost_procPost( RanoCGIEVar* evar, ZnkVarpAry post_vars, const char* est_val, ZnkStr msg,
		EstPostFuncT_filterMain filterMain );

void
EstPost_procHyperPost( RanoCGIEVar* evar, ZnkVarpAry post_vars, const char* est_val, ZnkStr msg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
