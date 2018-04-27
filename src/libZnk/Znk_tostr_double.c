#include "Znk_tostr_double.h"
#include <Znk_def_util.h>
#include <string.h>


#if defined(__BORLANDC__)
#  pragma warn -8071
#endif

enum {
	 Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_F = 512
	,Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_G = 512
	,Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_E = 32
	,Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_X = 32
	,Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE   = 512
};


/***
 * Private global function : Prototype declare.
 */
char*
ZnkToStr_Private_UInt32ToStr( char* p, uint32_t uval, bool term_nul,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags );


/*****************************************************************************/
/***
 * DoubleInfoBase
 */

/**
 * NanCallBackFunc 
 *   dval が NaN, Inf, -Inf であった場合にどのような文字列を設定するかを記述した
 *   コールバック関数の形式を以下のように定義する.
 *
 *   typedef char* (*NanCallBackFunc)( int, char*, bool );
 *
 *   第1引数 int type 
 *     dval が上記３種のうちのいずれであるかを判別するための type 値が
 *     設定されている. type の値が正のとき dval は Inf、type の値が負のとき
 *     dval は -Inf、type の値が 0 のとき dval は Nan であることを示す.
 *
 *   第2引数 char* p
 *     文字列を格納するバッファへのポインタ.
 *     この関数で与えられたものと等しい.
 *
 *   第3引数 char* p
 *     文字列を'\0'終端するするか否かを示す.
 *     この関数で与えられたものと等しい.
 *
 * NanCallBackFunc 型の引数をとる関数には、以下の２つの引数をとるものがある.
 *
 * nan_callback_func
 *   NanCallBackFunc 型コールバック関数へのポインタを与える.
 *   nan_callback_func に NULL を与えた場合、
 *   デフォルトの設定関数(setDefaultNanCStr)が呼び出される.
 *   setDefaultNanCStr関数は、それぞれ nan, inf, -inf という文字列を設定する.
 *
 * is_check_nan
 *   dval の Nan, Inf, -Inf チェックを行うか否かを示す.
 *   true のとき
 *     このチェックを行い、nan_callback_func の指定により、
 *     適切な文字列の設定が行われる.
 *   false のとき
 *     dval が Nan, Inf, -Inf ではないということが予めわかっており、
 *     チェックが不要の場合に限り、false を指定することができる.
 *     また、このとき、nan_callback_func がコールされることはない.
 */
typedef char* (*NanCallBackFunc)( int, char*, bool );

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
 * IEEE754 double型において
 * 2のidx乗値の指数部に相当するbit列は以下のようになる.
 * (最上位bitは厳密には符号部(負ならば1が立ち、正ならば0になる)であり、
 * 指数部ではない)
 *
 * (-1023)
 * 0x000, 0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007, 0x008, 0x009, 0x00a, ...
 * ...
 * ... 0x3f8, 0x3f9, 0x3fa, 0x3fb, 0x3fc, 0x3fd, 0x3fe, 0x3ff, //3fe 2^(-1), 3ff 2^0
 * //2^1  2^2    2^3
 * 0x400, 0x401, 0x402, 0x403, 0x404, 0x405, 0x406, 0x407, 0x408, 0x409, 0x40a, ...
 * ...
 * ...  0x7f8, 0x7f9, 0x7fa, 0x7fb, 0x7fc, 0x7fd, 0x7fe, 0x7ff,//(1024)
 *
 * idx の値からこれらを求めるには、0x3ff(=1023)を加算し、52ビット左シフトすればよい.
 * idx は -1023以上1024以下でなければならない.
 */
Znk_INLINE uint64_t getExp2Bits( int idx )
{
	return (uint64_t)(idx + 0x3ff) << 52;
}


#if 0
/**
 * 2のidx乗をdouble値で高速取得する.
 *
 * Inline_getExp2(0,-1023) は0( 指数部、仮数部ともにすべて0 ).
 * また log10( Inline_getExp2(0,1024) ) は 負の∞
 *
 * Inline_getExp2(N,-1023) (N>0) はすべて正常の正値(極めて0に近い).
 * また log10( Inline_getExp2(N,1024) ) は すべて正常の負値.
 *
 * Inline_getExp2(0,1024) は正の∞.
 * また log10( Inline_getExp2(0,1024) ) も 正の∞
 *
 * Inline_getExp2(N,1024) (N>0) はすべてNaN.
 * また log10( Inline_getExp2(N,1024) ) も すべてNaN.
 *
 * Inline_getExp2(N,1023) (Nは任意) はすべて正常の正値.
 * また、log10( Inline_getExp2(N,1023) ) も すべて正常の正値.
 */
Znk_INLINE double Inline_getExp2( int idx ) // make* という名前の方が適切か...
{
	uint64_t t = getExp2Bits(idx);
	return (double)( *(double*)( &t ) );
}
#endif


/**
 * 「fraction * 2のidx乗」 をdouble値で高速取得する.
 */
Znk_INLINE double Inline_getFracBitExp2( uint64_t fraction_bits, int idx )
{
	uint64_t bits = getExp2Bits(idx) | fraction_bits;
	return *(double*)(&bits);
}
Znk_INLINE double Inline_getFracExp2( double fraction, int idx )
{
    uint64_t fraction_bits = *(uint64_t*)&fraction;
    fraction_bits &= UINT64_C(0x000fffffffffffff);
	return Inline_getFracBitExp2( fraction_bits, idx );
}



/**
 * Infに関してVC/GCCともに以下がいえる.
 *
 * Inf==Inf( x, y が ともに  Inf であるならば x == y が常に成り立つ. )
 * -Inf==-Inf( x, y が ともに -Inf であるならば x == y が常に成り立つ. )
 * 1.0/0.0 == std::numeric_limits<double>::infinity().
 * Inf+1 == Inf
 * Inf-1 == Inf
 * Inf*2 == Inf
 * Inf/2 == Inf
 * 常に 1 < Inf
 * 1 > Inf  は不成立.
 * 1 == Inf は不成立.
 * Inf > -Inf
 * Inf > 0
 * -Inf < 0
 * Inf > DBL_MAX
 * -Inf < DBL_MIN
 */ 


/**
 * +Inf を意図的に得る.
 */
static double
getPositiveInf( void )
{
	static double inf = 0.0;
	if (inf == 0.0) {
		/**
		 * 最初の一回だけここが実行される.
		 * コンパイラの警告を防ぐため、非インライン関数から 0.0 を取得している.
		 * 本来ならば numeric_limits から取得するのが一番よいのだろうが、
		 * Boostによれば、STL limits の実装が不完全なコンパイラ環境が多く存在するとのこと.
		 * これは実際に確認してみなければなんともいえないが、
		 * とりあえず、ここではInline_getFracExp2 を用いる方法(bitレベルで設定)を使う.
		 */
		inf = Inline_getFracExp2( 1.0, 1024 );
	}
	return inf;
}

/**
 * -Inf を意図的に得る.
 */
static double
getNegativeInf( void )
{
	static double inf = 0.0;
	if (inf == 0.0) {
		/**
		 * 最初の一回だけここが実行される.
		 * コンパイラの警告を防ぐため、非インライン関数から 0.0 を取得している.
		 */
		inf = -getPositiveInf();
	}
	return inf;
}

Znk_INLINE bool
Inline_isPositiveInf( double dval )
{
	return (bool)( dval == getPositiveInf() );
}


Znk_INLINE bool
Inline_isNegativeInf( double dval )
{
	return (bool)( dval == getNegativeInf() );
}


/**
 * 正or負の無限大ならば true.
 * さもなければ false
 *
 * 概念的には以下の判定とも等しい.
 * (!Double_isFinite(n) && !Double_isNaN(n));
 */
Znk_INLINE bool Inline_isInf( double dval ) {
	return (bool)( Inline_isPositiveInf( dval ) || Inline_isNegativeInf( dval ) );
}


/**
 * NaN を意図的に得る.
 * 特に fraction == *.0(小数部分を0) と指定した場合、正のInfが得られる.
 */
Znk_INLINE double Inline_getNaN( double fraction ){
	return Inline_getFracExp2( fraction, 1024 );
}

/**
 * コンパイラによる最適化を抑止するために使用する非インライン関数群
 */
