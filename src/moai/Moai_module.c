#include "Moai_module.h"
#include "Moai_module_ary.h"
#include "Moai_log.h"
#include "Moai_myf.h"

#include <Znk_str.h>
#include <Znk_str_fio.h>
#include <Znk_str_ary.h>
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
	MoaiInitiateFunc  plg_initiate_;
	MoaiOnPostFunc    plg_on_post_;
	MoaiOnResponse    plg_on_response_;
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
		ZnkStrAry command_ary;
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
#elif defined(__CYGWIN__)
	Znk_snprintf( filename, sizeof(filename), "plugins/cyg%s.dll", target_name );
#else
	Znk_snprintf( filename, sizeof(filename), "plugins/%s.so", target_name );
#endif
	MoaiLog_printf( "Moai : Plugin Loading [%s]\n", filename );
	mod->plg_handle_ = ZnkDLink_open( filename );
	if( mod->plg_handle_ ){
		mod->plg_initiate_    = (MoaiInitiateFunc)ZnkDLink_getFunc( mod->plg_handle_, "initiate" );
		mod->plg_on_post_     = (MoaiOnPostFunc)  ZnkDLink_getFunc( mod->plg_handle_, "on_post" );
		mod->plg_on_response_ = (MoaiOnResponse)  ZnkDLink_getFunc( mod->plg_handle_, "on_response" );
	} else {
		mod->plg_initiate_    = NULL;
		mod->plg_on_post_     = NULL;
		mod->plg_on_response_ = NULL;
	}
	return result;
}

