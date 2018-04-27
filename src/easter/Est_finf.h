#ifndef INCLUDE_GUARD__Est_finf_h__
#define INCLUDE_GUARD__Est_finf_h__

#include <Znk_date.h>
#include <Znk_varp_ary.h>
#include <Est_sqi.h>

Znk_EXTERN_C_BEGIN

typedef struct EstFInfImpl* EstFInf;

void
EstFInfList_clear( ZnkVarpAry finf_list );

size_t
EstFInfList_find( ZnkVarpAry finf_list, const char* query_id );

EstFInf
EstFInfList_add( ZnkVarpAry finf_list, const char* id,
		const char* file_path, const char* file_tags, const ZnkDate* access_time, size_t file_size, const char* comment );

EstFInf
EstFInfList_regist( ZnkVarpAry finf_list, const char* id,
		const char* file_path, const char* file_tags, const ZnkDate* access_time, size_t file_size, const char* comment,
		bool is_marge_tags );

EstFInf
EstFInfList_addFileByMD5( ZnkVarpAry finf_list, const char* box_vname, const char* fsys_path, const char* file_tags, const char* comment );

EstFInf
EstFInfList_registFileByMD5( ZnkVarpAry finf_list, const char* box_vname, const char* fsys_path,
		const char* file_tags, const char* comment, bool is_marge_tags );

ZnkVarpAry
EstFInfList_create( void );

void
EstFInfList_destroy( ZnkVarpAry finf_list );

size_t
EstFInfList_size( const ZnkVarpAry finf_list );

EstFInf
EstFInfList_at( const ZnkVarpAry finf_list, size_t idx );

/**
 * target_idxî‘ñ⁄ÇÃóvëfÇç≈å„îˆÇ÷à⁄ìÆÇ∑ÇÈ.
 */
void
EstFInfList_shiftToLast( ZnkVarpAry finf_list, size_t target_idx );

bool
EstFInfList_erase_byIdx( ZnkVarpAry finf_list, size_t target_idx );

size_t
EstFInfList_findIdx_byName( ZnkVarpAry finf_list, const char* query_name );

const char*
EstFInf_id( const EstFInf finf );

const char*
EstFInf_file_path( const EstFInf finf );

const char*
EstFInf_file_tags( const EstFInf finf );

void
EstFInf_access_time( const EstFInf finf, ZnkDate* access_time );

void
EstFInf_set_access_time( EstFInf finf, const ZnkDate* access_time );

size_t
EstFInf_file_size( const EstFInf finf );

const char*
EstFInf_comment( const EstFInf finf );

bool
EstFInfList_load( ZnkVarpAry finf_list, ZnkVarpAry sqi_vars, const char* filename );
bool
EstFInfList_save( ZnkVarpAry finf_list, EstSQI sqi, const char* filename, bool is_marge_tags, bool is_modesty );
void
EstFInfList_sort( ZnkVarpAry finf_list );


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
