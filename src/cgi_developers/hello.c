#include "cgi_util.h"
#include <stdio.h>

int main( void )
{
	/***
	 * イキナリですが一つ問題があります.
	 * Moai CGI チュートリアルにおけるサンプルは全て一貫して、Windows/Linuxともに動作するように作成してあります.
	 *
	 * C言語でCGIを作成する場合、その一部においてHTTPヘッダを記述する必要があります.
	 * そしてHTTPヘッダにおける改行は \r\n で記述しなければならない仕様となっています.
	 * Linuxにおいては、それをそのまま記述すれば何も問題ないのですが、
	 * 一方Windowsのデフォルトでは \n 一つ書くとこれが \r\n へと内部で自動的に変換されてしまいます.
	 * つまりWindowsにおいては \r\n と書くとこのままでは \r\r\n の意味になってしまいます.
	 * かと言って \n で記述すれば今度はこの自動変換が行われないLinuxで問題になります.
	 *
	 * 改行を \n と \r\n のどちらで書くべきでしょうか？初っ端からこれはなかなかの難題です.
	 * CGI開発においてはWindowsにおけるこの自動変換は邪魔でしかありませんので、これを無効化してしまいましょう.
	 * そうすればWindowsでも、HTTPヘッダの仕様通りそのまま \r\n で記述することができます.
	 *
	 * cgi_util.hにはWindowsにおけるこの自動変換を無効にするための関数が用意されています.
	 * CGIUtil_Internal_setMode 関数がそれにあたります.
	 * ちなみにここでの第1引数の 1 は標準出力を意味します.
	 * 第2引数の true はバイナリモードへの変換を意味します.
	 *
	 * 尚libZnkにもこれと全く同様の関数が用意されています.
	 * Znk_stdc.h で宣言されている Znk_Internal_setMode 関数がそれにあたります.
	 * libZnkを使った開発については、後の章のサンプルで解説します.
	 */
	{
		static const int is_binary_mode = 1;
		CGIUtil_Internal_setMode( 1, is_binary_mode );
	}


	/***
	 * Output HTTP Header for CGI
	 * (改行コードは必ず \r\n として記述しよう)
	 */

	/* Content-Typeの指定は必須です.
	 * charset は最近では utf-8 を指定するのが教科書的には正しいのでしょうが、
	 * 現在のMoaiユーザのターゲット層を考えた場合 Shift_JISの方が良いケースが多いと思われますので
	 * ここではそうしておきます. */
	printf( "Content-Type: text/html; charset=Shift_JIS\r\n" );

	/* CGIの場合ブラウザに内容をキャッシュさせたくない場合がほとんどでしょう.
	 * ブラウザにこれを指示するため、この２行も加えます. */
	printf( "Pragma: no-cache\r\n" );
	printf( "Cache-Control: no-cache\r\n" );

	/* この改行はHTTPヘッダ部の終わりを意味し、必須です. */
	printf( "\r\n" );


	/***
	 * Output HTML for CGI
	 * (ここから先は改行コードはもはやどちらでもよいが \n にしておきます)
	 */
	printf( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
	printf( "<html>\n" );
	printf( "<head>\n" );
	printf( "<META http-equiv=\"Content-type\" content=\"text/html; charset=Shift_JIS\">\n" );
	printf( "<META http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n" );
	printf( "<META http-equiv=\"Content-Style-Type\" content=\"text/css\">\n" );
	printf( "</head>\n" );

	printf( "<body>\n" );

	printf( "Hello World on <b>Moai CGI</b>.\n" );

	printf( "</body></html>\n" );
	fflush( stdout );

	return 0;
}
