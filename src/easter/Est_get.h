#ifndef INCLUDE_GUARD__Est_get_h__
#define INCLUDE_GUARD__Est_get_h__

#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

void
EstGet_procHead( RanoCGIEVar* evar, ZnkVarpAry post_vars, const char* est_val );
void
EstGet_procGet( RanoCGIEVar* evar, ZnkVarpAry post_vars, const char* est_val, bool is_view );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