static bool double_ne( double d1, double d2 ) { return (bool)(d1 != d2); }

/**
 * NaN ならば true.
 * さもなければ false
 * VCでは _isnanと同じ.
 *
 * VC/GCCともに
 * x が NaN であるならば x != x が常に成り立つ.
 *
 * 実装上の注意:
 * コンパイラに最適化させないため、
 * 敢えて非インライン関数で比較させる.
 */
Znk_INLINE bool Inline_isNaN( double dval ) { return double_ne(dval, dval); }

/**
 * 有限数(NaNでもInfでもない)ならば true.
 * さもなければ false
 * VCでは _finiteと同じ.
 */
Znk_INLINE bool Inline_isFinite( double n ) { return (bool)( !Inline_isNaN(n) && !Inline_isInf(n) ); }


double ZnkDouble_getPositiveInf( void )      { return getPositiveInf(); }
double ZnkDouble_getNegativeInf( void )      { return getNegativeInf(); }

bool   ZnkDouble_isPositiveInf( double dval ){ return Inline_isPositiveInf( dval ); }
bool   ZnkDouble_isNegativeInf( double dval ){ return Inline_isPositiveInf( dval ); }
bool   ZnkDouble_isInf   ( double dval )     { return Inline_isInf   ( dval ); }

double ZnkDouble_getNaN  ( double fraction ) { return Inline_getNaN  ( fraction ); }
bool   ZnkDouble_isNaN   ( double dval )     { return Inline_isNaN   ( dval ); }
bool   ZnkDouble_isFinite( double dval )     { return Inline_isFinite( dval ); }

/*
 * endof DoubleInfoBase
 ***/
/*****************************************************************************/




Znk_INLINE char*
writeBytes( char* p, const void* src_bytes, size_t copy_size, bool term_nul )
{
	memcpy( p, src_bytes, copy_size );
	p += copy_size;
	if( term_nul ){ *p = '\0'; }
	return p;
}


/**
 * @brief
 *   Inline_decompToBase10 で取得される形式の fraction10 を与え、
 *   pで示されるbuf位置に fraction10 が示す符号なし仮数の文字列を作成する.
 *
 * @param
 * p: 
 *   バッファの開始位置を示すポインタ.
 *   この開始位置から結果がバッファへ上書きされる.
 *   必須バッファサイズは 32.
 *   (仮数の実質的な最大精度は15桁であるため、32byteあれば十分と考えられる)
 *
 * term_nul:
 *   最後に'\0'終端するか否か.
 *
 * fraction10:
 *   Inline_decompToBase10 で取得される形式の fraction10
 *
 * term_nul:
 *   最後に'\0'終端するか否か.
 *
 * decimal_width:
 *   仮数部の小数点以下の桁数を指定.
 *   ただし16以上を指定しても実質的な意味はない.
 *
 * @return
 *   作成された文字列の終端位置('\0'が格納されるべき位置)を返す.
 */
Znk_INLINE char*
Fraction10ToStr_asDecFix( char* p, double fraction10, bool term_nul, size_t decimal_width )
{
	/***
	 * uint64_t i = static_cast<uint64_t>(decimal);
	 * は、decimal > UINT32_MAX のとき、期待通りの変換にならない.
	 * いずれにせよ 10進数の場合、doubleの精度は15桁くらいしかないため、
	 * 9byteづつ取得しても問題ない.
	 */
	size_t radix = 10;

	uint32_t i = (uint32_t)fraction10;
	double decimal = fraction10 - i;
	size_t width = decimal_width;

	p = ZnkToStr_Private_UInt32ToStr( p, i,
			false, 0, '0', radix, 0 );
	*p++ = '.';

	if( decimal_width > 9 ){
		width = 9;
		decimal_width -= 9;
	}
	do {
		decimal *= st_pow10_table32[ width ];
		i = (uint32_t)decimal;
		p = ZnkToStr_Private_UInt32ToStr( p, i,
				false, width, '0', radix, ZnkToStr_e_IS_SUPPLEMENT | ZnkToStr_e_IS_TRUNCATE );
		decimal = decimal - i;

		if( decimal_width >= width ){
			decimal_width -= width;
		} else {
			break;
		}
	} while( decimal_width );

	if( decimal_width > 0 ){
		decimal *= st_pow10_table32[ decimal_width ];
		i = (uint32_t)(decimal);
		p = ZnkToStr_Private_UInt32ToStr( p, i,
				false, decimal_width, '0', radix, ZnkToStr_e_IS_SUPPLEMENT | ZnkToStr_e_IS_TRUNCATE );
	}
	if( term_nul ){ *p = '\0'; }
	return p;
}



/*****************************************************************************/
static const double st_table_plus[] = {
	1.0e00, 1.0e01, 1.0e02, 1.0e03, 1.0e04, 1.0e05, 1.0e06, 1.0e07, 1.0e08, 1.0e09,
	1.0e10, 1.0e11, 1.0e12, 1.0e13, 1.0e14, 1.0e15, 1.0e16, 1.0e17, 1.0e18, 1.0e19,
	1.0e20, 1.0e21, 1.0e22, 1.0e23, 1.0e24, 1.0e25, 1.0e26, 1.0e27, 1.0e28, 1.0e29,
	1.0e30, 1.0e31, 1.0e32, 1.0e33, 1.0e34, 1.0e35, 1.0e36, 1.0e37, 1.0e38, 1.0e39,
	1.0e40, 1.0e41, 1.0e42, 1.0e43, 1.0e44, 1.0e45, 1.0e46, 1.0e47, 1.0e48, 1.0e49,
	1.0e50, 1.0e51, 1.0e52, 1.0e53, 1.0e54, 1.0e55, 1.0e56, 1.0e57, 1.0e58, 1.0e59,
	1.0e60, 1.0e61, 1.0e62, 1.0e63, 1.0e64, 1.0e65, 1.0e66, 1.0e67, 1.0e68, 1.0e69,
	1.0e70, 1.0e71, 1.0e72, 1.0e73, 1.0e74, 1.0e75, 1.0e76, 1.0e77, 1.0e78, 1.0e79,
	1.0e80, 1.0e81, 1.0e82, 1.0e83, 1.0e84, 1.0e85, 1.0e86, 1.0e87, 1.0e88, 1.0e89,
	1.0e90, 1.0e91, 1.0e92, 1.0e93, 1.0e94, 1.0e95, 1.0e96, 1.0e97, 1.0e98, 1.0e99,
};
static const double st_table_minus[] = {
	1.0e-00, 1.0e-01, 1.0e-02, 1.0e-03, 1.0e-04, 1.0e-05, 1.0e-06, 1.0e-07, 1.0e-08, 1.0e-09,
	1.0e-10, 1.0e-11, 1.0e-12, 1.0e-13, 1.0e-14, 1.0e-15, 1.0e-16, 1.0e-17, 1.0e-18, 1.0e-19,
	1.0e-20, 1.0e-21, 1.0e-22, 1.0e-23, 1.0e-24, 1.0e-25, 1.0e-26, 1.0e-27, 1.0e-28, 1.0e-29,
	1.0e-30, 1.0e-31, 1.0e-32, 1.0e-33, 1.0e-34, 1.0e-35, 1.0e-36, 1.0e-37, 1.0e-38, 1.0e-39,
	1.0e-40, 1.0e-41, 1.0e-42, 1.0e-43, 1.0e-44, 1.0e-45, 1.0e-46, 1.0e-47, 1.0e-48, 1.0e-49,
	1.0e-50, 1.0e-51, 1.0e-52, 1.0e-53, 1.0e-54, 1.0e-55, 1.0e-56, 1.0e-57, 1.0e-58, 1.0e-59,
	1.0e-60, 1.0e-61, 1.0e-62, 1.0e-63, 1.0e-64, 1.0e-65, 1.0e-66, 1.0e-67, 1.0e-68, 1.0e-69,
	1.0e-70, 1.0e-71, 1.0e-72, 1.0e-73, 1.0e-74, 1.0e-75, 1.0e-76, 1.0e-77, 1.0e-78, 1.0e-79,
	1.0e-80, 1.0e-81, 1.0e-82, 1.0e-83, 1.0e-84, 1.0e-85, 1.0e-86, 1.0e-87, 1.0e-88, 1.0e-89,
	1.0e-90, 1.0e-91, 1.0e-92, 1.0e-93, 1.0e-94, 1.0e-95, 1.0e-96, 1.0e-97, 1.0e-98, 1.0e-99,
};
Znk_INLINE double getPow10D( int exp )
{
	/***
	 * d = 10^x = 2^p
	 * log2 d = x log2 10 = p
	 * a * 2^p (  0.5 < a < 2 )
	 * 0.5 * 2^p < a * 2^p < 2 * 2^p
	 * -1 + p < log2 a + p < 1 + p
	 */
	if( exp >= 0 ){
		if( exp < 100 ){
			return st_table_plus[ exp ];
		} else if( exp < 200 ){
			return st_table_plus[ exp-100 ] * 1.0e+100;
		} else if( exp < 300 ){
			return st_table_plus[ exp-200 ] * 1.0e+200;
		} else {
		}
		return st_table_plus[ exp-300 ] * 1.0e+300;
	}
	if( exp > -100 ){
		return st_table_minus[ -exp ];
	} else if( exp > -200 ){
		return st_table_minus[ -exp-100 ] * 1.0e-100;
	} else if( exp > -300 ){
		return st_table_minus[ -exp-200 ] * 1.0e-200;
	} else {
	}
	return st_table_minus[ -exp-300 ] * 1.0e-300;
}
#define M_LOG2_10 3.3219280948873626
Znk_INLINE double getAboutLog10( double dval ){
    uint64_t bits = *(uint64_t*)&dval;
    int      exp  = (int)( (bits >> 52) & UINT64_C(0x7ff) ) - (int)(0x3ff);
	return exp * ( 1.0 / M_LOG2_10 );
}
/*****************************************************************************/




