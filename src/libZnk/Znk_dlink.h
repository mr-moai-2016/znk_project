#ifndef INCLUDE_GUARD__Znk_dlink_h__
#define INCLUDE_GUARD__Znk_dlink_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef void* ZnkDLinkHandler;

/**
 * @brief
 *   Dynamic Load Libraryをメモリへロードする.
 */
ZnkDLinkHandler ZnkDLink_open( const char* lib_name );
/**
 * @brief
 *   Dynamic Load Library内の関数ポインタを取得する.
 */
void* ZnkDLink_getFunc( ZnkDLinkHandler handle, const char* func_name );
/**
 * @brief
 *   ロードしたDynamic Load Libraryをメモリから削除する.
 *   この関数を呼び出した後は、ZnkDLink_getFunc で取得した関数ポインタも無効となる.
 *   従ってそれらをもう完全に使わなくなった時点で初めて呼び出す必要がある.
 */
void ZnkDLink_close( ZnkDLinkHandler handle );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
