#ifndef INCLUDE_GUARD__Doc_util_h__
#define INCLUDE_GUARD__Doc_util_h__

#include <Znk_str_ary.h>
#include <Znk_myf.h>

Znk_EXTERN_C_BEGIN

/**
 * @brief
 *   HTMLÇHeaderÇê∂ê¨.
 */
void
DocUtil_drawHeader( ZnkStr ans, ZnkStrAry category_path, const char* urp, ZnkMyf menu_myf, const char* doc_title );

void
DocUtil_drawEnd( ZnkStr ans );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
