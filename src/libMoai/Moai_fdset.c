#include "Moai_fdset.h"
#include "Moai_connection.h"
#include <Rano_log.h>
#include <Znk_fdset.h>
#include <Znk_socket_ary.h>
#include <Znk_def_util.h>
#include <Znk_missing_libc.h>
#include <Znk_net_ip.h>
#include <time.h>
#include <assert.h>

struct MoaiFdSetImpl {
	ZnkSocketAry      listen_sockary_;
	ZnkFdSet          fdst_read_;
	ZnkFdSet          fdst_write_;
	ZnkFdSet          fdst_observe_r_;
	ZnkFdSet          fdst_observe_w_;
	ZnkSocketAry      connecting_socks_;
	ZnkSocketAry      reserve_accept_socks_;
	ZnkSocketAry      reserve_connect_socks_;
	ZnkSocketAry      wk_sock_ary_;
	struct ZnkTimeval waitval_;
};

Znk_INLINE double
getCurrentSec( void ){ return ( 1.0 / CLOCKS_PER_SEC ) * clock() + 1.0; }

MoaiFdSet
MoaiFdSet_create( ZnkSocket listen_sock, struct ZnkTimeval* waitval )
{
	MoaiFdSet mfds = Znk_malloc( sizeof( struct MoaiFdSetImpl ) );

	/* create+push_bk-one */
	mfds->listen_sockary_ = ZnkSocketAry_create();
	ZnkSocketAry_push_bk( mfds->listen_sockary_, listen_sock );

	mfds->fdst_read_    = ZnkFdSet_create();
	mfds->fdst_write_   = ZnkFdSet_create();

	mfds->fdst_observe_r_ = ZnkFdSet_create();
	mfds->fdst_observe_w_ = ZnkFdSet_create();
	ZnkFdSet_zero( mfds->fdst_observe_r_ );
	ZnkFdSet_zero( mfds->fdst_observe_w_ );

	/* �P��̃��X�j���O�\�P�b�g���Ď��Ώۂɒǉ� */
	ZnkFdSet_set( mfds->fdst_observe_r_, listen_sock );

	mfds->connecting_socks_      = ZnkSocketAry_create();
	mfds->reserve_accept_socks_  = ZnkSocketAry_create();
	mfds->reserve_connect_socks_ = ZnkSocketAry_create();
	mfds->wk_sock_ary_          = ZnkSocketAry_create();

	mfds->waitval_ = *waitval;
	return mfds;
}
MoaiFdSet
MoaiFdSet_create_ex( ZnkSocketAry listen_sockary, struct ZnkTimeval* waitval )
{
	MoaiFdSet mfds = Znk_malloc( sizeof( struct MoaiFdSetImpl ) );

	/* create+copy */
	mfds->listen_sockary_ = ZnkSocketAry_create();
	ZnkSocketAry_copy( mfds->listen_sockary_, listen_sockary );

	mfds->fdst_read_    = ZnkFdSet_create();
	mfds->fdst_write_   = ZnkFdSet_create();

	mfds->fdst_observe_r_ = ZnkFdSet_create();
	mfds->fdst_observe_w_ = ZnkFdSet_create();
	ZnkFdSet_zero( mfds->fdst_observe_r_ );
	ZnkFdSet_zero( mfds->fdst_observe_w_ );

	/* �����̃��X�j���O�\�P�b�g���Ď��Ώۂɒǉ� */
	{
		const size_t size = ZnkSocketAry_size( listen_sockary );
		size_t idx;
		for( idx=0; idx<size; ++idx ){
			ZnkFdSet_set( mfds->fdst_observe_r_, ZnkSocketAry_at( listen_sockary, idx ) );
		}
	}

	mfds->connecting_socks_      = ZnkSocketAry_create();
	mfds->reserve_accept_socks_  = ZnkSocketAry_create();
	mfds->reserve_connect_socks_ = ZnkSocketAry_create();
	mfds->wk_sock_ary_          = ZnkSocketAry_create();

	mfds->waitval_ = *waitval;
	return mfds;
}
void
MoaiFdSet_destroy( MoaiFdSet mfds )
{
	if( mfds ){
		ZnkSocketAry_destroy( mfds->listen_sockary_ );
		ZnkFdSet_destroy( mfds->fdst_read_ );
		ZnkFdSet_destroy( mfds->fdst_write_ );
		ZnkFdSet_destroy( mfds->fdst_observe_r_ );
		ZnkFdSet_destroy( mfds->fdst_observe_w_ );
		ZnkSocketAry_destroy( mfds->connecting_socks_ );
		ZnkSocketAry_destroy( mfds->reserve_accept_socks_ );
		ZnkSocketAry_destroy( mfds->reserve_connect_socks_ );
		ZnkSocketAry_destroy( mfds->wk_sock_ary_ );
		Znk_free( mfds );
	}
}

