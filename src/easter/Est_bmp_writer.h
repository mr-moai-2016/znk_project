#ifndef INCLUDE_GUARD__Est_bmp_writer_h__
#define INCLUDE_GUARD__Est_bmp_writer_h__

#include <Znk_str.h>
#include <Znk_stdc.h>

Znk_EXTERN_C_BEGIN

long
EstBmp_writeBFH( ZnkFile fp, size_t height, size_t bit_per_pixel, size_t rowbytes, ZnkStr ermsg );

long
EstBmp_writeBIH( ZnkFile fp, uint8_t* palette32, const size_t palette32_num, 
		size_t width, size_t height, size_t bit_per_pixel, size_t rowbytes,
		uint8_t* argb_idx, ZnkStr ermsg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
