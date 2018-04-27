#include "Rano_module.h"
#include "Rano_module_ary.h"
#include "Rano_log.h"
#include "Rano_myf.h"
#include "Rano_txt_filter.h"
#include "Rano_plugin_dev.h"
#include "Rano_parent_proxy.h"
#include "Rano_file_info.h"

#include <Znk_str.h>
#include <Znk_str_fio.h>
#include <Znk_str_ary.h>
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <Znk_htp_hdrs.h>
#include <Znk_htp_post.h>
#include <Znk_myf.h>
#include <Znk_dlink.h>
#include <Znk_missing_libc.h>
#include <Znk_cookie.h>
#include <string.h>

struct RanoModuleImpl {
	char              target_name_[ 256 ];
	ZnkDLinkHandler   plg_handle_;
	RanoInitiateFunc  plg_initiate_;
	RanoOnPostFunc    plg_on_post_;
	RanoOnResponse    plg_on_response_;
	ZnkMyf            ftr_send_;
	ZnkMyf            ftr_recv_;
	RanoTxtFilterAry  ftr_html_;
	RanoTxtFilterAry  ftr_js_;
	RanoTxtFilterAry  ftr_css_;
	ZnkStrAry         ftr_css_additional_;
	ZnkDate           ftr_send_date_;
	ZnkDate           ftr_recv_date_;
};


RanoModule
RanoModule_create( void )
{
	RanoModule mod = Znk_alloc0( sizeof( struct RanoModuleImpl ) );
	mod->ftr_send_ = ZnkMyf_create();
	mod->ftr_recv_ = ZnkMyf_create();
	mod->ftr_html_ = RanoTxtFilterAry_create();
	mod->ftr_js_   = RanoTxtFilterAry_create();
	mod->ftr_css_  = RanoTxtFilterAry_create();
	mod->ftr_css_additional_ = ZnkStrAry_create( true );
	return mod;
}

void
RanoModule_destroy( RanoModule mod )
{
	if( mod ){
		ZnkMyf_destroy( mod->ftr_send_ );
		ZnkMyf_destroy( mod->ftr_recv_ );
		RanoTxtFilterAry_destroy( mod->ftr_html_ );
		RanoTxtFilterAry_destroy( mod->ftr_js_ );
		RanoTxtFilterAry_destroy( mod->ftr_css_ );
		ZnkStrAry_destroy( mod->ftr_css_additional_ );
		if( mod->plg_handle_ ){
			ZnkDLink_close( mod->plg_handle_ );
		}
		Znk_free( mod );
	}
}

