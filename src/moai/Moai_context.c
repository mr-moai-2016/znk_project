#include "Moai_context.h"
#include <Znk_stdc.h>


MoaiContext
MoaiContext_create( void )
{
	MoaiContext ctx = Znk_alloc0( sizeof( struct MoaiContextImpl ) );
	ctx->msgs_    = ZnkStr_new( "" );
	ctx->text_    = ZnkStr_new( "" );
	ctx->draft_info_id_ = MoaiInfo_regist( NULL );
	ctx->draft_info_    = MoaiInfo_find( ctx->draft_info_id_ );
	return ctx;
}
void
MoaiContext_destroy( MoaiContext ctx )
{
	if( ctx ){
		ZnkStr_delete( ctx->msgs_ );
		ZnkStr_delete( ctx->text_ );
		MoaiInfo_erase( ctx->draft_info_id_ );
		Znk_free( ctx );
	}
}
