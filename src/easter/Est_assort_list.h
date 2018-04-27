#ifndef INCLUDE_GUARD__Est_assort_list_h__
#define INCLUDE_GUARD__Est_assort_list_h__

#include <Znk_bird.h>
#include <Znk_varp_ary.h>
#include <Rano_cgi_util.h>

Znk_EXTERN_C_BEGIN

void
EstAssortList_favoritize( ZnkBird bird,
		RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr backto, ZnkStr assort_msg, ZnkStr msg, const char* authentic_key,
		const char* bkt_manager, const char* bkt_vpath_key );
void
EstAssortList_stock( ZnkBird bird,
		RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr backto, ZnkStr assort_msg, ZnkStr msg, const char* authentic_key,
		const char* bkt_manager, const char* bkt_vpath_key );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
