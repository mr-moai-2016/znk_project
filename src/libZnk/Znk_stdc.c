#if defined(__GNUC__) && defined(__linux__)
#  define _FILE_OFFSET_BITS 64
#endif

#include "Znk_stdc.h"
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
#  if defined(__BORLANDC__)
	setmode( no, is_binary_mode ? _O_BINARY : _O_TEXT );
#  else
	_setmode( no, is_binary_mode ? _O_BINARY : _O_TEXT );
#  endif
#endif
}

ZnkFile
ZnkF_fopen( const char* filename, const char* mode ){
	return (ZnkFile)fopen( filename, mode );
}
void
ZnkF_fclose( ZnkFile fp )
{
	if( fp ){
		fclose( (FILE*)fp );
	}
}

int
ZnkF_fgetc( ZnkFile fp )
{
	return fgetc( (FILE*)fp );
}
int
ZnkF_fputc( int c, ZnkFile fp )
{
	return fputc( c, (FILE*)fp );
}

char*
ZnkF_fgets( char* buf, size_t size, ZnkFile fp )
{
	return fgets( buf, (int)size, (FILE*)fp );
}
int
ZnkF_fputs( const char* c_str, ZnkFile fp )
{
	return fputs( c_str, (FILE*)fp );
}

size_t
ZnkF_fread( uint8_t* buf, const size_t blk_size, const size_t nmemb, ZnkFile fp )
{
	return fread( buf, blk_size, nmemb, (FILE*)fp );
}
size_t
ZnkF_fwrite( const uint8_t* buf, const size_t blk_size, const size_t nmemb, ZnkFile fp )
{
	return fwrite( buf, blk_size, nmemb, (FILE*)fp );
}

bool ZnkF_feof( ZnkFile fp )
{
	return (bool)( feof( (FILE*)fp ) != 0 );
}
bool ZnkF_fflush( ZnkFile fp )
{
	return (bool)( fflush( (FILE*)fp ) == 0 );
}
bool ZnkF_fseek( ZnkFile fp, long offset, int whence ) 
{
	return (bool)( fseek( (FILE*)fp, offset, whence ) == 0 );
}
bool ZnkF_fseek_i64( ZnkFile fp, int64_t offset, int whence ) 
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
		 * ŒÃ‚¢VC, bcc, dmc‚É‚¨‚¢‚Ä‚Í‚Ç‚¤‚â‚ê‚Îint64_t‚Åfseek‚Å‚«‚é‚Ì‚©‚Í‚Ü‚¾‚í‚©‚ç‚È‚¢.
		 */
		return (bool)( fseek( f, (long)offset, whence ) == 0 );
#endif
	}
	return false;
}
int64_t ZnkF_ftell_i64( ZnkFile fp ) 
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
		 * ŒÃ‚¢VC, bcc, dmc‚É‚¨‚¢‚Ä‚Í‚Ç‚¤‚â‚ê‚Îint64_t‚Åftell‚Å‚«‚é‚Ì‚©‚Í‚Ü‚¾‚í‚©‚ç‚È‚¢.
		 */
		return ftell( f );
#endif
	}
	return false;
}

/***
 * va_start <=> va_end ŠÔ‚É‚¨‚¯‚é•¡”‰ñŒÄ‚Ño‚µ‚É‘Î‰.
 */
int
ZnkF_vfprintf( ZnkFile fp, const char* fmt, va_list ap )
{
	int ret;
	va_list ap_copy;
	va_copy( ap_copy, ap );
	ret = vfprintf( (FILE*)fp, fmt, ap_copy );
	va_end( ap_copy );
	return ret;
}
int
ZnkF_fprintf( ZnkFile fp, const char* fmt, ... )
{
	int str_len;
	va_list ap;
	va_start(ap, fmt);
	str_len = vfprintf( (FILE*)fp, fmt, ap );
	va_end(ap);
	return str_len;
}
int
ZnkF_printf( const char* fmt, ... )
{
	int str_len;
	va_list ap;
	va_start(ap, fmt);
	str_len = vfprintf( stdout, fmt, ap );
	va_end(ap);
	return str_len;
}
int
ZnkF_printf_e( const char* fmt, ... )
{
	int str_len;
	va_list ap;
	va_start(ap, fmt);
	str_len = vfprintf( stderr, fmt, ap );
	va_end(ap);
	return str_len;
}

ZnkFile
ZnkF_freopen( const char* filename, const char* mode, ZnkFile fp )
{
	return (ZnkFile)freopen( filename, mode, (FILE*)fp );
}
