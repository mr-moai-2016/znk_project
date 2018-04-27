#ifndef INCLUDE_GUARD__Znk_htp_util_h__
#define INCLUDE_GUARD__Znk_htp_util_h__

#include <Znk_bfr.h>
#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

size_t
ZnkHtpB64_decode_size( const char *bs64_str, size_t bs64_str_leng );

size_t
ZnkHtpB64_decode( ZnkBfr ans, const char* bs64_str, size_t bs64_str_leng );

size_t
ZnkHtpB64_encode_leng( size_t data_size );

size_t
ZnkHtpB64_encode( ZnkStr ans, const uint8_t* data, size_t data_size );

/**
 * @brief
 * 指定されたdataバイト列に関して URL Encodingを行い、その結果をansに追加的に格納する.
 */
void
ZnkHtpURL_escape( ZnkStr ans, const uint8_t* data, size_t data_size );

/**
 * @brief
 * / 文字だけはそのまま残す以外はZnkHtpURL_escapeと同じ.
 */
void
ZnkHtpURL_escape_withoutSlash( ZnkStr ans, const uint8_t* data, size_t data_size );

void
ZnkHtpURL_unescape( ZnkBfr ans, const char* esc_str, size_t esc_str_leng );

Znk_INLINE void
ZnkHtpURL_unescape_toStr( ZnkStr ans, const char* esc_str, size_t esc_str_leng )
{
	ZnkBfr_pop_bk_ex( ans, NULL, 1 ); /* 一旦最後のNULL文字を強制削除する */
	ZnkHtpURL_unescape( ans, esc_str, esc_str_leng );
	ZnkStr_terminate_null( ans, true );
}

/**
 * @brief
 *  req urp(下記note参照) において、さらにディレクトリに相当する部分をここでは req_urp_dirと呼称する.
 *  この関数ではこのreq_urp_dirをreq_urpより抽出する.
 *
 *  req_urp_dirのいくつか例を示す.
 *    /my_dir/my_data.html ならば /my_dir
 *    / ならば 空文字. 
 *  クエリーストリングの始まりを示す '?' が含まれる場合、? より前の範囲のみで考慮しなければならない.
 *  いくつか例を示す.
 *    /my_dir/my.cgi?arg=val ならば /my_dir
 *    /my_dir/my.cgi?arg=val&arg_dir=special_dir/special_data.dat ならば /my_dir ( ? 以降の / は考えない )
 *
 * @param req_urp_dir:
 *   req_urp_dirの結果を格納する.
 *
 * @param req_urp_tail:
 *   tail部分も必要な場合指定する.
 *   不要な場合はNULLを指定することもできる.
 *
 * @return
 *   req_urp_dir と それ以降のtail部分を分かつ / の位置を示すreq_urpにおけるポインタを返す.
 *   (req_urpにおけるポインタである. req_urp_dirにおけるそれではない.)
 *   req_urp 内にそのような / が存在しなかった場合は NULL を返し、これは同時にreq_urp_dirも空であることを示す.
 *
 * @note
 *  ここで使用する req urp という用語は URLにおける hostname 以降の部分を意味するものとする.
 *  これはGET/POSTリクエストなどの第１行目の２番目のトークンで使われるものを想定している.
 *  例えば http://127.0.0.1:8080/users/taro/my_data.html ならば /my_dir/my_data.html を示す(始まりの/も含むものとする).
 *  ホスト名だけであるような場合、例えば http://127.0.0.1:8080 ならば / 一文字と定める.
 *  クエリーストリングの始まりを示す '?' が含まれる場合、例えば http://127.0.0.1:8080/my_dir/my.cgi?arg=val ならば
 *  GETリクエストの場合は /my_dir/my.cgi?arg=val、POSTリクエストの場合は /my_dir/my_data.cgi で処理されると考えられる.
 */
void
ZnkHtpURL_getReqUrpDir( ZnkStr req_urp_dir, ZnkStr req_urp_tail, const char* req_urp );

/**
 * @brief
 *  req_urp_dirをsanitizeする(req_urp_dirについてはZnkHtpURL_getReqUrpDirのコメントを参照).
 *
 * @note:
 *  ここではいわゆる親ディレクトリを意味する .. が検出された場合は問答無用で __ へ変換する.
 *  また :// が含まれる場合はそれは URL Encodingする.
 *  さらに以下の文字をURL Encodingする.
 *    '\' ( DOSではDSPと判断されてしまうがその効果は打ち消しておくべきである )
 *    '?'、':'、';' '*' などファイルシステムでは無効あるいは危険となる可能性のある文字
 *  しかしShiftJISなどのファイル名の指定を有効としたい特別な場合などは逆に \ をEscapeしてしまっては困る.
 *  この場合はさらに特別な処理が必要となるため、ここでは is_url_encode でこの処理を行うか否かの指定ができるようになっている.
 */
void
ZnkHtpURL_sanitizeReqUrpDir( ZnkStr req_urp_dir, bool is_url_encode );

void
ZnkHtpURL_negateHtmlTagEffection( ZnkStr str );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
