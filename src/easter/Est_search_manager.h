#ifndef INCLUDE_GUARD__Est_search_manager_h__
#define INCLUDE_GUARD__Est_search_manager_h__

#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>
#include <Est_sqi.h>

Znk_EXTERN_C_BEGIN

void
EstSearchManager_searchInBox( EstSQI sqi, ZnkStr msg, const char* searched_key, bool force_fsys_scan, const char* searched_dir );

void
EstSearchManager_main( RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr msg, const char* authentic_key );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
