#include "Moai_connection.h"

#include <Rano_parent_proxy.h>
#include <Rano_log.h>
#include <Znk_obj_ary.h>
#include <time.h>


static ZnkObjAry st_connection_ary = NULL;
static ZnkSocketAry st_listen_sockary = NULL;
static bool      st_blocking_mode  = false;

Znk_INLINE double
getCurrentSec( void ){ return ( 1.0 / CLOCKS_PER_SEC ) * clock() + 1.0; }


static MoaiConnection
allocConnection( void )
{
	MoaiConnection mcn = (MoaiConnection)Znk_alloc0( sizeof( struct MoaiConnection_tag ) );
	mcn->hostname_ = ZnkStr_new( "" );
	mcn->info_id_list_ = ZnkBfr_create_null();
	mcn->invoked_info_id_list_ = ZnkBfr_create_null();
	/***
	 * ����Linux�ɂ�����, fd=0(stdin)��close�����悤�Ȃ��Ƃ͐�΂ɔ����˂΂Ȃ�Ȃ�.
	 * ����0��close�����0�Ԃ��s�g�p�ɂȂ邽�߁AZnkSocket_open�Œl0��socket�����s�����悤�ɂȂ�.
	 * ���ꂪ�����select�ɓn������select�� EBADF �G���[���o���A�T�[�o�Ƃ��Ă̋@�\��
	 * �������h������鎖�ԂɊׂ�.
	 * ���̂��߁A�����Ŋm����INVALID�ŏ��������Ă������Ƃ��d�v�ł���.
	 */
	mcn->I_sock_ = ZnkSocket_getInvalid();
	mcn->O_sock_ = ZnkSocket_getInvalid();
	return mcn;
}
static void
deleteConnection( void* elem )
{
	if( elem ){
		MoaiConnection mcn = (MoaiConnection)elem;
		ZnkStr_delete( mcn->hostname_ );
		ZnkBfr_destroy( mcn->info_id_list_ );
		ZnkBfr_destroy( mcn->invoked_info_id_list_ );
		Znk_free( mcn );
	}
}

