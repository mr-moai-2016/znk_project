#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>
#include <Znk_htp_util.h>
#include <stdio.h>
#include <string.h>

static void
show_result( const ZnkVarpAry post_vars )
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

	printf( "<pre>\n" );
	printf( "PostVars from Query String: \n" );
	{
		ZnkStr msg = ZnkStr_new( "" );
		size_t i; 
		for( i=0; i<ZnkVarpAry_size(post_vars); ++i ){
			ZnkVarp     var  = ZnkVarpAry_at( post_vars, i );
			const char* name = ZnkVar_name_cstr(var);
			const char* val  = ZnkVar_cstr(var);
			ZnkStr_addf( msg, "%s = [%s]\n", name, val );
		}
		ZnkHtpURL_negateHtmlTagEffection( msg ); /* for XSS */
		fputs( ZnkStr_cstr(msg), stdout );
		ZnkStr_delete( msg );
	}
	printf( "</pre>" );

	printf( "</body></html>\n" );
	fflush( stdout );
}

int main( int argc, char **argv )
{
	/***
	 * CGIにおける環境変数を取得します.
	 */
	RanoCGIEVar* evar = RanoCGIEVar_create();

	/***
	 * URLにおける Query String が指定されている場合はそれを取得します.
	 * 指定されていない場合、この関数は空文字を返します.
	 */
	const char* query_string = RanoCGIUtil_getQueryString( evar );

	/***
	 * ZnkVarpAryは変数の配列を格納するための汎用のデータ構造です.
	 * これを用いて、以下では POST変数群全体を格納します.
	 */
	ZnkVarpAry post_vars = ZnkVarpAry_create( true );

	/***
	 * Windowsにおける標準入力(0)における \n => \r\n 自動変換を無効にします.
	 */
	Znk_Internal_setMode( 0, true );
	/***
	 * Windowsにおける標準出力(1)における \n => \r\n 自動変換を無効にします.
	 */
	Znk_Internal_setMode( 1, true );

	/***
	 * query_string の内容は以下のように & で区切られる変数指定の連続となっているはずです.
	 *
	 * name1=val1&name2=val2&name3=val3&...
	 *
	 * これを & 文字に関して分割(split)して、結果をpost_vars に格納します.
	 */
	RanoCGIUtil_splitQueryString( post_vars, query_string, Znk_NPOS, false );

	/***
	 * post_varsの内容の出力です.
	 * 長くなるため、サブルーチン化します.
	 */
	show_result( post_vars );

	/***
	 * ZnkVarpAryは使用後解放する必要があります.
	 */
	ZnkVarpAry_destroy( post_vars );

	/***
	 * RanoCGIEVar* は使用後解放する必要があります.
	 */
	RanoCGIEVar_destroy( evar );
	return 0;
}
