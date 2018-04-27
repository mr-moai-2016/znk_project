#include "Znk_s_base.h"
#include "Znk_mem_find.h"
#include "Znk_stdc.h"
#include "Znk_def_util.h"
#include "Znk_s_posix.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define DECIDE_STRLENG( str_leng, str ) Znk_setStrLen_ifNPos( &str_leng, str )

char*
ZnkS_concatEx( char* buf, size_t buf_size, const char* cstr, size_t cstr_leng )
{
	size_t dst_len;
	if( buf_size == 0 || buf == NULL ){ return NULL; }
	dst_len = ZnkS_strnlen( buf, buf_size );
	if( dst_len == buf_size ){ return NULL; }

	if( cstr_leng < buf_size - dst_len ){
		memmove( buf + dst_len, cstr, cstr_leng );
		buf += ( dst_len + cstr_leng );
		*buf = '\0';
		return buf;
	}
	/* trancate over parts */
	memmove( buf + dst_len, cstr, buf_size - 1 - dst_len );
	buf += buf_size - 1;
	*buf = '\0';
	return buf;
}
char*
ZnkS_concat( char* buf, size_t buf_size, const char* cstr )
{
	return ZnkS_concatEx( buf, buf_size, cstr, Znk_strlen( cstr ) );
}

void
ZnkS_copy( char* buf, size_t buf_size, const char* cstr, size_t cstr_leng )
{
	if( buf_size ){
		const size_t cpy_size = ( cstr_leng == Znk_NPOS ) ?
			Znk_MIN( Znk_strlen(cstr), buf_size-1 ) :
			Znk_MIN( cstr_leng, buf_size-1 ) ;
		memmove( buf, cstr, cpy_size );
		buf[ cpy_size ] = '\0';
	}
}

/***
 * va_start <=> va_end 間における複数回呼び出しに対応.
 */
int
ZnkS_vsnprintf_sys__( char* buf, size_t buf_size, const char* fmt, va_list ap )
{
	int str_len;
	va_list ap_copy;

#ifdef _MSC_VER
	if( buf_size == 0 ){
		if( Znk_strchr( fmt, '%' ) ){
			return Znk_strlen( fmt ) + 512;
		}
		return Znk_strlen( fmt );
	}
#endif

	/**
	 * ap_copyを使うことによって, 
	 * 外部からZnkS_vsnprintf_sysを複数回呼び出すことを可能にする.
	 */
	va_copy( ap_copy, ap );

	/**
	 * ●VC の _vsnprintf の場合:
	 * buf_size == 0 の場合
	 *   -1 が返る.
	 *   buf に一切触れずなにもしない.
	 * str_len < buf_size の場合
	 *   str_len が返る.
	 *   buf[ str_len ] の位置にNULL文字が代入される.
	 * str_len == buf_size の場合
	 *   str_len が返る(-1ではない).
	 *   この場合NULL終端を行わない.
	 * str_len > buf_size の場合
	 *   -1が返る.
	 *   この場合NULL終端を行わない.
	 *
	 *
	 * ●GCC(or BSD) の vsnprintf の場合:
	 * C99と厳密に一致する.
	 * buf_size == 0 の場合
	 *   str_len が返る.
	 *   buf に一切触れずなにもしない.
	 * str_len < buf_size の場合
	 *   str_len が返る.
	 *   buf[ str_len ] の位置にNULL文字が代入される.
	 * str_len >= buf_size の場合
	 *   str_lenが返る.
	 *   buf[ buf_size-1 ] の位置にNULL文字が代入される.
	 * 内部エラーが発生した場合、-1 を返すこともあり得る.
	 *
	 */
#ifdef _MSC_VER
	str_len = _vsnprintf(buf, buf_size, fmt, ap_copy);
#else
	str_len = vsnprintf(buf, buf_size, fmt, ap_copy);
#endif

	va_end( ap_copy );

#ifdef _MSC_VER
	if( str_len < 0 || (size_t)str_len == buf_size ){
		/**
		 * VCの_vsnprintf に備えてNULL終端させる.
		 */
		buf[ buf_size-1 ] = '\0';
		str_len = Znk_strlen( fmt ) + 512;
		if( buf_size*2 > (size_t)str_len ){
			str_len = buf_size*2;
		}
	}
#endif
	return str_len;
}

int
ZnkS_snprintf_sys__( char* buf, size_t buf_size, const char* fmt, ... )
{
	int str_len;
	va_list ap;
	va_start(ap, fmt);
	str_len = ZnkS_vsnprintf_sys__( buf, buf_size, fmt, ap );
	va_end(ap);
	return str_len;
}

