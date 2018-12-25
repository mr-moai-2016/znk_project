#ifndef INCLUDE_GUARD__Rano_txt_filter_h__
#define INCLUDE_GUARD__Rano_txt_filter_h__

#include <Znk_str_ary.h>

Znk_EXTERN_C_BEGIN

typedef struct RanoTxtFilterImpl* RanoTxtFilter;

typedef bool (*RanoTxtFilterExecFunc_T)( ZnkStr text, uintptr_t cmd_type, ZnkStrAry args, void* exec_arg );
typedef bool (*RanoTxtFilterConvertStrFunc_T)( ZnkStr str, uintptr_t cmd_type );


typedef struct RanoTxtFilterCmdInfo_tag {
	const char* cmd_name_;
	uintptr_t   cmd_type_;
	uintptr_t   arg_num_;
} RanoTxtFilterCmdInfo;

typedef struct RanoTxtFilterAryImpl* RanoTxtFilterAry;

RanoTxtFilterAry
RanoTxtFilterAry_create( void );

void
RanoTxtFilterAry_destroy( RanoTxtFilterAry fltr_ary );

RanoTxtFilter
RanoTxtFilterAry_regist_byCommand( RanoTxtFilterAry fltr_ary, const char* command,
		const char* quote_begin, const char* quote_end,
		RanoTxtFilterCmdInfo* cmdinfo_table, size_t cmdinfo_table_size,
		RanoTxtFilterConvertStrFunc_T unescape_inquote );

Znk_INLINE void
RanoTxtFilterAry_regist_byCommandAry( RanoTxtFilterAry fltr_ary, ZnkStrAry command_ary,
		const char* quote_begin, const char* quote_end,
		RanoTxtFilterCmdInfo* cmdinfo_table, size_t cmdinfo_table_size,
		RanoTxtFilterConvertStrFunc_T unescape_inquote )
{
	const size_t size = ZnkStrAry_size( command_ary );
	size_t idx;
	const char* command;
	for( idx=0; idx<size; ++idx ){
		command = ZnkStrAry_at_cstr( command_ary, idx );
		RanoTxtFilterAry_regist_byCommand( fltr_ary,
				command, quote_begin, quote_end,
				cmdinfo_table, cmdinfo_table_size,
				unescape_inquote );
	}
}

bool
RanoTxtFilter_writeCommand( const RanoTxtFilter fltr, ZnkStr ans, const char* quote_begin, const char* quote_end,
		RanoTxtFilterCmdInfo* cmdinfo_table, size_t cmdinfo_table_size,
		RanoTxtFilterConvertStrFunc_T escape_inquote );

RanoTxtFilter
RanoTxtFilterAry_at( const RanoTxtFilterAry fltr_ary, size_t idx );

size_t
RanoTxtFilterAry_size( const RanoTxtFilterAry fltr_ary );

void
RanoTxtFilterAry_exec( const RanoTxtFilterAry fltr_ary, ZnkStr text, RanoTxtFilterExecFunc_T exec_func, void* exec_arg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
