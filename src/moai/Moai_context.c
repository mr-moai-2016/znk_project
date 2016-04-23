#include "Moai_context.h"
#include <Znk_stdc.h>


MoaiContext
MoaiContext_create( void )
{
	MoaiContext ctx = Znk_alloc0( sizeof( struct MoaiContextImpl ) );
	ctx->msgs_    = ZnkStr_new( "" );
	ctx->text_    = ZnkStr_new( "" );
	ctx->req_urp_ = ZnkStr_new( "" );
	ZnkHtpHdrs_compose( &ctx->hi_.hdrs_ );
	ctx->hi_.vars_   = ZnkVarpDAry_create( true );
	ctx->hi_.stream_ = ZnkBfr_create_null();
	ctx->target_myf_ = ZnkMyf_create();
	ctx->mod_ary_    = MoaiModuleAry_create( true );
	ctx->config_     = ZnkMyf_create();
	ctx->analysis_   = ZnkMyf_create();

	return ctx;
}
void
MoaiContext_destroy( MoaiContext ctx )
{
	if( ctx ){
		ZnkStr_delete( ctx->msgs_ );
		ZnkStr_delete( ctx->text_ );
		ZnkStr_delete( ctx->req_urp_ );
		ZnkHtpHdrs_dispose( &ctx->hi_.hdrs_ );
		ZnkVarpDAry_destroy( ctx->hi_.vars_ );
		ZnkBfr_destroy( ctx->hi_.stream_ );
		ZnkMyf_destroy( ctx->target_myf_ );
		MoaiModuleAry_destroy( ctx->mod_ary_ );
		ZnkMyf_destroy( ctx->config_ );
		ZnkMyf_destroy( ctx->analysis_ );
		Znk_free( ctx );
	}
}