/***
 * vscanfは多くの処理系で提供されているものの、標準ではない.
 * ただしC99からは標準となった.
 * これに相当する関数を提供するかどうかは現在未定.
 */
bool
ZnkS_getIntD( int* ans, const char* str )
{
	return (bool)( sscanf( str, "%d", ans ) == 1 );
}
bool
ZnkS_getIntX( int* ans, const char* str )
{
	/***
	 * 16進数
	 * printfでの%xと異なり、unsigned int* ではなくint*であることに注意する.
	 * (K&R 308P Table B-2)
	 */
	return (bool)( sscanf( str, "%x", ans ) == 1 );
}
bool
ZnkS_getIntO( int* ans, const char* str )
{
	/***
	 * 8進数
	 * printfでの%oと異なり、unsigned int* ではなくint*であることに注意する.
	 * (K&R 308P Table B-2)
	 */
	return (bool)( sscanf( str, "%o", ans ) == 1 );
}

bool
ZnkS_getI16D( int16_t* ans, const char* str )
{
	return (bool)( sscanf( str, "%hd", ans ) == 1 );
}
bool
ZnkS_getI32D( int32_t* ans, const char* str )
{
	return (bool)( sscanf( str, "%d", ans ) == 1 );
}
bool
ZnkS_getI64D( int64_t* ans, const char* str )
{
#if defined(__MINGW32__)
	/***
	 * MinGWでのscanfは標準ライブラリをstatic-linkした場合はLinux系相当の ll
	 * msvcrt.dllを動的リンクした場合はWindows相当の I64 の指定となり
	 * Znk_PFMD_64すら使えず厄介である. よってこの場合はstrtoll/strtoull系を使う.
	 * (...というか全環境でこれでもよい気もするがとりあえず)
	 */
	char* end = NULL;
	*ans = strtoll( str, &end, 10 );
	return (bool)( str != end );
#elif defined(__GNUC__)
	/* for -Wformat warning */
	Znk_LongLong lld;
	if( sscanf( str, "%" Znk_PFMD_64 "d", &lld ) == 1 ){
		*ans = lld;
		return true;
	}
	return false;
#else
	return (bool)( sscanf( str, "%" Znk_PFMD_64 "d", ans ) == 1 );
#endif
}

bool
ZnkS_getU16U( uint16_t* ans, const char* str )
{
	return (bool)( sscanf( str, "%hu", ans ) == 1 );
}
bool
ZnkS_getU32U( uint32_t* ans, const char* str )
{
	return (bool)( sscanf( str, "%u", ans ) == 1 );
}
bool
ZnkS_getU64U( uint64_t* ans, const char* str )
{
#if defined(__MINGW32__)
	/***
	 * MinGWでのscanfは標準ライブラリをstatic-linkした場合はLinux系相当の ll
	 * msvcrt.dllを動的リンクした場合はWindows相当の I64 の指定となり
	 * Znk_PFMD_64すら使えず厄介である. よってこの場合はstrtoll/strtoull系を使う.
	 * (...というか全環境でこれでもよい気もするがとりあえず)
	 */
	char* end = NULL;
	*ans = strtoull( str, &end, 10 );
	return (bool)( str != end );
#elif defined(__GNUC__)
	/* for -Wformat warning */
	Znk_LongLong llu;
	if( sscanf( str, "%" Znk_PFMD_64 "u", &llu ) == 1 ){
		*ans = llu;
		return true;
	}
	return false;
#else
	return (bool)( sscanf( str, "%" Znk_PFMD_64 "u", ans ) == 1 );
#endif
}

