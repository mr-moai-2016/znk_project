#include "Znk_mutex.h"
#include "Znk_bfr.h"
#include "Znk_stdc.h"

/***
 * ここのでの実装は最速性能よりも安全性を重視する.
 * とはいえ、Thread関係は厳密性を追及するときりがない(実装もAPIも限りなく複雑化してしまう)ので、
 * 適当に使用に関するポリシーで折り合いどころを見つけて妥協するほうが現実的であるとは思う.
 */

/***
 * Global Mutex
 */
#if defined(Znk_TARGET_WINDOWS)
/* windows API */
#  include <process.h>
#  include <windows.h>
typedef CRITICAL_SECTION SysAtomicCS;

#else
/* Unix というか pthread を使った実装 */
#  include <pthread.h>
#  include <unistd.h> /* for usleep */
#  define M_PTHREAD_MUTEX_INIT( mutex ) pthread_mutex_init( mutex, 0 );
typedef pthread_mutex_t SysAtomicCS;

#endif

#include <assert.h>


#if defined(Znk_TARGET_WINDOWS)
Znk_INLINE void SysAtomicCS_init( SysAtomicCS* cs ){
	InitializeCriticalSection( cs );
}
Znk_INLINE void SysAtomicCS_delete( SysAtomicCS* cs ){
	DeleteCriticalSection( cs );
}
Znk_INLINE void SysAtomicCS_lock( SysAtomicCS* cs ){
	EnterCriticalSection( cs );
}
Znk_INLINE void SysAtomicCS_unlock( SysAtomicCS* cs ){
	LeaveCriticalSection( cs );
}

#else
Znk_INLINE void SysAtomicCS_init( SysAtomicCS* cs ){
	M_PTHREAD_MUTEX_INIT( cs );
}
Znk_INLINE void SysAtomicCS_delete( SysAtomicCS* cs ){
	pthread_mutex_destroy( cs );
}
Znk_INLINE void SysAtomicCS_lock( SysAtomicCS* cs ){
	pthread_mutex_lock( cs );
}
Znk_INLINE void SysAtomicCS_unlock( SysAtomicCS* cs ){
	pthread_mutex_unlock( cs );
}

#endif


static SysAtomicCS*
theGlobalSysAtomicCS( void )
{
	/***
	 * ここはオブジェクトが未構築の状態では厳密にはthread-safeとは言えない.
	 * mutex 構築中に別のスレッドが割り込んでくる可能性があるからである.
	 * これを確実に防ぐには、同時アクセスはあり得ないとわかっている段階で
	 * 早々に構築を完了してしまうことである.
	 */
	/* Meyers Singleton */
	static bool is_initialized = false;
	static SysAtomicCS st_cs;
	if( !is_initialized ){
		is_initialized = true;
		SysAtomicCS_init( &st_cs );
	}
	return &st_cs;
}
void ZnkGlobalMutex_init( void )
{
	theGlobalSysAtomicCS(); /* 構築を促す */
}
void ZnkGlobalMutex_delete( void )
{
	SysAtomicCS_delete( theGlobalSysAtomicCS() );
}
void ZnkGlobalMutex_lock( void )
{
	SysAtomicCS_lock( theGlobalSysAtomicCS() );
}
void ZnkGlobalMutex_unlock( void )
{
	SysAtomicCS_unlock( theGlobalSysAtomicCS() );
}

/***
 * ZnkMutex_create :
 *   これはスレッドセーフではないが、そもそもMutexを異なるスレッドから同時に
 *   作成しようとすることは正常な処理とはいえず、そのような状況は除外してよいだろう.
 *
 * ZnkMutex_destroy :
 *   Mutexが複数のスレッドから同時に削除されるような処理は正常とはいえないため、
 *   この可能性も考慮する必要はないだろう. Singleton なMutexにおいても同様である.
 */
struct ZnkMutexImpl{
	SysAtomicCS cs_;
};

#define GET_CS( mtx ) (&((struct ZnkMutexImpl*)(mtx))->cs_)

ZnkMutex ZnkMutex_create( void )
{
	ZnkMutex mtx = (ZnkMutex)Znk_malloc( sizeof(struct ZnkMutexImpl) );
	SysAtomicCS_init( GET_CS(mtx) );
	return mtx;
}
void ZnkMutex_destroy( ZnkMutex mtx )
{
	if( mtx ){
		SysAtomicCS_delete( GET_CS(mtx) );
		Znk_free( mtx );
	}
}
void ZnkMutex_lock( ZnkMutex mtx )
{
	if( mtx ){ SysAtomicCS_lock( GET_CS(mtx) ); }
}
void ZnkMutex_unlock( ZnkMutex mtx )
{
	if( mtx ){ SysAtomicCS_unlock( GET_CS(mtx) ); }
}

