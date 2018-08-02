#include "Moai_server.h"
#include "Moai_post.h"
#include "Moai_context.h"
#include "Moai_io_base.h"
#include "Moai_connection.h"
#include "Moai_info.h"
#include "Moai_fdset.h"
#include "Moai_http.h"
#include "Moai_server_info.h"
#include "Moai_web_server.h"
#include "Moai_cgi.h"

#include <Rano_module_ary.h>
#include <Rano_log.h>
#include <Rano_parent_proxy.h>
#include <Rano_myf.h>
#include <Rano_cgi_util.h>

#include <Znk_server.h>
#include <Znk_socket_ary.h>
#include <Znk_fdset.h>
#include <Znk_err.h>
#include <Znk_net_base.h>
#include <Znk_socket.h>
#include <Znk_sys_errno.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <Znk_str_ary.h>
#include <Znk_str_fio.h>
#include <Znk_str_ptn.h>
#include <Znk_zlib.h>
#include <Znk_def_util.h>
#include <Znk_htp_hdrs.h>
#include <Znk_dlink.h>
#include <Znk_cookie.h>
#include <Znk_net_ip.h>
#include <Znk_dir.h>
#include <Znk_envvar.h>
#include <Znk_thread.h>

#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>


static ZnkStrAry   st_access_allow_ips = NULL;
static ZnkStrAry   st_access_deny_ips  = NULL;

static void
debugStrAry( ZnkStrAry ary, const char* name )
{
	size_t idx;
	size_t size = ZnkStrAry_size(ary);
	RanoLog_printf( "%s={", name );
	for(idx=0;idx<size;++idx){
		RanoLog_printf( "[%s]", ZnkStrAry_at_cstr( ary, idx ) );
	}
	RanoLog_printf( "}\n" );
}


typedef enum {
	 HtpScan_e_None=0
	,HtpScan_e_Request
	,HtpScan_e_Response
	,HtpScan_e_RecvZero
	,HtpScan_e_RecvError
	,HtpScan_e_NotHttpStart
	,HtpScan_e_UnknownMethod
	,HtpScan_e_CannotGetHost
	,HtpScan_e_HostBloking
} HtpScanType;


static void
sendHdrs( ZnkSocket sock, ZnkStrAry hdr1st, const ZnkVarpAry vars )
{
	ZnkBfr bfr = ZnkBfr_create_null();
	ZnkHtpHdrs_extendToStream( hdr1st, vars, bfr, true );
	ZnkSocket_send( sock, ZnkBfr_data(bfr), ZnkBfr_size(bfr) );
}

static void
safeDumpBuf( uint8_t* buf, size_t size )
{
	size_t idx;
	RanoLog_printf( "  safeDumpBuf [" );
	for( idx=0; idx<size; ++idx ){
		if( buf[idx] >= 0x20 && buf[idx] < 0x7f ){
			RanoLog_printf( "%c", buf[idx] );
		} else {
			RanoLog_printf( "%02x", buf[idx] );
		}
	}
	RanoLog_printf( "]\n" );
}

