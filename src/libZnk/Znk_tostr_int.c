#include <Znk_tostr_int.h>
#include <Znk_base.h>
#include <Znk_s_base.h>
#include <Znk_def_util.h>
#include <Znk_stdc.h>
#include <assert.h>

#undef  UINT32_MAX
#define UINT32_MAX 4294967295UL

#undef  INT32_MAX
#define INT32_MAX 2147483647

#undef  INT32_MIN
#define INT32_MIN (-INT32_MAX - 1)

#undef  INT64_MAX
#define INT64_MAX INT64_C(9223372036854775807)

#undef  INT64_MIN
#define INT64_MIN (-INT64_MAX - 1)

/***
 * コンパイラによって64bit整数のprintfモディファイヤは異なるため、
 * マクロによって抽象化してある.
 */
#define M_P_FMTI64 "%" Znk_PFMD_64 "d"
#define M_P_FMTU64 "%" Znk_PFMD_64 "u"
#define M_P_FMTX64 "%" Znk_PFMD_64 "x"
#define M_P_FMTO64 "%" Znk_PFMD_64 "o"

#if defined(Znk_C99_SUPPORTED) || ( defined(__linux__) && defined(Znk_RECENT_GNUC) )
#  define M_P_FMTZU "%zu"
#else
#  if   Znk_CPU_BIT == 32
#    define M_P_FMTZU "%u"
#  elif Znk_CPU_BIT == 64
#    define M_P_FMTZU "%" Znk_PFMD_64 "u"
#  else
#    define M_P_FMTZU "%u"
#  endif
#endif


/***
 * データサイズの基本事項については、DOC_base.n3 を参照.
 *
 * ＠以下では、一貫して *p-- = val, *p++ = val というイディオムを用いている.
 *   (まず*p に代入した後にインクリメントする.)
 */

/***
 * {UINT32_MAXの桁数, INT32_MINの'-'を除いた桁数, INT32_MAXの桁数}
 * というレコードからなる.
 * 2進数から36進数まで
 */
static size_t st_digit_num_table32[][3] = {
	{32, 32, 31}, // 2進数
	{21, 20, 20},
	{16, 16, 16},
	{14, 14, 14},
	{13, 12, 12},
	{12, 12, 12},
	{11, 11, 11}, // 8進数
	{11, 10, 10},
	{10, 10, 10}, // 10進数
	{10, 9,  9},
	{9,  9,  9},
	{9,  9,  9},
	{9,  9,  9},
	{9,  8,  8},
	{8,  8,  8}, // 16進数
	{8,  8,  8},
	{8,  8,  8},
	{8,  8,  8},
	{8,  8,  8},
	{8,  8,  8},
	{8,  7,  7},
	{8,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  7,  7},
	{7,  6,  6},
};
/***
 * {UINT64_MAXの桁数, INT64_MINの'-'を除いた桁数, INT64_MAXの桁数}
 * というレコードからなる.
 * 2進数から36進数まで
 */
static size_t st_digit_num_table64[][3] = {
	{64, 64, 63}, // 2進数
	{41, 40, 40},
	{32, 32, 32},
	{28, 28, 28},
	{25, 25, 25},
	{23, 23, 23},
	{22, 22, 21}, // 8進数
	{21, 20, 20},
	{20, 19, 19}, // 10進数
	{19, 19, 19},
	{18, 18, 18},
	{18, 18, 18},
	{17, 17, 17},
	{17, 17, 17},
	{16, 16, 16}, // 16進数
	{16, 16, 16},
	{16, 16, 16},
	{16, 15, 15},
	{15, 15, 15},
	{15, 15, 15},
	{15, 15, 15},
	{15, 14, 14},
	{14, 14, 14},
	{14, 14, 14},
	{14, 14, 14},
	{14, 14, 14},
	{14, 14, 14},
	{14, 13, 13},
	{14, 13, 13},
	{13, 13, 13},
	{13, 13, 13},
	{13, 13, 13},
	{13, 13, 13},
	{13, 13, 13},
	{13, 13, 13},
};


Znk_INLINE size_t getDigitNum_UMAX32( size_t radix ){
	assert( radix >= 2 && radix <= 36 );
	return st_digit_num_table32[ radix-2 ][0];
}
Znk_INLINE size_t getDigitNum_IMIN32( size_t radix ){
	assert( radix >= 2 && radix <= 36 );
	return st_digit_num_table32[ radix-2 ][1];
}
Znk_INLINE size_t getDigitNum_IMAX32( size_t radix ){
	assert( radix >= 2 && radix <= 36 );
	return st_digit_num_table32[ radix-2 ][2];
}
Znk_INLINE size_t getDigitNum_UMAX64( size_t radix ){
	assert( radix >= 2 && radix <= 36 );
	return st_digit_num_table64[ radix-2 ][0];
}
Znk_INLINE size_t getDigitNum_IMIN64( size_t radix ){
	assert( radix >= 2 && radix <= 36 );
	return st_digit_num_table64[ radix-2 ][1];
}
Znk_INLINE size_t getDigitNum_IMAX64( size_t radix ){
	assert( radix >= 2 && radix <= 36 );
	return st_digit_num_table64[ radix-2 ][2];
}



/***
 * 指数Nを配列インデックスとして与えると 32bit 10^N が得られる.
 * Nは 0以上9以下でなければならない.
 */
static const uint32_t st_pow10_table32[] = {
	1,
	10,
	100,
	1000,
	10000,
	100000,
	1000000,
	10000000,
	100000000,
	1000000000, //32bitの場合、ここまで.
};
/***
 * 指数Nを配列インデックスとして与えると 64bit 10^N が得られる.
 * Nは 0以上19以下でなければならない.
 */
static const uint64_t st_pow10_table64[] = {
	UINT64_C(1),
	UINT64_C(10),
	UINT64_C(100),
	UINT64_C(1000),
	UINT64_C(10000),
	UINT64_C(100000),
	UINT64_C(1000000),
	UINT64_C(10000000),
	UINT64_C(100000000),
	UINT64_C(1000000000), //32bitの場合、ここまで.
	UINT64_C(10000000000),
	UINT64_C(100000000000),
	UINT64_C(1000000000000),
	UINT64_C(10000000000000),
	UINT64_C(100000000000000),
	UINT64_C(1000000000000000),
	UINT64_C(10000000000000000),
	UINT64_C(100000000000000000),
	UINT64_C(1000000000000000000),
	UINT64_C(10000000000000000000), //64bitの場合、ここまで
};


Znk_INLINE uint32_t getPow10U32( size_t exp )
{
	assert( exp < 10 );
	return st_pow10_table32[ exp ];
}
Znk_INLINE uint64_t getPow10U64( size_t exp )
{
	assert( exp < 20 );
	return st_pow10_table64[ exp ];
}




/***
 * @brief 文字列を最後から先頭に並べ変える
 *
 * @param begin
 *   変換文字列の開始位置にあたるバッファ上のアドレス.
 *   並べ替えた文字列がこの位置からバッファ自身に上書きされる.
 *
 * @param last
 *   変換文字列の最終文字がある位置にあたるバッファ上のアドレス.
 */
Znk_INLINE void
reverseStr(char* begin, char* last)
{
	char c;
	while(last > begin) {
		// swap *begin and *last
		c = *begin; *begin = *last; *last = c;

		++begin; --last;
	}
	return;
}



/***
 * ival を 2^idx 倍したものを高速計算.
 */
#define M_P_MLTP_2N( ival, idx ) ((ival)<<idx)
/***
 * ival を 2^idx で割ったものを高速計算.
 */
#define M_P_DIV_2N(  ival, idx ) ((ival)>>idx)


/***
 * 36進数まで表示するための数字テーブル.
 */
static const char st_digits_table_lower[] = "0123456789abcdefghijklmnopqrstuvwxyz";
static const char st_digits_table_upper[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/***
 * tableのidx番目の要素を取り出すための簡易マクロ.
 */
#define M_P_TABLE(table, idx) ((table)[ (size_t)(idx) ])




/*****************************************************************************/
/***
 * @brief
 * N進数表示 汎用バージョン(全型に対応).
 * unsigned 整数をradix進数文字列に汎用的に変換する.
 *
 * @param p 
 *   変換後の数値文字列を格納するバッファの開始アドレス
 *
 * @param uval 
 *   変換元の整数
 *
 * @param radix
 *   何進数表示にするかを示す基数.
 *   2以上36以下でなければならない.
 *
 * @param is_lower
 *   結果を小文字にするか否か( ff or FF 等)
 *
 * @param T
 *   必ず unsigned 型を指定しなければならない.
 *   (uint8_t, uint16_t, uint32_t, uint64_t 等)
 *   符号付整数(int等)が指定された場合、うまく動作しない.
 *
 * @note
 * @code
 *   いわゆるitoaと同等の関数(ただし桁数の機能を強化してある).
 *   atoiと違い、itoa は標準では存在しない.
 *   itoa は VC では stdlib.h に含まれている.
 *   尚、この関数はVCの_itoaと本質は同じ処理をしていると思われるが、
 *   _itoa より少しだけ遅い.
 *
 *   引数 radix に関して補足:
 *   2進数,4進数,8進数,10進数,16進数については、別途高速アルゴリズムを用意した方が
 *   よいと考えられるため、通常は、これら以外の値を指定して使うことが想定される.
 *   digits table のサイズの都合上、最大36までをサポートする.
 *   長いtableさえ用意すれば、理論的には最大値は幾らでも大きく設定できる.
 *   ちなみに標準関数の strtol に与える radix の範囲も同様に 2 以上 36 以下となっている.
 * @endcode
 *
 */
Znk_INLINE char*
UIntToStr_generalFlex_32( char *p, uint32_t uval, bool term_nul, bool is_lower, size_t radix )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	char*       begin = p;
	uint32_t    div;
	/* 値を下位の桁から順に文字にする. */
	do {
		/***
		 * Cygwin gcc ではなぜか % を用いたほうが少し速い.
		 * VC, Linux gcc では % を用いないほうが少し速い.
		 */
		div  = uval / (uint32_t)(radix);
		*p++ = M_P_TABLE(table, uval - div * radix);
		uval = div;
	} while ( uval );

	if( term_nul ){ *p = '\0'; }
	--p;
	
	/* 文字列の上位の桁から始まるように並べ替える。*/
	reverseStr( begin, p );
	++p;
	return p;
}
Znk_INLINE char*
UIntToStr_generalFlex_64( char *p, uint64_t uval, bool term_nul, bool is_lower, size_t radix )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	char*       begin = p;
	uint64_t    div;
	/* 値を下位の桁から順に文字にする. */
	do {
		/***
		 * Cygwin gcc ではなぜか % を用いたほうが少し速い.
		 * VC, Linux gcc では % を用いないほうが少し速い.
		 */
		div  = uval / (uint64_t)(radix);
		*p++ = M_P_TABLE(table, uval - div * radix);
		uval = div;
	} while ( uval );

	if( term_nul ){ *p = '\0'; }
	--p;
	
	/* 文字列の上位の桁から始まるように並べ替える。*/
	reverseStr( begin, p );
	++p;
	return p;
}


