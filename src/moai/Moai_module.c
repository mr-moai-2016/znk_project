#include "Moai_module.h"
#include "Moai_module_ary.h"
#include "Moai_log.h"

#include <Znk_str.h>
#include <Znk_str_fio.h>
#include <Znk_str_dary.h>
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <Znk_htp_hdrs.h>
#include <Znk_myf.h>
#include <Znk_dlink.h>
#include <Znk_missing_libc.h>
#include <Znk_cookie.h>
#include <string.h>

struct MoaiModuleImpl {
	char              target_name_[ 256 ];
	ZnkDLinkHandler   plg_handle_;
	MoaiOnPostFunc    plg_on_post_before_;
	MoaiOnResponseHdr plg_on_response_hdr_;
	ZnkMyf            ftr_send_;
	ZnkMyf            ftr_recv_;
	ZnkTxtFilterAry   ftr_html_;
	ZnkTxtFilterAry   ftr_js_;
	ZnkTxtFilterAry   ftr_css_;
};


MoaiModule
MoaiModule_create( void )
{
	MoaiModule mod = Znk_alloc0( sizeof( struct MoaiModuleImpl ) );
	mod->ftr_send_ = ZnkMyf_create();
	mod->ftr_recv_ = ZnkMyf_create();
	mod->ftr_html_ = ZnkTxtFilterAry_create();
	mod->ftr_js_   = ZnkTxtFilterAry_create();
	mod->ftr_css_  = ZnkTxtFilterAry_create();
	return mod;
}

void
MoaiModule_destroy( MoaiModule mod )
{
	if( mod ){
		ZnkMyf_destroy( mod->ftr_send_ );
		ZnkMyf_destroy( mod->ftr_recv_ );
		ZnkTxtFilterAry_destroy( mod->ftr_html_ );
		ZnkTxtFilterAry_destroy( mod->ftr_js_ );
		ZnkTxtFilterAry_destroy( mod->ftr_css_ );
		if( mod->plg_handle_ ){
			ZnkDLink_close( mod->plg_handle_ );
		}
		Znk_free( mod );
	}
}

bool
MoaiModule_load( MoaiModule mod, const char* target_name )
{
	bool result = false;
	char filename[ 256 ];

	ZnkS_copy( mod->target_name_, sizeof(mod->target_name_), target_name, Znk_NPOS );

	/* Send filter */
	Znk_snprintf( filename, sizeof(filename), "filters/%s_send.myf", target_name );
	result = ZnkMyf_load( mod->ftr_send_, filename );
	if( result ){
		MoaiLog_printf( "Moai : Filter Loading [%s]\n", filename );
	}

	/* Recv filter */
	Znk_snprintf( filename, sizeof(filename), "filters/%s_recv.myf", target_name );
	result = ZnkMyf_load( mod->ftr_recv_, filename );
	if( result ){
		ZnkStrDAry command_ary;
		command_ary = ZnkMyf_find_lines( mod->ftr_recv_, "html_filter" );
		if( command_ary ){
			ZnkTxtFilterAry_regist_byCommandAry( mod->ftr_html_, command_ary,
					ZnkMyf_quote_begin( mod->ftr_recv_ ),
					ZnkMyf_quote_end( mod->ftr_recv_ ) );
		}

		command_ary = ZnkMyf_find_lines( mod->ftr_recv_, "js_filter" );
		if( command_ary ){
			ZnkTxtFilterAry_regist_byCommandAry( mod->ftr_js_, command_ary,
					ZnkMyf_quote_begin( mod->ftr_recv_ ),
					ZnkMyf_quote_end( mod->ftr_recv_ ) );
		}

		command_ary = ZnkMyf_find_lines( mod->ftr_recv_, "css_filter" );
		if( command_ary ){
			ZnkTxtFilterAry_regist_byCommandAry( mod->ftr_css_, command_ary,
					ZnkMyf_quote_begin( mod->ftr_recv_ ),
					ZnkMyf_quote_end( mod->ftr_recv_ ) );
		}
		MoaiLog_printf( "Moai : Filter Loading [%s]\n", filename );
	}

	/* Plugin module */
#if defined(Znk_TARGET_WINDOWS)
	Znk_snprintf( filename, sizeof(filename), "plugins/%s.dll", target_name );
#else
	Znk_snprintf( filename, sizeof(filename), "plugins/%s.so", target_name );
#endif
	MoaiLog_printf( "Moai : Plugin Loading [%s]\n", filename );
	mod->plg_handle_ = ZnkDLink_open( filename );
	if( mod->plg_handle_ ){
		mod->plg_on_post_before_  = (MoaiOnPostFunc)ZnkDLink_getFunc( mod->plg_handle_, "on_post_before" );
		mod->plg_on_response_hdr_ = (MoaiOnResponseHdr)ZnkDLink_getFunc( mod->plg_handle_, "on_response_hdr" );
	} else {
		mod->plg_on_post_before_  = NULL;
		mod->plg_on_response_hdr_ = NULL;
	}
	return result;
}

