#ifndef INCLUDE_GUARD__Znk_htp_sbio_h__
#define INCLUDE_GUARD__Znk_htp_sbio_h__

#include <Znk_stock_bio.h>
#include <Znk_htp_hdrs.h>
#include <Znk_htp_rar.h>
#include <Znk_varp_ary.h>
#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

bool
ZnkHtpSBIO_recv( ZnkStockBIO sbio, ZnkHtpReqMethod req_method,
		ZnkHtpHdrs recv_hdrs, ZnkHtpOnRecvFuncArg recv_fnca,
		ZnkVarpAry cookie, ZnkBfr wk_bfr, ZnkStr ermsg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