Znk_INLINE char*
UIntToStr_generalFix_32( char *p, uint32_t uval, bool term_nul,
		size_t width, char space_ch, bool is_lower, size_t radix )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	char* end;
	p += width;
	end = p; /* '\0'が来るべき場所. */
	--p;

	if( uval == 0 ){
		/* 特別処理が必要. 必ず'0'の一文字は入る. */
		*p-- = '0'; --width;
	} else {
		uint32_t div;
		/* width > 0 かつ uval > 0 である限り、各桁の値の取得＆代入を繰り返す. */
		while( width && uval ){
			div  = uval / (uint32_t)(radix);
			*p-- = M_P_TABLE(table, uval - div * radix);
			uval = div;
			--width;
		}
	}
	/* p より前に 残った width 個だけ space_ch を代入. */
	while( width ){ *p-- = space_ch; --width; }

	if( term_nul ){ *end = '\0'; }
	return end;
}
Znk_INLINE char*
UIntToStr_generalFix_64( char *p, uint64_t uval, bool term_nul,
		size_t width, char space_ch, bool is_lower, size_t radix )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	char* end;
	p += width;
	end = p; /* '\0'が来るべき場所. */
	--p;

	if( uval == 0 ){
		/* 特別処理が必要. 必ず'0'の一文字は入る. */
		*p-- = '0'; --width;
	} else {
		uint64_t div;
		/* width > 0 かつ uval > 0 である限り、各桁の値の取得＆代入を繰り返す. */
		while( width && uval ){
			div  = uval / (uint64_t)(radix);
			*p-- = M_P_TABLE(table, uval - div * radix);
			uval = div;
			--width;
		}
	}
	/* p より前に 残った width 個だけ space_ch を代入. */
	while( width ){ *p-- = space_ch; --width; }

	if( term_nul ){ *end = '\0'; }
	return end;
}

/*
 * endof N進数表示 汎用バージョン
 ***/
/*****************************************************************************/




/*****************************************************************************/
/***
 * 16進数表示高速バージョン
 * (全型に対応)
 *
 * @brief
 *   unsigned 整数を16進数文字列に変換する.
 *   固定幅 or 伸縮の両方をサポートする.
 *
 * @param
 * p: 
 *   バッファの開始位置を示すポインタ.
 *   この開始位置から結果がバッファへ上書きされる.
 *   必須バッファサイズは T の指定型によって異なるが
 *   32bit整数ならば 8+1, 64bit整数ならば 16+1の領域が必要なため、
 *   32 byte 以上が推奨される.
 *
 * hex: 
 *   変換対象の整数
 *
 * width:
 *   固定桁表示における桁数.
 *   例えば uint32_t は 4byteであるため、すべてダンプするには8桁必要となる.
 *
 * is_lower:
 *   結果を小文字にするか否か( ff or FF 等)
 *
 * space_ch:
 *   固定桁表示の時に上位空白に埋めるべき文字.
 *
 * T:
 *   必ず unsigned 型を指定しなければならない.
 *   (uint8_t, uint16_t, uint32_t, uint64_t 等)
 *   仮に符号付整数(int等)が指定された場合、負の数を受け付けることになるが
 *   うまく動作しない.
 *
 * @example:
 *   hex = 0xff, width = 8 のとき、結果は 000000ff
 *   hex = 0xff, width = 0 のとき、結果は ff
 *   hex = 0xff, width = 8, is_lower=false, space_ch=@ のとき、結果は @@@@@@FF
 *
 * @note
 *   UIntToStr_general_* よりもこっちの方が高速.
 *   8bit以上の任意の4*N bit 整数で問題なく処理されるはずである.
 */
Znk_INLINE char*
UIntToStr_asHexFlex_32( char* p, uint32_t hex, bool term_nul, bool is_lower )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	/***
	 * シフトbit量(Bit Shift Size)の初期値 = (移動桁数数の初期値)*4 = (sizeof(uint32_t)*2-1)*4
	 * ここではsizeof(uint32_t)が0になることは絶対にないと仮定するため、bss は size_t としてある.
	 */
	size_t bss = M_P_MLTP_2N( sizeof(uint32_t), 1 ) - 1; /* 16進数での桁数で考えた場合の移動量をまず得る. */
	bss <<= 2; /* bssを4倍し、bitでの移動量を得る. */
	/*
	 * 以下 bss は必ず 4 の倍数である.
	 ***/

	/***
	 * hexの上位桁から順に0でないものがくるまで調べる. ただし、最下位桁については調べない.
	 * 最下位桁は、0 or 非0 にかかわらず、table参照から値を得るためである.
	 * ( bss が 0 になった時点でループを終了するため、最下位桁については調べていない形になる )
	 */
	while( bss && ((hex>>bss) & 0xF) == 0 ){
		/* 4bitずつシフト量を減らす. */
		bss -= 4;
	}

	/* 最下位桁より上で非ゼロの桁をテーブル参照する. */
	while( bss ){
		*p++ = M_P_TABLE( table, (hex>>bss) & 0xF );
		/* 4bitずつシフト量を減らす. */
		bss -= 4;
	}
	/* 最下位桁は必ずテーブル参照する. */
	*p++ = M_P_TABLE( table, hex & 0xF );

	if( term_nul ){ *p = '\0'; }
	return p;
}
Znk_INLINE char*
UIntToStr_asHexFlex_64( char* p, uint64_t hex, bool term_nul, bool is_lower )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	/***
	 * シフトbit量(Bit Shift Size)の初期値 = (移動桁数数の初期値)*4 = (sizeof(uint64_t)*2-1)*4
	 * ここではsizeof(uint64_t)が0になることは絶対にないと仮定するため、bss は size_t としてある.
	 */
	size_t bss = M_P_MLTP_2N( sizeof(uint64_t), 1 ) - 1; /* 16進数での桁数で考えた場合の移動量をまず得る. */
	bss <<= 2; /* bssを4倍し、bitでの移動量を得る. */
	/*
	 * 以下 bss は必ず 4 の倍数である.
	 ***/

	/***
	 * hexの上位桁から順に0でないものがくるまで調べる. ただし、最下位桁については調べない.
	 * 最下位桁は、0 or 非0 にかかわらず、table参照から値を得るためである.
	 * ( bss が 0 になった時点でループを終了するため、最下位桁については調べていない形になる )
	 */
	while( bss && ((hex>>bss) & 0xF) == 0 ){
		/* 4bitずつシフト量を減らす. */
		bss -= 4;
	}

	/* 最下位桁より上で非ゼロの桁をテーブル参照する. */
	while( bss ){
		*p++ = M_P_TABLE( table, (hex>>bss) & 0xF );
		/* 4bitずつシフト量を減らす. */
		bss -= 4;
	}
	/* 最下位桁は必ずテーブル参照する. */
	*p++ = M_P_TABLE( table, hex & 0xF );

	if( term_nul ){ *p = '\0'; }
	return p;
}


Znk_INLINE char*
UIntToStr_asHexFix_32( char* p, uint32_t hex, bool term_nul, size_t width, char space_ch, bool is_lower )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	char* end;
	p += width;
	end = p; /* '\0'が来るべき場所. */

	/* width == 0 を与えた場合は、桁部が一切ない空文字が設定されるものとする.  */
	if( width == 0 ){
		if( term_nul ){ *end = '\0'; }
		return end;
	}

	--p;
	if( hex == 0 ){
		/* hex == 0 の場合は特別処理が必要. */
		*p-- = '0'; --width;
	} else {
		while( width && hex ){
			*p-- = M_P_TABLE( table, hex & 0x0F );
			hex >>= 4;
			--width;
		}
	}
	while( width ){ *p-- = space_ch; --width; }
	if( term_nul ){ *end = '\0'; }
	return end;
}
Znk_INLINE char*
UIntToStr_asHexFix_64( char* p, uint64_t hex, bool term_nul, size_t width, char space_ch, bool is_lower )
{
	const char* table = is_lower ?  st_digits_table_lower : st_digits_table_upper;
	char* end;
	p += width;
	end = p; /* '\0'が来るべき場所. */

	/* width == 0 を与えた場合は、桁部が一切ない空文字が設定されるものとする.  */
	if( width == 0 ){
		if( term_nul ){ *end = '\0'; }
		return end;
	}

	--p;
	if( hex == 0 ){
		/* hex == 0 の場合は特別処理が必要. */
		*p-- = '0'; --width;
	} else {
		while( width && hex ){
			*p-- = M_P_TABLE( table, hex & 0x0F );
			hex >>= 4;
			--width;
		}
	}
	while( width ){ *p-- = space_ch; --width; }
	if( term_nul ){ *end = '\0'; }
	return end;
}


/*
 * endof 16進数表示高速バージョン
 ***/
/*****************************************************************************/




/*****************************************************************************/
/***
 * 10進数表示高速バージョン
 * uint32_t/uint64_t に限る.
 */

/***
 * 2桁までの整数の文字列を100個テーブルとして用意したもの.
 * (高速化のため、処理を2桁ごとに行うにあたり、このようなテーブルが必要になる)
 */
static const char* st_dec_digit2_table[] = {
	"00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
	"50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
	"60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
	"70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
	"80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
	"90", "91", "92", "93", "94", "95", "96", "97", "98", "99",
};

/***
 * uvalが示すテーブル要素の右一桁分の文字を文字列pへ前から追加する.
 */
