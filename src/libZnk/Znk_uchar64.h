#ifndef INCLUDE_GUARD__Znk_uchar64_h__
#define INCLUDE_GUARD__Znk_uchar64_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef union ZnkUChar64_tag {
	uint64_t u64_; /* 初期化子で64bit整数での指定ができるようにこれを第１番目のメンバーにする */
	char     s_[ 8 ];
} ZnkUChar64;

typedef const uint8_t* (*ZnkUChar64FuncT_increment)( const uint8_t* );
typedef const uint8_t* (*ZnkUChar64FuncT_get)( ZnkUChar64* uc, const uint8_t* p );
typedef struct ZnkUChar64EncImpl* ZnkUChar64Enc;

ZnkUChar64Enc
ZnkUChar64Enc_find( const char* id );

const char*
ZnkUChar64Enc_id( const ZnkUChar64Enc enc );

ZnkUChar64FuncT_get
ZnkUChar64Enc_func_get( const ZnkUChar64Enc enc );

ZnkUChar64FuncT_increment
ZnkUChar64Enc_func_increment( const ZnkUChar64Enc enc );


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
