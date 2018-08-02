#include "Znk_process.h"	
#include "Znk_str.h"	
#include "Znk_stdc.h"	

#if defined( Znk_TARGET_WINDOWS )
#  include <windows.h>
#else
#  include <unistd.h>
#  include <sys/wait.h>
#endif

#include <stdlib.h>

unsigned int
ZnkProcess_getCurrentProcessId( void )
{
#if defined( Znk_TARGET_WINDOWS )
	return GetCurrentProcessId();
#else
	return 0;
#endif
}


static bool st_req_reprocess = false;

void
ZnkProcess_requestReprocess( void )
{
	st_req_reprocess = true;
}
bool ZnkProcess_isReqReproces( void )
{
	return st_req_reprocess;
}

/***
 * 現在のプロセスをexitする.
 */
void 
ZnkProcess_exit( bool result )
{
	exit( result ? EXIT_SUCCESS : EXIT_FAILURE );
}

#if defined( Znk_TARGET_WINDOWS )
static HANDLE
create_process( size_t argc, const char** argv, ZnkProcessConsoleType cons_type )
{
	ZnkStr cmd_str = ZnkStr_new( argv[0] );
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof( si );

	{
		size_t i;
		for( i=1; i<argc; ++i ){
			ZnkStr_add_c( cmd_str, ' ' );
			ZnkStr_add( cmd_str, argv[i] );
		}
	}

	switch( cons_type ){
	case ZnkProcessConsole_e_CreateNew:
		dwCreationFlags |= CREATE_NEW_CONSOLE;
		break;
	case ZnkProcessConsole_e_Detached:
		dwCreationFlags |= DETACHED_PROCESS;
		break;
	default:
		break;
	}

	CreateProcess( NULL, (LPTSTR)ZnkStr_cstr(cmd_str),
			NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, &si, &pi );
	ZnkStr_delete( cmd_str );

	/***
	 * ここで実際に行われることは参照カウントのデクリメントである.
	 */
	CloseHandle( pi.hThread );

	/***
	 * pi.hProcessについてはまだクローズしない.
	 * (この関数の外部でまだ使用する自由を与えるため.
	 * ただしその代償として、使い終わったら外部で責任を持ってクローズする必要がある.)
	 */

	return pi.hProcess;
}
#endif

#if defined( Znk_TARGET_UNIX )
/***
 * この関数は現在のプロセスを指定した新しいプロセスに置き換える.
 * (現在のプロセスの実行はここで終了する.
 */
static void
exec_restart( size_t argc, const char** argv )
{
	/***
	 * execvの第2引数は、mainのargvのようにargv[0]には実行ファイルそのものの名前が
	 * 入ることに注意すること. またargvの最後の要素の次には必ずNULLがなければならない.
	 * そのため、以下では念のため別途配列を確保してから渡している.
	 */
	const char* cmd = argv[0];
	char** arg_ary = Znk_malloc( (argc+1)*sizeof(const char*) );
	size_t i;
	for( i=0; i<argc; ++i ){
		arg_ary[i] = (char*)argv[i];
	}
	arg_ary[argc] = NULL;
	execv( cmd, arg_ary );
	Znk_free( arg_ary ); /* このfreeは実行されるのか ? (おそらく実行されないのでは?) */
}
#endif


/**
 * 注:
 * system関数は使えない. これは次の理由による.
 *
 * Windowsの場合:
 *   1. system関数ではパスがUNCパス(すなわち "\\remotehost\texteditor.exe \\remotehost\document.txt"というようなパス) 
 *      の場合、正常に機能しない.
 *
 *   2. path名の中に空白スペースがある場合、Win32のsystem関数ではこれをうまく認識できない.
 *      例えば、C:\Program Files のような場合、"C:\Program Files" とダブルクォートで
 *      くくってsystem関数に渡してもこれをうまく認識しない(C:\Program というコマンドはない
 *      などというエラーメッセージが表示される) 
 *
 *      追記:
 *      WindowsXP(SP3)ではこの問題は発生しなかった.
 *      これはWindows2000だけで起こる問題なのか？
 *      要調査.
 *
 *   3. systemが指定したプロセスが終了するまで、起動側のスレッドがブロック(停止)する.
 *
 *   CreateProcess関数の場合、上記の問題は発生しない.
 *
 * UNIX環境(Cygwinも含む)の場合:
 *   UNIXのsystem関数の場合、上記のWindowsのようなパスに関する問題はない.
 *   しかしsystemの場合それは子プロセスのような扱いになり、それが停止するまで起動側のスレッドが
 *   強制的にブロックされる. これを無効にする方法がないためやはり使えない.
 */

void 
ZnkProcess_execChild( size_t argc, const char** argv, bool is_wait, ZnkProcessConsoleType cons_type )
{
#if defined(Znk_TARGET_WINDOWS)

	HANDLE hProcess = create_process( argc, argv, cons_type );
	if( is_wait ){
		WaitForSingleObject( hProcess, INFINITE );
	}
	/***
	 * pi.hProcessはもう使わないのでここでクローズ.
	 */
	CloseHandle( hProcess );

#else
	/***
	 * UNIX系の fork は単に親プロセスの分身となる子プロセス作るだけであり、全く新しいプロセスを
	 * 作成するわけではない. またexecは新しいプロセスとして開始をするものの、カレントのプロセスを
	 * 終わらせてしまう. よってUNIX系でWindowsのCreateProcessと同等のことをするにはこの２つを
	 * 組み合わせる. これはUNIX系では一般的にfork+execと呼ばれている手法である. 
	 *
	 * ちなみにAndroid-NDKにおいては fork+execは使用してはならないらしい.
	 * D/Zygote ( 909): Process 7668 terminated by signal (11) 
	 * (fork+execで起動すると突然SEGVで落ちることがあるとのこと)
	 * この場合、バックグラウンドで長時間動くプログラムは android.app.Service使うのが正しいとのこと.
	 */
	pid_t pid;
	pid = fork();
	
	if( pid == 0 ){
		/***
		 * 0 : 子プロセス.
		 * exec_restartで新しいプロセスに置き換える.
		 * これにより、プログラムの最初からの実行となる.
		 * つまり以降の処理は実行されないはず.
		 */
		exec_restart( argc, argv );

	} else if( pid > 0 ){
		/***
		 * 0より大きい場合は親プロセス.
		 * exec_restartで新しいプロセスに置き換える.
		 */
		/* 子プロセスの終了までブロックしたい場合 */
		if( is_wait ){
	 		/***
			 * Windows の WaitForSingleObjectと同等の処理のはず.
			 */
			wait( NULL );
		}
	} else {
		/***
		 * forkがエラー.
		 */
	}

#endif
}

/***
 * 現在のプロセスを指定したプログラムとして再起動する.
 */
void 
ZnkProcess_execRestart( size_t argc, const char** argv, ZnkProcessConsoleType cons_type )
{
#if defined(Znk_TARGET_WINDOWS)

	/***
	 * create_processでは pi.hProcess のクローズは行わない仕様としてあるので、
	 * ここでクローズする必要がある.
	 */
	HANDLE hProcess = create_process( argc, argv, cons_type );
	CloseHandle( hProcess );
	ZnkProcess_exit( true );

#else
	/***
	 * 単にプロセスの再起動というだけならexecだけでよい. この関数はまさにそういう用途のものである.
	 */
	exec_restart( argc, argv );

#endif
}
