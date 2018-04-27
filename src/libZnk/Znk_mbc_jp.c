#include "Znk_mbc_jp.h"
#include "Znk_bfr_bif.h"
#include <string.h>


#ifdef __BORLANDC__
/**
 * 警告 W8071 hoge.cpp XX: 変換によって有効桁が失われる(関数hoge) )
 *
 * このモジュールでは、0xFF 等を8bit整数としてuint8_t と一緒に使う処理を多用しているため.
 * この警告が多く表示される.  このモジュールに限ればこれは無意味な警告である.
 */
#  pragma warn -8071
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1300)
/***
 * VC6.0のみ
 * warning C4761: 関数の仮引数と実引数のﾍﾞｰｽ型が異なっています。実引数を仮引数の型に変換します。 
 * を抑制(VC6.0では0x80などのリテラル表記をuint8_t型引数へ代入するなどするとこの警告が出る).
 */
#  pragma warning(disable: 4761)
#endif



Znk_INLINE bool UTF8_isMidByte   ( uint8_t b ){ return (bool)((b & 0xc0) == 0x80); } /* 2バイト目以降は必ず 10xxxxxx */
Znk_INLINE bool UTF8_isFirstByte2( uint8_t b ){ return (bool)((b & 0xe0) == 0xc0); } /* 2byte文字の開始 110xxxxx 10xxxxxx (実質11bit) */
Znk_INLINE bool UTF8_isFirstByte3( uint8_t b ){ return (bool)((b & 0xf0) == 0xe0); } /* 3byte文字の開始 1110xxxx 10xxxxxx, ... (実質16bit) */
Znk_INLINE bool UTF8_isFirstByte4( uint8_t b ){ return (bool)((b & 0xf8) == 0xf0); } /* 4byte文字の開始 11110xxx 10xxxxxx, ... (実質21bit) */
Znk_INLINE int  UTF8_getFirstByteType( uint8_t b )
{
	if( b < 0x80 ){ return 1; } /* ASCII. */
	if( UTF8_isMidByte(    b ) ){ return 0; }
	if( UTF8_isFirstByte2( b ) ){ return 2; }
	if( UTF8_isFirstByte3( b ) ){ return 3; }
	if( UTF8_isFirstByte4( b ) ){ return 4; }
	return -1; /* Invalid UTF8 */
}


/***
 * 指定するv32の値はUnicode空間でのスカラー値である.
 * いわゆるワイド文字の整数値ではない.
 * ワイト文字の整数値はUTF16のバイトイメージの方を2byteあるいは4byte単位で抜き出したものであり、
 * ここで指定するUnicode空間でのスカラー値v32とは異なる.
 */
Znk_INLINE bool
UTF8_addUnicodeVal32( ZnkBif utf8s, uint32_t v32 )
{
	if (v32 < 0x80) { /* 0x0000-0x007F */
		ZnkBif_push_bk( utf8s, (uint8_t)v32 );
	}
	else if (v32 < 0x800) { /* 0x0080-0x07FF */
		const uint8_t b2 = (uint8_t)((v32     ) & 0x3f);
		const uint8_t b1 = (uint8_t)((v32 >> 6)       );
		ZnkBif_push_bk( utf8s, (b1 | 0xc0) );
		ZnkBif_push_bk( utf8s, (b2 | 0x80) );
	}
	else if (v32 < 0x10000) { /* 0x0800-0xFFFF */
		const uint8_t b3 = (uint8_t)((v32      ) & 0x3f);
		const uint8_t b2 = (uint8_t)((v32 >>  6) & 0x3f);
		const uint8_t b1 = (uint8_t)((v32 >> 12)       );
		ZnkBif_push_bk( utf8s, (b1 | 0xe0) );
		ZnkBif_push_bk( utf8s, (b2 | 0x80) );
		ZnkBif_push_bk( utf8s, (b3 | 0x80) );
	}
	else if (v32 < 0x200000) { /* 0x00010000-0x001FFFFF */
		const uint8_t b4 = (uint8_t)((v32      ) & 0x3f);
		const uint8_t b3 = (uint8_t)((v32 >>  6) & 0x3f);
		const uint8_t b2 = (uint8_t)((v32 >> 12) & 0x3f);
		const uint8_t b1 = (uint8_t)((v32 >> 18)       );
		ZnkBif_push_bk( utf8s, (b1 | 0xf0) );
		ZnkBif_push_bk( utf8s, (b2 | 0x80) );
		ZnkBif_push_bk( utf8s, (b3 | 0x80) );
		ZnkBif_push_bk( utf8s, (b4 | 0x80) );
	}
	else if (v32 < 0x4000000) { /* 0x00200000-0x03FFFFFF */
		const uint8_t b5 = (uint8_t)((v32      ) & 0x3f);
		const uint8_t b4 = (uint8_t)((v32 >>  6) & 0x3f);
		const uint8_t b3 = (uint8_t)((v32 >> 12) & 0x3f);
		const uint8_t b2 = (uint8_t)((v32 >> 18) & 0x3f);
		const uint8_t b1 = (uint8_t)((v32 >> 24)       );
		ZnkBif_push_bk( utf8s, (b1 | 0xf8) );
		ZnkBif_push_bk( utf8s, (b2 | 0x80) );
		ZnkBif_push_bk( utf8s, (b3 | 0x80) );
		ZnkBif_push_bk( utf8s, (b4 | 0x80) );
		ZnkBif_push_bk( utf8s, (b5 | 0x80) );
	}
	else if (v32 < 0x80000000) { /* 0x04000000-0x7FFFFFFF */
		const uint8_t b6 = (uint8_t)((v32      ) & 0x3f);
		const uint8_t b5 = (uint8_t)((v32 >>  6) & 0x3f);
		const uint8_t b4 = (uint8_t)((v32 >> 12) & 0x3f);
		const uint8_t b3 = (uint8_t)((v32 >> 18) & 0x3f);
		const uint8_t b2 = (uint8_t)((v32 >> 24) & 0x3f);
		const uint8_t b1 = (uint8_t)((v32 >> 30)       );
		ZnkBif_push_bk( utf8s, (b1 | 0xfc) );
		ZnkBif_push_bk( utf8s, (b2 | 0x80) );
		ZnkBif_push_bk( utf8s, (b3 | 0x80) );
		ZnkBif_push_bk( utf8s, (b4 | 0x80) );
		ZnkBif_push_bk( utf8s, (b5 | 0x80) );
		ZnkBif_push_bk( utf8s, (b6 | 0x80) );
	}
	else {
		return false;
	}
	return true;
}

/***
 * UTF-16符号化形式
 * UTF-16では、Unicodeの符号位置U+0000..U+10FFFFを、16ビット符号なし整数を符号単位とした符号単位列で表す。
 * Unicodeの符号位置の最大がU+10FFFFなのは、それがUTF-16で表せる最大だからである。
 * 符号単位列は1つまたは2つの符号単位からなる。つまり、合計は16ビットまたは32ビットである。
 *
 * BMPに含まれるU+0000..U+D7FFとU+E000..U+FFFFは、そのまま符号単位1つで表す。
 * U+D800 ～ U+DFFF の符号位置は代用符号位置（Surrogate Code Point）と呼ばれ、特別な意味で使用されるため、
 * BMPのこの領域には文字が収録されておらず、UTF-16以外の符号化スキームでは通常は使用されない。
 *
 * スカラ値(16bit)          UTF-16(16bit)
 * xxxxxxxx xxxxxxxx        xxxxxxxx xxxxxxxx 
 *
 * BMP以外のU+10000..U+10FFFFは、以下のようにビットを配分して、符号単位2つで表す。
 * BMP外の1つの符号位置を表す連続した2つの代用符号位置のペアをサロゲートペアと呼ぶ。
 * 以下では 110110 と 110111 がそれにあたる.
 *
 * スカラ値(21bit)              UTF-16(32bit)
 * 000uuuuu xxxxxxxx xxxxxxxx   110110ww wwxxxxxx 110111xx xxxxxxxx
 * ここで wwww = uuuuu - 1 である. uuuuu はかならず 1 以上 10000以下であり、-1 するのは
 * 5bitを4bitにすることで領域の無駄を1bit減らせるからである.
 */

