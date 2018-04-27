/***
 * この実装では、ZnkVSNPrintfCtxを用いることにより、展開文字列のバッファへの追加を汎用化してある.
 * これは即ち、任意のデバイスやファイル、ソケットなどへの転送をvsnprintfで一旦文字列として
 * 取得せずダイレクトに行うことができるということであり、これにより高速化が望めるケースもある.
 *
 * しかしながら、大抵の場合、このようなデバイスやファイル、ソケットへの転送は低速であり、
 * 従って、展開文字列の部分要素(平文字列や%部分)が発生する度にこのような転送関数を呼び出すこと自体
 * かえって低速化を招いてしまう可能性もある. このような場合、メモリ上のバッファに完成したバッファ
 * を一旦作っておいて一気にブロック転送した方がむしろ高速である(メモリへの転送が最も高速であるから).
 *
 * WindowsのコマンドプロンプトやLinuxなどのxtermへの標準出力は、仮想デバイスであり実際には
 * メモリへの転送であろうから、このようなダイレクトな転送により高速化は期待できる可能性もある.
 * この場合、stdoutがテキストモードである場合は各展開部分要素ごとにNULL終端させてfputs関数を
 * 呼び出すなどの措置をしなければならない. ところが、実際に実装してみるとわかるが、これは意外と
 * うまくいかない. 各要素ごとにNULL終端させるとなると、fputsで渡す場合において、どうしても別
 * バッファを用意し、そこへ一旦コピーしなければならないからである. これでは結局本末転倒である.
 *
 * またApacheのようなWebサーバで標準出力を内部でソケット転送にリダイレクトしているケースもあり、
 * このような場合、stdoutでもかえって低速化を招く可能性もある.
 *
 * というわけで、Znk_vsnprintf_generalという関数は一応用意されているわけだが
 * これのZnkVSNPrintfCtxがいわゆるZnk_vsnprintf_C99の使い方以外で有用となる事例は
 * 現時点ではほぼ思いつかない.
 */
#include "Znk_vsnprintf.h"

#include <Znk_stdc.h>
#include <Znk_def_util.h>

#include <Znk_tostr_int.h>
#include <Znk_tostr_double.h>
//#include <Znk_f_std.h>

#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>


#if defined(__BORLANDC__)
/**
 * 警告 W8066 hoge.cpp XX: 実行されないコード(関数)
 */
#  pragma warn -8066
#endif


#ifdef __cplusplus
#  define M_STATIC_CAST(type,val)       static_cast<type>(val)
#  define M_REINTERPRET_CAST(type,val)  reinterpret_cast<type>(val)
#else
#  define M_STATIC_CAST(type,val)      ((type)(val))
#  define M_REINTERPRET_CAST(type,val) ((type)(val))
#endif

#define M_P_VA_ARG_AS_TYPE(as_type, ap, type)  M_STATIC_CAST( as_type, va_arg(ap, type) )

typedef long long          LongLong;
typedef unsigned long long ULongLong;


typedef struct {
	va_list ap_;
} VAWrap;

/***
 * va_listのコピーは必ずこのマクロを通して行わなければならない.
 * (va_listの実体は=で単純に代入できるオブジェクトとは限らない.
 * 64bit AMD上gccでは実体がレジスタであり、デリファレンス演算子すら期待通りの挙動とならない場合すらある)
 */
#if defined(__GNUC__)
#  define M_P_VA_COPY( dst, src ) va_copy( dst, src )
#else
#  define M_P_VA_COPY( dst, src ) (dst)=(src)
#endif


/*****************************************************************************/
/**
 * POD Utility
 */

Znk_INLINE int diffSize( size_t op1, size_t op2 )
{
	return M_STATIC_CAST(int, op1) - M_STATIC_CAST(int, op2);
}

#define M_PTR_DIFF_AS_TYPE(type, ptr1, ptr2)  (M_STATIC_CAST(type, ptr1) - M_STATIC_CAST(type, ptr2))


Znk_INLINE size_t
Buf_memset( void* buf, size_t buf_size, size_t pos, uint8_t ch, size_t req_size )
{
	if( buf_size > pos && req_size ){
		const size_t avail = buf_size - pos;
		const size_t copy_size = Znk_MIN( req_size, avail );
		memset( M_STATIC_CAST(uint8_t*, buf) + pos, ch, copy_size );
		return copy_size;
	}
	return 0;
}

/**
 * bufの位置posへ bytes[0,req_size)をコピーする.
 * ただし pos+req_size が buf_size をオーバーする場合は、
 * バッファオーバーが起きないようにreq_sizeが調整されたサイズ分のみがコピーされる.
 */
Znk_INLINE size_t
Buf_memcpy( void* buf, size_t buf_size, size_t pos, const void* bytes, size_t req_size )
{
	if( buf_size > pos && req_size ){
		const size_t avail = buf_size - pos;
		const size_t copy_size = Znk_MIN( req_size, avail );
		memcpy( M_STATIC_CAST(uint8_t*, buf) + pos, bytes, copy_size );
		return copy_size;
	}
	return 0;
}
Znk_INLINE size_t
Buf_memmove( void* buf, size_t buf_size, size_t pos, const void* bytes, size_t req_size )
{
	if( buf_size > pos && req_size ){
		const size_t avail = buf_size - pos;
		const size_t copy_size = Znk_MIN( req_size, avail );
		memmove( M_STATIC_CAST(uint8_t*, buf) + pos, bytes, copy_size );
		return copy_size;
	}
	return 0;
}

/**
 * buf[0, buf_size) までで最初に ch が現れる位置を求める.
 * 位置はアドレスではなく、buf[0] を 0 とした配列のインデックスとして取得される.
 * ch が見つからなかった場合は buf_size が戻り値となる.
 */
Znk_INLINE size_t
Buf_findByte( const void* buf, size_t buf_size, uint8_t ch )
{
	const void* end = memchr( buf, ch, buf_size );
	return ( end == NULL ) ?
		buf_size :
		M_PTR_DIFF_AS_TYPE( const uint8_t*, end, buf );
}


//M_PRIVATE_FUNC uint32_t castPtrToU32( void* ptr );
//M_PRIVATE_FUNC uint64_t castPtrToU64( void* ptr );


/**
 * BitExtend で示される部分は bit拡張時に必ず修正が必要な場所を示す.
 * 例えば、現状、最大 64 bit データまで扱えるが、これを 128 bit データが扱える用に
 * 拡張する場合、BitExtendをテキストエディタの検索キーワードに入れて
 * それが示す部分を修正すればよい.
 */