Znk_INLINE double getExp10AndFractionFast( double positive_dval, int* exp_10 )
{
	/**
	 * positive_dval = 0の場合、log10( positive_dval )は-∞になる.
	 */
	double L = getAboutLog10( positive_dval );

	/**
	 * rL = L - exp_10 = L - (int)(L)より
	 * -1.0 < rL < 1.0 が成り立つ.
	 */
	*exp_10 = (int)(L);

	/**
	 * rL < 1.0  より、10^rL < 10.0( =10^(1.0) )
	 * rL > -1.0 より、10^rL > 0.1 ( =10^(-1.0))
	 *
	 * ratio = 10^rLとおくと
	 * 0.1 < ratio <10.0 が成り立つ.
	 * このとき、ratio が 1より小さい場合は小数第1位は必ず非zero である.
	 *
	 * rLは一般に小数を含むため、pow関数を用いずに高速に10^rLを求めるのは難しい.
	 */
	return positive_dval / getPow10D( *exp_10 );
}


/***
 * @brief
 *   正のdouble値(0であってはならない)を10進数形式の仮数・指数に分解する.
 *
 * @param
 * positive_dval: 
 *   変換対象のdouble値.
 *   positive_dval > 0 でなければならない.
 *
 * fraction10: 
 *   dvalを10進指数表示に直した場合の仮数に相当する部分が格納される.
 *   fraction10の整数部分は常に一桁で、以下のように１の位と小数点と小数点以下からなる. 
 *   A.BCDEFGHIJ...
 *   １の位に当たるA は 0 より大きく 9 以下の数字になる.
 *
 * exp_10: 
 *   dvalを10進指数表示に直した場合の指数に相当する部分が格納される.
 *   -308から308までの値をとると考えてよい.
 *
 * @return
 *   なし.
 */
Znk_INLINE void
getBase10Fmt( double positive_dval, double* fraction10, int* exp_10 )
{
	/* Znk_ASSERT( positive_dval > 0.0 ); */
	/**
	 * 1.0 <= dval < 2.0,
	 * 0.1 < ratio < 10.0 より、
	 * 0.1 < dval*ratio < 20.0
	 *
	 * fraction10 = dval*ratio とおけば
	 * 0.1 < fraction10 < 20.0
	 */
	*fraction10 = getExp10AndFractionFast( positive_dval, exp_10 );

	/**
	 * ここで、10.0 <= fraction10 < 20.0 の場合に fraction10 を 10で割り、その代わりexp_10を 1 増やす.
	 * また、  0.1  <  fraction10 < 1.0 の場合に  fraction10 を 10倍し、  その代わりexp_10を 1 減らす.
	 * こうすることで、常に 1.0 <= fraction10 < 10.0 が成り立つ.
	 * このとき、fraction10 の1の位は必ず非zero になる.
	 */
	/* fraction10 += DBL_EPSILON; */
	if( *fraction10 >= 10.0 ){
		*fraction10 /= 10.0;
		++(*exp_10);
	} else if( *fraction10 < 1.0 ){
		*fraction10 *= 10.0;
		--(*exp_10);
	} else {
		// 何もしない.
	}
}


/***
 * @brief
 *   double値を10進数形式の仮数・指数に分解する.
 *
 * @param
 * dval: 
 *   変換対象のdouble値.
 *
 * sign_bit: 
 *   dval が 負のとき true
 *   非負(0 or 正)のとき falseが格納される.
 *
 * fraction10: 
 *   dvalを10進指数表示に直した場合の仮数に相当する部分が格納される.
 *   fraction10の整数部分は常に一桁で、以下のように１の位と小数点と小数点以下からなる. 
 *   A.BCDEFGHIJ...
 *   １の位に当たるA は dval が非ゼロである場合は 0 より大きく 9 以下の数字になる.
 *   dval が 0.0 のとき、A は 0 になる.
 *
 * exp_10: 
 *   dvalを10進指数表示に直した場合の指数に相当する部分が格納される.
 *   -308から308までの値をとると考えてよい.
 *
 * @return
 *   処理が成功した場合 true を返す.
 *   さもなければ false を返す.
 */
Znk_INLINE bool
Inline_decompToBase10( double dval, bool* sign_bit, double* fraction10, int* exp_10 )
{
    uint64_t bits = *(uint64_t*)&dval;
    *sign_bit = (bool)( (bits >> 63) == UINT64_C(0x1) );
	
	if( dval != 0.0 ){
		if( *sign_bit ){
			getBase10Fmt( -dval, fraction10, exp_10 );
		} else {
			getBase10Fmt( dval, fraction10, exp_10 );
		}
	} else {
		*fraction10 = 0.0;
		*exp_10 = 0;
	}
	return true;
}




#define addSignCh( p, sign_ch ) if( (sign_ch) != '\0' ){ *(p)++ = (sign_ch); }

Znk_INLINE char getSignCh( bool is_negative, bool is_zero, ZnkToStrFlags flags )
{
	bool is_none_sign  = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_NONE_SIGN);
	if( is_none_sign ){ return '\0'; }

	if( is_negative ){ return '-'; }

	if( is_zero ){
		bool is_zero_sign_white = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_WHITE);
		bool is_zero_sign_plus  = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_PLUS);
		bool is_zero_sign_minus = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_ZERO_SIGN_MINUS);

		if( is_zero_sign_minus ){
			return '-';
		}else if( is_zero_sign_plus ){
			return '+';
		}else if( is_zero_sign_white ){
			return ' ';
		}else{
			/* none */
		}
		return '\0';
	}
	if( Znk_IS_FLAG32(flags, ZnkToStr_e_IS_PLUS) ){ return '+'; }

	return '\0';
}

Znk_INLINE char getExpSignCh( bool is_negative, bool is_zero, ZnkToStrFlags flags )
{
	bool is_none_sign  = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_EXP_NONE_SIGN);
	if( is_none_sign ){ return '\0'; }

	if( is_negative ){ return '-'; }

	if( is_zero ){
		bool is_zero_sign_plus  = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_EXP_ZERO_SIGN_PLUS);
		bool is_zero_sign_minus = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_EXP_ZERO_SIGN_MINUS);

		if( is_zero_sign_minus ){
			return '-';
		}else if( is_zero_sign_plus ){
			return '+';
		}else{
			/* none */
		}
		return '\0';
	}

	if( Znk_IS_FLAG32(flags, ZnkToStr_e_IS_EXP_PLUS) ){  return '+'; }

	return '\0';
}