bool
RanoModule_load( RanoModule mod, const char* target_name,
		const char* filters_dir, const char* plugins_dir )
{
	bool result = false;
	char filename[ 256 ];

	ZnkS_copy( mod->target_name_, sizeof(mod->target_name_), target_name, Znk_NPOS );

	/* Send filter */
	Znk_snprintf( filename, sizeof(filename), "%s/%s_send.myf", filters_dir, target_name );
	result = ZnkMyf_load( mod->ftr_send_, filename );
	if( result ){
		RanoLog_printf( "Rano : Filter Loading [%s]\n", filename );
	}
	RanoFileInfo_getLastUpdatedTime( filename, &mod->ftr_send_date_ );

	/* Recv filter */
	Znk_snprintf( filename, sizeof(filename), "%s/%s_recv.myf", filters_dir, target_name );
	result = ZnkMyf_load( mod->ftr_recv_, filename );
	if( result ){
		ZnkStrAry command_ary;
		command_ary = ZnkMyf_find_lines( mod->ftr_recv_, "html_filter" );
		if( command_ary ){
			RanoTxtFilterAry_regist_byCommandAry( mod->ftr_html_, command_ary,
					ZnkMyf_quote_begin( mod->ftr_recv_ ),
					ZnkMyf_quote_end( mod->ftr_recv_ ) );
		}

		command_ary = ZnkMyf_find_lines( mod->ftr_recv_, "js_filter" );
		if( command_ary ){
			RanoTxtFilterAry_regist_byCommandAry( mod->ftr_js_, command_ary,
					ZnkMyf_quote_begin( mod->ftr_recv_ ),
					ZnkMyf_quote_end( mod->ftr_recv_ ) );
		}

		command_ary = ZnkMyf_find_lines( mod->ftr_recv_, "css_filter" );
		if( command_ary ){
			RanoTxtFilterAry_regist_byCommandAry( mod->ftr_css_, command_ary,
					ZnkMyf_quote_begin( mod->ftr_recv_ ),
					ZnkMyf_quote_end( mod->ftr_recv_ ) );
		}

		command_ary = ZnkMyf_find_lines( mod->ftr_recv_, "css_additional" );
		if( command_ary ){
			ZnkStrAry_swap( mod->ftr_css_additional_, command_ary );
		}
		RanoLog_printf( "Rano : Filter Loading [%s]\n", filename );
	}
	RanoFileInfo_getLastUpdatedTime( filename, &mod->ftr_recv_date_ );

	/* Plugin module */
#if defined(Znk_TARGET_WINDOWS)
	Znk_snprintf( filename, sizeof(filename), "%s/%s.dll",    plugins_dir, target_name );
#elif defined(__CYGWIN__)
	Znk_snprintf( filename, sizeof(filename), "%s/cyg%s.dll", plugins_dir, target_name );
#else
	Znk_snprintf( filename, sizeof(filename), "%s/%s.so",     plugins_dir, target_name );
#endif
	RanoLog_printf( "Rano : Plugin Loading [%s]\n", filename );
	mod->plg_handle_ = ZnkDLink_open( filename );
	if( mod->plg_handle_ ){
		mod->plg_initiate_    = (RanoInitiateFunc)ZnkDLink_getFunc( mod->plg_handle_, "initiate" );
		mod->plg_on_post_     = (RanoOnPostFunc)  ZnkDLink_getFunc( mod->plg_handle_, "on_post" );
		mod->plg_on_response_ = (RanoOnResponse)  ZnkDLink_getFunc( mod->plg_handle_, "on_response" );
	} else {
		mod->plg_initiate_    = NULL;
		mod->plg_on_post_     = NULL;
		mod->plg_on_response_ = NULL;
	}
	return result;
}

static void
updateReplaceCmdAry( ZnkMyf myf, const char* ftr_name, RanoTxtFilterAry txt_ftr_ary, const char* nl )
{
	ZnkStrAry command_ary  = ZnkMyf_find_lines( myf, ftr_name );
	if( command_ary ){
		const char* quote_begin = ZnkMyf_quote_begin( myf );
		const char* quote_end   = ZnkMyf_quote_end(   myf );
		size_t idx;
		size_t size;
		RanoTxtFilter txt_ftr;
		ZnkStr line = ZnkStr_new( "" );
		ZnkStrAry_clear( command_ary );
		size = RanoTxtFilterAry_size( txt_ftr_ary );
		for( idx=0; idx<size; ++idx ){
			ZnkStr_clear( line );
			txt_ftr = RanoTxtFilterAry_at( txt_ftr_ary, idx );
			RanoTxtFilter_writeCommand( txt_ftr, line, quote_begin, quote_end );
			ZnkStrAry_push_bk_snprintf( command_ary, Znk_NPOS, "%s%s", ZnkStr_cstr( line ), nl );
		}
		ZnkStr_delete( line );
	}
}
bool
RanoModule_saveFilter( const RanoModule mod )
{
	bool result = false;
	char filename[ 256 ];

	const char*  nl = "\n";
	const char*  target_name = mod->target_name_;

	/* Send filter */
	Znk_snprintf( filename, sizeof(filename), "filters/%s_send.myf", target_name );
	result = ZnkMyf_save( mod->ftr_send_, filename );
	RanoFileInfo_getLastUpdatedTime( filename, &mod->ftr_send_date_ );

	/* Recv filter */
	updateReplaceCmdAry( mod->ftr_recv_, "html_filter", mod->ftr_html_, nl );
	updateReplaceCmdAry( mod->ftr_recv_, "js_filter",   mod->ftr_js_, nl );
	updateReplaceCmdAry( mod->ftr_recv_, "css_filter",  mod->ftr_css_, nl );
	{
		ZnkStrAry dst_ary  = ZnkMyf_find_lines( mod->ftr_recv_, "css_additional" );
		ZnkStrAry_copy( dst_ary, mod->ftr_css_additional_ );
	}

	Znk_snprintf( filename, sizeof(filename), "filters/%s_recv.myf", target_name );
	result = ZnkMyf_save( mod->ftr_recv_, filename );
	RanoFileInfo_getLastUpdatedTime( filename, &mod->ftr_recv_date_ );

	return result;
}