#if 0
/**
 * interpretBitsToSignedInt
 * 任意の整数データ(ポインタ型は除く)型を与え、まずそれを同サイズのsigned型と見なす.
 * (与えたデータが unsigned A型の場合、static_cast<signed A>(unsigned A)というように処理する)
 * このとき、bitレベルでの変化は発生しない.
 * signedとみなされたデータをintmax_t型として取得する.
 *
 * interpretBitsToUnsignedInt
 * 任意の整数データ(ポインタ型は除く)型を与え、まずそれを同サイズのunsigned型と見なす.
 * (与えたデータが signed A型の場合、static_cast<unsigned A>(signed A)というように処理する)
 * このとき、bitレベルでの変化は発生しない.
 * unsignedとみなされたデータをuintmax_t型として取得する.
 *
 * 通常、signed と unsigned 版の双方を明確に持つデータならば、このような変換を行う場合、
 * 以下のようにすれば済む.
 *
 *   unsigned A a = initA_bySomeBitPattern();
 *   static_cast<intmax_t>( static_cast<signed A>( a ) );
 *
 *   signed B b = initB_bySomeBitPattern();
 *   static_cast<uintmax_t>( static_cast<unsigned B>( b ) );
 *
 * しかし、size_t や ptrdiff_t 等、それと同等のサイズでしかも対応するsigned(unsigned)な
 * データタイプ型が明確に存在しないものについては、このような変換をするには環境依存な
 * 記述になってしまう.
 *
 * interpretBitsToSignedInt と interpretBitsToUnsignedInt は、
 * 環境毎に#ifdefで切り替えることなく、これを実現することを目指したものである.
 * ただし、コンパイラはテンプレートの特殊化をサポートしている必要がある.
 *
 * 注意:
 * 1. Unix等にある ssize_t は標準ではないためここでは考慮しない.
 * 2. #ifdef で切り替えるといった手法をとらない理由は、
 *    未知の環境や新規環境での対応が煩雑になるため.
 *    (このような切り替えはなるべくB1_c_baseで閉じておくべき)
 *    ここで採用したtemplate の特殊化を用いる手法でも、新規環境に対応する必要性は残されているが、
 *    それはせいぜい32,64,128bit型といったデータサイズの調整と
 *    size_t, ptrdiff_t と同類の新しいデータ型の追加作業に限定される.
 *    以下に示すとおり、そのような対応のための記述は非常に簡潔に行える.
 */