bool
ZnkS_getU16X( uint16_t* ans, const char* str )
{
	return (bool)( sscanf( str, "%hx", ans ) == 1 );
}
bool
ZnkS_getU32X( uint32_t* ans, const char* str )
{
	return (bool)( sscanf( str, "%x", ans ) == 1 );
}
bool
ZnkS_getU64X( uint64_t* ans, const char* str )
{
#if defined(__MINGW32__)
	/***
	 * MinGWでのscanfは標準ライブラリをstatic-linkした場合はLinux系相当の ll
	 * msvcrt.dllを動的リンクした場合はWindows相当の I64 の指定となり
	 * Znk_PFMD_64すら使えず厄介である. よってこの場合はstrtoll/strtoull系を使う.
	 * (...というか全環境でこれでもよい気もするがとりあえず)
	 */
	char* end = NULL;
	*ans = strtoull( str, &end, 16 );
	return (bool)( str != end );
#elif defined(__GNUC__)
	/* for -Wformat warning */
	Znk_LongLong llx;
	if( sscanf( str, "%" Znk_PFMD_64 "x", &llx ) == 1 ){
		*ans = llx;
		return true;
	}
	return false;
#else
	return (bool)( sscanf( str, "%" Znk_PFMD_64 "x", ans ) == 1 );
#endif
}
bool
ZnkS_getSzU( size_t* ans, const char* str )
{
	bool result = false;

#if   Znk_CPU_BIT == 64
	uint64_t tmp = 0;
	result = ZnkS_getU64U( &tmp, str );
#elif Znk_CPU_BIT == 32
	uint32_t tmp = 0;
	result = ZnkS_getU32U( &tmp, str );
#else
	/* not support */
	size_t tmp = 0;
#endif
	*ans = (size_t)tmp;
	return result;
}
bool
ZnkS_getSzX( size_t* ans, const char* str )
{
	bool result = false;

#if   Znk_CPU_BIT == 64
	uint64_t tmp = 0;
	result = ZnkS_getU64X( &tmp, str );
#elif Znk_CPU_BIT == 32
	uint32_t tmp = 0;
	result = ZnkS_getU32X( &tmp, str );
#else
	/* not support */
	size_t tmp = 0;
#endif
	*ans = (size_t)tmp;
	return result;
}
bool
ZnkS_getReal( double* ans, const char* str )
{
	return (bool)( sscanf( str, "%lf", ans ) == 1 );
}
bool
ZnkS_getRealF( float* ans, const char* str )
{
	return (bool)( sscanf( str, "%f", ans ) == 1 );
}
bool
ZnkS_getBool( const char* str )
{
	if( !ZnkS_empty(str) ){
		const size_t size = Znk_strlen(str);
		switch( size ){
		case 1: return (bool)( str[0] == 'T' || str[0] == '1' );
		case 4: return (bool)( strcmp( str, "true" ) == 0 );
		default: break;
		}
	}
	return false;
}
bool
ZnkS_getPtrX( void** ans, const char* str )
{
	/***
	 * %pは処理系によって表記が色々と変わる可能性があり、
	 * これが出力する形式はポータブルなパッキング文字列としては使えない側面がある.
	 * ましてやsscanfの%pとなるとかなり怪しいものになってくる可能性があり、
	 * ここでは替わりにより確実な%xを用いた実装を採用する.
	 */
	bool result = false;
#if   Znk_CPU_BIT == 64
	uint64_t tmp = 0;
	result = ZnkS_getU64X( &tmp, str );
#elif Znk_CPU_BIT == 32
	uint32_t tmp = 0;
	result = ZnkS_getU32X( &tmp, str );
#else
	/* not support */
	void* tmp = NULL;
#endif
	*ans = (void*)(uintptr_t)tmp;
	return result;
}



