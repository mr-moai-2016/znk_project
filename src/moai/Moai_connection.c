#include "Moai_connection.h"
#include <Znk_obj_dary.h>


static ZnkObjDAry st_connection_dary = NULL;

static MoaiConnection
allocHtpConnection( void )
{
	MoaiConnection htcn = (MoaiConnection)Znk_alloc0( sizeof( struct MoaiConnection_tag ) );
	htcn->req_urp_ = ZnkStr_new( "" );
	return htcn;
}
static void
deleteHtpConnection( void* elem )
{
	if( elem ){
		MoaiConnection htcn = (MoaiConnection)elem;
		ZnkStr_delete( htcn->req_urp_ );
		Znk_free( htcn );
	}
}

void
MoaiConnection_initiate( void )
{
	if( st_connection_dary == NULL ){
		st_connection_dary = ZnkObjDAry_create( deleteHtpConnection );
	}
}

void
MoaiConnection_clear( MoaiConnection htcn )
{
	if( htcn ){
		htcn->hostname_[ 0 ]   = '\0';
		//htcn->ipaddr_[ 0 ]     = '\0';
		htcn->port_            = 0;
		htcn->src_sock_        = ZnkSocket_INVALID;
		htcn->sock_type_       = MoaiSockType_e_None;
		htcn->is_keep_alive_   = false;
		htcn->dst_sock_        = ZnkSocket_INVALID;
		htcn->content_length_remain_ = 0;
		htcn->as_local_proxy_  = false;
		ZnkStr_clear( htcn->req_urp_ );
		htcn->req_method_ = ZnkHtpReqMethod_e_Unknown;
		htcn->exile_time_ = 0;
		htcn->waiting_ = 0;
	}
}

MoaiConnection
MoaiConnection_find( const ZnkSocket query_sock )
{
	MoaiConnection htcn;
	const size_t size = ZnkObjDAry_size( st_connection_dary );
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		htcn = (MoaiConnection) ZnkObjDAry_at( st_connection_dary, idx );
		if( htcn->src_sock_ == query_sock ){
			/* found */
			return htcn;
		}
	}
	/* not found */
	return NULL;
}
MoaiConnection
MoaiConnection_find_dst_sock( const ZnkSocket query_sock )
{
	MoaiConnection htcn;
	const size_t size = ZnkObjDAry_size( st_connection_dary );
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		htcn = (MoaiConnection) ZnkObjDAry_at( st_connection_dary, idx );
		if( htcn->dst_sock_ == query_sock ){
			/* found */
			return htcn;
		}
	}
	/* not found */
	return NULL;
}
MoaiConnection
MoaiConnection_intern( ZnkSocket sock, MoaiSockType sock_type )
{
	MoaiConnection htcn = MoaiConnection_find( sock );
	if( htcn == NULL ){
		htcn = MoaiConnection_find( ZnkSocket_INVALID );
		if( htcn == NULL ){
			htcn = allocHtpConnection();
			ZnkObjDAry_push_bk( st_connection_dary, (ZnkObj)htcn );
		}
		MoaiConnection_clear( htcn );
		htcn->src_sock_ = sock;
		htcn->sock_type_ = sock_type;
	}
	return htcn;
}
void
MoaiConnection_clear_bySock( ZnkSocket sock )
{
	MoaiConnection htcn = MoaiConnection_find( sock );
	MoaiConnection_clear( htcn );
}
void
MoaiConnection_clearAll( void )
{
	MoaiConnection htcn;
	const size_t size = ZnkObjDAry_size( st_connection_dary );
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		htcn = (MoaiConnection) ZnkObjDAry_at( st_connection_dary, idx );
		MoaiConnection_clear( htcn );
	}
}

