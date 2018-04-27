#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>
#include <Znk_thread.h>
#include <stdio.h>
#include <string.h>

static size_t st_count = 0;

static void
very_long_work( void )
{
	while( st_count <= 100 ){
		ZnkFile fp = Znk_fopen( "../publicbox/state.dat", "wb" );
		if( fp ){
			Znk_fprintf( fp, "%zu", st_count );
			++st_count;
			Znk_fclose( fp );
		}
		ZnkThread_sleep( 100 ); /* 100ミリ秒Sleepする */
	}
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

	/* まずJavascript progress.jsを含んだHTMLを表示させる. */
	{
		ZnkBird bird = ZnkBird_create( "#[", "]#" );
		RanoCGIUtil_printTemplateHTML( evar, bird, "../publicbox/progress.html" );
		ZnkBird_destroy( bird );
	}

	Znk_fflush( Znk_stdout() );
	/* Broken-Pipeを強制的に引き起こし、Web Server側でのReadループを強制終了させる. */
	Znk_fclose( Znk_stdout() );

	/* 時間のかかる処理. */
	very_long_work();

	RanoCGIEVar_destroy( evar );
	return 0;
}