/***
 * 指定するv16,v32の値はUnicode空間でのスカラー値である.
 * いわゆるワイド文字の整数値ではない.
 * ワイト文字の整数値はUTF16のバイトイメージの方を2byteあるいは4byte単位で抜き出したものであり、
 * ここで指定するUnicode空間でのスカラー値v16,v32とは異なる.
 */
Znk_INLINE void
UTF16_addUnicodeVal16( ZnkBif utf16s, uint16_t v16 ) {
	ZnkBif_append( utf16s, (uint8_t*)&v16, sizeof(uint16_t) );
}

Znk_INLINE bool
UTF16_addUnicodeVal32( ZnkBif utf16s, uint32_t v32 )
{
	if( v32 < 0x10000 ){
		/***
		 * U+0000..U+D7FFとU+E000..U+FFFFの範囲(BMPに含まれる).
		 * そのまま符号単位1つで表す。
		 */
		UTF16_addUnicodeVal16( utf16s, (uint16_t)(v32) );
	} else if (v32 < 0x110000) {
		/***
		 * U+10000..U+10FFFFの範囲(BMP外)
		 * 最大21bit
		 */
		uint16_t high =
			(uint16_t)((v32 - 0x10000) >> 10) | /* 上位11bit(uuuuu xxxxxxに相当)の取得. */
			0xd800; /* 第1のサロゲートペア 110110 を付加するため、1101 1000 0000 0000(=0xd800) のbitを立てる */
		uint16_t low =
			(uint16_t)(v32 & 0x03ff) | /* 下位10bitの取得 */
			0xdc00; /* 第2のサロゲートペア 110111 を付加するため、1101 1100 0000 0000(=0xdc00) のbitを立てる */
		UTF16_addUnicodeVal16( utf16s, high );
		UTF16_addUnicodeVal16( utf16s, low );
	} else {
		/***
		 * 22bit以上の部分は使われない.
		 * したがってv32がこれ以上の値であった場合は異常.
		 */
		return false;
	}
	return true;
}


static bool st_shortest_flag   = true;
static int  st_replace_invalid = 0;

/**
 * Convert UTF-8 to UTF-16-LE (byte order: LittleEndian)
 */
bool
ZnkMbc_convertUTF8_toUTF16( const char* utf8s, size_t utf8s_leng, ZnkBif utf16s, ZnkErr* err )
{
	const uint8_t* utf8p = (const uint8_t*)utf8s;
	size_t         inlen = utf8s_leng;

	uint16_t u16 = 0;

	while( inlen > 0 ){
		const uint8_t c0 = utf8p[0];

		if( (c0 & 0x80) == 0 ){ /* 0xxxxxxx (7bit) : ASCIIコード互換 */
			if( c0 == 0 ){
				/***
				 * UTF-8の1-byte目が0:
				 * 一応UTF-8の仕様には違反していないが現実的には通常有り得ないTextと思われる.
				 */
				ZnkErr_intern( err, "Warning : Input may not be UTF-8 text.\n");
			}

			/***
			 * UTF16_addUnicodeVal32を呼んでもよいが、追加されるUnicodeValは明らかに16bit以下であるため、
			 * 少しでも効率化するためUTF16_addUnicodeVal16を直接呼ぶ.
			 */
			UTF16_addUnicodeVal16( utf16s, c0 );
			++utf8p;
			--inlen;

		} else if( inlen >= 2 && UTF8_isFirstByte2(c0)
		  && UTF8_isMidByte( utf8p[1] )
		){
			const uint8_t c1 = utf8p[1];

			/***
			 * c0は0xc2以上であるはずである.
			 * 0xc1以下とすると有効bitの示す値は0x7f以下となってしまい
			 * これはUTF-8の1byte系がカバーする範囲と被る.
			 */
			if( st_shortest_flag && (c0 == 0xc0 || c0 == 0xc1) ){
				if( st_replace_invalid ){
					UTF16_addUnicodeVal32( utf16s, st_replace_invalid );
					utf8p += 2;
					inlen -= 2;
					continue;
				} else {
					ZnkErr_internf( err, "Error : Non-shortest UTF-8 sequence (%02x).", c0);
					return false;
				}
			}
			u16 = ((c0 & 0x1f) << 6) | /* 1byte目の下位5bitを取得. */
			      (c1 & 0x3f);         /* 2byte目の下位6bitを取得. */

			/***
			 * UTF16_addUnicodeVal32を呼んでもよいが、追加されるUnicodeValは明らかに16bit以下であるため、
			 * 少しでも効率化するためUTF16_addUnicodeVal16を直接呼ぶ.
			 */
			UTF16_addUnicodeVal16( utf16s, u16 );
			utf8p += 2;
			inlen -= 2;

		} else if( inlen >= 3 && UTF8_isFirstByte3(c0)
		  && UTF8_isMidByte(utf8p[1]) && UTF8_isMidByte(utf8p[2]) )
		{
			const uint8_t c1 = utf8p[1];
			const uint8_t c2 = utf8p[2];

			if( st_shortest_flag && c0 == 0xe0 && c1 < 0xa0 ){
				if( st_replace_invalid ){
					UTF16_addUnicodeVal32( utf16s, st_replace_invalid );
					utf8p += 3;
					inlen -= 3;
					continue;
				} else {
					ZnkErr_internf( err, "Error : non-shortest UTF-8 sequence (%02x)", c0);
					return false;
				}
			}

			u16 = ((c0 & 0xf)  << 12) | /* 1byte目の下位4bitを取得. */
			      ((c1 & 0x3f) <<  6) | /* 2byte目の下位6bitを取得. */
			      ((c2 & 0x3f)      );  /* 3byte目の下位6bitを取得. */

			/* surrogate chars */
			if( u16 >= 0xd800 && u16 <= 0xdfff ){
				if( st_replace_invalid ){
					UTF16_addUnicodeVal32( utf16s, st_replace_invalid );
					utf8p += 3;
					inlen -= 3;
					continue;
				} else {
					ZnkErr_internf( err, "Error : none-UTF-16 char detected (%04x)", u16);
					return false;
				}
			}

			/***
			 * UTF16_addUnicodeVal32を呼んでもよいが、追加されるUnicodeValは明らかに16bit以下であるため、
			 * 少しでも効率化するためUTF16_addUnicodeVal16を直接呼ぶ.
			 */
			UTF16_addUnicodeVal16( utf16s, u16 );
			utf8p += 3;
			inlen -= 3;

		} else if( inlen >= 4 && UTF8_isFirstByte4(c0)
		  && UTF8_isMidByte(utf8p[1]) && UTF8_isMidByte(utf8p[2]) && UTF8_isMidByte(utf8p[3]) )
		{
			const uint8_t c1 = utf8p[1];
			const uint8_t c2 = utf8p[2];
			const uint8_t c3 = utf8p[3];
			uint32_t u32;

			if( st_shortest_flag && c0 == 0xf0 && c1 < 0x90 ){
				if( st_replace_invalid ){
					UTF16_addUnicodeVal32( utf16s, st_replace_invalid );
					utf8p += 4;
					inlen -= 4;
					continue;
				} else {
					ZnkErr_internf( err, "Error : non-shortest UTF-8 sequence (%02x)", c0);
					return false;
				}
			}

			u32 = ((c0 &  0x7) << 18) | /* 下位3bitを 上位から21-19bitの位置へ */
			      ((c1 & 0x3f) << 12) | /* 下位6bitを 上位から18-13bitの位置へ */
			      ((c2 & 0x3f) <<  6) | /* 下位6bitを 上位から12-7bitの位置へ */
			      ((c3 & 0x3f)      );  /* 下位6bitを 上位から6-1bitの位置へ */

			if( !UTF16_addUnicodeVal32( utf16s, u32 ) ){
				if( st_replace_invalid ){
					UTF16_addUnicodeVal32( utf16s, st_replace_invalid );
					utf8p += 4;
					inlen -= 4;
					continue;
				} else {
					ZnkErr_internf( err, "Error : none-UTF-16 char detected (%04x)", u32);
					return false;
				}
			}

			utf8p += 4;
			inlen -= 4;

		} else {
			if( st_replace_invalid ){
				UTF16_addUnicodeVal32( utf16s, st_replace_invalid );
				++utf8p;
				--inlen;
			} else {
				ZnkErr_internf( err, "Error : illegal UTF-8 sequence (%02x)", c0);
				return false;
			}
		}
	}

	return true;
}

