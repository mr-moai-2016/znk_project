#ifndef INCLUDE_GUARD__Rano_txt_filter_h__
#define INCLUDE_GUARD__Rano_txt_filter_h__

#include <Znk_str_ary.h>

Znk_EXTERN_C_BEGIN

typedef struct RanoTxtFilterImpl* RanoTxtFilter;

typedef enum {
	 RanoTxtFilter_e_Replace = 0
	,RanoTxtFilter_e_ReplaceINQ
} RanoTxtFilterType;

typedef struct RanoTxtFilterAryImpl* RanoTxtFilterAry;

RanoTxtFilterAry
RanoTxtFilterAry_create( void );

void
RanoTxtFilterAry_destroy( RanoTxtFilterAry fltr_ary );

RanoTxtFilter
RanoTxtFilterAry_regist_byCommand( RanoTxtFilterAry fltr_ary,
		const char* command, const char* quote_begin, const char* quote_end );

Znk_INLINE void
RanoTxtFilterAry_regist_byCommandAry( RanoTxtFilterAry fltr_ary,
		ZnkStrAry command_ary, const char* quote_begin, const char* quote_end )
{
	const size_t size = ZnkStrAry_size( command_ary );
	size_t idx;
	const char* command;
	for( idx=0; idx<size; ++idx ){
		command = ZnkStrAry_at_cstr( command_ary, idx );
		RanoTxtFilterAry_regist_byCommand( fltr_ary, command, quote_begin, quote_end );
	}
}

bool
RanoTxtFilter_writeCommand( const RanoTxtFilter fltr, ZnkStr ans, const char* quote_begin, const char* quote_end );

RanoTxtFilter
RanoTxtFilterAry_at( const RanoTxtFilterAry fltr_ary, size_t idx );

size_t
RanoTxtFilterAry_size( const RanoTxtFilterAry fltr_ary );

void
RanoTxtFilterAry_exec( const RanoTxtFilterAry fltr_ary, ZnkStr text );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
