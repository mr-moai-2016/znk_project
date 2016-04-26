#ifndef INCLUDE_GUARD__Znk_cookie_h__
#define INCLUDE_GUARD__Znk_cookie_h__

#include <Znk_varp_dary.h>

Znk_EXTERN_C_BEGIN

ZnkVarp
ZnkCookie_regist_byAssignmentStatement( ZnkVarpDAry cookie, const char* stmt, size_t stmt_leng );

void
ZnkCookie_regist_byCookieStatement( ZnkVarpDAry cok_vars, const char* cok_stmt, size_t cok_stmt_leng );

void
ZnkCookie_extend_toCookieStatement( const ZnkVarpDAry cok_vars, ZnkStr cok_stmt );

bool
ZnkCookie_load( ZnkVarpDAry cookie, const char* cookie_filename );

bool
ZnkCookie_save( const ZnkVarpDAry cookie, const char* cookie_filename );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