/***
 * Convert UTF-16-LE (st. byte order is reverse) to UTF-8
 * 結果はutf8sの最後尾へ追加される.
 */
bool
ZnkMbc_convertUTF16_toUTF8( const uint16_t* utf16s, size_t utf16s_leng, ZnkBif utf8s, bool eliminate_bom, ZnkErr* err )
{
	size_t len = utf16s_leng; /* utf16s_leng はバイトサイズではなく配列の個数 */
	size_t i;
	if( len == 0 ) return true;

	for( i=0; i<len; ++i ) {
		uint16_t u16 = utf16s[i];

		if (eliminate_bom && u16 == 0xfeff) { /* byte order mark */
			continue;
		} else if (u16 < 0x80) { /* ASCII etc */
			ZnkBif_push_bk( utf8s, (uint8_t)(u16) );

		} else if (u16 < 0x0800) { /* 0x0100-0x07FF */
			uint8_t b1 = (uint8_t)(u16 >> 6);   // 上位2bit
			uint8_t b2 = (uint8_t)(u16 & 0x3f); // 下位6bit
			ZnkBif_push_bk( utf8s, (b1 | 0xc0) ); /* 0b110000nn */
			ZnkBif_push_bk( utf8s, (b2 | 0x80) ); /* 0b10nnnnnn */

		} else if (u16 >= 0xdc00 && u16 <= 0xdfff) { /* sole low surrogate(1024) */
			if( st_replace_invalid ){
				UTF8_addUnicodeVal32( utf8s, st_replace_invalid );
				continue;
			} else {
				ZnkErr_intern( err, "Error : invalid surrogate detected\n");
				return false;
			}

		} else if (u16 >= 0xd800 && u16 <= 0xdbff) { /* high surrogate(1024) */
			uint16_t low;
			uint32_t u32;
			uint8_t  b1, b2, b3, b4;

			if( i+1 >= len ) { /* not enough length */
				if( st_replace_invalid ){
					UTF8_addUnicodeVal32(utf8s, st_replace_invalid);
					continue;
				} else {
					ZnkErr_intern( err, "Error : invalid surrogate detected\n");
					return false;
				}
			}

			low = utf16s[i+1];
			if (low < 0xdc00 || low > 0xdfff) { /* not low surrogate */
				if (st_replace_invalid) {
					UTF8_addUnicodeVal32(utf8s, st_replace_invalid);
					continue;
				} else {
					ZnkErr_intern( err, "Error : invalid surrogate detected\n");
					return false;
				}
			}

			/***
			 * u32は最大でも21bit
			 * これから4byteUTF8を組み立てる.
			 */
			u32 = ((u16 & 0x3ff) << 10) | /* 下位10bitを上位10bitへ */
			      (low & 0x3ff);          /* 下位10bitを下位10bitへ */
			u32 += 0x10000;

			b1 = (uint8_t)((u32 >> 18)       ); /* u32 19-21bit(3bit) */
			b2 = (uint8_t)((u32 >> 12) & 0x3f); /* u32 13-18bit(6bit) */
			b3 = (uint8_t)((u32 >>  6) & 0x3f); /* u32 7-12bit(6bit) */
			b4 = (uint8_t)((u32      ) & 0x3f); /* u32 1-6bit(6bit) */
			ZnkBif_push_bk( utf8s, (b1 | 0xf0) ); /* 0b11110nnn */
			ZnkBif_push_bk( utf8s, (b2 | 0x80) ); /* 0b10nnnnnn */
			ZnkBif_push_bk( utf8s, (b3 | 0x80) ); /* 0b10nnnnnn */
			ZnkBif_push_bk( utf8s, (b4 | 0x80) ); /* 0b10nnnnnn */

			++i;

		} else { /* 0x0800-0xFFFF (BMP) */
			/**
			 * 3byteUTF8を組み立てる.
			 */
			uint8_t b1 = (uint8_t)((u16 >> 12)       ); // u16 13-16bit(4bit)
			uint8_t b2 = (uint8_t)((u16 >>  6) & 0x3f); // u16 7-12bit(6bit)
			uint8_t b3 = (uint8_t)((u16      ) & 0x3f); // u16 1-6bit(6bit)
			ZnkBif_push_bk( utf8s, (b1 | 0xe0) ); /* 0b1110nnnn */
			ZnkBif_push_bk( utf8s, (b2 | 0x80) ); /* 0b10nnnnnn */
			ZnkBif_push_bk( utf8s, (b3 | 0x80) ); /* 0b10nnnnnn */
		}
	}
	return true;
}


/*****************************************************************************/


/*
 * Round trip incompatibility 
 *   0xa1c0   -> U+005c -> 0x5c
 *   0x8fa2b7 -> U+007e -> 0x7e
 */

/*
 *  0x0000-0x007f : ASCII
 *  0x00a1-0x00df : JIS X 0201 (通称ANK)
 *    191字
 *    ANK（アンク；Alphabetic Numeric and Kana）は、
 *    ASCIIコードを8ビット（256種類）に拡張し、
 *    独自のカタカナを加えて半角文字を表現している.
 *    アルファベット部分はASCIIそのものだが、JISローマ字とも呼ぶ.
 *    カナ部分をJISカナと呼ぶ.
 *
 *  0x8181-0xfefe : JIS X 0208 (JIS基本漢字)
 *    6,879字（漢字6,355、非漢字524）
 *    ひらがな、かたかな、漢字、全角記号などの2バイト文字.
 *    漢字コード部分は、よく使われる第1水準の漢字と、
 *    あまり使われないが、ないと不便な第2水準の漢字を
 *    16ビット（65,536種類）で表現するように規定したもの.
 *
 *  0x2121-0x7e7e : JIS X 0212 (JIS補助漢字)
 *    6,067字（漢字5,801、非漢字266）
 *    使用頻度の低い文字で構成される第3水準の漢字および非漢字が収録されている.
 *    ほとんど使われることがない.
 */
/**
 * size is 65536
 */
static const uint16_t st_cctbl_ucs2_to_euc[] = {
#include "cc_table/ucs2_to_euc.cctbl"
};
/**
 * size is 9120
 */
static const uint16_t st_cctbl_euc_jisx0212_to_ucs2[] = {
#include "cc_table/euc_jisx0212_to_ucs2.cctbl"
};
/**
 * size is 8192
 *   0x005c, // 0x2140 (REVERSE SOLIDUS)
 *   0xff3c, // 0x2140 (FULLWIDTH REVERSE SOLIDUS)
 */
static const uint16_t st_cctbl_euc_to_ucs2[] = {
#include "cc_table/euc_to_ucs2.cctbl"
};


void
ZnkMbc_convertUTF16_toEUC( const uint16_t* utf16s, size_t utf16s_leng, ZnkBif eucs )
{
	size_t len = utf16s_leng; /* これはバイトサイズではなく配列の個数 */
	size_t i;
	for( i=0; i<len; ++i ){

		uint16_t idx = utf16s[i];
		uint16_t e16 = st_cctbl_ucs2_to_euc[ idx ];
		if( e16 == 0 ){ /* Unknown char */
			/***
			 * 解釈できないUTF16文字が来た場合.
			 * とりあえず '?' 文字を追加しておく.
			 */
			ZnkBif_push_bk( eucs, '?' );

		} else if( e16 < 0x80 ){ /* ASCII */
			ZnkBif_push_bk( eucs, (uint8_t)e16 );

		} else if( e16 > 0xa0 && e16 <= 0xdf ){ /* EUC SS2 (JIS X 0201 kana) */
			ZnkBif_push_bk( eucs, (uint8_t)0x8e );
			ZnkBif_push_bk( eucs, (uint8_t)( e16 & 0xff ) );

		} else if( e16 >= 0x2121 && e16 <= 0x7e7e ){ /* EUC SS3 (JIS X 0212(0x2121-0x6d63) + α) */
			ZnkBif_push_bk( eucs, (uint8_t)0x8f );
			ZnkBif_push_bk( eucs, (uint8_t)( (e16 >> 8)   | 0x80 ) ); /* e16の上位8bitの下位7bit + 0x80 */
			ZnkBif_push_bk( eucs, (uint8_t)( (e16 & 0xff) | 0x80 ) ); /* e16の下位8bitの下位7bit + 0x80 */

		} else if( e16 != 0xffff ){ /* JIS X 0208(基本漢字) */
			ZnkBif_push_bk( eucs, (uint8_t)( e16 >> 8 ) );   /* e16の上位8bit */
			ZnkBif_push_bk( eucs, (uint8_t)( e16 & 0xff ) ); /* e16の下位8bit */

		} else {
		}
	}
	return;
}

