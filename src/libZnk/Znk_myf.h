#ifndef INCLUDE_GUARD__Znk_myf_h__
#define INCLUDE_GUARD__Znk_myf_h__

#include <Znk_var.h>
#include <Znk_str_ary.h>
#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkMyfSectionImpl* ZnkMyfSection;
typedef struct ZnkMyfImpl* ZnkMyf;

typedef enum {
	 ZnkMyfSection_e_None = 0
	,ZnkMyfSection_e_Lines
	,ZnkMyfSection_e_Vars
	,ZnkMyfSection_e_OutOfSection
} ZnkMyfSectionType;

ZnkStrAry
ZnkMyfSection_lines( const ZnkMyfSection sec );

ZnkVarpAry
ZnkMyfSection_vars( const ZnkMyfSection sec );

const char*
ZnkMyfSection_name( const ZnkMyfSection sec );


ZnkMyf
ZnkMyf_create( void );
void
ZnkMyf_destroy( ZnkMyf myf );
void
ZnkMyf_clear( ZnkMyf myf );

const char*
ZnkMyf_quote_begin( const ZnkMyf myf );
const char*
ZnkMyf_quote_end( const ZnkMyf myf );

ZnkMyfSection
ZnkMyf_registSection( ZnkMyf myf, const char* name, ZnkMyfSectionType type );

size_t
ZnkMyf_numOfSection( const ZnkMyf myf );
ZnkMyfSection
ZnkMyf_atSection( const ZnkMyf myf, size_t sec_idx );

bool
ZnkMyf_load( ZnkMyf myf, const char* filename );
bool
ZnkMyf_save( ZnkMyf myf, const char* filename );

ZnkVarpAry
ZnkMyf_find_vars( const ZnkMyf myf, const char* section_name );
ZnkStrAry
ZnkMyf_find_lines( const ZnkMyf myf, const char* section_name );

ZnkVarp
ZnkMyf_refVar( ZnkMyf myf, const char* section_name, const char* var_name );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
