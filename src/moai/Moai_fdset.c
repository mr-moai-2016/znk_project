#include "Moai_fdset.h"
#include "Moai_log.h"
#include <Znk_fdset.h>
#include <Znk_socket_dary.h>
#include <assert.h>

struct MoaiFdSetImpl {
	ZnkSocket         listen_sock_;
	ZnkFdSet          fdst_read_;
	ZnkFdSet          fdst_observe_;
	ZnkSocketDAry     reserve_accept_socks_;
	ZnkSocketDAry     reserve_connect_socks_;
	ZnkSocketDAry     wk_sock_dary_;
	struct ZnkTimeval waitval_;
};

MoaiFdSet
MoaiFdSet_create( ZnkSocket listen_sock, struct ZnkTimeval* waitval )
{
	MoaiFdSet mfds = Znk_malloc( sizeof( struct MoaiFdSetImpl ) );
	mfds->listen_sock_  = listen_sock;
	mfds->fdst_read_    = ZnkFdSet_create();

	mfds->fdst_observe_ = ZnkFdSet_create();
	ZnkFdSet_zero( mfds->fdst_observe_ );
	/* リスニングソケットを監視対象に追加 */
	ZnkFdSet_set( mfds->fdst_observe_, listen_sock );

	mfds->reserve_accept_socks_  = ZnkSocketDAry_create();
	mfds->reserve_connect_socks_ = ZnkSocketDAry_create();
	mfds->wk_sock_dary_          = ZnkSocketDAry_create();

	mfds->waitval_ = *waitval;
	return mfds;
}
void
MoaiFdSet_destroy( MoaiFdSet mfds )
{
	if( mfds ){
		ZnkFdSet_destroy( mfds->fdst_read_ );
		ZnkFdSet_destroy( mfds->fdst_observe_ );
		ZnkSocketDAry_destroy( mfds->reserve_accept_socks_ );
		ZnkSocketDAry_destroy( mfds->reserve_connect_socks_ );
		ZnkSocketDAry_destroy( mfds->wk_sock_dary_ );
		Znk_free( mfds );
	}
}

ZnkFdSet
MoaiFdSet_fdst_observe( MoaiFdSet mfds )
{
	return mfds->fdst_observe_;
}
ZnkSocketDAry
MoaiFdSet_wk_sock_dary( MoaiFdSet mfds )
{
	return mfds->wk_sock_dary_;
}
void
MoaiFdSet_reserveConnectSock( MoaiFdSet mfds, ZnkSocket sock )
{
	size_t idx = ZnkSocketDAry_find( mfds->reserve_connect_socks_, sock );
	if( idx == Znk_NPOS ){
		ZnkSocketDAry_push_bk( mfds->reserve_connect_socks_, sock );
	}
}

int
MoaiFdSet_select( MoaiFdSet mfds, bool* req_before_report, MoaiFdSetFuncArg_Report* fnca_report )
{
	ZnkFdSet fdst_read    = mfds->fdst_read_;
	ZnkFdSet fdst_observe = mfds->fdst_observe_;
	ZnkSocket maxfd;
	int sel_ret;

	/* fds_observe を fds_read にコピー */
	ZnkFdSet_copy( fdst_read, fdst_observe );
	
	maxfd = ZnkFdSet_getMaxOfSocket( fdst_read );

	/***
	 * 第1引数:
	 *   Unixではselectの第1引数はfdst_observeに指定している全sockのうちの
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
		//ZnkSocketDAry_clear( sock_dary );
		//ZnkFdSet_getSocketDAry( fdst_read, sock_dary );
		*req_before_report = false;

		//MoaiIO_reportFdst( "Moai : Observe ", mfds->wk_sock_dary_, mfds->fdst_observe_, true );
		if( fnca_report ){
			fnca_report->func_( mfds, fnca_report->arg_ );
		}
	}
	sel_ret = ZnkFdSet_select( maxfd+1, fdst_read, NULL, NULL, &mfds->waitval_ );
	/***
	 * 将来ここにafter_report系の関数をサポートするかもしれない.
	 */
	return sel_ret;
}