static void
updateReplaceCmdAry( ZnkMyf myf, const char* ftr_name, ZnkTxtFilterAry txt_ftr_ary, const char* nl )
{
	ZnkStrDAry command_ary  = ZnkMyf_find_lines( myf, ftr_name );
	if( command_ary ){
		const char* quote_begin = ZnkMyf_quote_begin( myf );
		const char* quote_end   = ZnkMyf_quote_end(   myf );
		size_t idx;
		size_t size;
		ZnkTxtFilter* txt_ftr;
		ZnkStrDAry_clear( command_ary );
		size = ZnkTxtFilterAry_size( txt_ftr_ary );
		for( idx=0; idx<size; ++idx ){
			txt_ftr = ZnkTxtFilterAry_at( txt_ftr_ary, idx );
			ZnkStrDAry_push_bk_snprintf( command_ary, Znk_NPOS, "replace %s%s%s %s%s%s%s",
					quote_begin, ZnkStr_cstr( txt_ftr->old_ptn_ ), quote_end,
					quote_begin, ZnkStr_cstr( txt_ftr->new_ptn_ ), quote_end,
					nl );
		}
	}
}
bool
MoaiModule_saveFilter( const MoaiModule mod )
{
	bool result = false;
	char filename[ 256 ];

	const char*  nl = "\n";
	const char*  target_name = mod->target_name_;

	/* Send filter */
	Znk_snprintf( filename, sizeof(filename), "filters/%s_send.myf", target_name );
	result = ZnkMyf_save( mod->ftr_send_, filename );

	/* Recv filter */
	updateReplaceCmdAry( mod->ftr_recv_, "html_filter", mod->ftr_html_, nl );
	updateReplaceCmdAry( mod->ftr_recv_, "js_filter",   mod->ftr_js_, nl );
	updateReplaceCmdAry( mod->ftr_recv_, "css_filter",  mod->ftr_css_, nl );

	Znk_snprintf( filename, sizeof(filename), "filters/%s_recv.myf", target_name );
	result = ZnkMyf_save( mod->ftr_recv_, filename );

	return result;
}

ZnkMyf
MoaiModule_ftrSend( const MoaiModule mod )
{
	return mod->ftr_send_;
}
ZnkTxtFilterAry
MoaiModule_ftrHtml( const MoaiModule mod )
{
	return mod->ftr_html_;
}
ZnkTxtFilterAry
MoaiModule_ftrJS( const MoaiModule mod )
{
	return mod->ftr_js_;
}
ZnkTxtFilterAry
MoaiModule_ftrCSS( const MoaiModule mod )
{
	return mod->ftr_css_;
}
bool
MoaiModule_invokeOnPostBefore( const MoaiModule mod )
{
	if( mod->plg_on_post_before_ ){
		return mod->plg_on_post_before_( mod->ftr_send_ );
	}
	return false;
}
bool
MoaiModule_invokeOnResponseHdr( const MoaiModule mod, ZnkVarpDAry hdr_vars )
{
	if( mod->plg_on_response_hdr_ ){
		return mod->plg_on_response_hdr_( mod->ftr_send_, hdr_vars );
	}
	return false;
}

