#include <Rano_cgi_util.h>
#include <Znk_bird.h>
#include <Znk_dir.h>
#include <stdio.h>
#include <string.h>

static void
show_result( RanoCGIEVar* evar )
{
	/* ZnkBirdの生成. */
	ZnkBird bird = ZnkBird_create( "#[", "]#" );
	/* BIRDタグの登録. */
	ZnkBird_regist( bird, "your_name", "Mr.Moai" );
	/* テンプレートHTMLを読み込み、BIRDタグを展開して表示. */
	RanoCGIUtil_printTemplateHTML( evar, bird, "../publicbox/hello_template.html" );
	/* ZnkBirdの破棄. */
	ZnkBird_destroy( bird );
}

int main(int argc, char **argv)
{
	/***
	 * Create+Get Environment Variables for CGI
	 */
	RanoCGIEVar* evar = RanoCGIEVar_create();

	/***
	 * for Windows : change stdin/stdout to binary-mode.
	 */
	Znk_Internal_setMode( 1, true );

	show_result( evar );

	RanoCGIEVar_destroy( evar );
	return 0;
}
