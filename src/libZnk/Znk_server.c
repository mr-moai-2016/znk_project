#include "Znk_server.h"
#include "Znk_socket.h"
#include "Znk_stdc.h"


#if defined(Znk_TARGET_WINDOWS)
#  include <winsock2.h>

#elif defined(Znk_TARGET_UNIX)
#  include <sys/socket.h> /* for socket,connect,bind,listen,accept,shutdown,recv,send etc. */
#  include <unistd.h>     /* for close */
#  include <arpa/inet.h>  /* for inet_* function (inet_addr etc) */
#  include <netdb.h>      /* for gethostbyname, getservbyname */
#  include <netinet/in.h> /* for struct sockaddr_in, htons etc. */

#else
#endif

#include <string.h>
#include <time.h>


struct ZnkServerImpl {
	int	               port_;
	ZnkSocket          listen_sock_;
	time_t             start_time_;
	char*              acceptable_host_;
	struct sockaddr_in addr_;
};

ZnkServer
ZnkServer_create( const char* acceptable_host, int port )
{
	ZnkServer server;
	ZnkSocket sock;
	int       opt;

	server = Znk_malloc( sizeof(struct ZnkServerImpl) );
	if( server == NULL ) return NULL;

	memset(server, 0, sizeof(struct ZnkServerImpl));
	server->port_ = port;
	server->acceptable_host_ = ( acceptable_host == NULL ) ? NULL : strdup(acceptable_host);

	sock = ZnkSocket_open();
	if( sock < 0 ){
		Znk_free(server);
		return NULL;
	}

	opt = 1;
	setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(int) );

	server->listen_sock_ = sock;
	memset(&server->addr_, 0, sizeof(server->addr_));
	server->addr_.sin_family = AF_INET;
	if( server->acceptable_host_ == NULL ){
		server->addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		server->addr_.sin_addr.s_addr = inet_addr(server->acceptable_host_);
	}
	server->addr_.sin_port = htons((u_short)server->port_);
	if( bind(sock, (struct sockaddr *)&server->addr_, sizeof(server->addr_)) < 0 ){
		ZnkSocket_close( sock );
		Znk_free(server);
		return NULL;
	}
	listen(sock, 128);
	server->start_time_ = time(NULL);
	return server;
}

void
ZnkServer_destroy( ZnkServer server )
{
	if( server ){
		if( server->acceptable_host_ ){
			Znk_free(server->acceptable_host_);
		}
		Znk_free(server);
	}
}

ZnkSocket
ZnkServer_getListenSocket( const ZnkServer server )
{
	return server->listen_sock_;
}

