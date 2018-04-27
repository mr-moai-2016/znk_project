#ifndef INCLUDE_GUARD__Znk_base_h__
#define INCLUDE_GUARD__Znk_base_h__

/***
 * When using winsock2,h on DMC, you have to: #define _WINSOCKAPI_ 
 * Before you include anything to prevent winsock.h from inclusion as that creates conflicts.
 * ( http://www.digitalmars.com/d/archives/c++/idde/326.html )
 */
#if defined(__DMC__)
/* Prevent winsock.h #include's. */
#  define _WINSOCKAPI_
#endif

/* size_tなどのC共通型の宣言 */
#include <stddef.h>


/**
 * C++においてincludeされた場合のみシンボルのマングリングを抑制.
 * これによりCのモジュールをC++からでも使用可能とする.
 */
#ifdef __cplusplus
#  if !defined(Znk_EXTERN_C_BEGIN) || !defined(Znk_EXTERN_C_END)
#    define Znk_EXTERN_C_BEGIN extern "C" {
#    define Znk_EXTERN_C_END   } 
#  endif
#else
#  define Znk_EXTERN_C_BEGIN
#  define Znk_EXTERN_C_END
#endif



/***
 * Suppressing Warning
 */
#if defined(__GNUC__)
/* GCC */
#elif defined(_MSC_VER)
#  if _MSC_VER < 1300
/* VC6.0 */
#    pragma warning(disable: 4786)
#    pragma warning(disable: 4284)
#  endif

#  pragma warning(disable: 4127) /* for while(1) etc. */
#  pragma warning(disable: 4100)
#  pragma warning(disable: 4054)
#  pragma warning(disable: 4055)

#  if _MSC_VER == 1400
/* VC8.0(VS2005) */
#    pragma warning(disable: 4819)
#  endif

#  if _MSC_VER >= 1400
#    pragma warning(disable: 4996)
#  endif

/* for WP64 MISGUIDE */
#  pragma warning(disable: 4267)

#elif defined(__BORLANDC__)
#  pragma warn -8008
#  pragma warn -8057
#  pragma warn -8004

#  if !defined(__cplusplus)
#    pragma warn -8080
#  endif

#else
#endif



#if defined(_MSC_VER)
#  if _MSC_VER >= 1300
#    define Znk_RECENT_VC
#    if _MSC_VER < 1500 && defined(_WIN32)
       /* 32bitコンパイラでの /Wp64 misguide 発生の可能性を考慮する */
#      define Znk_VC_WP64_MISGUIDE 1
#    endif
#  else
#    define Znk_OLD_VC /* VC6.0のみ */
#  endif
#endif


/* Target OS */
#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER) || \
	defined(__MINGW32__) || defined(__MINGW64__) || \
	defined(__BORLANDC__) || defined(__DMC__)
/* Windows API */
#  define Znk_TARGET_WINDOWS 1

#elif defined(__unix__) || defined(__linux__) || defined(__CYGWIN__) || \
	defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
/* POSIX UNIX + X11 およびその互換システム */
#  define Znk_TARGET_UNIX 1

#else
#  error "Not supported OS"

#endif


/* Target CPU BIT */
#if   defined(_WIN64) || defined(__x86_64__) || defined(__MINGW64__) || defined(__aarch64__) || defined(__mips64)
#  define Znk_CPU_BIT 64
#elif defined(_WIN32) || defined(__i386__)   || defined(__MINGW32__) || defined(__arm__) || defined(__mips__)
#  define Znk_CPU_BIT 32
#else
#  error "Not supported CPU bit"
#endif


#if __STDC_VERSION__ >= 199901L
#  ifndef   Znk_C99_SUPPORTED
#    define Znk_C99_SUPPORTED 1
#  endif
#endif


