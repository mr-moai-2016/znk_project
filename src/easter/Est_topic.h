#ifndef INCLUDE_GUARD__Est_topic_h__
#define INCLUDE_GUARD__Est_topic_h__

#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

void
EstTopicManager_main( RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr msg, const char* authentic_key );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