#if 0
Znk_INLINE char* addSign( char* p, bool sign_bit, char plus_ch ) {
	*p++ = sign_bit ? '-' : plus_ch;
	return p;
}

/**
 * plus_ch の値に応じて、符号を位置pに付加する.
 *
 * ●plus_ch が '-' のとき
 *   sign_bit が true のとき、'-' を付加する.
 *   sign_bit が false のとき、何も付加しない.
 *
 * ●plus_ch が '-' 以外のとき
 *   sign_bit が true のとき、'-' を付加する.
 *   sign_bit が false のとき、plus_ch を付加する.
 *
 * 何らかの符号が付加された場合は true を さもなければ false を返す.
 */
Znk_INLINE bool autoaddSign( char*& p, bool sign_bit, char plus_ch )
{
	bool is_added = false;
	switch( plus_ch ){
	case '-':
		if( sign_bit ){
			*p++ = '-';
			is_added = true;
		}
		break;
	default:
		p = addSign( p, sign_bit, plus_ch );
		is_added = true;
		break;
	}
	return is_added;
}
#endif

#if 0
/**
 * 何らかの符号が追加される場合か否かを判定する.
 */
Znk_INLINE bool isAddSign( bool sign_bit, char plus_ch )
{
	bool is_added = false;
	switch( plus_ch ){
	case '-':
		if( sign_bit ){
			is_added = true;
		}
		break;
	default:
		is_added = true;
		break;
	}
	return is_added;
}
#endif

/**
 * p からに size個の ch を追加.
 * 最終追加位置の次のアドレスを返す.
 */
Znk_INLINE char* fillChar( char* p, size_t size, char ch )
{
	memset( p, ch, size );
	p += size;
	return p;
}


/**
 * begin から size 分だけを削除する.
 * ただし、削除後のbegin位置は不変で end 位置のみが変わる.
 * 新しく更新された end 位置が返る.
 */
Znk_INLINE char* removeFrontByte_withBeginFix( char* begin, char* end, size_t size )
{
	if( size ){
		/**
		 * 最初のsize分をカットする.
		 * そのために、size byte以降の部分をbegin位置に移動している.
		 */
		char* new_end = end-size;
		memmove( begin, begin+size, new_end-begin );
		return new_end;
	}
	return end;
}

/**
 * 整数桁数固定における余剰スペースのspace_ch埋め.
 *
 * int_width < 2 のとき、何もしない.
 * int_width >= 2 のとき、int_width と exp_10 の値によって 最初に '0' いくつかつける. 
 */
Znk_INLINE char* addFrontZeros( char* p, size_t int_width, int exp_10, char space_ch, bool is_supplement )
{
	if( int_width < 2 || !is_supplement ){
		return p;
	} else {
		size_t remain_width = int_width - 1;
		if( exp_10 >= (int)(remain_width) ){
			return p;
		}
		if( exp_10 > 0 ){
			remain_width -= exp_10;
		}
	
		p = fillChar( p, remain_width, space_ch );
	}
	return p;
}

/**
 * 小数点を左に移動するために begin をどれだけ右へ移動しておけばよいかという
 * 位置調整値を得る.
 */
Znk_INLINE void modifyOffset_forFix( size_t* offset, int exp_10, size_t int_width )
{
	if( exp_10 < 0 ){
		*offset = (int_width == 0) ? -exp_10-1 : -exp_10;
	}
}
Znk_INLINE void modifyOffset_forFlex( size_t* offset, int exp_10 )
{
	if( exp_10 < 0 ){
		*offset = -exp_10;
	}
}


/**
 * is_sign_top == true のとき:
 *   is_compact== true のとき:
 *     桁数が少ないときはできるだけint_width内に-を収めるようにする.
 *     例 : int_width == 5 のとき
 *       @@@12.3456 => -@@12.3456
 *       23456.0000 => -23456.0000
 *   is_compact == false のとき:
 *     桁数が少ない場合でも int_width + 1 桁数で固定する.
 *     例 : int_width == 5 のとき
 *       @@@12.3456 => -@@@12.3456
 *       23456.0000 => -23456.0000
 *
 * is_sign_top == false のとき:
 *   常にis_compact == trueのように振舞う.
 *   すなわち、常に桁数が少ないときはできるだけint_width内に-を収めるようにする.
 *   例 : int_width == 5 のとき
 *     @@@12.3456
 *     @@-12.3456
 *     23456.0000
 *     -23456.0000
 *
 *  compact であるとは、一番左の文字がspace_chならばそれを符号で置換することを言う.
 *  compact でないとは、一番左の文字がなんであろうとその左側に符号を追加することを言う.
 */
/**
 * 理解しやすくするため、以下に典型的なパターンを用いて考える.
 *
 *   exp_10 == 3 のとき
 *     1.2345e2 => 1234.5 という変換
 *   exp_10 == 2 のとき
 *     1.2345e2 => 123.45 という変換
 *   exp_10 == 1 のとき
 *     1.2345e1 => 12.345 という変換
 *   exp_10 == 0 のとき
 *     1.2345e0 => 1.2345 という変換
 *   exp_10 == -1 のとき
 *     1.2345e-1 => 0.12345 という変換
 *   exp_10 == -2 のとき
 *     1.2345e-1 => 0.012345 という変換
 *   exp_10 == -3 のとき
 *     1.2345e-1 => 0.0012345 という変換
 *
 * このことより、次が言える.
 * exp_10 >= 0 のとき、整数部分の桁数は exp_10+1
 * exp_10 <  0 のとき、整数部分の桁数は常に 1
 * int_width == 0, int_width == 1 のときは、space_ch埋めの余地はない.
 * int_width >= 2 であり、以下の条件を満たすとき、space_ch埋めの可能性が発生する.
 * static_cast<int>(int_width) > exp_10+1
 */
Znk_INLINE char*
compaction( char* begin, bool is_supplement, bool is_compact,
		size_t int_width, int exp_10,
		char space_ch, char sign_ch,
		bool is_sign_top )
{
	if( is_sign_top ){
		if( is_compact && int_width >= 2 && (int)int_width > exp_10+1 ){
			/*
			 * int_width に比べて exp_10による繰上げ桁数が少ない場合.
			 * space_ch による埋めが発生する.
			 **/
			if( sign_ch != '\0' ){
				/**
				 * 符号が存在する場合は、数字部分を減らす.
				 * (桁数が少ないため切り捨ては発生しない)
				 */
				addSignCh( begin, sign_ch );
				begin = addFrontZeros( begin, int_width-1, exp_10, space_ch, is_supplement );
			} else {
				/**
				 * 符号を付けない場合.
				 */
				begin = addFrontZeros( begin, int_width, exp_10, space_ch, is_supplement );
			}
		} else {
			/**
			 * 符号を単に左に追加する場合.
			 */
			addSignCh( begin, sign_ch );
			begin = addFrontZeros( begin, int_width, exp_10, space_ch, is_supplement );
		}
	} else {
		if( int_width >= 2 && (int)int_width > exp_10+1 ){
			/*
			 * int_width に比べて exp_10による繰上げ桁数が少ない場合.
			 **/
			if( sign_ch != '\0' ){
				/**
				 * 符号が存在する場合は、数字部分を減らす.
				 * (桁数が少ないため切り捨ては発生しない)
				 */
				begin = addFrontZeros( begin, int_width-1, exp_10, space_ch, is_supplement );
				addSignCh( begin, sign_ch );
			} else {
				/**
				 * 符号を付けない場合.
				 */
				begin = addFrontZeros( begin, int_width, exp_10, space_ch, is_supplement );
			}
		} else {
			/**
			 * 符号を単に左に追加する場合.
			 */
			begin = addFrontZeros( begin, int_width, exp_10, space_ch, is_supplement );
			addSignCh( begin, sign_ch );
		}
	}
	return begin;
}


/**
 * sign_policy の値に応じて begin と int_width の値を調整し
 * 必要に応じて 整数部分を space_ch で埋める.
 */
