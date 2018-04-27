#ifndef INCLUDE_GUARD__Est_record_h__
#define INCLUDE_GUARD__Est_record_h__

#include <Znk_myf.h>
#include <Znk_primp_ary.h>
#include <Znk_date.h>

Znk_EXTERN_C_BEGIN

size_t
EstRecord_regist( ZnkMyf myf, const char* id,
		const char* file_path, const char* file_tags, const ZnkDate* date, uint64_t file_size, const char* comment,
		bool is_marge_tags, bool is_modesty );

bool
EstRecord_load( ZnkMyf myf, const char* tmpdir, const char* filename );

bool
EstRecord_save( ZnkMyf myf, const char* tmpdir, const char* filename );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
