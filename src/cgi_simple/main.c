#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>
#include <stdio.h>
#include <string.h>

static void
show_result( const RanoCGIEVar* evar )
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

	printf( "Moai CGI Simple:<br>\n" );

	printf( "</body></html>\n" );
	fflush( stdout );
}

int main( int argc, char** argv )
{
	/***
	 * CGIにおける環境変数を取得します.
	 */
	RanoCGIEVar* evar = RanoCGIEVar_create();

	/***
	 * Windowsにおける標準出力(1)における \n => \r\n 自動変換を無効にします.
	 */
	Znk_Internal_setMode( 1, true );
	/***
	 * Windowsにおける標準入力(0)における \n => \r\n 自動変換を無効にします.
	 */
	Znk_Internal_setMode( 0, true );

	/***
	 * 環境変数群evarの内容の出力です.
	 * 長くなるため、サブルーチン化します.
	 */
	show_result( evar );

	/***
	 * RanoCGIEVar* は使用後解放する必要があります.
	 */
	RanoCGIEVar_destroy( evar );
	return 0;
}