/**
 * @brief
 * Znk_int64_t Znk_uint64_t
 * ここでは long long や __int64型などを用いる.
 * いきなりint64_t,uint64_tを定義せず、まず独自名としてtypedefする.
 * これはstding.hのinclude有無によって型名衝突するか否かの判定を
 * できるだけ一箇所に集約したいからである.
 *
 * GCC
 *   long long は使用可能と考えてよい.
 *   (拡張無効オプションが設定されていてもコンパイルが通るように
 *   __extension__を付けてtypedefしてある.)
 *
 * VC
 *   long long はVC7.0以降から使用可能
 *   (VC6.0では使用不可. 替わりに__int64を使う).
 *
 * BorlandC++5.5
 *   long long は使用不可. 替わりに__int64を使う.
 *
 * DMC
 *   long long は使用可能.
 *
 * その他の環境
 *   とりあえずlong longは可能とみなし、さらにそれは8byte(64bit)整数とみなす.
 */
#if !defined(Znk_C99_SUPPORTED)
#  if defined(__GNUC__)
__extension__ typedef           long long  Znk__int64_t__;
__extension__ typedef  unsigned long long  Znk__uint64_t__;

#  elif defined(__BORLANDC__) || (defined(_MSC_VER) && _MSC_VER < 1300)
typedef           __int64  Znk__int64_t__;
typedef  unsigned __int64  Znk__uint64_t__;

#  else
typedef           long long  Znk__int64_t__;
typedef  unsigned long long  Znk__uint64_t__;

#  endif
#endif


/***
 * stdint.h系の整数typedef
 */
#if defined(Znk_C99_SUPPORTED)
#  include <stdint.h>

#elif defined(__GNUC__) || defined(__DMC__)
#  include <stdint.h>

#elif defined(_MSC_VER) && _MSC_VER >= 1600
#  include <stdint.h>

#elif ( defined(_MSC_VER) && _MSC_VER < 1600 ) || defined(__BORLANDC__)
typedef signed char     int8_t;
typedef unsigned char   uint8_t;
typedef short           int16_t;
typedef unsigned short  uint16_t;
typedef int             int32_t;
typedef unsigned int    uint32_t;
typedef Znk__int64_t__  int64_t;
typedef Znk__uint64_t__ uint64_t;
typedef int64_t         intmax_t;
typedef uint64_t        uintmax_t;
#  if Znk_CPU_BIT == 64
typedef uint64_t uintptr_t;
typedef  int64_t  intptr_t;
#  elif Znk_CPU_BIT == 32
typedef uint32_t uintptr_t;
typedef  int32_t  intptr_t;
#  endif

#endif


#if  defined(__BORLANDC__) || defined(__WATCOMC__) \
  || defined(Znk_OLD_VC) \
  || defined (__alpha) || defined (__DECC)
#  ifndef   UINT64_C
#    define UINT64_C(num) num##UI64
#  endif
#  ifndef   INT64_C
#    define INT64_C(num)  num##I64
#  endif
#else
#  ifndef   UINT64_C
#    define UINT64_C(num) num##ULL
#  endif
#  ifndef   INT64_C
#    define INT64_C(num)  num##LL
#  endif
#endif

#ifndef   INTMAX_C
#  define INTMAX_C(num)  INT64_C(num)
#endif
#ifndef   UINTMAX_C
#  define UINTMAX_C(num) UINT64_C(num)
#endif


/***
 * bool型および定数true/false
 */
typedef int8_t Znk__bool__;

#if defined(Znk_C99_SUPPORTED)
#  include <stdbool.h>

#elif defined(__GNUC__) || defined(__DMC__)
#  include <stdbool.h>

#elif !defined(__bool_true_false_are_defined) && !defined(__cplusplus)
#  if !defined(bool)
#    define bool  Znk__bool__
#  endif
#  if !defined(true)
#    define true 1
#  endif
#  if !defined(false)
#    define false 0
#  endif
#  define __bool_true_false_are_defined	1
#endif


/**
 * Keyword inline
 */
