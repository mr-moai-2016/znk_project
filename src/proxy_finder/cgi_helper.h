#ifndef INCLUDE_GUARD__cgi_helper_h__
#define INCLUDE_GUARD__cgi_helper_h__

#include <Rano_cgi_util.h>

#include <Znk_str.h>
#include <Znk_varp_ary.h>
#include <Znk_bird.h>

Znk_EXTERN_C_BEGIN

const char*
CGIHelper_theMoaiDir( ZnkStr ermsg );

const char*
CGIHelper_loadAuthenticKey( ZnkStr ermsg );

bool
CGIHelper_isAuthenticated( ZnkVarpAry post_vars, const char* authentic_key );

bool
CGIHelper_initEsscential( ZnkVarpAry post_vars,
		const char** moai_dir, const char** authentic_key, bool* is_authenticated,
		ZnkStr ermsg );

bool
CGIHelper_printTemplateHTML( RanoCGIEVar* evar, ZnkBird bird, const char* template_html_file, const char* authentic_key, ZnkStr ermsg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
