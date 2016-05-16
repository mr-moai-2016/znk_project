#include "Moai_connection.h"
#include "Moai_parent_proxy.h"
#include "Moai_log.h"
#include <Znk_obj_ary.h>
#include <time.h>


static ZnkObjAry st_connection_ary = NULL;
static ZnkSocket  st_listening_sock = ZnkSocket_INVALID;
static bool       st_blocking_mode  = false;

Znk_INLINE double
getCurrentSec( void ){ return ( 1.0 / CLOCKS_PER_SEC ) * clock() + 1.0; }


static MoaiConnection
allocConnection( void )
{
	MoaiConnection mcn = (MoaiConnection)Znk_alloc0( sizeof( struct MoaiConnection_tag ) );
	mcn->hostname_ = ZnkStr_new( "" );
	mcn->info_id_list_ = ZnkBfr_create_null();
	mcn->prev_req_method_list_ = ZnkBfr_create_null();
	/***
	 * 特にLinuxにおいて, fd=0(stdin)がcloseされるようなことは絶対に避けねばならない.
	 * 仮に0をcloseすると0番が不使用になるため、ZnkSocket_openで値0のsocketが発行されるようになる.
	 * これがさらにselectに渡されるとselectが EBADF エラーを出し、サーバとしての機能が
	 * 激しく撹乱される事態に陥る.
	 * そのため、ここで確実にZnkSocket_INVALIDで初期化しておくことが重要である.
	 */
	mcn->I_sock_ = ZnkSocket_INVALID;
	mcn->O_sock_ = ZnkSocket_INVALID;
	return mcn;
}
static void
deleteConnection( void* elem )
{
	if( elem ){
		MoaiConnection mcn = (MoaiConnection)elem;
		ZnkStr_delete( mcn->hostname_ );
		ZnkBfr_destroy( mcn->info_id_list_ );
		ZnkBfr_destroy( mcn->prev_req_method_list_ );
		Znk_free( mcn );
	}
}

void
MoaiConnection_initiate( void )
{
	if( st_connection_ary == NULL ){
		st_connection_ary = ZnkObjAry_create( deleteConnection );
	}
}

void
MoaiConnection_setListeningSock( ZnkSocket L_sock )
{
	st_listening_sock = L_sock;
}
bool
MoaiConnection_isListeningSock( ZnkSocket query_sock )
{
	return (bool)( st_listening_sock == query_sock );
}
void
MoaiConnection_setBlockingMode( bool blocking_mode )
{
	st_blocking_mode = blocking_mode;
}
bool
MoaiConnection_isBlockingMode( void )
{
	return st_blocking_mode;
}

