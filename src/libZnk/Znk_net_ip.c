#include <Znk_net_ip.h>
#include <Znk_stdc.h>

#if defined(Znk_TARGET_WINDOWS)
/***
 * for Windows.
 * How to get the local network addresses
 * 1. enumerate local network adapters
 * 2. get local IPv4 address
 * 3. get local IPv6 address (requires WinXP later)
 */
#  include <winsock2.h>
#  include <ws2tcpip.h>
/***
 * これらはDMCでは存在しない.
 */
//#  include <iptypes.h>
//#  include <iphlpapi.h>

#else
/***
 * for Linux.
 */
#  include <stdio.h>
#  include <string.h>
#  include <unistd.h>
#  include <sys/ioctl.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <net/if.h>
#  include <arpa/inet.h> /* for inet_ntoa() */

#endif


#if defined(Znk_TARGET_WINDOWS)

#if 0
/***
 * Do almost the same task as ipconfig /all
 * GetAdaptersInfoがDMCでは提供されていないため、
 * この方法は却下とする.
 */
static void
enum_local_nic( void )
{
	IP_ADAPTER_INFO  adapterInfo;
	PIP_ADAPTER_INFO pAdapterInfo = &adapterInfo;
	PIP_ADAPTER_INFO pAdapterInfo_hp = NULL;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	DWORD dwRetVal;

	/* Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable */
	dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen );
	if( dwRetVal == ERROR_BUFFER_OVERFLOW ){
		pAdapterInfo_hp = Znk_malloc( ulOutBufLen );
		pAdapterInfo = pAdapterInfo_hp;
		dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen );
	}

	if( dwRetVal == NO_ERROR ){
		PIP_ADAPTER_INFO pAdapter;
		for( pAdapter = pAdapterInfo; pAdapter; pAdapter = pAdapter->Next ){
			/* Take care of only the first IP Address */
			ZnkF_printf_e( "IpAddress=[%s]\n",   pAdapter->IpAddressList.IpAddress.String );
			ZnkF_printf_e( "IpMask=[%s]\n",      pAdapter->IpAddressList.IpMask.String );
			ZnkF_printf_e( "AdapterName=[%s]\n", pAdapter->AdapterName );
			ZnkF_printf_e( "Description=[%s]\n", pAdapter->Description );
#if 0
			{
				char *physic = Znk_malloc(3 * pAdapter->AddressLength);
				char *pt = physic;
				UINT i;
				sprintf(pt, "%02X", pAdapter->Address[0]);
				pt += 2;
				for (i = 1; i < pAdapter->AddressLength; i++) {
					sprintf(pt, "-%02X", pAdapter->Address[i]);
					pt += 3;
				}
				ZnkF_printf_e("physical address: %s\n", physic);
				Znk_free( physic );
			}
#endif
		}
	}
	Znk_DELETE_PTR( pAdapterInfo_hp, Znk_free, NULL );
}
#endif

static bool
isUnavailAddr( const struct sockaddr_in *sa )
{
	if (sa->sin_addr.s_addr == 0) {
		return true;
	}
	if (sa->sin_family == AF_INET) {
		/* see rfc3330 */
		if (sa->sin_addr.s_net == 0) {
			return true;
		}
		/* @see rfc3330 (link local) */
		if( sa->sin_addr.S_un.S_un_b.s_b1 == 169 && sa->sin_addr.S_un.S_un_b.s_b2 == 254 ){
			return true;
	    }
	}
	return false;
}

/***
 * http://tangentsoft.net/wskfaq/intermediate.html#getipaddr
 * This need to call WSAStartup() previously.
 */
bool
ZnkNetIP_getPrivateIP( char* ipaddr, size_t ipaddr_size )
{
	INTERFACE_INFO if_list[ 20 ];
	unsigned long ret_bytes;
	int num_if;
	int n;
	bool ret = false;
	
	SOCKET sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
	if (sd == SOCKET_ERROR) {
		return false;
	}
	
	if( WSAIoctl(sd, SIO_GET_INTERFACE_LIST, NULL, 0, &if_list,
				sizeof(if_list), &ret_bytes, NULL, NULL) == SOCKET_ERROR )
	{
		closesocket(sd);
		return false;
	}
	closesocket(sd);
	
	num_if = ret_bytes / sizeof(INTERFACE_INFO);
	
	/* output for debug */
	ZnkF_printf_e("IFF_UP=[%08x]\n", IFF_UP );
	ZnkF_printf_e("IFF_LOOPBACK=[%08x]\n", IFF_LOOPBACK );
	for( n = 0; n < num_if; ++n ){
		struct sockaddr_in *sa = (struct sockaddr_in*)&(if_list[n].iiAddress);
		if (sa->sin_family == AF_INET) {
			const char* addr_str = inet_ntoa(sa->sin_addr);
			ZnkF_printf_e("Debug : address=[%s] iiFlags=[%08x]\n", addr_str, if_list[n].iiFlags);
		}
	}
	
	/* select better address */
	for (n = 0; n < num_if; ++n) {
		struct sockaddr_in *sa;
		if( !(if_list[n].iiFlags & IFF_UP) ){
			continue;
		}
		if( if_list[n].iiFlags & IFF_LOOPBACK ){
			/* If there is a loopback address only, use it */
			if( n != num_if - 1 ){
				continue;
			}
		}
		sa = (struct sockaddr_in*) &(if_list[n].iiAddress);
		if( isUnavailAddr(sa) ){
			continue;
		}
		switch (sa->sin_family) {
		case AF_INET:
			ZnkF_printf_e( "IpAddress=[%s]\n", inet_ntoa(sa->sin_addr) );
			ZnkF_printf_e( "IpMask=[%s]\n", inet_ntoa(((struct sockaddr_in*)&if_list[n].iiNetmask)->sin_addr) );
			ZnkS_copy( ipaddr, ipaddr_size, inet_ntoa(sa->sin_addr), Znk_NPOS );
			ret = true;
			goto FUNC_END;
		case AF_INET6:
			/* TODO */
			goto FUNC_END;
		}
	}
FUNC_END:
	return ret;
}

