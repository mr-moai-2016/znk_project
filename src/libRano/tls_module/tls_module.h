#ifndef INCLUDE_GUARD__tls_module_h__
#define INCLUDE_GUARD__tls_module_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

bool
TlsModule_initiate( const char* lib_basename, const char* cert_pem );

void
TlsModule_finalize( void );

bool
TlsModule_getHtpsProcess( const char* cnct_hostname, uint16_t cnct_port,
		ZnkHtpHdrs send_hdrs, ZnkBfr send_body,
		ZnkHtpHdrs recv_hdrs, ZnkHtpOnRecvFuncArg recv_fnca, ZnkHtpOnRecvFuncArg prog_fnca, 
		ZnkVarpAry cookie,
		bool is_proxy, ZnkBfr wk_bfr, ZnkStr ermsg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
