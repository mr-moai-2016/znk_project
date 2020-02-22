#include <Znk_stdc.h>
#include <Znk_str.h>
#include <math.h>

/* �����͏�� sizeof(RGBQuad) �ł��� */
enum {
	e_palette_byte_per_pixel = 4
};

typedef struct tagRGBQuad {
	uint8_t rgbBlue;
	uint8_t rgbGreen;
	uint8_t rgbRed;
	uint8_t rgbReserved;
} RGBQuad;

#if 0
Znk_INLINE bool
fwrite_withCheck( const uint8_t* ptr, size_t size, ZnkFile fp )
{
	size_t result = Znk_fwrite( ptr, size, fp );
	return (bool)( result == size );
}
#endif
Znk_INLINE bool
isNeedSwap( bool is_LE )
{
	return ( Znk_isLE() && !is_LE ) || ( !Znk_isLE() && is_LE );
}
static bool
fwrite_blk_withSwap( uint8_t* ptr, size_t blk_size, size_t blk_num, ZnkFile fp, bool is_LE )
{
	size_t result = 0;
	uint8_t buf[ 8 ] = { 0 };
	Znk_memcpy( buf, ptr, blk_size );
	switch( blk_size ){
	case 2:
		if( isNeedSwap( is_LE ) ){
			Znk_swap2byte( buf );
		}
		break;
	case 4:
		if( isNeedSwap( is_LE ) ){
			Znk_swap4byte( buf );
		}
		break;
	case 8:
		if( isNeedSwap( is_LE ) ){
			Znk_swap8byte( buf );
		}
		break;
	default:
		break;
	}
	result = Znk_fwrite_blk( buf, blk_size, blk_num, fp );
	return (bool)( result == blk_num );
}
Znk_INLINE bool
fwriteI32( int32_t val, size_t blk_num, ZnkFile fp, bool is_LE ){
	return fwrite_blk_withSwap( (uint8_t*)&val, sizeof(int32_t), blk_num, fp, is_LE );
}
Znk_INLINE bool
fwriteI16( int16_t val, size_t blk_num, ZnkFile fp, bool is_LE ){
	return fwrite_blk_withSwap( (uint8_t*)&val, sizeof(int16_t), blk_num, fp, is_LE );
}

/***
 * write BitmapFileHeader
 */
long
EstBmp_writeBFH( ZnkFile fp, size_t height, size_t bit_per_pixel, size_t rowbytes, ZnkStr ermsg )
{
	static const bool  is_LE    = true; /* BMP�t�@�C���t�H�[�}�b�g�ł�LE */
	static const short reserved = 0;
	/* �摜�f�[�^�܂ł̃I�t�Z�b�g(�w�b�_�T�C�Y) */
	const size_t pal_n    = ( bit_per_pixel >= 24 ) ? 0 : 256;
	const size_t offset   = 14 + 40 + pal_n * sizeof(RGBQuad);
	const size_t size     = rowbytes * height;
	const long   bfoffset = (long)offset;
	const long   bfsize   = (long)( size + offset );

	long  count;

	/* �w�b�_�̐擪 : ���ʕ��� BM */
	Znk_fwrite( (uint8_t*)"BM", 2, fp );

	/* �t�@�C���T�C�Y : bfSize  */
	fwriteI32( bfsize, 1, fp, is_LE );

	/* �\��G���A : bfReserved1 */
	fwriteI16( reserved, 1, fp, is_LE );

	/* �\��G���A : bfReserved2 */
	fwriteI16( reserved, 1, fp, is_LE );

	/* �f�[�^���܂ł̃I�t�Z�b�g : bfOffBits */
	fwriteI32( bfoffset, 1, fp, is_LE);

	/* verify : �t�@�C���w�b�_�T�C�Y 14 Byte */
	count = (long)Znk_ftell_i64( fp );
	if( count != 14 ){
		if( ermsg ){
			ZnkStr_addf( ermsg, "Est_bmp_writer : writeBFH : BitmapFileHeader write error : %ld\n", count );
		}
		return 0;
	}
	return count;
}

/***
 * write BitmapInfoHeader
 */
