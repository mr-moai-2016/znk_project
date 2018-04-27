#ifndef INCLUDE_GUARD__cgi_util_h__
#define INCLUDE_GUARD__cgi_util_h__

#include <stddef.h>

/***
 * 標準入出力に関するバイナリモードの変更.
 */
void
CGIUtil_Internal_setMode( int no, int is_binary_mode );


/***
 * msecミリ秒sleepします.
 */
void
CGIUtil_sleep( size_t msec );


/***
 * @brief
 * 標準入力からのデータをstdin_bfrへと格納する.
 * stdin_bfrには最低でも stdin_bfr_size バイト以上のメモリが確保されていなければならない.
 *
 * @return
 * 実際に取得されたバイト数を返す.
 * すなわちこれがcontent_lengthと同じ場合はすべて取得できており、
 * そうでなければ、content_length が stdin_bfr_size より大きいか、
 * さもなければ予期せぬEOFが発生した可能性がある.
 */
size_t
CGIUtil_getStdInStr( char* stdin_bfr, size_t stdin_bfr_size, size_t content_length );


/***
 * CGIで扱う環境変数.
 */
typedef struct CGIEVar_tag {
	char* server_name_;
	char* server_port_;
	char* content_type_;
	char* content_length_;
	char* remote_addr_;
	char* remote_host_;
	char* remote_port_;
	char* request_method_;
	char* query_string_;
	char* http_cookie_;
	char* http_user_agent_;
	char* http_accept_;
} CGIEVar;

CGIEVar*
CGIEVar_create( void );

void
CGIEVar_destroy( CGIEVar* evar );


/***
 * @brief
 * CGIで扱うQuery String.
 *
 * Query String が指定されている場合はそれを取得する.
 * 指定されていない場合、この関数は空文字を返す.
 * Query String が指定されていない場合、evar->query_string_ は NULLとなっている.
 *
 * @return
 * 成功ならば1、失敗ならば0を返す.
 */
#define CGIUtil_getQueryString( evar ) ( (evar)->query_string_ ? (evar)->query_string_ : "" )

int
CGIUtil_getQueryStringToken( const char* query_string, size_t query_idx,
		char* key, size_t key_size,
		char* val, size_t val_size );


#endif /* INCLUDE_GUARD */
