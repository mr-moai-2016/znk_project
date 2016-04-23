#ifndef INCLUDE_GUARD__Znk_mutex_h__
#define INCLUDE_GUARD__Znk_mutex_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkMutex )

/**
 * @brief
 * グローバルMutex
 * このMutexはすべてのタイミング、すべてのThreadにおいて使用することができる.
 * アプリケーション一つにつき唯一存在するものである.
 * (それゆえdestroy系の関数は提供されない)
 * ただし、その分衝突が発生する可能性は増すため、使用は必要最小限に留めるべき.
 */
void ZnkGlobalMutex_init( void );
void ZnkGlobalMutex_delete( void );
void ZnkGlobalMutex_lock( void );
void ZnkGlobalMutex_unlock( void );

/**
 * @brief
 * ZnkMutexは複数のインスタンスを持つことができる.
 * 用途によってMutexを別インスタンスにすることで、その分衝突の発生する頻度を
 * 下げることができる.
 */
ZnkMutex ZnkMutex_create( void );
void     ZnkMutex_destroy( ZnkMutex mtx );
void     ZnkMutex_lock( ZnkMutex mtx );
void     ZnkMutex_unlock( ZnkMutex mtx );

/**
 * @brief
 * Singleton Mutex を返す関数を定義する.
 */
#define Znk_THE_MUTEX( theMutexPtr, theMutex ) \
	static ZnkMutex* theMutexPtr( void ) { \
		static ZnkMutex st_mutex = NULL; \
		if( st_mutex == NULL ){ \
			ZnkGlobalMutex_lock(); \
			if( st_mutex == NULL ){ st_mutex = ZnkMutex_create(); } \
			ZnkGlobalMutex_unlock(); \
		} \
		return &st_mutex; \
	} \
	Znk_INLINE ZnkMutex theMutex( void ){ return *(theMutexPtr()); }


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
