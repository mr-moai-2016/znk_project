#include "Rano_log.h"
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <stdarg.h>

static char st_filename[ 256 ] = "";
static ZnkFile st_fp = NULL;

void
RanoLog_open( const char* filename, bool keep_open, bool additional )
{
	/* ëΩèdopenñhé~ */
	if( st_fp == NULL ){
		ZnkFile fp = Znk_fopen( filename, additional ? "ab" : "wb" );
		if( fp ){
			ZnkS_copy( st_filename, sizeof(st_filename), filename, Znk_NPOS );
			if( keep_open ){
				st_fp = fp;
			} else {
				Znk_fclose( fp );
			}
		}
	}
}
void
RanoLog_close( void )
{
	if( st_fp ){
		Znk_fclose( st_fp );
	}
	st_filename[ 0 ] = '\0';
	st_fp = NULL;
}

void
RanoLog_printf( const char* fmt, ... )
{
	va_list ap;
	int str_len;
	Znk_UNUSED( str_len );
	va_start(ap, fmt);
	if( st_fp ){
		str_len = Znk_vfprintf( st_fp, fmt, ap );
		Znk_fflush( st_fp );
	} else if( !ZnkS_empty(st_filename) ){
		ZnkFile fp = Znk_fopen( st_filename, "ab" );
		if( fp ){
			str_len = Znk_vfprintf( fp, fmt, ap );
			Znk_fclose( fp );
		} else {
		}
	} else {
	}
	Znk_vfprintf( Znk_stderr(), fmt, ap );
	va_end(ap);
	return;
}