MoaiConnection
MoaiConnection_regist( const char* hostname, uint16_t port,
		ZnkSocket I_sock, ZnkSocket O_sock, MoaiFdSet mfds )
{
	MoaiConnection mcn = NULL;
	MoaiConnection_initiate();
	mcn = MoaiConnection_find_byISock( I_sock );
	if( mcn == NULL ){
		mcn = MoaiConnection_find_byISock( ZnkSocket_INVALID );
		if( mcn == NULL ){
			mcn = allocConnection();
			ZnkObjAry_push_bk( st_connection_ary, (ZnkObj)mcn );
		}
		MoaiConnection_clear( mcn, mfds );

		ZnkStr_set( mcn->hostname_, hostname );
		mcn->port_ = port;
		mcn->I_sock_      = I_sock;
		mcn->O_sock_      = O_sock;
	}
	return mcn;
}
MoaiConnection
MoaiConnection_connectO( const char* goal_hostname, uint16_t goal_port,
		const char* cnct_hostname, uint16_t cnct_port,
		ZnkSocket I_sock, MoaiFdSet mfds )
{
	MoaiConnection mcn = NULL;
	MoaiConnection_initiate();
	mcn = MoaiConnection_find_byISock( I_sock );
	if( mcn == NULL ){
		mcn = MoaiConnection_find_byISock( ZnkSocket_INVALID );
		if( mcn == NULL ){
			mcn = allocConnection();
			ZnkObjAry_push_bk( st_connection_ary, (ZnkObj)mcn );
		}
		mcn->O_sock_ = ZnkSocket_INVALID;
	}

	if( mcn->O_sock_ == ZnkSocket_INVALID ){
		ZnkErr_D( err );
		if( !ZnkStr_eq( mcn->hostname_, goal_hostname ) ){
			ZnkStr_set( mcn->hostname_, goal_hostname );
		}
		mcn->port_   = goal_port;
		mcn->I_sock_ = I_sock;
		mcn->O_sock_ = ZnkSocket_open();
		mcn->connect_begin_time_ = 0;

		/***
		 * Pipeline化に対応.
		 */
		ZnkBfr_clear( mcn->info_id_list_ );
		ZnkBfr_clear( mcn->prev_req_method_list_ );

		ZnkSocket_setBlockingMode( mcn->O_sock_, st_blocking_mode );

		/***
		 * dst側のサーバが明らかに稼動していない場合でも
		 * IPアドレスの解決ができればConnectInprogressとしてtrueを返す場合がある.
		 * この場合はConnectのTimeoutで防衛するしかない.
		 */
		MoaiLog_printf( "  Connecting... [%s:%d] O_sock=[%d]\n", cnct_hostname, cnct_port, mcn->O_sock_ );
		if( !ZnkSocket_connectToServer( mcn->O_sock_, cnct_hostname, cnct_port, &err, &mcn->is_connect_inprogress_ ) ){
			MoaiLog_printf( "  MoaiConnection_connectO : Error : cnct_host=[%s:%d] [%s] close O_sock=[%d]\n",
					cnct_hostname, cnct_port, ZnkErr_cstr(err), mcn->O_sock_ );
			ZnkSocket_close( mcn->O_sock_ );
			mcn->O_sock_ = ZnkSocket_INVALID;
		} else if( mcn->is_connect_inprogress_ ){
			mcn->connect_begin_time_ = (uint64_t)getCurrentSec();
		} else {
			/* Connectが即時成功した場合 */
		}
	}
	return mcn;
}
bool
MoaiConnection_connectFromISock( const char* hostname, uint16_t port, ZnkSocket I_sock, const char* label,
		MoaiInfoID info_id, MoaiFdSet mfds, bool is_proxy_use, MoaiConnectedCallback cb_func )
{
	MoaiConnection mcn = NULL;
	const char* cnct_hostname = "";
	uint16_t    cnct_port     = 0;
	ZnkSocket   O_sock;

	if( is_proxy_use ){
		cnct_hostname = MoaiParentProxy_getHostname();
		cnct_port     = MoaiParentProxy_getPort();
	} else {
		cnct_hostname = hostname;
		cnct_port     = port;
	}

	mcn = MoaiConnection_connectO( hostname, port, cnct_hostname, cnct_port, I_sock, mfds );
	O_sock = MoaiConnection_O_sock( mcn );
	if( O_sock == ZnkSocket_INVALID ){
		MoaiLog_printf( "  %s : Error connectO : goal_host=[%s:%d] cnct_host=[%s:%d]\n",
				label, hostname, port, cnct_hostname, cnct_port );
		return false;
	}

	MoaiConnection_pushConnectedEvent( mcn, cb_func, info_id );
	if( MoaiConnection_isConnectInprogress( mcn ) ){
		/* 非ブロッキングモードにおけるconnect進行中(inprogress)状態. */
		MoaiLog_printf( "  Connecting Inprogress : goal_host=[%s:%d] cnct_host=[%s:%d]\n",
				hostname, port, cnct_hostname, cnct_port );
		MoaiFdSet_addConnectingSock( mfds, O_sock );
	} else {
		/* connectが即時成功した場合 : 直ちに呼び出す */
		MoaiLog_printf( "  Connected : goal_host=[%s:%d] cnct_host=[%s:%d]\n",
				hostname, port, cnct_hostname, cnct_port );
		MoaiConnection_invokeCallback( mcn, mfds );
	}
	return true;
}