Znk_INLINE char* adjust_bySignPolicy( char* begin, size_t* int_width,
		int exp_10, char space_ch,
		char sign_ch, char sign_policy,
		ZnkToStrFlags flags,
		bool is_supplement, bool is_truncate )
{
	size_t offset = 0;
	bool is_sign_top = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SIGN_TOP);
	bool is_compact  = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_COMPACT);

	if( is_truncate ){
		switch( sign_policy ){
		case 'U': // 必要ならば符号をカットしてint_widthに収める.
		{
			//fprintf(stderr, "adjust_bySignPolicy:%c: int_width=[%u] exp_10=[%d]\n", sign_policy, int_width, exp_10 );
	
			if( *int_width >= 2 && (int)*int_width > exp_10+1 && sign_ch != '\0' ){
				/**
				 * space_ch埋めのときは常にcompactで
				 */
				if( is_sign_top ){
					addSignCh( begin, sign_ch );
					begin = addFrontZeros( begin, *int_width-1, exp_10, space_ch, is_supplement );
				} else {
					begin = addFrontZeros( begin, *int_width-1, exp_10, space_ch, is_supplement );
					addSignCh( begin, sign_ch );
				}
			} else {
				/**
				 * space_ch埋めがないときは、符号をつけない.
				 */
				begin = addFrontZeros( begin, *int_width, exp_10, space_ch, is_supplement );
			}
			break;
		}
		case 'S': // 符号の分だけ数値表現を減らしてint_widthに収める.
		{
			//fprintf(stderr, "adjust_bySignPolicy:%c: int_width=[%u] exp_10=[%d] is_sign_top=[%d]\n", sign_policy, int_width, exp_10, is_sign_top );
	
			if( *int_width > 0 && sign_ch != '\0' ){
				--*int_width;
				if( is_sign_top ){
					addSignCh( begin, sign_ch );
					begin = addFrontZeros( begin, *int_width, exp_10, space_ch, is_supplement );
				} else {
					begin = addFrontZeros( begin, *int_width, exp_10, space_ch, is_supplement );
					addSignCh( begin, sign_ch );
				}
			} else {
				/**
				 * space_ch埋めがないときは、符号をつけない.
				 */
				begin = addFrontZeros( begin, *int_width, exp_10, space_ch, is_supplement );
			}
			break;
		}
		case 'E':
		default:
			/* 符号の分だけint_widthを越えて拡張. */
			//fprintf(stderr, "adjust_bySignPolicy:%c: int_width=[%u] exp_10=[%d]\n", sign_policy, int_width, exp_10 );
	
			/**
			 * 整数桁数がint_widthを超える場合に上位が打ち切られる.
			 * 整数桁数がint_widthに満たない場合は
			 *   is_supplement == true  のときは space_ch 埋めを行う.
			 *   is_supplement == false のときは space_ch 埋めを行なわい.
			 */
			begin = compaction( begin, is_supplement, is_compact,
					*int_width, exp_10,
					space_ch, sign_ch,
					is_sign_top );
			break;
		}
		modifyOffset_forFix( &offset, exp_10, *int_width );

	} else {
		/**
		 * 整数桁数がint_widthを超える場合でも上位が打ち切られない.
		 * 整数桁数がint_widthに満たない場合は
		 *   is_supplement == true  のときは space_ch 埋めを行う.
		 *   is_supplement == false のときは space_ch 埋めを行なわい.
		 */
		begin = compaction( begin, is_supplement, is_compact,
				*int_width, exp_10,
				space_ch, sign_ch,
				is_sign_top );

		modifyOffset_forFlex( &offset, exp_10 );
	}

	/**
	 * 小数点を左に移動するための begin 位置調整.
	 *
	 * exp_10 >= 0 のとき:
	 *   左に移動することはあり得ない(右にしか移動し得ない)ため、
	 *   offset=0 でなければならない.
	 * exp_10 < 0 のとき:
	 *   offset = -exp_10 で基本的によい.
	 *   ただし、int_width == 0 かつ 固定幅のとき、offset を 1 増やしておかなければならない.
	 *   これをしないと左側に小数点が2つついてしまう.
	 */
	begin += offset;

	return begin;
}


/**
 * 小数点の移動に関する基本処理
 */
Znk_INLINE char* swapFloatingPoint( char* point )
{
	Znk_SWAP( char, *point, *(point-1) ); --point;
	return point;
}
Znk_INLINE char* setZero_onLeftOfPoint( char* point )
{
	*(point-1) = '0';
	return point;
}
Znk_INLINE char* moveFloatingPointToRight( char* point, size_t num )
{
	while( num > 0 ){
		Znk_SWAP( char, *point, *(point+1) ); ++point;
		--num;
	}
	return point;
}


/**
 * もとのdouble値は非ゼロでなければならない.
 * sign_policy != 'A' でなければならない.
 */
Znk_INLINE void getShortage(
		int* num_shortage, bool* sign_shortage,
		char sign_ch, int exp_10, size_t int_width, char sign_policy )
{

	*num_shortage = exp_10 - (int)(int_width) + 1;

	*sign_shortage = false;
	if( sign_ch != '\0' ){
		switch( sign_policy ){
		case 'U':
			if( int_width == 0 || int_width == 1 || *num_shortage >= 0 ){
				*sign_shortage = true;
			}
			break;
		case 'S':
			if( int_width == 0 ){
				*sign_shortage = true;
			} else if( *num_shortage >= 0 ){
				++*num_shortage;
			} else {
			}
			break;
		default:
			break;
		}
	}
}


/*****************************************************************************/


static char*
setDefaultNanCStr( int type, char* p, bool term_nul )
{
	if( type > 0 ){
		return writeBytes( p, "inf", 3, term_nul );
	} else if( type < 0 ){
		return writeBytes( p, "-inf", 4, term_nul );
	} else {
		/* none */
	}
	return writeBytes( p, "nan", 3, term_nul );
}


Znk_INLINE bool
checkNanAndInf( char** p, double dval, bool term_nul, NanCallBackFunc nan_callback_func )
{
	if( nan_callback_func == NULL ){ nan_callback_func = setDefaultNanCStr; }
	if( Inline_isNaN( dval ) ){
		*p = (*nan_callback_func)( 0, *p, term_nul );
		return true;
	}
	if( Inline_isInf( dval ) ){
		if( dval < 0.0 ){
			*p = (*nan_callback_func)( -1, *p, term_nul );
			return true;
		}
		*p = (*nan_callback_func)( 1, *p, term_nul );
		return true;
	}
	return false;
}



/**
 * @brief
 *   C99 printfの %a %A 風の変換処理を行う.
 *   Linux の glibc printfの出力にマッチするような仕様にしてあり、
 *   整数部は dval==0.0のとき0、それ以外のときは必ず 1 となる.
 *
 * @param
 *
 * term_nul:
 *   最後に'\0'終端するか否か.
 *
 * decimal_width:
 *   IS_FLEX_DECIMAL が true の場合
 *     無視される.
 *   IS_FLEX_DECIMAL が false の場合
 *     小数部を固定幅にしたい場合であり、その桁数を指定する.
 *     truncateであり、supplementでもあるような処理を行う.
 *     0を指定した場合は整数部のみになる.
 *
 * exp_width:
 *   指数部の桁数.
 *
 * flags:
 *   以下の属性を解釈する.
 *   IS_LOWER
 *   IS_EXP_LOWER
 *   IS_FLEX_DECIMAL
 *   IS_PLUS
 *   IS_NONE_SIGN
 *   IS_ZERO_SIGN_WHITE
 *   IS_ZERO_SIGN_PLUS
 *   IS_ZERO_SIGN_MINUS
 *   IS_NONE_EXP
 *   IS_EXP_PLUS
 *   IS_EXP_NONE_SIGN
 *   IS_EXP_ZERO_SIGN_PLUS
 *   IS_EXP_ZERO_SIGN_MINUS
 *
 * nan_callback_func
 *   NanCallBackFunc 宣言にあるコメントを参照.
 *
 * is_check_nan
 *   NanCallBackFunc 宣言にあるコメントを参照.
 */
