/*
 * Public domain
 * pthread.h compatibility shim
 */

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/*
 * Static once initialization values.
 */
//#define PTHREAD_ONCE_INIT   { INIT_ONCE_STATIC_INIT }
#define PTHREAD_ONCE_INIT   { 0 }

/*
 * Once definitions.
 */
struct pthread_once {
	//INIT_ONCE once;
	int once;
};
typedef struct pthread_once pthread_once_t;

#if 0
static inline BOOL CALLBACK
_pthread_once_win32_cb(PINIT_ONCE once, PVOID param, PVOID *context)
{
	void (*cb) (void) = param;
	cb();
	return TRUE;
}
#endif

#if 0
static pthread_mutex_t once_lock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

// This implementation doesn't support thread-cancelation nor fork during
// the execution of init_routine().
static inline int
pthread_once(pthread_once_t  *once_control, void (*init_routine)(void)) {
	// pthread_once always returns 0.
	
	if (once_control == NULL || init_routine == NULL) {
	  return 0;
	}
	
	// Double-checked locking is not a good solution in general, but it works
	// in this case in C.
	if (*once_control != PTHREAD_ONCE_INIT) {
	  return 0;
	}
	
	pthread_mutex_lock(&once_lock);
	
	if (*once_control == PTHREAD_ONCE_INIT) {
	  init_routine();
	  *once_control = PTHREAD_ONCE_INIT + 1;
	}
	
	pthread_mutex_unlock(&once_lock);
	
	return 0;
}
#endif

static inline int
pthread_once(pthread_once_t *once, void (*cb) (void))
{
	cb();
	return 0;
#if 0
	BOOL rc = InitOnceExecuteOnce(&once->once, _pthread_once_win32_cb, cb, NULL);
	if (rc == 0)
		return -1;
	else
		return 0;
#endif
}

struct pthread {
	HANDLE handle;
};
typedef struct pthread pthread_t;

static inline pthread_t
pthread_self(void)
{
	pthread_t self;
	self.handle = GetCurrentThread();
	return self;
}

static inline int
pthread_equal(pthread_t t1, pthread_t t2)
{
	return t1.handle == t2.handle;
}

#else
#include_next <pthread.h>
#endif
