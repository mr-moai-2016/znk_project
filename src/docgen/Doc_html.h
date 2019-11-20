#ifndef INCLUDE_GUARD__Doc_html_make_h__
#define INCLUDE_GUARD__Doc_html_make_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

/**
 * @brief
 *   inからHTMLを作成する.
 */
bool
DocHtml_make( const char* in_dir, const char* out_dir, ZnkStr ermsg );

/***
 * ダミーコメント.
 */

/**
 * @brief
 *   最後にポーズするか否かを返す.
 */
bool
DocHtml_isPauseOnExit( void );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