static int
StrCompare_StdFuncAdapter( const char* s1, const char* s2, size_t n, void* arg )
{
	typedef int (*FuncT_strncmp)( const char*, const char*, size_t );
	FuncT_strncmp strncmp_func = (FuncT_strncmp) arg;
	return (*strncmp_func)( s1, s2, n );
}
void ZnkS_makeStrCompare_fromStdFunc(
		ZnkS_FuncArg_StrCompare* strcompare_funcarg,
		int (*strncmp_func)( const char*, const char*, size_t ) )
{
	strcompare_funcarg->func_ = StrCompare_StdFuncAdapter;
	strcompare_funcarg->arg_  = (void*)strncmp_func;
}
bool
ZnkS_compareBegin(
		const char* str, size_t str_leng,
		const char* ptn, size_t ptn_leng, const ZnkS_FuncArg_StrCompare* strcompare_funcarg )
{
	if( ptn_leng == Znk_NPOS ){ ptn_leng = Znk_strlen( ptn ); }
	if( str_leng != Znk_NPOS ){
		/***
		 * この場合、str_leng の値はNULL終端までのサイズと一致するとは限らない.
		 *
		 * 1. str_leng < ptn_leng のとき
		 *   明らかにfalse である.
		 *
		 *   このとき strncmp系の比較関数では、strがNULL終端するまでを調べるが、
		 *   str_leng < Znk_strlen(str) が成り立つ場合、比較対象として欲しくない
		 *   [ str_leng, Znk_strlen(str) ) の範囲にあるstrの内容も比較対象となってしまう.
		 *   そのため、この場合をまず除外しなければならない.
		 *
		 * 2. str_leng >= ptn_leng のとき
		 *   このとき strは最大でもptn_leng分までしか比較されない.
		 *   str_leng < Znk_strlen(str) が成り立つ場合でも、ptn_leng < Znk_strlen(str)であるため
		 *   「比較対象として欲しくない」区間は発生し得ない.
		 *   従って、strncmp系関数に直接渡してもかまわない.
		 */
		if( str_leng < ptn_leng ){ return false; }
	}
	if( strcompare_funcarg == NULL || strcompare_funcarg->func_ == NULL ){
		return (bool)( strncmp( str, ptn, ptn_leng ) == 0 );
	}
	return (bool)( (*strcompare_funcarg->func_)( str, ptn, ptn_leng,
				strcompare_funcarg->arg_ ) == 0 );
}
bool
ZnkS_compareEnd(
		const char* str, size_t str_leng,
		const char* ptn, size_t ptn_leng, const ZnkS_FuncArg_StrCompare* strcompare_funcarg )
{
	if( str_leng == Znk_NPOS ){ str_leng = Znk_strlen( str ); }
	if( ptn_leng == Znk_NPOS ){ ptn_leng = Znk_strlen( ptn ); }
	/***
	 * この判定は必須である.
	 * 以下の処理において &(str[0]) よりも前の位置を示すようなことは避けねばならない.
	 */
	if( str_leng < ptn_leng ){ return false; }
	/***
	 * ptn_leng == 0のときは、常にtrueとする.
	 * str_leng == 0のとき、ptn_leng == 0の場合のみここに来る可能性はあるが、
	 * その場合もtrueとなり、矛盾しない.
	 */
	if( ptn_leng == 0 ){ return true; }

	if( strcompare_funcarg == NULL || strcompare_funcarg->func_ == NULL ){
		return (bool)( strncmp( str+str_leng-ptn_leng, ptn, ptn_leng ) == 0 );
	}
	return (bool)( (*strcompare_funcarg->func_)( str+str_leng-ptn_leng, ptn, ptn_leng,
				strcompare_funcarg->arg_ ) == 0 );
}
bool
ZnkS_compareContain(
		const char* str, size_t str_leng, size_t pos,
		const char* ptn, size_t ptn_leng, const ZnkS_FuncArg_StrCompare* strcompare_funcarg )
{
	DECIDE_STRLENG( str_leng, str );
	DECIDE_STRLENG( ptn_leng, ptn );
	/***
	 * この判定は必須である.
	 * 以下の処理において &(str[0]) よりも前の位置を示すようなことは避けねばならない.
	 */
	if( str_leng < ptn_leng ){
		/* 明らかにマッチしない */
		return false;
	}
	/***
	 * ptn_leng == 0のときは、常にtrueとする.
	 * str_leng == 0のとき、ptn_leng == 0の場合のみここに来る可能性はあるが、
	 * その場合もtrueとなり、矛盾しない.
	 */
	if( ptn_leng == 0 ){ return true; }

	if( pos == Znk_NPOS ){
		/* この場合、特別に最後にptnで終わっているかどうかを調べるものとする */
		pos = str_leng - ptn_leng;
	} else if( pos > str_leng - ptn_leng ){
		/***
		 * pos は最後の ptn_leng 文字分よりも後を指している.
		 * この時点で ptn_leng != 0 であるから、ここは確実にマッチせず false となる. */
		return false;
	} else {
		/* none */
	}

	if( strcompare_funcarg == NULL || strcompare_funcarg->func_ == NULL ){
		return (bool)( memcmp( str+pos, ptn, ptn_leng ) == 0 );
	}
	return (bool)( (*strcompare_funcarg->func_)( str+pos, ptn, ptn_leng,
				strcompare_funcarg->arg_ ) == 0 );
}

