#ifndef INCLUDE_GUARD__Est_sqi_h__
#define INCLUDE_GUARD__Est_sqi_h__

#include <Znk_varp_ary.h>
#include <Znk_date.h>

Znk_EXTERN_C_BEGIN

typedef struct EstSQIImpl* EstSQI;

const char*
EstSQI_search_path( const EstSQI sqi );
ZnkStrAry
EstSQI_file_ext( const EstSQI sqi );

ZnkStrAry
EstSQI_tags( const EstSQI sqi );
size_t
EstSQI_tags_num_min( const EstSQI sqi );
size_t
EstSQI_tags_num_max( const EstSQI sqi );
bool
EstSQI_is_tags_method_or( const EstSQI sqi );
bool
EstSQI_isMatchTags( const EstSQI sqi, const ZnkStrAry tags_list );
ZnkDate
EstSQI_date( const EstSQI sqi );
bool
EstSQI_isMatchDate( const EstSQI sqi, const ZnkDate* date );
size_t
EstSQI_size( const EstSQI sqi );
bool
EstSQI_isMatchSize( const EstSQI sqi, uint64_t file_size64u );
const char*
EstSQI_keyword( const EstSQI sqi );
ZnkStr
EstSQI_result_msg( const EstSQI sqi );

EstSQI
EstSQI_create( ZnkVarpAry post_vars );

void
EstSQI_destroy( EstSQI info );

bool
EstSQI_convert_toSQIVars( ZnkVarpAry sqi_vars, EstSQI sqi );

void
EstSQIVars_makeVars( ZnkVarpAry sqi_vars,
		const char* topdir, const char* file_ext,
		const char* tags, const char* tags_method, const char* tags_num,
		const char* keyword,
		const char* time, const char* time_method,
		const char* size, const char* size_method );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
