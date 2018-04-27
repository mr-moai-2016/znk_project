#ifndef INCLUDE_GUARD__Znk_stdc_h__
#define INCLUDE_GUARD__Znk_stdc_h__

#include <Znk_base.h>
#include <stdarg.h>

Znk_EXTERN_C_BEGIN

void* Znk_malloc( size_t size );
void  Znk_free( void* ptr );
void* Znk_realloc( void* ptr, size_t size );
void* Znk_calloc( size_t ary_num, size_t elem_size );
void* Znk_alloc0( size_t size );
char* Znk_strdup( const char* cstr );

void* Znk_memcpy( void* dst, const void* src, size_t size );
void* Znk_memmove( void* dst, const void* src, size_t size );
void* Znk_memset( void* dst, uint8_t c, size_t size );
int   Znk_memcmp( const void* m1, const void* m2, size_t size );
void* Znk_memchr( const void* m, int c, size_t size );

size_t  Znk_strlen( const char* cstr );
/**
 * @brief
 *   文字列リテラルの場合、このマクロの方がstrlenより高速に文字列長を取得できる.
 *   literal_cstrには必ず文字列リテラルを与えること.
 *
 * @note
 *   仮にliteral_cstrに文字列リテラル以外(一般の文字列変数や配列など)が与えられた場合、
 *   一応コンパイルエラーとなるように後ろに "" を連結してある.
 *
 *   このマクロの返す値はstrlenと完全には同じでないことに注意する.
 *   例えば、literal_cstr として "abcdef" が与えられた場合は期待通り 6 が返されるが
 *   literal_cstr として "ab\0cdef" が与えられた場合は 2 ではなく 7 が返される.
 *   これはsizeofが文字列リテラルを単純に配列とみなし、そのサイズを返しているためである.
 */
#define Znk_strlen_literal( literal_cstr ) ( sizeof( literal_cstr "" ) - 1 )

int Znk_strcmp(  const char* s1, const char* s2 );
int Znk_strncmp( const char* s1, const char* s2, size_t leng );
char* Znk_strchr( const char* cstr, char c );
char* Znk_strrchr( const char* cstr, char c );
char* Znk_strstr( const char* cstr, const char* ptn );

char* Znk_getenv( const char* varname );


Znk_DECLARE_HANDLE( ZnkFile );

ZnkFile Znk_Internal_getStdFP( int no );

/***
 * @param no:
 *   標準入出力を示すファイルディスクリプタ番号を指定する.
 *   すなわち、
 *   0 は 標準入力
 *   1 は 標準出力
 *   2 は 標準エラー出力
 *   を意味する.
 *
 * @param is_binary_mode:
 *   noで指定した標準入出力をbinary-modeに変更するか否かを指定する.
 *   trueならばbinary-modeに変更する.
 *   falseならばtext-modeに変更する.
 */
void Znk_Internal_setMode( int no, bool is_binary_mode );

Znk_INLINE ZnkFile Znk_stdin(  void ){ return Znk_Internal_getStdFP( 0 ); }
Znk_INLINE ZnkFile Znk_stdout( void ){ return Znk_Internal_getStdFP( 1 ); }
Znk_INLINE ZnkFile Znk_stderr( void ){ return Znk_Internal_getStdFP( 2 ); }


ZnkFile
Znk_fopen( const char* filename, const char* mode );
void
Znk_fclose( ZnkFile fp );

int
Znk_fgetc( ZnkFile fp );
int
Znk_fputc( int c, ZnkFile fp );

char*
Znk_fgets( char* buf, size_t size, ZnkFile fp );
int
Znk_fputs( const char* c_str, ZnkFile fp );

size_t
Znk_fread( uint8_t* buf, const size_t req_byte_size, ZnkFile fp );
size_t
Znk_fread_blk( uint8_t* buf, const size_t blk_size, const size_t blk_num, ZnkFile fp );
size_t
Znk_fwrite( const uint8_t* buf, const size_t req_byte_size, ZnkFile fp );
size_t
Znk_fwrite_blk( const uint8_t* buf, const size_t blk_size, const size_t blk_num, ZnkFile fp );

bool Znk_feof( ZnkFile fp );
bool Znk_fflush( ZnkFile fp );
bool Znk_fseek( ZnkFile fp, long offset, int whence );
bool Znk_fseek_i64( ZnkFile fp, int64_t offset, int whence );
int64_t Znk_ftell_i64( ZnkFile fp );

int
Znk_vfprintf( ZnkFile fp, const char* fmt, va_list ap );
int
Znk_fprintf( ZnkFile fp, const char* fmt, ... );
int
Znk_printf( const char* fmt, ... );
int
Znk_printf_e( const char* fmt, ... );

ZnkFile
Znk_freopen( const char* filename, const char* mode, ZnkFile fp );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
