#include "Znk_thread.h"
#include "Znk_bfr.h"
#include "Znk_stdc.h"
#include "Znk_mutex.h"

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

#else
/* Unix というか pthread を使った実装 */
#  include <pthread.h>
#  include <unistd.h> /* for usleep */
#  define M_PTHREAD_MUTEX_INIT( mutex ) pthread_mutex_init( mutex, 0 );

#endif

#include <assert.h>

void
ZnkThread_sleep( size_t msec )
{
#if defined(Znk_TARGET_WINDOWS)
	Sleep( (DWORD)msec );
#else
	usleep( msec * 1000 ); /* 指定はマイクロ秒 */
#endif
}


typedef struct ZnkThreadInfoImpl* ZnkThreadInfo;
struct ZnkThreadInfoImpl {
	ZnkThreadFunc   func_;
	void*           arg_;
	ZnkThreadHandle handle_;
	uintptr_t       id_;
	size_t          thno_;
	bool            active_;
};

static ZnkBfr st_pair_list = NULL;

struct znThreadListElem {
	ZnkThreadInfo info_;
	bool          used_;
};
static size_t
znThreadList_getListNum( ZnkBfr thlist )
{
	return ZnkBfr_size( thlist ) / sizeof( struct znThreadListElem );  
}
static ZnkBfr
znThreadList_theList( void )
{
	if( st_pair_list == NULL ){
		st_pair_list = ZnkBfr_create_null();
	}
	return st_pair_list;
}
static void znThreadList_deleteList()
{
	if( st_pair_list ){
		ZnkBfr_destroy( st_pair_list );
		st_pair_list = NULL;
	}
}

static ZnkMutex*
znThreadList_theMutex( void )
{
	static ZnkMutex st_mutex = NULL;
	return &st_mutex;
}
Znk_INLINE bool
znThreadList_getElem( size_t thno, struct znThreadListElem* ans )
{
	bool result = false;
	ZnkMutex mtx = *(znThreadList_theMutex());
	ZnkMutex_lock( mtx );
	{
		ZnkBfr thlist = znThreadList_theList();
		const size_t num = znThreadList_getListNum( thlist );
		struct znThreadListElem* data = (struct znThreadListElem*)ZnkBfr_data( thlist );
		if( thno < num ){
			*ans = data[ thno ];
			result = true;
		}
	}
	ZnkMutex_unlock( mtx );
	return result;
}
Znk_INLINE ZnkThreadInfo
znThreadList_getInfo( size_t thno )
{
	/* まずelemをdeepコピーで取り出す */
	struct znThreadListElem elem = { NULL, false };
	znThreadList_getElem( thno, &elem );
	return elem.info_;
}
Znk_INLINE bool
znThreadList_used( size_t thno )
{
	/* まずelemをdeepコピーで取り出す */
	struct znThreadListElem elem = { NULL, false };
	znThreadList_getElem( thno, &elem );
	return elem.used_;
}
Znk_INLINE bool
znThreadList_setUsed( size_t thno, bool used )
{
	bool result = false;
	ZnkMutex mtx = *(znThreadList_theMutex());
	ZnkMutex_lock( mtx );
	{
		ZnkBfr thlist = znThreadList_theList();
		const size_t num = znThreadList_getListNum( thlist );
		struct znThreadListElem* data = (struct znThreadListElem*)ZnkBfr_data( thlist );
		if( thno < num ){
			data[ thno ].used_ = used;
			result = true;
		}
	}
	ZnkMutex_unlock( mtx );
	return result;
}

Znk_INLINE void
allocInternal( ZnkBfr thlist, size_t pos, size_t num )
{
	const size_t size = num * sizeof(struct znThreadListElem);
	struct znThreadListElem* data = (struct znThreadListElem*)ZnkBfr_data( thlist );
	struct znThreadListElem* elemp;
	size_t idx;
	const size_t end = pos + num;
	ZnkBfr_fill( thlist, pos * sizeof(struct znThreadListElem), 0, size );
	for( idx=pos; idx<end; ++idx ){
		elemp = data + idx;
		elemp->info_ = (ZnkThreadInfo)Znk_malloc( sizeof( struct ZnkThreadInfoImpl ) );
		elemp->info_->thno_ = idx;
		elemp->info_->handle_ = NULL;
		elemp->info_->func_   = NULL;
		elemp->info_->arg_    = NULL;
		elemp->info_->id_     = 0;
		elemp->info_->active_ = false;
	}
}
/***
 * この関数は、lockは掛かっている状態で呼ばれることを前提とする.
 */