bool
ZnkMbc_convertEUC_toUTF16( const char* eucs, size_t eucs_leng, ZnkBif utf16s )
{
	const uint8_t* euc = (const uint8_t*)eucs;
	const size_t len = eucs_leng;
	
	size_t i;
	for( i=0; i<len; ++i ){
		if( euc[i] < 0x80 ){ /* ASCII */
			UTF16_addUnicodeVal16( utf16s, euc[i] );
		} else if( euc[i] == 0x8e ){ /* EUC SS2(JIS X 0201 kana) */
			uint8_t u8 = euc[i+1];
			uint16_t u16;
			if( u8 >= 0xa1 && u8 <= 0xdf ){
				u8 -= 0x40;
			} else {
				u8 = 0;
			}
			u16 = 0xff00 | u8;
			UTF16_addUnicodeVal16( utf16s, u16 );
			++i;

		} else if( euc[i] == 0x8f ){ /* EUC SS3(JIS X 0212) */
			uint8_t  hi  = euc[i+1] & 0x7f;
			uint8_t  low = euc[i+2] & 0x7f;
			size_t   idx = (hi - 0x20) * 96 + (low - 0x20);
			uint16_t u16 = 0;

			if( idx < Znk_NARY( st_cctbl_euc_jisx0212_to_ucs2 ) ){
				u16 = st_cctbl_euc_jisx0212_to_ucs2[ idx ];
			}

			if( u16 == 0 ){
				/***
				 * 未知のEUCコードが来た場合.
				 * とりあえず '?' 文字としておく.
				 */
				u16 = '?';
			}
			UTF16_addUnicodeVal16( utf16s, u16 );
			i += 2;

		} else if( euc[i] < 0xa0 ){  /* C1 */
		} else { /* JIX X 0208 */
			uint8_t  hi  = euc[i  ] &  0x7f;
			uint8_t  low = euc[i+1] &  0x7f;
			size_t   idx = (hi - 0x20) * 96 + (low - 0x20);
			uint16_t u16 = 0;

			if ( idx < Znk_NARY( st_cctbl_euc_to_ucs2 ) ){
				u16 = st_cctbl_euc_to_ucs2[ idx ];
			}

			if (u16 == 0) {
				/***
				 * 未知のEUCコードが来た場合.
				 * とりあえず '?' 文字としておく.
				 */
				u16 = '?';
			}
			UTF16_addUnicodeVal16( utf16s, u16 );
			++i;
		}
	}
	return true;
}



/***
 * UTF8であるutf8sを与え、それをEUCに変換した結果をeucsの最後尾に追加する.
 *
 * utf8s が ZnkBif_data(eucs) と同一のaggregateを示すポインタでも一応動作はする.
 * たとえば 最初にeucs内にはUTF8文字列が格納されていたとして、その末尾より後方へ
 * EUC変換された文字列が連結されるといった処理も一応可能ではある.
 * (その結果に実用上の意味があるか否かは置いといて).
 *
 * wsは与えられた他のいかなる引数とも同一のaggregateを示すものであってはならない.
 */
bool
ZnkMbc_convertUTF8_toEUC( const char* utf8s, size_t utf8s_leng, ZnkBif eucs, ZnkBif ws, ZnkErr* err )
{
	ZnkBif utf16s = ws;
	bool result;
	if( utf8s_leng == 0 ){ return true; }

	ZnkBif_resize( utf16s, 0 ); /* まずクリア */
	ZnkBif_reserve( utf16s, utf8s_leng );

	result = ZnkMbc_convertUTF8_toUTF16( utf8s, utf8s_leng, utf16s, err );
	if( result ){ 
		ZnkMbc_convertUTF16_toEUC( (uint16_t*)ZnkBif_data(utf16s), ZnkBif_size(utf16s)/2, eucs );
	}
	return result;
}
/***
 * UTF8であるselfを与え、それをEUCに変換した結果をselfに上書きする.
 */
bool
ZnkMbc_convertUTF8_toEUC_self( ZnkBif self, ZnkBif ws, ZnkErr* err )
{
	ZnkBif utf16s = ws;
	bool result;
	if( ZnkBif_size(self) == 0 ){ return true; }

	ZnkBif_resize( utf16s, 0 ); /* まずクリア */
	ZnkBif_reserve( utf16s, ZnkBif_size(self) );

	result = ZnkMbc_convertUTF8_toUTF16( (char*)ZnkBif_data(self), ZnkBif_size(self), utf16s, err );
	if( result ){ 
		ZnkBif_resize( self, 0 ); /* ここで一旦クリア */
		ZnkMbc_convertUTF16_toEUC( (uint16_t*)ZnkBif_data(utf16s), ZnkBif_size(utf16s)/2, self );
	}
	return result;
}

/***
 * EUCであるeucsを与え、それをUTF8に変換した結果をutf8sの最後尾に追加する.
 *
 * eucs と utf8s は同一のオブジェクトでもよく、その場合eucsの後ろにUTF8変換された文字列が
 * 連結されることになる. (その結果に実用上の意味があるか否かは置いといて).
 *
 * eucs が ZnkBif_data(utf8s) と同一のaggregateを示すポインタでも一応動作はする.
 * たとえば 最初にutf8s内にはEUC文字列が格納されていたとして、その末尾より後方へ
 * UTF8変換された文字列が連結されるといった処理も一応可能ではある.
 * (その結果に実用上の意味があるか否かは置いといて).
 *
 * wsは与えられた他のいかなる引数とも同一のaggregateを示すものであってはならない.
 */
bool
ZnkMbc_convertEUC_toUTF8( const char* eucs, size_t eucs_leng, ZnkBif utf8s, ZnkBif ws, ZnkErr* err )
{
	ZnkBif utf16s = ws;
	bool result;
	bool eliminate_bom = true;
	if( eucs_leng == 0 ){ return true; }

	ZnkBif_resize( utf16s, 0 ); /* まずクリア */
	ZnkBif_reserve( utf16s, eucs_leng );

	ZnkMbc_convertEUC_toUTF16( eucs, eucs_leng, utf16s );

	result = ZnkMbc_convertUTF16_toUTF8( (uint16_t*)ZnkBif_data(utf16s), ZnkBif_size(utf16s)/2,
			utf8s, eliminate_bom, err );
	return result;
}
/***
 * EUCであるselfを与え、それをUTF8に変換した結果をselfに上書きする.
 */
bool
ZnkMbc_convertEUC_toUTF8_self( ZnkBif self, ZnkBif ws, ZnkErr* err )
{
	ZnkBif utf16s = ws;
	bool result;
	bool eliminate_bom = true;
	if( ZnkBif_size(self) == 0 ){ return true; }

	ZnkBif_resize( utf16s, 0 ); /* まずクリア */
	ZnkBif_reserve( utf16s, ZnkBif_size(self) );

	ZnkMbc_convertEUC_toUTF16( (char*)ZnkBif_data(self), ZnkBif_size(self), utf16s );
	ZnkBif_resize( self, 0 ); /* selfを一旦クリアする. */

	result = ZnkMbc_convertUTF16_toUTF8( (uint16_t*)ZnkBif_data(utf16s), ZnkBif_size(utf16s)/2,
			self, eliminate_bom, err );
	return result;
}


