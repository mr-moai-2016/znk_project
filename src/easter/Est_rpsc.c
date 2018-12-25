#include <Est_rpsc.h>
#include <Est_parser.h>
#include <Est_filter.h>
#include <Est_link.h>
#include <Est_base.h>

#include <Rano_txt_filter.h>

#include <Znk_str_ex.h>
#include <Znk_str_ptn.h>
#include <Znk_stdc.h>
#include <Znk_varp_ary.h>


static int
filterClear( ZnkStr str, void* arg )
{
	ZnkStr_clear( str );
	return 1;
}
static int
filterSrcClearCB( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend )
{
	const bool  end_kind = (bool)( ZnkStr_first(tagname) == '/' );
	const char* tag_kind = end_kind ? ZnkStr_cstr(tagname) + 1 : ZnkStr_cstr(tagname);
	struct EstLinkInfo* link_info = Znk_force_ptr_cast( struct EstLinkInfo*, arg );

	if( ZnkS_eqCase( tag_kind, "iframe" ) ){
		if( !end_kind ){
			ZnkVarp attr = ZnkVarpAry_find_byName_literal( varp_ary, "src", true );
			if( attr ){
				ZnkStr str = EstHtmlAttr_str( attr );
				ZnkStr_clear( str );
			}
		}
	} else if( ZnkS_eqCase( tag_kind, "script" ) ){
		if( !end_kind ){
			ZnkVarp attr = ZnkVarpAry_find_byName_literal( varp_ary, "src", true );
			if( attr ){
				ZnkStr str = EstHtmlAttr_str( attr );
				if( EstFilter_ignoreHosts( str ) ){
					ZnkStr_clear( str );
				} else {
					link_info->est_req_ = EstRequest_e_get; /* íºê⁄éÊìæ */
					return EstLink_filterScript( str, link_info );
				}
			}
		}
	}
	return 0;
}
static int
filterSrcClear( ZnkStr str, void* arg )
{
	EstParser_invokeHtmlTagEx( str, filterSrcClearCB, arg, NULL, NULL, NULL );
	return 1;
}

static bool
exec_one( ZnkStr text, uintptr_t cmd_type, ZnkStrAry args, void* exec_arg )
{
	const int type = Znk_force_ptr_cast( int, cmd_type );
	switch( type ){
	case EstRpsc_e_Replace:
	case EstRpsc_e_ReplaceEx:
	{
		ZnkStr old_ptn = ZnkStrAry_at( args, 0 );
		ZnkStr new_ptn = ZnkStrAry_at( args, 1 );
		ZnkStrEx_replace_BF( text, 0, ZnkStr_cstr(old_ptn), ZnkStr_leng(old_ptn),
				ZnkStr_cstr(new_ptn), ZnkStr_leng(new_ptn), Znk_NPOS, Znk_NPOS ); 
		break;
	}
	case EstRpsc_e_ClearT:
	case EstRpsc_e_ClearF:
	{
		const char* begin_ptn = ZnkStrAry_at_cstr( args, 0 );
		const char* end_ptn   = ZnkStrAry_at_cstr( args, 1 );
		void*       arg       = NULL;
		ZnkStrPtn_invokeInQuote( text,
				begin_ptn, end_ptn,
				NULL, NULL,
				filterClear, arg, (bool)(type == EstRpsc_e_ClearT) );
		break;
	}
	case EstRpsc_e_ClearSrcT:
	case EstRpsc_e_ClearSrcF:
	{
		const char* begin_ptn = ZnkStrAry_at_cstr( args, 0 );
		const char* end_ptn   = ZnkStrAry_at_cstr( args, 1 );
		void*       arg       = exec_arg;
		ZnkStrPtn_invokeInQuote( text,
				begin_ptn, end_ptn,
				NULL, NULL,
				filterSrcClear, arg, (bool)(type == EstRpsc_e_ClearSrcT) );
		break;
	}
	default:
		break;
	}
	return true;
}

bool
EstRpsc_exec( RanoTxtFilterAry fltr_ary, ZnkStr text, void* arg )
{
	RanoTxtFilterAry_exec( fltr_ary, text, exec_one, arg );
	return true;
}

static bool
escapeArg( ZnkStr arg, uintptr_t cmd_type )
{
	if( cmd_type != EstRpsc_e_Replace ){
		EstBase_escapeToAmpForm( arg );
	}
	return true;
}
static bool
unescapeArg( ZnkStr arg, uintptr_t cmd_type )
{
	if( cmd_type != EstRpsc_e_Replace ){
		EstBase_unescapeToAmpForm( arg );
	}
	return true;
}

bool
EstRpsc_compile( RanoTxtFilterAry fltr_ary, const ZnkStrAry cmds )
{
	static RanoTxtFilterCmdInfo st_cmdinfo_table[] = {
		/* replace ['old_ptn'] ['new_ptn'] */
		{ "replace", EstRpsc_e_Replace, 2, },
		/* replace_ex ['old_ptn'] ['new_ptn'] */
		{ "replace_ex", EstRpsc_e_ReplaceEx, 2, },
		/* clear_t ['begin_ptn'] ['end_ptn'] */
		{ "clear_t", EstRpsc_e_ClearT,  2, },
		/* clear_f ['begin_ptn'] ['end_ptn'] */
		{ "clear_f", EstRpsc_e_ClearF,  2, },
		/* clear_src_t ['begin_ptn'] ['end_ptn'] */
		{ "clear_src_t", EstRpsc_e_ClearSrcT,  2, },
		/* clear_src_f ['begin_ptn'] ['end_ptn'] */
		{ "clear_src_f", EstRpsc_e_ClearSrcF,  2, },
	};
	const char* quote_begin = "['";
	const char* quote_end   = "']";

	RanoTxtFilterAry_regist_byCommandAry( fltr_ary, cmds,
			quote_begin, quote_end,
			st_cmdinfo_table, Znk_NARY(st_cmdinfo_table),
			unescapeArg );
	return true;
}

