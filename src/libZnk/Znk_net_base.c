#include "Znk_net_base.h"
#include "Znk_s_base.h"
#include "Znk_sys_errno.h"
#include "Znk_stdc.h"
#include "Znk_missing_libc.h"

#if defined(Znk_TARGET_WINDOWS)
/***
 * winsock2はWindows98/NT4.0以降が必須である
 */
#  include <winsock2.h>
#else
#  include <signal.h>
#endif


/***
 * VC6.0, DMCのヘッダ内には存在しないことを確認.
 */
#if defined(Znk_TARGET_WINDOWS)
#  if defined(Znk_RECENT_VC) || defined(__BORLANDC__)
  /* WSA_QOS 定数がwinsock2.hヘッダ内に存在する */
#  elif defined(__GNUC__)
  /***
   * MinGWにおいてもWSA_QOS 定数がwinsock2.hヘッダ内に存在するが、
   * 手元のバージョンのMinGWでは WSA_QOS_EUNKOWNPSOBJ が WSA_QOS_EUNKNOWNPSOBJ という名前で
   * defineされている.
   * (本来、本家 WSA_QOS_EUNKOWNPSOBJ の方が綴りミス(UNKOWNになっている)であると思われるが、
   * VC,bccなどでは、慣例的にこちらが使われ続けており、MinGWでもこれにあわせるのが適切かと思う)
   * というわけで、以下ではこれに特別に対処する.
   */
#    ifndef WSA_QOS_EUNKOWNPSOBJ
#      define WSA_QOS_EUNKOWNPSOBJ     (WSABASEERR + 1024)
#    endif
#  else
#    define WSA_QOS_RECEIVERS          (WSABASEERR + 1005)
#    define WSA_QOS_SENDERS            (WSABASEERR + 1006)
#    define WSA_QOS_NO_SENDERS         (WSABASEERR + 1007)
#    define WSA_QOS_NO_RECEIVERS       (WSABASEERR + 1008)
#    define WSA_QOS_REQUEST_CONFIRMED  (WSABASEERR + 1009)
#    define WSA_QOS_ADMISSION_FAILURE  (WSABASEERR + 1010)
#    define WSA_QOS_POLICY_FAILURE     (WSABASEERR + 1011)
#    define WSA_QOS_BAD_STYLE          (WSABASEERR + 1012)
#    define WSA_QOS_BAD_OBJECT         (WSABASEERR + 1013)
#    define WSA_QOS_TRAFFIC_CTRL_ERROR (WSABASEERR + 1014)
#    define WSA_QOS_GENERIC_ERROR      (WSABASEERR + 1015)
#    define WSA_QOS_ESERVICETYPE       (WSABASEERR + 1016)
#    define WSA_QOS_EFLOWSPEC          (WSABASEERR + 1017)
#    define WSA_QOS_EPROVSPECBUF       (WSABASEERR + 1018)
#    define WSA_QOS_EFILTERSTYLE       (WSABASEERR + 1019)
#    define WSA_QOS_EFILTERTYPE        (WSABASEERR + 1020)
#    define WSA_QOS_EFILTERCOUNT       (WSABASEERR + 1021)
#    define WSA_QOS_EOBJLENGTH         (WSABASEERR + 1022)
#    define WSA_QOS_EFLOWCOUNT         (WSABASEERR + 1023)
#    define WSA_QOS_EUNKOWNPSOBJ       (WSABASEERR + 1024)
#    define WSA_QOS_EPOLICYOBJ         (WSABASEERR + 1025)
#    define WSA_QOS_EFLOWDESC          (WSABASEERR + 1026)
#    define WSA_QOS_EPSFLOWSPEC        (WSABASEERR + 1027)
#    define WSA_QOS_EPSFILTERSPEC      (WSABASEERR + 1028)
#    define WSA_QOS_ESDMODEOBJ         (WSABASEERR + 1029)
#    define WSA_QOS_ESHAPERATEOBJ      (WSABASEERR + 1030)
#    define WSA_QOS_RESERVED_PETYPE    (WSABASEERR + 1031)
#  endif
#endif


bool
ZnkNetBase_initiate( bool verbose )
{
#if defined(Znk_TARGET_WINDOWS)
	WSADATA wsaData;

	if( WSAStartup(MAKEWORD(2,0), &wsaData) != 0 ){
		Znk_printf_e( "ZnkNetBase_initiate : WSAStartup : failure.\n" );
		return false;
	}
	if( verbose ){
		Znk_printf_e(
				"ZnkNetBase_initiate : WSA Version = [%d.%d]\n"
				"Description = [%s]\n"
				"SystemStatus = [%s]\n",
				(BYTE)wsaData.wHighVersion, wsaData.wHighVersion >> 8,
				wsaData.szDescription,
				wsaData.szSystemStatus
		);
	}
#else
	/***
	 * For avoiding to exit process by SIGPIPE broken pipe exception
	 * when we send to closed distination socket.
	 */
	signal( SIGPIPE, SIG_IGN );
#endif
	return true;
}