static char*
DoubleToStr_asHexE( char* p, double dval, bool term_nul,
		size_t decimal_width, size_t exp_width, ZnkToStrFlags flags,
		NanCallBackFunc nan_callback_func, bool is_check_nan )
{
	if( is_check_nan ){
		if( checkNanAndInf( &p, dval, term_nul, nan_callback_func ) ){ return p; }
	}

	{
		bool is_lower        = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_LOWER);
		bool is_exp_lower    = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_EXP_LOWER);
		bool is_flex_decimal = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_FLEX_DECIMAL);
	
	    uint64_t bits = *(uint64_t*)&dval;
	
	    bool     sign_bit = (bool)( (bits >> 63) == UINT64_C(0x1) );
	    int      exp      = (int)( (bits >> 52) & UINT64_C(0x7ff) ) - (int)0x3ff;
	    uint64_t fraction_bits = bits & UINT64_C(0x000fffffffffffff);
	
		const char* xdigit = is_lower ?
			"0123456789abcdef" :
			"0123456789ABCDEF";

	
		char sign_ch = getSignCh( sign_bit, (bool)(dval == 0), flags );
		if( sign_ch != '\0' ){ *p++ = sign_ch; }
	
		*p++ = '0';
		*p++ = is_lower ? 'x' : 'X';
	
		if( dval == 0.0 ){
			*p++ = '0';
	
			if( !is_flex_decimal ){
				/**
				 * 固定幅モードのときは、幅数だけ0を付け足す.
				 */
				if( decimal_width > 0 ){
					*p++ = '.';
					while( decimal_width ){
						*p++ = '0';
						--decimal_width;
					}
				}
			}
			*p++ = is_exp_lower ? 'p' : 'P';
			*p++ = '+';
			*p++ = '0';
			if( term_nul ){ *p++ = '\0'; }
			return p;
		} else {
			/**
			 * 小数の一番右側の桁(13位)から小数点(1位)の方向へ順に桁をスキャンする処理において
			 * 初めて非ゼロの桁が来た場合にtrueとなる制御フラグ.
			 */
			bool   in_body = false;

			/**
			 * 小数の一番右側の桁(13位)から小数点(1位)の方向へ順に桁をスキャンする処理において
			 * 初めて非ゼロの桁が来た位置から小数点に達するまでに何桁あるかを示す値.
			 */
			size_t decimal_width_enable=13;

			size_t idx;

			*p++ = '1';
			*p++ = '.';
		
			for(idx=0; idx<13; ++idx){
				p[ 12-idx ] = xdigit[ (uint8_t)(fraction_bits & UINT64_C(0xF)) ];
				fraction_bits >>= 4;
		
				if( !in_body ){
					if( p[ 12-idx ] == '0' ){
						--decimal_width_enable;
					} else {
						/**
						 * 初めて非ゼロの桁が来た.
						 */
						in_body = true;
					}
				}
			}
		
			if( is_flex_decimal || decimal_width_enable == decimal_width ){
				/**
				 * 特に加工なしに必要十分な桁数が提供される.
				 */
				if( decimal_width_enable > 0 ){
					p += decimal_width_enable;
				} else {
					/**
					 * 小数部がないため小数点の表示も必要ない.
					 * 小数点の分、一文字戻す.
					 */
					--p;
				}
		
			} else {
		
				if(decimal_width_enable > decimal_width ){
					if( decimal_width > 0 ){
						/**
						 * GCCではここでもう一つ後ろの桁を参照し、四捨五入をしている.
						 * とりあえずここでは、単に後ろの桁を切り捨てている.
						 */
						p += decimal_width;
					} else {
						/**
						 * 小数部がないため小数点の表示も必要ない.
						 * 小数点の分、一文字戻す.
						 */
						--p;
					}
				} else {
					size_t num;
					p += decimal_width_enable;
					/**
					 * 幅数を満たすため、残りの桁を0で埋める.
					 */
					num = decimal_width - decimal_width_enable;
					while( num ){
						*p++ = '0';
						--num;
					}
				}
		
			}
		
			if( ! Znk_IS_FLAG32(flags, ZnkToStr_e_IS_NONE_EXP) ){
				char exp_sign_ch;
				*p++ = is_exp_lower ? 'p' : 'P';
		
				exp_sign_ch = getExpSignCh( (bool)(exp < 0), (bool)(exp == 0), flags );
				if( exp_sign_ch != '\0' ){ *p++ = exp_sign_ch; }
		
				if( exp < 0 ){ exp = -exp; }
		
				flags |= ZnkToStr_e_IS_SUPPLEMENT;
				p = ZnkToStr_Private_UInt32ToStr( p, exp,
						false, exp_width, '0', 10, flags );
			}
		
			if( term_nul ){ *p++ = '\0'; }
		}
	}
	return p;
}



/***
 * @brief
 *   double値を仮数・指数形式の10進数文字列に変換する.
 *
 * @param
 * p: 
 *   バッファの開始位置を示すポインタ.
 *   この開始位置から結果がバッファへ上書きされる.
 *   必須バッファサイズは 32.
 *   (仮数の実質的な最大精度は15桁、小数点と各種符号、指数部を考慮しても32byteあれば十分と考えられる)
 *
 * dval: 
 *   変換対象のdouble値.
 *
 * term_nul:
 *   最後に'\0'終端するか否か.
 *
 * decimal_width:
 *   仮数部の小数点以下の桁数を指定.
 *   ただし16以上を指定しても実質的な意味はない.
 *
 * exp_width:
 *   指数部の桁数を指定.
 *   double 値 の 10進数でのexp_10の範囲は -308から308までであるため、
 *   3 桁ですべての場合をまかなえるため、これより大きな値を指定しても
 *   あまり意味はない.
 *
 *   IS_EXP_TRUNCATE が指定されている場合、
 *     実際の桁数がexp_width の指定幅を超えている場合は上位を打ち切る.
 *     さもなければ何もしない.
 *   IS_EXP_SUPPLEMENT が指定されている場合、
 *     実際の桁数がexp_width の指定幅に満たない場合は不足分を '0' で埋める.
 *     さもなければ何もしない.
 *
 *   ※ GCC の標準printf のデフォルトの動作は exp_width == 2,
 *      IS_EXP_SUPPLEMENT == true, IS_EXP_TRUNCATE == false である.
 *
 *   ※ VC の標準printf のデフォルトの動作は exp_width == 3,
 *      IS_EXP_SUPPLEMENT == true, IS_EXP_TRUNCATE == false である.
 *      VC8.0以降に限れば、_set_output_formatで調整すれば、
 *      GCC と同じ動作( exp_width == 2) とすることもできるとのこと.
 *
 * flags
 *   以下の属性を解釈する.
 *   IS_EXP_LOWER
 *   IS_PLUS
 *   IS_NONE_SIGN
 *   IS_ZERO_SIGN_WHITE
 *   IS_ZERO_SIGN_PLUS
 *   IS_ZERO_SIGN_MINUS
 *   IS_NONE_EXP
 *   IS_EXP_PLUS
 *   IS_EXP_NONE_SIGN
 *   IS_EXP_ZERO_SIGN_PLUS
 *   IS_EXP_ZERO_SIGN_MINUS
 *   IS_EXP_TRUNCATE
 *   IS_EXP_SUPPLEMENT
 *
 * nan_callback_func
 *   NanCallBackFunc 宣言にあるコメントを参照.
 *
 * is_check_nan
 *   NanCallBackFunc 宣言にあるコメントを参照.
 */