#define M_P_ADD1( p, uval ) {\
		*(p)++ = st_dec_digit2_table[ uval ][1]; \
}

/***
 * uvalが示すテーブル要素２桁分の文字を文字列pへ前から追加する.
 */
#define M_P_ADD2( p, uval ) {\
		*(p)++ = st_dec_digit2_table[ uval ][0]; \
		*(p)++ = st_dec_digit2_table[ uval ][1]; \
}

/***
 * uval1、uval2が示すテーブル要素４桁分の文字を文字列pへ前から追加する.
 */
#define M_P_ADD4( p, uval1, uval2 ) {\
		M_P_ADD2( p, uval1 ) \
		M_P_ADD2( p, uval2 ) \
}

/***
 * uvalが示すテーブル要素の文字を文字列pへ追加する.
 * ただし、uvalが１桁の整数の場合は、要素の右一桁だけ、
 * uvalが２桁の整数の場合は、要素２桁分だけ追加する.
 */
#define M_P_ADD2_FLEX( p, uval ) \
	switch( uval ){ \
	case 0: case 1: case 2: case 3: case 4: \
	case 5: case 6: case 7: case 8: case 9: \
		M_P_ADD1( (p), (uval) ) \
		break; \
	default: \
		M_P_ADD2( (p), (uval) ) \
		break; \
	} \


/***
 * uvalが示すテーブル要素の右一桁分の文字を文字列pへ後ろから追加する.
 */
#define M_P_ADD1_RV( p, uval ) {\
		*(p)-- = st_dec_digit2_table[ uval ][1]; \
}

/***
 * uvalが示すテーブル要素２桁分の文字を文字列pへ後ろから追加する.
 */
#define M_P_ADD2_RV( p, uval ) {\
		*(p)-- = st_dec_digit2_table[ uval ][1]; \
		*(p)-- = st_dec_digit2_table[ uval ][0]; \
}

/***
 * uval1、uval2が示すテーブル要素４桁分の文字を文字列pへ後ろから追加する.
 */
#define M_P_ADD4_RV( p, uval1, uval2 ) {\
		M_P_ADD2_RV( p, uval1 ) \
		M_P_ADD2_RV( p, uval2 ) \
}



/***
 * Move Upper
 *
 * 行っている処理を明解な擬似コードとして表現すると以下になる.
 * {
 *   upper = uval / base; // 商を取得
 *   uval  = uval % base; // 剰余を取得
 * }
 *
 * base が基底の累乗(= B^N )であるとすれば、
 * これはあたかも uval の第N位より上位の桁が upper へ移動したように見えるため、
 * MOVE というネーミングにしてある. uval には 下N桁のみが残る.
 *
 * 例:
 *   uval = 12345(10進), base = 10^2 = 100(10進)  のとき upper = 123, uval=45
 *   uval = 12345(10進), base = 10^3 = 1000(10進) のとき upper = 12,  uval=345
 *   uval = 0x37BF(16進), base = 0x100(16進) のとき  upper = 0x37,  uval=0xBF
 *   uval = 0x37BF(16進), base = 0x1000(16進) のとき upper = 0x3,   uval=0x7BF
 *
 * ただし、16,8,4,2進数の場合、これよりもっと高速な方法が考えられるため、
 * これは通常は使用しない.
 */
#define M_P_MOVE_UPPER_BASE( upper, uval, base ) {\
		(upper) = (uval) / (base); \
		(uval) -= (upper) * (base); \
}


/***
 * @brief
 * M_P_MOVE_UPPER_BASE を 10進展開で使用するときに、
 * base の替わりに 底10に対する指数idxを指定することができる.
 * このとき、idxはuvalに残る桁数にも等しい.
 *
 * uvalが32bit unsigned int である場合にのみ使用できる.
 *
 * @note:
 * (uval) / (st_pow10_table32[idx]) によって
 * uval の 第 idx+1 位以上の桁を取得することになる.
 */
#define M_P_MOVE_UPPER_DEC32( upper, uval, idx )  M_P_MOVE_UPPER_BASE( (upper), (uval), st_pow10_table32[idx] )


/***
 * uval において 10進桁数が base 桁以上の桁をカットして結果を uvalに上書きする.
 * また、この処理が行われた場合、is_cut フラグに true が設定される.
 * 10進桁数がbase桁未満なら何もしない.
 */
#define M_P_CUT_LARGE_DIGIT( uval, base, is_cut ) if( (uval) >= st_pow10_table32[base] ){\
	(uval) %= st_pow10_table32[base];\
	(is_cut) = true;\
}


/***
 * 4桁の整数を一括取得登録.
 * 4桁よりも大きい桁数のuvalを指定してはならない.
 * 4桁未満の桁数のuvalが指定された場合、上位には'0'が格納される.
 */
Znk_INLINE char* 
UInt32ToStr_asDec_Digit_Fix4( char* p, uint32_t uval )
{
	/***
	 * 処理の高速化のために一時的に格納する0以上99以下の整数.
	 */
	uint32_t upper_2dig = 0;

	/***
	 * 第4,3位を取得登録.
	 */
	M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 2 )
	M_P_ADD2( p, upper_2dig )

	/***
	 * 第2,1位を登録.
	 */
	M_P_ADD2( p, uval )

	return p;
}

/***
 * 6桁の整数を一括取得登録.
 * 6桁よりも大きい桁数のuvalを指定してはならない.
 * 6桁未満の桁数のuvalが指定された場合、上位には'0'が格納される.
 */
Znk_INLINE char*
UInt32ToStr_asDec_Digit_Fix6( char* p, uint32_t uval )
{
	/***
	 * 処理の高速化のために一時的に格納する0以上99以下の整数.
	 */
	uint32_t upper_2dig = 0;

	/***
	 * 第6,5位を取得登録.
	 */
	M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 4 )
	M_P_ADD2( p, upper_2dig )

	/***
	 * 第4,3位を取得登録.
	 */
	M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 2 )
	M_P_ADD2( p, upper_2dig )

	/***
	 * 第2,1位を登録.
	 */
	M_P_ADD2( p, uval )

	return p;
}


/***
 * uval が 4294000000(32bitでのUINT32_MAXがこのあたりにある) 以上である場合、
 * 効率のため特別に処理.
 */
Znk_INLINE char*
processUInt32MaxBound4294( char* p, uint32_t uval )
{
	*p++ = '4'; *p++ = '2'; *p++ = '9'; *p++ = '4';
	/***
	 * 下6桁にする.
	 */
	uval -= 4294000000U;

	if( uval >= 967200U ){
		*p++ = '9'; *p++ = '6'; *p++ = '7'; *p++ = '2';
		/***
		 * 下2桁にする.
		 */
		uval -= 967200U;

		/***
		 * 第2,1位を登録.
		 */
		M_P_ADD2( p, uval )

	} else if( uval >= 960000U ){
		*p++ = '9'; *p++ = '6';
		/***
		 * 下4桁にする.
		 */
		uval -= 960000U;

		/***
		 * 下4桁を一括取得登録.
		 */
		p = UInt32ToStr_asDec_Digit_Fix4( p, uval );

	} else {
		/***
		 * 下6桁の一括取得登録.
		 */
		p = UInt32ToStr_asDec_Digit_Fix6( p, uval );
	}
	return p;
}


/***
 * 9桁以上ある場合(1億以上)の処理.
 */
Znk_INLINE char*
UInt32ToStr_asDec_DigitOver9( char* p, uint32_t uval )
{
	/***
	 * UINT32_MAX = 4294967295 を利用した悪あがき
	 * overflowが起きたとき、このあたり周辺の値は頻繁に参照されるため、
	 * 高速化を図る.
	 */
	if( uval >= 4294000000U ){
		return processUInt32MaxBound4294( p, uval );
	} else {
		uint32_t upper_2dig = 0;
	
		/***
		 * 第10,9位を取得登録.
		 * (第10位は存在しない可能性もある)
		 */
		M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 8 )
		M_P_ADD2_FLEX( p, upper_2dig )
	
		/***
		 * 第8,7位を取得登録.
		 */
		M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 6 )
		M_P_ADD2( p, upper_2dig )
	
		/***
		 * 下6桁の一括取得登録.
		 */
		p = UInt32ToStr_asDec_Digit_Fix6( p, uval );
	}
	return p;
}


/***
 * @brief
 * uval を10進数とみなして文字列に変換する.
 * 固定幅ではなく、桁数に応じて文字列が伸縮取得される.
 *
 * 2桁単位でテーブル参照しているため、いわゆるitoaの実装よりはかなり高速.
 *
 * @param
 * p:
 *   バッファの開始位置を示すポインタ.
 *   この開始位置から結果がバッファへ上書きされる.
 *   バッファサイズは 16 byte 以上を必須とする.
 *
 * uval:
 *   変換対象となる32bit unsigned 整数.
 *
 * term_nul:
 *   最後に'\0'終端するか否か.
 *
 * @return
 *   p はバッファ内を走査するカーソルとして動作し、最終的に'\0'が来るべき
 *   場所に到達する. この最終的な p を戻り値として返す.
 *
 *   次のようにして、結果文字列('\0'文字は含まない)の長さを
 *   呼び出し側から導出することができる.
 *
 *   size_t str_leng = UInt32ToStr_asDecFlex( buf, uval, term_nul ) - buf;
 *
 * @throw
 * @assert
 *
 * @note
 *   実装は32bit整数用に高速化してある.
 *   UIntToStr_general_T よりもこっちの方が高速.
 */
