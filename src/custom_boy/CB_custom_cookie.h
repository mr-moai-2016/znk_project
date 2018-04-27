#ifndef INCLUDE_GUARD__CB_custom_cookie_h__
#define INCLUDE_GUARD__CB_custom_cookie_h__

#include <CB_virtualizer.h>
#include <CB_fgp_info.h>
#include <CB_config.h>

Znk_EXTERN_C_BEGIN

CBStatus
CBCustomCookie_main( ZnkVarpAry cb_vars,
		ZnkBird bird, ZnkStr RE_key, CBFgpInfo fgp_info,
		ZnkVarpAry main_vars, uint64_t* ptua64,
		CBConfigInfo* info, bool is_authenticated );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