template< typename T, size_t data_size > inline intmax_t interpretBitsToSignedInt_impl( T val );
#define M_DEF_SPECIALIZE_interpretBTSI( type, bit ) \
	template<> inline intmax_t interpretBitsToSignedInt_impl<type, bit>( type val ){\
		return static_cast<intmax_t>( static_cast<int##bit##_t>( val ) );\
	}
template< typename T, size_t data_size > inline uintmax_t interpretBitsToUnsignedInt_impl( T val );
#define M_DEF_SPECIALIZE_interpretBTUI( type, bit ) \
	template<> inline uintmax_t interpretBitsToUnsignedInt_impl<type, bit>( type val ){\
		return static_cast<uintmax_t>( static_cast<uint##bit##_t>( val ) );\
	}


/**
 * BitExtend
 * N bit データ型に対応させるには、以下を追加すればよい.
 * M_DEF_SPECIALIZE_interpretBTSI( type, N )
 * M_DEF_SPECIALIZE_interpretBTUI( type, N )
 */
#define M_DEF_SPECIALIZE_interpretBTSI_ALL( type ) \
	M_DEF_SPECIALIZE_interpretBTSI( type, 8 ) \
	M_DEF_SPECIALIZE_interpretBTSI( type, 16 ) \
	M_DEF_SPECIALIZE_interpretBTSI( type, 32 ) \
	M_DEF_SPECIALIZE_interpretBTSI( type, 64 ) \

#define M_DEF_SPECIALIZE_interpretBTUI_ALL( type ) \
	M_DEF_SPECIALIZE_interpretBTUI( type, 8 ) \
	M_DEF_SPECIALIZE_interpretBTUI( type, 16 ) \
	M_DEF_SPECIALIZE_interpretBTUI( type, 32 ) \
	M_DEF_SPECIALIZE_interpretBTUI( type, 64 ) \

/**
 * 新しいデータ型 type に対応させるには、以下を追加すればよい.
 * type が unsigned 型のデータである場合:
 *   M_DEF_SPECIALIZE_interpretBTSI_ALL( type )
 * type が signed 型のデータである場合:
 *   M_DEF_SPECIALIZE_interpretBTUI_ALL( type )
 */
M_DEF_SPECIALIZE_interpretBTSI_ALL( size_t )
M_DEF_SPECIALIZE_interpretBTUI_ALL( ptrdiff_t )
M_DEF_SPECIALIZE_interpretBTSI_ALL( clock_t )

template< typename T > inline
intmax_t interpretBitsToSignedInt( T val ){
	return interpretBitsToSignedInt_impl<T, sizeof(T)*8>(val);
}
template< typename T > inline
uintmax_t interpretBitsToUnsignedInt( T val ){
	return interpretBitsToUnsignedInt_impl<T, sizeof(T)*8>(val);
}
#endif



Znk_INLINE uintmax_t
cast_to_uintmax_t_from_ptrdiff_t( ptrdiff_t src )
{
#if defined(Znk_RECENT_VC) || defined(__GNUC__)
	return (uintmax_t)(size_t)( src );
#else
	switch( sizeof(ptrdiff_t) ){
	case 1: return (uintmax_t)( (uint8_t )( src ) );
	case 2: return (uintmax_t)( (uint16_t)( src ) );
	case 4: return (uintmax_t)( (uint32_t)( src ) );
	case 8: return (uintmax_t)( (uint64_t)( src ) );
	default: break;
	}
	return (uintmax_t)(size_t)( src );
#endif
}
Znk_INLINE intmax_t
cast_to_intmax_t_from_size_t( size_t src )
{
#if defined(Znk_RECENT_VC) || defined(__GNUC__)
	return (intmax_t)(ptrdiff_t)( src );
#else
	switch( sizeof(size_t) ){
	case 1: return (intmax_t)( (int8_t )( src ) );
	case 2: return (intmax_t)( (int16_t)( src ) );
	case 4: return (intmax_t)( (int32_t)( src ) );
	case 8: return (intmax_t)( (int64_t)( src ) );
	default: break;
	}
	return (intmax_t)(ptrdiff_t)( src );
#endif
}


/*
 * endof POD Utility
 ***/
/*****************************************************************************/


/*****************************************************************************/
/**
 * Data
 */

/**
 * ZeroPad
 */
typedef struct {
	/**
	 * index into body where zero padding is to be inserted
	 */
	size_t pos;
	/**
	 * number of zeros to be inserted for numeric conversions
	 * as required by the precision or minimal field width
	 */
	size_t size;
}ZeroPad;

Znk_INLINE void
ZeroPad_clear( ZeroPad* zeropad )
{
	zeropad->pos  = 0;
	zeropad->size = 0;
}


/**
 * PercentFlags
 */
typedef struct {
	bool zero_padding;   //余剰を'0'で埋めるか否か(falseの場合' 'で埋める)
	bool justify_left;   //左寄せを行う.
	bool alternate_form; //代替形式(#フラグ)指定がされているか否か.
	bool force_sign;     //正の数の場合に符号をつけるか否か.
	bool space_for_positive; //+の代わりに' 'を使用するか否か.
}PercentFlags;

Znk_INLINE void
PercentFlags_clear( PercentFlags* pfg )
{
	pfg->zero_padding   = false;
	pfg->justify_left   = false;
	pfg->alternate_form = false;
	pfg->force_sign     = false;
	pfg->space_for_positive = true;
}
Znk_INLINE const char*
PercentFlags_get( PercentFlags* pfg, const char* p )
{
	/**
	 * If both the ' ' and '+' flags appear,
	 * the ' ' flag should be ignored.
	 *
	 * If the '0' and '-' flags both appear,
	 * the '0' flag should be ignored.
	 */

	while (*p == '0' || *p == '-' || *p == '+' ||
	       *p == ' ' || *p == '#' || *p == '\'')
	{
		switch (*p) {
		case '0': pfg->zero_padding = true; break;
		case '-': pfg->justify_left = true; break;
		case '+':
			pfg->force_sign         = true;
			pfg->space_for_positive = false;
			break;
		case ' ':
			pfg->force_sign = true;
			/**
			 * If both the ' ' and '+' flags appear, the ' ' flag should be ignored.
			 * But in Perl the last of ' ' and '+' applies.
			 */
#ifdef PERL_COMPATIBLE
			pfg->space_for_positive = true;
#endif
			break;
		case '#': pfg->alternate_form = true; break;
		case '\'': break;
		default:
			break;
		}
		p++;
	}
	return p;
}


/**
 * MinFieldWidth
 */
Znk_INLINE size_t
MinFieldWidth_get( const char** pp, VAWrap* vawp, bool* justify_left )
{
	size_t min_field_width = 0;
	const char* p = *pp;

	if( *p == '*' ){
		int aster_arg;
		++p;
		aster_arg = va_arg(vawp->ap_, int);

		if( aster_arg >= 0 ){
			min_field_width = aster_arg;
		} else {
			min_field_width = -aster_arg;
			*justify_left = true;
		}

	} else if( isdigit((int)(*p)) ){
		/**
		 * size_t could be wider than unsigned int;
		 * make sure we treat argument like common implementations do
		 */
		unsigned int dec_sum = *p - '0';
		++p;
		while( isdigit((int)(*p)) ){
			dec_sum = 10 * dec_sum + (*p - '0');
			++p;
		}

		min_field_width = dec_sum;
	}

	*pp = p;
	return min_field_width;
}


/**
 * PrecisionInfo
 */
typedef struct {
	size_t size;
	bool   specified;
} PrecisionInfo;

Znk_INLINE void
PrecisionInfo_clear( PrecisionInfo* prec_info )
{
	prec_info->size = 0;
	prec_info->specified = false;
}
Znk_INLINE const char*
PrecisionInfo_get( PrecisionInfo* prec_info, const char* p, VAWrap* vawp )
{
	/**
	 * parse PrecisionInfo
	 * %XXX.YYY の ".YYY" に相当する部分.
	 */
	if (*p != '.') {
		return p;
	}
	p++; // skip '.'

	prec_info->specified = true;

	if (*p == '*') {
		int j = va_arg(vawp->ap_, int);
		p++;

		if (j >= 0){
			prec_info->size = j;
		} else {
			/**
			 * NOTE:
			 *   Solaris 2.6 man page claims
			 *   that in this case the precision should be set to 0.
			 *
			 *   Digital Unix 4.0, HPUX 10 and BSD man page claim
			 *   that this case should be treated as unspecified precision,
			 *   which is what we do here.
			 */
			prec_info->specified = false;
			prec_info->size = 0;
		}
	} else if ( isdigit( (int)(*p) ) ){
		/* size_t could be wider than unsigned int;
		 * make sure we treat argument like common implementations do
		 */
		unsigned int uj = *p++ - '0';
		while( isdigit( (int)(*p) ) ){
			uj = 10*uj + (unsigned int)(*p++ - '0');
		}

		prec_info->size = uj;
	}

	return p;
}


/***
 * LengthModifier
 */
Znk_INLINE int
LengthModifier_get( const char** pp )
{
	/**
	 * 0 はlength_modifierが指定されていないことを示す(int型とみなす)
	 * int16_t/uint16_t整数型は内部表現として1に変換される.
	 * int32_t/uint32_t整数型は内部表現として2に変換される.
	 * int64_t/uint64_t整数型は内部表現として3に変換される.
	 */
	int length_modifier = 0;
	const char* p = *pp;

	switch( *p ){
	case 'h':
	case 'z':
	case 'j':
	case 't':
	case 'L':
		length_modifier = *p;
		++p;
		break;
	case 'l':
		length_modifier = *p;
		++p;
		/**
		 * ll = long long
		 */
		if (length_modifier == 'l' && *p == 'l') {
			length_modifier = '2'; /* encoded as '2' */
			++p;
		}
		break;
	case 'I':
		/**
		 * BitExtend
		 * VC, bcc にある I64 modifiers をサポート.
		 * C99標準ではない.
		 * またついでにI16,I32もサポートする.
		 */
		switch( *(p+1) ){
		case '1':
			if( *(p+2) == '6' ){
				/* I16 */
				length_modifier = 1; /* encoded as 1 */
				p += 3;
			}
			break;
		case '3':
			if( *(p+2) == '2' ){
				/* I32 */
				length_modifier = 2; /* encoded as 2 */
				p += 3;
			}
			break;
		case '6':
			if( *(p+2) == '4' ){
				/* I64 */
				length_modifier = 3; /* encoded as 3 */
				p += 3;
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	*pp = p;
	return length_modifier;
}

/*
 * endof Data
 **/
/*****************************************************************************/



/*****************************************************************************/
/**
 * GetBodyOfInteger
 */

/**
 * fmt_spec, arg_sign, pfg で指定される情報からバッファにprefixを付加する.
 * prefix とは、'+'(' ') or 0x(0X) をいう.
 * バッファの開始位置はpで示される.
 * prefixのバッファ内での終端位置(最終文字の次の位置)が戻り値となる.
 */
Znk_INLINE char*
setPrefix_bySignAndFlags( char* p, char fmt_spec, int arg_sign, const PercentFlags* pfg )
{
	if (fmt_spec == 'd') {
		if (pfg->force_sign && arg_sign >= 0)
			*p++ = pfg->space_for_positive ? ' ' : '+';

		/* leave negative numbers for sprintf to handle,
		 * to avoid handling tricky cases like (short int)(-32768) */

#ifdef LINUX_COMPATIBLE
	} else if (fmt_spec == 'p' && pfg->force_sign && arg_sign > 0) {
		*p++ = pfg->space_for_positive ? ' ' : '+';
#endif
	} else if (pfg->alternate_form) {

		/**
		 * alternate form should have no effect for p conversion,
		 * but HPUX 10: for an alternate form of p conversion,
		 * a nonzero result is prefixed by 0x.
		 */
		if (arg_sign != 0 && (fmt_spec == 'x' || fmt_spec == 'X') ) {
			*p++ = '0';
			*p++ = fmt_spec;
		} else {

#ifdef HPUX_COMPATIBLE
			if (fmt_spec == 'p'
#ifndef HPUX_BUG_COMPATIBLE
					/* Actually it uses 0x prefix even for a zero value. */
					&& arg_sign != 0
#endif
			){
				*p++ = '0';
				*p++ = 'x';
			}
#endif

		}

	} else {
	}
	return p;
}

/**
 * buf[ idx ] から始まる文字列が "-" or "0x" or "0X" の
 * いずれかである場合は、それらをスキップするため、idxを加算する.
 */
Znk_INLINE void
updateZeroPaddingPos( size_t* idx, const char* buf, size_t buf_size )
{
	if(*idx < buf_size && buf[*idx] == '-') { ++*idx; }

	if(*idx <= buf_size - 2 &&
		buf[*idx] == '0' && (buf[*idx+1] == 'x' || buf[*idx+1] == 'X')
	){
		*idx += 2;
	}
}


/***
 * 単に va_arg( ap, uint16_t ) などとすると 64bit上のgcc(Ver4.8.1)では次のような警告が発生する.
 *
 *   Znk_vsnprintf.c:892:42: 警告: ‘int16_t’ is promoted to ‘int’ when passed through ‘...’ [デフォルトで有効]
 *
 * 指摘している内容は適切であるが、しかしながら以下のように明らかに問題ないようなコードでもこの警告は発生してしまう.
 *   val = ( sizeof(uint16_t) <= sizeof(int) ) ?
 *       (uint16_t)va_arg(ap, int) : va_arg(ap, uint16_t);
 *
 * つまり va_arg( ap, uint16_t ) という記述が含まれる時点で(それが明らかに実行されない場合でも)この警告は出てしまう.
 *
 * gccは長らくファイルの一部のコードに対してのみ警告オプションを変更する機能がなかったが,
 * gcc 4.6.0から#pragmaで変更可能となった. すなわち以下のようにして警告を部分的かつ一時的に無効にすることができる.
 *   #pragma GCC diagnostic push
 *   #pragma GCC diagnostic ignored "-Wformat"
 *       ...
 *   #pragma GCC diagnostic pop
 *
 * しかしながらこれは[デフォルトで有効]とある通り、いかなる警告オプションにも属さないため、
 * そもそも抑制することができない.
 *
 * よってここで残された手段としては、intのbit数を厳密に調べてdefineし、
 * この値によって警告の発生するコード自体を含めないように切り替えるしかない.
 */

Znk_INLINE int16_t va_arg_int16_t( VAWrap* vawp ){
#if Znk_BIT_int >= 16
	return (int16_t)va_arg( vawp->ap_, int );
#else
	return va_arg( vawp->ap_, int16_t );
#endif
}
Znk_INLINE int32_t va_arg_int32_t( VAWrap* vawp ){
#if Znk_BIT_int >= 32
	return (int32_t)va_arg( vawp->ap_, int );
#else
	return va_arg( vawp->ap_, int32_t );
#endif
}
Znk_INLINE int64_t va_arg_int64_t( VAWrap* vawp ){
#if Znk_BIT_int >= 64
	return (int64_t)va_arg( vawp->ap_, int );
#else
	return va_arg( vawp->ap_, int64_t );
#endif
}
Znk_INLINE uint16_t va_arg_uint16_t( VAWrap* vawp ){
#if Znk_BIT_int >= 16
	return (uint16_t)va_arg( vawp->ap_, unsigned int );
#else
	return va_arg( vawp->ap_, uint16_t );
#endif
}
Znk_INLINE uint32_t va_arg_uint32_t( VAWrap* vawp ){
#if Znk_BIT_int >= 32
	return (uint32_t)va_arg( vawp->ap_, unsigned int );
#else
	return va_arg( vawp->ap_, uint32_t );
#endif
}
Znk_INLINE uint64_t va_arg_uint64_t( VAWrap* vawp ){
#if Znk_BIT_int >= 64
	return (uint64_t)va_arg( vawp->ap_, unsigned int );
#else
	return va_arg( vawp->ap_, uint64_t );
#endif
}


#define M_GET_IVAL_SIGN( ival, sign ) {\
			if      ((ival) > 0) (sign) =  1; \
			else if ((ival) < 0) (sign) = -1; \
}
#define M_GET_UVAL_SIGN( uval, sign ) {\
			(sign) = (uval != 0); \
}

#define M_VA_GET_IVAL( ap, type, val, sign, data_size ) {\
			(val) = va_arg((ap), type); \
			M_GET_IVAL_SIGN( val, sign ) \
			(data_size) = sizeof(type); \
}
#define M_VA_GET_UVAL( ap, type, val, sign, data_size ) {\
			(val) = va_arg((ap), type); \
			M_GET_UVAL_SIGN( val, sign ) \
			(data_size) = sizeof(type); \
}

#define M_VA_GET_IVAL_EX( vawp, type, val, sign, data_size ) {\
			val = va_arg_##type( vawp ); \
			M_GET_IVAL_SIGN( val, sign ) \
			(data_size) = sizeof(type); \
}
#define M_VA_GET_UVAL_EX( vawp, type, val, sign, data_size ) {\
			val = va_arg_##type( vawp ); \
			M_GET_UVAL_SIGN( val, sign ) \
			(data_size) = sizeof(type); \
}