Znk_INLINE char*
UInt32ToStr_asDecFlex( char* p, uint32_t uval, bool term_nul )
{
	/*
	 * 1-2桁(100未満)
	 */
	if( uval < st_pow10_table32[2] ){
		M_P_ADD2_FLEX( p, uval )

		if( term_nul ){ *p='\0'; }
		return p;
	} else {
		/*
		 * 処理の高速化のために一時的に格納する0以上99以下の整数.
		 */
		uint32_t upper_2dig = 0;
	
		/*
		 * 3-4桁(1万未満)
		 */
		if( uval < st_pow10_table32[4] ){
			/*
			 * 第4,3位を取得登録.
			 */
			M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 2 )
			M_P_ADD2_FLEX( p, upper_2dig )
	
			/*
			 * 第2,1位を登録.
			 */
			M_P_ADD2( p, uval )
	
			if( term_nul ){ *p='\0'; }
			return p;
		}
	
		/*
		 * 5-6桁(1万以上100万未満)
		 */
		if( uval < st_pow10_table32[6] ){
			/*
			 * 第6,5位を取得登録.
			 * (第6位は存在しない可能性もある)
			 */
			M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 4 )
			M_P_ADD2_FLEX( p, upper_2dig )
		
			/*
			 * 下4桁を一括取得登録.
			 */
			p = UInt32ToStr_asDec_Digit_Fix4( p, uval );
	
			if( term_nul ){ *p='\0'; }
			return p;
		}
	
		/*
		 * 7-8桁 (100万以上1億未満)
		 */
		if( uval < st_pow10_table32[8] ){
			/*
			 * 第8,7位を取得登録.
			 * (第8位は存在しない可能性もある)
			 */
			M_P_MOVE_UPPER_DEC32( upper_2dig, uval, 6 )
			M_P_ADD2_FLEX( p, upper_2dig )
		
			/*
			 * 下6桁を一括取得登録.
			 */
			p = UInt32ToStr_asDec_Digit_Fix6( p, uval );
	
			if( term_nul ){ *p='\0'; }
			return p;
		}
	
		/*
		 * 1億以上(32bit環境の場合、1～42億9千万程度).
		 */
		p = UInt32ToStr_asDec_DigitOver9( p, uval );
	
		if( term_nul ){ *p='\0'; }
	}
	return p;
}


/***
 * @brief
 * uval を10進数とみなして文字列に変換する.
 * 固定幅を指定し、その幅に満たない場合は space_ch 文字で埋め、
 * その幅を超える場合は上位の桁を切り捨てる.
 *
 * 2桁単位でテーブル参照しているため、いわゆるitoaの実装よりはかなり高速.
 *
 * @param
 * p: 
 *   バッファの開始位置を示すポインタ.
 *   この開始位置から結果がバッファへ上書きされる.
 *   バッファサイズは 16 byte 以上を必須とする.
 *
 * uval: 
 *   変換対象となる32bit unsigned 整数.
 *
 * term_nul:
 *   最後に'\0'終端するか否か.
 *
 * width:
 *   格納される結果文字列の最大幅.
 *   必ず 1 以上 10 以下を指定しなければならない.
 *
 *   得られる数字の桁数がこの幅数に満たない場合は左側に space_ch で指定した
 *   文字が埋められる.
 *   例:
 *     uval = 21, width=4, space_ch='@' の場合、結果は @@21
 *     uval = 0,  width=4, space_ch='@' の場合、結果は @@@0
 *
 *   得られる数字の桁数がこの幅数を越える場合は上位の桁は切り捨てられる. 
 *   例:
 *     uval = 12345, width=4, space_ch='@' の場合、結果は 2345
 *
 *   考慮すべき特殊なケースとして、最上位から0が連続して出現する場合がある.
 *   このとき、space_ch として '0' 以外が指定されていても、必ず
 *   その 0 の連続部分には '0' が埋められる.
 *   (オーバーフローを明確にするため、このような仕様にしてある)
 *   例:
 *     uval = 10000, width=4, space_ch='@' の場合、結果は 0000
 *
 * space_ch:
 *   widthで指定した幅数に満たないとき、最上位より上の空白の部分を
 *   文字 space_ch で埋める( space_ch は大抵の場合 '0' or ' ' と思われる ).
 *   width についても参照のこと.
 *
 * @return
 *   最終的に'\0'が来るべき場所を示すアドレス p + width を戻り値として返す.
 *   (その位置に'\0'が格納されているか否かは term_nul の指定による)
 *
 *   次のようにして、結果文字列('\0'文字は含まない)の長さを
 *   呼び出し側から導出することができる.
 *
 *   size_t str_leng = buf + width;
 *
 * @assert
 *   width で異常値が与えられた場合.
 *
 * @throw
 *
 * @note
 *   実装は32bit整数用に高速化してある.
 *   UIntToStr_general_T よりもこっちの方が高速.
 */
Znk_INLINE char*
UInt32ToStr_asDecFix( char* p, uint32_t uval, bool term_nul, size_t width, char space_ch )
{
	/***
	 * 実装手順の解説.
	 *  uval を下桁から2桁づつ分解して考える.
	 *  以下は width=5の場合である.
	 *
	 *  uval = 00123 =
	 *  	00000 +
	 *  	 0100 +
	 *  	   23
	 *  uval = 01234 =
	 *  	00000 +
	 *  	 1230 +
	 *  	   34
	 *  uval = 12345 =
	 *  	10000 +
	 *  	 2300 +
	 *  	   45
	 *  uval = 123456 => 23456
	 *  	 20000 +
	 *  	  3400 +
	 *  	    56
	 *
	 *  以下は実際のコード例(width=5の場合)である.
	 *
	 *  // width桁を越える場合、まず M_P_CUT_LARGE_DIGIT で余計な上位桁をカット.
	 *  // カットを行ったか否かを is_cut_large_digit フラグに保存しておく.
	 *  // (このフラグは、後にspace_chで埋める処理で必要となる)
	 *  M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )
	 *  
	 *  // uval の 第4位より上の桁からなる新しい整数を dig[0] に移動.
	 *  // (width=5のため、dig[0]は99以下であることが保証される. uvalは下4桁が残る)
	 *  M_P_MOVE_UPPER_DEC32( dig[0], uval, 4 )
	 *
	 *  // uval の 第2位より上の桁からなる新しい整数を dig[1] に移動.
	 *  // (uvalは4桁以下のため、dig[1]は99以下であることが保証される. uvalは下2桁が残る)
	 *  M_P_MOVE_UPPER_DEC32( dig[1], uval, 2 )
	 *  
	 *  // uval, dig[1], dig[0] には0以上99以下の整数が格納されており、
	 *  // これをtableのインデックスに与えて該当文字を得て、p へ
	 *  // 逆方向(配列の開始位置の方向)に格納する.
	 *  M_P_ADD4_RV( p, uval, dig[1] )
	 *  M_P_ADD1_RV( p, dig[0] )
	 */

	bool is_cut_large_digit = false;
	uint32_t dig[4];
	char* end;

	/*
	 * 取得結果はwidthで指定された桁数で固定.
	 */
	p += width;
	end = p; /* '\0'が格納されるべき位置. */
	--p;

	switch( width ){
	case 1:
		/*
		 * is_cut_large_digit を 取得する必要はない.
		 * width=1のとき、space_ch で埋められることはあり得ないため.
		 */
		uval %= 10;
		M_P_ADD1_RV( p, uval )
		break;
	case 2:
	{
		M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )
		M_P_ADD2_RV( p, uval )
		break;
	}
	case 3:
	{
		M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )

		M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )

		M_P_ADD2_RV( p, uval )
		M_P_ADD1_RV( p, dig[0] )
		break;
	}
	case 4:
	{
		M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )

		M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )

		M_P_ADD2_RV( p, uval )
		M_P_ADD2_RV( p, dig[0] )

		break;
	}
	case 5:
		/*
		 * 4桁以下の場合、より無駄のない処理を実行するようにしてある.
		 * 以下現れる同様の分岐は、同様の意図がある.
		 */
		if( uval < st_pow10_table32[4] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )
			M_P_ADD4_RV( p, uval, dig[0] )

			*p-- = '0';

		} else {
			M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )

			M_P_MOVE_UPPER_DEC32( dig[0], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 2 )

			M_P_ADD4_RV( p, uval, dig[1] )
			M_P_ADD1_RV( p, dig[0] )
		}

		break;
	case 6:
		if( uval < st_pow10_table32[4] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )
			M_P_ADD4_RV( p, uval, dig[0] )

			*p-- = '0'; *p-- = '0';

		} else {
			M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )

			M_P_MOVE_UPPER_DEC32( dig[0], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 2 )

			M_P_ADD4_RV( p, uval, dig[1] )
			M_P_ADD2_RV( p, dig[0] )
		}
		break;
	case 7:
		if( uval < st_pow10_table32[4] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )
			M_P_ADD4_RV( p, uval, dig[0] )

			*p-- = '0'; *p-- = '0';
			*p-- = '0';

		} else {
			M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )

			M_P_MOVE_UPPER_DEC32( dig[0], uval, 6 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[2], uval, 2 )

			M_P_ADD4_RV( p, uval, dig[2] )
			M_P_ADD2_RV( p, dig[1] )
			M_P_ADD1_RV( p, dig[0] )
		}
		break;
	case 8:
		if( uval < st_pow10_table32[4] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )
			M_P_ADD4_RV( p, uval, dig[0] )

			*p-- = '0'; *p-- = '0';
			*p-- = '0'; *p-- = '0';

		} else {
			M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )

			M_P_MOVE_UPPER_DEC32( dig[0], uval, 6 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[2], uval, 2 )

			M_P_ADD4_RV( p, uval,   dig[2] )
			M_P_ADD4_RV( p, dig[1], dig[0] )
		}
		break;
	case 9:
		if( uval < st_pow10_table32[4] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )
			M_P_ADD4_RV( p, uval, dig[0] )

			*p-- = '0'; *p-- = '0';
			*p-- = '0'; *p-- = '0';
			*p-- = '0';

		} else if( uval < st_pow10_table32[8] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 6 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[2], uval, 2 )

			M_P_ADD4_RV( p, uval,   dig[2] )
			M_P_ADD4_RV( p, dig[1], dig[0] )

			*p-- = '0';

		} else {
			M_P_CUT_LARGE_DIGIT( uval, width, is_cut_large_digit )

			M_P_MOVE_UPPER_DEC32( dig[0], uval, 8 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 6 )
			M_P_MOVE_UPPER_DEC32( dig[2], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[3], uval, 2 )

			M_P_ADD4_RV( p, uval,   dig[3] )
			M_P_ADD4_RV( p, dig[2], dig[1] )
			M_P_ADD1_RV( p, dig[0] )
		}
		break;
	case 10:
		/*
		 * is_cut_large_digit は必ず false
		 * width=10のとき、桁数がそれに満たない場合は、
		 * 必ず最上位から space_ch で埋められる.
		 */
		if( uval < st_pow10_table32[4] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 2 )
			M_P_ADD4_RV( p, uval, dig[0] )

			*p-- = '0'; *p-- = '0';
			*p-- = '0'; *p-- = '0';
			*p-- = '0'; *p-- = '0';

		} else if( uval < st_pow10_table32[8] ){
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 6 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[2], uval, 2 )

			M_P_ADD4_RV( p, uval,   dig[2] )
			M_P_ADD4_RV( p, dig[1], dig[0] )

			*p-- = '0'; *p-- = '0';

		} else {
			M_P_MOVE_UPPER_DEC32( dig[0], uval, 8 )
			M_P_MOVE_UPPER_DEC32( dig[1], uval, 6 )
			M_P_MOVE_UPPER_DEC32( dig[2], uval, 4 )
			M_P_MOVE_UPPER_DEC32( dig[3], uval, 2 )

			M_P_ADD4_RV( p, uval,   dig[3] )
			M_P_ADD4_RV( p, dig[2], dig[1] )
			M_P_ADD2_RV( p, dig[0] )
		}
		break;
	default:
		{
			assert( 0 && "Invalid width" );
			break;
		}
	}

	/*
	 * 上位桁のカットが行われていない場合に限り、
	 * 最上位から連続する'0'の連続を space_ch に置換する.
	 * この置換は、非'0'の文字が最初に現れるまで最下位の方向へ向かって行われるが、
	 * 最終文字(下1桁に該当する文字)は、置換しない.
	 *
	 * 例:
	 *   space_ch='@', 0010 の場合、@@10 と置換
	 *   space_ch='@', 0000 の場合、@@@0 と置換
	 */
	if( !is_cut_large_digit && space_ch != '0' ){
		char* last = end-1;
		++p;
		while( p < last && *p == '0' ){
			*p++ = space_ch;
		} 
	}

	if( term_nul ){ *end = '\0'; }

	return end;
}