bool
ZnkS_isBegin( const char* str, const char* ptn )
{
	return (bool)( strncmp( str, ptn, Znk_strlen(ptn) ) == 0 );
}
bool
ZnkS_isEnd( const char* str, const char* ptn )
{
	ZnkS_FuncArg_StrCompare cmp_funcarg; ZnkS_makeStrCompare_fromStdFunc( &cmp_funcarg, strncmp );
	return ZnkS_compareEnd( str, Znk_NPOS, ptn, Znk_NPOS, &cmp_funcarg );
}
bool
ZnkS_isBeginEx( const char* str, const size_t str_leng, const char* ptn, size_t ptn_leng )
{
	ZnkS_FuncArg_StrCompare cmp_funcarg; ZnkS_makeStrCompare_fromStdFunc( &cmp_funcarg, strncmp );
	return ZnkS_compareBegin( str, str_leng, ptn, ptn_leng, &cmp_funcarg );
}
bool
ZnkS_isEndEx( const char* str, const size_t str_leng, const char* ptn, size_t ptn_leng )
{
	ZnkS_FuncArg_StrCompare cmp_funcarg; ZnkS_makeStrCompare_fromStdFunc( &cmp_funcarg, strncmp );
	return ZnkS_compareEnd( str, str_leng, ptn, ptn_leng, &cmp_funcarg );
}

bool
ZnkS_isCaseBegin( const char* str, const char* ptn )
{
	return (bool)( ZnkS_strncasecmp( str, ptn, Znk_strlen(ptn) ) == 0 );
}
bool
ZnkS_isCaseEnd( const char* str, const char* ptn )
{
	ZnkS_FuncArg_StrCompare cmp_funcarg; ZnkS_makeStrCompare_fromStdFunc( &cmp_funcarg, ZnkS_strncasecmp );
	return ZnkS_compareEnd( str, Znk_NPOS, ptn, Znk_NPOS, &cmp_funcarg );
}
bool
ZnkS_isCaseBeginEx( const char* str, size_t str_leng, const char* ptn, size_t ptn_leng )
{
	ZnkS_FuncArg_StrCompare cmp_funcarg; ZnkS_makeStrCompare_fromStdFunc( &cmp_funcarg, ZnkS_strncasecmp );
	return ZnkS_compareBegin( str, str_leng, ptn, ptn_leng, &cmp_funcarg );
}
bool
ZnkS_isCaseEndEx( const char* str, size_t str_leng, const char* ptn, size_t ptn_leng )
{
	ZnkS_FuncArg_StrCompare cmp_funcarg; ZnkS_makeStrCompare_fromStdFunc( &cmp_funcarg, ZnkS_strncasecmp );
	return ZnkS_compareEnd( str, str_leng, ptn, ptn_leng, &cmp_funcarg );
}


bool
ZnkS_isMatchBeginEx(
		const char* str, size_t str_leng,
		const char* ptn, size_t ptn_leng,
		const ZnkS_FuncArg_IsKeyChar*  iskeychar_funcarg,
		const ZnkS_FuncArg_StrCompare* strcompare_funcarg )
{
	if( iskeychar_funcarg != NULL && iskeychar_funcarg->func_ != NULL ){
		if( ptn_leng == Znk_NPOS ){ ptn_leng = Znk_strlen( ptn ); }

		/* このstrncmp が成功すれば、当然 Znk_strlen(str) >= ptn_leng である */
		if( !ZnkS_compareBegin( str, str_leng, ptn, ptn_leng, strcompare_funcarg ) ){ return false; }
		if( str[ ptn_leng ] == '\0' ){ return true; }

		return (bool)( !(*iskeychar_funcarg->func_)( str[ ptn_leng ], iskeychar_funcarg->arg_ ) );
	}
	return ZnkS_compareBegin( str, str_leng, ptn, ptn_leng, strcompare_funcarg );
}
static bool
IsKeyChar_isMatchBegin( uint32_t ch, void* arg )
{
	const char* keychars = (const char*) arg;
	return (bool)( strchr( keychars, (int)ch ) != NULL );
}
bool
ZnkS_isMatchBegin( const char* str, const char* ptn, const char* keychars )
{
	ZnkS_FuncArg_IsKeyChar  iskeychar_funcarg  = { IsKeyChar_isMatchBegin, NULL };
	iskeychar_funcarg.arg_ = (void*)keychars;
	return ZnkS_isMatchBeginEx( str, Znk_NPOS, ptn, Znk_NPOS,
			&iskeychar_funcarg,
			NULL );
}
bool
ZnkS_isMatchSWC( const char* ptn, size_t ptn_leng,
		const char* query, size_t query_leng )
{
	Znk_setStrLen_ifNPos( &ptn_leng, ptn );
	Znk_setStrLen_ifNPos( &query_leng, query );

	if( ptn_leng == 1 ){
		if( ptn[ 0 ] == '*' ){
			return true;
		}
		if( query_leng == 1 ){
			return (bool)( ptn[ 0 ] == query[ 0 ] );
		} 
		return false;
	}

	if( ptn[ 0 ] == '*' ){
		/* *ABC : ABCで終るかどうか */
		if( ZnkS_isEndEx( query, query_leng, ptn+1, ptn_leng-1 ) ){
			/* found */
			return true;
		}
	} else if( ptn[ ptn_leng-1 ] == '*' ){
		/* ABC* : ABCで始まるかどうか */
		if( ZnkS_isBeginEx( query, query_leng, ptn, ptn_leng-1 ) ){
			/* found */
			return true;
		}
	} else {
		const char* p = (char*)Znk_memchr( ptn, '*', ptn_leng );
		if( p ){
			/* AB*CD : ABで始まりCDで終るかどうか */
			const size_t prev_leng = p - ptn;
			if(  ZnkS_isBeginEx( query, query_leng, ptn, prev_leng )
			  && ZnkS_isEndEx( query+prev_leng, query_leng-prev_leng, ptn+prev_leng+1, ptn_leng-prev_leng-1 ) )
			{
				/* found */
				return true;
			}
		} else if( query_leng == ptn_leng && ZnkS_eqEx( query, ptn, query_leng ) ){
			/* found */
			return true;
		}
	}
	/* not found */
	return false;
}



