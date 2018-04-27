#include <Znk_envvar.h>
#include <Znk_missing_libc.h>
#include <Znk_mutex.h>
#include <stdlib.h>
#include <string.h>

char*
ZnkEnvVar_get( const char* varname )
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
	 * その上でユーザはgetenvやputenvを直接呼び出すようなことはせず、常にZnkEnvVar_getや
	 * ZnkEnvVar_setを使うように徹底することでようやく万全となる.
	 */
	const char* unsafe_ptr = NULL;
	char* ans = NULL;
	ZnkGlobalMutex_lock();
	unsafe_ptr = getenv( varname );
	if( unsafe_ptr ){
		ans = strdup( unsafe_ptr );
	}
	ZnkGlobalMutex_unlock();
	return ans;
}

typedef struct {
	const char* varname_;
	char* cmd_str_;
} EnvVarInfo;

static EnvVarInfo** st_info_ary;
static size_t       st_info_ary_size = 0;

static EnvVarInfo*
internEnvVarInfo( const char* varname )
{
	EnvVarInfo* info;
	size_t i;
	for( i=0; i<st_info_ary_size; ++i ){
		info = st_info_ary[ i ];
		if( strcmp(info->varname_, varname) == 0 ){
			/* found */
			return info;
		}
	}
	/* not found */
	if( st_info_ary_size ){
		++st_info_ary_size;
		st_info_ary = realloc( st_info_ary, st_info_ary_size * sizeof(EnvVarInfo*) );
	} else {
		st_info_ary_size = 1;
		st_info_ary = malloc( sizeof(EnvVarInfo*) );
	}
	info = malloc( sizeof(EnvVarInfo) );
	info->varname_ = strdup( varname );
	info->cmd_str_ = NULL;
	st_info_ary[ st_info_ary_size-1 ] = info;
	return info;
}

void
ZnkEnvVar_set( const char* varname, const char* val )
{
	/***
	 * putenv関数に関する危険性はgetenvよりもさらに深刻である.
	 * putenv関数は、引数として渡された文字列のコピーを作成しない.
	 * この関数は引数として渡された文字列へのポインタをグローバルな環境配列に挿入するが、
	 * もし自動記憶域期間を持つバッファを指すポインタが putenv に引数として渡された場合、
	 * バッファに割り当てられたメモリは、呼び出し側の関数がリターンした時に破壊される.
	 * 要するに putenv の引数は、staticな領域へのポインタか、または適切な寿命管理下に置かれた
	 * ヒープへのポインタでなければならない.
	 *
	 * また、スレッドセーフではないという問題に関してはgetenvと同様である.
	 */
	const size_t require_size = ( strlen(varname) + 1 + (val ? strlen(val) : 0) + 1 ) / 8 * 8 + 8;
	char* cmd_str = NULL;

	cmd_str = malloc( require_size );
	if( val ){
		Znk_snprintf( cmd_str, require_size, "%s=%s", varname, val );
	} else {
		/* unset */
		Znk_snprintf( cmd_str, require_size, "%s=", varname );
	}

	ZnkGlobalMutex_lock();
	{
		EnvVarInfo* info = internEnvVarInfo( varname );
		putenv( cmd_str );
		/* 旧info->cmd_str_は、putenv内でもう参照されていないはずなので破棄し、
		 * 今現在参照中の新しいものに置き換える */
		if( info->cmd_str_ ){ free( info->cmd_str_ ); }
		info->cmd_str_ = cmd_str;
	}
	ZnkGlobalMutex_unlock();

	if( val == NULL ){
		/* 本来ならst_info_aryをshrinkすべきだが、
		 * とりあえず時間がないので、今はやらない */
	}
}

void
ZnkEnvVar_free( char* val )
{
	if( val ){
		free( val );
	}
}
