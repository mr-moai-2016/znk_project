#ifndef INCLUDE_GUARD__Moai_connection_h__
#define INCLUDE_GUARD__Moai_connection_h__

#include <Znk_socket.h>
#include <Znk_htp_hdrs.h>
#include <Znk_fdset.h>
#include "Moai_info.h"
#include "Moai_fdset.h"

Znk_EXTERN_C_BEGIN

/***
 * DNS名:
 * DNS関連のRFCであるRFC1035によれば、ドメイン名は最大でも255オクテット以下とある.
 *
 * IP:
 * IPV6も含めれば64もあれば十分.
 */

typedef enum {
	 MoaiSockType_e_None=0 /* 未接続状態など */
	,MoaiSockType_e_Listen /* リスニングソケット */
	,MoaiSockType_e_Outer  /* 外部サイトへの接続ソケット */
	,MoaiSockType_e_Inner  /* ローカルホスト内のブラウザからのAcceptソケット */
} MoaiSockType;

typedef struct MoaiConnection_tag* MoaiConnection;
typedef bool (*MoaiConnectedCallback)( MoaiConnection mcn, MoaiFdSet mfds, MoaiInfoID );

struct MoaiConnection_tag {
	ZnkStr          hostname_;        /* ゴールとなるホスト名(Proxyではない) */
	uint16_t        port_;            /* ゴールとなるホストとの通信に用いるport(Proxyではない) */
	ZnkSocket       I_sock_;          /* I側sock */
	bool            I_is_keep_alive_; /* I側sockがkeep-aliveであるか否か */
	ZnkSocket       O_sock_;          /* O側sock */
	/***
	 * Pipelineに対応するためには、Inner方向とOuter方向の通信用に
	 * content_length_remain_をそれぞれ独立して用意する必要がある.
	 */
	size_t          req_content_length_remain_;
	size_t          res_content_length_remain_;
	uint64_t        exile_time_;
	bool            is_connect_inprogress_;
	uint64_t        connect_begin_time_;
	MoaiConnectedCallback cb_func_;

	/***
	 * HTTP Pipeline化に対応.
	 */
	ZnkBfr          info_id_list_;
	ZnkBfr          invoked_info_id_list_;
	size_t          response_idx_;
};


void
MoaiConnection_initiate( void );

void
MoaiConnection_setListeningSock( ZnkSocket L_sock );
bool
MoaiConnection_isListeningSock( ZnkSocket query_sock );
void
MoaiConnection_setBlockingMode( bool blocking_mode );
bool
MoaiConnection_isBlockingMode( void );

MoaiConnection
MoaiConnection_regist( const char* hostname, uint16_t port,
		ZnkSocket I_sock, ZnkSocket O_sock, MoaiFdSet mfds );

MoaiConnection
MoaiConnection_connectO( const char* goal_hostname, uint16_t goal_port,
		const char* cnct_hostname, uint16_t cnct_port,
		ZnkSocket I_sock, MoaiFdSet mfds );
bool
MoaiConnection_connectFromISock( const char* hostname, uint16_t port, ZnkSocket I_sock, const char* label,
		MoaiInfoID info_id, MoaiFdSet mfds, bool is_proxy_use, MoaiConnectedCallback cb_func );

const char*
MoaiConnection_hostname( const MoaiConnection mcn );
uint16_t
MoaiConnection_port( const MoaiConnection mcn );
ZnkSocket
MoaiConnection_I_sock( const MoaiConnection mcn );
ZnkSocket
MoaiConnection_O_sock( const MoaiConnection mcn );
void
MoaiConnection_pushConnectedEvent( MoaiConnection mcn, MoaiConnectedCallback cb_func, MoaiInfoID info_id );
bool
MoaiConnection_invokeCallback( MoaiConnection mcn, MoaiFdSet mfds );

bool
MoaiConnection_isConnectInprogress( const MoaiConnection mcn );
void
MoaiConnection_setConnectInprogress( MoaiConnection mcn, bool is_connect_inprogress );

void
MoaiConnection_clear( MoaiConnection mcn, MoaiFdSet mfds );

MoaiConnection
MoaiConnection_find_byISock( const ZnkSocket query_sock );
MoaiConnection
MoaiConnection_find_byOSock( const ZnkSocket query_sock );

Znk_INLINE void
MoaiConnection_clear_byISock( ZnkSocket I_sock, MoaiFdSet mfds ){
	MoaiConnection mcn = MoaiConnection_find_byISock( I_sock );
	MoaiConnection_clear( mcn, mfds );
}
Znk_INLINE void
MoaiConnection_clear_byOSock( ZnkSocket O_sock, MoaiFdSet mfds ){
	MoaiConnection mcn = MoaiConnection_find_byOSock( O_sock );
	MoaiConnection_clear( mcn, mfds );
}

void
MoaiConnection_clearAll( MoaiFdSet mfds );

void
MoaiConnection_erase( MoaiConnection mcn, MoaiFdSet mfds );

MoaiInfo*
MoaiConnection_getInvokedInfo( const MoaiConnection mcn, size_t idx );
size_t
MoaiConnection_countResponseIdx( MoaiConnection mcn );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
