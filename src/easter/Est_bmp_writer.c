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

Znk_INLINE bool
fwrite_withCheck( const uint8_t* ptr, size_t size, ZnkFile fp )
{
	size_t result = Znk_fwrite( ptr, size, fp );
	return (bool)( result == size );
}
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


#if 0
A1_INLINE void
repack_2bitTo4bit( uint8_t* dst_image, const uint8_t* src_image, size_t width, size_t height,
		size_t dst_rowbytes, size_t src_rowbytes )
{
	uint8_t*       dst_line;
	const uint8_t* src_line;
	size_t i,j;
	for( j=0; j<height; ++j ){
		dst_line = dst_image + dst_rowbytes * j;
		src_line = src_image + src_rowbytes * j; 
		for( i=0; i<width; ++i ){
			C1IgBit4_set( dst_line, i, C1IgBit2_get( src_line, i ) );
		}
	}
}

/***
 * 画像データの書き出し
 */
static bool
writeInternalImage( BImgIO io_info, C1Img bimg, FILE* fp,
		const uint8_t* image, size_t width, size_t height, size_t rowbytes, size_t bit_per_pixel, C1PixelOrder pixel_order,
		bool is_vertical_reverse, bool is_flip_pixel_for_sys, C1Err* c1err )
{
	/***
	 * info.rowbytesの値は info.bit_per_pixelの値も考慮されている.
	 */
	const size_t pad_byte   = C1Img_pad_byte( bimg );
	const size_t image_size = rowbytes * height;
	uint8_t* row_pointer = NULL;
	//C1Str msg_str = BImgIO_msg_str( io_info );

	/***
	 * bit_per_pixel の値に関わらず、BMPでは4byte 境界でimageを格納する.
	 */
	switch( bit_per_pixel ){
	case 32:
		if( is_vertical_reverse && pixel_order == C1PixelOrder_e_BGRA && pad_byte == 4 ){
			/* この場合、一括して書き込むことができる */
			if( !fwrite_withCheck( image, image_size, fp ) ){ goto FUNC_ERROR; }
		} else {
			bool is_ok = true;
			size_t j,J;
	
			if( pixel_order == C1PixelOrder_e_BGRA ){
				/* flip不要 */
				for( j=0; j<height; ++j ){
					/* is_vertical_reverse がtrueの場合はそのままの順序で格納する */
					J = is_vertical_reverse ? j : (height-1-j);
					row_pointer = (uint8_t*)image + J*rowbytes;
					if( !fwrite_withCheck( row_pointer, rowbytes, fp ) ){
						is_ok = false;
						break;
					}
				}
			} else {
				C1Buf wkbuf1 = BImgIO_wkbuf1( io_info );
				uint8_t* tmp_line;
				if( !C1Buf_resize( wkbuf1, rowbytes ) ){
					C1Err_internf( c1err,
							"writeInternalImage : memory_allocation failure\n" );
					goto FUNC_ERROR;
				}
				tmp_line = C1Buf_data(wkbuf1);

				for( j=0; j<height; ++j ){
					/* is_vertical_reverse がtrueの場合はそのままの順序で格納する */
					J = is_vertical_reverse ? j : (height-1-j);
					row_pointer = (uint8_t*)image + J*rowbytes;
					memcpy( tmp_line, row_pointer, rowbytes );
					C1IgByte_flipPixel32( tmp_line,
							width, 1, rowbytes, C1PixelOrder_e_BGRA, pixel_order );
					if( !fwrite_withCheck( tmp_line, rowbytes, fp ) ){
						is_ok = false;
						break;
					}
				}
				C1Buf_resize( wkbuf1, 0 );
			}
			if( !is_ok ){ goto FUNC_ERROR; }
		}
		break;
	case 24:
		if( is_vertical_reverse && pixel_order == C1PixelOrder_e_BGR && pad_byte == 4 ){
			/* この場合、一括して書き込むことができる */
			if( !fwrite_withCheck( image, image_size, fp ) ){ goto FUNC_ERROR; }
		} else {
			bool is_ok = true;
			size_t j,J;
	
			if( pixel_order == C1PixelOrder_e_BGR ){
				/* flip不要 */
				for( j=0; j<height; ++j ){
					/* is_vertical_reverse がtrueの場合はそのままの順序で格納する */
					J = is_vertical_reverse ? j : (height-1-j);
					row_pointer = (uint8_t*)image + J*rowbytes;
					if( !fwrite_withCheck( row_pointer, rowbytes, fp ) ){
						is_ok = false;
						break;
					}
				}
			} else {
				C1Buf wkbuf1 = BImgIO_wkbuf1( io_info );
				uint8_t* tmp_line;
				if( !C1Buf_resize( wkbuf1, rowbytes ) ){
					C1Err_internf( c1err,
							"writeInternalImage : memory_allocation failure\n" );
					goto FUNC_ERROR;
				}
				tmp_line = C1Buf_data(wkbuf1);

				for( j=0; j<height; ++j ){
					/* is_vertical_reverse がtrueの場合はそのままの順序で格納する */
					J = is_vertical_reverse ? j : (height-1-j);
					row_pointer = (uint8_t*)image + J*rowbytes;
					memcpy( tmp_line, row_pointer, rowbytes );
					C1IgByte_flipPixel24( tmp_line,
							width, 1, rowbytes, C1PixelOrder_e_BGR, pixel_order );
					if( !fwrite_withCheck( tmp_line, rowbytes, fp ) ){
						is_ok = false;
						break;
					}
				}
				C1Buf_resize( wkbuf1, 0 );
			}
			if( !is_ok ){ goto FUNC_ERROR; }
		}
		break;
	default:
		/* gray-scale=>BGRA変換は上位過程で済ませているため、
		 * この時点ではgray-scaleの可能性はない */

		if( is_vertical_reverse && pad_byte == 4 ){
			/* この場合、一括して書き込むことができる */
			if( !fwrite_withCheck( image, image_size, fp ) ){ goto FUNC_ERROR; }
		} else {
			bool is_ok = true;
			size_t j,J;
			for( j=0; j<height; ++j ){
				/* is_vertical_reverse がtrueの場合はそのままの順序で格納する */
				J = is_vertical_reverse ? j : (height-1-j);
				row_pointer = (uint8_t*)image + J*rowbytes;
				if( !fwrite_withCheck( row_pointer, rowbytes, fp ) ){
					is_ok = false;
					break;
				}
			}
			if( !is_ok ){ goto FUNC_ERROR; }
		}
		break;
	}

	return true;

FUNC_ERROR:
	return false;
}