#if 0
static int
get_local_address_v6( void )
{
	DWORD ret;
	IP_ADAPTER_ADDRESSES addr[256];/* XXX */
	ULONG len = sizeof(addr);
	
	ret = GetAdaptersAddresses(AF_INET6, 0, NULL, addr, &len);
	if (ret == ERROR_SUCCESS) {
		IP_ADAPTER_ADDRESSES *padap = &addr[0];
		do {
			IP_ADAPTER_UNICAST_ADDRESS *uni = padap->FirstUnicastAddress;
			if (!uni) {
				continue;
			}
			do {
				SOCKET_ADDRESS addr = uni->Address;
				struct sockaddr_in6 *sa;
				if (!(uni->Flags & IP_ADAPTER_ADDRESS_DNS_ELIGIBLE)) {
					continue;
				}
				sa = (struct sockaddr_in6*)addr.lpSockaddr;
#if 0
				assert(sa->sin6_family == AF_INET6);
				int sz = 40;
				*ret_ip = apr_malloc(mp, sz);
				apr_inet_ntop(sa->sin6_family, &sa->sin6_addr, (char*)*ret_ip, sz);
#endif
				return TRUE;
			} while ((uni = uni->Next));
		} while ((padap = padap->Next));
	}
	return FALSE;
}
#endif

void
ZnkNetIP_printTest( void )
{
	/* no need to call WSAStartup() */
	//enum_local_nic();

	/* need to call WSAStartup() */
	// get_local_address_v6();
}

#else

#define MAX_NUMOF_IFR 20

static void
getIfName( char* ifname, size_t ifname_size )
{
	struct ifreq ifr[ MAX_NUMOF_IFR ];
	struct ifconf ifc;
	int fd;
	size_t numof_ifr;
	size_t i;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	/* データを受け取る部分の長さ */
	ifc.ifc_len = sizeof(ifr);
	
	/* kernelからデータを受け取る部分を指定 */
	ifc.ifc_ifcu.ifcu_buf = (void *)ifr;
	
	ioctl( fd, SIOCGIFCONF, &ifc );
	
	/* kernelから帰ってきた数を計算 */
	numof_ifr = ifc.ifc_len / sizeof(struct ifreq);
	
	/* 全てのインターフェース名を表示 */
	for( i=0; i<numof_ifr; i++ ){
		ZnkF_printf_e( "%s\n", ifr[i].ifr_name );
	}
	/***
	 * とりあえず最初のものを採用.
	 */
	if( numof_ifr ){
		ZnkS_copy( ifname, ifname_size, ifr[0].ifr_name, Znk_NPOS );
	}
	
	close(fd);
}

void
ZnkNetIP_printTest( void )
{
	struct ifreq ifr;
	char ifname[ 256 ] = "eth0";
	struct sockaddr_in* sin = NULL;
	int sock = socket( AF_INET, SOCK_DGRAM, 0 );

	getIfName( ifname, sizeof(ifname) );
	strcpy(ifr.ifr_name, ifname);
	ifr.ifr_addr.sa_family = AF_INET;
	if( ioctl(sock, SIOCGIFADDR, &ifr) == 0 ){
		sin = (struct sockaddr_in*)&ifr.ifr_addr;
		ZnkF_printf_e( "IpAddress=[%s]\n", inet_ntoa(sin->sin_addr) );
	}
	if( ioctl(sock, SIOCGIFNETMASK, &ifr) == 0 ){
		sin = (struct sockaddr_in*)&ifr.ifr_netmask;
		ZnkF_printf_e( "IpMask=[%s]\n", inet_ntoa(sin->sin_addr) );
	}
	if( ioctl(sock, SIOCGIFBRDADDR, &ifr) == 0 ){
		sin = (struct sockaddr_in*)&ifr.ifr_broadaddr;
		ZnkF_printf_e( "BroadAddr=[%s]\n", inet_ntoa(sin->sin_addr) );
	}

	close(sock);
} 
bool
ZnkNetIP_getPrivateIP( char* ipaddr, size_t ipaddr_size )
{
	struct ifreq ifr;
	char ifname[ 256 ] = "eth0";
	struct sockaddr_in* sin = NULL;
	int sock = socket( AF_INET, SOCK_DGRAM, 0 );

	getIfName( ifname, sizeof(ifname) );
	strcpy(ifr.ifr_name, ifname);
	ifr.ifr_addr.sa_family = AF_INET;
	if( ioctl(sock, SIOCGIFADDR, &ifr) == 0 ){
		sin = (struct sockaddr_in*)&ifr.ifr_addr;
		ZnkS_copy( ipaddr, ipaddr_size, inet_ntoa(sa->sin_addr), Znk_NPOS );
		return true;
	}
	return false;
}

#endif