/***
 * 下請け関数として 32版を呼び出している部分がある.
 */
Znk_INLINE char*
UInt64ToStr_asDecFlex( char* p, uint64_t uval, bool term_nul )
{
	const uint64_t billion = st_pow10_table64[9];

	/*
	 * uint64_t の最大値は以下の20桁.
	 * 1844,6744,0737,0955,1615 
	 *
	 * これは9桁づつ分けると
	 * 18,446744073,709551615 
	 *
	 * よってこれらを upper, middle, lower の３パートに分ける.
	 */

	if( uval < billion ){
		return UInt32ToStr_asDecFlex( p, (uint32_t)uval, term_nul );
	} else {
		/*
		 * 以下、変数定義を伴うためあえてelse内へ記述.
		 * この時点で uval は 10桁以上
		 */
	
		/*
		 * 下9桁よりも上位の桁(uint64_tで取得する必要あり)
		 * 非ゼロのはず.
		 */
		uint64_t middle = uval / billion;
		/*
		 * 下9桁を得る.
		 */
		uint32_t lower  = (uint32_t)(uval % billion);
	
		if( middle < billion ){
			p =    UInt32ToStr_asDecFlex( p, (uint32_t)middle, false ); /* null終端しない. */
			return UInt32ToStr_asDecFix ( p, lower,            term_nul, 9, '0' );
		}

		/***
		 * この時点で uval は 19桁以上
		 */
		{
			/*
			 * 最大でも2桁のはず. 非ゼロのはず.
			 */
			uint32_t upper = (uint32_t)(middle / billion);
			/*
			 * 中9桁を得る.
			 */
			middle %= billion;
		
			p = UInt32ToStr_asDecFlex( p, upper,            false ); /* null終端しない. */
			p = UInt32ToStr_asDecFix ( p, (uint32_t)middle, false,    9, '0' ); /* null終端しない. */
			p = UInt32ToStr_asDecFix ( p, lower,            term_nul, 9, '0' );
		}
	}
	return p;
}
/***
 * 下請け関数として 32版を呼び出している部分がある.
 */
Znk_INLINE char*
UInt64ToStr_asDecFix( char* p, uint64_t uval, bool term_nul, size_t width, char space_ch )
{
	const uint64_t billion = st_pow10_table64[9];

	/*
	 * widthが20より大きい場合、NG.
	 */

	static const uint64_t uint32_max = UINT32_MAX;

	/*
	 * uint64_t の最大値は以下の20桁.
	 * 1844,6744,0737,0955,1615 
	 *
	 * これは9桁づつ分けると
	 * 18,446744073,709551615 
	 *
	 * よってこれらを upper, middle, lower の３パートに分ける.
	 */

	if( width <= 9 || uval < billion ){
		/*
		 * 0<width<=9, uval 値は問わず.
		 * or
		 * width>=10, uval は9桁以下.
		 *
		 * 9桁目より上位の余剰スペースはspace_chで埋める.
		 */
		while( width > 9 ){
			*p++ = space_ch;
			--width;
		}
		/*
		 * uint32_maxより大きい場合、単純にキャストすると各桁の数値が変わってしまう.
		 * キャストする前にそれ以下にしなければならない.
		 * 今、最大9桁必要なので、この条件を満たすには billionの剰余をとればよい.
		 * uint32_max以下の場合は、余計な剰余をとらないように注意する.
		 * この処理を行った場合は、オーバーフローを表現するため、space_chを強制的に'0'にする.
		 */
		if( uval > uint32_max ){
			uval %= billion;
			space_ch = '0';
		}
		return UInt32ToStr_asDecFix( p, (uint32_t)uval, term_nul, width, space_ch );
	}

	/***
	 * この時点で uval は 10桁以上 かつ width は10以上.
	 */
	{
		/*
		 * 下9桁よりも上位の桁(uint64_tで取得する必要あり)
		 * 非ゼロのはず.
		 */
		uint64_t middle = uval / billion;
		/*
		 * 下9桁を得る.
		 */
		uint32_t lower  = (uint32_t)(uval % billion);
	
		if( width <= 18 || middle < billion ){
			/*
			 * 10<=width<=18, uval 値は問わず.
			 * or
			 * width>=19, uval は18桁以下.
			 *
			 * 18桁目より上位の余剰スペースはspace_chで埋める.
			 */
			while( width > 18 ){
				*p++ = space_ch;
				--width;
			}
			/*
			 * uint32_maxより大きい場合、単純にキャストすると各桁の数値が変わってしまう.
			 * キャストする前にそれ以下にしなければならない.
			 * 今、最大9桁必要なので、この条件を満たすには billionの剰余をとればよい.
			 * uint32_max以下の場合は、余計な剰余をとらないように注意する.
			 * この処理を行った場合は、オーバーフローを表現するため、space_chを強制的に'0'にする.
			 */
			if( middle > uint32_max ){
				middle %= billion;
				space_ch='0';
			}
			p =    UInt32ToStr_asDecFix( p, (uint32_t)middle, false,    width-9, space_ch ); /* null終端しない. */
			return UInt32ToStr_asDecFix( p, lower,            term_nul, 9,       '0' );
		}
		/***
		 * この時点で uval は 19桁以上 かつ width は19以上.
		 */
		{
			/*
			 * 最大でも2桁のはず.
			 * 非ゼロのはず.
			 */
			uint32_t upper = (uint32_t)(middle / billion);
			/*
			 * 中9桁を得る.
			 */
			middle %= billion;
		
			p = UInt32ToStr_asDecFix( p, upper,            false,    width-18, space_ch ); /* null終端しない. */
			p = UInt32ToStr_asDecFix( p, (uint32_t)middle, false,    9,        '0' );      /* null終端しない. */
			p = UInt32ToStr_asDecFix( p, lower,            term_nul, 9,        '0' );
		}
	}
	return p;
}


/*
 * endof 10進数表示高速バージョン
 ***/
/*****************************************************************************/



/*****************************************************************************/
/***
 * ALL:
 * radix,is_supplement,is_truncate吸収IF
 *
 *
 * is_truncate:
 *   width > 0 のときのみ意味を持つ.
 *   true のとき
 *     uval の桁数がwidthを超える場合、上位桁を打ち切る.
 *   false のとき
 *     uval の桁数がwidthを超える場合、上位桁を適切に拡張して表示する.
 *     printfの動作に等しい.
 *
 *   printfの動作 は is_truncate == false && is_supplement == true と等価.
 *
 *
 * uint*_t 10進数表示の総合インターフェース.
 * 与えられたwidth を基準とした 0埋め、打ち切りのすべての状況に対応する.
 * ただし、pが指し示すbufにはuint*_t用必須サイズが確保されていると仮定する.
 *
 * is_supplement :
 *   true  のとき、uval の桁数が width に満たない場合は space_ch で不足分を埋める.
 *   false のとき、uval の桁数が width に満たない場合でも何も付加しない.
 * is_truncate :
 *   true   のとき、uval の桁数が width を超える場合は上位を打ち切る.
 *   false  のとき、uval の桁数が width を超える場合でも上位をすべて表示する.
 */
Znk_INLINE char*
UIntToStr_ALL_flex_32( char* p, uint32_t uval, bool term_nul, size_t radix, bool is_lower, bool use_general )
{
	if( use_general ){ return UIntToStr_generalFlex_32( p, uval, term_nul, is_lower, radix ); }
	switch( radix ){
	case 10: return UInt32ToStr_asDecFlex(  p, uval, term_nul );
	case 16: return UIntToStr_asHexFlex_32( p, uval, term_nul, is_lower );
	default: break;
	}
	return UIntToStr_generalFlex_32( p, uval, term_nul, is_lower, radix );
}
Znk_INLINE char*
UIntToStr_ALL_flex_64( char* p, uint64_t uval, bool term_nul, size_t radix, bool is_lower, bool use_general )
{
	if( use_general ){ return UIntToStr_generalFlex_64( p, uval, term_nul, is_lower, radix ); }
	switch( radix ){
	case 10: return UInt64ToStr_asDecFlex(  p, uval, term_nul );
	case 16: return UIntToStr_asHexFlex_64( p, uval, term_nul, is_lower );
	default: break;
	}
	return UIntToStr_generalFlex_64( p, uval, term_nul, is_lower, radix );
}

