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

	/* 単一のリスニングソケットを監視対象に追加 */
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

	/* 複数のリスニングソケットを監視対象に追加 */
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
	 * サイトによってタイムアウトの時間を動的に切り替えたい.
	 * 例えば一度に大量の接続を行う画像掲示板などでは短くてよいが、
	 * 動画ストリーミングの再生などでは逆に無限としたい.
	 * (途中で接続が切断されると、また最初から再生しなければならないようなケースもあるので)
	 * さらにこの部分に簡易な学習を導入し、動画サイトが使用するホストを推定したいところ.
	 */
	static const int connection_timeout_sec = 45;
	for( idx=0; idx<size; ++idx ){
		/***
		 * erase処理を伴うため、逆順に取り出す.
		 */
		cncting_sock = ZnkSocketAry_at( mfds->connecting_socks_, size-1-idx );
		if( cncting_sock == ZnkSocket_INVALID ){
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
				mcn->O_sock_ = ZnkSocket_INVALID;
			}
		}
	}
}

int
MoaiFdSet_select( MoaiFdSet mfds, bool* req_before_report, MoaiFdSetFuncArg_Report* fnca_report )
{
	ZnkFdSet fdst_read      = mfds->fdst_read_;
	ZnkFdSet fdst_write     = mfds->fdst_write_;
	ZnkFdSet fdst_observe_r = mfds->fdst_observe_r_;
	ZnkFdSet fdst_observe_w = mfds->fdst_observe_w_;
	ZnkSocket maxfd_r;
	ZnkSocket maxfd_w;
	ZnkSocket maxfd;
	int sel_ret;

	/* fds_observe_r/fds_observe_w を fds_read/fdst_write にコピー */
	ZnkFdSet_copy( fdst_read,  fdst_observe_r );
	ZnkFdSet_copy( fdst_write, fdst_observe_w );
	
	/***
	 * ZnkSocket_INVALID(=-1)の場合は除外するように注意すること.
	 * 特にZnkSocketはunsigned 整数として定義してあるので
	 * この場合、下記のZnk_MAXはZnkSocket_INVALIDを返し、
	 * maxfdの値が-1としてselectに渡されてしまう.
	 */
	maxfd_r = ZnkFdSet_getMaxOfSocket( fdst_read );
	maxfd_w = ZnkFdSet_getMaxOfSocket( fdst_write );
	if( maxfd_r != ZnkSocket_INVALID && maxfd_w != ZnkSocket_INVALID ){
		maxfd = Znk_MAX( maxfd_r, maxfd_w );
	} else if( maxfd_r != ZnkSocket_INVALID ){
		maxfd = maxfd_r;
	} else {
		maxfd = maxfd_w;
	}

	/***
	 * 第1引数:
	 *   Unixではselectの第1引数は fdst_observe_r および fdst_observe_w に指定している全sockのうちの
	 *   最大値+1以上の値を指定する. しかしsock値はFD_SETSIZEを超えないことも保障されているので
	 *   手抜きをしたい場合はここにFD_SETSIZEを指定してもよい.
	 *
	 *   Windowsではselectの第1引数は使われていないため、実のところなんでもよい.
	 *
	 * 第2引数:
	 *   fdst_readはこれ一つで入力と出力の両方を兼ね備えている.
	 *   監視対象としたいすべてのsocketの集合をfdst_readにセットしておく.
	 *   これが入力となる.
	 *   selectから制御か帰って来たとき、fdst_readには変化の発生したsocketの集合のみが
	 *   格納されている(これは最初に与えたfdst_readの部分集合である).
	 *   これが出力となる.
	 *   換言すれば、変化の発生していないsocket群は元の集合から差し引かれている.
	 *   以下ではこのfdst_readの結果を調査すればよい.
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
	 * 将来ここにafter_report系の関数をサポートするかもしれない.
	 */
	return sel_ret;
}

