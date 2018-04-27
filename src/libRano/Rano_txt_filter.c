#include <Rano_txt_filter.h>
#include <Znk_missing_libc.h>
#include <Znk_s_base.h>
#include <Znk_obj_ary.h>
#include <Znk_str_ptn.h>
#include <Znk_mem_find.h>
#include <Znk_str_ex.h>
#include <string.h>

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

static struct CmdInfo {
	const char*       cmd_name_;
	RanoTxtFilterType cmd_type_;
	size_t            arg_num_;
} st_cmdinfo_table[] = {
	/* replace ['old_ptn'] ['new_ptn'] という形式 */
	{ "replace",     RanoTxtFilter_e_Replace,    2, },

	/* replace_inq ['old_ptn'] ['new_ptn'] ['quote_begin'] ['quote_end'] という形式 */
	{ "replace_inq", RanoTxtFilter_e_ReplaceINQ, 4, },
};

RanoTxtFilter
RanoTxtFilterAry_regist_byCommand( RanoTxtFilterAry fltr_ary,
		const char* command, const char* quote_begin, const char* quote_end )
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

	for( tbl_idx=0; tbl_idx<Znk_NARY(st_cmdinfo_table); ++tbl_idx ){
		const char*       cmd_name = st_cmdinfo_table[ tbl_idx ].cmd_name_;
		RanoTxtFilterType cmd_type = st_cmdinfo_table[ tbl_idx ].cmd_type_;
		const size_t      arg_num  = st_cmdinfo_table[ tbl_idx ].arg_num_;

		q = p;
		/* skip non-whitespace */
		while( *q != '\0' &&
			*q != ' ' && *q != '\t' && *q != '\r' && *q != '\n' ) ++q;

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
			}
			ZnkObjAry_push_bk( fltr_ary->obj_ary_, (ZnkObj)fltr );
			return fltr;
		}
	}

FUNC_ERROR:
	deleteRanoTxtFilter( fltr );
	return NULL;
}

static struct CmdInfo*
findCmdInfo_byType( RanoTxtFilterType query_type )
{
	size_t tbl_idx;
	for( tbl_idx=0; tbl_idx<Znk_NARY(st_cmdinfo_table); ++tbl_idx ){
		struct CmdInfo* info = st_cmdinfo_table + tbl_idx;
		if( info->cmd_type_ == query_type ){
			return info;
		}
	}
	return NULL;
}
bool
RanoTxtFilter_writeCommand( const RanoTxtFilter fltr, ZnkStr ans, const char* quote_begin, const char* quote_end )
{
	struct CmdInfo* info = findCmdInfo_byType( fltr->type_ );
	if( info ){
		size_t arg_idx;
		ZnkStr_set( ans, info->cmd_name_ );
		for( arg_idx=0; arg_idx<info->arg_num_; ++arg_idx ){
			ZnkStr_addf2( ans, " %s%s%s", quote_begin, ZnkStrAry_at_cstr( fltr->args_, arg_idx ), quote_end );
		}
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
RanoTxtFilterAry_exec( const RanoTxtFilterAry fltr_ary, ZnkStr text )
{
	const size_t size = RanoTxtFilterAry_size( fltr_ary );
	size_t idx;
	static const size_t delta_to_next = Znk_NPOS;
	RanoTxtFilter fltr = NULL;
	for( idx=0; idx<size; ++idx ){
		fltr = RanoTxtFilterAry_at( fltr_ary, idx );
		switch( fltr->type_ ){
		case RanoTxtFilter_e_Replace:
		{
			const char* old_ptn = ZnkStrAry_at_cstr( fltr->args_, 0 );
			const char* new_ptn = ZnkStrAry_at_cstr( fltr->args_, 1 );
			ZnkStrEx_replace_BF( text, 0,
					old_ptn, Znk_NPOS,
					new_ptn, Znk_NPOS,
					Znk_NPOS, delta_to_next );
			break;
		}
		case RanoTxtFilter_e_ReplaceINQ:
		{
			const char* old_ptn     = ZnkStrAry_at_cstr( fltr->args_, 0 );
			const char* new_ptn     = ZnkStrAry_at_cstr( fltr->args_, 1 );
			const char* quote_begin = ZnkStrAry_at_cstr( fltr->args_, 2 );
			const char* quote_end   = ZnkStrAry_at_cstr( fltr->args_, 3 );
			const char* coesc_begin = ZnkStrAry_at_cstr( fltr->args_, 4 );
			const char* coesc_end   = ZnkStrAry_at_cstr( fltr->args_, 5 );
			ZnkStrPtn_replaceInQuote( text,
					quote_begin, quote_end,
					coesc_begin, coesc_end,
					old_ptn, new_ptn, false, delta_to_next );
			break;
		}
		default:
			break;
		}
	}
}

