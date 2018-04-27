#if defined(__GNUC__) && defined(__linux__)
#  define _FILE_OFFSET_BITS 64
#endif

#include "Znk_stdc.h"
#include "Znk_bfr.h"
#include "Znk_vsnprintf.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#if defined(Znk_TARGET_WINDOWS)
#  include <io.h>
#  include <fcntl.h>
#endif


void* Znk_malloc( size_t size ){
	return malloc( size );
}
void  Znk_free( void* ptr ){
	/* 現在のfreeではNULLチェックは不要 */
	free( ptr );
}
void* Znk_realloc( void* ptr, size_t size ){
	return realloc( ptr, size );
}
void* Znk_calloc( size_t ary_num, size_t elem_size ){
	return calloc( ary_num, elem_size );
}
void* Znk_alloc0( size_t size ){
	return ( size % sizeof(void*) ) ?
		calloc( size, 1 ) :
		calloc( size/sizeof(void*), sizeof(void*) );
}
char* Znk_strdup( const char* cstr ){
	return strdup( cstr );
}

void* Znk_memcpy( void* dst, const void* src, size_t size ){
	return memcpy( dst, src, size );
}
void* Znk_memmove( void* dst, const void* src, size_t size ){
	return memmove( dst, src, size );
}
void* Znk_memset( void* dst, uint8_t c, size_t size ){
	return memset( dst, c, size );
}
int   Znk_memcmp( const void* m1, const void* m2, size_t size ){
	return memcmp( m1, m2, size );
}
void* Znk_memchr( const void* m, int c, size_t size ){
	return memchr( m, c, size );
}

size_t Znk_strlen( const char* cstr ){
	return strlen( cstr );
}

int Znk_strcmp(  const char* s1, const char* s2 )
{
	return strcmp( s1, s2 );
}
int Znk_strncmp( const char* s1, const char* s2, size_t leng )
{
	return strncmp( s1, s2, leng );
}
char* Znk_strchr( const char* cstr, char c )
{
	return strchr( cstr, c );
}
char* Znk_strrchr( const char* cstr, char c )
{
	return strrchr( cstr, c );
}
char* Znk_strstr( const char* cstr, const char* ptn )
{
	return strstr( cstr, ptn );
}

char* Znk_getenv( const char* varname ){
	return getenv( varname );
}


ZnkFile Znk_Internal_getStdFP( int no )
{
	switch( no ){
	case 0: return (ZnkFile) stdin;
	case 1: return (ZnkFile) stdout;
	case 2: return (ZnkFile) stderr;
	default:
		break;
	}
	return NULL;
}

void Znk_Internal_setMode( int no, bool is_binary_mode )
{
#if defined(Znk_TARGET_WINDOWS)
	/* モード変更に先立ってまず確実にfflushしておく必要がある */
	fflush( (FILE*)Znk_Internal_getStdFP( no ) );
#  if defined(__BORLANDC__)
	setmode( no, is_binary_mode ? _O_BINARY : _O_TEXT );
#  else
	_setmode( no, is_binary_mode ? _O_BINARY : _O_TEXT );
#  endif
#endif
}

ZnkFile
Znk_fopen( const char* filename, const char* mode ){
	return (ZnkFile)fopen( filename, mode );
}
void
Znk_fclose( ZnkFile fp )
{
	if( fp && fp != (ZnkFile)stdin && fp != (ZnkFile)stdout && fp != (ZnkFile)stderr ){
		fclose( (FILE*)fp );
	}
}

int
Znk_fgetc( ZnkFile fp )
{
	return fgetc( (FILE*)fp );
}
int
Znk_fputc( int c, ZnkFile fp )
{
	return fputc( c, (FILE*)fp );
}

char*
Znk_fgets( char* buf, size_t size, ZnkFile fp )
{
	return fgets( buf, (int)size, (FILE*)fp );
}
int
Znk_fputs( const char* c_str, ZnkFile fp )
{
	return fputs( c_str, (FILE*)fp );
}

size_t
Znk_fread( uint8_t* buf, const size_t req_byte_size, ZnkFile fp )
{
	return fread( buf, 1, req_byte_size, (FILE*)fp );
}
size_t
Znk_fread_blk( uint8_t* buf, const size_t blk_size, const size_t blk_num, ZnkFile fp )
{
	return fread( buf, blk_size, blk_num, (FILE*)fp );
}
size_t
Znk_fwrite( const uint8_t* buf, const size_t req_byte_size, ZnkFile fp )
{
	return fwrite( buf, 1, req_byte_size, (FILE*)fp );
}
size_t
Znk_fwrite_blk( const uint8_t* buf, const size_t blk_size, const size_t blk_num, ZnkFile fp )
{
	return fwrite( buf, blk_size, blk_num, (FILE*)fp );
}