uint32_t
ZnkS_get_id32( const char* s, size_t leng )
{
	union { uint32_t u32; uint8_t u[4]; } U;
	const uint8_t* us = (const uint8_t*)s;
	U.u32 = 0;
	if( leng == Znk_NPOS ){
		/***
		 * strncpyは最後に余計なzero-fillを行うのでその分無駄であるため、
		 * (すでに0-clearされているので)ここでは用いない.
		 */
		leng = 0;
		while( *us && leng != 4 ){ U.u[ leng++ ] = *(us++); }
	} else {
		Znk_CLAMP_MAX( leng, 4 );
		Znk_memcpy( U.u, us, leng );
	}
	return U.u32;
}
uint64_t
ZnkS_get_id64( const char* s, size_t leng )
{
	union { uint64_t u64; uint8_t u[8]; } U;
	const uint8_t* us = (const uint8_t*)s;
	U.u64 = 0;
	if( leng == Znk_NPOS ){
		/***
		 * strncpyは最後に余計なzero-fillを行うのでその分無駄であるため、
		 * (すでに0-clearされているので)ここでは用いない.
		 */
		leng = 0;
		while( *us && leng != 8 ){ U.u[ leng++ ] = *(us++); }
	} else {
		Znk_CLAMP_MAX( leng, 8 );
		Znk_memcpy( U.u, us, leng );
	}
	return U.u64;
}

const char*
ZnkS_get_extension( const char* str, char dot_ch )
{
	/***
	 * strrchrの第2引数に0を指定することもでき、その場合はstrrchrは最後のNULL終端文字の
	 * 位置を返す. しかしながら、拡張子を取得したいというこの用途の場合、この結果に
	 * 意味があるとは思えない. したがってこの場合は無条件に""を返すという仕様とする.
	 */
	if( dot_ch != '\0' ){
		const char* ans = strrchr( str, dot_ch );
		if( ans == NULL ){
			/***
			 * セパレータとなるdot_chが含まれていなかった場合、strrchrはNULLを返す.
			 */
			return "";
		}
		return ans+1; /* dot_chのある位置の次の位置を返す */
	}
	return "";
}


size_t
ZnkS_lfind_one_of( const char* str, size_t begin, size_t end, const char* chset, size_t chset_size )
{
	if( begin == Znk_NPOS ){ return Znk_NPOS; }
	DECIDE_STRLENG( end, str );
	if( end == 0 ){     return Znk_NPOS; }
	if( begin >= end ){ return Znk_NPOS; }

	DECIDE_STRLENG( chset_size, chset );
	if( chset_size == 0 ){ return begin; }

	while( true ){
		if( ZnkMem_lfind_8( (uint8_t*)chset, chset_size, (uint8_t)str[begin], 1 ) != Znk_NPOS ){
			/***
			 * str[begin] ∈ chset を満たす begin が見つかった.
			 */
			break;
		}
		++begin;
		if( begin >= end ){
			/***
			 * strの終端文字までスキャンしたが、結局すべて chset 内にない文字だった.
			 */
			begin = Znk_NPOS;
			break;
		}
	}
	return begin;
}