const char*
MoaiConnection_hostname( const MoaiConnection mcn )
{
	return ZnkStr_cstr( mcn->hostname_ );
}
uint16_t
MoaiConnection_port( const MoaiConnection mcn )
{
	return mcn->port_;
}
bool
MoaiConnection_isConnectInprogress( const MoaiConnection mcn )
{
	return mcn->is_connect_inprogress_;
}
void
MoaiConnection_setConnectInprogress( MoaiConnection mcn, bool is_connect_inprogress )
{
	mcn->is_connect_inprogress_ = is_connect_inprogress;
	if( !is_connect_inprogress ){
		mcn->connect_begin_time_ = 0;
	}
}
ZnkSocket
MoaiConnection_I_sock( const MoaiConnection mcn )
{
	return mcn->I_sock_;
}
ZnkSocket
MoaiConnection_O_sock( const MoaiConnection mcn )
{
	return mcn->O_sock_;
}
void
MoaiConnection_pushConnectedEvent( MoaiConnection mcn, MoaiConnectedCallback cb_func, MoaiInfoID info_id )
{
	/***
	 * Pipeline化に対応.
	 */
	mcn->cb_func_ = cb_func;
	ZnkBfr_push_bk_64( mcn->info_id_list_, info_id, Znk_isLE() );
}
static MoaiInfoID
atInfoID( MoaiConnection mcn, size_t idx )
{
	const MoaiInfoID* info_id_ary = (MoaiInfoID*)ZnkBfr_data( mcn->info_id_list_ );
	return info_id_ary[ idx ];
}
void
MoaiConnection_invokeCallback( MoaiConnection mcn, MoaiFdSet mfds )
{
	if( mcn->cb_func_ ){
		/***
		 * Pipeline化に対応.
		 */
		size_t idx;
		MoaiInfoID info_id = 0;
		MoaiInfo* info = NULL;
		const size_t info_id_list_size = ZnkBfr_size( mcn->info_id_list_ ) / sizeof( MoaiInfoID );
		if( info_id_list_size ){
			MoaiInfoIDStr id_str = {{ 0 }};
			ZnkBfr_clear( mcn->prev_req_method_list_ );
			for( idx=0; idx<info_id_list_size; ++idx ){
				info_id = atInfoID( mcn, idx );
				MoaiInfoID_getStr( info_id, &id_str );
				info = MoaiInfo_find( info_id );
				MoaiLog_printf( "MoaiConnection_invokeCallback idx=[%u] info_id=[%s] req_method=[%s]\n",
						idx, id_str.buf_,
						ZnkHtpReqMethod_getCStr( info->req_method_ ) );
				mcn->cb_func_( mcn, mfds, info_id );

				/***
				 * invoke済みなReqMethodをその処理された順で登録しておく.
				 * これは主に前回のReqMethodがCONNECTであったか否かの判定や
				 * デバッグ表示などで使う.
				 */
				ZnkBfr_push_bk_32( mcn->prev_req_method_list_, (uint32_t)info->req_method_, Znk_isLE() );
			}
			/***
			 * 万一、再invokeされてしまうことがないように
			 * invokeの完了したリストはここで確実にクリアしておく.
			 * (MoaiInfoの実体も削除してかまわないかもしれないがとりあえずほうっておく)
			 */
			for( idx=0; idx<info_id_list_size; ++idx ){
				ZnkBfr_clear( mcn->info_id_list_ );
			}
		}
	}
}

void
MoaiConnection_clear( MoaiConnection mcn, MoaiFdSet mfds )
{
	if( mcn ){
		ZnkStr_clear( mcn->hostname_ );
		if( mfds && mcn->I_sock_ != ZnkSocket_INVALID ){
			ZnkFdSet fdst_observe_r = MoaiFdSet_fdst_observe_r( mfds );
			ZnkFdSet_clr( fdst_observe_r, mcn->I_sock_ );
			ZnkSocket_close( mcn->I_sock_ );
			//MoaiLog_printf( "  MoaiConnection_clear close I_sock=[%d]\n", mcn->I_sock_ );
		}
		if( mfds && mcn->O_sock_ != ZnkSocket_INVALID ){
			ZnkFdSet fdst_observe_r = MoaiFdSet_fdst_observe_r( mfds );
			MoaiFdSet_removeConnectingSock( mfds, mcn->O_sock_ );
			ZnkFdSet_clr( fdst_observe_r, mcn->O_sock_ );
			ZnkSocket_close( mcn->O_sock_ );
			//MoaiLog_printf( "  MoaiConnection_clear close O_sock=[%d]\n", mcn->O_sock_ );
		}
		mcn->port_            = 0;
		mcn->I_sock_          = ZnkSocket_INVALID;
		mcn->I_is_keep_alive_ = false;
		mcn->O_sock_          = ZnkSocket_INVALID;
		mcn->content_length_remain_ = 0;
		mcn->exile_time_ = 0;
		mcn->connect_begin_time_    = 0;
		mcn->is_connect_inprogress_ = false;
		/***
		 * Pipeline化に対応.
		 */
		ZnkBfr_clear( mcn->info_id_list_ );
		ZnkBfr_clear( mcn->prev_req_method_list_ );
	}
}