ZnkFdSet
MoaiFdSet_fdst_observe_r( MoaiFdSet mfds )
{
	return mfds->fdst_observe_r_;
}
ZnkFdSet
MoaiFdSet_fdst_observe_w( MoaiFdSet mfds )
{
	return mfds->fdst_observe_w_;
}
ZnkSocketAry
MoaiFdSet_wk_sock_ary( MoaiFdSet mfds )
{
	return mfds->wk_sock_ary_;
}
void
MoaiFdSet_reserveConnectSock( MoaiFdSet mfds, ZnkSocket sock )
{
	size_t idx = ZnkSocketAry_find( mfds->reserve_connect_socks_, sock );
	if( idx == Znk_NPOS ){
		ZnkSocketAry_push_bk( mfds->reserve_connect_socks_, sock );
	}
}
void
MoaiFdSet_addConnectingSock( MoaiFdSet mfds, ZnkSocket sock )
{
	ZnkFdSet fdst_observe_w = MoaiFdSet_fdst_observe_w( mfds );
	size_t idx = ZnkSocketAry_find( mfds->connecting_socks_, sock );
	if( idx == Znk_NPOS ){
		RanoLog_printf( "  MoaiFdSet_addConnectingSock : sock=[%d]\n", sock );
		ZnkSocketAry_push_bk( mfds->connecting_socks_, sock );
	}
	ZnkFdSet_set( fdst_observe_w, sock );
}
void
MoaiFdSet_removeConnectingSock( MoaiFdSet mfds, ZnkSocket sock )
{
	ZnkFdSet fdst_observe_w = MoaiFdSet_fdst_observe_w( mfds );
	ZnkSocketAry_erase( mfds->connecting_socks_, sock );
	ZnkFdSet_clr( fdst_observe_w, sock );
}
void
MoaiFdSet_procConnectionTimeout( MoaiFdSet mfds )
{
	const size_t size = ZnkSocketAry_size( mfds->connecting_socks_ );
	size_t idx;
	ZnkSocket cncting_sock;
	MoaiConnection mcn;
	/***
	 * TODO:
	 * �T�C�g�ɂ���ă^�C���A�E�g�̎��Ԃ𓮓I�ɐ؂�ւ�����.
	 * �Ⴆ�Έ�x�ɑ�ʂ̐ڑ����s���摜�f���Ȃǂł͒Z���Ă悢���A
	 * ����X�g���[�~���O�̍Đ��Ȃǂł͋t�ɖ����Ƃ�����.
	 * (�r���Őڑ����ؒf�����ƁA�܂��ŏ�����Đ����Ȃ���΂Ȃ�Ȃ��悤�ȃP�[�X������̂�)
	 * ����ɂ��̕����ɊȈՂȊw�K�𓱓����A����T�C�g���g�p����z�X�g�𐄒肵�����Ƃ���.
	 */
	static const int connection_timeout_sec = 45;
	for( idx=0; idx<size; ++idx ){
		/***
		 * erase�����𔺂����߁A�t���Ɏ��o��.
		 */
		cncting_sock = ZnkSocketAry_at( mfds->connecting_socks_, size-1-idx );
		if( ZnkSocket_isInvalid( cncting_sock ) ){
			MoaiFdSet_removeConnectingSock( mfds, cncting_sock );
			continue;
		}
		mcn = MoaiConnection_find_byOSock( cncting_sock );
		if( mcn && mcn->connect_begin_time_ ){
			uint64_t now = (uint64_t)getCurrentSec();
			if( now - mcn->connect_begin_time_ > connection_timeout_sec ){
				MoaiFdSet_removeConnectingSock( mfds, mcn->O_sock_ );
				ZnkSocket_close( mcn->O_sock_ );
				RanoLog_printf( "procOnnectionTimeout : [%s:%u] Interrupt connecting by Timeout. close sock=[%d]\n",
						ZnkStr_cstr(mcn->hostname_), mcn->port_, mcn->O_sock_ );
				mcn->O_sock_ = ZnkSocket_getInvalid();
			}
		}
	}
}