bool Znk_feof( ZnkFile fp )
{
	return (bool)( feof( (FILE*)fp ) != 0 );
}
bool Znk_fflush( ZnkFile fp )
{
	return (bool)( fflush( (FILE*)fp ) == 0 );
}
bool Znk_fseek( ZnkFile fp, long offset, int whence ) 
{
	return (bool)( fseek( (FILE*)fp, offset, whence ) == 0 );
}
bool Znk_fseek_i64( ZnkFile fp, int64_t offset, int whence ) 
{
	if( fp ){
		FILE* f = (FILE*)fp;
#if   defined(__MINGW32__) || defined(__MINGW64__)
		return (bool)( fseeko64( f, offset, whence ) == 0 );
#elif defined(__GNUC__) && defined(__linux__)
		off_t off = offset;
		return (bool)( fseeko( f, off, whence ) == 0 );
#elif defined(_MSC_VER) && _MSC_VER >= 1400
		return (bool)( _fseeki64( f, offset, whence ) == 0 );
#else
		/***
		 * 古いVC, bcc, dmcにおいてはどうやればint64_tでfseekできるのかはまだわからない.
		 */
		return (bool)( fseek( f, (long)offset, whence ) == 0 );
#endif
	}
	return false;
}
int64_t Znk_ftell_i64( ZnkFile fp ) 
{
	if( fp ){
		FILE* f = (FILE*)fp;
#if   defined(__MINGW32__) || defined(__MINGW64__)
		return ftello64( f );
#elif defined(__GNUC__) && defined(__linux__)
		return ftello( f );
#elif defined(_MSC_VER) && _MSC_VER >= 1400
		return _ftelli64( f );
#else
		/***
		 * 古いVC, bcc, dmcにおいてはどうやればint64_tでftellできるのかはまだわからない.
		 */
		return ftell( f );
#endif
	}
	return false;
}

static int
vfprintf_C99( FILE* fp, const char* fmt, va_list ap )
{
	char buf[ 4096 ];
	int str_len;
	str_len = Znk_vsnprintf_C99( buf, sizeof(buf), fmt, ap );
	if( str_len > 0 ){
		if( str_len < sizeof(buf) ){
			/***
			 * fpがstdoutかつstdoutがバイナリモードではない場合(テキストモードである場合）、
			 * これをfwriteに指定するのはあまりよろしくない.
			 * このとき何が起こるかは完全に処理系に依存している可能性がある.
			 * 例えば指定文字列がNULL終端していない場合や、NULL終端していても実際の文字列長を超えた値を
			 * sizeとして指定した場合などに問題が発生するかもしれない.
			 * 手元のWindowsではたまたま問題がないが、他の処理系でもそうであるとは限らない.
			 *
			 * よってここでは一度表示文字列を完成させ、fputsさせるのがもっとも確実と判断する.
			 */
			fputs( buf, fp );
		} else {
			/***
			 * str_lenが4096以上である場合、動的に確保した一時バッファを使う.
			 * 確保/解放を伴うため低速だが、そもそもこれほど長い文字列を出力させることは滅多にない上
			 * 長い文字列を扱うことそのものにかかる時間と比較した場合のコストとしては許容範囲内であると考える.
			 */
			ZnkBfr tmp =  ZnkBfr_create( NULL, str_len+1, false, ZnkBfr_Pad8 );
			str_len = Znk_vsnprintf_C99( (char*)ZnkBfr_data(tmp), ZnkBfr_size(tmp), fmt, ap );
			fputs( (char*)ZnkBfr_data(tmp), fp );
			ZnkBfr_destroy( tmp );
		}
	}
	return str_len;
}


/***
 * va_start <=> va_end 間における複数回呼び出しに対応.
 * つまり下記のようなコードを許容する.
 *
 * va_start(ap, fmt);
 * Znk_vfprintf( fp, fmt, ap );
 * Znk_vfprintf( fp, fmt, ap ); // なんらかの理由でもう一度実行したい場合
 * va_end(ap);
 *
 * そのため、以下の実装では va_copyで一旦 ap のクローンを作ってから
 * それを使って処理している.
 */
int
Znk_vfprintf( ZnkFile fp, const char* fmt, va_list ap )
{
	int ret;
	va_list ap_copy;
	va_copy( ap_copy, ap );
	ret = vfprintf_C99( (FILE*)fp, fmt, ap_copy );
	va_end( ap_copy );
	return ret;
}
int
Znk_fprintf( ZnkFile fp, const char* fmt, ... )
{
	int str_len;
	va_list ap;
	va_start(ap, fmt);
	str_len = vfprintf_C99( (FILE*)fp, fmt, ap );
	va_end(ap);
	return str_len;
}
int
Znk_printf( const char* fmt, ... )
{
	int str_len;
	va_list ap;
	va_start(ap, fmt);
	str_len = vfprintf_C99( stdout, fmt, ap );
	va_end(ap);
	return str_len;
}
int
Znk_printf_e( const char* fmt, ... )
{
	int str_len;
	va_list ap;
	va_start(ap, fmt);
	str_len = vfprintf_C99( stderr, fmt, ap );
	va_end(ap);
	return str_len;
}

ZnkFile
Znk_freopen( const char* filename, const char* mode, ZnkFile fp )
{
	return (ZnkFile)freopen( filename, mode, (FILE*)fp );
}
