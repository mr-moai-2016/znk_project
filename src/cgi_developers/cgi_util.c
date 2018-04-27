#include "cgi_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#if _MSC_VER >= 1400
#  pragma warning(disable: 4996)
#endif


/***
 * Target OSを判定するマクロ.
 * TARGET_* という形式の名前となる.
 */
#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER) || \
	defined(__MINGW32__) || defined(__MINGW64__) || \
	defined(__BORLANDC__) || defined(__DMC__)
/* Windows API */
#  define TARGET_WINDOWS 1

#elif defined(__unix__) || defined(__linux__) || defined(__CYGWIN__) || \
	defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
/* POSIX UNIX + X11 およびその互換システム */
#  define TARGET_UNIX 1

#else
#  error "Not supported OS"
#endif


#if defined(TARGET_WINDOWS)
/* windows API */
#  include <windows.h> /* for Sleep */
#  include <io.h>      /* for setmode */
#  include <fcntl.h>   /* for setmode */

#else
/* UNIXというかPosixを使った実装 */
#  include <unistd.h> /* for usleep */

#endif

#if defined(TARGET_WINDOWS)
#endif


#define M_MIN(x,y) ( (x)<(y) ? (x) : (y) )


static void
copyStr_safely( char* buf, size_t buf_size, const char* cstr, size_t cstr_leng )
{
	if( buf_size ){
		const size_t cpy_size = M_MIN( cstr_leng, buf_size-1 );
		memmove( buf, cstr, cpy_size );
		buf[ cpy_size ] = '\0';
	}
}


#if defined(TARGET_WINDOWS)
static FILE* Internal_getStdFP( int no )
{
	switch( no ){
	case 0: return stdin;
	case 1: return stdout;
	case 2: return stderr;
	default:
		break;
	}
	return NULL;
}
#endif

void
CGIUtil_Internal_setMode( int no, int is_binary_mode )
{
#if defined(TARGET_WINDOWS)
	/* モード変更に先立ってまず確実にfflushしておく必要がある */
	fflush( (FILE*)Internal_getStdFP( no ) );
#  if defined(__BORLANDC__)
	setmode( no, is_binary_mode ? _O_BINARY : _O_TEXT );
#  else
	_setmode( no, is_binary_mode ? _O_BINARY : _O_TEXT );
#  endif
#endif
}


void
CGIUtil_sleep( size_t msec )
{
#if defined(TARGET_WINDOWS)
	Sleep( (DWORD)msec );
#else
	usleep( msec * 1000 ); /* 指定はマイクロ秒 */
#endif
}


size_t
CGIUtil_getStdInStr( char* stdin_bfr, size_t stdin_bfr_size, size_t content_length )
{
	int chr  = 0;
	size_t count = 0;

	if( stdin_bfr_size == 0 ){
		return 0;
	}
	while( count < content_length && count < stdin_bfr_size-1 ){
		/* 渡されたデータを標準入力より取得する */
		chr = fgetc( stdin );

		/* データ終了のチェック */
		if( chr == EOF || chr == content_length ){
			/* content_lengthに満たないうちに EOFが出現 */
			break;
		} else if( count < stdin_bfr_size ){
			stdin_bfr[ count ] = (unsigned char)chr;
			++count;
		}
	}
	/***
	 * count は最大でも stdin_bfr_size-1
	 * すなわち最大でもstdin_bfrの最後を指し示す.
	 * よって以下でstdin_bfrから確実にはみ出すことなくnull終端できる.
	 */
	stdin_bfr[ count ] = '\0';
	return count;
}


