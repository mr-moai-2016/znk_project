#include "cgi_util.h"
#include <stdio.h>
#include <string.h>

static const char*
rejectHtmlTag( const char* cstr )
{
	/***
	 * 一応最低限のXSS対策はしておく.
	 * 本来はもっと真面目に無効化処理を行うべきですが、これは所詮サンプルなので極力簡単化するため、
	 * HTMLタグを含む可能性がある時点でNULLを返して弾くという猛烈な制限をかけて対処しています.
	 */
	if( cstr && ( strchr( cstr, '<' ) || strchr( cstr, '>' ) ) ){
		return NULL;
	}
	return cstr;
}

static void
show_result( const CGIEVar* evar )
{
	/***
	 * Output HTTP Header for CGI
	 */
	printf( "Content-type: text/html; charset=Shift_JIS\r\n" );
	printf( "Pragma: no-cache\r\n" );
	printf( "Cache-Control: no-cache\r\n" );
	printf( "\r\n" );

	/***
	 * Output HTML for CGI
	 */
	printf( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
	printf( "<html>\n" );
	printf( "<head>\n" );
	printf( "<META http-equiv=\"Content-type\" content=\"text/html; charset=Shift_JIS\">\n" );
	printf( "<META http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n" );
	printf( "<META http-equiv=\"Content-Style-Type\" content=\"text/css\">\n" );
	printf( "</head>\n" );

	printf( "<body>\n" );

	printf( "Moai CGI Enviroment Variables:<br>\n" );

	printf( "<pre>\n" );
	printf( "server_name=[%s]\n",     rejectHtmlTag(evar->server_name_) );
	printf( "server_port=[%s]\n",     rejectHtmlTag(evar->server_port_) );
	printf( "content_type=[%s]\n",    rejectHtmlTag(evar->content_type_) );
	printf( "content_length=[%s]\n",  rejectHtmlTag(evar->content_length_) );
	printf( "remote_addr=[%s]\n",     rejectHtmlTag(evar->remote_addr_) );
	printf( "remote_host=[%s]\n",     rejectHtmlTag(evar->remote_host_) );
	printf( "remote_port=[%s]\n",     rejectHtmlTag(evar->remote_port_) );
	printf( "request_method=[%s]\n",  rejectHtmlTag(evar->request_method_) );
	printf( "query_string=[%s]\n",    rejectHtmlTag(evar->query_string_) );
	printf( "http_cookie=[%s]\n",     rejectHtmlTag(evar->http_cookie_) );
	printf( "http_user_agent=[%s]\n", rejectHtmlTag(evar->http_user_agent_) );
	printf( "http_accept=[%s]\n",     rejectHtmlTag(evar->http_accept_) );
	printf( "</pre>" );

	printf( "</body></html>\n" );
	fflush( stdout );
}

int main( void )
{
	/***
	 * CGIにおける環境変数を取得します.
	 */
	CGIEVar* evar = CGIEVar_create();

	/***
	 * Windowsにおける標準出力(1)における \n => \r\n 自動変換を無効にします.
	 */
	{
		static const int is_binary_mode = 1;
		CGIUtil_Internal_setMode( 1, is_binary_mode );
	}

	/***
	 * 環境変数群evarの内容の出力です.
	 * 長くなるため、サブルーチン化します.
	 */
	show_result( evar );

	/***
	 * CGIEVar* は使用後解放する必要があります.
	 */
	CGIEVar_destroy( evar );
	return 0;
}
