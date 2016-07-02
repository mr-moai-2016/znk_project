#ifndef INCLUDE_GUARD__Znk_txt_filter_h__
#define INCLUDE_GUARD__Znk_txt_filter_h__

#include <Znk_str_ary.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 ZnkTxtFilter_e_Replace = 0
	,ZnkTxtFilter_e_ReplaceBetween
} ZnkTxtFilterType;

typedef struct ZnkTxtFilter_tag {
	ZnkTxtFilterType type_;
	ZnkStr old_ptn_;
	ZnkStr new_ptn_;
} ZnkTxtFilter;

typedef struct ZnkTxtFilterAryImpl* ZnkTxtFilterAry;

ZnkTxtFilterAry
ZnkTxtFilterAry_create( void );

void
ZnkTxtFilterAry_destroy( ZnkTxtFilterAry fltr_ary );

ZnkTxtFilter*
ZnkTxtFilterAry_regist_byCommand( ZnkTxtFilterAry fltr_ary,
		const char* command, const char* quote_begin, const char* quote_end );

Znk_INLINE void
ZnkTxtFilterAry_regist_byCommandAry( ZnkTxtFilterAry fltr_ary,
		ZnkStrAry command_ary, const char* quote_begin, const char* quote_end )
{
	const size_t size = ZnkStrAry_size( command_ary );
	size_t idx;
	const char* command;
	for( idx=0; idx<size; ++idx ){
		command = ZnkStrAry_at_cstr( command_ary, idx );
		ZnkTxtFilterAry_regist_byCommand( fltr_ary, command, quote_begin, quote_end );
	}
}

ZnkTxtFilter*
ZnkTxtFilterAry_at( const ZnkTxtFilterAry fltr_ary, size_t idx );

size_t
ZnkTxtFilterAry_size( const ZnkTxtFilterAry fltr_ary );

void
ZnkTxtFilterAry_exec( const ZnkTxtFilterAry fltr_ary, ZnkStr text );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
