#include "Znk_str.h"
#include "Znk_stdc.h"
#include "Znk_s_base.h"
#include "Znk_vsnprintf.h"

#include <stdarg.h>
#include <string.h>

#define M_data( zkbfr )     ZnkBfr_data( zkbfr )
#define M_size( zkbfr )     ZnkBfr_size( zkbfr )
#define M_c_str( zkbfr )    ((char*)ZnkBfr_data( zkbfr ))

#define DECIDE_LENG( leng, str ) do if( (leng) == Znk_NPOS ){ (leng) = Znk_strlen(str); } while(0)

Znk_INLINE void
I_zkstr_releng( ZnkStr zkstr, size_t new_leng )
{
	ZnkBfr_resize( zkstr, new_leng+1 );
	M_data( zkstr )[ new_leng ] = '\0';
}

void
ZnkStr_releng( ZnkStr zkstr, size_t new_leng )
{
	I_zkstr_releng( zkstr, new_leng );
}

void
ZnkStr_terminate_null( ZnkBfr zkbfr, bool plus_one )
{
	const size_t size = M_size(  zkbfr );
	char*        data = M_c_str( zkbfr );
	if( size == 0 ){
		ZnkBfr_push_bk( zkbfr, '\0' );
		return;
	}
	if( plus_one ){
		if( data[size-1] != '\0' ){
			ZnkBfr_push_bk( zkbfr, '\0' );
		}
	} else {
		/* 無条件に最終文字を NULL文字で上書きする */
		data[ size-1 ] = '\0';
	}
}

void
ZnkStr_normalize( ZnkBfr zkbfr, bool plus_one )
{
	const size_t size = M_size(  zkbfr );
	char*        data = M_c_str( zkbfr );
	if( size == 0 ){
		/* NULL文字を後ろに連結する */
		ZnkBfr_push_bk( zkbfr, '\0' );
		return;
	} else {
		const char* null_p = memchr( data, '\0', size );
		const size_t null_pos = null_p ? null_p - data : Znk_NPOS;
		if( null_pos == Znk_NPOS ){
			/* 範囲内にNULL文字が見つからない場合 */
			if( plus_one ){
				/* NULL文字を後ろに連結する */
				ZnkBfr_push_bk( zkbfr, '\0' );
			} else {
				/* 無条件に最終文字を NULL文字で上書きする */
				data[ size-1 ] = '\0';
			}
		} else if( null_pos < size-1 ){
			/* 範囲内にNULL文字があり、かつそれが最終文字よりも前にある場合 */
			ZnkBfr_resize( zkbfr, null_pos+1 ); /* これによるreallocは発生しないはず */
		}
	}
}

ZnkStr
ZnkStr_create_ex( const char* init_data, size_t leng, ZnkBfrType type )
{
	ZnkBfr zkbfr;
	if( init_data ){
		if( leng == Znk_NPOS ){ leng = Znk_strlen(init_data); }
		zkbfr = ZnkBfr_create( (uint8_t*)init_data, leng, false, type );
		ZnkStr_terminate_null( zkbfr, true );
	} else {
		if( leng == Znk_NPOS ){ leng = 0; }
		zkbfr = ZnkBfr_create( NULL, leng+1, false, type );
		I_zkstr_releng( zkbfr, 0 );
	}
	return zkbfr;
}
ZnkStr
ZnkStr_newf( const char* fmt, ... )
{
	ZnkStr zkstr = ZnkStr_new( "" );
	va_list ap;
	va_start(ap, fmt);
	ZnkStr_vsnprintf( zkstr, 0, Znk_NPOS, fmt, ap );
	va_end(ap);
	return zkstr;
}

void
ZnkStr_assign( ZnkStr zkstr_dst, size_t dst_pos, const char* src, size_t src_leng )
{
	size_t size = M_size(zkstr_dst);
	if( size == 0 ){
		dst_pos = 0;
	} else {
		dst_pos = Znk_clampSize( dst_pos, size-1 );
	}
	if( src_leng == 0 ){
		/* erase */
		ZnkBfr_resize( zkstr_dst, dst_pos );
		ZnkStr_terminate_null( zkstr_dst, true );
		return;
	} else {
		uint8_t* data;
		if( src_leng == Znk_NPOS ){ src_leng = Znk_strlen(src); }
		ZnkBfr_resize( zkstr_dst, dst_pos + src_leng + 1 );
		data = M_data(zkstr_dst);
		Znk_memmove( data+dst_pos, src, src_leng );
		data[ dst_pos + src_leng ] = '\0';
	}
}