Znk_INLINE bool
getBody_forInteger( ZnkVSNPrintfCtx* vsnp_ctx,
		int length_modifier, char fmt_spec, size_t min_field_width,
		VAWrap* vawp, PercentFlags* pfg )
{

	/**
	 * NOTE:
	 * the u, o, x, X and p conversion specifiers imply the value is unsigned;
	 * d implies a signed value
	 */

	/**
	 * 0 if numeric argument is zero (or if pointer is NULL for 'p'),
	 * +1 if greater than zero (or nonzero for unsigned arguments),
	 * -1 if negative (unsigned argument is never negative)
	 */
	int arg_sign = 0;

	size_t data_size = 0;

	union {
		/**
		 * only defined for length modifier h, or for no length modifiers
		 */
		int          ival;
		unsigned int uval;
		/**
		 * only defined for length modifier l
		 */
		long          lval;
		unsigned long ulval;
		/**
		 * pointer value only defined for p conversion
		 */
		void*         ptr;
		/**
		 * only defined for length modifier ll
		 */
		LongLong   llval;
		ULongLong  ullval;

		intmax_t  intmax_val;
		uintmax_t uintmax_val;
		size_t    size_val;
		ptrdiff_t ptrdiff_val;

		/**
		 * BitExtend
		 */
		int32_t  i32;
		uint32_t u32;
		int64_t  i64;
		uint64_t u64;
	} u;
	u.uintmax_val = 0;


	if (fmt_spec == 'p') {
		/**
		 * HPUX 10:
		 *   An l, h, ll or L before any other conversion character
		 *   (other than d, i, u, o, x, or X) is ignored.
		 *
		 * Digital Unix:
		 *   not specified, but seems to behave as HPUX does.
		 *
		 * Solaris:
		 *   If an h, l, or L appears before any other conversion specifier
		 *   (other than d, i, u, o, x, or X), the behavior is undefined.
		 *   (Actually %hp converts only 16-bits of address and %llp treats address
		 *   as 64-bit data which is incompatible with (void *) argument on a 32-bit system).
		 *
		 * We adopt HPUX behavior.
		 */

		u.ptr = va_arg( vawp->ap_, void* );
		if (u.ptr != NULL) arg_sign = 1;
		data_size = sizeof( void* );

	} else if (fmt_spec == 'd') {  /* signed */

		/**
		 * It is non-portable to specify a second argument of char or short to va_arg,
		 * because arguments seen by the called function are not char or short.
		 * C converts char and short arguments to int before passing them to a function.
		 *
		 * 変数のサイズを指定する h 、l 、L については注意が必要である.
		 * %hd は short int 、%d は int 、%ld は long int の引数に対応し、
		 * %f は float 、%lf は double 、%Lf は long double に対応していることになっている.
		 * しかしながら、可変長引数を持つ関数宣言の場合(または引数に型を記述しない古いタイプ
		 * の関数宣言の場合)には、各引数には標準変換がなされ、int より短い整数引数は int に、
		 * double より短い浮動小数点数引数は double に暗黙のうちに変換される(※).
		 * このことより、%hd と %d 、%f と %lf は結局同じ意味になってしまう.
		 * よって va_arg(ap, short) や va_arg(ap, float) といった記述は常に正しく動くとは
		 * 限らないし、そもそも使う必要がない（警告を出してくれる処理系もあるらしい).
		 *
		 * (※)
		 * このような可変長関数引数における「intより小さい型のintへの引き上げ」と
		 * 「float型のdouble型への引き上げ」は プログラミング言語C 第2版(K&R)の A7.3.2 
		 * 関数呼び出しの項(p247,p248)に明示的に記載されている. また以下のサイトにも
		 * わかりやすい説明がある. 「標準変換」という用語についてはこのサイトから借用した.
		 *
		 * http://www.k-cube.co.jp/wakaba/server/func/va_arg.html
		 */

		/**
		 * %jd, %td を認める.
		 * %zd (符号付きsize_t)は、size_t型を受け取ってそれを符号付10進数で表示するという
		 * 指示だが、これをどう解釈するか? そもそも size_t型とは符号なしの型である.
		 * これを普通に符号なし10進数で表示するならば、ユーザは %zu と指定すべきだが、
		 * ここではあえてそうせず %zd と指定されている. これは size_t 型を ssize_t へ
		 * キャストしたものを表示せよという意志とみなすことにする. 例えば size_t の実体が
		 * uint16_t の環境の場合で、渡された値が 65534 であった場合、ここは -2 と表示される.
		 * これは %d とあるところへ (巨大な値の)unsigned int の値を指定した場合の挙動に準ずる.
		 * Unix等にあるssize_t は標準ではない. 例えばVC,bccではエラーになる. size_t のサイズ
		 * は一般にはわからないため、以下ではB1_M_CAST_TO_SINTを用いて同型かつ異符号への
		 * 抽象化されたキャストを実現している.
		 */
		switch (length_modifier) {
		case 1  : M_VA_GET_IVAL_EX( vawp, int16_t,     u.i32,         arg_sign, data_size ) break;
		case 2  : M_VA_GET_IVAL_EX( vawp, int32_t,     u.i32,         arg_sign, data_size ) break;
		case 3  : M_VA_GET_IVAL_EX( vawp, int64_t,     u.i64,         arg_sign, data_size ) break;
		case 0  : /* fall */
		case 'h': M_VA_GET_IVAL(    vawp->ap_, int,         u.ival,        arg_sign, data_size ) break;
		case 'l': M_VA_GET_IVAL(    vawp->ap_, long,        u.lval,        arg_sign, data_size ) break;
		case '2': M_VA_GET_IVAL(    vawp->ap_, LongLong, u.llval,       arg_sign, data_size ) break;
		case 'j': M_VA_GET_IVAL(    vawp->ap_, intmax_t,    u.intmax_val,  arg_sign, data_size ) break;
		case 't': M_VA_GET_IVAL(    vawp->ap_, ptrdiff_t,   u.ptrdiff_val, arg_sign, data_size ) break;
		case 'z':
		{
			size_t size_val = va_arg( vawp->ap_, size_t );
			u.intmax_val = cast_to_intmax_t_from_size_t( size_val );
			M_GET_IVAL_SIGN( u.intmax_val, arg_sign );
			data_size = sizeof(size_t);
			break;
		}
		default:
			assert(0);
			break;
		}

	} else {  /* unsigned */

		/**
		 * 以下で * とは、u, o, x, Xなどの符号無し整数を表示するための指定子のいずれかで
		 * あるとする.
		 *
		 * %j*, %z* を認める.
		 * %t* (符号なしptrdiff_t)は、ptrdiff_t型を受け取ってそれを符号無し整数で表示するという
		 * 指示だが、これをどう解釈するか? そもそも ptrdiff_t型とは符号付きの型である.
		 * これを普通に符号付き10進数などで表示するならば、ユーザは %td などと指定すべきだが、
		 * ここではあえてそうせず %t* (*はd以外)と指定されている. これは ptrdiff_t 型を同型の
		 * unsigned ptrdiff_t 型とでも言うべき型 へキャストしたものを表示せよという意志と
		 * みなすことにする. 例えば ptrdiff_t の実体が int16_t の環境の場合で、渡された値が
		 * -2 であり、%tu と指定されている場合、ここは 65534 と表示される.
		 * これは %u とあるところへ signed int の負の値を指定した場合の挙動に準ずる.
		 * ptrdiff_t のサイズは一般にはわからないため、以下ではB1_M_CAST_TO_UINTを用いて
		 * 同型かつ異符号への抽象化されたキャストを実現している.
		 */
		switch (length_modifier) {
		case 1  : M_VA_GET_UVAL_EX( vawp, uint16_t,     u.u32,         arg_sign, data_size ) break;
		case 2  : M_VA_GET_UVAL_EX( vawp, uint32_t,     u.u32,         arg_sign, data_size ) break;
		case 3  : M_VA_GET_UVAL_EX( vawp, uint64_t,     u.u64,         arg_sign, data_size ) break;
		case 0  : /* fall */
		case 'h': M_VA_GET_UVAL(    vawp->ap_, unsigned int,  u.uval,        arg_sign, data_size ) break;
		case 'l': M_VA_GET_UVAL(    vawp->ap_, unsigned long, u.ulval,       arg_sign, data_size ) break;
		case '2': M_VA_GET_UVAL(    vawp->ap_, ULongLong,  u.ullval,      arg_sign, data_size ) break;
		case 'j': M_VA_GET_UVAL(    vawp->ap_, uintmax_t,     u.uintmax_val, arg_sign, data_size ) break;
		case 'z': M_VA_GET_UVAL(    vawp->ap_, size_t,        u.size_val,    arg_sign, data_size ) break;
		case 't':
		{
			ptrdiff_t ptrdiff_val = va_arg( vawp->ap_, ptrdiff_t );
			u.uintmax_val = cast_to_uintmax_t_from_ptrdiff_t( ptrdiff_val );
			M_GET_UVAL_SIGN( u.uintmax_val, arg_sign );
			data_size = sizeof(ptrdiff_t);
			break;
		}
		default:
			assert(0);
			break;
		}
	}

	{
		/**
		 * justify_left への対応も必要か...
		 */
	
		char   body[sizeof(uintmax_t)*16];
		size_t body_leng = setPrefix_bySignAndFlags( body, fmt_spec, arg_sign, pfg ) - body;
		size_t body_size = sizeof(body);

		ZnkToStrFlags flags = ZnkToStr_e_IS_SUPPLEMENT | ZnkToStr_e_IS_COMPACT;
		char space_ch = pfg->zero_padding ? '0' : ' ';

		char*  begin    = body + body_leng;
		size_t capacity = body_size - body_leng;
		size_t width = min_field_width;
		size_t radix = 10;
		bool   is_unsigned = true;

		if( space_ch == '0' ){
			flags |= ZnkToStr_e_IS_SIGN_TOP;
		}
	
		if (fmt_spec == 'p'){
			width = data_size * 2;
			radix = 16;
			is_unsigned = true;
	
		} else if (fmt_spec == 'd') {  /* signed */
			/**
			 * %d の場合、min_field_width == 0
			 */
			width = min_field_width;
			radix = 10;
			is_unsigned = false;
	
		} else { /* unsigned */
			/**
			 * %u 等の場合、min_field_width == 0
			 */
			width = min_field_width;
	
			/**
			 * radix調整.
			 */
			switch( fmt_spec ){
			case 'x':
				flags |= ZnkToStr_e_IS_LOWER;
				radix = 16;
				break;
			case 'X':
				flags &= ~ZnkToStr_e_IS_LOWER;
				radix = 16;
				break;
			case 'o':
				radix = 8;
				break;
			default:
				radix = 10;
				break;
			}
			is_unsigned = true;
		}

		/**
		 * BitExtend
		 */
		if( is_unsigned ){
			switch( data_size ){
			case 1: case 2: case 4:
				body_leng += ZnkToStr_U32_getStr( begin, capacity, u.u32, width, space_ch, radix, flags ); break;
			case 8:
				body_leng += ZnkToStr_U64_getStr( begin, capacity, u.u64, width, space_ch, radix, flags ); break;
			default:
				assert( 0 );
				break;
			}
		} else {
			switch( data_size ){
			case 1: case 2: case 4:
				body_leng += ZnkToStr_I32_getStr( begin, capacity, u.i32, width, space_ch, radix, flags ); break;
			case 8:
				body_leng += ZnkToStr_I64_getStr( begin, capacity, u.i64, width, space_ch, radix, flags ); break;
			default:
				assert( 0 );
				break;
			}
		}

		vsnp_ctx->add_func_( vsnp_ctx, body, body_leng );
		vsnp_ctx->pos_ += body_leng;
	}

	return true;
}