int
MoaiFdSet_select( MoaiFdSet mfds, bool* req_before_report, MoaiFdSetFuncArg_Report* fnca_report )
{
	const ZnkSocket invalid_sock = ZnkSocket_getInvalid();
	ZnkFdSet fdst_read      = mfds->fdst_read_;
	ZnkFdSet fdst_write     = mfds->fdst_write_;
	ZnkFdSet fdst_observe_r = mfds->fdst_observe_r_;
	ZnkFdSet fdst_observe_w = mfds->fdst_observe_w_;
	ZnkSocket maxfd_r;
	ZnkSocket maxfd_w;
	ZnkSocket maxfd;
	int sel_ret;

	/* fds_observe_r/fds_observe_w �� fds_read/fdst_write �ɃR�s�[ */
	ZnkFdSet_copy( fdst_read,  fdst_observe_r );
	ZnkFdSet_copy( fdst_write, fdst_observe_w );
	
	/***
	 * INVALID �̏ꍇ�͏��O����悤�ɒ��ӂ��邱��.
	 * ����ZnkSocket��unsigned �����Ƃ��Ē�`���Ă���̂�
	 * ���̏ꍇ�A���L��Znk_MAX�� INVALID ��Ԃ��A
	 * maxfd�̒l�� INVALID �Ƃ���select�ɓn����Ă��܂�.
	 */
	maxfd_r = ZnkFdSet_getMaxOfSocket( fdst_read );
	maxfd_w = ZnkFdSet_getMaxOfSocket( fdst_write );
	if( maxfd_r != invalid_sock && maxfd_w != invalid_sock ){
		maxfd = Znk_MAX( maxfd_r, maxfd_w );
	} else if( maxfd_r != invalid_sock ){
		maxfd = maxfd_r;
	} else {
		maxfd = maxfd_w;
	}

	/***
	 * ��1����:
	 *   Unix�ł�select�̑�1������ fdst_observe_r ����� fdst_observe_w �Ɏw�肵�Ă���Ssock�̂�����
	 *   �ő�l+1�ȏ�̒l���w�肷��. ������sock�l��FD_SETSIZE�𒴂��Ȃ����Ƃ��ۏႳ��Ă���̂�
	 *   �蔲�����������ꍇ�͂�����FD_SETSIZE���w�肵�Ă��悢.
	 *
	 *   Windows�ł�select�̑�1�����͎g���Ă��Ȃ����߁A���̂Ƃ���Ȃ�ł��悢.
	 *
	 * ��2����:
	 *   fdst_read�͂����œ��͂Əo�̗͂��������˔����Ă���.
	 *   �Ď��ΏۂƂ��������ׂĂ�socket�̏W����fdst_read�ɃZ�b�g���Ă���.
	 *   ���ꂪ���͂ƂȂ�.
	 *   select���琧�䂩�A���ė����Ƃ��Afdst_read�ɂ͕ω��̔�������socket�̏W���݂̂�
	 *   �i�[����Ă���(����͍ŏ��ɗ^����fdst_read�̕����W���ł���).
	 *   ���ꂪ�o�͂ƂȂ�.
	 *   ��������΁A�ω��̔������Ă��Ȃ�socket�Q�͌��̏W�����獷��������Ă���.
	 *   �ȉ��ł͂���fdst_read�̌��ʂ𒲍�����΂悢.
	 *
	 */
	if( *req_before_report ){
		*req_before_report = false;
		if( fnca_report ){
			fnca_report->func_( mfds, fnca_report->arg_ );
		}
	}
	sel_ret = ZnkFdSet_select( maxfd+1, fdst_read, fdst_write, NULL, &mfds->waitval_ );
	/***
	 * ����������after_report�n�̊֐����T�|�[�g���邩������Ȃ�.
	 */
	return sel_ret;
}

static void
adoptReserveSocks( ZnkFdSet fdst_observe_r, ZnkSocketAry reserve_socks, ZnkSocketAry wk_sock_ary )
{
	const ZnkSocket invalid_sock = ZnkSocket_getInvalid();
	size_t idx;
	size_t size;
	ZnkSocket sock;
	bool updated = false;
	size = ZnkSocketAry_size( reserve_socks );
	for( idx=0; idx<size; ++idx ){ 
		sock = ZnkSocketAry_at( reserve_socks, idx );
		if( ZnkFdSet_set( fdst_observe_r, sock ) ){
			/* marking erase */
			updated = true;
			RanoLog_printf( "  Moai : adoptReserveSocks [%d] under fdst_observe_r.\n", sock );
			ZnkSocketAry_set( reserve_socks, idx, invalid_sock );
		}
	}
	/* compaction */
	if( updated ){
		ZnkSocketAry_clear( wk_sock_ary );
		for( idx=0; idx<size; ++idx ){ 
			sock = ZnkSocketAry_at( reserve_socks, idx );
			if( sock != invalid_sock ){
				ZnkSocketAry_push_bk( wk_sock_ary, sock );
			}
		}
		ZnkSocketAry_swap( wk_sock_ary, reserve_socks );
	}
}