static void
updateReplaceCmdAry( ZnkMyf myf, const char* ftr_name, ZnkTxtFilterAry txt_ftr_ary, const char* nl )
{
	ZnkStrAry command_ary  = ZnkMyf_find_lines( myf, ftr_name );
	if( command_ary ){
		const char* quote_begin = ZnkMyf_quote_begin( myf );
		const char* quote_end   = ZnkMyf_quote_end(   myf );
		size_t idx;
		size_t size;
		ZnkTxtFilter* txt_ftr;
		ZnkStrAry_clear( command_ary );
		size = ZnkTxtFilterAry_size( txt_ftr_ary );
		for( idx=0; idx<size; ++idx ){
			txt_ftr = ZnkTxtFilterAry_at( txt_ftr_ary, idx );
			ZnkStrAry_push_bk_snprintf( command_ary, Znk_NPOS, "replace %s%s%s %s%s%s%s",
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

const char*
MoaiModule_target_name( const MoaiModule mod )
{
	return mod->target_name_;
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
MoaiModule_invokeInitiate( const MoaiModule mod, const char* parent_proxy, ZnkStr result_msg )
{
	if( mod->plg_initiate_ ){
		return mod->plg_initiate_( mod->ftr_send_, parent_proxy, result_msg );
	}
	return false;
}
bool
MoaiModule_invokeOnPost( const MoaiModule mod )
{
	if( mod->plg_on_post_ ){
		return mod->plg_on_post_( mod->ftr_send_ );
	}
	return false;
}
bool
MoaiModule_invokeOnResponse( const MoaiModule mod, ZnkVarpAry hdr_vars, ZnkStr text, const char* req_urp )
{
	if( mod->plg_on_response_ ){
		return mod->plg_on_response_( mod->ftr_send_, hdr_vars, text, req_urp );
	}
	return false;
}

size_t
MoaiModule_filtHtpHeader( const MoaiModule mod, ZnkVarpAry hdr_vars )
{
	size_t            count = 0;
	const ZnkVarpAry ftr_vars = ZnkMyf_find_vars( mod->ftr_send_, "header_vars" );
	if( ftr_vars == NULL ){
		/* Does not exist send filter file or cannot load it. */
	} else {
		const size_t      ftr_size = ZnkVarpAry_size( ftr_vars );
		size_t            ftr_idx;
		ZnkVarp           ftr_var;
		const char*       ftr_name;
		ZnkVarp           htp_var;
		ZnkStr            htp_val;
	
		for( ftr_idx=0; ftr_idx<ftr_size; ++ftr_idx ){
			ftr_var  = ZnkVarpAry_at( ftr_vars, ftr_idx );
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
MoaiModule_filtPostVars( const MoaiModule mod, ZnkVarpAry post_vars )
{
	size_t            count = 0;
	const ZnkVarpAry ftr_vars = ZnkMyf_find_vars( mod->ftr_send_, "post_vars" );
	if( ftr_vars == NULL ){
		/* Does not exist send filter file or cannot load it. */
	} else {
		const size_t      ftr_size = ZnkVarpAry_size( ftr_vars );
		size_t            ftr_idx;
		ZnkVarp           ftr_var;
		const char*       ftr_name;
		const char*       ftr_val;
		ZnkVarp           pst_var;
	
		for( ftr_idx=0; ftr_idx<ftr_size; ++ftr_idx ){
			ftr_var  = ZnkVarpAry_at( ftr_vars, ftr_idx );
			ftr_name = ZnkStr_cstr( ftr_var->name_ );
			ftr_val  = ZnkStr_cstr( ftr_var->prim_.u_.str_ );
	
			pst_var = ZnkVarpAry_find_byName( post_vars, ftr_name, strlen(ftr_name), false );
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
updateCokVars( ZnkVarpAry cok_vars, const ZnkVarpAry cok_ftr )
{
	size_t count = 0;
	if( cok_ftr ){
		ZnkVarp      cok_var;
		const char*  new_val = NULL;
		size_t       new_val_leng = 0;
		const size_t ftr_size = ZnkVarpAry_size( cok_ftr );
		ZnkVarp      ftr_var;
		size_t       ftr_idx;
		for( ftr_idx=0; ftr_idx<ftr_size; ++ftr_idx ){
			ftr_var      = ZnkVarpAry_at( cok_ftr, ftr_idx );
			new_val      = ZnkVar_cstr(ftr_var);
			new_val_leng = ZnkVar_str_leng(ftr_var);
	
			cok_var = ZnkVarpAry_find_byName( cok_vars, ZnkVar_name_cstr(ftr_var), Znk_NPOS, false );
			if( cok_var ){
				/***
				 * ftr_varの値で上書きする.
				 * ftr_varの値が空の場合でもその空値で上書きする(countもインクリメントさせる).
				 * このとき、既にcok_vars内にあるCookie変数を削除するという扱いになるが、
				 * その処理はZnkCookie_extend_toCookieStatementが計らう.
				 */
				if( !ZnkS_eq( ZnkVar_cstr(cok_var), new_val ) ){
					MoaiLog_printf( "  filtCookie : var_name=[%s] replace cok_val=[%s]=>ftr_val=[%s]\n",
							ZnkVar_name_cstr(cok_var), ZnkVar_cstr(cok_var), new_val );
					ZnkVar_set_val_Str( cok_var, new_val, new_val_leng );
					++count;
				}
			} else {
				/***
				 * ftr_varの値が非空ならcok_varsへ新規追加.
				 * ftr_varの値が空のときはcok_varsへ何も追加しないし、countも変更しない.
				 */
				if( new_val_leng ){
					ZnkVarp new_var = ZnkVarp_create( ZnkVar_name_cstr(ftr_var), "", 0, ZnkPrim_e_Str );
					ZnkVar_set_val_Str( new_var, new_val, Znk_NPOS );
					MoaiLog_printf( "  filtCookie : new var=[%s] val=[%s] is added to cok_vars.\n",
							ZnkVar_name_cstr(ftr_var), new_val );
					ZnkVarpAry_push_bk( cok_vars, new_var );
					++count;
				}
			}
		}
	}
	return count;
}

static size_t
filtCookieStatement( ZnkStr cok_stmt, const ZnkVarpAry ftr_vars, const ZnkVarpAry ftr_force )
{
	ZnkVarpAry   cok_vars = ZnkVarpAry_create( true );
	size_t       count = 0;

	MoaiLog_printf( "  filtCookie : start cok_stmt=[%s]\n", ZnkStr_cstr(cok_stmt) );
	/* Parse orignal Cookie header field statement */
	ZnkCookie_regist_byCookieStatement( cok_vars, ZnkStr_cstr(cok_stmt), ZnkStr_leng(cok_stmt) );

	count += updateCokVars( cok_vars, ftr_vars );
	count += updateCokVars( cok_vars, ftr_force );

	/***
	 * 一つ以上更新があればcok_stmtを構築しなおす.
	 */
	if( count ){
		ZnkStr_clear( cok_stmt );
		ZnkCookie_extend_toCookieStatement( cok_vars, cok_stmt );
		MoaiLog_printf( "  filtCookie : final cok_stmt=[%s]\n", ZnkStr_cstr(cok_stmt) );
	}
	ZnkVarpAry_destroy( cok_vars );
	return count;
}

static size_t
addNewCokVars( ZnkVarpAry cok_vars, const ZnkVarpAry cok_ftr )
{
	size_t count = 0;
	if( cok_ftr ){
		const size_t ftr_size = ZnkVarpAry_size( cok_ftr );
		ZnkVarp      ftr_var;
		size_t       ftr_idx;
		for( ftr_idx=0; ftr_idx<ftr_size; ++ftr_idx ){
			ftr_var = ZnkVarpAry_at( cok_ftr, ftr_idx );
			if( ZnkVar_str_leng(ftr_var) ){
				/* found */
				ZnkVarp new_var = ZnkVarp_create( ZnkVar_name_cstr(ftr_var), "", 0, ZnkPrim_e_Str );
				ZnkVar_set_val_Str( new_var, ZnkVar_cstr(ftr_var), ZnkVar_str_leng(ftr_var) );
				ZnkVarpAry_push_bk( cok_vars, new_var );
				++count;
			}
		}
	}
	return count;
}

bool
MoaiModule_filtCookieVars( const MoaiModule mod, ZnkVarpAry hdr_vars )
{
	size_t           count    = 0;
	const ZnkVarpAry ftr_vars  = ZnkMyf_find_vars( mod->ftr_send_, "cookie_vars" );
	const ZnkVarpAry ftr_force = ZnkMyf_find_vars( mod->ftr_send_, "cookie_force" );
	if( ftr_vars == NULL ){
		/* Does not exist send filter file or cannot load it. */
	} else {
		ZnkVarp htp_var;
		ZnkStr  cok_stmt;
	
		htp_var = ZnkHtpHdrs_find_literal( hdr_vars, "Cookie" );
		if( htp_var ){
			cok_stmt = ZnkHtpHdrs_val( htp_var, 0 );
			count = filtCookieStatement( cok_stmt, ftr_vars, ftr_force );
			if( ZnkStr_empty( cok_stmt ) ){
				/***
				 * この場合Cookieフィールドそのものを削除する.
				 */
				ZnkHtpHdrs_erase( hdr_vars, "Cookie" );
			}
		} else {
			/***
			 * ftr_vars内のCookieの変数の値のうち、非空なものがあれば
			 * それらから構成されるCookieヘッダフィールドを新たに追加する.
			 */
			ZnkVarpAry cok_vars = ZnkVarpAry_create( true );
			count += addNewCokVars( cok_vars, ftr_vars );
			count += addNewCokVars( cok_vars, ftr_force );

			if( ZnkVarpAry_size( cok_vars ) ){
				/* この場合のみCookieフィールドを新規追加 */
				htp_var = ZnkHtpHdrs_regist( hdr_vars,
						"Cookie", Znk_strlen_literal( "Cookie" ),
						"", 0 );
				cok_stmt = ZnkHtpHdrs_val( htp_var, 0 );
				ZnkCookie_extend_toCookieStatement( cok_vars, cok_stmt );
				MoaiLog_printf( "  filtCookie : final cok_stmt=[%s]\n", ZnkStr_cstr(cok_stmt) );
			}

			ZnkVarpAry_destroy( cok_vars );
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
	ZnkStrAry    hosts;
	for( idx=0; idx<size; ++idx ){
		sec   = ZnkMyf_atSection( mtgt, idx );
		hosts = ZnkMyfSection_lines( sec ); 
		//if( ZnkStrAry_find( hosts, 0, hostname, Znk_NPOS ) != Znk_NPOS ){
		if( ZnkStrAry_find_isMatch( hosts, 0, hostname, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
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
	return (MoaiModuleAry)ZnkObjAry_create( deleter );
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
MoaiModuleAry_find_byHostname( const MoaiModuleAry mod_ary, const char* hostname )
{
	ZnkMyf target_myf = MoaiMyf_theTarget();
	const char* target_name = MoaiTarget_findTargetName( target_myf, hostname );
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