const char*
RanoModule_target_name( const RanoModule mod )
{
	return mod->target_name_;
}

ZnkMyf
RanoModule_ftrSend( const RanoModule mod )
{
	return mod->ftr_send_;
}
RanoTxtFilterAry
RanoModule_ftrHtml( const RanoModule mod )
{
	return mod->ftr_html_;
}
RanoTxtFilterAry
RanoModule_ftrJS( const RanoModule mod )
{
	return mod->ftr_js_;
}
RanoTxtFilterAry
RanoModule_ftrCSS( const RanoModule mod )
{
	return mod->ftr_css_;
}
ZnkStrAry
RanoModule_ftrCSSAdditional( const RanoModule mod )
{
	return mod->ftr_css_additional_;
}
bool
RanoModule_invokeInitiate( const RanoModule mod, const char* parent_proxy, ZnkStr result_msg )
{
	if( mod->plg_initiate_ ){
		RanoPluginState state = mod->plg_initiate_( mod->ftr_send_, parent_proxy, result_msg );
		return (bool)( state != RanoPluginState_e_Error );
	}
	return false;
}
bool
RanoModule_invokeOnPost( const RanoModule mod, ZnkVarpAry hdr_vars, ZnkVarpAry post_vars, bool* updated )
{
	if( mod->plg_on_post_ ){
		RanoPluginState state = mod->plg_on_post_( mod->ftr_send_, hdr_vars, post_vars );
		*updated = (bool)( state == RanoPluginState_e_Updated );
		return (bool)( state != RanoPluginState_e_Error );
	}
	return false;
}
bool
RanoModule_invokeOnResponse( const RanoModule mod, ZnkVarpAry hdr_vars, ZnkStr text, const char* req_urp )
{
	if( mod->plg_on_response_ ){
		RanoPluginState state = mod->plg_on_response_( mod->ftr_send_, hdr_vars, text, req_urp );
		return (bool)( state != RanoPluginState_e_Error );
	}
	return false;
}

size_t
RanoModule_filtHtpHeader( const RanoModule mod, ZnkVarpAry hdr_vars )
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
			} else {
				/* not found */
				/* この場合新規追加とする */
				const ZnkStr val_str = ftr_var->prim_.u_.str_;
				ZnkHtpHdrs_regist( hdr_vars,
						ftr_name, strlen(ftr_name),
						ZnkStr_cstr(val_str), ZnkStr_leng(val_str), false );
			}
		}
	}
	return count;
}

size_t
RanoModule_filtPostVars( const RanoModule mod, ZnkVarpAry post_vars )
{
	size_t           count = 0;
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
					/* filenameをmiscへセット */
					ZnkStr_set( ZnkVar_misc(pst_var), ftr_val );
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
					//RanoLog_printf( "  filtCookie : var_name=[%s] replace cok_val=[%s]=>ftr_val=[%s]\n",
					//		ZnkVar_name_cstr(cok_var), ZnkVar_cstr(cok_var), new_val );
					ZnkVar_set_val_Str( cok_var, new_val, new_val_leng );
					++count;
				}
			} else {
				/***
				 * ftr_varの値が非空ならcok_varsへ新規追加.
				 * ftr_varの値が空のときはcok_varsへ何も追加しないし、countも変更しない.
				 */
				if( new_val_leng ){
					ZnkVarp new_var = ZnkVarp_create( ZnkVar_name_cstr(ftr_var), "", 0, ZnkPrim_e_Str, NULL );
					ZnkVar_set_val_Str( new_var, new_val, Znk_NPOS );
					//RanoLog_printf( "  filtCookie : new var=[%s] val=[%s] is added to cok_vars.\n",
					//		ZnkVar_name_cstr(ftr_var), new_val );
					ZnkVarpAry_push_bk( cok_vars, new_var );
					++count;
				}
			}
		}
	}
	return count;
}

