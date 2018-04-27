#ifndef INCLUDE_GUARD__Znk_htp_rar_h__
#define INCLUDE_GUARD__Znk_htp_rar_h__

#include <Znk_bfr.h>
#include <Znk_varp_ary.h>
#include <Znk_socket.h>
#include <Znk_htp_hdrs.h>
#include <Znk_err.h>

Znk_EXTERN_C_BEGIN

/***
 * recvされたdataをどうするかを規定する関数.
 */
typedef int (*ZnkHtpOnRecvFunc)( void* param, const uint8_t* data, size_t data_size );
typedef struct {
	ZnkHtpOnRecvFunc func_;
	void* arg_;
} ZnkHtpOnRecvFuncArg;

/**
 * @brief
 *  urlを解釈し、hostnameとportを分離して取得する.
 */
void
ZnkHtpRAR_getHostnameAndPort( const char* url,
		char* hostname_buf, size_t hostname_buf_size, uint16_t* port );

/**
 * @brief
 *  HTTP Requestをsendし、Responseをrecvする.
 *
 * @param cnct_hostname:
 * @param cnct_port:
 *  ソケットにより接続するホストとポート.
 *  cnct_hostnameとsend_hdrs内のHostフィールドの値が異なる場合は
 *  cnct_hostname、cnct_port はProxyのホスト名とポート番号であるとみなす.
 *  cnct_hostname に hostname:portという形式を指定することはできない.
 *
 * @param send_hdrs:
 *  sendするHTTPのRequestヘッダをまずすべてここにセットしてから渡す必要がある.
 *  現状ではこの指定は必須であり、NULLは指定できない.
 *
 * @param send_body:
 *  sendするHTTPのボディを指定する.
 *  ただしこの情報が不要な場合はNULLを指定することもでき、その場合は
 *  ボディの存在しないRequestとなる.
 *
 * @param recv_hdrs:
 *  recvした結果のHTTP Responseヘッダが格納される.
 *  現状ではこの指定は必須であり、NULLは指定できない.
 *
 * @param recv_fnca:
 *  recvされたdataを加工するためのコールバックFuncArg.
 *
 * @param cookie:
 *  recvレシーブしたHTTPのSet-Cookieフィールドよりcookie情報を取得し、これに格納する.
 *  ただしこの情報が不要な場合はNULLを指定することもできる.
 *
 * @param try_connect_num:
 *  サーバへの接続の最大試行回数を指定する.
 *
 * @param is_proxy:
 *  Proxyサーバを介してsendAndRecvを行いたい場合:
 *    is_proxyをtrueにする.
 *    この場合、引数として与えるcnct_hostnameとcnct_portはproxyサーバのものである必要がある.
 *    尚、req_uriがホスト名から始まっていない場合はその前にホスト名が自動的に追加される.
 *  Proxyサーバを介さずsendAndRecvを行いたい場合:
 *    is_proxyをfalseにする.
 *    この場合、引数として与えるcnct_hostnameとcnct_portはダイレクトな接続先である.
 *    尚、このときはreq_uriはホスト名から始まっていてはならない.
 *
 * @param wk_bfr:
 *  これは内部作業用の一時バッファとして使われる.
 *  NULLを指定することもできるが、その場合はこの関数が呼ばれる度に内部で確保/解放が行われる.
 *  この関数を何度も呼び出す場合は予め確保したwk_bfrをここで与えた方が無駄が省け、
 *  処理効率はよくなる.
 *
 * @param ermsg:
 *  エラー兼リポートメッセージ格納用文字列.
 */
bool
ZnkHtpRAR_sendAndRecv( const char* cnct_hostname, uint16_t cnct_port,
		ZnkHtpHdrs send_hdrs, ZnkBfr send_body,
		ZnkHtpHdrs recv_hdrs, ZnkHtpOnRecvFuncArg recv_fnca,
		ZnkVarpAry cookie,
		size_t try_connect_num, bool is_proxy, ZnkBfr wk_bfr, ZnkStr ermsg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