Znk_INLINE char*
UIntToStr_ALL_fix_32( char* p, uint32_t uval, bool term_nul,
		size_t width, char space_ch, size_t radix, bool is_lower, bool use_general )
{
	if( use_general ){ return UIntToStr_generalFix_32( p, uval, term_nul, width, space_ch, is_lower, radix ); }
	switch( radix ){
	case 10: return UInt32ToStr_asDecFix(  p, uval, term_nul, width, space_ch );
	case 16: return UIntToStr_asHexFix_32( p, uval, term_nul, width, space_ch, is_lower );
	default: break;
	}
	return UIntToStr_generalFix_32( p, uval, term_nul, width, space_ch, is_lower, radix );
}
Znk_INLINE char*
UIntToStr_ALL_fix_64( char* p, uint64_t uval, bool term_nul,
		size_t width, char space_ch, size_t radix, bool is_lower, bool use_general )
{
	if( use_general ){ return UIntToStr_generalFix_64( p, uval, term_nul, width, space_ch, is_lower, radix ); }
	switch( radix ){
	case 10: return UInt64ToStr_asDecFix(  p, uval, term_nul, width, space_ch );
	case 16: return UIntToStr_asHexFix_64( p, uval, term_nul, width, space_ch, is_lower );
	default: break;
	}
	return UIntToStr_generalFix_64( p, uval, term_nul, width, space_ch, is_lower, radix );
}

Znk_INLINE uintmax_t
Pow10U( size_t exp )
{
	switch( sizeof(uintmax_t) ){
	case 4: return (uintmax_t)getPow10U32( exp );
	case 8: return (uintmax_t)getPow10U64( exp );
	default: break;
	}
	return (uintmax_t)getPow10U64( exp );
}

/***
 * 自然数radixの自然数exp乗を得る.
 * ただし、radix は 0～100 まで.
 */
static uintmax_t
getPowU( uintmax_t radix, size_t exp )
{
	switch( radix ){
	case 0:
		if( exp == 0 ){
			/***
			 * 数学的には不定形
			 * C99 math.h ではドメインエラーを返すとされるが、
			 * ここでは、二項定理を成立させるため、0^0 = 1 という立場をとる.
			 */
			return 1;
		}
		return 0;
	case  1: return 1;
	case  2: return (uintmax_t)(0x1) << exp;      // exp*1 bit左シフト
	case  4: return (uintmax_t)(0x1) << (exp<<1); // exp*2 bit左シフト
	case  8: return (uintmax_t)(0x1) << (exp*3);  // exp*3 bit左シフト
	case 10: return Pow10U( exp );
	case 16: return (uintmax_t)(0x1) << (exp<<2); // exp*4 bit左シフト
	case 32: return (uintmax_t)(0x1) << (exp*5);  // exp*5 bit左シフト
	default:
		break;
	}

	switch( exp ){
	case 0: return 1;
	case 1: return radix;
	case 2: return radix * radix;
	case 3: return radix * radix * radix;
	case 4: { uintmax_t tmp = radix * radix; return tmp * tmp; }
	case 5: { uintmax_t tmp = radix * radix; return tmp * tmp * radix; }
	case 6: { uintmax_t tmp = radix * radix; return tmp * tmp * tmp; }
	case 7: { uintmax_t tmp = radix * radix; return tmp * tmp * tmp * radix; }
	case 8: { uintmax_t tmp = radix * radix; tmp = tmp * tmp; return tmp * tmp; }
	default: break;
	}

	/***
	 * exp >= 9 以降は２分木的な再帰処理で高速化を図る.
	 * 尚、なるべく再帰の階層を浅くするため、
	 * 奇数ならば9乗分を、偶数ならば8乗分を一旦括り出して２分している.
	 */
	if( exp & 0x1 ){ /* exp は 奇数 */
		exp -= 9;
		exp >>= 1; /* 2 で割る. */
		return getPowU( radix, exp ) * getPowU( radix, exp ) * getPowU( radix, 8 ) * radix;
	}
	exp -= 8;
	exp >>= 1; /* 2 で割る. */
	return getPowU( radix, exp ) * getPowU( radix, exp ) * getPowU( radix, 8 );
}



