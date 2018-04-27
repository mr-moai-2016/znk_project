#ifndef INCLUDE_GUARD__Znk_mbc_jp_h__
#define INCLUDE_GUARD__Znk_mbc_jp_h__

#include <Znk_bif.h>
#include <Znk_err.h>

Znk_EXTERN_C_BEGIN

/**
 * @brief
 * 文字コードを示す列挙型.
 *
 * ZnkMbcType_BINARYは指定時情報として、文字列ではなく一般的なバイナリであることを示す.
 * ZnkMbcType_AUTOは指定時情報として、文字コードを自動的に判断して欲しいことを示す.
 * ZnkMbcType_UNKNOWNは終了コードとして、与えられた文字列の文字コードが未知のものであったことを示す.
 * ZnkMbcType_UTF16:
 *   プログラム上はbit演算で処理する限りLE環境であろうとBE環境であろうとそのコードに差は生じない.
 *   よってLE/BEの差はメモリイメージが重要になる場合に改めて変換処理すればよいこととして
 *   ここではその区別を行わない.
 */
typedef enum {
	 ZnkMbcType_BINARY=0
	,ZnkMbcType_ASCII
	,ZnkMbcType_EUC
	,ZnkMbcType_SJIS
	,ZnkMbcType_JIS
	,ZnkMbcType_UTF8
	,ZnkMbcType_UTF16
	,ZnkMbcType_AUTO
	,ZnkMbcType_UNKNOWN
} ZnkMbcType;

/**
 * @note:
 *  Windowsにおけるワイド文字列はUTF16LEと同じものと考えてよい.
 */
bool
ZnkMbc_convertUTF8_toUTF16( const char* utf8s, size_t utf8s_leng, ZnkBif utf16s, ZnkErr* err );

/***
 * Convert UTF-16-LE (st. byte order is reverse) to UTF-8
 * 結果はutf8sの最後尾へ追加される.
 */
bool
ZnkMbc_convertUTF16_toUTF8( const uint16_t* utf16s, size_t utf16s_leng, ZnkBif utf8s, bool eliminate_bom, ZnkErr* err );

void
ZnkMbc_convertUTF16_toEUC( const uint16_t* utf16s, size_t utf16s_leng, ZnkBif eucs );

bool
ZnkMbc_convertEUC_toUTF16( const char* eucs, size_t eucs_leng, ZnkBif utf16s );

/***
 * UTF8であるutf8sを与え、それをEUCに変換した結果をeucsの最後尾に追加する.
 */
bool
ZnkMbc_convertUTF8_toEUC( const char* utf8s, size_t utf8s_leng, ZnkBif eucs, ZnkBif ws, ZnkErr* err );
/***
 * UTF8であるselfを与え、それをEUCに変換した結果をselfに上書きする.
 */
bool
ZnkMbc_convertUTF8_toEUC_self( ZnkBif self, ZnkBif ws, ZnkErr* err );

/***
 * EUCであるeucsを与え、それをUTF8に変換した結果をutf8sの最後尾に追加する.
 */
bool
ZnkMbc_convertEUC_toUTF8( const char* eucs, size_t eucs_leng, ZnkBif utf8s, ZnkBif ws, ZnkErr* err );
/***
 * EUCであるselfを与え、それをUTF8に変換した結果をselfに上書きする.
 */
bool
ZnkMbc_convertEUC_toUTF8_self( ZnkBif self, ZnkBif ws, ZnkErr* err );

void
ZnkMbc_convert_self( ZnkBif self, ZnkMbcType src_mbc_type, ZnkMbcType dst_mbc_type, ZnkBif tmp, ZnkErr* err );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