void
ZnkNetBase_finalize( void )
{
#if defined(Znk_TARGET_WINDOWS)
	WSACleanup();
#endif
}

int
ZnkNetBase_getLastErrCode( void )
{
#if defined(Znk_TARGET_WINDOWS)
	return WSAGetLastError();
#else
	return (int)ZnkSysErrno_errno();
#endif
}


const char*
ZnkNetBase_getErrKey( int err_code )
{
#if defined(Znk_TARGET_WINDOWS)
	ZnkSysErrnoInfo* info;
	switch( err_code ){
	/* Windows Sockets definitions of regular Microsoft C error constants */
	Znk_TOSTR_CASE( WSAEINTR  );
	Znk_TOSTR_CASE( WSAEBADF  );
	Znk_TOSTR_CASE( WSAEACCES );
	Znk_TOSTR_CASE( WSAEFAULT );
	Znk_TOSTR_CASE( WSAEINVAL );
	Znk_TOSTR_CASE( WSAEMFILE );

	/* Windows Sockets definitions of regular Berkeley error constants */
	Znk_TOSTR_CASE( WSAEWOULDBLOCK   );
	Znk_TOSTR_CASE( WSAEINPROGRESS   );
	Znk_TOSTR_CASE( WSAEALREADY      );
	Znk_TOSTR_CASE( WSAENOTSOCK      );
	Znk_TOSTR_CASE( WSAEDESTADDRREQ  );
	Znk_TOSTR_CASE( WSAEMSGSIZE      );
	Znk_TOSTR_CASE( WSAEPROTOTYPE    );
	Znk_TOSTR_CASE( WSAENOPROTOOPT   );
	Znk_TOSTR_CASE( WSAEPROTONOSUPPORT );
	Znk_TOSTR_CASE( WSAESOCKTNOSUPPORT );
	Znk_TOSTR_CASE( WSAEOPNOTSUPP    );
	Znk_TOSTR_CASE( WSAEPFNOSUPPORT  );
	Znk_TOSTR_CASE( WSAEAFNOSUPPORT  );
	Znk_TOSTR_CASE( WSAEADDRINUSE    );
	Znk_TOSTR_CASE( WSAEADDRNOTAVAIL );
	Znk_TOSTR_CASE( WSAENETDOWN      );
	Znk_TOSTR_CASE( WSAENETUNREACH   );
	Znk_TOSTR_CASE( WSAENETRESET     );
	Znk_TOSTR_CASE( WSAECONNABORTED  );
	Znk_TOSTR_CASE( WSAECONNRESET    );
	Znk_TOSTR_CASE( WSAENOBUFS       );
	Znk_TOSTR_CASE( WSAEISCONN       );
	Znk_TOSTR_CASE( WSAENOTCONN      );
	Znk_TOSTR_CASE( WSAESHUTDOWN     );
	Znk_TOSTR_CASE( WSAETOOMANYREFS  );
	Znk_TOSTR_CASE( WSAETIMEDOUT     );
	Znk_TOSTR_CASE( WSAECONNREFUSED  );
	Znk_TOSTR_CASE( WSAELOOP         );
	Znk_TOSTR_CASE( WSAENAMETOOLONG  );
	Znk_TOSTR_CASE( WSAEHOSTDOWN     );
	Znk_TOSTR_CASE( WSAEHOSTUNREACH  );
	Znk_TOSTR_CASE( WSAENOTEMPTY     );
	Znk_TOSTR_CASE( WSAEPROCLIM      );
	Znk_TOSTR_CASE( WSAEUSERS        );
	Znk_TOSTR_CASE( WSAEDQUOT        );
	Znk_TOSTR_CASE( WSAESTALE        );
	Znk_TOSTR_CASE( WSAEREMOTE       );
	Znk_TOSTR_CASE( WSAEDISCON       );

	/* Extended Windows Sockets error constant definitions */
	Znk_TOSTR_CASE( WSASYSNOTREADY );
	Znk_TOSTR_CASE( WSAVERNOTSUPPORTED );
	Znk_TOSTR_CASE( WSANOTINITIALISED );

	/*
	 * Error return codes from gethostbyname() and gethostbyaddr()
	 * (when using the resolver). Note that these errors are
	 * retrieved via WSAGetLastError() and must therefore follow
	 * the rules for avoiding clashes with error numbers from
	 * specific implementations or language run-time systems.
	 * For this reason the codes are based at WSABASEERR+1001.
	 * Note also that [WSA]NO_ADDRESS is defined only for
	 * compatibility purposes.
	 */
	/* Authoritative Answer: Host not found */
	Znk_TOSTR_CASE( WSAHOST_NOT_FOUND );
	/* Non-Authoritative: Host not found, or SERVERFAIL */
	Znk_TOSTR_CASE( WSATRY_AGAIN );
	/* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
	Znk_TOSTR_CASE( WSANO_RECOVERY );
	/* Valid name, no data record of requested type */
	Znk_TOSTR_CASE( WSANO_DATA );

	/* Define QOS related error return codes */
	/* at least one Reserve has arrived */
	Znk_TOSTR_CASE( WSA_QOS_RECEIVERS );
	/* at least one Path has arrived */
	Znk_TOSTR_CASE( WSA_QOS_SENDERS );
	/* there are no senders */
	Znk_TOSTR_CASE( WSA_QOS_NO_SENDERS );
	/* there are no receivers */
	Znk_TOSTR_CASE( WSA_QOS_NO_RECEIVERS );
	/* Reserve has been confirmed */
	Znk_TOSTR_CASE( WSA_QOS_REQUEST_CONFIRMED );
	/* error due to lack of resources */
	Znk_TOSTR_CASE( WSA_QOS_ADMISSION_FAILURE );
	/* rejected for administrative reasons - bad credentials */
	Znk_TOSTR_CASE( WSA_QOS_POLICY_FAILURE );
	/* unknown or conflicting style */
	Znk_TOSTR_CASE( WSA_QOS_BAD_STYLE );
	/* problem with some part of the filterspec or providerspecific buffer in general */
	Znk_TOSTR_CASE( WSA_QOS_BAD_OBJECT );
	/* problem with some part of the flowspec */
	Znk_TOSTR_CASE( WSA_QOS_TRAFFIC_CTRL_ERROR );
	/* general error */
	Znk_TOSTR_CASE( WSA_QOS_GENERIC_ERROR );
	/* invalid service type in flowspec */
	Znk_TOSTR_CASE( WSA_QOS_ESERVICETYPE );
	/* invalid flowspec */
	Znk_TOSTR_CASE( WSA_QOS_EFLOWSPEC );
	/* invalid provider specific buffer */
	Znk_TOSTR_CASE( WSA_QOS_EPROVSPECBUF );
	/* invalid filter style */
	Znk_TOSTR_CASE( WSA_QOS_EFILTERSTYLE );
	/* invalid filter type */
	Znk_TOSTR_CASE( WSA_QOS_EFILTERTYPE );
	/* incorrect number of filters */
	Znk_TOSTR_CASE( WSA_QOS_EFILTERCOUNT );
	/* invalid object length */
	Znk_TOSTR_CASE( WSA_QOS_EOBJLENGTH );
	/* incorrect number of flows */
	Znk_TOSTR_CASE( WSA_QOS_EFLOWCOUNT );
	/* unknown object in provider specific buffer */
	Znk_TOSTR_CASE( WSA_QOS_EUNKOWNPSOBJ );
	/* invalid policy object in provider specific buffer */
	Znk_TOSTR_CASE( WSA_QOS_EPOLICYOBJ );
	/* invalid flow descriptor in the list */
	Znk_TOSTR_CASE( WSA_QOS_EFLOWDESC );
	/* inconsistent flow spec in provider specific buffer */
	Znk_TOSTR_CASE( WSA_QOS_EPSFLOWSPEC );
	/* invalid filter spec in provider specific buffer */
	Znk_TOSTR_CASE( WSA_QOS_EPSFILTERSPEC );
	/* invalid shape discard mode object in provider specific buffer */
	Znk_TOSTR_CASE( WSA_QOS_ESDMODEOBJ );
	/* invalid shaping rate object in provider specific buffer */
	Znk_TOSTR_CASE( WSA_QOS_ESHAPERATEOBJ );
	/* reserved policy element in provider specific buffer */
	Znk_TOSTR_CASE( WSA_QOS_RESERVED_PETYPE );
	default: break;
	}
	info = ZnkSysErrno_getInfo( err_code );
	return info->sys_errno_key_;
#else
	ZnkSysErrnoInfo* info = ZnkSysErrno_getInfo( err_code );
	return info->sys_errno_key_;
#endif
}

