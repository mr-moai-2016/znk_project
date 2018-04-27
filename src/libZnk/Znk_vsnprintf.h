#ifndef INCLUDE_GUARD__Znk_vsnprintf_h__
#define INCLUDE_GUARD__Znk_vsnprintf_h__

#include <Znk_base.h>
#include <stdarg.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkVSNPrintfCtx_tag ZnkVSNPrintfCtx;

typedef size_t (*ZnkVSNPrintfAppendFunc_T)( struct ZnkVSNPrintfCtx_tag* vsnp_ctx, const void* src, size_t req_size );
typedef void   (*ZnkVSNPrintfTerminateNullFunc_T)( struct ZnkVSNPrintfCtx_tag* vsnp_ctx );

struct ZnkVSNPrintfCtx_tag {
	ZnkVSNPrintfAppendFunc_T add_func_;
	ZnkVSNPrintfTerminateNullFunc_T terminate_null_func_;
	uintptr_t pos_;
	void* user_info_;
};

/**
 * @brief:
 *  C99 を指向したvsnprintf
 *
 * @note
 *  サポートされているlength_modifierは以下である.
 *    h   : short型(C標準)
 *    z   : size_t型(C99標準)
 *    j   : intmax_t型(C99標準)
 *    t   : ptrdiff_t型(C99標準)
 *    L   : long double型(C標準)
 *    l   : long 型(C標準)
 *    ll  : long long 型(C99標準)
 *    I16 : 16bit整数(これはZnk_vsnprintf独自の拡張となる)
 *    I32 : 32bit整数(これはZnk_vsnprintf独自の拡張となる)
 *    I64 : 64bit整数(VC,bccでサポートされている拡張)
 */
bool
Znk_vsnprintf_general( ZnkVSNPrintfCtx* vsnp_ctx, const char* fmt, va_list ap );

int
Znk_vsnprintf_C99( char* buf, size_t buf_size, const char* fmt, va_list ap );


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