size_t
MoaiModule_filtHtpHeader( const MoaiModule mod, ZnkVarpDAry hdr_vars )
{
	size_t            count = 0;
	const ZnkVarpDAry ftr_vars = ZnkMyf_find_vars( mod->ftr_send_, "header_vars" );
	if( ftr_vars == NULL ){
		/* Does not exist send filter file or cannot load it. */
	} else {
		const size_t      ftr_size = ZnkVarpDAry_size( ftr_vars );
		size_t            ftr_idx;
		ZnkVarp           ftr_var;
		const char*       ftr_name;
		ZnkVarp           htp_var;
		ZnkStr            htp_val;
	
		for( ftr_idx=0; ftr_idx<ftr_size; ++ftr_idx ){
			ftr_var  = ZnkVarpDAry_at( ftr_vars, ftr_idx );
			ftr_name = ZnkStr_cstr( ftr_var->name_ );
	
			htp_var = ZnkHtpHdrs_find( hdr_vars, ftr_name, strlen(ftr_name) );
			if( htp_var ){
				/* found */
				htp_val = ZnkHtpHdrs_val( htp_var, 0 );
				ZnkStr_set( htp_val, ZnkStr_cstr(ftr_var->prim_.u_.str_) );
				++count;
			}
		}
	}
	return count;
}

size_t
MoaiModule_filtPostVars( const MoaiModule mod, ZnkVarpDAry post_vars )
{
	size_t            count = 0;
	const ZnkVarpDAry ftr_vars = ZnkMyf_find_vars( mod->ftr_send_, "post_vars" );
	if( ftr_vars == NULL ){
		/* Does not exist send filter file or cannot load it. */
	} else {
		const size_t      ftr_size = ZnkVarpDAry_size( ftr_vars );
		size_t            ftr_idx;
		ZnkVarp           ftr_var;
		const char*       ftr_name;
		const char*       ftr_val;
		ZnkVarp           pst_var;
	
		for( ftr_idx=0; ftr_idx<ftr_size; ++ftr_idx ){
			ftr_var  = ZnkVarpDAry_at( ftr_vars, ftr_idx );
			ftr_name = ZnkStr_cstr( ftr_var->name_ );
			ftr_val  = ZnkStr_cstr( ftr_var->prim_.u_.str_ );
	
			pst_var = ZnkVarpDAry_find_byName( post_vars, ftr_name, strlen(ftr_name), false );
			if( pst_var ){
				/* found */
				if( pst_var->type_ == ZnkHtpPostVar_e_BinaryData ){
					ZnkStr_set( pst_var->filename_, ftr_val );
					/* filenameが変わるならbfrを再読み込みすべきだが、とりあえず今はしない */
				} else {
					ZnkStr_set( pst_var->prim_.u_.str_, ftr_val );
				}
				++count;
			}
		}
	}
	return count;
}

static size_t
filtCookieStatement( const ZnkVarpDAry ftr_vars, ZnkStr cok_stmt )
{
	const size_t ftr_size = ZnkVarpDAry_size( ftr_vars );
	ZnkVarp      ftr_var;
	size_t       ftr_idx;
	ZnkVarpDAry  cok_vars = ZnkVarpDAry_create( true );
	ZnkVarp      cok_var;
	size_t       count = 0;

	/* Parse orignal Cookie header field statement */
	ZnkCookie_regist_byCookieStatement( cok_vars, ZnkStr_cstr(cok_stmt), ZnkStr_leng(cok_stmt) );

	for( ftr_idx=0; ftr_idx<ftr_size; ++ftr_idx ){
		ftr_var = ZnkVarpDAry_at( ftr_vars, ftr_idx );

		cok_var = ZnkVarpDAry_find_byName( cok_vars, ZnkVar_name_cstr(ftr_var), Znk_NPOS, false );
		if( cok_var ){
			/* ftr_varの値で上書き */
			ZnkVar_set_val_Str( cok_var, ZnkVar_cstr(ftr_var), ZnkVar_str_leng(ftr_var) );
			++count;
		} else {
			/* ftr_varの値が非空なら新規追加 */
			ZnkVarp new_var = ZnkVarp_create( ZnkVar_name_cstr(ftr_var), "", 0, ZnkPrim_e_Str );
			ZnkVarpDAry_push_bk( cok_vars, new_var );
			++count;
		}
	}

	/***
	 * 一つ以上更新があればcok_stmtを構築しなおす.
	 */
	if( count ){
		ZnkStr_clear( cok_stmt );
		ZnkCookie_extend_toCookieStatement( cok_vars, cok_stmt );
	}
	ZnkVarpDAry_destroy( cok_vars );
	return count;
}