static char*
EnvVar_get( const char* varname )
{
	/***
	 * getenvが返すポインタはプログラマが予期しない形で非常に無効化しやすく危険である.
	 * このポインタが示す内容は、putenvやsetenvの呼び出しによってメモリが書き換えられ、
	 * ポインタが無効化される可能性がある. しかしそれだけではない. getenv関数はその
	 * 名前とは裏腹に環境変数が存在するメモリ領域を書き換える場合がある. よって、
	 * 次の単なるgetenvの呼び出しによってすらメモリが書き換えられ、このポインタが
	 * 無効化される可能性がある. このことは一見なんともないような次のようなコードが
	 * 完全に不適合なコードであることを示す.
	 *
	 * const char* var_TMP  = getenv( "TMP" );
	 * const char* var_TEMP = getenv( "TEMP" ); // <= この時点でvar_TMPは無効化する恐れがある! 
	 *
	 * 従ってユーザは、これが指す内容を直ちに別バッファへコピーすべきである.
	 * しかしこれでも万全ではない. getenvはスレッドセーフでもないため、上記の例で
	 * 別バッファへコピーするようにしたとしても、コピーが完了する前に他のスレッドにおいて、
	 * putenvやgetenvが呼び出されることで、var_TMPが突然無効化され、コピーに失敗するシナリオも
	 * 有り得る.
	 *
	 * よって、さらにこれを防ぐには、ここでGlobalMutexにより lock/unlockを掛ける必要がある.
	 * しかしこのcgi_utilは入門向けに用意されたユーティリティなのでそこまではやらない.
	 * 本格的なものについてはlibZnkのZnk_envarに実装されているので、そちらを使用していただきたい.
	 */
	const char* unsafe_ptr = NULL;
	char* ans = NULL;
	/* GlobalMutex_lock(); マルチスレッドの場合なら必要 */
	unsafe_ptr = getenv( varname );
	if( unsafe_ptr ){
		ans = strdup( unsafe_ptr );
	}
	/* GlobalMutex_unlock(); マルチスレッドの場合なら必要 */
	return ans;
}
static void
EnvVar_free( char* val )
{
	if( val ){
		free( val );
	}
}


CGIEVar*
CGIEVar_create( void )
{
	CGIEVar* evar = malloc( sizeof( struct CGIEVar_tag ) );
	evar->server_name_    = EnvVar_get( "SERVER_NAME" );
	evar->server_port_    = EnvVar_get( "SERVER_PORT" );
	evar->content_type_   = EnvVar_get( "CONTENT_TYPE" );
	evar->content_length_ = EnvVar_get( "CONTENT_LENGTH" );
	evar->remote_addr_    = EnvVar_get( "REMOTE_ADDR" );
	evar->remote_host_    = EnvVar_get( "REMOTE_HOST" );
	evar->remote_port_    = EnvVar_get( "REMOTE_PORT" );
	evar->request_method_ = EnvVar_get( "REQUEST_METHOD" );
	evar->query_string_   = EnvVar_get( "QUERY_STRING" );
	evar->http_cookie_    = EnvVar_get( "HTTP_COOKIE" );
	evar->http_user_agent_= EnvVar_get( "HTTP_USER_AGENT" );
	evar->http_accept_    = EnvVar_get( "HTTP_ACCEPT" );
	return evar;
}
void
CGIEVar_destroy( CGIEVar* evar )
{
	if( evar ){
		EnvVar_free( evar->server_name_ );
		EnvVar_free( evar->server_port_ );
		EnvVar_free( evar->content_type_ );
		EnvVar_free( evar->content_length_ );
		EnvVar_free( evar->remote_addr_ );
		EnvVar_free( evar->remote_host_ );
		EnvVar_free( evar->remote_port_ );
		EnvVar_free( evar->request_method_ );
		EnvVar_free( evar->query_string_ );
		EnvVar_free( evar->http_cookie_ );
		EnvVar_free( evar->http_user_agent_ );
		EnvVar_free( evar->http_accept_ );
		free( evar );
	}
}


int
CGIUtil_getQueryStringToken( const char* query_string, size_t query_idx,
		char* key, size_t key_size,
		char* val, size_t val_size )
{
	/***
	 * まず & で区切られたquery_idx番目のtokenを求める.
	 * そのようなtokenの開始位置をbegin,
	 * その終了位置(tokenの最後の文字の次の位置)をendとする.
	 */
	const char* begin = query_string;
	const char* end   = strchr( begin, '&' );
	while( query_idx ){
		if( end == NULL ){
			/* Error : 要求されたquery_idx番目のtokenは存在しない */	
			return 0;
		}
		begin = end+1; /* &の次 */
		end   = strchr( begin, '&' );
		--query_idx;
	}
	if( end == NULL ){
		/***
		 * 要求されたquery_idx番目のtokenは最後にある.
		 * endをquery_stringの終端文字のある位置へ補正.
		 */	
		end = query_string + strlen( query_string );
	}

	/***
	 * 次に query_idx番目のtoken内の = 位置を探索.
	 * これより前がkey, 後がvalの値であるから、
	 * これをそれぞれのバッファにコピーする.
	 */
	{
		const char* eq_pos = strchr( begin, '=' );
		if( eq_pos && eq_pos < end ){
			copyStr_safely( key, key_size, begin,    eq_pos-begin );
			copyStr_safely( val, val_size, eq_pos+1, end-(eq_pos+1) );
		} else {
			/* Error : [ begin, end ) 内に '=' が存在しない */	
			return 0;
		}
	}

	return 1;
}