Znk_INLINE char*
UIntToStr_ALL_32( char *p, uint32_t uval, bool term_nul,
		size_t width, char space_ch,
		size_t radix, bool is_lower,
		bool is_supplement, bool is_truncate,
		bool use_general )
{
	//size_t digit_num_umax = B1::Private::DigitNumMax<T>()( radix );
	const size_t digit_num_umax = getDigitNum_UMAX32( radix );
	assert( width <= digit_num_umax );

	/*
	 * uval == 0 && width == 0 の特殊ケース等があるため、
	 * 先に width == 0 の場合を特別に処理する.
	 */
	if( width == 0 ){
		if( is_truncate ){
			if( term_nul ){ *p = '\0'; }
			return p;
		}
		/*
		 * 全桁表示.
		 */
		return UIntToStr_ALL_flex_32( p, uval, term_nul, radix, is_lower, use_general );
	}


	/*
	 * 速度向上のため、即座に判断できるパターンについては拾っておく.
	 */
	if( !is_supplement && !is_truncate ){
		/*
		 * 全桁表示.
		 */
		return UIntToStr_ALL_flex_32( p, uval, term_nul, radix, is_lower, use_general );
	}
	if( is_supplement && is_truncate ){
		/*
		 * width 桁で打ち切る かつ 不足分をspace_chで埋める.
		 */
		return UIntToStr_ALL_fix_32( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
	}


	/*
	 * getPowU のwidthとして digit_num_umax はサイズオーバーのため使用できない.
	 * width == digit_num_umax のときだけ先に特別に扱う.
	 * このとき「truncate」の必要性はない.
	 */
	if( width == digit_num_umax ){
		if( is_supplement ){
			/*
			 * 全桁表示 かつ 不足分を space_ch で埋める.
			 */
			return UIntToStr_ALL_fix_32( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
		}
		/*
		 * 全桁表示.
		 */
		return UIntToStr_ALL_flex_32( p, uval, term_nul, radix, is_lower, use_general );
	}
	/*
	 * この時点で width < digit_num_umax
	 **/

	/*
	 * uval の桁数が width 以下であるとき
	 * このとき「truncate」の必要性はない.
	 *
	 * ここでの getPowU は正しく動作するが、やはり重い処理になると思われる.
	 * シンプルにtmp bufを用意して一旦Flexで全体を求め、その後にwidthに応じてmemcpyする方が
	 * 高速かもしれないとテストしてみたが、getPowUを使う方が成績はよかった.
	 *  is_truncate==true,is_supplement==false の場合、widthが少ない段階ではgetPowU の方が速い.
	 *  is_truncate==false,is_supplement==true の場合、どちらもあまり変わらない.
	 */
	if( (uintmax_t)uval < getPowU( (uintmax_t)radix, width ) ){
		if( is_supplement ){
			/*
			 * 全桁表示 かつ 不足分を space_ch で埋める.
			 */
			return UIntToStr_ALL_fix_32( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
		}
		/***
		 * 全桁表示.
		 */
		return UIntToStr_ALL_flex_32( p, uval, term_nul, radix, is_lower, use_general );
	}
	/*
	 * この時点で uvalの桁数は width よりも大きい.
	 * これ以降「supplement」の必要性はない.
	 **/

	if( is_truncate ){
		/***
		 * width 桁で打ち切る.
		 */
		return UIntToStr_ALL_fix_32( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
	}
	/***
	 * 全桁表示.
	 */
	return UIntToStr_ALL_flex_32( p, uval, term_nul, radix, is_lower, use_general );

}
/***
 * 基本的に下請け関数の呼び出しが64になっているところ以外は UIntToStr_ALL_32 と同様.
 */
Znk_INLINE char*
UIntToStr_ALL_64( char *p, uint64_t uval, bool term_nul,
		size_t width, char space_ch,
		size_t radix, bool is_lower,
		bool is_supplement, bool is_truncate,
		bool use_general )
{
	const size_t digit_num_umax = getDigitNum_UMAX64( radix );
	assert( width <= digit_num_umax );

	/*
	 * uval == 0 && width == 0 の特殊ケース等があるため、
	 * 先に width == 0 の場合を特別に処理する.
	 */
	if( width == 0 ){
		if( is_truncate ){
			if( term_nul ){ *p = '\0'; }
			return p;
		}
		/*
		 * 全桁表示.
		 */
		return UIntToStr_ALL_flex_64( p, uval, term_nul, radix, is_lower, use_general );
	}


	/*
	 * 速度向上のため、即座に判断できるパターンについては拾っておく.
	 */
	if( !is_supplement && !is_truncate ){
		/*
		 * 全桁表示.
		 */
		return UIntToStr_ALL_flex_64( p, uval, term_nul, radix, is_lower, use_general );
	}
	if( is_supplement && is_truncate ){
		/*
		 * width 桁で打ち切る かつ 不足分をspace_chで埋める.
		 */
		return UIntToStr_ALL_fix_64( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
	}


	/*
	 * getPowU のwidthとして digit_num_umax はサイズオーバーのため使用できない.
	 * width == digit_num_umax のときだけ先に特別に扱う.
	 * このとき「truncate」の必要性はない.
	 */
	if( width == digit_num_umax ){
		if( is_supplement ){
			/*
			 * 全桁表示 かつ 不足分を space_ch で埋める.
			 */
			return UIntToStr_ALL_fix_64( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
		}
		/*
		 * 全桁表示.
		 */
		return UIntToStr_ALL_flex_64( p, uval, term_nul, radix, is_lower, use_general );
	}
	/*
	 * この時点で width < digit_num_umax
	 **/

	/*
	 * uval の桁数が width 以下であるとき
	 * このとき「truncate」の必要性はない.
	 *
	 * ここでの getPowU は正しく動作するが、やはり重い処理になると思われる.
	 * シンプルにtmp bufを用意して一旦Flexで全体を求め、その後にwidthに応じてmemcpyする方が
	 * 高速かもしれないとテストしてみたが、getPowUを使う方が成績はよかった.
	 *  is_truncate==true,is_supplement==false の場合、widthが少ない段階ではgetPowU の方が速い.
	 *  is_truncate==false,is_supplement==true の場合、どちらもあまり変わらない.
	 */
	if( (uintmax_t)uval < getPowU( (uintmax_t)radix, width ) ){
		if( is_supplement ){
			/*
			 * 全桁表示 かつ 不足分を space_ch で埋める.
			 */
			return UIntToStr_ALL_fix_64( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
		}
		/***
		 * 全桁表示.
		 */
		return UIntToStr_ALL_flex_64( p, uval, term_nul, radix, is_lower, use_general );
	}
	/*
	 * この時点で uvalの桁数は width よりも大きい.
	 * これ以降「supplement」の必要性はない.
	 **/

	if( is_truncate ){
		/***
		 * width 桁で打ち切る.
		 */
		return UIntToStr_ALL_fix_64( p, uval, term_nul, width, space_ch, radix, is_lower, use_general );
	}
	/***
	 * 全桁表示.
	 */
	return UIntToStr_ALL_flex_64( p, uval, term_nul, radix, is_lower, use_general );

}

/*
 * endof radix,is_supplement,is_truncate吸収IF
 ***/
/*****************************************************************************/




/*****************************************************************************/
/***
 * SIntToStr_T
 * (digit_num_max 以内のbody部分を取得する.
 * 符号に関するプリプロセスを行った後、UIntToStr_ALL に処理を委譲する)
 */
Znk_INLINE char*
setCompactiveSignCh( char* p, char* begin, char space_ch, char sign_ch )
{
	if( begin[0] != space_ch ){ return p; }
	Znk_memmove( begin, begin+1, p - begin );
	--p;
	return p;
}


Znk_INLINE char*
UIntToStr_withCast_I32( char* p, int32_t ival, bool term_nul,
		size_t width, char space_ch, size_t radix, bool is_lower,
		bool is_supplement, bool is_truncate,
		bool use_general )
{
	return UIntToStr_ALL_32( p, (uint32_t)(ival), term_nul,
			width, space_ch, radix, is_lower,
			is_supplement, is_truncate,
			use_general );
}
Znk_INLINE char*
UIntToStr_withCast_I64( char* p, int64_t ival, bool term_nul,
		size_t width, char space_ch, size_t radix, bool is_lower,
		bool is_supplement, bool is_truncate,
		bool use_general )
{
	return UIntToStr_ALL_64( p, (uint64_t)(ival), term_nul,
			width, space_ch, radix, is_lower,
			is_supplement, is_truncate,
			use_general );
}


/***
 * Note:
 * 下記 A の部分について.
 *
 * p は十分なサイズ(st_uint*_str_buf_size以上)のバッファを指すと仮定.
 *
 * int32_t の場合
 * INT32_MAX  2147483647 : 0111 1111 1111 1111 1111 1111 1111 1111
 * INT32_MIN -2147483648 : 1000 0000 0000 0000 0000 0000 0000 0000
 *
 * 仮に ival に INT32_MIN が指定された場合、-ival とやるとオーバーフローを起こすため、
 * 特別に処理する. ただし、vc,bcc,gcc供に、-INT32_MIN とやるとそれは INT32_MIN に完全に等しい.
 * bitレベルで比較しても -INT32_MIN と INT32_MIN は等しい結果となった.
 * (gccではこのような使用に対して警告が出る).
 *
 * int64_t の場合
 * INT64_MAX  9223372036854775807LL : 0x7fffffffffffffff 
 * INT64_MIN -9223372036854775808LL : 0x8000000000000000 
 * 仮に ival に INT64_MIN が指定された場合、-ival とやるとオーバーフローを起こすため、
 * 特別に処理する. ただし、vc,bcc,gcc供に、-INT64_MIN とやるとそれは INT64_MIN に完全に等しい.
 * bitレベルで比較しても -INT64_MIN と INT64_MIN は等しい結果となった.
 * (gccではこのような使用に対して警告が出る).
 *
 *
 * 下記 B の部分について.
 * 例えば、uint32_t の場合
 * INT32_MIN + INT32_MAX == (すべてのbitが1) == INT32_MAX * 2 + 1
 * ∴ INT32_MIN == INT32_MAX + 1 == static_cast<uint32_t>(INT32_MAX) + 1
 * ∴ INT32_MIN の bit配列は、uint32_tでの INT32_MAX+1 のbit配列に等しく、
 *    これはすなわち、static_cast<uint32_t>(INT32_MIN) == static_cast<uint32_t>(INT32_MAX)+1
 *    が成り立つことを示す.
 * 以下ではこれを利用して、static_cast<uint32_t>(INT32_MIN) を値として渡すことで、
 * INT32_MIN の 符号なしでのradix進数文字列を得ている.
 *
 * uint64_t の場合も同様.
 */
/***
 * width <= digit_num_max でなければならない.
 */
static char*
SIntToStr_I32( char* p, int32_t ival, bool term_nul,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags )
{
	bool is_supplement      = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SUPPLEMENT);
	bool is_truncate        = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_TRUNCATE);
	bool is_lower           = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_LOWER);
	bool use_general        = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_USE_GENERAL);

	const size_t digit_num_max = ( ival < 0 ) ?
		getDigitNum_IMIN32( radix ) :
		getDigitNum_IMAX32( radix );

	if( ival == 0 ){
		bool is_zero_sign_white = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_WHITE);
		bool is_zero_sign_plus  = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_PLUS);
		bool is_zero_sign_minus = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_MINUS);

		if( is_zero_sign_minus ){
			*p++ = '-';
		}else if( is_zero_sign_plus ){
			*p++ = '+';
		}else if( is_zero_sign_white ){
			*p++ = ' ';
		}else{
			/* none */
		}
		return UIntToStr_withCast_I32( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
				radix, is_lower,
				is_supplement, is_truncate,
				use_general );
	}

	/* A */
	if( ival == INT32_MIN ){
		*p++ = '-';
		/* B */
		return UIntToStr_withCast_I32( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
				radix, is_lower,
				is_supplement, is_truncate,
				use_general );

	} else {
		bool is_plus = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_PLUS);
		char sign_ch = '\0';
		bool is_compact;

		if( ival < 0 ){
			ival = -ival;
			sign_ch = '-';
		} else {
			if( is_plus ){ sign_ch = '+'; }
		}
	
		if( sign_ch == '\0' ){
			return UIntToStr_withCast_I32( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
					radix, is_lower,
					is_supplement, is_truncate,
					use_general );
		}
	
		is_compact = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_COMPACT);
		if( is_compact ){
			char* begin;
			*p++ = sign_ch;
			begin = p;
			p = UIntToStr_withCast_I32( p, ival, term_nul, width, space_ch,
					radix, is_lower,
					is_supplement, is_truncate,
					use_general );
			p = setCompactiveSignCh( p, begin, space_ch, sign_ch );
		} else {
			*p++ = sign_ch;
			p = UIntToStr_withCast_I32( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
					radix, is_lower,
					is_supplement, is_truncate,
					use_general );
		}
	}
	return p;
}
/***
 * 基本的にUIntToStr_withCast_I64の呼び出し以外はI32の場合と同様.
 */
static char*
SIntToStr_I64( char* p, int64_t ival, bool term_nul,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags )
{
	bool is_supplement      = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SUPPLEMENT);
	bool is_truncate        = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_TRUNCATE);
	bool is_lower           = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_LOWER);
	bool use_general        = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_USE_GENERAL);

	const size_t digit_num_max = ( ival < 0 ) ?
		getDigitNum_IMIN64( radix ) :
		getDigitNum_IMAX64( radix );

	if( ival == 0 ){
		bool is_zero_sign_white = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_WHITE);
		bool is_zero_sign_plus  = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_PLUS);
		bool is_zero_sign_minus = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_MINUS);

		if( is_zero_sign_minus ){
			*p++ = '-';
		}else if( is_zero_sign_plus ){
			*p++ = '+';
		}else if( is_zero_sign_white ){
			*p++ = ' ';
		}else{
			/* none */
		}
		return UIntToStr_withCast_I64( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
				radix, is_lower,
				is_supplement, is_truncate,
				use_general );
	}

	/* A */
	if( ival == INT64_MIN ){
		*p++ = '-';
		/* B */
		return UIntToStr_withCast_I64( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
				radix, is_lower,
				is_supplement, is_truncate,
				use_general );

	} else {
		bool is_plus = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_PLUS);
		char sign_ch = '\0';
		bool is_compact;

		if( ival < 0 ){
			ival = -ival;
			sign_ch = '-';
		} else {
			if( is_plus ){ sign_ch = '+'; }
		}
	
		if( sign_ch == '\0' ){
			return UIntToStr_withCast_I64( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
					radix, is_lower,
					is_supplement, is_truncate,
					use_general );
		}
	
		is_compact = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_COMPACT);
		if( is_compact ){
			char* begin;
			*p++ = sign_ch;
			begin = p;
			p = UIntToStr_withCast_I64( p, ival, term_nul, width, space_ch,
					radix, is_lower,
					is_supplement, is_truncate,
					use_general );
			p = setCompactiveSignCh( p, begin, space_ch, sign_ch );
		} else {
			*p++ = sign_ch;
			p = UIntToStr_withCast_I64( p, ival, term_nul, Znk_MIN(width,digit_num_max), space_ch,
					radix, is_lower,
					is_supplement, is_truncate,
					use_general );
		}
	}
	return p;
}


/*
 * endof SIntToStr_T
 ***/
/*****************************************************************************/



/*****************************************************************************/
/***
 * UIntToStr_T
 */

/***
 * width <= digit_num_max でなければならない.
 */
Znk_INLINE char*
UIntToStr_32( char* p, uint32_t uval, bool term_nul,
		size_t width, char space_ch, size_t radix, const ZnkToStrFlags flags )
{
	bool is_lower      = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_LOWER);
	bool is_supplement = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SUPPLEMENT);
	bool is_truncate   = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_TRUNCATE);
	bool use_general   = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_USE_GENERAL);
	return UIntToStr_ALL_32( p, uval, term_nul,
			width, space_ch, radix, is_lower,
			is_supplement, is_truncate,
			use_general );
}
Znk_INLINE char*
UIntToStr_64( char* p, uint64_t uval, bool term_nul,
		size_t width, char space_ch, size_t radix, const ZnkToStrFlags flags )
{
	bool is_lower      = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_LOWER);
	bool is_supplement = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SUPPLEMENT);
	bool is_truncate   = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_TRUNCATE);
	bool use_general   = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_USE_GENERAL);
	return UIntToStr_ALL_64( p, uval, term_nul,
			width, space_ch, radix, is_lower,
			is_supplement, is_truncate,
			use_general );
}