void
ZnkStr_replace( ZnkStr str, size_t dst_pos, size_t dst_leng, const char* src, size_t src_leng )
{
	src_leng = ( src_leng == Znk_NPOS ) ? Znk_strlen(src) : src_leng;
	dst_pos  = Znk_clampSize( dst_pos,  ZnkStr_leng(str) );
	dst_leng = Znk_clampSize( dst_leng, ZnkStr_leng(str)-dst_pos );
	ZnkBfr_replace( str, dst_pos, dst_leng, (uint8_t*)src, src_leng );
	ZnkStr_terminate_null( str, true );
}
void
ZnkStr_insert( ZnkStr str, size_t dst_pos, const char* src, size_t src_leng )
{
	src_leng = ( src_leng == Znk_NPOS ) ? Znk_strlen(src) : src_leng;
	dst_pos  = Znk_clampSize( dst_pos,  ZnkStr_leng(str) );
	ZnkBfr_replace( str, dst_pos, 0, (uint8_t*)src, src_leng );
	ZnkStr_terminate_null( str, true );
}

void
ZnkStr_add_c( ZnkStr zkstr, char val )
{
	const size_t size = ZnkBfr_size( zkstr );
	uint8_t* data;
	if( size == 0 ){
		ZnkBfr_resize( zkstr, 2 );
		data = ZnkBfr_data( zkstr );
		data[ 0 ] = (uint8_t)val;
		data[ 1 ] = '\0';
		return;
	}
	ZnkBfr_resize( zkstr, size+1 );
	data = ZnkBfr_data( zkstr );
	data[ size-1 ] = (uint8_t)val;
	data[ size   ] = '\0';
}

/* sys and C99 両対応版 */
typedef int (*ZnkVSNPrintfFuncT)( char* buf, size_t buf_size, const char* fmt, va_list ap );
typedef enum {
	ZnkVSNPrintfType_e_C99=0,
	ZnkVSNPrintfType_e_Sys
} ZnkVSNPrintfType; 
static ZnkVSNPrintfType st_vsnprintf_type = ZnkVSNPrintfType_e_C99;
int
ZnkStr_vsnprintf( ZnkStr zkstr, size_t pos, size_t size, const char* fmt, va_list ap )
{
	/**
	 * ここで指定する関数は va_start <=> va_end 間における複数回呼び出しに
	 * 対応したものでなければならない.
	 */
	ZnkVSNPrintfFuncT vsnprintf_func = st_vsnprintf_type == ZnkVSNPrintfType_e_C99 ?
		Znk_vsnprintf_C99 : ZnkS_vsnprintf_sys__;
	int  ret_len = -1;
	bool result  = true;

	if( size > 0 ){
		int ret_len;
		char* data;
		size_t str_leng = ZnkStr_leng( zkstr );
		pos = Znk_clampSize( pos, str_leng );
	
		if( size == Znk_NPOS ){
			size_t backword_capacity = 512;
			size_t backword_capacity_real;
			/***
			 * size制限なし.
			 * この場合、可能な限りバッファの拡張を行う.
			 */
			while( true ){
				ZnkBfr_reserve( zkstr, pos + backword_capacity );
				backword_capacity_real = ZnkBfr_capacity( zkstr ) - pos;
				data = M_c_str( zkstr ); /* Note : think realloc */
				ret_len = vsnprintf_func( data + pos, backword_capacity_real, fmt, ap );

				/***
				 * ret_lenがret_len < backword_capacity_realの場合、指定されたバッファ内に
				 * 展開文字列のすべてが書き込まれており完了である.
				 * ret_lenが -1 の場合、内部エラーが発生したことを示すため、
				 * この場合はresult=falseにした上で中止する.
				 */
				if( (size_t)ret_len < backword_capacity_real ){
					/* OK. */
					break;
				} else if( ret_len < 0 ){
					/* 内部エラー. */
					result = false;
					break;
				}
				/***
				 * string is trucated. retry.
				 *
				 * C99の仕様では戻り値であるret_lenは、backword_capacity_realの指定値に関わらず
				 * 正確に展開後の文字列サイズを示す.
				 * 一方、ZnkS_vsnprintf_sysではC99の仕様とは異なり、backword_capacity_realの指定値を
				 * Overした場合は、その戻り値として予測文字列長を返す.
				 * これは正確に展開後の文字列サイズであるとは限らないが、多くの場合十分なサイズである.
				 *
				 * いずれにせよ次回は容量をret_len + 1 として試みればよい.
				 */
				backword_capacity = ret_len + 1;
			}

		} else {
			ZnkBfr_resize( zkstr, pos + size );
			data = M_c_str( zkstr ); /* Note : think realloc */
			//ret_len = Znk_vsnprintf_C99( data + pos, size, fmt, ap );
			ret_len = vsnprintf_func( data + pos, size, fmt, ap );
			if( ret_len < 0 ){
				/* 内部エラー. */
				result = false;
			}
		}
		/***
		 * 上記ではZnkBfrとして処理しているため、最後にZnkStrとしてrelengしておく.
		 */
		if( result ){
			ZnkStr_releng( zkstr, pos + ret_len );
		} else {
			/* 内部エラーが発生した場合はpos以降の内容は破壊されている可能性がある.
			 * サイズは pos に切り詰めておくのが望ましいと思われる. */
			ZnkStr_releng( zkstr, pos );
		}
		/***
		 * 処理の性質上、無駄なメモリ確保が生じた場合、その差分サイズが大きいので、
		 * capacityがある程度大きいケースについては shrink to fit を行っておく.
		 */
		if( ZnkBfr_capacity( zkstr ) > 4096 ){
			ZnkBfr_shrink_to_fit( zkstr );
		}
	}
	return ret_len;
}