static ZnkThreadInfo
issueInfoInteranal( size_t* thno )
{
	static size_t begin = 0;
	ZnkBfr thlist = znThreadList_theList();
	const size_t num = znThreadList_getListNum( thlist );
	struct znThreadListElem* elemp = NULL;
	size_t idx;
	struct znThreadListElem* data = (struct znThreadListElem*)ZnkBfr_data( thlist );

	/***
	 * beginの位置はサイクリックに移動させる.
	 */
	if( begin >= num ){ begin = num; }
	for( idx=begin; idx<num; ++idx ){
		elemp = data + idx;
		if( !elemp->used_ ){
			/* 現在未使用 */
			begin = idx; /* 次回検索を高速化するためのヒューリスティック */
			*thno = idx;
			elemp->used_ = true; /* 使用中に切り替える */
			return elemp->info_;
		}
	}
	for( idx=0; idx<begin; ++idx ){
		elemp = data + idx;
		if( !elemp->used_ ){
			/* 現在未使用 */
			begin = idx; /* 次回検索を高速化するためのヒューリスティック */
			*thno = idx;
			elemp->used_ = true; /* 使用中に切り替える */
			return elemp->info_;
		}
	}

	/***
	 * 用意していたThreadの定員数がオーバーした.
	 * この状態はかなりイレギュラーである.
	 * 本来ならばこのような状況が発生しないようにinit時に与えるnumを十分に用意すべきである.
	 * ここでは、num をそれまでの 2 倍に増やす.
	 */
	ZnkBfr_resize( thlist, 2 * num * sizeof(struct znThreadListElem) );
	allocInternal( thlist, num, num );
	data = (struct znThreadListElem*)ZnkBfr_data( thlist );
	*thno = num;
	elemp = data + num;
	elemp->used_ = true; /* 使用中に切り替える */
	return elemp->info_;
}
/***
 * 現在空きのあるリスト位置をシークしてその領域へのポインタを返す.
 * この関数は内部でlockを掛け、しかも探索を行う.
 * 一見時間がかかりそうであるが、実のところ探索の開始値をサイクリックに変動
 * させているため、Listの要素数が十分であれば、実質この探索は一瞬で済むと期待される.
 */
Znk_INLINE ZnkThreadInfo
znThreadList_issueInfo( size_t* thno )
{
	ZnkThreadInfo ans;
	ZnkMutex mtx = *(znThreadList_theMutex());
	ZnkMutex_lock( mtx );
	ans = issueInfoInteranal( thno );
	ZnkMutex_unlock( mtx );
	return ans;
}

/***
 * この時点で子スレッドは存在しないものと仮定する.
 * 通常のアプリであれば numは128くらいあれば十分であろう.
 * この値の指定に上限はない(単にListのメモリサイズに影響するのみで
 * 大きな値を指定してもたかが知れている).
 * 実際に使用する数よりもやや多めに指定した方がよい.
 * ぎりぎりくらいに設定するとissue処理の際、探索に時間が
 * かかる可能性が高くなってしまう.
 */
void
ZnkThreadList_initiate( size_t num )
{
	ZnkBfr thlist;
	ZnkMutex* mtxp = znThreadList_theMutex();
	if( *mtxp == NULL ){
		*mtxp = ZnkMutex_create();
	}
	thlist = znThreadList_theList();
	ZnkBfr_resize( thlist, num * sizeof( struct znThreadListElem ) );
	allocInternal( thlist, 0, num );
}
void
ZnkThreadList_finalize( void )
{
	/***
	 * この時点で子スレッドは存在しないものと仮定する.
	 */
	ZnkMutex* mtxp;
	size_t idx;
	ZnkBfr thlist = znThreadList_theList();
	const size_t num = znThreadList_getListNum( thlist );
	for( idx=0; idx<num; ++idx ){
		ZnkThreadInfo info = znThreadList_getInfo( idx );
		if( info == NULL ){ continue; }
		Znk_free( info );
	}

	znThreadList_deleteList();

	mtxp = znThreadList_theMutex();
	Znk_DELETE_PTR( *mtxp, ZnkMutex_destroy, NULL );
}

