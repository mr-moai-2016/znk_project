#ifndef INCLUDE_GUARD__Est_boxmap_viewer_h__
#define INCLUDE_GUARD__Est_boxmap_viewer_h__

#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>
#include <Znk_bird.h>

Znk_EXTERN_C_BEGIN

void
EstCacheManager_main( RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr msg, const char* authentic_key );
size_t
EstCache_assortFile( ZnkVarpAry post_vars, ZnkStr msg, const char* begin_key, ZnkStrAry assorted_filelist );
size_t
EstCache_assortOne( ZnkVarpAry post_vars, const char* path, ZnkStr msg, ZnkStr renamed_filename );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