static void
debugInterestGoal( const MoaiInfo* info, const MoaiConnection mcn, HtpScanType scan_type, const char* label )
{
	ZnkMyf    analysis       = RanoMyf_theAnalysis();
	ZnkStrAry interest_hosts = ZnkMyf_find_lines( analysis, "interest_hosts" );
	/* analysis.myfは存在しない場合もあるため、NULLチェックが必要 */
	if( interest_hosts ){
		const char* hostname       = ZnkStr_cstr( mcn->hostname_ );
		const char* req_urp        = ZnkStr_cstr( info->req_urp_ );
		ZnkStrAry   interest_urp   = ZnkMyf_find_lines( analysis, "interest_urp" );
		RanoLog_printf( "  Moai Interest HTTP Hdr Report : hostname=[%s] req_urp=[%s] (%s)\n", hostname, req_urp, label );
		if( ZnkStrAry_find_isMatch( interest_hosts, 0, hostname, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
			if( ZnkStrAry_find_isMatch( interest_urp, 0, req_urp, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
				if( ZnkStrAry_size(info->hdrs_.hdr1st_) > 1 ){
					//const char* req_method_str = ZnkHtpReqMethod_getCStr( info->req_method_ );
					RanoLog_printf( "  Moai Interest HTTP Hdr Report : URL=[%s%s] (%s)\n", hostname, req_urp, label );
#if 0
					if( scan_type == HtpScan_e_Request ){
						RanoLog_printf( "  Request %s\n",
								req_method_str );
					} else {
						RanoLog_printf( "  Response %s %s%s\n",
								req_method_str, ZnkStrAry_at_cstr(info->hdrs_.hdr1st_,1) );
					}
#endif
				}
				MoaiHttp_debugHdrVars( info->hdrs_.vars_, "    " );
			}
		}
	}
}

static HtpScanType
scanHttpFirst( MoaiContext ctx, MoaiConnection mcn,
		ZnkSocket sock, MoaiSockType sock_type, ZnkFdSet fdst_observe_r, MoaiFdSet mfds )
{
	RanoHtpType htp_type = RanoHtpType_e_None;
	MoaiInfo* draft_info = ctx->draft_info_;
	char sock_str[ 4096 ];
	bool response_for_req_HEAD = false;
	bool enable_safe_dump = true;

	/***
	 * ここではパイプライン化され、複数存在する場合は最後の要素を返すことになるが、
	 * そもそもパイプライン化はHEAD/GETなどが組み合わさっているような場合のみに起こる.
	 * POST/CONNECTについては、「これらをパイプライン化してはならない」というHTTP上の規則があるため、
	 * その可能性を心配しなくてもよい.
	 * そして以下ではlast_invoked_infoをCONNECTの判定のみに使用する.
	 */
	MoaiInfo*       last_invoked_info = MoaiConnection_getInvokedInfo( mcn, Znk_NPOS );
	ZnkHtpReqMethod last_req_method   = last_invoked_info ? last_invoked_info->req_method_ : ZnkHtpReqMethod_e_Unknown;

	ctx->buf_[0] = '\0';
	ctx->result_size_ = 0;
	ctx->recv_executed_ = false;
	ctx->req_method_ = ZnkHtpReqMethod_e_Unknown;
	ZnkSocket_getPeerIPandPort( sock, &ctx->peer_ipaddr_, NULL );

	MoaiBodyInfo_clear( &ctx->body_info_  );
	ZnkStr_clear( ctx->msgs_ );
	ZnkStr_clear( ctx->text_ );
	MoaiInfo_clear( draft_info );

	RanoLog_printf( "Moai : scanHttpFirst[%s] %s\n",
			sock_type == MoaiSockType_e_Inner ? "Inner" : sock_type == MoaiSockType_e_Outer ? "Outer" : "None",
			MoaiIO_makeSockStr( sock_str, sizeof(sock_str), sock, true ) );
	{
		size_t arg_pos[ 3 ];
		size_t arg_leng[ 3 ];
		char   arg_tkns[ 3 ][ 4096 ] = { "", "", "" };
		int http_version = 0;
		bool check_Host = false;

		ctx->result_size_ = ZnkSocket_recv( sock, ctx->buf_, sizeof(ctx->buf_) );
		if( ctx->result_size_ <= 0 ){
			MoaiIO_addAnalyzeLabel( ctx->msgs_, sock, ctx->result_size_, "scanHttpFirst:first_recv" );
			/***
			 * 特にCONNECTの場合、RecvZeroを検出したのがI側であれ、O側であれ、
			 * CONNECT通信の終了を意味し、その相手側も閉じなければならないことに注意する.
			 */
			RanoLog_printf( "Moai : MoaiConnection_erase (first_recv)\n" );
			MoaiConnection_erase( mcn, mfds );
			if( ctx->result_size_ == 0 ){
				ZnkStr_add( ctx->msgs_, "RecvZero.\n" );
			} else {
				char errmsg_buf[ 4096 ];
				int  err_code = ZnkNetBase_getLastErrCode();
				ZnkNetBase_getErrMsg( errmsg_buf, sizeof(errmsg_buf), err_code );
				ZnkStr_add( ctx->msgs_, "RecvError.\n" );
				ZnkStr_addf( ctx->msgs_, "  errmsg=[%s]\n", errmsg_buf );
			}
			RanoLog_printf( "%s", ZnkStr_cstr( ctx->msgs_ ) );
			return ctx->result_size_ == 0 ? HtpScan_e_RecvZero : HtpScan_e_RecvError;
		}

		if( enable_safe_dump ){
			safeDumpBuf( ctx->buf_, 16 );
		}

		ZnkBfr_append_dfr( draft_info->stream_, ctx->buf_, ctx->result_size_ );
		ctx->recv_executed_ = true;

		if( last_req_method == ZnkHtpReqMethod_e_CONNECT ){
			/***
			 * これはCONNECTによるUnknownな通信である.
			 * この場合ただちにNotHttpStartとしてよい.
			 */
			ZnkStr_addf( ctx->msgs_, "  NotHttpStart (CONNECT tunneling : result_size=[%d]).\n", ctx->result_size_ );
			RanoLog_printf( "%s", ZnkStr_cstr( ctx->msgs_ ) );
			return HtpScan_e_NotHttpStart;
		}

		/***
		 * 通常のHttpによる通信であるはず.
		 * この場合、content_length_remain_が非zeroであるなら、
		 * Httpの開始ではない.
		 *
		 * TODO:
		 * 一つのHttpの終了のタイミングはRecvZeroに頼るわけにはいかない.
		 * Httpにおいてはこれが送られてくる保障はなく、それどころか
		 * 同じソケットを次の全く異なるサーバへの新しいHttp接続を何の予告もなしに
		 * 行ってくることすらある(SrcRecycle).
		 * よって、その終了のタイミングはここで完全に掌握しておく必要がある.
		 * その判定は、Content-LengthとTrasfer-Encodingの指定に頼る他ない.
		 * Content-Lengthが指定されていないあるいは0であるにも関わらず
		 * Content bodyが存在する場合はTrasfer-Encodingにchunkedが指定されているケースである.
		 * これらをまとめて処理するため、Content-Lengthが指定されている場合もchunk_size_として扱う.
		 */
		if( sock_type == MoaiSockType_e_Inner ){
			/* Request */
			if( mcn->req_content_length_remain_ ){
				if( mcn->req_content_length_remain_ >= (size_t)ctx->result_size_ ){
					mcn->req_content_length_remain_ -= ctx->result_size_;
					ZnkStr_addf( ctx->msgs_, "  NotHttpStart Inner (remain=[%zu] result_size=[%d]).\n",
							mcn->req_content_length_remain_, ctx->result_size_ );
					RanoLog_printf( "%s", ZnkStr_cstr( ctx->msgs_ ) );
					return HtpScan_e_NotHttpStart;
				} else {
					/***
					 * この場合、req_content_length_remain_の値の方がもはや信用ならない.
					 * 旧通信において途中致命的な中断が発生し、その名残が残っている状況と思われる.
					 * この場合はreq_content_length_remain_の値を一旦 0 に戻し、first_lineの取得を
					 * 試みるべきである.
					 */
					/* for debugger print */
					const size_t extra_size = ctx->result_size_ - mcn->req_content_length_remain_;
					ZnkStr_addf( ctx->msgs_, "  Invalid req_content_length_remain : (remain=[%zu] result_size=[%d] extra_size=[%zu]).\n",
							mcn->req_content_length_remain_, ctx->result_size_, extra_size );
					RanoLog_printf( "%s", ZnkStr_cstr( ctx->msgs_ ) );
					mcn->req_content_length_remain_ = 0;
				}
			}
		} else if( sock_type == MoaiSockType_e_Outer ){
			/* Response */
			if( mcn->res_content_length_remain_ ){
				if( mcn->res_content_length_remain_ >= (size_t)ctx->result_size_ ){
					mcn->res_content_length_remain_ -= ctx->result_size_;
					ZnkStr_addf( ctx->msgs_, "  NotHttpStart Outer (remain=[%zu] result_size=[%d]).\n",
							mcn->res_content_length_remain_, ctx->result_size_ );
					RanoLog_printf( "%s", ZnkStr_cstr( ctx->msgs_ ) );
					return HtpScan_e_NotHttpStart;
				} else {
					/***
					 * この場合、res_content_length_remain_の値の方がもはや信用ならない.
					 * 旧通信において途中致命的な中断が発生し、その名残が残っている状況と思われる.
					 * この場合はres_content_length_remain_の値を一旦 0 に戻し、first_lineの取得を
					 * 試みるべきである.
					 */
					/* for debugger print */
					const size_t extra_size = ctx->result_size_ - mcn->res_content_length_remain_;
					ZnkStr_addf( ctx->msgs_, "  Invalid res_content_length_remain : (remain=[%zu] result_size=[%d] extra_size=[%zu]).\n",
							mcn->res_content_length_remain_, ctx->result_size_, extra_size );
					RanoLog_printf( "%s", ZnkStr_cstr( ctx->msgs_ ) );
					mcn->res_content_length_remain_ = 0;
				}
			}
		}

		/***
		 * buf内に行の終了パターンが含まれるかどうかを調べ、
		 * 最初の行を全取得するまでrecvを繰り返す.
		 */
		RanoLog_printf( "  Try to recv HTTP First.\n" );
		if( !MoaiIO_recvInPtn( draft_info->stream_, sock, mfds, "\r\n", NULL ) ){
			RanoLog_printf( "  Error : Fail to recv HTTP First.\n" );
			return HtpScan_e_RecvError;
		}

#define GET_ARG_POS( idx ) \
		arg_pos[ idx ] = ZnkS_lfind_arg( (char*)ZnkBfr_data(draft_info->stream_), 0, ZnkBfr_size(draft_info->stream_), \
				idx, arg_leng+idx, " \t\r\n", 4 ); \

#define COPY_ARG_TKNS( idx ) \
		ZnkS_copy( arg_tkns[ idx ], sizeof(arg_tkns[ idx ]), (char*)ZnkBfr_data(draft_info->stream_)+arg_pos[ idx ], arg_leng[ idx ] )

		GET_ARG_POS( 0 );
		COPY_ARG_TKNS( 0 );
		GET_ARG_POS( 1 );
		COPY_ARG_TKNS( 1 );
		GET_ARG_POS( 2 );
		{
			char* p = (char*)ZnkBfr_data(draft_info->stream_)+arg_pos[ 2 ];
			char* q = strchr( p, '\r' );
			ZnkS_copy( arg_tkns[ 2 ], sizeof(arg_tkns[ 2 ]), p, (size_t)(q-p) );
		}

#undef GET_ARG_POS
#undef COPY_ARG_TKNS

		/***
		 * TODO:
		 * ここでは、最初の一回のrecv で最低限 first line 全体の取得は成功したことを
		 * 前提としている. 仮にresult_sizeが極めて小さく、これの取得にも失敗した場合、
		 * Httpの開始であるにも関わらず以下の比較に失敗するシナリオも考えられる.
		 * 従って、この最初の一回のrecvは、厳格には一つの行が読み込まれるまで繰り返すべきである.
		 */
		if( ZnkS_eqCase( arg_tkns[ 2 ], "HTTP/1.1" ) ){
			/* HTTP request */
			http_version = 11;
			htp_type = RanoHtpType_e_Request;
		} else if( ZnkS_eqCase( arg_tkns[ 2 ], "HTTP/1.0" ) ){
			/* HTTP request */
			http_version = 10;
			htp_type = RanoHtpType_e_Request;
		} else if( ZnkS_eqCase( arg_tkns[ 0 ], "HTTP/1.1" ) ){
			/* HTTP response */
			http_version = 11;
			htp_type = RanoHtpType_e_Response;
		} else if( ZnkS_eqCase( arg_tkns[ 0 ], "HTTP/1.0" ) ){
			/* HTTP response */
			http_version = 10;
			htp_type = RanoHtpType_e_Response;
		} else {
			MoaiIO_addAnalyzeLabel( ctx->msgs_, sock, ctx->result_size_, "scanHttpFirst:NotHttpStart" );
			ZnkStr_add( ctx->msgs_, "NotHttpStart.\n" );
			RanoLog_printf( "%s", ZnkStr_cstr( ctx->msgs_ ) );
			htp_type = RanoHtpType_e_NotHttpStart;
			return HtpScan_e_NotHttpStart;
		}

		/* とりあえず現時点では未使用 */
		Znk_UNUSED( http_version );

		/***
		 * この時点でHttpの開始らしきパターンは検出されたことになるが、
		 * ここでcontent_length_remain_の値を参照し、これが非zeroである場合には、
		 * このデータをHttp Bodyの途中とみなし、NotHttpStartとすべきである.
		 * 例えば、Httpヘッダそのものを解説しているようなサイトがあったとして、
		 * content body内にヘッダの開始部のような文字列が記載されているようなケースが考えられる.
		 * もしもrecvの切れ目が調度そのような部分にあたるような場所でたまたま区切れていた場合
		 * その部分でHttpの開始と判断するのは誤りである.
		 * そんなことが起こる確率は極めて低いが、可能性がある限りこれに対処しなければならない.
		 *
		 * 一方、CONNECTなどでHttp以外の通信が行われている場合は、もはやその内容に関しての
		 * 判断の手段がない. この場合、content_length_remain_は常に 0 とするものと定め、
		 * この種の判定は行わない(RecvZeroで適切にRARが区切られることを期待するしかない).
		 */

		/***
		 * content_length_remain_ == 0ならばHttpの開始として確定させる.
		 * buf内にヘッダの終了パターンが含まれるかどうかを調べ、
		 * ヘッダ部を全取得するまでrecvを繰り返す.
		 */
		RanoLog_printf( "  Try to recv HTTP Hdrs.\n" );
		if( !MoaiIO_recvInPtn( draft_info->stream_, sock, mfds, "\r\n\r\n", &draft_info->hdr_size_ ) ){
			RanoLog_printf( "  Hdr Recv Error.\n" );
			return HtpScan_e_RecvError;
		}
		RanoLog_printf( "  recv Hdrs OK.\n" );

		/***
		 * localhostへのloopback接続をなんとしても検出し、
		 * 接続の無限ループを避けねばならない.
		 * 考えられるシナリオは以下である.
		 * moaiが開いているport番号をPとする.
		 *
		 * Browserから localhost:Pへ明示的にGET接続
		 * このときPが80でなければBrowserはproxyを介さず直接moaiに接続される.
		 * moaiはwebserverとしてこれを処理しなければならないのに
		 * 誤ってproxyとしてこれを中継処理.
		 * このとき中継先はまたlocalhost:Pとなる.
		 *
		 * Browserから localhost:Pへ明示的にGET接続
		 * このときPが80であればまずmoaiがproxyとしてこれを中継し、localhost:Pへ接続される.
		 * moaiはwebserverとしてこれを処理しなければならないのに
		 * 誤ってproxyとしてこれを中継処理.
		 * このとき中継先はまたlocalhost:Pとなる.
		 */

		ZnkHtpHdrs_clear( &draft_info->hdrs_ );
		{
			const uint16_t moai_port    = MoaiServerInfo_port();
			const RanoModuleAry mod_ary = MoaiServerInfo_mod_ary();
			const char*    server_name  = MoaiServerInfo_server_name();
			MoaiInfo_parseHdr( draft_info, &ctx->body_info_,
					&ctx->as_local_proxy_, moai_port, (bool)(htp_type == RanoHtpType_e_Request),
					ZnkStr_cstr(mcn->hostname_), mod_ary, server_name );
		}

		switch( htp_type ){
		case RanoHtpType_e_Response:
		{
			MoaiBodyInfo* body_info      = &ctx->body_info_;
			const char*   req_method_str = "Unknown";
			const char*   req_urp        = "";
			const size_t  response_idx   = MoaiConnection_countResponseIdx( mcn );
			MoaiInfo* info_on_response   = MoaiConnection_getInvokedInfo( mcn, response_idx );
			if( info_on_response ){
				req_method_str = ZnkHtpReqMethod_getCStr( info_on_response->req_method_ );
				req_urp        = ZnkStr_cstr( info_on_response->req_urp_ );
				draft_info->req_method_ = info_on_response->req_method_;
				ZnkStr_set( draft_info->req_urp_, req_urp );
			}

			RanoLog_printf( "  Http %s Response %s %s URL=[%s%s]\n",
					req_method_str, arg_tkns[ 1 ], arg_tkns[ 2 ],
					MoaiConnection_hostname(mcn), req_urp );
			debugInterestGoal( draft_info, mcn, htp_type, "Original HTTP Header" );

			if( info_on_response ){
				switch( info_on_response->req_method_ ){
				case ZnkHtpReqMethod_e_GET:
				case ZnkHtpReqMethod_e_POST:
				case ZnkHtpReqMethod_e_HEAD:
				{
					ZnkS_getIntD( &info_on_response->response_state_, arg_tkns[ 1 ] );
					if( info_on_response->response_state_ == 200 ){
						RanoLog_printf( "  body_info : is_chunked=[%s] is_gzip=[%s] txt_type=[%s]\n",
								body_info->is_chunked_ ? "true" : "false",
								body_info->is_gzip_    ? "true" : "false",
								RanoTextType_getCStr( body_info->txt_type_ ) );
					} else {
						if( body_info->is_unlimited_ ){
							body_info->is_unlimited_ = false;
						}
					}
					break;
				}
				default:
					break;
				}
				response_for_req_HEAD = (bool)( info_on_response->req_method_ == ZnkHtpReqMethod_e_HEAD );
			}
			break;
		}
		case RanoHtpType_e_Request:
		{
			char hostname_buf[ 4096 ] = "";
			uint16_t port = 0;

			if( ZnkS_isCaseBegin( arg_tkns[ 1 ], "http://" ) || ZnkS_isCaseBegin( arg_tkns[ 1 ], "//" ) ){
				/* Request-URIにおいてホスト名が記載されているとみなす.
				 * 最初のhttp:// や // は除去してもよかろう.
				 * さらに外部のproxyへ渡す場合、このままでよいが、
				 * そうではない場合、urpの形に修正しなければならない.
				 */
				ZnkS_copy( hostname_buf, sizeof(hostname_buf), arg_tkns[ 1 ]+7, arg_leng[ 1 ]-7 );
			} else if( ZnkS_isCaseBegin( arg_tkns[ 1 ], "/" ) ){
				check_Host = true;
			} else if( ZnkS_isCaseBegin( arg_tkns[ 1 ], "*" ) ){
				check_Host = true;
			} else {
				/* Request-URIにおいてホスト名が記載されているとみなす */
				ZnkS_copy( hostname_buf, sizeof(hostname_buf), arg_tkns[ 1 ], arg_leng[ 1 ] );
			}

			/***
			 * 念のため、第1番目のtknも確認.
			 * 以下のいずれでもない場合はエラーとみなす.
			 */
			if( ZnkS_eqCase(        arg_tkns[ 0 ], "GET" ) ){
				ctx->req_method_ =   ZnkHtpReqMethod_e_GET;
			} else if( ZnkS_eqCase( arg_tkns[ 0 ], "POST" ) ){
				ctx->req_method_ =   ZnkHtpReqMethod_e_POST;
			} else if( ZnkS_eqCase( arg_tkns[ 0 ], "HEAD" ) ){
				ctx->req_method_ =   ZnkHtpReqMethod_e_HEAD;
			} else if( ZnkS_eqCase( arg_tkns[ 0 ], "PUT" ) ){
				ctx->req_method_ =   ZnkHtpReqMethod_e_PUT;
			} else if( ZnkS_eqCase( arg_tkns[ 0 ], "DELETE" ) ){
				ctx->req_method_ =   ZnkHtpReqMethod_e_DELETE;
			} else if( ZnkS_eqCase( arg_tkns[ 0 ], "CONNECT" ) ){
				ctx->req_method_ =   ZnkHtpReqMethod_e_CONNECT;
			} else if( ZnkS_eqCase( arg_tkns[ 0 ], "OPTIONS" ) ){
				ctx->req_method_ =   ZnkHtpReqMethod_e_OPTIONS;
			} else if( ZnkS_eqCase( arg_tkns[ 0 ], "TRACE" ) ){
				ctx->req_method_ =   ZnkHtpReqMethod_e_TRACE;
			} else {
				/* not support */
				MoaiIO_addAnalyzeLabel( ctx->msgs_, sock, ctx->result_size_, "scanHttpFirst:Request" );
				ZnkStr_add( ctx->msgs_, "UnknownMethod.\n" );
				RanoLog_printf( "%s", ZnkStr_cstr( ctx->msgs_ ) );
				MoaiIO_close_ISock( "  UnknownMethod", sock, mfds );
				return HtpScan_e_UnknownMethod;
			}

			{
				const char* method = arg_tkns[ 0 ];
				RanoLog_printf( "  Http %s Request [%s].\n", method, arg_tkns[ 1 ] );
				debugInterestGoal( draft_info, mcn, htp_type, "Original HTTP Header" );
			}

			if( check_Host ){
				const uint8_t* hdr_data = ZnkBfr_data( draft_info->stream_ );
				const size_t   hdr_size = draft_info->hdr_size_;
				if( !MoaiHttp_scanHeaderVar( "Host", hdr_data, hdr_size, hostname_buf, sizeof(hostname_buf) ) ){
					MoaiIO_close_ISock( "  CannotGetHost", sock, mfds );
					return HtpScan_e_CannotGetHost;
				}
			}
			if( !ZnkS_empty(hostname_buf) ){
				char* colon;
				char* slash;
				colon = strchr( hostname_buf, ':' );
				slash = strchr( hostname_buf, '/' );
				if( colon ){
					if( slash && slash < colon ){
						/***
						 * オーソリティ部より後ろにはURLエンコーディングなどにより
						 * : が含まれる可能性がある. もしcolonがslashより後ろのときは
						 * このケースであると考えられるため、このcolonは無視する必要がある.
						 */
						port = 80;
					} else {
						/***
						 * slashがないかまたはcolonがslashよりも前である場合は
						 * hostnameとport番号を区切る : である.
						 */
						*colon = '\0';
						sscanf( colon+1, "%hd", &port );
					}
				} else {
					port = 80;
				}
				if( slash ){
					*slash = '\0';
				}
			}
			if( !ZnkStr_eq( mcn->hostname_, hostname_buf ) ||  mcn->port_ != port ){
				/***
				 * この場合、mcn->O_sock_との旧RASは完了した後、ブラウザはmcn->I_sock_の接続を
				 * そのまま維持しつつリサイクルして、新たなるHTTPで接続していると思われる.
				 * しかし、新たな接続先ホストが以前のそれと同じとは限らない.  
				 * それをチェックするため、上の比較を行い、ホスト名またはportが以前のそれとは
				 * 異なる場合、旧mcn->O_sock_をcloseし、新たな接続を作ってこれを新しいmcn->O_sock_とする.
				 *
				 * このとき、mcn->I_sock_はリサイクルされ、mcn->O_sock_のみが作りなおされることになる.
				 * ブラウザによってはこのようなことが有り得る.
				 * 例えばFirefoxでwww.google.co.jpに接続し、直後に別のサイトへ接続する場合などである.
				 */
				MoaiIO_close_OSock( "  SrcRecycle", mcn->O_sock_, mfds );
			}
			/* 更新 */
			ZnkStr_set( mcn->hostname_, hostname_buf );
			mcn->port_ = port;
			break;
		}
		default:
			assert( 0 );
			break;
		}
	}

	/***
	 * content_length_remain_の初期化.
	 */
	if( ctx->req_method_ == ZnkHtpReqMethod_e_HEAD ){
		mcn->req_content_length_remain_ = 0;
		RanoLog_printf( "Moai : mcn->req_content_length_remain_=0 (ReqMethod HEAD)\n" );
	} else if( sock_type == MoaiSockType_e_Inner ){
		mcn->req_content_length_remain_ = ctx->body_info_.content_length_;
		RanoLog_printf( "Moai : mcn->req_content_length_remain_=[%zu] (body content_length Inner)\n", mcn->req_content_length_remain_ );
	} else if( sock_type == MoaiSockType_e_Outer ){
		mcn->res_content_length_remain_ = ctx->body_info_.content_length_;
		RanoLog_printf( "Moai : mcn->req_content_length_remain_=[%zu] (body content_length Outer)\n", mcn->req_content_length_remain_ );
	}

	/***
	 * Connection keep-aliveの有無を確認する.
	 */
	{
		ZnkVarp varp = ZnkHtpHdrs_find( draft_info->hdrs_.vars_, "Connection", Znk_NPOS );
		mcn->I_is_keep_alive_ = true;
		if( varp ){
			const char* val  = ZnkHtpHdrs_val_cstr( varp, 0 );
			mcn->I_is_keep_alive_ = ZnkS_eqCase( val, "keep-alive" );
		}
	}

	if( sock_type == MoaiSockType_e_Inner ){
		if( mcn->req_content_length_remain_ ){
			size_t recved_content_length = ZnkBfr_size( draft_info->stream_ ) - draft_info->hdr_size_;
			if( mcn->req_content_length_remain_ < recved_content_length ){
				mcn->req_content_length_remain_ = 0;
			} else {
				mcn->req_content_length_remain_ -= recved_content_length;
			}
			RanoLog_printf( "Moai : mcn->req_content_length_remain_=[%zu] (recved)\n", mcn->req_content_length_remain_ );
		}
	} else if( sock_type == MoaiSockType_e_Outer ){
		if( mcn->res_content_length_remain_ ){
			size_t recved_content_length = ZnkBfr_size( draft_info->stream_ ) - draft_info->hdr_size_;
			if( mcn->res_content_length_remain_ < recved_content_length ){
				mcn->res_content_length_remain_ = 0;
			} else {
				mcn->res_content_length_remain_ -= recved_content_length;
			}
		}
	}

	if( response_for_req_HEAD ){
		mcn->res_content_length_remain_ = 0;
	}
#if 0
	RanoLog_printf( "  **content_length_remain : req=[%zu] res=[%zu] rfh=[%d]\n",
			mcn->req_content_length_remain_,
			mcn->res_content_length_remain_,
			response_for_req_HEAD );
#endif

	if( htp_type == RanoHtpType_e_Response ){
		MoaiConnection mcn      = MoaiConnection_find_byOSock( sock );
		const char*    hostname = MoaiConnection_hostname( mcn );
		RanoModuleAry  mod_ary  = MoaiServerInfo_mod_ary();
		RanoModule     mod      = RanoModuleAry_find_byHostname( mod_ary, hostname );

		if( mod ){
			ZnkMyf ftr_send = RanoModule_ftrSend( mod );
			RanoFltr_updateCookieFilter_bySetCookie( draft_info->hdrs_.vars_, ftr_send );
			/***
			 * 次回起動時にも状態を引き継ぐため、saveが必要.
			 */
			{
				char        filename[ 256 ];
				const char* target_name = RanoModule_target_name( mod );
				Znk_snprintf( filename, sizeof(filename), "%s/%s_send.myf", MoaiServerInfo_filters_dir(), target_name );
				if( !ZnkMyf_save( ftr_send, filename ) ){
					RanoLog_printf( "  Response : Cannot save %s\n", filename );
				}
			}
		}

		/***
		 * HEADの場合はConnect-Lengthが非ゼロであるにも関わらずそのデータ部がない
		 */
		if( last_req_method == ZnkHtpReqMethod_e_HEAD ){
			if( mod ){
				RanoModule_invokeOnResponse( mod, draft_info->hdrs_.vars_, NULL, "" );
			}
		} else {
			MoaiBodyInfo* body_info = &ctx->body_info_;
		
			/***
			 * Textデータの場合は全取得を試みる.
			 * txt_type => Unknown(実際はBinary) => 全取得を試みる => Freeze? 
			 */
			if( body_info->txt_type_ != RanoText_Binary || body_info->is_unlimited_ ){
				MoaiHttp_processResponse_forText( sock, ctx, mfds, &mcn->res_content_length_remain_, mod );
			} else if( mod ){
				RanoModule_invokeOnResponse( mod, draft_info->hdrs_.vars_, NULL, "" );
			}
		} 
		return HtpScan_e_Response;
	}
	{
		const char* hostname     = MoaiConnection_hostname( mcn );
		uint16_t    port         = MoaiConnection_port( mcn );
		ZnkMyf      hosts        = RanoMyf_theHosts();
		ZnkStrAry   ignore_hosts = ZnkMyf_find_lines( hosts, "ignore_hosts" );

		/***
		 * hostname:port と指定されていた場合は
		 * そのportへの接続のみをブロックする.
		 * hostname と指定されていた場合は
		 * 80のみではなく全portをブロックする.
		 */
		RanoLog_printf( "  New Destination : [%s:%u]\n", hostname, port );
		if( ZnkStrAry_find_isMatch( ignore_hosts, 0, hostname, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
			int ret = MoaiIO_sendTxtf( sock, "text/html", "Moai : Ignored Host Blocking[%s] sock=[%d].\n",
					hostname, sock );
			RanoLog_printf( "  Ignored hostname=[%s]\n", hostname );
			if( ret < 0 || !mcn->I_is_keep_alive_ || ctx->req_method_ == ZnkHtpReqMethod_e_CONNECT ){
				MoaiIO_close_ISock( "  IgnoreBlocking", sock, mfds );
			} else {
				/* 試験的 */
				MoaiIO_close_ISock( "  IgnoreBlocking", sock, mfds );
			}
			return HtpScan_e_HostBloking;
		}
	}

	return HtpScan_e_Request;
}

static int
sendToDst( ZnkSocket dst_sock, MoaiInfo* info, RanoTextType txt_type, ZnkStr text, bool with_header )
{
	int result_size = 0;
	if( with_header ){
		/* HTTPヘッダを送信 */
		sendHdrs( dst_sock, info->hdrs_.hdr1st_, info->hdrs_.vars_ ); 
		if( txt_type != RanoText_Binary ){
			result_size = ZnkSocket_send( dst_sock,
					(uint8_t*)ZnkStr_cstr(text), ZnkStr_leng(text) );
		} else {
			/* データを送信 */
			result_size = ZnkSocket_send( dst_sock,
					ZnkBfr_data(info->stream_)+info->hdr_size_, ZnkBfr_size(info->stream_)-info->hdr_size_ );
		}
	} else {
		/* データを送信 */
		result_size = ZnkSocket_send( dst_sock, ZnkBfr_data(info->stream_), ZnkBfr_size(info->stream_) );
	}
	return result_size;
}

static MoaiRASResult
requestOnConnectCompleted_CONNECT( ZnkSocket O_sock,
		const char* hostname, MoaiInfoID info_id, MoaiFdSet mfds )
{
	ZnkErr_D( err );
	MoaiConnection mcn            = MoaiConnection_find_byOSock( O_sock );
	ZnkSocket      I_sock         = MoaiConnection_I_sock( mcn );
	MoaiInfo*      info           = MoaiInfo_find( info_id );
	ZnkMyf         hosts          = RanoMyf_theHosts();
	const bool     is_proxy_use   = RanoParentProxy_isAppliedHost2( hosts, hostname, Znk_NPOS );
	bool           connection_established = true;

	if( is_proxy_use ){
		/***
		 * この場合はO側(外部proxy)にもCONNECTヘッダを送り、
		 * その応答内容が200であるか否かを確認する必要がある.
		 */
		ZnkBfr bfr = ZnkBfr_create_null();
		ZnkHtpHdrs_extendToStream( info->hdrs_.hdr1st_, info->hdrs_.vars_, bfr, true );
		ZnkSocket_send( O_sock, ZnkBfr_data(bfr), ZnkBfr_size(bfr) );

		MoaiInfo_clear( info );
		ZnkBfr_clear( bfr );
		RanoLog_printf( "  Try to recv CONNECT request return from proxy.\n" );
		if( !MoaiIO_recvInPtn( bfr, O_sock, mfds, "\r\n\r\n", &info->hdr_size_ ) ){
			RanoLog_printf( "  Hdr Recv Error.\n" );
			ZnkBfr_destroy( bfr );
			return MoaiRASResult_e_Ignored;
		}

		/***
		 * 外部proxyからの応答内容を検証.
		 * statusが200ならCONNECT可とする.
		 */
		connection_established = false;
		{
			const char* p = (char*)ZnkBfr_data(bfr);
			const char* q = strstr( p, "\r\n" );
			if( q ){
				ZnkHtpHdrs_registHdr1st( info->hdrs_.hdr1st_, p, q - p );
				RanoLog_printf( "Recving status [%s]\n", ZnkStrAry_at_cstr( info->hdrs_.hdr1st_, 1 ) );
				if( ZnkS_eq( ZnkStrAry_at_cstr( info->hdrs_.hdr1st_, 1 ), "200" ) ){
					connection_established = true;
				}
			}
		}
		ZnkBfr_destroy( bfr );
	}
	if( connection_established ){
		/**
		 * まだO側に何も送らない.
		 * I側へ単に HTTP/1.0 200 connection established を返し、改めてI側からのrecvをselectにより待つ.
		 * このとき、(Unknownな)通信の開始となるI_sockは、最初にCONNECTメソッドを送ってきたI_sockと
		 * 同じである(そうでなければならない).
		 * よって、そのような通信の開始や途中であるか否かはreq_method_値によって簡単に識別できる.
		 * また、そのような通信の終了は、そのいずれかの側の接続が閉じられたと検出した時である.
		 * その場合、閉じられた側からの残りのデータをすべて送信し、その後両方の接続を閉じ、
		 * 残ったデータがあれば捨てる、という処理を試みなければならないとある.
		 *
		 * RFC7231: CONNECT
		 * A tunnel is closed when a tunnel intermediary detects that either side
		 * has closed its connection: the intermediary MUST attempt to send any
		 * outstanding data that came from the closed side to the other side,
		 * close both connections, and then discard any remaining data left undelivered.
		 */
		const char* msg = "HTTP/1.0 200 Connection established. \r\n\r\n";
		int result_size = ZnkSocket_send( I_sock, (uint8_t*)msg, strlen(msg) );
		if( result_size == -1 ){
			ZnkSysErrnoInfo* err_info = ZnkSysErrno_getInfo( ZnkSysErrno_errno() );
			ZnkErr_internf( &err,
					"  ZnkSocket_send : Failure : SysErr=[%s:%s]",
					err_info->sys_errno_key_, err_info->sys_errno_msg_ );
			MoaiConnection_erase( mcn, mfds );
			return MoaiRASResult_e_Ignored;
		}
		RanoLog_printf( "  CONNECT established I[%d] => O[%d].\n", I_sock, O_sock );
	} else {
		ZnkBfr bfr = ZnkBfr_create_null();
		int result_size;
		ZnkHtpHdrs_extendToStream( info->hdrs_.hdr1st_, info->hdrs_.vars_, bfr, true );
		result_size = ZnkSocket_send( I_sock, ZnkBfr_data(bfr), ZnkBfr_size(bfr) );
		ZnkBfr_destroy( bfr );
		if( result_size == -1 ){
			ZnkSysErrnoInfo* err_info = ZnkSysErrno_getInfo( ZnkSysErrno_errno() );
			ZnkErr_internf( &err,
					"  ZnkSocket_send : Failure : SysErr=[%s:%s]",
					err_info->sys_errno_key_, err_info->sys_errno_msg_ );
			MoaiConnection_erase( mcn, mfds );
			return MoaiRASResult_e_Ignored;
		}
	}

	return MoaiRASResult_e_OK;
}

static MoaiRASResult
requestOnConnectCompleted_GET( ZnkSocket O_sock,
		const char* hostname, MoaiInfoID info_id, MoaiFdSet mfds )
{
	ZnkErr_D( err );
	MoaiConnection mcn  = MoaiConnection_find_byOSock( O_sock );
	MoaiInfo*      info = MoaiInfo_find( info_id );

	/***
	 * とりあえずこのsendは失敗しないと仮定.
	 */
	int result_size = 0;

	sendHdrs( O_sock, info->hdrs_.hdr1st_, info->hdrs_.vars_ ); 

	/* データを送信 */
	result_size = ZnkSocket_send( O_sock,
			ZnkBfr_data(info->stream_)+info->hdr_size_, ZnkBfr_size(info->stream_)-info->hdr_size_ );

	if( result_size == -1 ){
		ZnkSysErrnoInfo* err_info = ZnkSysErrno_getInfo( ZnkSysErrno_errno() );
		ZnkErr_internf( &err,
				"  ZnkSocket_send : Failure : SysErr=[%s:%s]",
				err_info->sys_errno_key_, err_info->sys_errno_msg_ );
		RanoLog_printf( "Moai : MoaiConnection_erase (requestOnConnectCompleted_GET)\n" );
		MoaiConnection_erase( mcn, mfds );
		return MoaiRASResult_e_Ignored;
	}
	return MoaiRASResult_e_OK;
}

static bool
sendOnConnected_CONNECT( MoaiConnection mcn, MoaiFdSet mfds, MoaiInfoID info_id )
{
	ZnkSocket   O_sock   = MoaiConnection_O_sock( mcn );
	const char* hostname = MoaiConnection_hostname( mcn );
	bool result;

	result = (bool)( requestOnConnectCompleted_CONNECT( O_sock, hostname, info_id, mfds ) == MoaiRASResult_e_OK );
	{
		ZnkFdSet fdst_observe = MoaiFdSet_fdst_observe_r( mfds );
		if( !ZnkFdSet_set( fdst_observe, O_sock ) ){
			MoaiFdSet_reserveConnectSock( mfds, O_sock );
		}
	}
	return result;
}

static bool
sendOnConnected_POST( MoaiConnection mcn, MoaiFdSet mfds, MoaiInfoID info_id )
{
	ZnkSocket   O_sock       = MoaiConnection_O_sock( mcn );
	ZnkFdSet    fdst_observe = MoaiFdSet_fdst_observe_r( mfds );

	if( MoaiPost_sendRealy( info_id, true, O_sock, mfds ) ){
		mcn->req_content_length_remain_ = 0;
		RanoLog_printf( "Moai : mcn->req_content_length_remain_=0 (sendOnConnected_POST)\n" );
	}
	if( !ZnkFdSet_set( fdst_observe, O_sock ) ){
		MoaiFdSet_reserveConnectSock( mfds, O_sock );
	}
	return true;
}

static bool
sendOnConnected_GET( MoaiConnection mcn, MoaiFdSet mfds, MoaiInfoID info_id )
{
	ZnkSocket   O_sock   = MoaiConnection_O_sock( mcn );
	const char* hostname = MoaiConnection_hostname( mcn );
	bool result;

	result = (bool)( requestOnConnectCompleted_GET( O_sock, hostname, info_id, mfds ) == MoaiRASResult_e_OK  );
	{
		ZnkFdSet fdst_observe = MoaiFdSet_fdst_observe_r( mfds );
		if( !ZnkFdSet_set( fdst_observe, O_sock ) ){
			MoaiFdSet_reserveConnectSock( mfds, O_sock );
		}
	}
	return result;
}


static MoaiRASResult
doLocalProxy( MoaiContext ctx, MoaiConnection mcn, MoaiFdSet mfds, RanoHtpType htp_type, MoaiSockType sock_type )
{
	ZnkErr_D( err );
	ZnkSocket   I_sock   = MoaiConnection_I_sock( mcn );
	ZnkSocket   O_sock   = MoaiConnection_O_sock( mcn );
	const char* hostname = MoaiConnection_hostname( mcn );
	uint16_t    port     = MoaiConnection_port( mcn );
	ZnkMyf      hosts    = RanoMyf_theHosts();
	const bool  is_proxy_use     = RanoParentProxy_isAppliedHost2( hosts, hostname, Znk_NPOS );
	const bool  is_authenticated = true;
	MoaiInfo*             info    = NULL;
	MoaiConnectedCallback cb_func = NULL;

	if( htp_type == RanoHtpType_e_Request ){
		const RanoModuleAry mod_ary = MoaiServerInfo_mod_ary();
		RanoModule          mod     = RanoModuleAry_find_byHostname( mod_ary, hostname );
		MoaiInfoID          info_id = MoaiInfo_regist( ctx->draft_info_ );

		info = MoaiInfo_find( info_id );
		info->req_method_ = ctx->req_method_;

		switch( info->req_method_ ){
		case ZnkHtpReqMethod_e_CONNECT:
			cb_func = sendOnConnected_CONNECT;
			break;
		case ZnkHtpReqMethod_e_POST:
		{
			ZnkStr str = ZnkStr_new( "" );
			MoaiPostMode post_mode = MoaiPost_decidePostMode2( hosts, hostname );
			ZnkVarp varp = NULL;

			/***
			 * 更新時間が異なる場合のみfilters内を再読み込み.
			 */
			MoaiServerInfo_reloadFilters( mod, hostname );

			/* PostVar and Cookie filtering */
			if( MoaiPost_parsePostAndCookieVars( I_sock, mfds,
					str,
					info->hdr_size_, &info->hdrs_,
					ctx->body_info_.content_length_, info->stream_,
					info->vars_,
					mod, is_authenticated ) )
			{
				RanoLog_printf( "  Filtering of POST Request done.\n" );
			}

			varp = ZnkVarpAry_find_byName( info->vars_, "Moai_PostInfoID", Znk_NPOS, false );
			if( varp ){
				MoaiInfoIDStr id_str = {{ 0 }};
				const char* moai_post_info_id = ZnkVar_cstr( varp );
				post_mode = MoaiPostMode_e_SendAfterConfirm;
				Znk_snprintf( id_str.buf_, sizeof(id_str.buf_), "%s", moai_post_info_id );
				info_id = MoaiInfoID_scanIDStr( &id_str );
			}
#if 0
			{
				MoaiInfoIDStr id_str = {{ 0 }};
				MoaiInfoID_getStr( info_id, &id_str );
				RanoLog_printf( "  doLocalProxy : ZnkHtpReqMethod_e_POST id=[%s] mod=[%p]\n", id_str.buf_, mod );
			}
#endif
			if( post_mode == MoaiPostMode_e_Confirm ){
				const char* req_urp = ZnkStr_cstr( info->req_urp_ );
				MoaiInfoIDStr id_str = {{ 0 }};
				MoaiInfoID_getStr( info_id, &id_str );
				ZnkStr_addf( str, "<p><b>Moai : POST to %s%s.</b></p>", hostname, req_urp );
				ZnkStr_addf( str, "<form action=\"http://%s%s\" method=\"POST\" enctype=\"multipart/form-data\">",
						hostname, req_urp );
				ZnkStr_addf( str, "<input type=hidden name=Moai_PostInfoID value=\"%s\">", id_str.buf_ );
				ZnkStr_add( str, "<input type=submit value=\"Send\"> " );
				ZnkStr_add( str, "</form>\n" );
				MoaiIO_sendTxtf( I_sock, "text/html", "<pre>%s</pre>", ZnkStr_cstr(str) );
				cb_func = NULL;
			} else {
				cb_func = sendOnConnected_POST;
			}
			ZnkStr_delete( str );
			break;
		}
		case ZnkHtpReqMethod_e_GET:
		default:
			//RanoLog_printf( "  doLocalProxy : Request info->req_method_=[%s]\n", ZnkHtpReqMethod_getCStr(info->req_method_) );

			/***
			 * 更新時間が異なる場合のみfilters内を再読み込み.
			 */
			MoaiServerInfo_reloadFilters( mod, hostname );

			/***
			 * Header and Cookie filtering
			 * GET時にもこれらは送信されるのでここで行っておく.
			 */
			if( mod ){
				const bool is_all_replace = MoaiServerInfo_isAllReplaceCookie();
				ZnkVarpAry extra_vars     = MoaiServerInfo_refCookieExtraVars();
				RanoModule_filtHtpHeader(  mod, info->hdrs_.vars_ );
				RanoModule_filtCookieVars( mod, info->hdrs_.vars_, is_all_replace, extra_vars );
				RanoLog_printf( "  Filtering of GET Request done.\n" );
			}
			cb_func = sendOnConnected_GET;
			break;
		}
		if( !MoaiConnection_connectFromISock( hostname, port, I_sock, "doLocalProxy",
				info_id, mfds, is_proxy_use, cb_func ) )
		{
			RanoLog_printf( "  doLocalProxy : makeConnectionFromISock failure.\n" );
			MoaiIO_sendTxtf( I_sock, "text/html", "<p><b>Moai : doLocalProxy : makeConnectionFromISock Error.</b></p>\n" );
			MoaiIO_close_ISock( "  doLocalProxy", I_sock, mfds );
			return MoaiRASResult_e_Ignored;
		}
		I_sock = MoaiConnection_I_sock( mcn );
		O_sock = MoaiConnection_O_sock( mcn );
	}
	if( htp_type == RanoHtpType_e_Response ){
		info = ctx->draft_info_;
	}
	if( htp_type == RanoHtpType_e_NotHttpStart ){
		info = ctx->draft_info_;
	}

	if( ctx->recv_executed_ && O_sock != ZnkSocket_INVALID ){

		switch( htp_type ){
		case RanoHtpType_e_Request:
			/* filtering適用後のヘッダ */
			debugInterestGoal( info, mcn, htp_type, "on doLocalProxy" );
			break;
		case RanoHtpType_e_Response:
		default:
		{
			/* Response or NotHttpStart */
			/***
			 * とりあえずこのsendは失敗しないと仮定.
			 */
			const bool with_header = (bool)( htp_type == RanoHtpType_e_Response );
			int result_size = 0;

			/***
			 * sock_typeは送信元であるから、送信先であるsockはその逆となる.
			 */
			ZnkSocket sock = ( sock_type == MoaiSockType_e_Inner ) ? O_sock 
				: ( sock_type == MoaiSockType_e_Outer ) ? I_sock
				: ZnkSocket_INVALID;

			debugInterestGoal( info, mcn, htp_type, "on doLocalProxy" );

			if( sock == ZnkSocket_INVALID ){
				RanoLog_printf( "  sock=[%d] is invalid\n", sock );
				return MoaiRASResult_e_Ignored;
			}
			sendToDst( sock, info, ctx->body_info_.txt_type_, ctx->text_, with_header );
			if( result_size == -1 ){
				ZnkSysErrnoInfo* err_info = ZnkSysErrno_getInfo( ZnkSysErrno_errno() );
				ZnkErr_internf( &err,
						"  ZnkSocket_send : Failure : SysErr=[%s:%s]",
						err_info->sys_errno_key_, err_info->sys_errno_msg_ );
				RanoLog_printf( "Moai : MoaiConnection_erase (doLocalProxy)\n" );
				MoaiConnection_erase( mcn, mfds );
				return MoaiRASResult_e_Ignored;
			}

			break;
		}
		}

	}
	return MoaiRASResult_e_OK;

}


static MoaiRASResult
recvAndSendCore( MoaiContext ctx, ZnkSocket sock, MoaiSockType sock_type, MoaiConnection mcn, MoaiFdSet mfds )
{
	static size_t count = 0;
	ZnkFdSet      fdst_observe_r = MoaiFdSet_fdst_observe_r( mfds );
	MoaiRASResult ras_result     = MoaiRASResult_e_OK;
	HtpScanType   scan_type      = HtpScan_e_None;
	RanoHtpType   htp_type       = RanoHtpType_e_None;

	scan_type = scanHttpFirst( ctx, mcn, sock, sock_type, fdst_observe_r, mfds );
	switch( scan_type ){
	case HtpScan_e_Request:
		htp_type = RanoHtpType_e_Request;
		break;
	case HtpScan_e_Response:
		htp_type = RanoHtpType_e_Response;
		break;
	case HtpScan_e_RecvZero:
	case HtpScan_e_RecvError:
		return MoaiRASResult_e_Ignored;
	case HtpScan_e_NotHttpStart:
		htp_type = RanoHtpType_e_NotHttpStart;
		break;
	case HtpScan_e_UnknownMethod:
	case HtpScan_e_CannotGetHost:
	case HtpScan_e_HostBloking:
		return MoaiRASResult_e_Ignored;
	default:
		return MoaiRASResult_e_Ignored;
	}

	if( ctx->as_local_proxy_ ){
		/* LocalProxyとして処理 */
		ras_result = doLocalProxy( ctx, mcn, mfds, htp_type, sock_type );
	} else {
		/* WebServerとして処理 */
		ras_result = MoaiWebServer_do( ctx, sock, mcn, mfds );
	}
	++count;
	if( count % 20 == 0 ){
		static const bool keep_open = true;
		RanoLog_close();
		RanoCGIUtil_initLog( "./tmp/moai_log", "./tmp/moai_count.txt", 500, 10, keep_open );
		count = 0;
	}
	return ras_result;
}

static void report_observe( MoaiFdSet mfds, void* arg )
{
	ZnkSocketAry wk_sock_ary    = MoaiFdSet_wk_sock_ary( mfds );
	ZnkFdSet     fdst_observe_r = MoaiFdSet_fdst_observe_r( mfds );
	ZnkFdSet     fdst_observe_w = MoaiFdSet_fdst_observe_w( mfds );
	const bool   is_enable_log_verbose = MoaiServerInfo_is_enable_log_verbose();
	if( is_enable_log_verbose ){
		MoaiIO_reportFdst( "Moai : ObserveR ", wk_sock_ary, fdst_observe_r, true );
	} else {
		size_t sock_ary_size;
		ZnkSocketAry_clear( wk_sock_ary );
		ZnkFdSet_getSocketAry( fdst_observe_r, wk_sock_ary );
		sock_ary_size = ZnkSocketAry_size( wk_sock_ary );
		RanoLog_printf( "Moai : ObserveR : %zu-th sock events by select.\n", sock_ary_size );
	}
	MoaiIO_reportFdst( "Moai : ObserveW ", wk_sock_ary, fdst_observe_w, true );
}
static void on_accept( MoaiFdSet mfds, ZnkSocket new_accept_sock, void* arg )
{
	RanoLog_printf( "Moai : new_accept_sock=[%d]\n", new_accept_sock );
	MoaiConnection_regist( "", 0, new_accept_sock, ZnkSocket_INVALID, mfds );
}
static MoaiRASResult recv_and_send( MoaiFdSet mfds, ZnkSocket sock, void* arg )
{
	MoaiConnection mcn = NULL;
	MoaiContext    ctx = (MoaiContext)arg;
	MoaiSockType   sock_type = MoaiSockType_e_None;

	mcn = MoaiConnection_find_byISock( sock );
	if( mcn ){
		sock_type = MoaiSockType_e_Inner;
		return recvAndSendCore( ctx, sock, sock_type, mcn, mfds );
	}

	mcn = MoaiConnection_find_byOSock( sock );
	if( mcn ){
		sock_type = MoaiSockType_e_Outer;
		return recvAndSendCore( ctx, sock, sock_type, mcn, mfds );
	}

	{
		ZnkFdSet fdst_observe_r = MoaiFdSet_fdst_observe_r( mfds );
		RanoLog_printf( "  NotFoundConnection : close sock=[%d].\n", sock );
		ZnkSocket_close( sock );
		ZnkFdSet_clr( fdst_observe_r, sock );
	}
	return MoaiRASResult_e_Ignored;
}

static bool
isAccessAllowIP( ZnkSocket new_accept_sock, uint32_t* ipaddr_ans )
{
	uint32_t ipaddr = 0;
	bool result = false;
	char ipstr[ 64 ] = "";
	uint32_t private_ip = MoaiServerInfo_private_ip();
	
	ZnkSocket_getPeerIPandPort( new_accept_sock, &ipaddr, NULL );
	ZnkNetIP_getIPStr_fromU32( ipaddr, ipstr, sizeof(ipstr) );

	RanoLog_printf( "Moai : peer ip=[%s]\n", ipstr );

	if( ipaddr == 0x0100007f || ipaddr == private_ip ){
		/* loopback接続 : この場合は無条件に許可 */
		result = true;
	} else {
		bool is_allow = (bool)( ZnkStrAry_find_isMatch( st_access_allow_ips,
					0, ipstr, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS );
		bool is_deny = (bool)( ZnkStrAry_find_isMatch( st_access_deny_ips,
					0, ipstr, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS );
		result = is_allow && !is_deny;
		RanoLog_printf( "Moai : is_allow=[%d] is_deny=[%d]\n", is_allow, is_deny );
		debugStrAry( st_access_allow_ips, "st_access_allow_ips" );
		debugStrAry( st_access_deny_ips, "st_access_deny_ips" );
#if 0
		if( (ipaddr & 0xffff) == 0x0000a8c0 ){
			/* 192.168.* からの接続 : この場合は許可 */
			result = true;
		}
#endif
	}
	if( ipaddr_ans ){
		*ipaddr_ans = ipaddr;
	}
	return result;
}

MoaiRASResult
MoaiServer_main( bool first_initiate, bool enable_parent_proxy )
{
	static bool st_first_initiated = false;
	int       sel_ret;
	MoaiFdSet mfds = NULL;

	const char*   acceptable_host = "127.0.0.1";
	ZnkServer     moai_srver    = NULL;
	ZnkServer     xhr_dmz_srver = NULL;
	ZnkSocket     listen_sock = ZnkSocket_INVALID;
	ZnkSocketAry  listen_sockary = ZnkSocketAry_create();
	MoaiFdSetFuncArg_Report   fnca_report    = { report_observe, NULL };
	MoaiFdSetFuncArg_OnAccept fnca_on_accept = { on_accept, NULL };
	MoaiFdSetFuncArg_RAS      fnca_ras       = { recv_and_send, NULL };
	struct ZnkTimeval waitval;
	bool          req_fdst_report = true;
	MoaiContext   ctx = MoaiContext_create();
	MoaiRASResult ras_result = MoaiRASResult_e_Ignored;
	ZnkMyf        config;
	ZnkMyf        hosts;
	uint16_t      moai_port    = 8124;
	uint16_t      xhr_dmz_port = 8125;
	const char*   version_str = MoaiServerInfo_version( true );
	bool          issue_new_authentic_key = true;

	RanoLog_printf( "\n" );
	RanoLog_printf( "Moai : MoaiServer_main Ver%s start.\n", version_str );

	if( ZnkDir_getType( "__authentic_key_reuse__" ) == ZnkDirType_e_File ){
		/***
		 * 既に存在する authentic_key.dat を読み込んでそれで初期化する.
		 * Moaiを再起動した場合など.
		 */
		char moai_authentic_key[ 256 ] = "";
		ZnkFile fp = Znk_fopen( "authentic_key.dat", "rb" );
		if( fp ){
			Znk_fgets( moai_authentic_key, sizeof(moai_authentic_key), fp );
			Znk_fclose( fp );
			ZnkS_chompNL( moai_authentic_key );
			if( !ZnkS_empty( moai_authentic_key ) ){
				MoaiServerInfo_set_authenticKey( moai_authentic_key );
				issue_new_authentic_key = false;
			}
		}
		ZnkDir_deleteFile_byForce( "__authentic_key_reuse__" );
	}
	if( issue_new_authentic_key ){
		/***
		 * 新規発行.
		 */
		const char* moai_authentic_key = MoaiServerInfo_authenticKey();
		ZnkFile fp = Znk_fopen( "authentic_key.dat", "wb" );
		if( fp ){
			Znk_fputs( moai_authentic_key, fp );
			Znk_fflush( fp );
			Znk_fclose( fp );
		}
	}

	/***
	 * filtersディレクトリがない場合のみ新規作成し、
	 * default/filtersから中身をコピーする.
	 */
	if( ZnkDir_getType( "filters" ) != ZnkDirType_e_Directory ){ 
		ZnkDirId dir = NULL;
		const char* name;
		char src_path[ 256 ] = "";
		char dst_path[ 256 ] = "";
		ZnkDir_mkdirPath( "filters", Znk_NPOS, '/', NULL );
		dir = ZnkDir_openDir( "default/filters" );
		if( dir ) while( true ){ /* if-while */
			name = ZnkDir_readDir( dir );
			if( name == NULL ){
				break;
			}
			Znk_snprintf( src_path, sizeof(src_path), "default/filters/%s", name );
			Znk_snprintf( dst_path, sizeof(dst_path), "filters/%s", name );
			ZnkDir_copyFile_byForce( src_path, dst_path, NULL );
		}
		ZnkDir_closeDir( dir );
	}

	fnca_ras.arg_ = ctx;

	{
		static const bool keep_open = true;
		ZnkDir_mkdirPath( "./tmp", Znk_NPOS, '/', NULL );
		RanoCGIUtil_initLog( "./tmp/moai_log", "./tmp/moai_count.txt", 500, 5, keep_open );
	}

	if( !RanoMyf_loadConfig() ){
		/***
		 * この場合は致命的エラーとすべき.
		 */
		RanoLog_printf( "Moai : Cannot load config.myf.\n" );
		getchar();
		goto FUNC_END;
	}
	RanoMyf_loadHosts();
	RanoMyf_loadTarget();
	RanoMyf_loadAnalysis();
	MoaiCGIManager_load();

	config = RanoMyf_theConfig();
	hosts  = RanoMyf_theHosts();

	/***
	 * parse config
	 */
	{
		ZnkVarpAry vars = ZnkMyf_find_vars( config, "config" );
		if( vars ){
			ZnkVarp var = NULL;

			if( enable_parent_proxy ){
				var = ZnkVarpAry_find_byName_literal( vars, "parent_proxy", false );
				if( var ){
					RanoParentProxy_set_byAuthority( ZnkVar_cstr(var) );
				}
			} else {
				RanoParentProxy_set_byAuthority( "NONE" );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "post_confirm", false );
			if( var ){
				MoaiPost_setPostConfirm( ZnkS_eq( ZnkVar_cstr(var), "on" ) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "moai_port", false );
			if( var ){
				sscanf( ZnkVar_cstr( var ), "%hu", &moai_port );
				MoaiServerInfo_set_port( moai_port );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "xhr_dmz_port", false );
			if( var ){
				sscanf( ZnkVar_cstr( var ), "%hu", &xhr_dmz_port );
				MoaiServerInfo_setXhrDMZPort( xhr_dmz_port );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "proxy_indicating_mode", false );
			if( var ){
				if( ZnkS_eq( ZnkVar_cstr(var), "minus" ) ){
					RanoParentProxy_setIndicatingMode( -1 );
				} else if( ZnkS_eq( ZnkVar_cstr(var), "plus" ) ){
					RanoParentProxy_setIndicatingMode( 1 );
				} else {
					RanoParentProxy_setIndicatingMode( 0 );
				}
			}

			var = ZnkVarpAry_find_byName_literal( vars, "enable_log_file", false );
			if( var ){
				MoaiServerInfo_set_enable_log_file( ZnkS_eq( ZnkVar_cstr(var), "on" ) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "enable_log_verbose", false );
			if( var ){
				MoaiServerInfo_set_enable_log_verbose( ZnkS_eq( ZnkVar_cstr(var), "on" ) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "blocking_mode", false );
			if( var ){
				MoaiConnection_setBlockingMode( ZnkS_eq( ZnkVar_cstr(var), "on" ) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "acceptable_host", false );
			if( var ){
				const char* var_str = ZnkVar_cstr(var);
				MoaiServerInfo_set_acceptable_host( var_str );
				if( ZnkS_eq( var_str, "ANY" ) ){
					/***
					 * localhost以外からの接続もすべて受け付ける.
					 * 現状のMoaiではいわゆるIPアドレスやドメインに応じたForbidden機能などが
					 * 実装されていないので、これを指定した場合はNIC(ルータなど)やOSのファイア
					 * ウォール機能などで接続可能なIPをLANからのもののみにするなど適切に制限すること.
					 */
					acceptable_host = NULL;
				} else if( ZnkS_eq( var_str, "LOOPBACK" ) || ZnkS_empty( var_str ) ){
					/***
					 * この場合、localhostからの接続(loopback)のみを受け付ける.
					 * 純粋にローカルプロキシとしてのみの使用ならばこの指定が一番安全である.
					 */
					acceptable_host = "127.0.0.1";
				} else {
					/***
					 * この場合、acceptable_hostにおいて指定したIPからの接続のみを受け付ける.
					 * 試験用である.
					 */
					acceptable_host = var_str;
				}
			}
			var = ZnkVarpAry_find_byName_literal( vars, "server_name", false );
			if( var ){
				MoaiServerInfo_set_server_name( ZnkVar_cstr(var) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "filters_dir", false );
			if( var ){
				MoaiServerInfo_set_filters_dir( ZnkVar_cstr(var) );
			}

			{
				char* envvar_val = ZnkEnvVar_get( "MOAI_PROFILE_DIR" );
				if( envvar_val ){
					/* defined in OS */
					MoaiServerInfo_set_profile_dir( envvar_val );
				} else {
					var = ZnkVarpAry_find_byName_literal( vars, "profile_dir", false );
					if( var ){
						MoaiServerInfo_set_profile_dir( ZnkVar_cstr(var) );
					}
				}
				ZnkEnvVar_free( envvar_val );
			}
		}
	}

	/* Rano Modules */
	MoaiServerInfo_initiate_mod_ary();

	/* Parent Proxy */
	//RanoParentProxy_loadCandidate( "parent_proxy.txt" );

	if( st_access_allow_ips == NULL ){
		st_access_allow_ips = ZnkStrAry_create( true );
	}
	if( st_access_deny_ips == NULL ){
		st_access_deny_ips = ZnkStrAry_create( true );
	}
	ZnkStrAry_copy( st_access_allow_ips, ZnkMyf_find_lines( hosts, "access_allow_ips" ) );
	ZnkStrAry_copy( st_access_deny_ips,  ZnkMyf_find_lines( hosts, "access_deny_ips" ) );

	RanoLog_printf( "Moai : acceptable_host=[%s]\n", MoaiServerInfo_acceptable_host() );
	RanoLog_printf( "Moai : blocking_mode=[%d]\n", MoaiConnection_isBlockingMode() );

	if( !MoaiServerInfo_is_enable_log_file() ){
		RanoLog_printf( "Moai : config : enable_log_file is off.\n" );
		RanoLog_printf( "Moai : Stop writing to log file and close it here.\n" );
		RanoLog_close();
	}

	/***
	 * pluginにおける初期化関数を呼び出す.
	 */
	if( first_initiate && !st_first_initiated )
	{
		RanoModuleAry mod_ary = MoaiServerInfo_mod_ary();
		RanoModuleAry_initiateFilters( mod_ary, NULL );
		st_first_initiated = true;
	}

	/* 待ち時間に 2.500 秒を指定 */
	waitval.tv_sec  = 2;
	waitval.tv_usec = 500;

	ZnkNetBase_initiate( false );

	//ZnkNetIP_printTest();
	MoaiServerInfo_autosetServerName();

	MoaiConnection_initiate();
	MoaiConnection_clearAll( mfds );
	MoaiInfo_initiate();
	                              
	/* Moai本体用のポート. */
	moai_port = MoaiServerInfo_port();
	moai_srver = ZnkServer_create( acceptable_host, moai_port );
	if( moai_srver == NULL ){
		RanoLog_printf( "Moai : Cannot create server. May be moai_port=[%u] is already used.\n", moai_port );
		goto FUNC_END;
	}

	/* XhrDMZ用のポート. */
	xhr_dmz_port = MoaiServerInfo_XhrDMZPort();
	xhr_dmz_srver = ZnkServer_create( acceptable_host, xhr_dmz_port );
	if( xhr_dmz_srver == NULL ){
		RanoLog_printf( "Moai : Cannot create server. May be xhr_dmz_port=[%u] is already used.\n", xhr_dmz_port );
		goto FUNC_END;
	}

	{
		listen_sock = ZnkServer_getListenSocket( moai_srver );
		MoaiConnection_setListeningSock( listen_sock );
		ZnkSocketAry_push_bk( listen_sockary, listen_sock );

		listen_sock = ZnkServer_getListenSocket( xhr_dmz_srver );
		MoaiConnection_setListeningSock( listen_sock );
		ZnkSocketAry_push_bk( listen_sockary, listen_sock );

		mfds = MoaiFdSet_create_ex( listen_sockary, &waitval );
	}

	RanoLog_printf( "Moai : Listen port %u...\n", moai_port );
	RanoLog_printf( "Moai : XhrDMZ Listen port %u...\n", xhr_dmz_port );
	RanoLog_printf( "\n" );
	
	while( true ){
		sel_ret = MoaiFdSet_select( mfds, &req_fdst_report, &fnca_report );

		/***
		 * fdst_read
		 */
		if( sel_ret < 0 ){
			/* select error */
			uint32_t sys_errno = ZnkSysErrno_errno();
			ZnkSysErrnoInfo* errinfo = ZnkSysErrno_getInfo( sys_errno );
			RanoLog_printf( "Moai : MoaiFdSet_select Error : sel_ret=[%d] : [%s]\n", sel_ret, errinfo->sys_errno_msg_ );
			MoaiFdSet_printf_fdst_read( mfds );
			ras_result = MoaiRASResult_e_RestartServer;
			goto FUNC_END;
		}
		if( sel_ret == 0 ){
			static size_t count = 0;
			/* select timeout */
			ZnkSocketAry wk_sock_ary    = MoaiFdSet_wk_sock_ary( mfds );
			ZnkFdSet     fdst_observe_r = MoaiFdSet_fdst_observe_r( mfds );
			if( count % 10 == 0 ){
				//RanoLog_printf( "Moai : MoaiFdSet_select timeout : count=[%zu]\n", count );
			}
			if( MoaiIO_procExile( wk_sock_ary, fdst_observe_r, mfds ) ){
				req_fdst_report = true;
			}
			MoaiFdSet_procConnectionTimeout( mfds );
			++count;
			continue;
		}
		                            
		ras_result = MoaiFdSet_process( mfds,
				&fnca_on_accept, &fnca_ras, isAccessAllowIP );
		switch( ras_result ){
		case MoaiRASResult_e_CriticalError:
		case MoaiRASResult_e_RestartServer:
			goto FUNC_END;
		case MoaiRASResult_e_RestartProcess:
			/* ブラウザ側が待機画面を受信するまで少し待つ.
			 * (受信途中にsock等を、サーバであるこちら側からcloseしてしまってはまずい). */
			//ZnkThread_sleep( 1000 );
			goto FUNC_END;
		default:
			break;
		}

		/***
		 * 現状、常に表示するようにしているが、
		 * 明らかな変更が発生した場合のみ表示するようにした方がスマートである.
		 */
		req_fdst_report = true;

	}
	ras_result = MoaiRASResult_e_OK;

FUNC_END:
	MoaiFdSet_destroy( mfds );
	{
		const size_t size = ZnkSocketAry_size( listen_sockary );
		size_t idx;
		for( idx=0; idx<size; ++idx ){
			ZnkSocket_close( ZnkSocketAry_at( listen_sockary, idx ) );
		}
		ZnkSocketAry_destroy( listen_sockary );
	}
	ZnkServer_destroy( moai_srver );
	ZnkServer_destroy( xhr_dmz_srver );
	ZnkNetBase_finalize();
	MoaiContext_destroy( ctx );
	MoaiServerInfo_finalize_mod_ary();
	RanoLog_close();

	return ras_result;
}