size_t
ZnkS_rfind_one_of( const char* str, size_t begin, size_t end, const char* chset, size_t chset_size )
{
	if( begin == Znk_NPOS ){ return Znk_NPOS; }
	DECIDE_STRLENG( end, str );
	if( end == 0 ){     return Znk_NPOS; }
	if( begin >= end ){ return Znk_NPOS; }
	--end;

	DECIDE_STRLENG( chset_size, chset );
	if( chset_size == 0 ){ return end; }

	while( true ){
		if( ZnkMem_lfind_8( (uint8_t*)chset, chset_size, (uint8_t)str[end], 1 ) != Znk_NPOS ){
			/***
			 * str を 位置pos から逆順に見て、初めて chset にある文字が現れた.
			 */
			break;
		}

		if( end == 0 ){
			/***
			 * strの一番初めの文字までスキャンしたが、結局すべて chset 内にない文字だった.
			 */
			end = Znk_NPOS;
			break;
		}
		--end;
	}
	return end;
}


size_t
ZnkS_lfind_one_not_of( const char* str, size_t begin, size_t end, const char* chset, size_t chset_size )
{
	if( begin == Znk_NPOS ){ return Znk_NPOS; }
	DECIDE_STRLENG( end, str );
	if( end == 0 ){     return Znk_NPOS; }
	if( begin >= end ){ return Znk_NPOS; }

	DECIDE_STRLENG( chset_size, chset );
	if( chset_size == 0 ){ return begin; }

	while( true ){
		if( ZnkMem_lfind_8( (uint8_t*)chset, chset_size, (uint8_t)str[begin], 1 ) == Znk_NPOS ){
			/***
			 * str を 位置begin から順に見て、初めて chset にない文字が現れた.
			 */
			break;
		}
		++begin;
		if( begin >= end ){
			/***
			 * strの終端文字までスキャンしたが、結局すべて chset 内にある文字だった.
			 */
			begin = Znk_NPOS;
			break;
		}
	}
	return begin;
}
size_t
ZnkS_rfind_one_not_of( const char* str, size_t begin, size_t end, const char* chset, size_t chset_size )
{
	if( begin == Znk_NPOS ){ return Znk_NPOS; }
	DECIDE_STRLENG( end, str );
	if( end == 0 ){     return Znk_NPOS; }
	if( begin >= end ){ return Znk_NPOS; }
	--end;

	DECIDE_STRLENG( chset_size, chset );
	if( chset_size == 0 ){ return end; }

	while( true ){
		if( ZnkMem_lfind_8( (uint8_t*)chset, chset_size, (uint8_t)str[end], 1 ) == Znk_NPOS ){
			/***
			 * str を 位置end から逆順に見て、初めて chset にない文字が現れた.
			 */
			break;
		}

		if( end == 0 ){
			/***
			 * strの一番初めの文字までスキャンしたが、結局すべて chset 内にある文字だった.
			 */
			end = Znk_NPOS;
			break;
		}
		--end;
	}
	return end;
}

size_t
ZnkS_find_key_and_val( const char* str, size_t begin, size_t end,
		size_t* key_begin, size_t* key_end,
		size_t* val_begin, size_t* val_end,
		const char* delimiter,  size_t delimiter_leng,
		const char* skip_chset, size_t skip_chset_leng )
{
	size_t delimiter_pos;
	DECIDE_STRLENG( delimiter_leng, delimiter );

	/***
	 * まず delimiter の開始位置である delimiter_pos をfind.
	 */
	delimiter_pos = ZnkMem_lfind_data_BF( (uint8_t*)str + begin, end - begin,
			(uint8_t*)delimiter, delimiter_leng, 1 );
	if( delimiter_pos == Znk_NPOS ){
		/***
		 * str[begin,end) 内にdelimiterが存在しない場合、
		 * val_begin, val_end には Znk_NPOS をセットする.
		 * また、key_begin, key_end についてのみ、ここで単独でfindする.
		 */
		ZnkS_find_side_skip( str, begin, end,
				key_begin, key_end, skip_chset, skip_chset_leng );
		*val_begin = Znk_NPOS; *val_end = Znk_NPOS;
		return Znk_NPOS;
	}
	delimiter_pos += begin; /* 相対位置から絶対位置へ直す */

	/***
	 * key_begin, key_end を確定.
	 */
	ZnkS_find_side_skip( str, begin, delimiter_pos,
			key_begin, key_end, skip_chset, skip_chset_leng );

	/***
	 * val_begin, val_end を確定.
	 */
	ZnkS_find_side_skip( str, delimiter_pos+delimiter_leng, end,
			val_begin, val_end, skip_chset, skip_chset_leng );

	return delimiter_pos;

}