Znk_INLINE size_t
findNo_bySysId( uintptr_t sys_id )
{
	ZnkMutex mtx = *(znThreadList_theMutex());
	ZnkMutex_lock( mtx );
	{
		ZnkBfr thlist = znThreadList_theList();
		const size_t num = znThreadList_getListNum( thlist );
		size_t i;
		struct znThreadListElem* data = (struct znThreadListElem*)ZnkBfr_data( thlist );
		struct znThreadListElem* elemp = NULL;
		for( i=0; i<num; ++i ){
			elemp = data + i;
			if( elemp->info_->id_ == sys_id ){
				ZnkMutex_unlock( mtx );
				return i; /* found */
			}
		}
	}
	ZnkMutex_unlock( mtx );
	return Znk_NPOS;
}
Znk_INLINE size_t
findNo_bySysHandle( ZnkThreadHandle handle )
{
	ZnkMutex mtx = *(znThreadList_theMutex());
	ZnkMutex_lock( mtx );
	{
		ZnkBfr thlist = znThreadList_theList();
		const size_t num = znThreadList_getListNum( thlist );
		size_t i;
		struct znThreadListElem* data = (struct znThreadListElem*)ZnkBfr_data( thlist );
		struct znThreadListElem* elemp = NULL;
		for( i=0; i<num; ++i ){
			elemp = data + i;
			if( elemp->info_->handle_ == handle ){
				ZnkMutex_unlock( mtx );
				return i; /* found */
			}
		}
	}
	ZnkMutex_unlock( mtx );
	return Znk_NPOS;
}
Znk_INLINE ZnkThreadHandle
findHandle_bySysId( uintptr_t sys_id )
{
	ZnkMutex mtx = *(znThreadList_theMutex());
	ZnkMutex_lock( mtx );
	{
		ZnkBfr thlist = znThreadList_theList();
		const size_t num = znThreadList_getListNum( thlist );
		size_t i;
		struct znThreadListElem* data = (struct znThreadListElem*)ZnkBfr_data( thlist );
		struct znThreadListElem* elemp = NULL;
		for( i=0; i<num; ++i ){
			elemp = data + i;
			if( elemp->info_->id_ == sys_id ){
				ZnkMutex_unlock( mtx );
				return elemp->info_->handle_; /* found */
			}
		}
	}
	ZnkMutex_unlock( mtx );
	return NULL;
}

size_t ZnkThread_getCurrentNo( void )
{
#if defined(Znk_TARGET_WINDOWS)
  	const uintptr_t sys_id = (uintptr_t)GetCurrentThreadId();
	return findNo_bySysId( sys_id );
#elif defined(Znk_TARGET_UNIX)
	const ZnkThreadHandle handle = (ZnkThreadHandle)pthread_self();
	return findNo_bySysHandle( handle );
#endif
}

/***
 * ZnkThread_create
 *
 * 本来はactive_がfalseになる時点でhandleをクローズしたいところだが、
 * そうするとwaitEnd系の関数が使えなくなってしまう可能性がある.
 * WinAPIやpthreadが提供するwaitEnd系関数は有効なhandleを引数として
 * 要求するため、ThreadFuncが自殺的にこれをクローズしてしまうと、
 * これらのwaitEnd系関数が未定義の動作を引き起こす可能性がある.
 * (実際、pthread_joinでは、これが２重に呼ばれた場合の動作は未定義である)
 *
 * そこで、これをクローズするタイミングをwaitEnd内と新規にThreadを作る
 * create関数内に限定してある. また、ThreadInfo内には以前のThreadハンドルを
 * あえてそのまま残すポリシーとし、クローズした場合はその値をNULLで上書き
 * するものとする.
 */

/***
 * createThread によって呼ばれる内部関数.
 * 事実上のThreadのEntry関数と考えてよい.
 */
static void*
Znk__Internal__ThreadFunc( void* internal_info )
{
	ZnkThreadInfo thinfo = (ZnkThreadInfo)internal_info;
	void* result;
	/* createThread関数の終了とactive_フラグが確実に立つのを待つ.
	 * active_ が立つことで、thinfo の必要な初期化はすべて完了したとみなす. */
	while( !thinfo->active_ ){ ZnkThread_sleep( 1 ); }
	/* この時点で active_ が true であることが保証される */
	result = (*thinfo->func_)( thinfo->arg_ );
	thinfo->active_ = false;
	znThreadList_setUsed( thinfo->thno_, false );
	return result;
}

#if defined(Znk_TARGET_WINDOWS)