static void
getExtraVars( ZnkVarpAry extra_vars, const ZnkVarpAry cok_vars, const ZnkVarpAry ftr_vars, const ZnkVarpAry ftr_force )
{
	const size_t cok_size = ZnkVarpAry_size( cok_vars );
	ZnkVarp      cok_var;
	size_t       cok_idx;
	const char*  var_name = NULL;
	for( cok_idx=0; cok_idx<cok_size; ++cok_idx ){
		cok_var  = ZnkVarpAry_at( cok_vars, cok_idx );
		var_name = ZnkVar_name_cstr( cok_var );
		if( ZnkVarpAry_find_byName( ftr_vars, var_name, Znk_NPOS, false ) ){
			continue;
		}
		if( ZnkVarpAry_find_byName( ftr_force, var_name, Znk_NPOS, false ) ){
			continue;
		}
		{
			ZnkVarp new_var = ZnkVarp_create( var_name, "", 0, ZnkPrim_e_Str, NULL );
			ZnkVar_set_val_Str( new_var, ZnkVar_cstr(cok_var), ZnkVar_str_leng(cok_var) );
			ZnkVarpAry_push_bk( extra_vars, new_var );
		}
	}
}

static size_t
filtCookieStatement( ZnkStr cok_stmt, const ZnkVarpAry ftr_vars, const ZnkVarpAry ftr_force, bool is_all_replace, ZnkVarpAry extra_vars )
{
	ZnkVarpAry   cok_vars = ZnkVarpAry_create( true );
	size_t       count = 0;

	RanoLog_printf( "  filtCookie : start cok_stmt=[%s] is_all_replace=[%d]\n", ZnkStr_cstr(cok_stmt), is_all_replace );
	if( !is_all_replace ){
		/* Parse orignal Cookie header field statement */
		ZnkCookie_regist_byCookieStatement( cok_vars, ZnkStr_cstr(cok_stmt), ZnkStr_leng(cok_stmt) );
	}

	if( extra_vars ){
		getExtraVars( extra_vars, cok_vars, ftr_vars, ftr_force );
	}

	count += updateCokVars( cok_vars, ftr_vars );
	count += updateCokVars( cok_vars, ftr_force );

	/***
	 * 一つ以上更新があればcok_stmtを構築しなおす.
	 */
	if( count ){
		ZnkStr_clear( cok_stmt );
		ZnkCookie_extend_toCookieStatement( cok_vars, cok_stmt );
		RanoLog_printf( "  filtCookie : final cok_stmt=[%s]\n", ZnkStr_cstr(cok_stmt) );
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
				ZnkVarp new_var = ZnkVarp_create( ZnkVar_name_cstr(ftr_var), "", 0, ZnkPrim_e_Str, NULL );
				ZnkVar_set_val_Str( new_var, ZnkVar_cstr(ftr_var), ZnkVar_str_leng(ftr_var) );
				ZnkVarpAry_push_bk( cok_vars, new_var );
				++count;
			}
		}
	}
	return count;
}

size_t
RanoModule_filtCookieVars( const RanoModule mod, ZnkVarpAry hdr_vars, bool is_all_replace, ZnkVarpAry extra_vars )
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
			count = filtCookieStatement( cok_stmt, ftr_vars, ftr_force, is_all_replace, extra_vars );
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
						"", 0, false );
				cok_stmt = ZnkHtpHdrs_val( htp_var, 0 );
				ZnkCookie_extend_toCookieStatement( cok_vars, cok_stmt );
				RanoLog_printf( "  filtCookie : final cok_stmt=[%s]\n", ZnkStr_cstr(cok_stmt) );
			}

			ZnkVarpAry_destroy( cok_vars );
		}
	}
	return count;
}

void
RanoFltr_updateCookieFilter_bySetCookie( const ZnkVarpAry hdr_vars, ZnkMyf ftr_send )
{
	ZnkVarp set_cookie = ZnkHtpHdrs_find_literal( hdr_vars, "Set-Cookie" );
	if( set_cookie ){
		const size_t val_size = ZnkHtpHdrs_val_size( set_cookie );
		size_t       val_idx  = 0;
		ZnkVarpAry   ftr_vars = ZnkMyf_find_vars( ftr_send, "cookie_vars" );

		for( val_idx=0; val_idx<val_size; ++val_idx ){
			const char* p = ZnkHtpHdrs_val_cstr( set_cookie, val_idx );
			const char* e = p + strlen( p );
			const char* q = memchr( p, ';', (size_t)(e-p) );
			if( q == NULL ){
				q = e;
			}
			ZnkCookie_regist_byAssignmentStatement( ftr_vars, p, (size_t)(q-p) );
		}
	}
}

