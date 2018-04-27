#ifndef INCLUDE_GUARD__Est_img_viewer_h__
#define INCLUDE_GUARD__Est_img_viewer_h__

#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>
#include <Znk_bird.h>

Znk_EXTERN_C_BEGIN

void
EstImgViewer_makeTopbarUI_forLoading( ZnkStr topbar_ui, bool is_video );

void
EstImgViewer_makeNowLoading( ZnkStr ans, RanoTextType txt_type, const char* result_filename );

void
EstImgViewer_main( RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr msg, const char* authentic_key, bool is_video );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