Znk_INLINE MoaiInfoID
atInfoID( ZnkBfr info_id_list_bfr, size_t idx )
{
	const MoaiInfoID* info_id_ary = (MoaiInfoID*)ZnkBfr_data( info_id_list_bfr );
	return info_id_ary[ idx ];
}
Znk_INLINE size_t
numofInfoID( ZnkBfr info_id_list_bfr )
{
	return ZnkBfr_size( info_id_list_bfr ) / sizeof( MoaiInfoID );
}
static void
eraseInvokedInfo( MoaiConnection mcn )
{
	const size_t size = numofInfoID( mcn->invoked_info_id_list_ );
	size_t       idx;
	MoaiInfoID   info_id;
	for( idx=0; idx<size; ++idx ){
		info_id = atInfoID( mcn->invoked_info_id_list_, idx );
		MoaiInfo_erase( info_id );
	}
	ZnkBfr_clear( mcn->invoked_info_id_list_ );
	mcn->response_idx_ = 0;
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
	if( st_listen_sockary == NULL ){
		st_listen_sockary = ZnkSocketAry_create();
	}
	if( ZnkSocketAry_find( st_listen_sockary, L_sock ) == Znk_NPOS ){
		ZnkSocketAry_push_bk( st_listen_sockary, L_sock );
	}
}
bool
MoaiConnection_isListeningSock( ZnkSocket query_sock )
{
	return (bool)( ZnkSocketAry_find( st_listen_sockary, query_sock ) != Znk_NPOS );
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
		mcn = MoaiConnection_find_byISock( ZnkSocket_getInvalid() );
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
		mcn = MoaiConnection_find_byISock( ZnkSocket_getInvalid() );
		if( mcn == NULL ){
			mcn = allocConnection();
			ZnkObjAry_push_bk( st_connection_ary, (ZnkObj)mcn );
		}
		mcn->O_sock_ = ZnkSocket_getInvalid();
	}

	if( ZnkSocket_isInvalid( mcn->O_sock_ ) ){
		ZnkErr_D( err );
		if( !ZnkStr_eq( mcn->hostname_, goal_hostname ) ){
			ZnkStr_set( mcn->hostname_, goal_hostname );
		}
		mcn->port_   = goal_port;
		mcn->I_sock_ = I_sock;
		mcn->O_sock_ = ZnkSocket_open();
		mcn->connect_begin_time_ = 0;

		/***
		 * Pipeline���ɑΉ�.
		 * invoked_info_id_list_ �͂����ł͂܂��N���A���Ȃ�.
		 * �܂�invoked���response�Ȃǂ���M����Ă��Ȃ��\�����L�蓾�邩��ł���.
		 * ����ɂ��Ă�connection�����S�N���[�Y����^�C�~���O�ŃN���A���s��.
		 */
		ZnkBfr_clear( mcn->info_id_list_ );

		ZnkSocket_setBlockingMode( mcn->O_sock_, st_blocking_mode );

		/***
		 * dst���̃T�[�o�����炩�ɉғ����Ă��Ȃ��ꍇ�ł�
		 * IP�A�h���X�̉������ł����ConnectInprogress�Ƃ���true��Ԃ��ꍇ������.
		 * ���̏ꍇ��Connect��Timeout�Ŗh�q���邵���Ȃ�.
		 */
		RanoLog_printf( "  Connecting... [%s:%u] O_sock=[%d]\n", cnct_hostname, cnct_port, mcn->O_sock_ );
		if( !ZnkSocket_connectToServer( mcn->O_sock_, cnct_hostname, cnct_port, &err, &mcn->is_connect_inprogress_ ) ){
			RanoLog_printf( "  MoaiConnection_connectO : Error : cnct_host=[%s:%u] [%s] close O_sock=[%d]\n",
					cnct_hostname, cnct_port, ZnkErr_cstr(err), mcn->O_sock_ );
			ZnkSocket_close( mcn->O_sock_ );
			mcn->O_sock_ = ZnkSocket_getInvalid();
		} else if( mcn->is_connect_inprogress_ ){
			mcn->connect_begin_time_ = (uint64_t)getCurrentSec();
		} else {
			/* Connect���������������ꍇ */
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
		cnct_hostname = RanoParentProxy_getHostname();
		cnct_port     = RanoParentProxy_getPort();
	} else {
		cnct_hostname = hostname;
		cnct_port     = port;
	}

	mcn = MoaiConnection_connectO( hostname, port, cnct_hostname, cnct_port, I_sock, mfds );
	O_sock = MoaiConnection_O_sock( mcn );
	if( ZnkSocket_isInvalid( O_sock ) ){
		RanoLog_printf( "  %s : Error connectO : goal_host=[%s:%u] cnct_host=[%s:%u]\n",
				label, hostname, port, cnct_hostname, cnct_port );
		return false;
	}

	MoaiConnection_pushConnectedEvent( mcn, cb_func, info_id );
	if( MoaiConnection_isConnectInprogress( mcn ) ){
		/* ��u���b�L���O���[�h�ɂ�����connect�i�s��(inprogress)���. */
		RanoLog_printf( "  Connecting Inprogress : goal_host=[%s:%u] cnct_host=[%s:%u]\n",
				hostname, port, cnct_hostname, cnct_port );
		MoaiFdSet_addConnectingSock( mfds, O_sock );
	} else {
		/* connect���������������ꍇ : �����ɌĂяo�� */
		RanoLog_printf( "  Connected : goal_host=[%s:%u] cnct_host=[%s:%u]\n",
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
	 * Pipeline���ɑΉ�.
	 */
	mcn->cb_func_ = cb_func;
	ZnkBfr_push_bk_64( mcn->info_id_list_, info_id, Znk_isLE() );
}
bool
MoaiConnection_invokeCallback( MoaiConnection mcn, MoaiFdSet mfds )
{
	if( mcn->cb_func_ ){
		/***
		 * Pipeline���ɑΉ�.
		 */
		size_t idx;
		MoaiInfoID info_id = 0;
		MoaiInfo* info = NULL;
		const size_t info_id_list_size = numofInfoID( mcn->info_id_list_ );
		if( info_id_list_size ){
			MoaiInfoIDStr id_str = {{ 0 }};
			for( idx=0; idx<info_id_list_size; ++idx ){
				info_id = atInfoID( mcn->info_id_list_, idx );
				MoaiInfoID_getStr( info_id, &id_str );
				info = MoaiInfo_find( info_id );
				RanoLog_printf( "  MoaiConnection_invokeCallback idx=[%zu] info_id=[%s] req_method=[%s] req_urp=[%s]\n",
						idx, id_str.buf_,
						ZnkHtpReqMethod_getCStr( info->req_method_ ),
						ZnkStr_cstr(info->req_urp_) );
				if( !mcn->cb_func_( mcn, mfds, info_id ) ){
					/***
					 * ���̏ꍇ cb_func_����mcn���폜����Ă���\��������.
					 * ����Ă����ŋ������f.
					 */
					return false;
				}

				/***
				 * invoke�ς݂�info_id�����̏������ꂽ���œo�^���Ă���.
				 * ����͎�ɑO���ReqMethod��CONNECT�ł��������ۂ��̔����
				 * Response���̕\���ȂǂŎg��.
				 */
				ZnkBfr_push_bk_64( mcn->invoked_info_id_list_, (uint64_t)info_id, Znk_isLE() );
			}
			/***
			 * ����A��invoke����Ă��܂����Ƃ��Ȃ��悤��
			 * invoke�̊����������X�g�͂����Ŋm���ɃN���A���Ă���.
			 * MoaiInfo�̎��͍̂폜�����c���Ă���.
			 */
			for( idx=0; idx<info_id_list_size; ++idx ){
				ZnkBfr_clear( mcn->info_id_list_ );
			}
		}
	}
	return true;
}

void
MoaiConnection_clear( MoaiConnection mcn, MoaiFdSet mfds )
{
	if( mcn ){
		const ZnkSocket invalid_sock = ZnkSocket_getInvalid();
		ZnkStr_clear( mcn->hostname_ );
		if( mfds && mcn->I_sock_ != invalid_sock ){
			ZnkFdSet fdst_observe_r = MoaiFdSet_fdst_observe_r( mfds );
			ZnkFdSet_clr( fdst_observe_r, mcn->I_sock_ );
			ZnkSocket_close( mcn->I_sock_ );
		}
		if( mfds && mcn->O_sock_ != invalid_sock ){
			ZnkFdSet fdst_observe_r = MoaiFdSet_fdst_observe_r( mfds );
			MoaiFdSet_removeConnectingSock( mfds, mcn->O_sock_ );
			ZnkFdSet_clr( fdst_observe_r, mcn->O_sock_ );
			ZnkSocket_close( mcn->O_sock_ );
		}
		mcn->port_            = 0;
		mcn->I_sock_          = invalid_sock;
		mcn->I_is_keep_alive_ = false;
		mcn->O_sock_          = invalid_sock;
		mcn->req_content_length_remain_ = 0;
		RanoLog_printf( "  mcn->req_content_length_remain_=0 (MoaiConnection_clear)\n" );
		mcn->res_content_length_remain_ = 0;
		//mcn->exile_time_ = getCurrentSec();
		mcn->exile_time_ = 0;
		mcn->connect_begin_time_    = 0;
		mcn->is_connect_inprogress_ = false;
		/***
		 * Pipeline���ɑΉ�.
		 */
		ZnkBfr_clear( mcn->info_id_list_ );
		eraseInvokedInfo( mcn );
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


MoaiInfo*
MoaiConnection_getInvokedInfo( const MoaiConnection mcn, size_t idx )
{
	const size_t size = numofInfoID( mcn->invoked_info_id_list_ );
	MoaiInfoID info_id;
	if( size ){
		if( idx == Znk_NPOS ){ idx = size-1; }
		info_id = atInfoID( mcn->invoked_info_id_list_, idx );
		return MoaiInfo_find( info_id );
	}
	return NULL;
}
size_t
MoaiConnection_countResponseIdx( MoaiConnection mcn )
{
	size_t idx = mcn->response_idx_;
	++mcn->response_idx_;
	return idx;
}

