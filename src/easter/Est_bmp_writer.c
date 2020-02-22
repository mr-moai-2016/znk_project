#include <Znk_stdc.h>
#include <Znk_str.h>
#include <math.h>

/* ここは常に sizeof(RGBQuad) である */
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
	static const bool  is_LE    = true; /* BMPファイルフォーマットではLE */
	static const short reserved = 0;
	/* 画像データまでのオフセット(ヘッダサイズ) */
	const size_t pal_n    = ( bit_per_pixel >= 24 ) ? 0 : 256;
	const size_t offset   = 14 + 40 + pal_n * sizeof(RGBQuad);
	const size_t size     = rowbytes * height;
	const long   bfoffset = (long)offset;
	const long   bfsize   = (long)( size + offset );

	long  count;

	/* ヘッダの先頭 : 識別文字 BM */
	Znk_fwrite( (uint8_t*)"BM", 2, fp );

	/* ファイルサイズ : bfSize  */
	fwriteI32( bfsize, 1, fp, is_LE );

	/* 予約エリア : bfReserved1 */
	fwriteI16( reserved, 1, fp, is_LE );

	/* 予約エリア : bfReserved2 */
	fwriteI16( reserved, 1, fp, is_LE );

	/* データ部までのオフセット : bfOffBits */
	fwriteI32( bfoffset, 1, fp, is_LE);

	/* verify : ファイルヘッダサイズ 14 Byte */
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
	static const bool is_LE = true; /* BMPファイルフォーマットではLE */
	const long pal_n = ( bit_per_pixel >= 24 ) ? 0 : 256;
	long  count;
	long  var_long;
	short var_short;

	/***
	 * 情報ヘッダのサイズ biSize (Windows BMP は 40) 
	 */
	var_long = 40;
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * 画像の幅 biWidth 
	 */
	var_long = (long)width;
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * 画像の高さ biHeight 
	 * (正数ならば左下から右上, マイナスならば左上から右下) 
	 */
	var_long = (long)height;
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * プレーン数 biPlanes (必ず 1) 
	 */
	var_short = 1;
	fwriteI16( var_short, 1, fp, is_LE );

	/***
	 * 1ピクセルのデータ数 biBitCount (1, 4, 8, 24, 32)
	 */
	var_short = (short)bit_per_pixel;
	fwriteI16( var_short, 1, fp, is_LE );

	/***
	 * 圧縮 biCompression (無圧縮ならば 0) 
	 */
	var_long = 0;
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * 画像のサイズ biSizeImage 
	 */
	var_long = (long)( rowbytes * height );
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * 横方向解像度 pixel/m biXPelPerMeter 
	 * (96dpi, 1inch = 0.0254m のとき 96/0.0254 = 3780) 
	 */
	var_long = 0;
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * 縦方向解像度 pixel/m biYPelPerMeter 
	 * (96dpi, 1inch = 0.0254m のとき 96/0.0254 = 3780) 
	 */
	var_long = 0;
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * パレット数 biClrUsed: 1 << info->bit_per_pixel;
	 */
	if( bit_per_pixel >= 24 ){
		var_long = 0;
		fwriteI32( var_long, 1, fp, is_LE );
	} else {
		var_long = (long)palette32_num;
		fwriteI32( var_long, 1, fp, is_LE );
	}

	/***
	 * パレット中の重要な色  biClrImportant 
	 */
	var_long = 0;
	fwriteI32( var_long, 1, fp, is_LE );

	/***
	 * パレットの書き出し
	 */
	if( palette32 && bit_per_pixel < 24 ){
		if( argb_idx == NULL ){
			/* このとき palette32 は BGRA のPixelOrderでなければならない. */
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
	 * ファイルヘッダ(14 Byte) + 情報ヘッダサイズ(40 Byte) + パレット数
	 */
	count = (long)Znk_ftell_i64( fp );
	if( (size_t)count != 54+sizeof(RGBQuad)*pal_n ){
		ZnkStr_addf( ermsg,
				"Est_bmp_writer : writeBIH : BitmapInfoHeader write error : %ld\n", count );
		return 0;
	}
	return count;
}


