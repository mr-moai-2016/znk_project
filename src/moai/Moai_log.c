#include "Moai_log.h"
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <stdarg.h>

static const char* st_filename = NULL;
static ZnkFile st_fp = NULL;

void
MoaiLog_open( const char* filename, bool keep_open )
{
	ZnkFile fp = ZnkF_fopen( filename, "wb" );
	if( fp ){
		st_filename = filename;
		if( keep_open ){
			st_fp = fp;
		} else {
			ZnkF_fclose( fp );
		}
	}
}
void
MoaiLog_close( void )
{
	if( st_fp ){
		ZnkF_fclose( st_fp );
	}
	st_filename = NULL;
	st_fp = NULL;
}

void
MoaiLog_printf( const char* fmt, ... )
{
	va_list ap;
	int str_len;
	Znk_UNUSED( str_len );
	va_start(ap, fmt);
	if( st_fp ){
		str_len = ZnkF_vfprintf( st_fp, fmt, ap );
	} else if( !ZnkS_empty(st_filename) ){
		ZnkFile fp = ZnkF_fopen( st_filename, "ab" );
		if( fp ){
			str_len = ZnkF_vfprintf( fp, fmt, ap );
			ZnkF_fclose( fp );
		} else {
		}
	} else {
	}
	ZnkF_vfprintf( ZnkF_stderr(), fmt, ap );
	va_end(ap);
	return;
}
