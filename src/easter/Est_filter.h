#ifndef INCLUDE_GUARD__Est_filter_h__
#define INCLUDE_GUARD__Est_filter_h__

#include <Rano_type.h>
#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

void
EstFilter_main( const char* result_file, const char* src, const char* target, RanoTextType txt_type, ZnkStr console_msg, bool save_img_cache );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