size_t
ZnkS_lfind_arg( const char* str, size_t begin, size_t end,
		size_t arg_idx, size_t* arg_leng,
		const char* skip_chset, size_t skip_chset_leng )
{
	size_t arg_end = Znk_NPOS;

	while( true ){
		/***
		 * 現在のbeginからの skip_chset からなる部分をskipし、argの開始位置を求める.
		 */
		begin = ZnkS_lfind_one_not_of( str, begin, end, skip_chset, skip_chset_leng );
		if( begin == Znk_NPOS ){
			/***
			 * arg はもはや存在せず、目的のarg_idx番目のargも存在しない.
			 */
			*arg_leng = 0;
			return Znk_NPOS;
		}
		/***
		 * skip_chset が始まる位置(argの終わる位置)を求める.
		 */
		arg_end = ZnkS_lfind_one_of( str, begin, end, skip_chset, skip_chset_leng );
		if( arg_end == Znk_NPOS ){
			/***
			 * arg は end まで続く文字列であった場合.
			 */
			if( arg_idx == 0 ){
				/***
				 * OK. 該当のargを取得完了.
				 */
				*arg_leng = end - begin;
				return begin;
			} else {
				/***
				 * arg はもはや存在せず、目的のarg_idx番目のargも存在しない.
				 */
				*arg_leng = 0;
				return Znk_NPOS;
			}
		}

		if( arg_idx == 0 ){
			/***
			 * OK. 該当のargを取得完了.
			 */
			*arg_leng = arg_end - begin;
			return begin;
		} else {
			/***
			 * 次の arg 取得に移る.
			 */
			--arg_idx;
			begin = arg_end;
		}
	}
}
size_t
ZnkS_rfind_arg( const char* str, size_t begin, size_t end,
		size_t arg_idx, size_t* arg_leng,
		const char* skip_chset, size_t skip_chset_leng )
{
	size_t arg_begin = Znk_NPOS;

	while( true ){
		/***
		 * 現在のendからの skip_chset からなる部分をskipし、argの終了位置を求める.
		 */
		end = ZnkS_rfind_one_not_of( str, begin, end, skip_chset, skip_chset_leng );
		if( end == Znk_NPOS ){
			/***
			 * arg はもはや存在せず、目的のarg_idx番目のargも存在しない.
			 */
			*arg_leng = 0;
			return Znk_NPOS;
		}
		/***
		 * end は 現在 arg の一番最後の文字そのものの位置を指している.
		 * この arg の arg_leng(文字列長)が 1 より大きい場合はこれを次の処理の
		 * 出発値としてもよさそうに見えるが、arg_leng 取得の計算時に + 1 しなければ
		 * ならないという注意事項が発生する. また arg_leng が 1 に等しい場合、
		 * begin と end が等しくなる可能性が生じる. ZnkS_rfind_one_not_of では
		 * begin と end の値が等しい場合はZnk_NPOSを返すが、これはここで期待する
		 * 結果ではない. ここではこのような場合も考慮して、end を確実に beginよりも大きい
		 * 本来の意味でのend値にしておく必要がある.
		 */
		++end;

		/***
		 * skip_chset が始まる位置(argの開始位置の一つ前の位置)を求める.
		 */
		arg_begin = ZnkS_rfind_one_of( str, begin, end, skip_chset, skip_chset_leng );
		if( arg_begin == Znk_NPOS ){
			/***
			 * arg は begin まで続く文字列であった場合.
			 */
			if( arg_idx == 0 ){
				/***
				 * OK. 該当のargを取得完了.
				 */
				*arg_leng = end - begin;
				return begin;
			} else {
				/***
				 * arg はもはや存在せず、目的のarg_idx番目のargも存在しない.
				 */
				*arg_leng = 0;
				return Znk_NPOS;
			}
		}
		/***
		 * arg_begin が Znk_NPOS 以外であった場合、この時点で arg の一番最初の文字の
		 * 一つ前の位置を指している. そのため、ここで 1 インクリメントせねばならない.
		 */
		++arg_begin;

		if( arg_idx == 0 ){
			/***
			 * OK. 該当のargを取得完了.
			 */
			*arg_leng = end - arg_begin;
			return arg_begin;
		} else {
			/***
			 * 次の arg 取得に移る.
			 */
			--arg_idx;
			end = arg_begin;
		}
	}
}