long
EstBmp_writeBIH( ZnkFile fp, uint8_t* palette32, const size_t palette32_num, 
		size_t width, size_t height, size_t bit_per_pixel, size_t rowbytes,
		uint8_t* argb_idx, ZnkStr ermsg )
{
	static const bool is_LE = true; /* BMP�t�@�C���t�H�[�}�b�g�ł�LE */
	const long pal_n = ( bit_per_pixel >= 24 ) ? 0 : 256;
	long  count;
	long  var_long;
	short var_short;

	/***
	 * ���w�b�_�̃T�C�Y biSize (Windows BMP �� 40) 
	 */
	var_long = 40;
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * �摜�̕� biWidth 
	 */
	var_long = (long)width;
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * �摜�̍��� biHeight 
	 * (�����Ȃ�΍�������E��, �}�C�i�X�Ȃ�΍��ォ��E��) 
	 */
	var_long = (long)height;
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * �v���[���� biPlanes (�K�� 1) 
	 */
	var_short = 1;
	fwriteI16( var_short, 1, fp, is_LE );

	/***
	 * 1�s�N�Z���̃f�[�^�� biBitCount (1, 4, 8, 24, 32)
	 */
	var_short = (short)bit_per_pixel;
	fwriteI16( var_short, 1, fp, is_LE );

	/***
	 * ���k biCompression (�����k�Ȃ�� 0) 
	 */
	var_long = 0;
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * �摜�̃T�C�Y biSizeImage 
	 */
	var_long = (long)( rowbytes * height );
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * �������𑜓x pixel/m biXPelPerMeter 
	 * (96dpi, 1inch = 0.0254m �̂Ƃ� 96/0.0254 = 3780) 
	 */
	var_long = 0;
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * �c�����𑜓x pixel/m biYPelPerMeter 
	 * (96dpi, 1inch = 0.0254m �̂Ƃ� 96/0.0254 = 3780) 
	 */
	var_long = 0;
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * �p���b�g�� biClrUsed: 1 << info->bit_per_pixel;
	 */
	if( bit_per_pixel >= 24 ){
		var_long = 0;
		fwriteI32( var_long, 1, fp, is_LE );
	} else {
		var_long = (long)palette32_num;
		fwriteI32( var_long, 1, fp, is_LE );
	}

	/***
	 * �p���b�g���̏d�v�ȐF  biClrImportant 
	 */
	var_long = 0;
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * �p���b�g�̏����o��
	 */
	if( palette32 && bit_per_pixel < 24 ){
		if( argb_idx == NULL ){
			/* ���̂Ƃ� palette32 �� BGRA ��PixelOrder�łȂ���΂Ȃ�Ȃ�. */
			Znk_fwrite_blk( palette32, e_palette_byte_per_pixel, palette32_num, fp );
		} else {
			size_t i;
			uint8_t plt_pix4[ 4 ];
			uint8_t* pos;
			if( argb_idx[ 0 ] == 0xf ){
				for( i=0; i<palette32_num; ++i ){
					pos = palette32 + i*e_palette_byte_per_pixel;
					plt_pix4[ 0 ] = pos[ argb_idx[ 3 ] ];
					plt_pix4[ 1 ] = pos[ argb_idx[ 2 ] ];
					plt_pix4[ 2 ] = pos[ argb_idx[ 1 ] ];
					plt_pix4[ 3 ] = 0xf;
					Znk_fwrite_blk( plt_pix4, e_palette_byte_per_pixel, 1, fp );
				}
			} else {
				for( i=0; i<palette32_num; ++i ){
					pos = palette32 + i*e_palette_byte_per_pixel;
					plt_pix4[ 0 ] = pos[ argb_idx[ 3 ] ];
					plt_pix4[ 1 ] = pos[ argb_idx[ 2 ] ];
					plt_pix4[ 2 ] = pos[ argb_idx[ 1 ] ];
					plt_pix4[ 3 ] = pos[ argb_idx[ 0 ] ];
					Znk_fwrite_blk( plt_pix4, e_palette_byte_per_pixel, 1, fp );
				}
			}
		}
	}
	/***
	 * �t�@�C���w�b�_(14 Byte) + ���w�b�_�T�C�Y(40 Byte) + �p���b�g��
	 */
	count = (long)Znk_ftell_i64( fp );
	if( (size_t)count != 54+sizeof(RGBQuad)*pal_n ){
		ZnkStr_addf( ermsg,
				"Est_bmp_writer : writeBIH : BitmapInfoHeader write error : %ld\n", count );
		return 0;
	}
	return count;
}


