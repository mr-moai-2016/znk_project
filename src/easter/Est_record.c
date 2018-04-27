#include "Est_record.h"
#include "Est_base.h"

#include <Rano_sset.h>

#include <Znk_dir.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <Znk_str_ex.h>
#include <Znk_date.h>
#include <Znk_htp_util.h>
#include <Znk_mem_find.h>

size_t
EstRecord_regist( ZnkMyf myf, const char* id,
		const char* file_path, const char* file_tags, const ZnkDate* date, uint64_t file_size, const char* comment,
		bool is_marge_tags, bool is_modesty )
{
	size_t     processed_count = 0;
	ZnkVarpAry vars    = NULL;
	ZnkStr     str_dst = NULL;

	if( is_modesty ){
		vars = ZnkMyf_find_vars( myf, id );
		if( vars == NULL ){
			goto FUNC_END;
		}
	} else {
		vars = ZnkMyf_intern_vars( myf, id );
	}

	EstBase_registStrToVars( vars, "file_path", file_path );

	str_dst = EstBase_registStrToVars( vars, "file_tags", NULL );
	if( !is_marge_tags ){
		ZnkStr_clear( str_dst );
	}
	processed_count = RanoSSet_addSeq( str_dst, file_tags );

	str_dst = EstBase_registStrToVars( vars, "access_time", NULL );
	ZnkDate_getStr( str_dst, 0, date, ZnkDateStr_e_Std );

	str_dst = EstBase_registStrToVars( vars, "file_size", NULL );
	ZnkStr_setf( str_dst, "%I64u", file_size );

	str_dst = EstBase_registStrToVars( vars, "comment", comment );
	EstBase_escapeToAmpForm( str_dst );

FUNC_END:
	return processed_count;
}

bool
EstRecord_load( ZnkMyf myf, const char* tmpdir, const char* filename )
{
	char dat_path[ 256 ] = "";
	Znk_snprintf( dat_path, sizeof(dat_path), "./%s/%s", tmpdir, filename );
	return ZnkMyf_load( myf, dat_path );
}

bool
EstRecord_save( ZnkMyf myf, const char* tmpdir, const char* filename )
{
	char dat_path[ 256 ] = "";
	ZnkDir_mkdirPath( tmpdir, Znk_NPOS, '/', NULL );
	Znk_snprintf( dat_path, sizeof(dat_path), "./%s/%s", tmpdir, filename );
	return ZnkMyf_save( myf, dat_path );
}