Znk_INLINE void
updatePrecision_byAlternateForm( PrecisionInfo* prec_info,
		bool alternate_form, char fmt_spec, size_t zeropad_pos,
		const char* body, size_t body_leng )
{
	/**
	 * 0埋めを除いた数字本体が来る桁数.
	 */
	const size_t digit_size = body_leng - zeropad_pos;

	if( alternate_form && fmt_spec == 'o'
#ifdef HPUX_COMPATIBLE
			/* ("%#.o",0) -> ""  */
			&& (body_leng > 0)
#endif

#ifdef DIGITAL_UNIX_BUG_COMPATIBLE
			/* ("%#o",0) -> "00" */
#else
			/**
			 * unless zero is already the first character
			 */
			&& !( zeropad_pos < body_leng && body[ zeropad_pos ] == '0' )
#endif
	){
		/**
		 * assure leading zero for alternate-form octal numbers
		 */
		if( !prec_info->specified || prec_info->size <= digit_size ){
			/**
			 * prec_info.size is increased to force the first character to be zero,
			 * except if a zero value is formatted with an explicit prec_info.size of zero
			 */
			prec_info->size      = digit_size + 1;
			prec_info->specified = true;
		}
	}
}

/*
 * endof GetBodyOfInteger
 **/
/*****************************************************************************/




