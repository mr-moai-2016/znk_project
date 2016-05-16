#ifndef INCLUDE_GUARD__Moai_enc_util_h__
#define INCLUDE_GUARD__Moai_enc_util_h__

#include <Znk_bfr.h>
#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

size_t
MoaiEncB64_decode_size( const char *bs64_str, size_t bs64_str_leng );

size_t
MoaiEncB64_decode( ZnkBfr ans, const char* bs64_str, size_t bs64_str_leng );

size_t
MoaiEncB64_encode_leng( size_t data_size );

size_t
MoaiEncB64_encode( ZnkStr ans, const uint8_t* data, size_t data_size );

void
MoaiEncURL_escape( ZnkStr ans, const uint8_t* data, size_t data_size );

void
MoaiEncURL_unescape( ZnkBfr ans, const char* esc_str, size_t esc_str_leng );

Znk_INLINE void
MoaiEncURL_unescape_toStr( ZnkStr ans, const char* esc_str, size_t esc_str_leng )
{
	ZnkBfr_pop_bk_ex( ans, NULL, 1 ); /* àÍíUç≈å„ÇÃNULLï∂éöÇã≠êßçÌèúÇ∑ÇÈ */
	MoaiEncURL_unescape( ans, esc_str, esc_str_leng );
	ZnkStr_terminate_null( ans, true );
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
