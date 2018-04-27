#ifndef INCLUDE_GUARD__Znk_tostr_int_h__
#define INCLUDE_GUARD__Znk_tostr_int_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 ZnkToStr_e_IS_LOWER           = (0x1 << 0)
	,ZnkToStr_e_IS_EXP_LOWER       = (0x1 << 1)
	,ZnkToStr_e_IS_SUPPLEMENT      = (0x1 << 2)
	,ZnkToStr_e_IS_TRUNCATE        = (0x1 << 3)
	,ZnkToStr_e_IS_FLEX_DECIMAL    = (0x1 << 4)
	,ZnkToStr_e_IS_SIGN_TOP        = (0x1 << 5)
	,ZnkToStr_e_IS_COMPACT         = (0x1 << 6)
	,ZnkToStr_e_IS_NONE_EXP        = (0x1 << 7)
	,ZnkToStr_e_IS_NUM_SHORTAGE    = (0x1 << 8)
	,ZnkToStr_e_IS_SIGN_SHORTAGE   = (0x1 << 9)

	/***
	 * ●整数および実数型仮数部における符号の置き方に関するフラグ.
	 */
	,ZnkToStr_e_IS_NONE_SIGN       = (0x1 << 10)
	,ZnkToStr_e_IS_PLUS            = (0x1 << 11)
	,ZnkToStr_e_IS_ZERO_SIGN_WHITE = (0x1 << 12)
	,ZnkToStr_e_IS_ZERO_SIGN_PLUS  = (0x1 << 13)
	,ZnkToStr_e_IS_ZERO_SIGN_MINUS = (0x1 << 14)

	/***
	 * ●パフォーマンス、妥当性検証に関するもの
	 */
	,ZnkToStr_e_IS_USE_GENERAL     = (0x1 << 15)

	/***
	 * ●指数部の符号の置き方に関するフラグ.
	 */
	,ZnkToStr_e_IS_EXP_NONE_SIGN       = (0x1 << 16)
	,ZnkToStr_e_IS_EXP_PLUS            = (0x1 << 17)
	,ZnkToStr_e_IS_EXP_ZERO_SIGN_PLUS  = (0x1 << 18)
	,ZnkToStr_e_IS_EXP_ZERO_SIGN_MINUS = (0x1 << 19)

	/***
	 * ●指数部の幅に関するフラグ.
	 */
	,ZnkToStr_e_IS_EXP_SUPPLEMENT      = (0x1 << 20)
	,ZnkToStr_e_IS_EXP_TRUNCATE        = (0x1 << 21)
}ZnkToStrMask;

/**
 * @brief フォーマット情報を保持するフラグ.
 * いくつかの ZnkToStrMask の組み合わせからなる.
 */
typedef uint32_t ZnkToStrFlags;

size_t
ZnkToStr_U32_getStr( char* buf, size_t buf_size, uint32_t uval,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags );
size_t
ZnkToStr_U64_getStr( char* buf, size_t buf_size, uint64_t uval,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags );
size_t
ZnkToStr_I32_getStr( char* buf, size_t buf_size, int32_t ival,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags );
size_t
ZnkToStr_I64_getStr( char* buf, size_t buf_size, int64_t ival,
		size_t width, char space_ch, size_t radix, ZnkToStrFlags flags );

const char* 
ZnkToStr_Bool_getStr( bool bool_val, char type );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
