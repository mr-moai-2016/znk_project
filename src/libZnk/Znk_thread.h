#ifndef INCLUDE_GUARD__Znk_thread_h__
#define INCLUDE_GUARD__Znk_thread_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef void*  (*ZnkThreadFunc)( void* );

Znk_DECLARE_HANDLE( ZnkThreadHandle )

void
ZnkThread_sleep( size_t msec );

void ZnkThreadList_initiate( size_t list_num );
void ZnkThreadList_finalize( void );

/**
 * @brief
 * 新しいスレッドを作成する.
 * 戻り値はスレッドNoである.
 *
 * この関数が制御を返した時点で active フラグは厳密に子スレッドの
 * 処理の寿命を反映する. つまり子スレッドが仕事をまっとうし、終了した場合以外
 * これが false になる可能性はない.
 * ( 言い換えれば、active フラグが true に設定されるより前の段階で
 * 子スレッドの処理が始まることはありえないということ )
 */
size_t ZnkThread_create( ZnkThreadFunc func, void* arg );

/**
 * @brief
 * 指定したスレッドNoに相当するスレッドの終了を待つ.
 * この関数を呼び出した側のスレッドは、その待ちの間は制御がブロックされ、
 * 停止する.
 *
 * 戻り値は処理系が提供するThread終了待ち関数が返す整数値であり、
 * その意味は処理系に依存する.
 * (Windowsの場合WaitForSingleObject、pthread の場合 pthread_join が返す値となる)
 */
int ZnkThread_waitEnd( size_t thno );
void ZnkThread_waitEndAll( void );

/**
 * @brief
 * 与えられたスレッドNoより、ZnkThreadHandle を得る.
 */
ZnkThreadHandle ZnkThread_getHandle( size_t thno );

/**
 * @brief
 * 与えられたスレッドNoがactiveであるか否かを返す.
 * Znk_NPOSを与えた場合、メインスレッドについてのQueryであるとみなす.
 * その場合、当然ながら常に true を返す.
 */
Znk_INLINE bool ZnkThread_isActive( size_t thno ){
	if( thno == Znk_NPOS ){ return true; }
	/* この手の処理は、pthreadならば一見するとpthread_killを用いれば
	 * 実現できそうだが、実際は処理系によってはうまくいかないことが
	 * あるらしい. (Web上の情報だが、スレッドがない場合クラッシュしたり
	 * 親スレッドが反応したりするケースがあるとのこと)
	 */
	return (bool)( ZnkThread_getHandle( thno ) != NULL );
}

/**
 * @brief
 * 現在のスレッドのスレッドNo を得る.
 * ただし、メインスレッドにおいて呼ばれた場合、Znk_NPOSを返す.
 */
size_t ZnkThread_getCurrentNo( void );

/**
 * @brief
 * 現在のスレッドのZnkThreadHandle を得る.
 * ただし、メインスレッドにおいて呼ばれた場合、NULLを返す.
 */
Znk_INLINE ZnkThreadHandle ZnkThread_getCurrentHandle( void ){
	return ZnkThread_getHandle( ZnkThread_getCurrentNo() );
}

bool ZnkThread_terminateForce( size_t thno );


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
