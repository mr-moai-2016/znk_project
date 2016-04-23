#ifndef INCLUDE_GUARD__Znk_s_posix_h__
#define INCLUDE_GUARD__Znk_s_posix_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

/**
 * @brief
 *   大文字と小文字を無視したstrcmpであり、正確には指定した文字列s1,s2をすべて
 *   小文字に直したとしてstrcmpした結果を返す(小文字への変換処理はテーブル参照で
 *   行われるため通常のstrcmpに比べ、速度はそれほど低下しない).
 *   ただし、指定する文字列はASCII文字だけからなるものであるか、またはマルチバイト
 *   文字を含む場合でもその文字コードがEUCまたはUTF-8に限るものとする.
 *   SJISの全角文字など、2byte目以降がASCIIの領域と被るようなマルチバイト文字が
 *   含まれる場合、その結果は意味のあるものにならない可能性がある.
 *
 * @note
 *   glibcやBSDで標準提供されているstrcasecmp/strncasecmpと同機能の関数である.
 *   windowsにおける_stricmp/_strnicmpも同様の機能を提供する関数であるが、
 *   windowsにおけるこれらの関数ではLC_CTYPEの値によって、ASCII文字だけからなる
 *   場合と、SJISのようなマルチバイト文字が含まれていても問題ないような場合の
 *   二通りに分けて処理している. 後者はおそらく低速と思われる.
 */
int
ZnkS_strcasecmp( const char* s1, const char* s2 );
int
ZnkS_strncasecmp( const char* s1, const char* s2, size_t n );


/**
 * @brief
 *   NULL終端する位置を返す.
 *   ただし、buf内に'\0'がみつからない場合はbuf_sizeを返す.
 *
 * @note
 *   POSIXで標準提供されているstrnlenと同機能の関数である.
 *   strnlen は、既知のサイズのバッファ(ネットワークパケットなど)内の受信した
 *   信頼できないデータ内の文字列長を取得するような場合に使用できる.
 */
size_t 
ZnkS_strnlen( const char* buf, size_t buf_size );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
