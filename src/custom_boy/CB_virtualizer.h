#ifndef INCLUDE_GUARD__CB_virtualizer_h__
#define INCLUDE_GUARD__CB_virtualizer_h__

#include <CB_fgp_info.h>
#include <CB_ua_info.h>

#include <Rano_cgi_util.h>

#include <Znk_varp_ary.h>
#include <Znk_bird.h>


Znk_EXTERN_C_BEGIN

typedef enum {
	 CBStatus_e_Sealed
	,CBStatus_e_Editing
	,CBStatus_e_InputLacking
}CBStatus;

bool
CBVirtualizer_load( ZnkVarpAry main_vars, const char* filename, const char* sec_name );
bool
CBVirtualizer_save( ZnkVarpAry main_vars, const char* filename, const char* sec_name );
void
CBVirtualizer_registBird_byVars( ZnkBird bird, ZnkVarpAry vars );
void
CBVirtualizer_initVars_byFutabaSendMyf( ZnkVarpAry main_vars, const char* moai_dir );
bool
CBVirtualizer_doMainProc( RanoCGIEVar* evar, ZnkVarpAry cb_vars, const char* cb_src, ZnkBird bird, bool is_step1, bool is_step2, ZnkStr RE_key,
		CBFgpInfo fgp_info, CBUAInfo ua_info, ZnkVarpAry main_vars, uint64_t* ptua64, ZnkStr msg, ZnkStr category, ZnkStr lacking_var,
		const char* moai_dir, bool all_cookie_clear );
void
CBVirtualizer_registBird_byFutabaSendMyf( ZnkBird bird, const char* moai_dir );
void
CBVirtualizer_registBirdCookie( ZnkBird bird, bool is_view, const char* moai_dir );
void
CBVirtualizer_saveCookie_byCBVars( ZnkVarpAry cb_vars, const char* moai_dir );
void
CBVirtualizer_loadProxyList( const char* moai_dir, ZnkStr parent_proxy, ZnkStrAry proxy_list );
void
CBVirtualizer_addProxyList( const char* prxy, const char* moai_dir );

#if 0
CBStatus
CBVirtualizer_processFgpStep2( ZnkBird bird, ZnkVarpAry cb_vars, ZnkVarpAry main_vars, uint64_t* ptua64,
		CBFgpInfo fgp_info, ZnkStr RE_key, RanoCGIEVar* evar, const char* cb_src,
		ZnkStr lacking_var, ZnkStr msg, const char* moai_dir );
CBStatus
CBVirtualizer_processFgpUpdate( ZnkBird bird, ZnkVarpAry cb_vars, ZnkVarpAry main_vars, uint64_t* ptua64,
		CBFgpInfo fgp_info, ZnkStr RE_key,
		ZnkStr lacking_var, ZnkStr msg );
#endif

void
CBVirtualizer_registPtuaCtx( ZnkBird bird, uint64_t ptua64 );
void
CBVirtualizer_copyMainCtx_byVars( ZnkVarpAry dst, const ZnkVarpAry src );
void
CBVirtualizer_drawCustomSnp( ZnkBird bird, const char* hint_base_Snp, uint64_t ptua64 );
void
CBVirtualizer_registBirdFgp( ZnkBird bird, CBFgpInfo fgp_info, const char* RE_key, bool is_update_main );
void
CBVirtualizer_setFiltersDir( const char* filters_dir );
void
CBVirtualizer_setRERandomFactor( double RE_random_factor );

bool
CBVirtualizer_initiateAndSave( RanoCGIEVar* evar, const ZnkVarpAry cb_vars,
		ZnkStr msg, const char* cb_src, ZnkStr caco, ZnkStr ua_str, ZnkStr lacking_var,
		bool all_cookie_clear );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