/*****************************************************************************/

/**
 * @brief
 * 与えた uint8_t c の値が、SJISの1バイト目が取り得る値か否かを調べる.
 *
 * @note
 * @code
 * SJISの先頭バイトが取り得る値
 * ( c>=0x81 && c<=0x9f ) || ( c>=0xe0 && c<=0xfc )
 * ただし、実装では高速化のため、これを変形した条件で判定してある。
 * @endcode
 */

/***
 * SJISの先頭バイトが取り得る値
 *
 *   ( c>=0x81 && c<=0x9f ) || ( c>=0xe0 && c<=0xfc )
 *
 * この判定は、以下と同値という公式がある.
 * (uint8_tによるキャストは必須.
 * Cでは、たとえuint8_t同士であっても、引き算をした時点でintへ暗黙変換される)
 *
 *   ( static_cast<uint8_t>( ( c^0x20 ) - 0xa1 ) <= 0x3b )
 *
 *
 * 証明:
 *   上記の基本条件に0x20でXOR演算をすると以下になる.
 *    ( cx>=0xa1 && cx<=0xbf ) ||  ( cx>=0xc0 && cx<=0xdc )
 *   ただし、cx = c^0x20 とする.
 *   (通常、0x20でXOR演算をすると、±0x20 されるが、
 *   上記の範囲内では、(境界±0x20近傍の増減を考慮すれば)不等号が変化しない)
 *
 *    0xbf+1 = 0xc0 なので結局以下になる.
 *    ( cx>=0xa1 && cx<=0xdc )
 *
 *   上記から 0xa1 を引くと、
 *    ( cy>=0x00 && cy<=0x3b )
 *   ただし、cy = c^0x20 - 0xa1 とする.
 *
 *   よって、cy を uint8_t にキャストしてからの条件式で判定可能となる.
 */
Znk_INLINE bool is1stByte_ofSJIS( uint8_t c )
{
	/**
	 * 基本形:
	 * return ( c>=0x81 && c<=0x9f ) || ( c>=0xe0 && c<=0xfc );
	 */
	/* 高速形 */
	return (bool)( (uint8_t)( ( c^0x20 ) - 0xa1 ) <= 0x3b );
}

/***
 * Char Conversion
 */

/***
 * 参考:
 * http://www.tohoho-web.com/wwwkanji.htm
 * http://www.tohoho-web.com/lng/200001/00010045.htm
 */

/***
 * EUC⇔JIS一文字変換
 */
#define M_convertEUC_toJIS( c1, c2 ) (c1) -= 0x80; (c2) -= 0x80
#define M_convertJIS_toEUC( c1, c2 ) (c1) += 0x80; (c2) += 0x80

/***
 * JIS⇔SJIS一文字変換
 */
#define M_convertJIS_toSJIS( c1, c2 ) do{ \
	/* if (c1 % 2) */ \
	if (c1 & 0x01) { \
		/* c1 = ((c1 + 1) / 2) + 0x70; */ \
		c1 = ((c1 + 1) >> 1) + 0x70; \
		c2 += 0x1f; \
	} else { \
		/* c1 = (c1 / 2) + 0x70; */ \
		c1 = (c1 >> 1) + 0x70; \
		c2 += 0x7d; \
	} \
	if (c1 >= 0xa0) { c1 += 0x40; } \
	if (c2 >= 0x7f) { ++c2; } \
}while(0)

#define M_convertSJIS_toJIS( c1, c2 ) do{ \
	if (c1 >= 0xe0) { c1 -= 0x40; } \
	if (c2 >= 0x80) { --c2; } \
 \
	if (c2 >= 0x9e) { \
		/* c1 = (c1 - 0x70) * 2; */ \
		c1 = ( (c1 - 0x70) << 1 ); \
		c2 -= 0x7d; \
	} else { \
		/* c1 = ((c1 - 0x70) * 2) - 1; */ \
		c1 = ( (c1 - 0x70) << 1 ) - 1; \
		c2 -= 0x1f; \
	} \
}while(0)

/***
 * EUC⇔SJIS一文字変換
 * とりあえず一旦JIS変換を介すのが定石らしい.
 */
#define M_convertEUC_toSJIS( c1, c2 ) \
	M_convertEUC_toJIS ( c1, c2 ); M_convertJIS_toSJIS( c1, c2 )

#define M_convertSJIS_toEUC( c1, c2 ) \
	M_convertSJIS_toJIS( c1, c2 ); M_convertJIS_toEUC ( c1, c2 )

/*
 * endof Char Conversion
 ***/

/***
 * EUC⇔SJIS一文字変換
 * とりあえず一旦JIS変換を介すのが定石らしい.
 */
#define M_convertEUC_toSJIS( c1, c2 ) \
	M_convertEUC_toJIS ( c1, c2 ); M_convertJIS_toSJIS( c1, c2 )

#define M_convertSJIS_toEUC( c1, c2 ) \
	M_convertSJIS_toJIS( c1, c2 ); M_convertJIS_toEUC ( c1, c2 )

/***
 * eucs => sjis へとコンバートする.
 * eucs と sjis は同一オブジェクトであってはならない.
 */
Znk_INLINE void
convertStr_EUC_toSJIS( const char* eucs, size_t eucs_leng, ZnkBif sjis )
{
	uint8_t ch1;
	uint8_t ch2;
	size_t idx = 0;
	if( eucs_leng == 0 ){ return; }

	while( idx < eucs_leng ){
		ch1 = eucs[ idx ];

		/***
		 * EUC版半角カタカナは、EUC(SS2)と呼ばれている.
		 * コードは、0x8E 0xA1 ... 0x8E 0xDF までに割り当てられたマルチバイト文字で、
		 * SJIS の１バイト文字である半角カナ(0xA1～0xDF)に相当する.
		 * そのため、この EUC⇒SJIS 変換は、第1バイトを捨て、第2バイトを残すようにすればよい.
		 */
		if( ch1 == 0x8E ){
			if( idx+1 < eucs_leng ){
				ch2 = eucs[ idx+1 ];
				if( ch2 >= 0xA1 && ch2 <= 0xDF ){
					ZnkBif_push_bk( sjis, ch2 );
					idx += 2;
					continue;
				}
			}
		}

		if( ch1 < 0xA1 ){
			/***
			 * マルチバイトではない.
			 * とりあえずASCII とみなす.
			 */
			ZnkBif_push_bk( sjis, ch1 );
			++idx;
			continue;
		}

		/***
		 * ch1 は EUC のマルチバイト文字の第1文字.
		 */
		if( idx+1 >= eucs_leng ){ break; }
		ch2 = eucs[ idx+1 ];
		M_convertEUC_toSJIS( ch1, ch2 );

		ZnkBif_push_bk( sjis, ch1 );
		ZnkBif_push_bk( sjis, ch2 );
		idx += 2;
	}
	return;
}


/***
 * sjis => eucs へとコンバートする.
 * sjis と eucs は同一オブジェクトであってはならない.
 */
Znk_INLINE void
convertStr_SJIS_toEUC( const char* sjis, size_t sjis_leng, ZnkBif eucs )
{
	uint8_t ch1;
	uint8_t ch2;
	size_t idx = 0;
	if( sjis_leng == 0 ){ return; }

	while( idx < sjis_leng ){
		ch1 = sjis[ idx ];

		/***
		 * SJIS 半角カタカナ
		 *
		 * EUC版半角カタカナは、EUC(SS2)と呼ばれている.
		 * コードは、0x8E 0xA1 ... 0x8E 0xDF までに割り当てられ、
		 * SJIS の半角カナ(0xA1...0xDF)に相当する.
		 * そのため、この SJIS⇒EUC 変換は、第1バイトの前に新しく 0x8E を追加すればよい.
		 */
		if( ch1 >= 0xA1 && ch1 <= 0xDF ){
			ZnkBif_push_bk( eucs, (uint8_t)0x8E );
			ZnkBif_push_bk( eucs, ch1 );
			++idx;
			continue;
		}

		/***
		 * SJIS のマルチバイトではない.
		 * ASCII とみなす.
		 */
		if( !is1stByte_ofSJIS(ch1) ){
			ZnkBif_push_bk( eucs, ch1 );
			++idx;
			continue;
		}

		/***
		 * ch1 は SJIS のマルチバイト文字の第1文字.
		 */
		if( idx+1 >= sjis_leng ){ break; }
		ch2 = sjis[ idx+1 ];
		M_convertSJIS_toEUC( ch1, ch2 );

		ZnkBif_push_bk( eucs, ch1 );
		ZnkBif_push_bk( eucs, ch2 );
		idx += 2;
	}
}