int ZnkStr_snprintf( ZnkStr zkstr, size_t pos, size_t size, const char* fmt, ... )
{
	int ret_len;
	va_list ap;
	va_start(ap, fmt);
	ret_len = ZnkStr_vsnprintf( zkstr, pos, size, fmt, ap );
	va_end(ap);
	return ret_len;
}
int ZnkStr_sprintf( ZnkStr zkstr, size_t pos, const char* fmt, ... )
{
	int ret_len;
	va_list ap;
	va_start(ap, fmt);
	ret_len = ZnkStr_vsnprintf( zkstr, pos, Znk_NPOS, fmt, ap );
	va_end(ap);
	return ret_len;
}
int ZnkStr_addf( ZnkStr zkstr, const char* fmt, ... )
{
	int ret_len;
	va_list ap;
	va_start(ap, fmt);
	ret_len = ZnkStr_vsnprintf( zkstr, Znk_NPOS, Znk_NPOS, fmt, ap );
	va_end(ap);
	return ret_len;
}
int ZnkStr_setf( ZnkStr zkstr, const char* fmt, ... )
{
	int ret_len;
	va_list ap;
	va_start(ap, fmt);
	ret_len = ZnkStr_vsnprintf( zkstr, 0, Znk_NPOS, fmt, ap );
	va_end(ap);
	return ret_len;
}

/*****************************************************************************/
/* eq */
bool
ZnkStr_eq( const ZnkStr str, const char* cstr ){
	return (bool)( strcmp( ZnkStr_cstr(str), cstr ) == 0 );
}

bool
ZnkStr_eqEx( const ZnkStr str, size_t pos, const char* data, size_t data_leng )
{
	const size_t leng = ZnkStr_leng( str );
	DECIDE_LENG( data_leng, data );
	if( pos >= leng ){
		/* このときstr側の範囲は空とみなす. 従ってdata_lengが0の場合のみ真とする. */
		return (bool) ( data_leng == 0 );
	}
	/* この時点で leng > 0 である */ 
	if( leng != data_leng ){ return false; }
	return (bool)( Znk_memcmp( M_data(str), data, leng ) == 0 );
}

bool
ZnkStr_isContain( const ZnkStr zkstr, size_t pos, const char* data, size_t data_size )
{
	const size_t leng = ZnkStr_leng( zkstr );
	DECIDE_LENG( data_size, data );
	if( data_size > leng ){
		/* 明らかにマッチしない */
		return false;
	}

	if( pos == Znk_NPOS ){
		/* この場合最後にdataで終わっているかどうかを調べるものとする */
		pos = leng - data_size;
	} else if( pos > leng ){
		pos = leng;
	}

	if( pos == leng ){
		/* pos は最後のNULL文字を指している. つまり空文字列と考える */
		return (bool)( data_size == 0 );
	} else {
		const size_t size = leng - pos;
		if( size < data_size ){ return false; }
	}
	return (bool)( Znk_memcmp( M_data(zkstr) + pos, data, data_size ) == 0 );
}

/* endof eq */
/*****************************************************************************/