static char*
DoubleToStr_asDecE( char* p, double dval, bool term_nul,
		size_t decimal_width, size_t exp_width, ZnkToStrFlags flags,
		NanCallBackFunc nan_callback_func, bool is_check_nan )
{
	if( is_check_nan ){
		if( checkNanAndInf( &p, dval, term_nul, nan_callback_func ) ){ return p; }
	}

	{
		//bool is_truncate_exp = true; // これを false にすることによって、exp_width == 4 という処理が不要になるかもしれない.
		/**
		 * dval より下記３値を得る.
		 * (これらはInline_decompToBase10により取得された後は基本的に変化しない.)
		 */
		bool   sign_bit   = false;
		double fraction10 = 0.0;
		int    exp_10     = 0;
		char   sign_ch;
		Inline_decompToBase10( dval, &sign_bit, &fraction10, &exp_10 );
	
		sign_ch = getSignCh( sign_bit, (bool)(dval == 0), flags );
		if( sign_ch != '\0' ){ *p++ = sign_ch; }
	
		p = Fraction10ToStr_asDecFix( p, fraction10, term_nul, decimal_width );
	
		if( ! Znk_IS_FLAG32(flags, ZnkToStr_e_IS_NONE_SIGN) ){
			char exp_sign_ch;
			ZnkToStrFlags exp_flags = 0;
			bool is_exp_lower = Znk_IS_FLAG32(flags,ZnkToStr_e_IS_EXP_LOWER);

			*p++ = is_exp_lower ? 'e' : 'E';
			exp_sign_ch = getExpSignCh( (bool)(exp_10 < 0), (bool)(exp_10 == 0), flags );
			if( exp_sign_ch != '\0' ){ *p++ = exp_sign_ch; }
	
			if( exp_10 < 0 ){ exp_10 = -exp_10; }

			if( Znk_IS_FLAG32(flags, ZnkToStr_e_IS_EXP_TRUNCATE) )  { exp_flags |= ZnkToStr_e_IS_TRUNCATE; }
			if( Znk_IS_FLAG32(flags, ZnkToStr_e_IS_EXP_SUPPLEMENT) ){ exp_flags |= ZnkToStr_e_IS_SUPPLEMENT; }
			p = ZnkToStr_Private_UInt32ToStr( p, exp_10, false, exp_width, '0', 10, exp_flags );
		}
		if( term_nul ){ *p = '\0'; }
	}
	return p;
}



/**
 * @brief
 *   double値を仮数・指数形式ではない通常の10進数文字列に変換する.
 *   固定幅 or 伸縮の両方をサポートする.
 *
 * @param
 * p: 
 *   バッファの開始位置を示すポインタ.
 *   この開始位置から結果がバッファへ上書きされる.
 *   必須バッファサイズは 512.
 *
 * dval: 
 *   変換対象のdouble値
 *
 * term_nul:
 *   最後に'\0'終端するか否か.
 *
 * int_width:
 *   整数桁数の基準値を与える.
 *   IS_TRUNCATE が true のとき
 *     指数表示から展開された結果、整数桁数がこれを超える場合は上位の桁を切り捨てる.
 *     さもなければ何もしない.
 *   IS_SUPPLEMENT が true のとき
 *     指数表示から展開された結果、整数桁数がこれに満たない場合は、不足分をspace_chで埋める.
 *     さもなければ何もしない.
 *   int_width == 0, IS_TRUNCATE == true の場合やsign_policyの指定によっては、
 *   -.123 や -00123.56 というような表記になることもあり得る.
 *
 * decimal_width:
 *   小数部分の桁数を指定.
 * 
 * space_ch:
 *   int_width に満たない桁部分を埋めるのに使用する文字を指定.
 *   最も典型的な例は、space_ch = '0' や space_ch = ' ' である.
 *
 * flags:
 *   以下の属性を解釈する.
 *   IS_TRUNCATE
 *   IS_SUPPLEMENT
 *   IS_SIGN_TOP
 *   IS_COMPACT
 *   IS_NONE_SIGN
 *   IS_PLUS
 *   IS_ZERO_SIGN_WHITE
 *   IS_ZERO_SIGN_PLUS
 *   IS_ZERO_SIGN_MINUS
 *
 * sign_policy:
 *   IS_TRUNCATE == true かつ整数桁数が指定桁を超える場合、
 *   符号の扱いに関して以下のどれを優先して処理するかを指定する.
 *
 *       符号  数字表現  指定幅
 *     U △    ○        ◎
 *     S ○    △        ◎(int_width=0の場合は符号も省略される)
 *     E ◎    ○        ○(符号は必ず表示される. 符号一文字分だけint_widthをオーバーすることがある)
 *
 *   詳細説明:
 *   U : 小数点よりも上位の文字列数が確実にint_widthになることを保証する.
 *       その代わり、整数部分の桁数がint_widthを超え、かつ符号が付加される場合、
 *       符号を無視する(数字部分は指定桁数になる).
 *       IS_COMPACT の指定にかかわらず、必ずcompactとなる.
 *
 *   S : 小数点よりも上位の文字列数が確実にint_widthになることを保証する.
 *       その代わり、整数部分の桁数がint_widthを超え、かつ符号が付加される場合、
 *       符号は必ず含め、その分数字一桁犠牲にする.
 *       int_width=1の場合、整数部が符号のみといったケースもあり得る.
 *       int_width=0の場合、符号も省略される.
 *       IS_COMPACT の指定にかかわらず、必ずcompactとなる.
 *
 *   E : 小数点よりも上位の文字列数が確実にint_widthかint_width+1になることを保証する.
 *       符号が付加されない場合、整数部分の桁数はint_widthになることが保証される. 
 *       整数部分の桁数がint_widthを超え、かつ符号が付加される場合、
 *       符号一文字分のサイズだけ拡張され、トータルではint_widthを一つオーバーする.
 *
 *       is_sign_top が true で符号が存在する場合は、
 *         IS_COMPACT == true のとき:
 *           桁数が少ないときはできるだけint_width内に-を収めるようにする.
 *           例 : space_ch=@, int_width == 5 のとき
 *             @@@12.3456
 *             -@@12.3456
 *             23456.0000
 *             -23456.0000
 *         IS_COMPACT == false のとき:
 *           桁数が少ない場合でも int_width + 1 桁数で固定する.
 *           例 : space_ch=@, int_width == 5 のとき
 *             @@@12.3456
 *             -@@@12.3456
 *             23456.0000
 *             -23456.0000
 *
 *       int_width=0の場合も、符号が付加される.
 *
 * ans_num_shortage:
 *   IS_TRUNCATE 指定のとき、整数の上位桁部分がどれだけ失われたかを示す値が格納される.
 *   正数ならばその数だけ失われており、0 以下ならば損失がまったくないことを示す.
 *   ans_num_shortage or ans_sign_shortage のいずれかに NULL が指定された場合、
 *   この値を取得する処理は行われない.
 *
 * ans_sign_shortage:
 *   IS_TRUNCATE 指定のとき、dval が負の数にもかかわらず符号が付加されなかった場合は true
 *   さもなければ false が格納される.
 *   ans_num_shortage or ans_sign_shortage のいずれかに NULL が指定された場合、
 *   この値を取得する処理は行われない.
 *
 * nan_callback_func
 *   NanCallBackFunc 宣言にあるコメントを参照.
 *
 * is_check_nan
 *   NanCallBackFunc 宣言にあるコメントを参照.
 *
 */