typedef enum {
	 JIS_ES_NONE=0
	,JIS_ES_ASCII
	,JIS_ES_0201_ROMAN
	,JIS_ES_0201_KATAKANA
	,JIS_ES_0208_1978
	,JIS_ES_0208_1983
	,JIS_ES_0208_1990
	,JIS_ES_0212_1990
} JIS_ES_Type;

/***
 * @brief
 *   strはJIS文字列とし、idxはEscapeSequenceの開始位置を示すとする.
 *   このとき、その位置が示す文字がマルチバイトか否か(1byte文字か2byte文字か)を
 *   示すフラグ値を is_multibyte として取得し、EscapeSequenceの種類(これは要するに
 *   文字コードの種類を示す)を返す. また idx の値はEscapeSequenceの最後の位置の
 *   次の位置を指すように上書き更新される.
 *
 * 参考: http://www.tohoho-web.com/wwwkanji.htm
 *
 * ESC : 0x1b
 * $   : 0x24
 * &   : 0x26
 * (   : 0x28
 * @   : 0x40
 * B   : 0x42
 * D   : 0x44
 * J   : 0x4a
 *
 * ASCII            : ESC ( B
 * JIS X 0201 Roman : ESC ( J
 * JIS X 0201 半角カタカナ : ESC ( I
 * JIS X 0208 1978  : ESC $ @
 * JIS X 0208 1983  : ESC $ B 
 * JIS X 0208 1990  : ESC & @ ESC $ B
 * JIS X 0212 1990  : ESC $ ( D 
 *
 * 注: JIS X 0201 Roman は ASCIIとほぼ同じだが、
 *     バックスラッシュが 円マークになっているのと 
 *     チルダがオーバーラインになっている点が異なる.
 */
Znk_INLINE JIS_ES_Type
JIS_processEscapeSequence( const char* str, size_t str_leng, size_t* idx, bool* is_multibyte )
{
	const uint8_t ec0 = (uint8_t)(str[ *idx ]);
	const size_t size = str_leng;

	if( ec0 != 0x1b ){ return JIS_ES_NONE; }
	if( (*idx)+2 >= size ){
		return JIS_ES_NONE;
	} else {
		const uint8_t ec1 = str[ (*idx)+1 ];
		const uint8_t ec2 = str[ (*idx)+2 ];
	
		switch( ec1 ){
		case '$' :
			switch( ec2 ){
			case '@':
				/***
				 * 2バイト文字の開始
				 *   JIS X 0208 1978  : ESC $ @
				 */
				(*idx) += 3;
				*is_multibyte = true;
				return JIS_ES_0208_1978;
			case 'B':
				/***
				 * 2バイト文字の開始
				 *   JIS X 0208 1983  : ESC $ B 
				 */
				(*idx) += 3;
				*is_multibyte = true;
				return JIS_ES_0208_1983;
			case '(':
				if( (*idx)+3 < size && str[ (*idx)+3 ] == 'D' ){
					/***
					 * 2バイト文字の開始
					 *   JIS X 0212 1990  : ESC $ ( D 
					 */
					(*idx) += 4;
					*is_multibyte = true;
					return JIS_ES_0212_1990;
				}
			default:
				break;
			}
			break;
	
		case '(' :
			switch( ec2 ){
			case 'B':
				/***
				 * 1バイト文字の開始
				 *   ASCII            : ESC ( B
				 */
				(*idx) += 3;
				*is_multibyte = false;
				return JIS_ES_ASCII;
			case 'J':
				/***
				 * 1バイト文字の開始
				 *   JIS X 0201 Roman : ESC ( J
				 */
				(*idx) += 3;
				*is_multibyte = false;
				return JIS_ES_0201_ROMAN;
			case 'I':
				/***
				 * 1バイト文字の開始
				 *   JIS X 0201 半角カタカナ : ESC ( I
				 */
				(*idx) += 3;
				*is_multibyte = false;
				return JIS_ES_0201_KATAKANA;
			default:
				break;
			}
			break;
		case '&':
			if( (*idx)+5 < size ){
				if ( ec2 == '@'
				  && str[ (*idx)+3 ] == 0x1b
				  && str[ (*idx)+4 ] == '$'
				  && str[ (*idx)+5 ] == 'B' )
				{
					/***
					 * 2バイト文字の開始
					 *   JIS X 0208 1990  : ESC & @ ESC $ B
					 */
					(*idx) += 6;
					*is_multibyte = true;
					return JIS_ES_0208_1990;
				}
			}
			break;
	
		default:
			break;
		}
	}

	return JIS_ES_NONE;
}

#define M_DEFINE_JIS_BEGINER_ASCII     static const uint8_t st_beginer_ascii[]     = { 0x1b, '(', 'B', }      /* ASCIIを採用する */ 
#define M_DEFINE_JIS_BEGINER_0208_1983 static const uint8_t st_beginer_0208_1983[] = { 0x1b, '$', 'B', }      /* JIS X 0208-1983を採用する. */
#define M_DEFINE_JIS_BEGINER_0212_1990 static const uint8_t st_beginer_0212_1990[] = { 0x1b, '$', '(', 'D', } /* JIS X 0212 1990 */
#define M_DEFINE_JIS_BEGINER_KATAKANA  static const uint8_t st_beginer_katakana[]  = { 0x1b, '(', 'I', }      /* JIS X 0201 KATAKANA */

#define M_APPEND_BEGINER( msl, beginer ) \
	ZnkBif_append( (msl), beginer, sizeof(beginer) )

/***
 * sjis => jis へとコンバートする.
 * sjis と jis は同一オブジェクトであってはならない.
 */
Znk_INLINE void
convertStr_SJIS_toJIS( const char* sjis, size_t sjis_leng, ZnkBif jis )
{
	M_DEFINE_JIS_BEGINER_ASCII;
	M_DEFINE_JIS_BEGINER_0208_1983;

	uint8_t ch1;
	uint8_t ch2;
	JIS_ES_Type conv_state = JIS_ES_ASCII;
	size_t idx = 0;

	if( sjis_leng == 0 ){ return; }

	while( idx < sjis_leng ){
		ch1 = sjis[ idx ];

		/***
		 * SJIS 半角カタカナ
		 *
		 * JIS(iso-2022-jp)には半角カタカナがないが、
		 * JIS X 0201 KATAKANA に変換させることにする.
		 */
		if( ch1 >= 0xA1 && ch1 <= 0xDF ){
			if( conv_state != JIS_ES_ASCII ){
				conv_state  = JIS_ES_ASCII;
				M_APPEND_BEGINER( jis, st_beginer_ascii );
			}
			ZnkBif_push_bk( jis, ch1 );
			++idx;
			continue;
		}

		/***
		 * マルチバイトか否かを判定
		 */
		if( is1stByte_ofSJIS( ch1 ) ){
			if( conv_state != JIS_ES_0208_1983 ){
				conv_state  = JIS_ES_0208_1983;
				/***
				 * マルチバイトの開始.
				 */
				M_APPEND_BEGINER( jis, st_beginer_0208_1983 );
			}
		} else {
			if( conv_state != JIS_ES_ASCII ){
				conv_state  = JIS_ES_ASCII;
				/***
				 * ASCIIの開始.
				 */
				M_APPEND_BEGINER( jis, st_beginer_ascii );
			}
		}

		/***
		 * ch1はASCII
		 */
		if( conv_state == JIS_ES_ASCII ){
			ZnkBif_push_bk( jis, ch1 );
			++idx;
			continue;
		}

		/***
		 * ch1,ch2 は SJIS 2byte文字
		 */
		if( idx+1 >= sjis_leng ){ break; }
		ch2 = sjis[ idx+1 ];
		M_convertSJIS_toJIS( ch1, ch2 );

		ZnkBif_push_bk( jis, ch1 );
		ZnkBif_push_bk( jis, ch2 );
		idx += 2;
	}

	/***
	 * ASCII(1byte文字)で完了していない場合は、最後にASCIIの開始を追加.
	 */
	if( ZnkBif_size(jis) > 0 && conv_state != JIS_ES_ASCII ){
		M_APPEND_BEGINER( jis, st_beginer_ascii );
	}
}


