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


/***
 * AccessAllowIPか否かの判定用関数.
 */
typedef bool (*MoaiFdSetFuncT_IsAccessAllowIP)( ZnkSocket, uint32_t* ipaddr_ans );

MoaiFdSet
MoaiFdSet_create( ZnkSocket listen_sock, struct ZnkTimeval* waitval );
MoaiFdSet
MoaiFdSet_create_ex( ZnkSocketAry listen_sockary, struct ZnkTimeval* waitval );

void
MoaiFdSet_destroy( MoaiFdSet mfds );

ZnkFdSet
MoaiFdSet_fdst_observe_r( MoaiFdSet mfds );
ZnkFdSet
MoaiFdSet_fdst_observe_w( MoaiFdSet mfds );

ZnkSocketAry
MoaiFdSet_wk_sock_ary( MoaiFdSet mfds );

void
MoaiFdSet_reserveConnectSock( MoaiFdSet mfds, ZnkSocket sock );
void
MoaiFdSet_addConnectingSock( MoaiFdSet mfds, ZnkSocket sock );
void
MoaiFdSet_removeConnectingSock( MoaiFdSet mfds, ZnkSocket sock );
void
MoaiFdSet_procConnectionTimeout( MoaiFdSet mfds );

int
MoaiFdSet_select( MoaiFdSet mfds, bool* req_before_report, MoaiFdSetFuncArg_Report* fnca_report );

MoaiRASResult
MoaiFdSet_process( MoaiFdSet mfds,
		MoaiFdSetFuncArg_OnAccept* fnca_on_accept,
		MoaiFdSetFuncArg_RAS* fnca_ras,
		MoaiFdSetFuncT_IsAccessAllowIP is_access_allow_ip );

void
MoaiFdSet_printf_fdst_read( MoaiFdSet mfds );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
