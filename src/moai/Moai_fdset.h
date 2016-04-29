#ifndef INCLUDE_GUARD__Moai_fdset_h__
#define INCLUDE_GUARD__Moai_fdset_h__

#include <Moai_type.h>
#include <Znk_fdset.h>

Znk_EXTERN_C_BEGIN

typedef struct MoaiFdSetImpl* MoaiFdSet;

/***
 * 報告用コールバック.
 */
typedef void (*MoaiFdSetFuncT_Report)( MoaiFdSet, void* arg );
typedef struct {
	MoaiFdSetFuncT_Report func_;
	void*                 arg_;
} MoaiFdSetFuncArg_Report;

/***
 * 新規接続要求が発生した場合(換言すればaccept sockが生成された場合)に呼び出すコールバック.
 */
typedef void (*MoaiFdSetFuncT_OnAccept)( MoaiFdSet, ZnkSocket, void* arg );
typedef struct {
	MoaiFdSetFuncT_OnAccept func_;
	void*                   arg_;
} MoaiFdSetFuncArg_OnAccept;

/***
 * RAS(RecvAndSend)用コールバック.
 */
typedef MoaiRASResult (*MoaiFdSetFuncT_RAS)( MoaiFdSet, ZnkSocket, void* arg );
typedef struct {
	MoaiFdSetFuncT_RAS func_;
	void*              arg_;
} MoaiFdSetFuncArg_RAS;


MoaiFdSet
MoaiFdSet_create( ZnkSocket listen_sock, struct ZnkTimeval* waitval );

void
MoaiFdSet_destroy( MoaiFdSet mfds );

ZnkFdSet
MoaiFdSet_fdst_observe( MoaiFdSet mfds );

ZnkSocketDAry
MoaiFdSet_wk_sock_dary( MoaiFdSet mfds );

void
MoaiFdSet_reserveConnectSock( MoaiFdSet mfds, ZnkSocket sock );

int
MoaiFdSet_select( MoaiFdSet mfds, bool* req_before_report, MoaiFdSetFuncArg_Report* fnca_report );

MoaiRASResult
MoaiFdSet_process( MoaiFdSet mfds,
		MoaiFdSetFuncArg_OnAccept* fnca_on_accept, MoaiFdSetFuncArg_RAS* fnca_ras );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
