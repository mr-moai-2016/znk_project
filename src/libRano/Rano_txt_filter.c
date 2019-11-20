#include <Rano_txt_filter.h>

#include <Znk_missing_libc.h>
#include <Znk_s_base.h>
#include <Znk_obj_ary.h>
#include <Znk_str_ptn.h>
#include <Znk_mem_find.h>
#include <Znk_str_ex.h>
#include <string.h>


typedef uintptr_t RanoTxtFilterType;

struct RanoTxtFilterImpl {
	RanoTxtFilterType type_;
	ZnkStrAry args_;
};

struct RanoTxtFilterAryImpl {
	ZnkObjAry obj_ary_;
};

static RanoTxtFilter
makeRanoTxtFilter( RanoTxtFilterType type )
{
	RanoTxtFilter fltr = (RanoTxtFilter)Znk_malloc( sizeof( struct RanoTxtFilterImpl ) );
	fltr->type_ = type;
	fltr->args_ = ZnkStrAry_create( true );
	return fltr;
}
static void
deleteRanoTxtFilter( void* elem )
{
	RanoTxtFilter fltr = (RanoTxtFilter)elem;
	if( fltr ){
		ZnkStrAry_destroy( fltr->args_ );
		Znk_free( fltr );
	}
}

RanoTxtFilterAry
RanoTxtFilterAry_create( void )
{
	RanoTxtFilterAry fltr_ary = (RanoTxtFilterAry)Znk_malloc( sizeof( struct RanoTxtFilterAryImpl ) );
	fltr_ary->obj_ary_ = ZnkObjAry_create( deleteRanoTxtFilter );
	return fltr_ary;
}

void
RanoTxtFilterAry_destroy( RanoTxtFilterAry fltr_ary )
{
	if( fltr_ary ){
		ZnkObjAry_destroy( fltr_ary->obj_ary_ );
		Znk_free( fltr_ary );
	}
}

RanoTxtFilter
RanoTxtFilterAry_regist_byCommand( RanoTxtFilterAry fltr_ary,
		const char* command, const char* quote_begin, const char* quote_end,
		RanoTxtFilterCmdInfo* cmdinfo_table, size_t cmdinfo_table_size,
		RanoTxtFilterConvertStrFunc_T unescape_inquote )
{
	const char* p = command;
	const char* q = NULL;
	const size_t quote_begin_leng = strlen( quote_begin );
	const size_t quote_end_leng   = strlen( quote_end );
	RanoTxtFilter fltr = NULL;
	size_t tbl_idx;
	size_t arg_idx;

	/* skip whitespace */
	while( *p == ' ' || *p == '\t' ) ++p;

	q = p;
	/* skip non-whitespace */
	while( *q != '\0' &&
		*q != ' ' && *q != '\t' && *q != '\r' && *q != '\n' ) ++q;

	for( tbl_idx=0; tbl_idx<cmdinfo_table_size; ++tbl_idx ){
		const char*       cmd_name = cmdinfo_table[ tbl_idx ].cmd_name_;
		RanoTxtFilterType cmd_type = cmdinfo_table[ tbl_idx ].cmd_type_;
		const size_t      arg_num  = cmdinfo_table[ tbl_idx ].arg_num_;

		if( (size_t)(q-p) == strlen(cmd_name) && ZnkS_eqEx( p, cmd_name, q-p ) ){
			p = q;
			fltr = makeRanoTxtFilter( cmd_type );

			ZnkStrAry_resize( fltr->args_, arg_num, "" );

			/***
			 * とりあえず現時点ではquoteを必須とする.
			 */
			for( arg_idx=0; arg_idx<arg_num; ++arg_idx ){
				p = strstr( p, quote_begin );
				if( p == NULL ){ goto FUNC_ERROR; }
				p += quote_begin_leng;
			
				q = strstr( p, quote_end );
				if( p == NULL ){ goto FUNC_ERROR; }
				ZnkStr_assign( ZnkStrAry_at(fltr->args_,arg_idx), 0, p, q-p );
				p = q + quote_end_leng;

				if( unescape_inquote ){
					unescape_inquote( ZnkStrAry_at(fltr->args_,arg_idx), cmd_type );
				}
			}
			ZnkObjAry_push_bk( fltr_ary->obj_ary_, (ZnkObj)fltr );
			return fltr;
		}
	}

FUNC_ERROR:
	deleteRanoTxtFilter( fltr );
	return NULL;
}