static void
processListenSock( MoaiFdSet mfds, ZnkSocket listen_sock,
		ZnkFdSet fdst_read,
		ZnkFdSet fdst_observe_r,
		MoaiFdSetFuncArg_OnAccept* fnca_on_accept,
		MoaiFdSetFuncT_IsAccessAllowIP is_access_allow_ip )
{
	uint32_t peer_ipaddr = 0;
	/***
	 * fdst_read��ListenSocket���c���Ă����ꍇ�́A
	 * ListenSocket�ɐV�K�ڑ��v���������������Ƃ�����.
	 * ���̏ꍇ�A�܂������D��I�ɏ�������.
	 */
	ZnkSocket new_accept_sock = ZnkSocket_accept( listen_sock );
	if( is_access_allow_ip &&
		!is_access_allow_ip( new_accept_sock, &peer_ipaddr ) )
	{
		/***
		 * ������Ȃ�IP����̐ڑ��ł������ꍇ.
		 */
		char response[ 4096 ] = "";
		char ipstr[ 64 ] = "";
		ZnkNetIP_getIPStr_fromU32( peer_ipaddr, ipstr, sizeof(ipstr) );
		RanoLog_printf( "Moai : peer ipaddr=[%s] is forbidden.\n", ipstr );
		Znk_snprintf( response, sizeof(response), 
				"HTTP/1.0 403 Forbidden\r\nContent-Type: text/html\r\n\r\n"
				"Sorry, your IP=[%s] is forbidden for security reason.", ipstr );
		ZnkSocket_send( new_accept_sock, (uint8_t*)response, Znk_strlen(response) );
		ZnkSocket_close( new_accept_sock );
	} else {
		/***
		 * ���̏ꍇ�ڑ�������.
		 */
		if( fnca_on_accept ){
			fnca_on_accept->func_( mfds, new_accept_sock, fnca_on_accept->arg_ );
		}

		/***
		 * �Ď��\�P�b�g�W���֒ǉ�.
		 */
		if( !ZnkFdSet_set( fdst_observe_r, new_accept_sock ) ){
			/***
			 * ���s�����ꍇ��reserve_accept_socks_�փX�g�b�N���Ă���.
			 */
			RanoLog_printf( "  ZnkFdSet_set failure. sock(by accept)=[%d]\n", new_accept_sock );
			ZnkSocketAry_push_bk( mfds->reserve_accept_socks_, new_accept_sock ); 
		}
	}
	/***
	 * listen_sock�Ɋւ��鏈���͏I�����̂ŁA
	 * fdst_read���炱��͏���.
	 */
	ZnkFdSet_clr( fdst_read, listen_sock );
}