void
ZnkStr_cut_back( ZnkStr zkstr, size_t cut_size )
{
	const size_t size = M_size( zkstr );
	switch( size ){
	case 0:  return;
	case 1:  M_data(zkstr)[ 0 ] = '\0'; return;
	default: break;
	}

	/***
	 * cut_size == 1 のとき、以下の ( cut_size >= size-1 ) という判定は無駄である.
	 * (このとき、単に I_zkstr_releng( zkstr, size-2 ) を呼び出せばよい).
	 * しかし、それを嫌って switch( cut_size ) により、cut_size が 1 の場合とそうでない場合とを
	 * 分けたとしても、そもそもこの条件判定一回分とswitchによるオーバーヘッドにはほとんど差はため、
	 * あまり意味がある措置とは思えない. また、ZnkStr_cut_back_c についてこの部分だけをなくした
	 * 実装を特別に用意するのは大げさすぎるし、むしろ余計な実装を増やすだけになる.
	 * よって、ここでは単に以下のような呼び出しに留める形とした.
	 */
	I_zkstr_releng( zkstr, 
			( cut_size >= size-1 ) ? 0 : size-1-cut_size );
}
void
ZnkStr_erase( ZnkStr zkstr, size_t pos, size_t cut_size )
{
	const size_t size = M_size( zkstr );
	switch( size ){
	case 0:  return;
	case 1:  M_data(zkstr)[ 0 ] = '\0'; return;
	default: break;
	}
	if( pos >= size-1 ){ return; }
	if( pos + cut_size >= size-1 ){
		/* このとき、pos で releng でよい */
		I_zkstr_releng( zkstr, pos );
		return;
	}
	/***
	 * pos + cut_size <= size-2
	 * このとき、消されるのは必ず終端NULLより前の文字列である.
	 * よって結果は依然としてNULL終端の状態を保つ.
	 * 厳密には pos + cut_size <= size-1 でも、以下の処理は問題はないのだが、
	 * ZnkBfr_eraseはZnkStr_clearより低速であるため、ここではなるべく条件を搾った.
	 */
	ZnkBfr_erase( zkstr, pos, cut_size );
}

void
ZnkStr_chompC( ZnkStr zkstr, size_t pos, char ch )
{
	const size_t size = M_size( zkstr );
	if( size <= 1 ){
		/* このときはどういう状態であれ、文字列長は 0 とみなしてなにもしない */
		return;
	} else {
		const size_t last = size-2;
		uint8_t* data = M_data( zkstr );
		if( pos == Znk_NPOS || pos >= last ){
			if( data[ last ] == ch ){
				data[ last ] = '\0';
				ZnkBfr_resize( zkstr, size-1 );
			}
		} else {
			if( data[ pos ] == ch ){
				//ZnkBfr_replace( zkstr, pos, 1, NULL, 0 );
				ZnkBfr_erase( zkstr, pos, 1 );
			}
		}
	}
}
void
ZnkStr_chompNL( ZnkStr line )
{
	/***
	 * 結果は
	 *   ZnkStr_chompC( line, Znk_NPOS, '\n' );
	 *   ZnkStr_chompC( line, Znk_NPOS, '\r' );
	 * と実行したのと同じであるが、ここでの実装は最適化してある.
	 */
	const size_t size = M_size( line );
	if( size <= 1 ){
		/* このときはどういう状態であれ、文字列長は 0 とみなしてなにもしない */
		return;
	} else {
		size_t last = size-2;
		uint8_t* data = M_data( line );

		if( data[ last ] == '\n' ){
			ZnkStr_releng( line, last );

			if( last == 0 ){ return; }
			--last;
			data = M_data( line );
		}
		if( data[ last ] == '\r' ){
			ZnkStr_releng( line, last );
		}
	}
}

char ZnkStr_at( const ZnkStr zkstr, size_t idx )
{
	const size_t size = M_size( zkstr );
	switch( size ){
		case 0: case 1: return '\0';
		default: break;
	}
	return (char)M_data(zkstr)[ idx ];
}
char ZnkStr_last( const ZnkStr zkstr )
{
	const size_t size = M_size( zkstr );
	switch( size ){
		case 0: case 1: return '\0';
		default: break;
	}
	/* size >= 2 */
	return (char)M_data( zkstr )[ size-2 ];
}

void
ZnkStr_swap( ZnkStr str1, ZnkStr str2 )
{
	ZnkBfr_swap( str1, str2 );
}