#if ( !defined(__cplusplus) && !defined(inline) )
#  if defined(Znk_C99_SUPPORTED)
/* inline キーワードはすでに存在する */
#  elif defined(_MSC_VER) || defined(__BORLANDC__)
#    define inline __inline
#  elif defined(__GNUC__)
#    define inline __inline__
#  elif defined(__DMC__)
     /* DMCではinlineが最初から定義されている(C言語でもデフォルトで使える) */
#  else
#    define inline /* none */
#  endif
#endif

#define Znk_INLINE static inline


#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/* nothing */
#elif defined(__GNUC__)
#  if  (__GNUC__ >= 3)
#    define restrict __restrict
#  else
#    define restrict /**/
#  endif
#else
#  define restrict /**/
#endif

#if defined(__GNUC__) && (__GNUC__ >= 3)
#  define Znk_ATTRIBUTE(x) __attribute__(x)
#else
#  define Znk_ATTRIBUTE(x) /**/
#endif

/* This is not going to work if va_list is interesting.  But then, if you
   have a non-trivial implementation of va_list, you should have va_copy. */
#ifndef va_copy
#define va_copy(a, b) do { a = b; } while(0)
#endif



/* 使われていない変数に対する警告を特別に抑制したい場合に使う. */
#define Znk_UNUSED( param ) (void)(param);

/* 配列の要素数を取得 */
#define Znk_NARY( ary )  ( sizeof(ary)/sizeof((ary)[0]) )

/* 汎用Swap */
#define Znk_SWAP(type, a, b) do{type w; w=(a); (a)=(b); (b)=w;}while(0)

/**
 * @brief
 *   size_t 型における特殊値として使う.
 *   std::string::nposのような使い方を想定する.
 */
#define Znk_NPOS ((size_t)(-1))


Znk_INLINE size_t
Znk_clampSize( const size_t size, const size_t max_size ){
	return ( size > max_size || size == Znk_NPOS ) ? max_size : size;
}
/**
 * @brief
 *  LittleEndianな環境か否かを返す.
 */
Znk_INLINE bool
Znk_isLE( void ){
	const int i = 1;
	return (bool)( ((const uint8_t*)&i)[0] != 0 );
}

Znk_INLINE void
Znk_swap2byte( uint8_t* p ){ 
	register uint8_t t;
	t = p[1]; p[1] = p[0]; p[0] = t;
}
Znk_INLINE void
Znk_swap3byte( uint8_t* p ){ 
	register uint8_t t;
	t = p[2]; p[2] = p[0]; p[0] = t;
}
Znk_INLINE void
Znk_swap4byte( uint8_t* p ){ 
	register uint8_t t;
	t = p[3]; p[3] = p[0]; p[0] = t;
	t = p[2]; p[2] = p[1]; p[1] = t;
}
Znk_INLINE void
Znk_swap8byte( uint8_t* p ){ 
	register uint8_t t;
	t = p[7]; p[7] = p[0]; p[0] = t;
	t = p[6]; p[6] = p[1]; p[1] = t;
	t = p[5]; p[5] = p[2]; p[2] = t;
	t = p[4]; p[4] = p[3]; p[3] = t;
}
Znk_INLINE void Znk_swapU16( uint16_t* val ){ Znk_swap2byte( (uint8_t*)val ); }
Znk_INLINE void Znk_swapU32( uint32_t* val ){ Znk_swap4byte( (uint8_t*)val ); }
Znk_INLINE void Znk_swapU64( uint64_t* val ){ Znk_swap8byte( (uint8_t*)val ); }

/**
 * @brief
 *   コード上のシンボルを文字列にしたものと結びつける.
 *   例えばswitch文の中などで、symbol を文字列化したものstrへ代入するといった処理を
 *   書く場合、通常は case symbol: str="symbol"; break; と書かねばならず冗長である.
 *   これを使えば、case Znk_BIND_STR( : str=, symbol ) break; と書くことができ、
 *   symbolの登場を１回のみに抑えられる.
 */