MoaiRASResult
MoaiFdSet_process( MoaiFdSet mfds,
		MoaiFdSetFuncArg_OnAccept* fnca_on_accept,
		MoaiFdSetFuncArg_RAS* fnca_ras,
		MoaiFdSetFuncT_IsAccessAllowIP is_access_allow_ip )
{
	ZnkFdSet fdst_read      = mfds->fdst_read_;
	ZnkFdSet fdst_write     = mfds->fdst_write_;
	ZnkFdSet fdst_observe_r = mfds->fdst_observe_r_;
	ZnkSocketAry wk_sock_ary = mfds->wk_sock_ary_;
	MoaiRASResult ras_result = MoaiRASResult_e_Ignored;

	/***
	 * �����ł͕�����listen_sock�̏����ɑΉ�����.
	 */
	{
		const size_t size = ZnkSocketAry_size( mfds->listen_sockary_ );
		size_t idx;
		for( idx=0; idx<size; ++idx ){
			ZnkSocket listen_sock = ZnkSocketAry_at( mfds->listen_sockary_, idx );
			if( ZnkFdSet_isset( fdst_read, listen_sock ) ){
				processListenSock( mfds, listen_sock,
						fdst_read,
						fdst_observe_r,
						fnca_on_accept,
						is_access_allow_ip );
			}
		}
	}

	/***
	 * fdst_write�̏���.
	 * connect�̊����̕ߑ��ȂǂŎg�p.
	 */
	{
		const ZnkSocket invalid_sock = ZnkSocket_getInvalid();
		size_t cnt_idx;
		const size_t cnt_size = ZnkSocketAry_size( mfds->connecting_socks_ );
		ZnkSocket connecting_sock = invalid_sock;

		/***
		 * mfds->connecting_socks_�� MoaiConnection_invokeCallback( mcn, mfds ) �Ăяo����
		 * �v�f��erase�����\�������邽�߁A��Uwk_sock_ary_�ɔz��S�̂��R�s�[���Ă���.
		 */
		ZnkSocketAry_copy( wk_sock_ary, mfds->connecting_socks_ );

		for( cnt_idx=0; cnt_idx<cnt_size; ++cnt_idx ){
			connecting_sock = ZnkSocketAry_at( wk_sock_ary, cnt_idx );
			if( ZnkFdSet_isset( fdst_write, connecting_sock ) ){
				/* OK. */
				MoaiConnection mcn = MoaiConnection_find_byOSock( connecting_sock );
				ZnkSocketAry_set( mfds->connecting_socks_, cnt_idx, invalid_sock );
				if( mcn ){
					MoaiConnection_setConnectInprogress( mcn, false );
					RanoLog_printf( "  Connection Inprogress Completed : sock=[%d]\n", connecting_sock );
					MoaiConnection_invokeCallback( mcn, mfds );
				} else {
					/***
					 * �ʏ�N���肦�Ȃ����Ȃ�炩�̗��R��MoaiConnection��connecting_sock��o�^�����т�Ă���.
					 */
					RanoLog_printf( "  Connection Inprogress Failure : sock=[%d] does not exist in MoaiConnection\n", connecting_sock );
				}
				ZnkFdSet_clr( mfds->fdst_observe_w_, connecting_sock );
			}
		}

		/* Compaction (reverse order iteration for erase) */
		if( cnt_size ){
			for( cnt_idx=cnt_size-1; cnt_idx > 0; --cnt_idx ){
				connecting_sock = ZnkSocketAry_at( mfds->connecting_socks_, cnt_idx );
				if( connecting_sock == invalid_sock ){
					ZnkSocketAry_erase_byIdx( mfds->connecting_socks_, cnt_idx );
				}
			}
		}
	}

	{
		/***
		 * ���̎��_��fdst_read�Ɋi�[����Ă���̂́A���ɃN���C�A���g�Ƃ�
		 * �ڑ��ς�Socket�݂̂ł���͂��ł���.
		 * �����ł͂�������o���Arecv/send�����Ȃǂ��s��.
		 */
		size_t i;
		size_t sock_ary_size;
		const size_t fd_setsize = ZnkFdSet_FD_SETSIZE();

		ZnkSocketAry_clear( wk_sock_ary );
		ZnkFdSet_getSocketAry( fdst_read, wk_sock_ary );
		sock_ary_size = ZnkSocketAry_size( wk_sock_ary );

		Znk_UNUSED( fd_setsize );
		assert( sock_ary_size <= fd_setsize );

		for( i=0; i<sock_ary_size; ++i ){
			ZnkSocket  sock = ZnkSocketAry_at( wk_sock_ary, i );
			if( fnca_ras ){
				ras_result = fnca_ras->func_( mfds, sock, fnca_ras->arg_ );
				switch( ras_result ){
				case MoaiRASResult_e_CriticalError:
				case MoaiRASResult_e_RestartServer:
					return ras_result;
				default:
					break;
				}
			}
		}
	}

	/***
	 * �Ō�� reserve_accept_socks_ ����� reserve_connect_socks_ �������ł��Ȃ���������.
	 * (fnca_ras->func_�̌Ăяo����fdst_observe_r�ɋ󂫂��������\��������)
	 */
	adoptReserveSocks( mfds->fdst_observe_r_, mfds->reserve_accept_socks_, mfds->wk_sock_ary_ );
	adoptReserveSocks( mfds->fdst_observe_r_, mfds->reserve_connect_socks_, mfds->wk_sock_ary_ );
	return ras_result;
}

void
MoaiFdSet_printf_fdst_read( MoaiFdSet mfds )
{
	ZnkFdSet      fdst_read    = mfds->fdst_read_;
	ZnkSocketAry wk_sock_ary = ZnkSocketAry_create();
	size_t idx;
	size_t size;

	ZnkFdSet_getSocketAry( fdst_read, wk_sock_ary );
	size = ZnkSocketAry_size( wk_sock_ary );
	RanoLog_printf( "  fdst_read=[" );
	for( idx=0; idx<size; ++idx ){
		RanoLog_printf( "%d", ZnkSocketAry_at( wk_sock_ary, idx ) );
		if( idx != size-1 ){
			RanoLog_printf( "," );
		}
	}
	RanoLog_printf( "]\n" );
	ZnkSocketAry_destroy( wk_sock_ary );
}