static unsigned int __stdcall
Znk__Internal__ThreadFunc_stdcall( void* internal_info )
{
	/***
	 * VCの警告C4311は64bit版ならばわかるが、32bit版では無意味.
	 */
#  if (_MSC_VER >= 1300) && defined(_WIN32)
#    pragma warning(push)
#    pragma warning(disable:4311)
#  endif
	void* result = Znk__Internal__ThreadFunc( internal_info );
	return Znk_force_ptr_cast( unsigned int, result );
#  if (_MSC_VER >= 1300) && defined(_WIN32)
#    pragma warning(pop)
#  endif
}
static ZnkThreadHandle
createThread( void* internal_info, uintptr_t* sys_id )
{
	/***
	 * _beginthread に与える関数の呼び出し規約は __cdecl なのに対して
	 * _beginthreadex に与える関数の呼び出し規約は __stdcall でなければならないことに注意する.
	 */
	unsigned int thread_id = 0;
	ZnkThreadHandle handle = (ZnkThreadHandle)_beginthreadex( NULL, 0,
			Znk__Internal__ThreadFunc_stdcall, internal_info,
			0,
			&thread_id );
	if( sys_id ){ *sys_id = thread_id; }
	return handle;
}
Znk_INLINE int
closeHandle( ZnkThreadInfo thinfo )
{
	int result = 0;
	CloseHandle( (HANDLE)thinfo->handle_ );
	thinfo->handle_ = NULL;
	return result;
}
Znk_INLINE int waitEnd( ZnkThreadInfo thinfo )
{
	int result = (int)WaitForSingleObject( (HANDLE)thinfo->handle_, INFINITE ); 
	closeHandle( thinfo );
	return result;
}

#elif defined(Znk_TARGET_UNIX)

static ZnkThreadHandle
createThread( void* internal_info, uintptr_t* sys_id )
{
	pthread_t handle;
	pthread_create( &handle, NULL, Znk__Internal__ThreadFunc, internal_info );
	if( sys_id ){ *sys_id = 0; /* nouse */ }
	return (ZnkThreadHandle)handle;
}
Znk_INLINE int
closeHandle( ZnkThreadInfo thinfo )
{
	int result = 0;
    void* thread_result;
	result = pthread_join( (pthread_t)thinfo->handle_, &thread_result );
	thinfo->handle_ = NULL;
	return result;
}
Znk_INLINE int
waitEnd( ZnkThreadInfo thinfo )
{
	return closeHandle( thinfo );
}

#endif

size_t
ZnkThread_create( ZnkThreadFunc func, void* arg )
{
	uintptr_t thread_id = 0;
	size_t thno = Znk_NPOS;
	ZnkThreadInfo thinfo;
	ZnkMutex* mtx = znThreadList_theMutex();
	if( *mtx == NULL ){
		/* まだ初期化されていない */
		ZnkThreadList_initiate( 128 );
	}

	thinfo = znThreadList_issueInfo( &thno );

	/***
	 * 以下、thinfo は used_ が true になっているため、
	 * 他のスレッドがこの内容を書き換えることはないはず.
	 */
	assert( func );
	thinfo->func_ = func;
	thinfo->arg_  = arg;
	if( thinfo->handle_ ){
		/* 過去の残骸が存在する. */
		closeHandle( thinfo );
	}

	thinfo->handle_ = createThread( thinfo, &thread_id );
	thinfo->id_ = thread_id;
	thinfo->active_ = true;
	/* この時点で active_ が true であることが保証される */
	return thno;
}

int
ZnkThread_waitEnd( size_t thno )
{
	const size_t current_thno = ZnkThread_getCurrentNo();
	if( current_thno != thno ){
		/***
		 * 現在のスレッドと同じ番号が指定された場合は除外する.
		 * (終了するはずがないので)
		 */
		int result = 0;
		ZnkThreadInfo thinfo = znThreadList_getInfo( thno );
		if( thinfo && thinfo->handle_ ){
			result = waitEnd( thinfo );
		}
		return result;
	}
	return 0;
}

/***
 * すべての子スレッドが終わるのを待つ.
 * この関数は通常アプリ終了時にメインスレッドで呼び出すことを想定したものであるが、
 * 子スレッドで呼び出すこともできなくはない. その場合、その子スレッドとメインスレッド
 * を除く他すべての子スレッドの終了待ちを行うことになる.
 */
void
ZnkThread_waitEndAll( void )
{
	ZnkBfr thlist = znThreadList_theList();
	const size_t num = znThreadList_getListNum( thlist );
	size_t idx;
	for( idx=0; idx<num; ++idx ){
		ZnkThread_waitEnd( idx );
	}
}

ZnkThreadHandle
ZnkThread_getHandle( size_t thno )
{
	ZnkThreadInfo thinfo = znThreadList_getInfo( thno );
	/* activeではない場合、そのハンドル値はもはや有効ではないと考える. */
	if( thinfo == NULL || !thinfo->active_ ){ return NULL; }
	return thinfo->handle_;
}

bool
ZnkThread_terminateForce( size_t thno )
{
#if defined(Znk_TARGET_WINDOWS)
	HANDLE handle = (HANDLE)ZnkThread_getHandle( thno );
	if( handle ){
		TerminateThread( handle, 0 );
		return true;
	}
#endif
	return false;
}