bool
RanoModule_isTargetHost( const RanoModule mod, const ZnkMyf mtgt, const char* hostname )
{
	const char* target_name = RanoTarget_findTargetName( mtgt, hostname );
	if( target_name ){
		return ZnkS_eq( mod->target_name_, target_name );
	}
	return false;
}

const char*
RanoTarget_findTargetName( const ZnkMyf mtgt, const char* hostname )
{
	const size_t  size = ZnkMyf_numOfSection( mtgt );
	size_t        idx;
	ZnkMyfSection sec;
	ZnkStrAry    hosts;
	for( idx=0; idx<size; ++idx ){
		sec   = ZnkMyf_atSection( mtgt, idx );
		hosts = ZnkMyfSection_lines( sec ); 
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
	RanoModule_destroy( (RanoModule)elem );
}
RanoModuleAry
RanoModuleAry_create( bool elem_responsibility )
{
	ZnkElemDeleterFunc deleter = elem_responsibility ? deleteElem : NULL;
	return (RanoModuleAry)ZnkObjAry_create( deleter );
}

void
RanoModuleAry_loadAllModules( RanoModuleAry mod_ary, const ZnkMyf mtgt,
		const char* filters_dir, const char* plugins_dir )
{
	const size_t  size = ZnkMyf_numOfSection( mtgt );
	size_t        idx;
	ZnkMyfSection sec;
	const char*   name;
	RanoModule    mod;
	for( idx=0; idx<size; ++idx ){
		sec  = ZnkMyf_atSection( mtgt, idx );
		name = ZnkMyfSection_name( sec );
		mod  = RanoModule_create();
		if( RanoModule_load( mod, name, filters_dir, plugins_dir ) ){
			RanoModuleAry_push_bk( mod_ary, mod );
		} else {
			RanoModule_destroy( mod );
		}
	}
}
RanoModule
RanoModuleAry_find_byHostname( const RanoModuleAry mod_ary, const char* hostname )
{
	ZnkMyf target_myf = RanoMyf_theTarget();
	const char* target_name = RanoTarget_findTargetName( target_myf, hostname );
	if( target_name ){
		const size_t mod_size = RanoModuleAry_size( mod_ary );
		size_t mod_idx;
		RanoModule mod;
		for( mod_idx=0; mod_idx<mod_size; ++mod_idx ){
			mod = RanoModuleAry_at( mod_ary, mod_idx );
			if( ZnkS_eq( mod->target_name_, target_name ) ){
				/* found */
				return mod;
			}
		}
	}
	/* not found */
	return NULL;
}

void
RanoModuleAry_initiateFilters( RanoModuleAry mod_ary, ZnkStrAry result_msgs )
{
	const size_t mod_size = RanoModuleAry_size( mod_ary );
	size_t       mod_idx;
	RanoModule   mod;
	const char*  parent_proxy_hostname = RanoParentProxy_getHostname();
	uint16_t     port                  = RanoParentProxy_getPort();
	char         parent_proxy[ 4096 ]  = "NONE";
	ZnkStr       result_msg            = ZnkStr_new( "" );
	const char*  target_name           = NULL;
	bool         result                = false;

	if( !ZnkS_empty( parent_proxy_hostname ) && !ZnkS_eq( parent_proxy_hostname, "NONE" ) ){
		Znk_snprintf( parent_proxy, sizeof(parent_proxy), "%s:%u", parent_proxy_hostname, port );
	}
	for( mod_idx=0; mod_idx<mod_size; ++mod_idx ){
		mod = RanoModuleAry_at( mod_ary, mod_idx );
		if( mod ){
			RanoLog_printf( "Rano : invokeInitiate [%s]\n", parent_proxy );
			target_name = RanoModule_target_name( mod );
			ZnkStr_set( result_msg, "Cannot call plugin function" );
			result = RanoModule_invokeInitiate( mod, parent_proxy, result_msg );

			if( result_msgs ){
				ZnkStrAry_push_bk_snprintf( result_msgs, Znk_NPOS,
						"Initiate %s : %s : <blockquote><pre>%s</pre></blockquote>",
						target_name, result ? "Success" : "Failure", ZnkStr_cstr(result_msg) );
			}
			RanoLog_printf( "Rano : Initiate %s : %s : %s\n",
					target_name, result ? "Success" : "Failure", ZnkStr_cstr(result_msg) );
		}
	}
}

void
RanoModule_filtTxt( const RanoModule mod, ZnkStr text, RanoTextType txt_type )
{
	RanoTxtFilterAry txt_ftr = NULL;
	switch( txt_type ){
	case RanoText_HTML:
		txt_ftr = RanoModule_ftrHtml( mod );
		break;
	case RanoText_JS:
		txt_ftr = RanoModule_ftrJS( mod );
		break;
	case RanoText_CSS:
		txt_ftr = RanoModule_ftrCSS( mod );
		break;
	default:
		break;
	}
	if( txt_ftr ){
		RanoTxtFilterAry_exec( txt_ftr, text );
		if( txt_type == RanoText_CSS ){
			const ZnkStrAry css_additional = RanoModule_ftrCSSAdditional( mod );
			const size_t size = ZnkStrAry_size( css_additional );
			size_t idx;
			ZnkStr line;
			for( idx=0; idx<size; ++idx ){
				line =  ZnkStrAry_at( css_additional, idx );
				ZnkStr_add_c( text, '\n' );
				ZnkStr_append( text, ZnkStr_cstr(line), ZnkStr_leng(line) );
			}
		}
	}
}

bool
RanoModule_reloadFilters_byDate( RanoModule mod, const char* target_name, const char* filters_dir )
{
	bool result = false;
	char filename[ 256 ];
	ZnkDate date = { 0 };

	ZnkS_copy( mod->target_name_, sizeof(mod->target_name_), target_name, Znk_NPOS );

	/* Send filter */
	Znk_snprintf( filename, sizeof(filename), "%s/%s_send.myf", filters_dir, target_name );
	RanoFileInfo_getLastUpdatedTime( filename, &date );
	if( ZnkDate_compareDay( &date, &mod->ftr_send_date_ ) || ZnkDate_diffSecond( &date, &mod->ftr_send_date_, 0 ) ){
		/* 更新時刻が異なっていた場合のみ再読み込み */
		result = ZnkMyf_load( mod->ftr_send_, filename );
		if( result ){
			RanoLog_printf( "Rano : Filter Loading [%s]\n", filename );
		}
		mod->ftr_send_date_ = date;
	}

	/* Recv filter */
	Znk_snprintf( filename, sizeof(filename), "%s/%s_recv.myf", filters_dir, target_name );
	RanoFileInfo_getLastUpdatedTime( filename, &date );
	if( ZnkDate_compareDay( &date, &mod->ftr_recv_date_ ) || ZnkDate_diffSecond( &date, &mod->ftr_recv_date_, 0 ) ){
		result = ZnkMyf_load( mod->ftr_recv_, filename );
		if( result ){
			ZnkStrAry command_ary;
			command_ary = ZnkMyf_find_lines( mod->ftr_recv_, "html_filter" );
			if( command_ary ){
				RanoTxtFilterAry_regist_byCommandAry( mod->ftr_html_, command_ary,
						ZnkMyf_quote_begin( mod->ftr_recv_ ),
						ZnkMyf_quote_end( mod->ftr_recv_ ) );
			}
	
			command_ary = ZnkMyf_find_lines( mod->ftr_recv_, "js_filter" );
			if( command_ary ){
				RanoTxtFilterAry_regist_byCommandAry( mod->ftr_js_, command_ary,
						ZnkMyf_quote_begin( mod->ftr_recv_ ),
						ZnkMyf_quote_end( mod->ftr_recv_ ) );
			}
	
			command_ary = ZnkMyf_find_lines( mod->ftr_recv_, "css_filter" );
			if( command_ary ){
				RanoTxtFilterAry_regist_byCommandAry( mod->ftr_css_, command_ary,
						ZnkMyf_quote_begin( mod->ftr_recv_ ),
						ZnkMyf_quote_end( mod->ftr_recv_ ) );
			}
	
			command_ary = ZnkMyf_find_lines( mod->ftr_recv_, "css_additional" );
			if( command_ary ){
				ZnkStrAry_swap( mod->ftr_css_additional_, command_ary );
			}
			RanoLog_printf( "Rano : Filter Loading [%s]\n", filename );
		}
		mod->ftr_recv_date_ = date;
	}

	return result;
}