/***
 * eucs => jis へとコンバートする.
 * eucs と jis は同一オブジェクトであってはならない.
 */
void Znk_INLINE
convertStr_EUC_toJIS( const char* eucs, size_t eucs_leng, ZnkBif jis )
{
	M_DEFINE_JIS_BEGINER_ASCII;
	M_DEFINE_JIS_BEGINER_0208_1983;
	M_DEFINE_JIS_BEGINER_0212_1990;

	uint8_t ch1;
	uint8_t ch2;
	uint8_t ch3;
	JIS_ES_Type conv_state = JIS_ES_ASCII;
	size_t idx = 0;

	if( eucs_leng == 0 ){ return; }

	while( idx < eucs_leng ){
		ch1 = eucs[ idx ];

		if( ch1 == 0x8e ){
			/***
			 * EUC版半角カタカナ(SS2)の判定.
			 *
			 * JIS(iso-2022-jp)には半角カタカナがないが、
			 * JIS X 0201 KATAKANA に変換させることにする.
			 * 
			 * vim(libiconv)で結果を確認すると、cp932 とみなされるかもしれない.
			 */
			if( idx+1 < eucs_leng ){ 
				ch2 = eucs[ idx+1 ];
				if( ch2 >= 0xA1 && ch2 <= 0xDF ){
#if 0
					if( conv_state != JIS_ES_0201_KATAKANA ){
						conv_state  = JIS_ES_0201_KATAKANA;
						M_APPEND_BEGINER( jis, st_beginer_katakana );
					}
#endif
					if( conv_state != JIS_ES_ASCII ){
						conv_state  = JIS_ES_ASCII;
						M_APPEND_BEGINER( jis, st_beginer_ascii );
					}
					ZnkBif_push_bk( jis, ch2 );
					idx += 2;
					continue;
				}
			}
		}

		/***
		 * EUC 3byte文字(SS3)の判定.
		 *   このとき、JIS X 0212 1990 に変換させることにする.
		 *
		 * vim(libiconv)で結果を確認する場合は、encoding=iso-2022-jp-2 と指定
		 * しなければならないかもしれない.
		 */
		if( ch1 == 0x8f ){
			if( idx+2 < eucs_leng ){ 
				ch2 = eucs[ idx+1 ];
				ch3 = eucs[ idx+2 ];
				if( conv_state != JIS_ES_0212_1990 ){
					conv_state  = JIS_ES_0212_1990;
					M_APPEND_BEGINER( jis, st_beginer_0212_1990 );
				}
				ZnkBif_push_bk( jis, (uint8_t)(ch2 - 0x80) );
				ZnkBif_push_bk( jis, (uint8_t)(ch3 - 0x80) );
				idx += 3;
				continue;
			}
		}

		/***
		 * マルチバイトか否かを判定
		 */
		if( ch1 >= 0xa1 ){
			if( conv_state != JIS_ES_0208_1983 ){
				conv_state  = JIS_ES_0208_1983;
				/***
				 * マルチバイトの開始.
				 */
				M_APPEND_BEGINER( jis, st_beginer_0208_1983 );
			}
		} else {
			if( conv_state != JIS_ES_ASCII ){
				conv_state  = JIS_ES_ASCII;
				/***
				 * ASCIIの開始.
				 */
				M_APPEND_BEGINER( jis, st_beginer_ascii );
			}
		}

		/***
		 * 1byte文字.
		 */
		if( conv_state == JIS_ES_ASCII ){
			ZnkBif_push_bk( jis, ch1 );
			++idx;
			continue;
		}

		/***
		 * ch1, ch2 は EUC 2byte文字.
		 */
		if( idx+1 >= eucs_leng ){ break; }
		ch2 = eucs[ idx+1 ];
		M_convertEUC_toJIS( ch1, ch2 );

		ZnkBif_push_bk( jis, ch1 );
		ZnkBif_push_bk( jis, ch2 );
		idx += 2;

	}

	/***
	 * ASCIIで完了していない場合は、最後にASCIIの開始を追加.
	 */
	if( ZnkBif_size(jis) > 0 && conv_state != JIS_ES_ASCII ){
		M_APPEND_BEGINER( jis, st_beginer_ascii );
	}
}


/***
 * jis => sjis へとコンバートする.
 * jis と sjis は同一オブジェクトであってはならない.
 */
Znk_INLINE void
convertStr_JIS_toSJIS( const char* jis, size_t jis_leng, ZnkBif sjis )
{
	uint8_t ch1;
	uint8_t ch2;
	bool    is_multibyte = false;
	size_t idx = 0;

	if( jis_leng == 0 ){ return; }

	while( idx<jis_leng ){
		JIS_processEscapeSequence( jis, jis_leng, &idx, &is_multibyte );

		if( idx >= jis_leng ){ break; }
		ch1 = jis[ idx ];
		/***
		 * JIS 1byte文字.
		 * 半角カタカナの場合はそのまま(SJISと共通).
		 */
		if( !is_multibyte ){
			ZnkBif_push_bk( sjis, ch1 );
			++idx;
			continue;
		}

		if( idx+1 >= jis_leng ){ break; }
		ch2 = jis[ idx+1 ];
		/***
		 * ch1,ch2 は JIS 2byte文字
		 */
		M_convertJIS_toSJIS( ch1, ch2 );

		ZnkBif_push_bk( sjis, ch1 );
		ZnkBif_push_bk( sjis, ch2 );
		idx += 2;

	}
}


/***
 * jis => eucs へとコンバートする.
 * jis と eucs は同一オブジェクトであってはならない.
 */
Znk_INLINE void
convertStr_JIS_toEUC( const char* jis, size_t jis_leng, ZnkBif eucs )
{
	uint8_t ch1;
	uint8_t ch2;
	bool    is_multibyte = false;
	JIS_ES_Type es_type = JIS_ES_NONE;
	size_t idx = 0;

	if( jis_leng == 0 ){ return; }

	while( idx<jis_leng ){
		es_type = JIS_processEscapeSequence( jis, jis_leng, &idx, &is_multibyte );

		if( idx >= jis_leng ){ break; }
		ch1 = jis[ idx ];
		/***
		 * JIS 1byte文字.
		 * 半角カタカナの場合は 0x8e を追加する.
		 */
		if( !is_multibyte ){
			if( es_type == JIS_ES_0201_KATAKANA ){
				/***
				 * JIS7bit半角カタカナとみなす.
				 */
				ZnkBif_push_bk( eucs, (uint8_t)0x8e );
			} else if( ch1 >= 0xA1 && ch1 <= 0xDF ){
				/***
				 * JIS8bit半角カタカナとみなす.
				 */
				ZnkBif_push_bk( eucs, (uint8_t)0x8e );
			}
			ZnkBif_push_bk( eucs, ch1 );
			++idx;
			continue;
		}

		if( idx+1 >= jis_leng ){ break; }
		ch2 = jis[ idx+1 ];
		/***
		 * ch1,ch2 は JIS 2byte文字
		 */
		M_convertJIS_toEUC( ch1, ch2 );

		ZnkBif_push_bk( eucs, ch1 );
		ZnkBif_push_bk( eucs, ch2 );
		idx += 2;
	}
}


#define M_DIFFER_SELF_CONVERT( conv_func ) \
	ZnkBif_resize( tmp, 0 ); \
	ZnkBif_reserve( tmp, ZnkBif_size(self) ); \
	conv_func( (char*)ZnkBif_data(self), ZnkBif_size(self), tmp ); \
	ZnkBif_swap( self, tmp )