/*****************************************************************************/
/**
 * GetBodyOfDouble
 */
Znk_INLINE void
supplement_forDecE( char* body, size_t body_size,
		size_t min_field_width, size_t* body_leng, char space_ch )
{
	if( min_field_width > 0 && min_field_width > *body_leng ){
		char* mid_ch;
		/**
		 * %e に min_field_width を与えることはないと思うが
		 * 与えた場合は、それは指数部までをすべて含めた全体の長さを示す.
		 * 従って、space_ch を埋める supplement_size は min_field_width - body_leng となる.
		 * また、Buf_memmove で body 内を memmove する処理が入るため、少し遅くなる.
		 */
		const size_t supplement_size = min_field_width - *body_leng;

		if( body_size ){ /* body_sizeが0の場合は省略することで高速化 */
			Buf_memmove( body, body_size, supplement_size, body, *body_leng+1 );
			Buf_memset( body, body_size, 0, space_ch, supplement_size );

			/**
			 * 符号文字はもしあるならばbodyの中間にあるはずである.
			 * (flagsによっては負でも付かない場合もあるし、正でも付く場合もある)
			 * 符号があり、space_ch == '0' のときはこれだとおかしいので、
			 * 符号と先頭の文字('0')と入れ替える
			 */
			mid_ch = body + supplement_size;
			if( space_ch == '0' &&
			  ( *mid_ch == '+' || *mid_ch == '-' || *mid_ch == ' ' ) ){
				Znk_SWAP( char, *mid_ch, body[0] );
			}
		}
		*body_leng += supplement_size;
	}
}

/**
 * buf 内を指すポインタ del_begin が指す文字から
 * del_end が指す文字の手前までを削除する.
 * 削除後の文字列の長さをbuf_lengに更新する.
 */
Znk_INLINE void
CStr_deletePart( char* buf, size_t buf_size, size_t* buf_leng, char* del_begin, char* del_end )
{
	char* end = buf + *buf_leng;
	size_t back_size = end - del_end; 
	size_t del_pos = del_begin - buf;
	Buf_memmove( buf, buf_size, del_pos, del_end, back_size );
	*buf_leng = del_pos + back_size;
}

Znk_INLINE bool
getBody_forDouble( ZnkVSNPrintfCtx* vsnp_ctx,
		double dval, size_t min_field_width, char fmt_spec,
		const PrecisionInfo* prec_info, char space_ch )
{
	/**
	 * 整数と実数オブジェクトが来た場合に使用する一時バッファ.
	 * temporary buffer for simple integer or double->cstring conversion
	 */
	char   body[ 512 ];
	size_t body_leng     = 0;
	size_t decimal_width = 0;
	char   sign_policy   = 'E';
	/**
	 * VCならばここは3
	 * GCCならばここは2
	 * BCCならばここは2
	 */
	size_t exp_width = 2;

	/**
	 * printfでは短いinteger部分に0が埋められることはあっても
	 * 長いintegerが切り捨てられることはない.
	 */
	ZnkToStrFlags flags = ZnkToStr_e_IS_SUPPLEMENT | ZnkToStr_e_IS_COMPACT |
		ZnkToStr_e_IS_EXP_PLUS | ZnkToStr_e_IS_EXP_ZERO_SIGN_PLUS |
		ZnkToStr_e_IS_EXP_SUPPLEMENT;
	if( space_ch == '0' ){ flags |= ZnkToStr_e_IS_SIGN_TOP; }

	switch( fmt_spec ){
	case 'f': case 'F':
	{
		size_t int_width;
		decimal_width = prec_info->specified ? prec_info->size : 6;
		int_width = (min_field_width == 0) ?
			0 :
			(min_field_width >= decimal_width + 1) ?
				min_field_width - (decimal_width + 1) :
				0;

		body_leng = ZnkToStr_Double_getStrF( body, sizeof(body), dval,
				int_width, decimal_width,
				space_ch, flags, sign_policy );

		//fprintf(stderr, "dval=[%e] f=[%s] body_leng=[%u] int_width=[%u] decimal_width=[%u]\n", dval, body, body_leng, int_width, decimal_width);
		break;
	}
	case 'E': case 'e':
	{
		if( fmt_spec == 'e' ){ flags |= ZnkToStr_e_IS_EXP_LOWER; }

		/**
		 * printfでは短いinteger部分に0が埋められることはあっても
		 * 長いintegerが切り捨てられることはない.
		 */
		decimal_width = prec_info->specified ? prec_info->size : 6;

		body_leng = ZnkToStr_Double_getStrE( body, sizeof(body), dval,
				decimal_width, exp_width, flags );

		supplement_forDecE( body, sizeof(body), min_field_width, &body_leng, space_ch );

		//fprintf(stderr, "dval=[%e] e=[%s] body_leng=[%u]\n", dval, body, body_leng);
		break;
	}
	case 'G': case 'g':
	{
		size_t int_width = 0;
		char selected_method;
		char* p;

		if( fmt_spec == 'g' ){ flags |= ZnkToStr_e_IS_EXP_LOWER; }

		/**
		 * %g では %f と精度の意味が異なり、整数幅と小数幅の和を示す.
		 * そのため、仮に%eと解釈されて仮数部(1)となった場合、
		 * 小数部のデフォルト幅は 6-1=5 ということになる.
		 */
		decimal_width = prec_info->specified ? prec_info->size : 6;

		body_leng = ZnkToStr_Double_getStrG_Ex( body, sizeof(body), dval,
				int_width, decimal_width, exp_width,
				space_ch, flags, sign_policy,
				&selected_method );

		//fprintf(stderr, "body=[%s], decimal_width=[%u] selected_method=[%c]\n", body, decimal_width, selected_method);
		/**
		 * この処理が必要な理由は以下による.
		 * まず %X.Yg とあった場合に これが %f と解釈された場合は
		 * Yは小数点以下の桁数ではなく、整数桁数+小数桁数の個数となる.
		 * 小数桁数は実際の整数桁数を求めてから出ないと決定できない.
		 * 実際の整数桁数は縮小されないが、小数桁数は整数桁数に応じて縮小されるからである.
		 * そのため、ここでは一旦Y(すなわちここでは decimal_width) だけ先頭からスキャンをかけ,
		 * body_leng を更新している.
		 */
		p = body;
		{
			size_t num = 0;
			/**
			 * 符号をスキップ
			 */
			if( *p == '+' || *p == '-' || *p == ' ' ){ ++p; }
			/**
			 * space_ch埋めをスキップ
			 * (もっとも、int_width=0としているため、これはないはず)
			 */
			while( *p == space_ch ){ ++p; }
			while( *p != '\0' && num < decimal_width ){
				if( *p == '.' ){ ++p; continue; }
				++p;
				++num;
			}
		}

		if( selected_method == 'f' || *p == '\0' ){
			body_leng = p - body;
		} else {
			/**
			 * selected_method == 'e' の場合、今の位置pから'e' または'E'が始まる手前までを
			 * 消去しなければならない.
			 */
			char* del_begin = p;
			while( *p != '\0' && *p != 'e' && *p != 'E' ){ ++p; }
			if( *p == '\0' ){
				/**
				 * 'e' or 'E' がない.
				 */
				body_leng = del_begin - body;
			} else {
				/**
				 * del_begin から p までを消去する. 
				 * 消去後の文字列長をbody_lengに更新する.
				 */
				CStr_deletePart( body, sizeof(body), &body_leng, del_begin, p );
			}

			//0123456789
			//...x..e...
		}
		supplement_forDecE( body, sizeof(body), min_field_width, &body_leng, space_ch );

		//fprintf(stderr, "dval=[%e] e=[%s] body_leng=[%u]\n", dval, body, body_leng);
		break;
	}
	case 'A': case 'a':
	{
		if( fmt_spec == 'a' ){ flags |= ZnkToStr_e_IS_LOWER; flags |= ZnkToStr_e_IS_EXP_LOWER; }
		if( !prec_info->specified ){ flags |= ZnkToStr_e_IS_FLEX_DECIMAL; }

		/**
		 * %a の場合だけなぜか指数部はGCC4,VC8ともにsupplementなし.
		 * (C99にそう定められている?)
		 * そのため、ここではexp_width=0を指定している.
		 */
		decimal_width = prec_info->specified ? prec_info->size : 0;

		body_leng = ZnkToStr_Double_getStrHexE( body, sizeof(body), dval,
				decimal_width, exp_width, flags );
		break;
	}
	default:
		assert(0);
		break;
	}//endof switch

	{
		vsnp_ctx->add_func_( vsnp_ctx, body, body_leng );
		vsnp_ctx->pos_ += body_leng;
	}
	return true;
}

