#ifndef INCLUDE_GUARD__Moai_connection_h__
#define INCLUDE_GUARD__Moai_connection_h__

#include <Znk_socket.h>
#include <Znk_htp_hdrs.h>

Znk_EXTERN_C_BEGIN

/* DNS関連のRFCであるRFC1035によれば、ドメイン名は最大でも255オクテット以下とある.
 * 従って、ここではhostnameのバッファサイズとして256あれば十分 */
#define MOAI_HOSTNAME_MAX 256  
/* IPV6も含めれば64もあれば十分 */
#define MOAI_IPADDR_MAX 64  

typedef enum {
	 MoaiSockType_e_None=0 /* 未接続状態など */
	,MoaiSockType_e_Listen /* リスニングソケット */
	,MoaiSockType_e_Outer  /* 外部サイトへの接続ソケット */
	,MoaiSockType_e_Inner  /* ローカルホスト内のブラウザからのAcceptソケット */
} MoaiSockType;

typedef struct MoaiConnection_tag* MoaiConnection;
struct MoaiConnection_tag {
	char            hostname_[ MOAI_HOSTNAME_MAX ]; /* O側ホスト名 */
	//char            ipaddr_[ MOAI_IPADDR_MAX ];     /* O側IP アドレス */
	uint16_t        port_;                          /* O側との通信に用いるport */
	ZnkSocket       src_sock_;      /* keyとなるsock */
	MoaiSockType    sock_type_;     /* src_sockのタイプ */
	bool            is_keep_alive_; /* src_sockがkeep-aliveであるか否か */
	ZnkSocket       dst_sock_;      /* tunnelingモードである場合のdst_sock */
	size_t          content_length_remain_;
	bool            as_local_proxy_;
	//ZnkStr          req_uri_;
	ZnkStr          req_urp_; /* Request URL Path : URLにおけるオーソリティの終了以降の/で始まる部分を示す */
	ZnkHtpReqMethod req_method_;
	uint64_t        exile_time_;
	uint64_t        waiting_;
	/* TODO:timeoutすると自動的にcloseするための情報も追加したい */
};

void
MoaiConnection_initiate( void );

void
MoaiConnection_clear( MoaiConnection htcn );

MoaiConnection
MoaiConnection_find( const ZnkSocket query_sock );
MoaiConnection
MoaiConnection_find_dst_sock( const ZnkSocket query_sock );

MoaiConnection
MoaiConnection_intern( ZnkSocket sock, MoaiSockType sock_type );

void
MoaiConnection_clear_bySock( ZnkSocket sock );

void
MoaiConnection_clearAll( void );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
