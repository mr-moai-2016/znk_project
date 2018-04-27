#include "cgi_util.h"
#include <stdio.h>

static void
show_result( void )
{
	/***
	 * Output HTML for CGI
	 */
	printf( "Content-type: text/html; charset=Shift_JIS\r\n" );
	/* ブラウザにキャッシュさせない。 */
	printf( "Pragma: no-cache\r\n" );
	printf( "Cache-Control: no-cache\r\n" );
	printf( "\r\n" );

	printf( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
	printf( "<html>\n" );
	printf( "<head>\n" );
	printf( "<META http-equiv=\"Content-type\" content=\"text/html; charset=Shift_JIS\">\n" );
	printf( "<META http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n" );
	printf( "<META http-equiv=\"Content-Style-Type\" content=\"text/css\">\n" );
	printf( "</head>\n" );

	printf( "<body>\n" );

	printf( "Moai CGI Transfer chunked test.<br>" );
	fflush( stdout );
	CGIUtil_sleep( 1000 );

	printf( "Chunk1 fflush<br>\n" );
	fflush( stdout );
	/* バッファに何もない場合は何も起こらない.
	 * (0-sizeなchunkが送信されることはない) */
	fflush( stdout );
	CGIUtil_sleep( 1000 );

	printf( "Chunk2 fflush<br>\n" );
	fflush( stdout );
	CGIUtil_sleep( 1000 );

	printf( "C" );
	/* fflush を実行しなと、通常は中途半端な状態でchunkは送信されない */
	CGIUtil_sleep( 1000 );

	printf( "hunk3 fflush<br>\n" );
	fflush( stdout );
	CGIUtil_sleep( 1000 );

	printf( "Chunk4<br>\n" ); /* 改行を入れただけではchunkは送信されない */
	CGIUtil_sleep( 1000 );

	printf( "End." );
	printf( "</body></html>\n" );
	/* 最後のfflushは別になくともプロセスの終了で自動的に送信される */
}

int main( void )
{
	static const int is_binary_mode = 1;
	/***
	 * Windowsにおける標準出力(1)における \n => \r\n 自動変換を無効にします.
	 */
	CGIUtil_Internal_setMode( 1, is_binary_mode );

	show_result();
	return 0;
}
