#ifndef INCLUDE_GUARD__Znk_liba_scan_h__
#define INCLUDE_GUARD__Znk_liba_scan_h__

#include <Znk_str_ary.h>

Znk_EXTERN_C_BEGIN

bool
ZnkLibaScan_getExportFunctionSymbols( const char* slib_filename, ZnkStrAry export_symbols, bool debug );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
