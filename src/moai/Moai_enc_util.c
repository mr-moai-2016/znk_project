#include "Moai_enc_util.h"
#include <string.h>
#include <ctype.h>

/* Base64 encoder/decoder table. Originally Apache file ap_base64.c */
/* aaaack but it's fast and const should make it shared text page. */
static const uint8_t st_pr2six[256] = {
	/* ASCII table */
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

size_t
MoaiEncB64_decode_size( const char *bs64_str, size_t bs64_str_leng )
{
	size_t decoded_size;
	register const uint8_t* up;
	register size_t bs64_remain;

    up = (const uint8_t*) bs64_str;
	if( bs64_str_leng == Znk_NPOS ){
		while( st_pr2six[ *up ] <= 63 ){ ++up; }
	} else {
		while( st_pr2six[ *up ] <= 63 ){
			if( up - (const uint8_t*) bs64_str >= (ptrdiff_t)bs64_str_leng ){
				break;
			}
			++up;
		}
	}
	bs64_remain = up - (const uint8_t*) bs64_str;
	decoded_size = ((bs64_remain + 3) / 4) * 3;

	return decoded_size;
}

size_t
MoaiEncB64_decode( ZnkBfr ans, const char* bs64_str, size_t bs64_str_leng )
{
	size_t decoded_size;
	register const uint8_t* up;
	register size_t bs64_remain;
	
	up = (const uint8_t*) bs64_str;
	if( bs64_str_leng == Znk_NPOS ){
		while( st_pr2six[ *up ] <= 63 ){ ++up; }
	} else {
		while( st_pr2six[ *up ] <= 63 ){
			if( up - (const uint8_t*) bs64_str >= (ptrdiff_t)bs64_str_leng ){
				break;
			}
			++up;
		}
	}
	bs64_remain = up - (const uint8_t*) bs64_str;
	decoded_size = ((bs64_remain + 3) / 4) * 3;
	
	up = (const uint8_t*) bs64_str;

	while( bs64_remain >= 4 ){
		ZnkBfr_push_bk( ans, (uint8_t)(st_pr2six[up[0]] << 2 | st_pr2six[up[1]] >> 4) );
		ZnkBfr_push_bk( ans, (uint8_t)(st_pr2six[up[1]] << 4 | st_pr2six[up[2]] >> 2) );
		ZnkBfr_push_bk( ans, (uint8_t)(st_pr2six[up[2]] << 6 | st_pr2six[up[3]]     ) );
		up += 4;
		bs64_remain -= 4;
	}

	/* Note: (bs64_remain == 1) would be an error, so just ingore that case */
	if( bs64_remain > 1 ){
		ZnkBfr_push_bk( ans, (uint8_t)(st_pr2six[up[0]] << 2 | st_pr2six[up[1]] >> 4) );
	}
	if( bs64_remain > 2 ){
		ZnkBfr_push_bk( ans, (uint8_t)(st_pr2six[up[1]] << 4 | st_pr2six[up[2]] >> 2) );
	}
	
	decoded_size -= (4 - bs64_remain) & 3;
	return decoded_size;
}

static const char st_basis_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

size_t
MoaiEncB64_encode_leng( size_t data_size )
{
    return ((data_size + 2) / 3 * 4) + 1;
}

size_t
MoaiEncB64_encode( ZnkStr ans, const uint8_t* data, size_t data_size )
{
	size_t i=0;
	if( data_size > 2 ){
		/***
		 * 3byte分のバイナリバイト列を6bitずつ取得して4byteのBase64文字列コードを得る.
		 */
		for( i=0; i<data_size-2; i+=3 ){
			ZnkStr_add_c( ans, st_basis_64[ (data[i] >> 2) & 0x3F ] );
			ZnkStr_add_c( ans, st_basis_64[ ((data[i    ] & 0x3) << 4) | ((int) (data[i + 1] & 0xF0) >> 4) ] );
			ZnkStr_add_c( ans, st_basis_64[ ((data[i + 1] & 0xF) << 2) | ((int) (data[i + 2] & 0xC0) >> 6) ] );
			ZnkStr_add_c( ans, st_basis_64[ data[i + 2] & 0x3F ] );
	    }
	}
    if( i < data_size ){
		ZnkStr_add_c( ans, st_basis_64[ (data[i] >> 2) & 0x3F ] );
		if( i == (data_size - 1) ){
			ZnkStr_add_c( ans, st_basis_64[ ((data[i] & 0x3) << 4) ] );
			ZnkStr_add_c( ans, '=' );
		} else {
			ZnkStr_add_c( ans, st_basis_64[((data[i] & 0x3) << 4) | ((int) (data[i + 1] & 0xF0) >> 4)] );
			ZnkStr_add_c( ans, st_basis_64[((data[i + 1] & 0xF) << 2)] );
		}
		ZnkStr_add_c( ans, '=' );
	}
	return ZnkStr_leng( ans );
}

Znk_INLINE bool
isReserved( uint8_t ch )
{
	switch( ch ){
	/* gen-delims */
	case ':': case '/': case '?': case '#':
	case '[': case ']': case '@':
	/* sub-delims */
	case '!': case '$': case '&': case '\'':
	case '(': case ')': case '*': case '+':
	case ',': case ';': case '=':
		return true;
	default:
		break;
	}
	return false;
}
Znk_INLINE bool
isNonReserved( uint8_t ch )
{
	switch( ch ){
	case '-': case '.': case '_': case '~':
		return true;
	default:
		break;
	}
	return (bool)( isalnum( ch ) != 0 );
}
Znk_INLINE uint8_t
scanHexCh( uint8_t ch )
{
	switch( ch ){
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return (uint8_t)( ch - '0' );
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		return (uint8_t)( ch - 'A' + 10 );
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		return (uint8_t)( ch - 'a' + 10 );
	default:
		break;
	}
	/* not hex */
	return 0;
}

void
MoaiEncURL_escape( ZnkStr ans, const uint8_t* data, size_t data_size )
{
	const uint8_t* begin = data;
	const uint8_t* end   = data + data_size;
	const uint8_t* p = begin;
	while( p != end ){
		if( isNonReserved(*p) ){
			ZnkStr_add_c( ans, *p );
		} else if( *p == ' ' ){
			ZnkStr_add_c( ans, '+' );
		} else {
			ZnkStr_addf( ans, "%%%02X", *p );
		}
		++p;
	}
}
void
MoaiEncURL_unescape( ZnkBfr ans, const char* esc_str, size_t esc_str_leng )
{
	const char* begin = esc_str;
	const char* end   = esc_str + esc_str_leng;
	const char* p = begin;
	int ival = 0;
	while( p != end ){
		if( p[0] == '%' && p+2 < end ){
			ival =  scanHexCh( (uint8_t)p[1] );
			ival *= 16;
			ival += scanHexCh( (uint8_t)p[2] );
			ZnkBfr_push_bk( ans, (uint8_t)ival );
			p += 3;
		} else if( p[0] == '+' ){
			ZnkBfr_push_bk( ans, (uint8_t)' ' );
			++p;
		} else {
			ZnkBfr_push_bk( ans, (uint8_t)p[0] );
			++p;
		}
	}
}