/*
 * endof GetBodyOfDouble
 **/
/*****************************************************************************/




/*****************************************************************************/
/**
 * Znk_vsnprintf_general
 */

Znk_INLINE const char*
parsePercentFormat( const char* p, ZnkVSNPrintfCtx* vsnp_ctx, VAWrap* vawp )
{
#if defined(PERL_COMPATIBLE) || defined(LINUX_COMPATIBLE)
	const char *begin_p = p;
#endif
	PercentFlags pfg;
	size_t min_field_width;
	PrecisionInfo prec_info;
	int  length_modifier;
	char fmt_spec;

	/**
	 * オブジェクトが整数の場合のみ、0 以外の値に調整される場合がある.
	 * その他のオブジェクトの場合、基本的に zeropad.pos == 0, zeropad.size == 0 のままになる.
	 */
	ZeroPad zeropad;


	//fprintf(stderr, "p=[%s] pos=[%u]\n", p, pos);
	
	++p;  /* skip '%' */


	/**
	 * PercentFlags
	 */
	PercentFlags_clear( &pfg );
	p = PercentFlags_get( &pfg, p );


	/**
	 * MinFieldWidth
	 * %9d or %9f の 9 の部分
	 * デフォルト値(%dと記述した場合等)は 0 
	 */
	min_field_width = MinFieldWidth_get( &p, vawp, &pfg.justify_left );


	/**
	 * PrecisionInfo
	 * %6.3f の 3 の部分
	 */
	PrecisionInfo_clear( &prec_info );
	p = PrecisionInfo_get( &prec_info, p, vawp );


	/**
	 * LengthModifier
	 * %ld or %hd or %lld の l, h, ll の部分. 
	 * ll は 処理の便宜上、内部的に 2 に置換される.
	 */
	length_modifier = LengthModifier_get( &p );


	/**
	 * fmt_spec
	 * 例えば %10d の d の部分.
	 * (current conversion specifier character)
	 */
	fmt_spec = *p;

	/**
	 * common synonyms:
	 * modify fmt_spec and length_modifier by fmt_spec.
	 */
	{
		switch (fmt_spec) {
		case 'i': fmt_spec = 'd'; break;
		case 'D': fmt_spec = 'd'; length_modifier = 'l'; break;
		case 'U': fmt_spec = 'u'; length_modifier = 'l'; break;
		case 'O': fmt_spec = 'o'; length_modifier = 'l'; break;
		default: break;
		}
	}


	ZeroPad_clear( &zeropad );


	/**
	 * get parameter value, do initial processing
	 */
	switch (fmt_spec) {
	case '%': /* % behaves similar to 's' regarding flags and field widths */
	case 'c': /* c behaves similar to 's' regarding flags and field widths */
	case 's':
	{
		const char *body = NULL;
		size_t body_leng = 0;

		/**
		 * %c のときに使用する一時バッファ.
		 *
		 * unsigned char argument value - only defined for c conversion.
		 * N.B. standard explicitly states the char argument for the c conversion is unsigned
		 */
		unsigned char uchar_arg;

		length_modifier = 0;          /* wint_t and wchar_t not supported */

		/**
		 * the result of zero padding flag with non-numeric conversion specifier is undefined.
		 * Solaris and HPUX 10 does zero padding in this case, Digital Unix and Linux does not.
		 */
#if !defined(SOLARIS_COMPATIBLE) && !defined(HPUX_COMPATIBLE)
		pfg.zero_padding = false;    /* turn zero padding off for cstring conversions */
#endif

		body_leng = 1;
		switch (fmt_spec) {
		case '%':
			body = p;
			break;
		case 'c':
		{
			int ival = va_arg(vawp->ap_, int);
			/**
			 * standard demands unsigned char
			 * このスコープを出てもbodyは使用されるため、
			 * 必ず uchar_arg に一旦取得しなければならない.
			 */
			uchar_arg = M_STATIC_CAST(unsigned char, ival);
			body = M_REINTERPRET_CAST( const char*, &uchar_arg );
			break;
		}
		case 's':
			body = va_arg(vawp->ap_, const char *);
			if (!body){
				/**
				 * make sure not to address cstring beyond the specified precision !!!
				 */
				body_leng = 0;
			} else if (!prec_info.specified) {
				/**
				 * truncate cstring if necessary as requested by precision
				 */
				body_leng = strlen(body);
			} else if (prec_info.size == 0) {
				body_leng = 0;
			} else {
				body_leng = Buf_findByte( body, prec_info.size, '\0' );
			}
			break;
		default: break;
		}
		{
			vsnp_ctx->add_func_( vsnp_ctx, body, body_leng );
			vsnp_ctx->pos_ += body_leng;
		}

		break;
	}
	/*
	 * va_arg から non-POD 型を取得してはいけない(未定義の動作になる)
	 * 規格書の5.2.2 Function callの第7段落によると
	 * non-POD 型を可変引数に渡した時の振る舞いは未定義とされている.
	 * ( va_argではビット単位のコピーしか行われないため、non-PODではコピーが
	 * 失敗する可能性がある. )
	 *
	 * VCの場合これが素通りし、期待通り動作もしてしまうが完全に処理系依存である.
	 * g++ の場合、このような記述をすると、コンパイル時に以下のような警告が出る.
	 * warning: cannot receive objects of non-POD type
	 * `const struct std::string' through `...'; call will abort at runtime
	 * さらに実際上の警告通り、実行時に Abortする.
	 *
	 * std::basic_string に %S を対応させようと考えていたが、これは不可能である.
	 **/

	case 'd': case 'u':
	case 'o':
	case 'x': case 'X':
	case 'p':
	{
		getBody_forInteger( vsnp_ctx,
				length_modifier, fmt_spec, min_field_width,
				vawp, &pfg );
		break;
	}

	case 'f': case 'F':
	case 'e': case 'E':
	case 'g': case 'G':
	case 'a': case 'A':
	{
		double dval;
		if(length_modifier == 'L') {
			/**
			 * long doubleは今はサポートしない.
			 */
			dval = M_P_VA_ARG_AS_TYPE( double, vawp->ap_, long double );
		} else {
			/***
			 * C言語仕様「可変引数での float=>double標準変換」により、floatの場合は考慮不要.
			 */
			dval = va_arg(vawp->ap_, double);
		}

		getBody_forDouble( vsnp_ctx,
				dval, min_field_width, fmt_spec,
				&prec_info, (char)(pfg.zero_padding ? '0' : ' ') );
		break;
	}

	default:
	{
		/*
		 * default of fmt_spec switch:
		 * unrecognized conversion specifier, keep format cstring as-is
		 **/
		const char *body = NULL;
		size_t body_leng = 0;

		/**
		 * turn zero padding off for non-numeric convers.
		 */
		pfg.zero_padding = false;

#ifndef DIGITAL_UNIX_COMPATIBLE
		/**
		 * reset flags
		 */
		pfg.justify_left = true;
		min_field_width  = 0;
#endif

#if defined(PERL_COMPATIBLE) || defined(LINUX_COMPATIBLE)
		/**
		 * keep the entire format cstring unchanged.
		 * well, not exactly so for Linux, which does something inbetween,
		 * and I don't feel an urge to imitate it: "%+++++hy" -> "%+y"
		 */
		body      = begin_p;
		body_leng = p - begin_p;
#else
		/**
		 * discard the unrecognized conversion,
		 * just keep the unrecognized conversion character.
		 */
		body      = p;
		body_leng = 0;
#endif

		/**
		 * include invalid conversion specifier unchanged
		 * if not at end-of-cstring
		 */
		if( *p ){ ++body_leng; }

		{
			vsnp_ctx->add_func_( vsnp_ctx, body, body_leng );
			vsnp_ctx->pos_ += body_leng;
		}

		break;
	}
	} // endof switch

	/**
	 * まだ次の文字列が続くようなら
	 * fmt_spec の次の文字を指すように p を進めておく.
	 * step over the just processed conversion specifier
	 * if not at end-of-cstring
	 */
	if( *p ){ ++p; }

	return p;
}