#define Znk_BIND_STR( btwn, symbol ) symbol btwn #symbol
#define Znk_TOSTR_CASE( type ) case Znk_BIND_STR( :return, type )
#define Znk_TO_STR( symbol ) #symbol

#define Znk_DELETE_PTR( ptr, ptr_deleter, null_ptr ) \
	( ( (ptr) != (null_ptr) ) ? ( ptr_deleter( ptr ), (ptr) = (null_ptr) ) : (null_ptr) )

/**
 * @brief
 *   一般的なハンドルを定義する.
 *   これはあるモジュール等で使用するポインタのためのユニークな型を定義する.
 *   モジュールの実装側は実質キャストでハンドルと真の型の変換を行う.
 *   またユーザに公開するインターフェースでは真の型ではなくこのハンドラを提供する
 *   といった使い方が一般的な使用方法である.
 *
 *   例:
 *   Znk_DECLARE_HANDLE( MyWindow )
 *   ...以下、MyWindowという型名が(ポインタ型として)使える.
 */
#define Znk_DECLARE_HANDLE( id_name ) \
struct id_name##_tag { int unused; }; typedef struct id_name##_tag* id_name;

#if defined(Znk_TARGET_WINDOWS)

#  define WIN32_LEAN_AND_MEAN
#  ifndef   NOMINMAX
#    define NOMINMAX
#  endif

/* Windows98以降は必須とする. */
#  define WINVER 0x0410

#endif

/**
 * @brief
 *  Znk_LongLong Znk_ULongLong
 *  (unsigned) long long (標準ではC99以降) ( Compiler Intrinsic )
 *  ここではこれを抽象して Znk_LongLong, Znk_ULongLong を定義する.
 */
#if defined(Znk_C99_SUPPORTED)
typedef           long long  Znk_LongLong;
typedef  unsigned long long  Znk_ULongLong;

#else
#  if defined(__GNUC__)
__extension__ typedef           long long  Znk_LongLong;
__extension__ typedef  unsigned long long  Znk_ULongLong;

#  elif defined(__BORLANDC__) || defined(Znk_OLD_VC) || defined(__WATCOMC__) || defined(__alpha) || defined(__DECC)
/* 古いコンパイラでは long long が存在しないので替わりに__int64を使う. */
typedef           __int64  Znk_LongLong;
typedef  unsigned __int64  Znk_ULongLong;

#  else
typedef           long long  Znk_LongLong;
typedef  unsigned long long  Znk_ULongLong;

#  endif
#endif


/**
 * @brief
 *  Znk_PFMD_64 Znk_PFMD_MAX
 *  long long printfフォーマット(標準ではC99以降)
 *  例えば次のようにして使用すればよい.
 *
 *   int64_t val;
 *   printf( "val=%08" Znk_PFMD_64 "d", val );
 *
 *   uint64_t val;
 *   printf( "val=%08" Znk_PFMD_64 "u", val );
 *
 *   intmax_t val;
 *   printf( "val=%08" Znk_PFMD_MAX "d", val );
 *
 *   uintmax_t val;
 *   printf( "val=%08" Znk_PFMD_MAX "u", val );
 *
 */
#if defined(_MSC_VER) || defined(__BORLANDC__)
#  define Znk_PFMD_64  "I64"
#  define Znk_PFMD_MAX "I64"
#else
#  define Znk_PFMD_64  "ll"
#  define Znk_PFMD_MAX "ll"
#endif

#define Znk_force_ptr_cast( dst_int_type, ptr ) ((dst_int_type)(uintptr_t)(ptr))

#define Znk_FUNCARG_INIT( fnca, func, arg ) ( (fnca).func_=(func), (fnca).arg_=(arg) )

#endif /* INCLUDE_GUARD */