static char*
DoubleToStr_asDecF( char* p, double dval, bool term_nul,
		size_t int_width, size_t decimal_width,
		char space_ch, ZnkToStrFlags flags, char sign_policy,
		int* ans_num_shortage, bool* ans_sign_shortage,
		NanCallBackFunc nan_callback_func, bool is_check_nan )
{
	char* end;
	if( is_check_nan ){
		if( checkNanAndInf( &p, dval, term_nul, nan_callback_func ) ){ return p; }
	}

	{
		bool is_truncate   = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_TRUNCATE);
		bool is_supplement = Znk_IS_FLAG32(flags, ZnkToStr_e_IS_SUPPLEMENT);
		/**
		 * dval より下記３値を得る.
		 * (これらはInline_decompToBase10により取得された後は基本的に変化しない.)
		 */
		bool   sign_bit   = false;
		double fraction10 = 0.0;
		int    exp_10     = 0;
		char   sign_ch;
		char*  begin;
		char*  point;
		Inline_decompToBase10( dval, &sign_bit, &fraction10, &exp_10 );
	
		sign_ch = getSignCh( sign_bit, (bool)(dval == 0), flags );
	
		/**
		 * Shortage を計算取得.
		 */
		if( ans_num_shortage != NULL && ans_sign_shortage != NULL ) {
			//if( sign_policy == 'A' || dval == 0.0 ){
			if( dval == 0.0 ){
				*ans_num_shortage  = 0;
				*ans_sign_shortage = false;
			} else {
				getShortage( ans_num_shortage, ans_sign_shortage,
						sign_ch, exp_10, int_width, sign_policy );
			}
		}
	
		/**
		 * sign_policy により、begin と int_width を調整.
		 * また、整数桁数固定モードの場合、上位を適切に space_ch で埋める.
		 * begin は整数部分の開始位置(符号の次の位置)を示すが
		 * 先の処理で変化する可能性もある.
		 */
		begin = p;
		begin = adjust_bySignPolicy( begin, &int_width,
				exp_10, space_ch,
				sign_ch, sign_policy,
				flags,
				is_supplement, is_truncate );
	
		/**
		 * fraction10 を与え、現在のbegin位置に 符号なし仮数の文字列を作成する.
		 * end は文字列の終端位置('\0'が格納されるべき位置)を示すが
		 * 先の処理で変化する可能性もある.
		 */
		{
			size_t taken_size = decimal_width;
			if( exp_10 > 0 ){
				taken_size += exp_10;
			}
			end = Fraction10ToStr_asDecFix( begin, fraction10, term_nul, taken_size );
		}
	
		/**
		 * 小数点の現在位置.
		 * 先の処理で変化する可能性もある.
		 */
		point = begin+1;
	
		/**
		 * 桁数固定モードか否か
		 */
		//bool is_fix_mode = (sign_policy != 'A');
	
		if( exp_10 >= 0 ){
			/**
			 * exp_10 が正の場合
			 * pointが右へと移動する.
			 */
	
			/**
			 * 最後尾に exp_10個の '0' を追加.
			 */
			//end = fillChar( end, exp_10, '0' );
	
			/**
			 * 浮動小数点を右へexp_10個だけ遷移
			 */
			point = moveFloatingPointToRight( point, exp_10 );
	
			if( is_truncate ){
				/**
				 * 指定桁数の最終調整.
				 * point - begin が整数部分の現在の桁数を示す.
				 * リクエストされた int_width からカットすべきサイズを割り出す.
				 *
				 * cut_size が正の場合:
				 *   int_width=5として、
				 *   1234567.89...のように、整数桁数が int_width を越えている場合に起きる.
				 *
				 * cut_size が0の場合:
				 *   int_width=5として、
				 *   12345.678...のように、整数桁数が int_width と調度等しい場合に起きる.
				 *   この場合は考慮しなくてよい.
				 *
				 * cut_size が負の場合:
				 *   int_width=5として、
				 *   00123.456...のように0が補われている場合に起きる.
				 *   この場合は考慮しなくてよい.
				 */
				int cut_size = (int)( point - begin ) - (int)( int_width );
				if( cut_size > 0 ){
					end = removeFrontByte_withBeginFix( begin, end, cut_size );
				}
			}
		} else {
			/**
			 * exp_10 が負の場合
			 * 先頭に 0 が付加されながら、pointが左へと移動する.
			 */
	
			/**
			 * 浮動小数点を左へ -exp_10-1 個だけ遷移
			 * 同時にすぐ左側を'0'に置換していく.
			 * 例えば、1.2345E-3 => 0.0012345 と変換される場合、
			 * 実際の浮動小数点の移動量は3になるのだが、
			 * 以下のように最終回だけは特別処理が入るため、
			 * まず3-1=2回だけループさせている形になる.
			 *
			 * ??1.2345
			 * ??.12345
			 * ?0.12345
			 *
			 * ?0.12345
			 * ?.012345
			 * 0.012345
			 *
			 * 0.012345
			 * .0012345
			 * ここで、最終的に整数部分を付けない場合はこのまま終了する.
			 * 整数部分を付ける場合は、pointの左側を0に置換しなければならない.
			 */
			size_t num = -exp_10-1;
			begin -= num; 
			while( num > 0 ){
				point = swapFloatingPoint( point );
				point = setZero_onLeftOfPoint( point );
				--num;
			}
	
			/**
			 * 最終回
			 * 浮動小数点を左へ 1 個だけ遷移させるだけ.
			 * とりあえず左側を0に置換しない.
			 */
			point = swapFloatingPoint( point );
	
			if( !is_truncate || int_width > 0 ) {
				/**
				 * pointの左側を0に置換する.
				 */
				point = setZero_onLeftOfPoint( point );
			}
	
			end = point + decimal_width + 1;
		}
	
	
		if( term_nul ){ *end = '\0'; }
	}
	return end;
}


static char*
DoubleToStr_asDecG( char* p, double dval, bool term_nul,
		size_t int_width, size_t decimal_width, size_t exp_width,
		char space_ch, ZnkToStrFlags flags, char sign_policy,
		char* selected_method,
		NanCallBackFunc nan_callback_func, bool is_check_nan )
{
	if( is_check_nan ){
		if( checkNanAndInf( &p, dval, term_nul, nan_callback_func ) ){ return p; }
	}

	{
		/**
		 * dval より下記３値を得る.
		 * (これらはInline_decompToBase10により取得された後は基本的に変化しない.)
		 */
		bool   sign_bit   = false;
		double fraction10 = 0.0;
		int    exp_10     = 0;
		Inline_decompToBase10( dval, &sign_bit, &fraction10, &exp_10 );
	
		/**
		 * 変換される値の指数が、-4 より小さいか、精度以上の場合に、e 形式が使用される.
		 */
		if( exp_10 < -4 || ( exp_10 >= 0 && (size_t)(exp_10) >= decimal_width ) ){
			p = DoubleToStr_asDecE( p, dval, term_nul,
					decimal_width, exp_width, flags,
					NULL, false );
			if( selected_method != NULL ){ *selected_method = 'e'; }
		} else {
			p = DoubleToStr_asDecF( p, dval, term_nul,
					int_width, decimal_width,
					space_ch, flags, sign_policy,
					NULL, NULL, NULL, false );
			if( selected_method != NULL ){ *selected_method = 'f'; }
		}
	}
	return p;
}


/*****************************************************************************/


size_t
ZnkToStr_Double_getStrF( char* buf, size_t buf_size, double dval,
		size_t int_width, size_t decimal_width,
		char space_ch, ZnkToStrFlags flags, char sign_policy )
{
	if( buf_size < Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_F ){
		return 0;
	}
	return DoubleToStr_asDecF( buf, dval, true, int_width, decimal_width,
				space_ch, flags, sign_policy,
				NULL, NULL, NULL, true ) - buf;
}
size_t
ZnkToStr_Double_getStrE( char* buf, size_t buf_size, double dval,
		size_t decimal_width, size_t exp_width, ZnkToStrFlags flags )
{
	if( buf_size < Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_E ){
		return 0;
	}
	return DoubleToStr_asDecE( buf, dval, true,
			decimal_width, exp_width, flags,
			NULL, true ) - buf;
}
size_t
ZnkToStr_Double_getStrG( char* buf, size_t buf_size, double dval,
		size_t int_width, size_t decimal_width, size_t exp_width,
		char space_ch, ZnkToStrFlags flags, char sign_policy )
{
	if( buf_size < Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_G ){
		return 0;
	}
	return DoubleToStr_asDecG( buf, dval, true,
			int_width, decimal_width, exp_width,
			space_ch, flags, sign_policy,
			NULL,
			NULL, true ) - buf;
}
size_t
ZnkToStr_Double_getStrG_Ex( char* buf, size_t buf_size, double dval,
		size_t int_width, size_t decimal_width, size_t exp_width,
		char space_ch, ZnkToStrFlags flags, char sign_policy,
		char* selected_method )
{
	if( buf_size < Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_G ){
		return 0;
	}
	return DoubleToStr_asDecG( buf, dval, true,
			int_width, decimal_width, exp_width,
			space_ch, flags, sign_policy,
			selected_method,
			NULL, true ) - buf;
}
size_t
ZnkToStr_Double_getStrHexE( char* buf, size_t buf_size, double dval,
		size_t decimal_width, size_t exp_width, ZnkToStrFlags flags )
{
	if( buf_size < Znk_TOSTR_ENOUGH_BUFSIZE_FOR_DOUBLE_X ){
		return 0;
	}
	Znk_CLAMP_MAX( decimal_width, 16 );
	return DoubleToStr_asHexE( buf, dval, true,
			decimal_width, exp_width, flags,
			NULL, true ) - buf;
}