bool
Znk_vsnprintf_general( ZnkVSNPrintfCtx* vsnp_ctx, const char* fmt, va_list ap )
{
#if defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
	/***
	 * このposの値は最終的にはfmtを展開して作られる文字列が何byteになるかを示すものとなる.
	 * よってbuf_sizeの制限とは基本的に関係がない.
	 */
	//size_t pos = 0;

	const char* p = fmt;
	const char* q = NULL;
	size_t copy_size   = 0;
	size_t copied_size = 0;
	VAWrap vawp;
	M_P_VA_COPY( vawp.ap_, ap );

	/**
	 * In contrast with POSIX, the ISO C99 now says
	 * that buf can be NULL and buf_size can be 0.
	 * This is more useful than the old:  if (buf_size < 1) return -1;
	 */
	if (!p) p = "";

	while( *p ){
		if( *p != '%' ){
			/**
			 * if (pos < buf_size) buf[pos++] = *p++;    -- this would be sufficient
			 * but the following code achieves better performance for cases
			 * where format cstring is long and contains few conversions
			 */
			q = strchr(p+1,'%');

			if( q == NULL ){
				/***
				 * p以降、もはや%は出現しない.
				 * すなわち残りをそのままコピーして終了でよい.
				 */
				copy_size = strlen(p);
				/***
				 * ここで必要とされるposは見込みのサイズであり、bufに実際何byteコピーされたかは関係がない.
				 * よってうっかり pos += Buf_memcpy( ... ) などとしてはいけない.
				 */
				vsnp_ctx->add_func_( vsnp_ctx, p, copy_size );
				vsnp_ctx->pos_ += copy_size;
				break;
			}

			/***
			 * %が出現する一つ手前までをそのままコピーする.
			 */
			copy_size = q - p;
			copied_size = vsnp_ctx->add_func_( vsnp_ctx, p, copy_size );

			/***
			 * ZnkVSNPrintfCtxの実装に不具合が存在する場合、
			 * この逆転現象が発生するかもしれない.
			 */
			assert( copied_size <= copy_size );

			/***
			 * ここで必要とされるposは見込みのサイズであり、bufに実際何byteコピーされたかは関係がない.
			 * よってうっかり pos += copied_size などとしてはいけない.
			 */
			vsnp_ctx->pos_ += copy_size;
			p   += copy_size;

			/***
			 * この時点で copied_size < copy_size の場合:
			 *   buf_size制限にひっかかっている.
			 *   しかしながらここでbreakなどしてはならない.
			 *   中断するとposの値も未完成のまま終わってしまう.
			 *
			 * この時点で copied_size == copy_size の場合:
			 *   %の一つ手前までのコピーが完了している.
			 *   この時点で p は必ず次の % を指しているはずである.
			 *   (qが非NULLであったため、pがNULL文字を指している可能性はない)
			 *
			 * 結局、いずれの場合でも何もせず、そのままparsePercentFormatへ移行すればよい.
			 */
		}

		p = parsePercentFormat( p, vsnp_ctx, &vawp );
	}
	

	if( vsnp_ctx->terminate_null_func_ ){
		vsnp_ctx->terminate_null_func_( vsnp_ctx );
	}

	/**
	 * Return the number of characters formatted (excluding trailing null
	 * character), that is, the number of characters that would have been
	 * written to the buffer if it were large enough.
	 *
	 * The value of pos should be returned,
	 * but pos is of unsigned type size_t, and snprintf is int,
	 * possibly leading to an undetected integer overflow,
	 * resulting in a negative return value, which is illegal.
	 * Both XSH5 and ISO C99 (at least the draft) are silent on this issue.
	 * Should errno be set to EOVERFLOW and EOF returned in this case???
	 */
	//return (int) pos;
	return true;
#if defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif
}
/*
 * endof Znk_vsnprintf_general
 **/
/*****************************************************************************/


struct BufAndSizeInfo {
	char*   buf_ref_;
	size_t  buf_size_;
};
static size_t
My_memcpy( ZnkVSNPrintfCtx* vsnp_ctx, const void* src, size_t req_size )
{
	struct BufAndSizeInfo* my_info = (struct BufAndSizeInfo*)vsnp_ctx->user_info_;
	return Buf_memcpy( my_info->buf_ref_, my_info->buf_size_, vsnp_ctx->pos_, src, req_size );
}
static void
My_terminate_null( ZnkVSNPrintfCtx* vsnp_ctx )
{
	struct BufAndSizeInfo* my_info = (struct BufAndSizeInfo*)vsnp_ctx->user_info_;
	/**
	 * make sure the cstring is null-terminated
	 * even at the expense of overwriting the last character
	 * (shouldn't happen, but just in case)
	 */
	my_info->buf_ref_[ Znk_MIN(vsnp_ctx->pos_, my_info->buf_size_-1) ] = '\0';
}

int
Znk_vsnprintf_C99( char* buf, size_t buf_size, const char* fmt, va_list ap )
{
	struct BufAndSizeInfo my_info;
	ZnkVSNPrintfCtx vsnp_ctx;

	my_info.buf_ref_  = buf;
	my_info.buf_size_ = buf_size;

	vsnp_ctx.user_info_ = &my_info;
	vsnp_ctx.add_func_  = My_memcpy;
	vsnp_ctx.terminate_null_func_ = My_terminate_null;
	vsnp_ctx.pos_ = 0;

	Znk_vsnprintf_general( &vsnp_ctx, fmt, ap );
	return (int)vsnp_ctx.pos_;
}