/***
 * B1_double_to_str で使用している部分があるため、
 * とりあえず残している
 */
char*
ZnkToStr_Private_UInt32ToStr( char* p, uint32_t uval, bool term_nul,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags )
{
	return UIntToStr_32( p, uval, term_nul,
			width, space_ch, radix, flags );
}

/*
 * endof UIntToStr_T
 ***/
/*****************************************************************************/


/*****************************************************************************/
/***
 * getCStr IF
 */
Znk_INLINE bool
getIsSign( intmax_t ival, ZnkToStrFlags flags )
{
	if( ival < 0 ) { return true; }

	if( ival == 0 ) {
		if ( Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_WHITE) 
		  || Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_PLUS)
		  || Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_MINUS) )
		{
			return true;
		}
		return false;
	}

	if( Znk_IS_FLAG32(flags, ZnkToStr_e_IS_PLUS) ){ return true; }
	return false;
}

Znk_INLINE size_t
getRemainWidth( ZnkToStrFlags* flags, bool is_sign, size_t width, size_t digit_num_max )
{
	const bool is_compact = Znk_IS_FLAG32(*flags, ZnkToStr_e_IS_COMPACT);
	if( !is_sign || !is_compact ){
		return width - digit_num_max;
	}

	/*
	 * ここで一つ減らすため、
	 * 以降は compact処理を行わない.
	 */
	*flags &= ~ZnkToStr_e_IS_COMPACT;
	return width - digit_num_max - 1;
}

Znk_INLINE char*
fillRemainU( char* p, size_t width, char space_ch,
		ZnkToStrFlags flags, size_t digit_num_max )
{
	/*
	 * width が digit_num_max を超える場合、is_supplement が true ならば
	 * 超過分を space_ch で埋める. ( is_supplement が false の場合は何もしないが、
	 * それは結局、width の与え方が、このデータ型とradixに対して大きすぎたということになる )
	 * 以降の処理で width <= digit_num_maxdigit_num_umax となるようにしなければならない.
	 */
	bool is_supplement = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SUPPLEMENT);
	if( is_supplement && width > digit_num_max ){
		size_t remain_width = width - digit_num_max;
		if( remain_width ){
			Znk_memset( p, space_ch, remain_width );
			p += remain_width;
		}
	}
	return p;
}
Znk_INLINE char*
fillRemainI( char* p, bool is_sign,
		size_t width, char space_ch,
		ZnkToStrFlags* flags, size_t digit_num_max )
{
	/*
	 * width が digit_num_max を超える場合、is_supplement が true ならば
	 * 超過分を space_ch で埋める. ( is_supplement が false の場合は何もしないが、
	 * それは結局、width の与え方が、このデータ型とradixに対して大きすぎたということになる )
	 * 以降の処理で width <= digit_num_maxdigit_num_umax となるようにしなければならない.
	 *
	 * is_compact が true のとき、以下のように分けて考える.
	 * 1. width == digit_num_max のとき
	 *    最左がspace_chのときbody取得関数がこれを符号に置き換える.
	 *
	 * 2. width > digit_num_max
	 *    ここで符号分だけ差し引いてspace_chを埋める.
	 *
	 * 3. width < digit_num_max
	 *    最左がspace_chのときbody取得関数がこれを符号に置き換える.
	 */
	bool is_supplement = Znk_IS_FLAG32(*flags, ZnkToStr_e_IS_SUPPLEMENT);
	if( is_supplement && width > digit_num_max ){
		size_t remain_width = getRemainWidth( flags, is_sign, width, digit_num_max );
		if( remain_width ){
			Znk_memset( p, space_ch, remain_width );
			p += remain_width;
		}
	}
	return p;
}

Znk_INLINE size_t
getMustBufSize( size_t width, size_t digit_num_max, size_t buf_size_for_body )
{
	size_t additional_size = 0;
	if( width > digit_num_max ){
		additional_size = width - digit_num_max;
	}
	return buf_size_for_body + additional_size;
}



size_t
ZnkToStr_U32_getStr( char* buf, size_t buf_size, uint32_t uval,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags )
{
	static const bool term_nul = true;
	size_t digit_num_max = getDigitNum_UMAX32( radix );
	char* p = buf;

	if( buf_size <= digit_num_max ){ return 0; }
	if( width >= buf_size ){
		width = buf_size-1;
	}

	/***
	 * 必要ならば、digit_num_max 以降にある上位桁をspace_chで埋める.
	 */
	p = fillRemainU( p, width, space_ch, flags, digit_num_max );
	/***
	 * UIntToStr_T は width <= digit_num_max である必要がある.
	 */
	width = Znk_MIN(width, digit_num_max);
	/***
	 * 取得文字列長を返す.
	 */
	return UIntToStr_32( p, uval, term_nul, width, space_ch, radix, flags ) - buf;
}
size_t
ZnkToStr_U64_getStr( char* buf, size_t buf_size, uint64_t uval,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags )
{
	static const bool term_nul = true;
	size_t digit_num_max = getDigitNum_UMAX64( radix );
	char* p = buf;

	if( buf_size <= digit_num_max ){ return 0; }
	if( width >= buf_size ){
		width = buf_size-1;
	}

	/***
	 * 必要ならば、digit_num_max 以降にある上位桁をspace_chで埋める.
	 */
	p = fillRemainU( p, width, space_ch, flags, digit_num_max );
	/***
	 * UIntToStr_T は width <= digit_num_max である必要がある.
	 */
	width = Znk_MIN(width, digit_num_max);
	/***
	 * 取得文字列長を返す.
	 */
	return UIntToStr_64( p, uval, term_nul, width, space_ch, radix, flags ) - buf;
}


size_t
ZnkToStr_I32_getStr( char* buf, size_t buf_size, int32_t ival,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags )
{
	static const bool term_nul = true;
	size_t digit_num_max = (ival >= 0) ?
		getDigitNum_IMAX32( radix ) :
		getDigitNum_IMIN32( radix );
	char*  p = buf;
	size_t mid_sign_pos;
	size_t str_leng = 0;
	bool   is_sign_top;

	if( buf_size <= digit_num_max ){ return 0; }
	if( width >= buf_size ){
		width = buf_size-1;
	}

	/***
	 * 必要ならば、digit_num_max 以降にある上位桁をspace_chで埋める.
	 */
	p = fillRemainI( p, getIsSign(ival, flags),
			width, space_ch, &flags, digit_num_max );

	/***
	 * SIntToStr_I32で取得される文字列の先頭位置(符号が来る可能性のある位置)を
	 * buf でのインデックスとして保存しておく.
	 */
	mid_sign_pos = p - buf;

	/***
	 * SIntToStr_I32 は width <= digit_num_max である必要がある.
	 */
	width = Znk_MIN(width, digit_num_max);
	str_leng = SIntToStr_I32( p, ival, term_nul, width, space_ch, radix, flags ) - buf;

	/***
	 * is_sign_top の処理:
	 * もし mid_sign_pos > 0 ならば、SIntToStr_I32 で取得した符号部分が
	 * 先頭に来ていないということである.
	 * この場合、bufの最初にあるspace_ch と mid_sign_pos で示される位置に
	 * 格納されているはずの符号文字を入れ替える.
	 */
	is_sign_top = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SIGN_TOP);
	if( is_sign_top && mid_sign_pos ){
		if ( buf[ mid_sign_pos ] == '-' || buf[ mid_sign_pos ] == '+'){
			Znk_SWAP( char, buf[ 0 ], buf[ mid_sign_pos ] );
		}
	}
	return str_leng;
}
size_t
ZnkToStr_I64_getStr( char* buf, size_t buf_size, int64_t ival,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags )
{
	static const bool term_nul = true;
	size_t digit_num_max = (ival >= 0) ?
		getDigitNum_IMAX64( radix ) :
		getDigitNum_IMIN64( radix );
	char*  p = buf;
	size_t mid_sign_pos;
	size_t str_leng = 0;
	bool   is_sign_top;

	if( buf_size <= digit_num_max ){ return 0; }
	if( width >= buf_size ){
		width = buf_size-1;
	}

	/***
	 * 必要ならば、digit_num_max 以降にある上位桁をspace_chで埋める.
	 */
	p = fillRemainI( p, getIsSign(ival, flags),
			width, space_ch, &flags, digit_num_max );

	/***
	 * SIntToStr_I64で取得される文字列の先頭位置(符号が来る可能性のある位置)を
	 * buf でのインデックスとして保存しておく.
	 */
	mid_sign_pos = p - buf;

	/***
	 * SIntToStr_I64 は width <= digit_num_max である必要がある.
	 */
	width = Znk_MIN(width, digit_num_max);
	str_leng = SIntToStr_I64( p, ival, term_nul, width, space_ch, radix, flags ) - buf;

	/***
	 * is_sign_top の処理:
	 * もし mid_sign_pos > 0 ならば、SIntToStr_I64 で取得した符号部分が
	 * 先頭に来ていないということである.
	 * この場合、bufの最初にあるspace_ch と mid_sign_pos で示される位置に
	 * 格納されているはずの符号文字を入れ替える.
	 */
	is_sign_top = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SIGN_TOP);
	if( is_sign_top && mid_sign_pos ){
		if ( buf[ mid_sign_pos ] == '-' || buf[ mid_sign_pos ] == '+'){
			Znk_SWAP( char, buf[ 0 ], buf[ mid_sign_pos ] );
		}
	}
	return str_leng;
}


/*
 * endof getCStr IF
 ***/
/*****************************************************************************/

const char* 
ZnkToStr_Bool_getStr( bool bool_val, char type )
{
#define M_BOOL_STR( T, F ) bool_val ? T : F
	switch( type ){
	case 'T' : return M_BOOL_STR( "T", "F" );
	case 't' : return M_BOOL_STR( "t", "f" );
	case '1' : return M_BOOL_STR( "1", "0" );
	case 'A' : return M_BOOL_STR( "TRUE", "FALSE" );
	case 'L' : return M_BOOL_STR( "True", "False" );
	case 'a' :
	default: break;
	}
	return M_BOOL_STR( "true", "false" );
}

