#ifndef INCLUDE_GUARD__Znk_rgx_h__
#define INCLUDE_GUARD__Znk_rgx_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkRgxInfoImpl* ZnkRgxInfo;
typedef struct ZnkRgxEncodeImpl* ZnkRgxEncode;

ZnkRgxInfo
ZnkRgxInfo_create( void );
bool
ZnkRgxInfo_setup( ZnkRgxInfo info, const char* regexp_str, const ZnkRgxEncode enc, ZnkStr rep_msg );
void
ZnkRgxInfo_destroy( ZnkRgxInfo info );

/**
 * DFAを使って文字列beginに対してパターンマッチを行う.
 * from: マッチした部分の先頭位置を返す.
 * to:   マッチした部分の直後の位置を返す.
 * パターンマッチに失敗したらfromにNULLをセットする.
 */
void
ZnkRgxInfo_match( ZnkRgxInfo rgxinfo, const char* begin, size_t* from, size_t* to, ZnkStr rep_msg );

ZnkRgxEncode
ZnkRgxEncode_find( const char* id );

const char*
ZnkRgxEncode_id( const ZnkRgxEncode enc );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