/**
 * 書き出し
 */
static bool
BImgBmp_saveFP( BImgIO io_info, C1Img bimg, FILE* fp )
{
	bool result = false;
	const size_t width         = C1Img_width( bimg );
	const size_t height        = C1Img_height( bimg );
	const uint32_t req_type    = BImgIO_Req_get( io_info );
	const uint32_t state_type  = C1Img_State_get( bimg );
	const bool is_vertical_reverse   = (bool)( state_type & C1Img_e_IsVerticalReverse );
	const bool is_flip_pixel_for_sys = (bool)( req_type & BImgIO_e_FlipPixelForSys );
	const uint8_t* image   = C1Img_data( bimg );
	C1Buf    buf_2nd       = C1Img_buf_2nd( bimg );
	uint8_t* palette32     = C1Buf_data( buf_2nd );
	size_t   palette32_num = C1Buf_size( buf_2nd ) / 4;
	C1Buf  wkbuf1 = BImgIO_wkbuf1( io_info );
	size_t rowbytes      = C1Img_rowbytes( bimg );
	size_t bit_per_pixel = C1Img_bit_per_pixel( bimg );
	C1PixelOrder pixel_order = C1Img_pixel_order( bimg );
	C1Err* c1err = BImgIO_c1err( io_info );

	if( (state_type & C1Img_e_HasPalette) == 0 ){
		palette32 = NULL;
		palette32_num = 0;
	}

	if( C1Img_State_is( bimg, C1Img_e_HasPalette ) ){
		if( bit_per_pixel == 2 ){
			/***
			 * BMPでは2bitインデックスをサポートしていない.
			 * よって4 or 8bitへ変換しなければならない.
			 * ここでは4bitへ変換する.
			 */
			size_t rowbytes_new = A1_M_BIT_TO_BYTE_PAD_4( width * 4 );
			if( !C1Buf_resize( wkbuf1, height*rowbytes_new ) ){
				C1Err_internf( c1err,
						"BImgBmp_saveFile : memory_allocation failure." );
				goto FUNC_END;
			}
			repack_2bitTo4bit( C1Buf_data(wkbuf1), image, width, height, rowbytes_new, rowbytes );
			bit_per_pixel = 4;
			rowbytes = rowbytes_new;
			image = C1Buf_data( wkbuf1 );
		}
	} else if( C1Img_State_is( bimg, C1Img_e_IsGray ) ){
		/* gray-scaleと考えられる. RGBへの変換が必要.
		 * ここでは32bitイメージに変換する. */
		size_t rowbytes_new = A1_M_GET_PAD_4( width * 4 );
		if( !C1Buf_resize( wkbuf1, height*rowbytes_new ) ){
			C1Err_internf( c1err,
					"BImgBmp_saveFile : memory_allocation failure." );
			goto FUNC_END;
		}
		if( bit_per_pixel == 16 ){
			if( !C1Buf_assignBuf( wkbuf1, 0, C1Img_buf_1st(bimg), 0, height*rowbytes ) ){
				C1Err_internf( c1err,
						"BImgBmp_saveFile : C1Buf_assignBuf failure." );
				goto FUNC_END;
			}
			if( !C1Ig32_expand_byGrayGA2( wkbuf1,
					width, height, C1Img_pad_byte(bimg),
					C1PixelOrder_e_BGRA, pixel_order ) ){
				C1Err_internf( c1err,
						"BImgBmp_saveFile : C1Ig32_expand_byGrayGA2 failure." );
				goto FUNC_END;
			}
		} else {
			if( !C1Ig32_make_byGray2( wkbuf1, image,
					width, height, bit_per_pixel, C1Img_pad_byte(bimg),
					C1PixelOrder_e_BGRA, 0xff ) ){
				C1Err_internf( c1err,
						"BImgBmp_saveFile : C1Ig32_make_byGray2 failure." );
				goto FUNC_END;
			}
		}
		bit_per_pixel = 32;
		pixel_order = C1PixelOrder_e_BGRA;
		rowbytes = rowbytes_new;
		image = C1Buf_data( wkbuf1 );
	}

	writeBFH( io_info, fp, height, bit_per_pixel, rowbytes );
	writeBIH( io_info, fp, palette32, palette32_num,
			width, height, bit_per_pixel, rowbytes, pixel_order );

	result = writeInternalImage( io_info, bimg, fp,
			image, width, height, rowbytes, bit_per_pixel, pixel_order,
			is_vertical_reverse, is_flip_pixel_for_sys, c1err );
	if( !result ){
		C1Err_internf( c1err,
				"BImgBmp_saveFile : Error : [%s]\n", C1Err_cstr(*c1err) );
	}

FUNC_END:
	C1Buf_resize( wkbuf1, 0 );

	return result;
}

bool
BImgBmp2_saveFile( BImgIO io_info, C1Img bimg, const char* filename )
{
	bool result;
	FILE* fp = fopen( filename, "wb" );
	if( fp==NULL ){
		C1Err* c1err = BImgIO_c1err( io_info );
		C1Err_internf( c1err,
				"BImgBmp_saveFile : Error : Cannot open file [%s]", filename );
		return false;
	}
	result = BImgBmp_saveFP( io_info, bimg, fp );

	fclose(fp);
	return result;
}

/* endof Writer */
/*****************************************************************************/
#endif