#if defined(Znk_TARGET_WINDOWS)
/***
 * err_codeはGetLastErrorまたはWSAGetLastErrorで取得した値を指定する.
 */
Znk_INLINE void
getMSWinErrMsg( char* errmsg_buf, size_t errmsg_buf_size, int err_code )
{
	LPVOID lpMsgBuf;
	/* エラー表示文字列作成 */
	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, err_code,
			MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL );
	ZnkS_copy( errmsg_buf, errmsg_buf_size, (const char*)lpMsgBuf, Znk_NPOS );
	LocalFree(lpMsgBuf);
}
#endif

void
ZnkNetBase_getErrMsg( char* errmsg_buf, size_t errmsg_buf_size, int err_code )
{
#if defined(Znk_TARGET_WINDOWS)
	size_t leng;
	Znk_snprintf( errmsg_buf, errmsg_buf_size, "%s:", ZnkNetBase_getErrKey(err_code) );
	leng = Znk_strlen(errmsg_buf);
	if( errmsg_buf_size > leng ){
		getMSWinErrMsg( errmsg_buf+leng, errmsg_buf_size-leng, err_code );
	}
#else
	ZnkSysErrnoInfo* info = ZnkSysErrno_getInfo( err_code );
	ZnkS_copy( errmsg_buf, errmsg_buf_size, info->sys_errno_msg_, Znk_NPOS );
#endif
}