MoaiConnection
MoaiConnection_find_byISock( const ZnkSocket query_sock )
{
	MoaiConnection mcn;
	const size_t size = ZnkObjAry_size( st_connection_ary );
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		mcn = (MoaiConnection) ZnkObjAry_at( st_connection_ary, idx );
		if( mcn->I_sock_ == query_sock ){
			/* found */
			return mcn;
		}
	}
	/* not found */
	return NULL;
}
MoaiConnection
MoaiConnection_find_byOSock( const ZnkSocket query_sock )
{
	MoaiConnection mcn;
	const size_t size = ZnkObjAry_size( st_connection_ary );
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		mcn = (MoaiConnection) ZnkObjAry_at( st_connection_ary, idx );
		if( mcn->O_sock_ == query_sock ){
			/* found */
			return mcn;
		}
	}
	/* not found */
	return NULL;
}

void
MoaiConnection_clearAll( MoaiFdSet mfds )
{
	MoaiConnection mcn;
	const size_t size = ZnkObjAry_size( st_connection_ary );
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		mcn = (MoaiConnection) ZnkObjAry_at( st_connection_ary, idx );
		MoaiConnection_clear( mcn, mfds );
	}
}

void
MoaiConnection_erase( MoaiConnection mcn, MoaiFdSet mfds )
{
	MoaiConnection_clear( mcn, mfds );
	ZnkObjAry_erase( st_connection_ary, (ZnkObj)mcn );
}

void
MoaiConnection_getPrevRequestMethodStr( const MoaiConnection mcn, char* buf, size_t buf_size )
{
	const size_t prev_req_method_size = ZnkBfr_size( mcn->prev_req_method_list_ ) / sizeof( ZnkHtpReqMethod );
	if( prev_req_method_size ){
		ZnkHtpReqMethod req_method = ZnkHtpReqMethod_e_Unknown;
		const char* req_method_str = "";
		size_t idx;
		uint32_t* ary = (uint32_t*)ZnkBfr_data( mcn->prev_req_method_list_ );
		for( idx=0; idx<prev_req_method_size; ++idx ){ 
			req_method = ary[ idx ];
			req_method_str = ZnkHtpReqMethod_getCStr( req_method );
			ZnkS_concat( buf, buf_size, req_method_str );
			if( idx != prev_req_method_size-1 ){
				ZnkS_concat( buf, buf_size, ":" );
			}
		}
	}
}

ZnkHtpReqMethod
MoaiConnection_getPrevRequestMethod( const MoaiConnection mcn, size_t idx )
{
	/***
	 * TODO:
	 * とりあえず通常 idxには Znk_NPOS を指定して取得すればよいだろう.
	 * この場合最後の要素を返すことにするが、HEAD/GETなどが組み合わさっているような場合に問題となる
	 * 可能性があるのか? それへの対処を考えなくてはならないかもしれない.
	 * POST/CONNECTについては、「これらをパイプライン化してはならない」というHTTP上の規則があるため、
	 * その可能性を心配しなくてもよい.
	 */
	const size_t prev_req_method_size = ZnkBfr_size( mcn->prev_req_method_list_ ) / sizeof( ZnkHtpReqMethod );
	if( prev_req_method_size ){
		ZnkHtpReqMethod req_method = ZnkHtpReqMethod_e_Unknown;
		uint32_t* ary = (uint32_t*)ZnkBfr_data( mcn->prev_req_method_list_ );
		if( idx == Znk_NPOS ){ idx = prev_req_method_size-1; }
		req_method = ary[ idx ];
		return req_method;
	}
	return ZnkHtpReqMethod_e_Unknown;
}
