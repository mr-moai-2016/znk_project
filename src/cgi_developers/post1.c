#include "cgi_util.h"
#include <stdio.h>
#include <stdlib.h>
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
show_result( CGIEVar* evar, const char* query_string )
{
	/***
	 * Output HTTP Header for CGI
	 */
	printf( "Content-type: text/html; charset=Shift_JIS\r\n" );
	/* ブラウザにキャッシュさせない。 */
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

	printf( "Moai CGI Post test.\n" );
	printf( "<form action=\"post1.cgi\" method=\"POST\">\n" );
	printf( "<b>E-mail</b><input type=text name=email size=\"28\"><br>\n" );
	printf( "<b>コメント</b><textarea name=com cols=\"48\" rows=\"4\"></textarea><br>\n" );
	printf( "<input type=submit value=\"返信する\">\n" );
	printf( "</form>\n" );

	printf( "<pre>\n" );
	printf( "URL Query String Tokens: \n" );
	/***
	 * これは所詮サンプルであるため、あまり柔軟性を考慮するとコードが煩雑になり過ぎて本質がぼやけてしまいます.
	 * ここでは、変数の最大数、値バッファや入力バッファの最大サイズなどを固定化してあります.
	 */
	{
		size_t i = 0; 
		for( i=0; i<64; ++i ){
			char key[ 256 ] = ""; 
			char val[ 256 ] = ""; 
			int result = CGIUtil_getQueryStringToken( query_string, i,
					key, sizeof(key),
					val, sizeof(val) );
			if( result == 0 ){
				break;
			}
			printf( "%s = [%s]\n", rejectHtmlTag(key), rejectHtmlTag(val) );
		}
	}

	printf( "\n" );

	/***
	 * CGIにおけるフォームの投稿データをpost_varsへと取得します.
	 * またこの関数においては第1引数evarの指定も必要となります.
	 */
	printf( "StdIn Posting Data: \n" );
	{
		char stdin_bfr[ 4096 ] = "";
		char* end = NULL;
		size_t content_length = strtoul( evar->content_length_, &end, 10 );
		printf( "Content-Length = [%u]\n", (unsigned int)content_length );
		if( evar->content_length_ != end ){
			size_t i = 0; 
			CGIUtil_getStdInStr( stdin_bfr, sizeof(stdin_bfr), content_length );
			printf( "Original data = [%s]\n", rejectHtmlTag(stdin_bfr) );
			printf( "Tokens: \n" );
			for( i=0; i<64; ++i ){
				char key[ 256 ] = ""; 
				char val[ 256 ] = ""; 
				int result = CGIUtil_getQueryStringToken( stdin_bfr, i,
						key, sizeof(key),
						val, sizeof(val) );
				if( result == 0 ){
					break;
				}
				printf( "%s = [%s]\n", rejectHtmlTag(key), rejectHtmlTag(val) );
			}
		} else {
			printf( "Error : content_length is not found.\n" );
		}
	}

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
	 * URLにおける Query String が指定されている場合はそれを取得します.
	 * 指定されていない場合、この関数は空文字を返します.
	 */
	const char* query_string = CGIUtil_getQueryString( evar );

	{
		static const int is_binary_mode = 1;
		/***
		 * Windowsにおける標準入力(0)における \n => \r\n 自動変換を無効にします.
		 */
		CGIUtil_Internal_setMode( 0, is_binary_mode );
		/***
		 * Windowsにおける標準出力(1)における \n => \r\n 自動変換を無効にします.
		 */
		CGIUtil_Internal_setMode( 1, is_binary_mode );
	}

	/***
	 * query_string の内容は以下のように & で区切られる変数指定の連続となっているはずです.
	 *
	 * name1=val1&name2=val2&name3=val3&...
	 *
	 * これを & 文字に関して分割(split)して、結果を出力します.
	 * また標準入力よりCGIにおけるフォームの投稿データを取得して、結果を出力します.
	 * 長くなるため、サブルーチン化します.
	 */
	show_result( evar, query_string );

	/***
	 * CGIEVar* は使用後解放する必要があります.
	 */
	CGIEVar_destroy( evar );
	return 0;
}
