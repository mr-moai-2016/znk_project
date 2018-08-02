#ifndef INCLUDE_GUARD__CB_custom_automatic_h__
#define INCLUDE_GUARD__CB_custom_automatic_h__

#include <CB_virtualizer.h>
#include <CB_fgp_info.h>
#include <CB_ua_info.h>
#include <CB_config.h>

#include <Rano_cgi_util.h>

Znk_EXTERN_C_BEGIN

CBStatus
CBCustomAutomatic_main( RanoCGIEVar* evar, ZnkVarpAry cb_vars, const char* cb_src,
		ZnkBird bird, ZnkStr RE_key, CBFgpInfo fgp_info, struct CBUAInfo_tag* ua_info,
		ZnkVarpAry main_vars, uint64_t* ptua64, ZnkStr msg, ZnkStr category, ZnkStr lacking_var,
		CBConfigInfo* info, bool is_authenticated, bool all_cookie_clear );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