static RanoTxtFilterCmdInfo*
findCmdInfo_byType( RanoTxtFilterType query_type,
		RanoTxtFilterCmdInfo* cmdinfo_table, size_t cmdinfo_table_size )
{
	size_t tbl_idx;
	for( tbl_idx=0; tbl_idx<cmdinfo_table_size; ++tbl_idx ){
		RanoTxtFilterCmdInfo* info = cmdinfo_table + tbl_idx;
		if( info->cmd_type_ == query_type ){
			return info;
		}
	}
	return NULL;
}
bool
RanoTxtFilter_writeCommand( const RanoTxtFilter fltr, ZnkStr ans, const char* quote_begin, const char* quote_end,
		RanoTxtFilterCmdInfo* cmdinfo_table, size_t cmdinfo_table_size,
		RanoTxtFilterConvertStrFunc_T escape_inquote )
{
	RanoTxtFilterCmdInfo* info = findCmdInfo_byType( fltr->type_, cmdinfo_table, cmdinfo_table_size );
	if( info ){
		size_t      arg_idx;
		const char* arg_cstr = NULL;
		ZnkStr      esc_arg  = escape_inquote ? ZnkStr_new( "" ) : NULL;

		ZnkStr_set( ans, info->cmd_name_ );
		for( arg_idx=0; arg_idx<info->arg_num_; ++arg_idx ){
			if( escape_inquote ){
				ZnkStr_copy( esc_arg, ZnkStrAry_at( fltr->args_, arg_idx ) );
				escape_inquote( esc_arg, fltr->type_ );
				arg_cstr = ZnkStr_cstr( esc_arg );
			} else {
				arg_cstr = ZnkStrAry_at_cstr( fltr->args_, arg_idx );
			}
			ZnkStr_addf( ans, " %s%s%s", quote_begin, arg_cstr, quote_end );
		}
		ZnkStr_delete( esc_arg );
		return true;
	}
	return false;
}


RanoTxtFilter
RanoTxtFilterAry_at( const RanoTxtFilterAry fltr_ary, size_t idx )
{
	RanoTxtFilter fltr = (RanoTxtFilter)ZnkObjAry_at( fltr_ary->obj_ary_, idx );
	return fltr;
}
size_t
RanoTxtFilterAry_size( const RanoTxtFilterAry fltr_ary )
{
	return ZnkObjAry_size( fltr_ary->obj_ary_ );
}


void
RanoTxtFilterAry_exec( const RanoTxtFilterAry fltr_ary, ZnkStr text, RanoTxtFilterExecFunc_T exec_func, void* exec_arg )
{
	const size_t size = RanoTxtFilterAry_size( fltr_ary );
	size_t idx;
	RanoTxtFilter fltr = NULL;
	for( idx=0; idx<size; ++idx ){
		fltr = RanoTxtFilterAry_at( fltr_ary, idx );
		exec_func( text, fltr->type_, fltr->args_, exec_arg );
	}
}

void
RanoTxtFilterAry_test( const RanoTxtFilterAry fltr_ary, ZnkStr text )
{
	/* example */
	enum {
		Replace,
		ReplaceINQ,
	};
	static RanoTxtFilterCmdInfo st_cmdinfo_table[] = {
		/* replace ['old_ptn'] ['new_ptn'] という形式 */
		{ "replace",     Replace,    2, },
		/* replace_inq ['old_ptn'] ['new_ptn'] ['quote_begin'] ['quote_end'] という形式 */
		{ "replace_inq", ReplaceINQ, 4, },
	};
	Znk_UNUSED( st_cmdinfo_table );
}