static void
adoptReserveSocks( ZnkFdSet fdst_observe, ZnkSocketDAry reserve_socks, ZnkSocketDAry wk_sock_dary )
{
	size_t idx;
	size_t size;
	ZnkSocket sock;
	bool updated = false;
	size = ZnkSocketDAry_size( reserve_socks );
	for( idx=0; idx<size; ++idx ){ 
		sock = ZnkSocketDAry_at( reserve_socks, idx );
		if( ZnkFdSet_set( fdst_observe, sock ) ){
			/* marking erase */
			updated = true;
			MoaiLog_printf( "  Moai : adoptReserveSocks [%d] under fdst_observe.\n", sock );
			ZnkSocketDAry_set( reserve_socks, idx, ZnkSocket_INVALID );
		}
	}
	/* compaction */
	if( updated ){
		ZnkSocketDAry_clear( wk_sock_dary );
		for( idx=0; idx<size; ++idx ){ 
			sock = ZnkSocketDAry_at( reserve_socks, idx );
			if( sock != ZnkSocket_INVALID ){
				ZnkSocketDAry_push_bk( wk_sock_dary, sock );
			}
		}
		ZnkSocketDAry_swap( wk_sock_dary, reserve_socks );
	}
}

/***
 * ZnkSocketDAryを使うバージョン.
 * Windows/Unixの双方で使用可能である.
 */
void
MoaiFdSet_process( MoaiFdSet mfds,
		MoaiFdSetFuncArg_OnAccept* fnca_on_accept, MoaiFdSetFuncArg_RAS* fnca_ras )
{
	ZnkFdSet fdst_read    = mfds->fdst_read_;
	ZnkFdSet fdst_observe = mfds->fdst_observe_;
	ZnkSocketDAry wk_sock_dary = mfds->wk_sock_dary_;

	if( ZnkFdSet_isset( fdst_read, mfds->listen_sock_ ) ){
		/***
		 * fdst_readにListenSocketが残っていた場合は、
		 * ListenSocketに新規接続要求が発生したことを示す.
		 * この場合、まずこれを優先的に処理する.
		 */
		ZnkSocket new_accept_sock = ZnkSocket_accept( mfds->listen_sock_ );

		if( fnca_on_accept ){
			fnca_on_accept->func_( mfds, new_accept_sock, fnca_on_accept->arg_ );
		}

		/***
		 * 監視ソケット集合へ追加.
		 */
		if( !ZnkFdSet_set( fdst_observe, new_accept_sock ) ){
			/***
			 * 失敗した場合はreserve_accept_socks_へストックしておく.
			 */
			MoaiLog_printf( "  ZnkFdSet_set failure. sock(by accept)=[%d]\n", new_accept_sock );
			ZnkSocketDAry_push_bk( mfds->reserve_accept_socks_, new_accept_sock ); 
		}
		/***
		 * listen_sockに関する処理は終ったので、
		 * fdst_readからこれは除去.
		 */
		ZnkFdSet_clr( fdst_read, mfds->listen_sock_ );
	}

	{
		/***
		 * この時点でfdst_readに格納されているのは、既にクライアントとの
		 * 接続済のSocketのみであるはずである.
		 * ここではこれを取り出し、recv/send処理などを行う.
		 */
		size_t i;
		size_t sock_dary_size;
		const size_t fd_setsize = ZnkFdSet_FD_SETSIZE();

		ZnkSocketDAry_clear( wk_sock_dary );
		ZnkFdSet_getSocketDAry( fdst_read, wk_sock_dary );
		sock_dary_size = ZnkSocketDAry_size( wk_sock_dary );

		Znk_UNUSED( fd_setsize );
		assert( sock_dary_size <= fd_setsize );

		for( i=0; i<sock_dary_size; ++i ){
			ZnkSocket  sock = ZnkSocketDAry_at( wk_sock_dary, i );
			if( fnca_ras ){
				fnca_ras->func_( mfds, sock, fnca_ras->arg_ );
			}
		}
	}

	/***
	 * 最後に reserve_accept_socks_ および reserve_connect_socks_ を消化できないかを試す.
	 * (fnca_ras->func_の呼び出しでfdst_observeに空きが生じた可能性がある)
	 */
	adoptReserveSocks( mfds->fdst_observe_, mfds->reserve_accept_socks_, mfds->wk_sock_dary_ );
	adoptReserveSocks( mfds->fdst_observe_, mfds->reserve_connect_socks_, mfds->wk_sock_dary_ );
}