static void
adoptReserveSocks( ZnkFdSet fdst_observe_r, ZnkSocketAry reserve_socks, ZnkSocketAry wk_sock_ary )
{
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
			ZnkSocketAry_set( reserve_socks, idx, ZnkSocket_INVALID );
		}
	}
	/* compaction */
	if( updated ){
		ZnkSocketAry_clear( wk_sock_ary );
		for( idx=0; idx<size; ++idx ){ 
			sock = ZnkSocketAry_at( reserve_socks, idx );
			if( sock != ZnkSocket_INVALID ){
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
	 * fdst_readにListenSocketが残っていた場合は、
	 * ListenSocketに新規接続要求が発生したことを示す.
	 * この場合、まずこれを優先的に処理する.
	 */
	ZnkSocket new_accept_sock = ZnkSocket_accept( listen_sock );
	if( is_access_allow_ip &&
		!is_access_allow_ip( new_accept_sock, &peer_ipaddr ) )
	{
		/***
		 * 許可されないIPからの接続であった場合.
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
		 * この場合接続を許可.
		 */
		if( fnca_on_accept ){
			fnca_on_accept->func_( mfds, new_accept_sock, fnca_on_accept->arg_ );
		}

		/***
		 * 監視ソケット集合へ追加.
		 */
		if( !ZnkFdSet_set( fdst_observe_r, new_accept_sock ) ){
			/***
			 * 失敗した場合はreserve_accept_socks_へストックしておく.
			 */
			RanoLog_printf( "  ZnkFdSet_set failure. sock(by accept)=[%d]\n", new_accept_sock );
			ZnkSocketAry_push_bk( mfds->reserve_accept_socks_, new_accept_sock ); 
		}
	}
	/***
	 * listen_sockに関する処理は終ったので、
	 * fdst_readからこれは除去.
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
	 * ここでは複数のlisten_sockの処理に対応する.
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
	 * fdst_writeの処理.
	 * connectの完了の捕捉などで使用.
	 */
	{
		size_t cnt_idx;
		const size_t cnt_size = ZnkSocketAry_size( mfds->connecting_socks_ );
		ZnkSocket connecting_sock = ZnkSocket_INVALID;

		/***
		 * mfds->connecting_socks_は MoaiConnection_invokeCallback( mcn, mfds ) 呼び出しで
		 * 要素がeraseされる可能性があるため、一旦wk_sock_ary_に配列全体をコピーしておく.
		 */
		ZnkSocketAry_copy( wk_sock_ary, mfds->connecting_socks_ );

		for( cnt_idx=0; cnt_idx<cnt_size; ++cnt_idx ){
			connecting_sock = ZnkSocketAry_at( wk_sock_ary, cnt_idx );
			if( ZnkFdSet_isset( fdst_write, connecting_sock ) ){
				/* OK. */
				MoaiConnection mcn = MoaiConnection_find_byOSock( connecting_sock );
				ZnkSocketAry_set( mfds->connecting_socks_, cnt_idx, ZnkSocket_INVALID );
				if( mcn ){
					MoaiConnection_setConnectInprogress( mcn, false );
					RanoLog_printf( "  Connection Inprogress Completed : sock=[%d]\n", connecting_sock );
					MoaiConnection_invokeCallback( mcn, mfds );
				} else {
					/***
					 * 通常起こりえないがなんらかの理由でMoaiConnectionにconnecting_sockを登録しそびれている.
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
				if( connecting_sock == ZnkSocket_INVALID ){
					ZnkSocketAry_erase_byIdx( mfds->connecting_socks_, cnt_idx );
				}
			}
		}
	}

	{
		/***
		 * この時点でfdst_readに格納されているのは、既にクライアントとの
		 * 接続済のSocketのみであるはずである.
		 * ここではこれを取り出し、recv/send処理などを行う.
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
	 * 最後に reserve_accept_socks_ および reserve_connect_socks_ を消化できないかを試す.
	 * (fnca_ras->func_の呼び出しでfdst_observe_rに空きが生じた可能性がある)
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
