#ifndef INCLUDE_GUARD__Est_linf_h__
#define INCLUDE_GUARD__Est_linf_h__

#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

typedef struct EstLInfImpl* EstLInf;

void
EstLInfList_clear( ZnkVarpAry linf_list );

size_t
EstLInfList_find( ZnkVarpAry linf_list, const char* query_id );

EstLInf
EstLInfList_add( ZnkVarpAry linf_list, const char* id, const char* url, const char* comment );

EstLInf
EstLInfList_regist( ZnkVarpAry linf_list, const char* id, const char* url, const char* comment );

ZnkVarpAry
EstLInfList_create( void );

void
EstLInfList_destroy( ZnkVarpAry linf_list );

size_t
EstLInfList_size( const ZnkVarpAry linf_list );

EstLInf
EstLInfList_at( const ZnkVarpAry linf_list, size_t idx );

void
EstLInfList_shiftToLast( ZnkVarpAry linf_list, size_t target_idx );

bool
EstLInfList_erase_byIdx( ZnkVarpAry linf_list, size_t target_idx );

size_t
EstLInfList_findIdx_byName( ZnkVarpAry linf_list, const char* query_name );

const char*
EstLInf_id( const EstLInf linf );

const char*
EstLInf_url( const EstLInf linf );

const char*
EstLInf_comment( const EstLInf linf );

void
EstLInf_set_url( const EstLInf linf, const char* url );

void
EstLInf_set_comment( const EstLInf linf, const char* comment );

bool
EstLInfList_load( ZnkVarpAry linf_list, const char* filename );

bool
EstLInfList_save( ZnkVarpAry linf_list, const char* filename );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