#define M_PREPARE_TMP \
	ZnkBif_resize( tmp, 0 ); \
	ZnkBif_reserve( tmp, ZnkBif_size(self) )

void
ZnkMbc_convert_self( ZnkBif self, ZnkMbcType src_mbc_type, ZnkMbcType dst_mbc_type, ZnkBif tmp, ZnkErr* err )
{
	switch( src_mbc_type ){
	case ZnkMbcType_EUC:
		switch( dst_mbc_type ){
		case ZnkMbcType_EUC:  break;
		case ZnkMbcType_SJIS: M_DIFFER_SELF_CONVERT( convertStr_EUC_toSJIS ); break;
		case ZnkMbcType_JIS:  M_DIFFER_SELF_CONVERT( convertStr_EUC_toJIS  ); break;
		case ZnkMbcType_UTF8: ZnkMbc_convertEUC_toUTF8_self( self, tmp, err ); /* 専用の関数を用いるのが最速 */ break;
		case ZnkMbcType_UTF16:
			ZnkBif_resize( tmp, 0 );
			ZnkBif_reserve( tmp, ZnkBif_size(self) );
			ZnkMbc_convertEUC_toUTF16( (char*)ZnkBif_data(self), ZnkBif_size(self), tmp );
			ZnkBif_swap( tmp, self );
			break;
		default: break;
		}
		break;

	case ZnkMbcType_SJIS:
		switch( dst_mbc_type ){
		case ZnkMbcType_EUC:  M_DIFFER_SELF_CONVERT( convertStr_SJIS_toEUC ); break;
		case ZnkMbcType_SJIS: break;
		case ZnkMbcType_JIS:  M_DIFFER_SELF_CONVERT( convertStr_SJIS_toJIS ); break;
		case ZnkMbcType_UTF8:
#if 0
			/* ひとまず SJIS=>EUC変換したものをtmpへ格納 */
			M_PREPARE_TMP;
			convertStr_SJIS_toEUC( (char*)ZnkBif_data(self), ZnkBif_size(self), tmp );
			ZnkBif_resize( self, 0 ); /* ここで一旦 selfをクリア */
			ZnkMbc_convertEUC_toUTF8( (char*)ZnkBif_data(tmp), ZnkBif_size(tmp), self, tmp2, err );
#else
			ZnkMbc_convert_self( self, ZnkMbcType_SJIS, ZnkMbcType_EUC,   tmp, err );
			ZnkMbc_convertEUC_toUTF8_self( self, tmp, err );
#endif
			break;
		case ZnkMbcType_UTF16:
			ZnkMbc_convert_self( self, ZnkMbcType_SJIS, ZnkMbcType_EUC,   tmp, err );
			ZnkMbc_convert_self( self, ZnkMbcType_EUC,  ZnkMbcType_UTF16, tmp, err );
			break;
		default: break;
		}
		break;

	case ZnkMbcType_JIS:
		switch( dst_mbc_type ){
		case ZnkMbcType_EUC:  M_DIFFER_SELF_CONVERT( convertStr_JIS_toEUC  ); break;
		case ZnkMbcType_SJIS: M_DIFFER_SELF_CONVERT( convertStr_JIS_toSJIS ); break;
		case ZnkMbcType_JIS:  break;
		case ZnkMbcType_UTF8:
#if 0
			/* ひとまず JIS=>EUC変換したものをtmpへ格納 */
			M_PREPARE_TMP;
			convertStr_JIS_toEUC( (char*)ZnkBif_data(self), ZnkBif_size(self), tmp );
			ZnkBif_resize( self, 0 ); /* ここで一旦 selfをクリア */
			ZnkMbc_convertEUC_toUTF8( (char*)ZnkBif_data(tmp), ZnkBif_size(tmp), self, tmp2, err );
#else
			ZnkMbc_convert_self( self, ZnkMbcType_JIS, ZnkMbcType_EUC,   tmp, err );
			ZnkMbc_convertEUC_toUTF8_self( self, tmp, err );
#endif
			break;
		case ZnkMbcType_UTF16:
			ZnkMbc_convert_self( self, ZnkMbcType_JIS,  ZnkMbcType_EUC,   tmp, err );
			ZnkMbc_convert_self( self, ZnkMbcType_EUC,  ZnkMbcType_UTF16, tmp, err );
			break;
		default: break;
		}
		break;

	case ZnkMbcType_UTF8:
		switch( dst_mbc_type ){
		case ZnkMbcType_EUC: ZnkMbc_convertUTF8_toEUC_self( self, tmp, err ); /* 専用の関数を用いるのが最速 */ break;
		case ZnkMbcType_SJIS:
#if 0
			/* ひとまず UTF8=>EUC変換したものをtmpへ格納 */
			M_PREPARE_TMP;
			if( !ZnkMbc_convertUTF8_toEUC( (char*)ZnkBif_data(self), ZnkBif_size(self), tmp, tmp2, err ) ){
				break;
			}
			ZnkBif_resize( self, 0 ); /* ここで一旦 selfをクリア */
			convertStr_EUC_toSJIS( (char*)ZnkBif_data(tmp), ZnkBif_size(tmp), self );
#else
			ZnkMbc_convertUTF8_toEUC_self( self, tmp, err );
			M_DIFFER_SELF_CONVERT( convertStr_EUC_toSJIS );
#endif
			break;
		case ZnkMbcType_JIS:
#if 0
			/* ひとまず UTF8=>EUC変換したものをtmpへ格納 */
			M_PREPARE_TMP;
			if( !ZnkMbc_convertUTF8_toEUC( (char*)ZnkBif_data(self), ZnkBif_size(self), tmp, tmp2, err ) ){
				break;
			}
			ZnkBif_resize( self, 0 ); /* ここで一旦 selfをクリア */
			convertStr_EUC_toJIS( (char*)ZnkBif_data(tmp), ZnkBif_size(tmp), self );
#else
			ZnkMbc_convertUTF8_toEUC_self( self, tmp, err );
			M_DIFFER_SELF_CONVERT( convertStr_EUC_toJIS );
#endif
			break;
		case ZnkMbcType_UTF8: break;
		case ZnkMbcType_UTF16:
			ZnkBif_resize( tmp, 0 );
			ZnkBif_reserve( tmp, ZnkBif_size(self) );
			ZnkMbc_convertUTF8_toUTF16( (char*)ZnkBif_data(self), ZnkBif_size(self), tmp, err );
			ZnkBif_swap( self, tmp );
			break;
		default: break;
		}
		break;

	case ZnkMbcType_UTF16:
		switch( dst_mbc_type ){
		case ZnkMbcType_EUC:
			ZnkBif_resize( tmp, 0 );
			ZnkBif_reserve( tmp, ZnkBif_size(self) );
			ZnkMbc_convertUTF16_toEUC( (uint16_t*)ZnkBif_data(self), ZnkBif_size(self)/2, tmp );
			ZnkBif_swap( self, tmp );
			break;
		case ZnkMbcType_SJIS:
			ZnkMbc_convert_self( self, ZnkMbcType_UTF16, ZnkMbcType_EUC,  tmp, err );
			ZnkMbc_convert_self( self, ZnkMbcType_EUC,   ZnkMbcType_SJIS, tmp, err );
			break;
		case ZnkMbcType_JIS:
			ZnkMbc_convert_self( self, ZnkMbcType_UTF16, ZnkMbcType_EUC, tmp, err );
			ZnkMbc_convert_self( self, ZnkMbcType_EUC,   ZnkMbcType_JIS, tmp, err );
			break;
		case ZnkMbcType_UTF8:
			ZnkBif_resize( tmp, 0 );
			ZnkBif_reserve( tmp, ZnkBif_size(self) );
			ZnkMbc_convertUTF16_toUTF8( (uint16_t*)ZnkBif_data(self), ZnkBif_size(self)/2, tmp, true, err );
			ZnkBif_swap( self, tmp );
			break;
		case ZnkMbcType_UTF16:
			break;
		default: break;
		}
		break;

	default: break;
	}
}