bool
MoaiModule_filtCookieVars( const MoaiModule mod, ZnkVarpDAry hdr_vars )
{
	size_t            count = 0;
	const ZnkVarpDAry ftr_vars = ZnkMyf_find_vars( mod->ftr_send_, "cookie_vars" );
	if( ftr_vars == NULL ){
		/* Does not exist send filter file or cannot load it. */
	} else {
		ZnkVarp           htp_var;
		ZnkStr            htp_val;
	
		htp_var = ZnkHtpHdrs_find_literal( hdr_vars, "Cookie" );
		if( htp_var ){
			htp_val = ZnkHtpHdrs_val( htp_var, 0 );
			count = filtCookieStatement( ftr_vars, htp_val );
		} else {
			/***
			 * ftr_vars内のCookieの変数の値のうち、非空なものがあれば
			 * それらから構成されるCookieヘッダフィールドを新たに追加する.
			 */
			const size_t ftr_size = ZnkVarpDAry_size( ftr_vars );
			ZnkVarp      ftr_var;
			size_t       ftr_idx;
			ZnkVarpDAry  cok_vars = ZnkVarpDAry_create( true );
			for( ftr_idx=0; ftr_idx<ftr_size; ++ftr_idx ){
				ftr_var = ZnkVarpDAry_at( ftr_vars, ftr_idx );
				if( ZnkVar_str_leng(ftr_var) ){
					/* found */
					ZnkVarp new_var = ZnkVarp_create( ZnkVar_name_cstr(ftr_var), "", 0, ZnkPrim_e_Str );
					ZnkVar_set_val_Str( new_var, ZnkVar_cstr(ftr_var), ZnkVar_str_leng(ftr_var) );
					ZnkVarpDAry_push_bk( cok_vars, new_var );
					++count;
				}
			}
			if( count ){
				ZnkHtpHdrs_registCookie( hdr_vars, cok_vars );
			}
			ZnkVarpDAry_destroy( cok_vars );
		}
	}
	return count;
}

bool
MoaiModule_isTargetHost( const MoaiModule mod, const ZnkMyf mtgt, const char* hostname )
{
	const char* target_name = MoaiTarget_findTargetName( mtgt, hostname );
	if( target_name ){
		return ZnkS_eq( mod->target_name_, target_name );
	}
	return false;
}

const char*
MoaiTarget_findTargetName( const ZnkMyf mtgt, const char* hostname )
{
	const size_t  size = ZnkMyf_numOfSection( mtgt );
	size_t        idx;
	ZnkMyfSection sec;
	ZnkStrDAry    hosts;
	for( idx=0; idx<size; ++idx ){
		sec   = ZnkMyf_atSection( mtgt, idx );
		hosts = ZnkMyfSection_lines( sec ); 
		if( ZnkStrDAry_find( hosts, 0, hostname, Znk_NPOS ) != Znk_NPOS ){
			/* found */
			return ZnkMyfSection_name( sec );
		}
	}
	/* not found */
	return NULL;
}

static void
deleteElem( void* elem ){
	MoaiModule_destroy( (MoaiModule)elem );
}
MoaiModuleAry
MoaiModuleAry_create( bool elem_responsibility )
{
	ZnkElemDeleterFunc deleter = elem_responsibility ? deleteElem : NULL;
	return (MoaiModuleAry)ZnkObjDAry_create( deleter );
}

void
MoaiModuleAry_loadAllModules( MoaiModuleAry mod_ary, const ZnkMyf mtgt )
{
	const size_t  size = ZnkMyf_numOfSection( mtgt );
	size_t        idx;
	ZnkMyfSection sec;
	const char*   name;
	MoaiModule    mod;
	for( idx=0; idx<size; ++idx ){
		sec  = ZnkMyf_atSection( mtgt, idx );
		name = ZnkMyfSection_name( sec );
		mod  = MoaiModule_create();
		if( MoaiModule_load( mod, name ) ){
			MoaiModuleAry_push_bk( mod_ary, mod );
		} else {
			MoaiModule_destroy( mod );
		}
	}
}
MoaiModule
MoaiModuleAry_find_byHostname( const MoaiModuleAry mod_ary, const ZnkMyf mtgt, const char* hostname )
{
	const char* target_name = MoaiTarget_findTargetName( mtgt, hostname );
	if( target_name ){
		const size_t mod_size = MoaiModuleAry_size( mod_ary );
		size_t mod_idx;
		MoaiModule mod;
		for( mod_idx=0; mod_idx<mod_size; ++mod_idx ){
			mod = MoaiModuleAry_at( mod_ary, mod_idx );
			if( ZnkS_eq( mod->target_name_, target_name ) ){
				/* found */
				return mod;
			}
		}
	}
	/* not found */
	return NULL;
}


