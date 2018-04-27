#ifndef INCLUDE_GUARD__Znk_md5_h__
#define INCLUDE_GUARD__Znk_md5_h__

/*
 ***********************************************************************
 ** md5.h -- header file for implementation of MD5                    **
 ** RSA Data Security, Inc. MD5 Message-Digest Algorithm              **
 ** Created: 2/17/90 RLR                                              **
 ** Revised: 12/27/90 SRD,AJ,BSK,JT Reference C version               **
 ** Revised (for MD5): RLR 4/27/91                                    **
 ***********************************************************************
 */

/*
 ***********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved.  **
 **                                                                   **
 ** License to copy and use this software is granted provided that    **
 ** it is identified as the "RSA Data Security, Inc. MD5 Message-     **
 ** Digest Algorithm" in all material mentioning or referencing this  **
 ** software or this function.                                        **
 **                                                                   **
 ** License is also granted to make and use derivative works          **
 ** provided that such works are identified as "derived from the RSA  **
 ** Data Security, Inc. MD5 Message-Digest Algorithm" in all          **
 ** material mentioning or referencing the derived work.              **
 **                                                                   **
 ** RSA Data Security, Inc. makes no representations concerning       **
 ** either the merchantability of this software or the suitability    **
 ** of this software for any particular purpose.  It is provided "as  **
 ** is" without express or implied warranty of any kind.              **
 **                                                                   **
 ** These notices must be retained in any copies of any part of this  **
 ** documentation and/or software.                                    **
 ***********************************************************************
 */

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

/**
 * MD5アルゴリズムにおいてこの値は常に16であるが一応名前をつけておく.
 */
#define ZnkMD5_DIGEST_SIZE 16

/* Data structure for MD5 (Message-Digest) computation */
typedef struct {
	uint32_t i__[ 2 ];      /* Private : number of _bits_ handled mod 2^64 */
	uint32_t buf__[ 4 ];    /* Private : scratch buffer */
	uint8_t  in__[ 64 ];    /* Private : input buffer */
	uint8_t  digest_[ ZnkMD5_DIGEST_SIZE ]; /* Public  : actual digest after MD5Final call */
}ZnkMD5_CTX;

Znk_INLINE const uint8_t*
ZnkMD5_digest( const ZnkMD5_CTX* mdContext ){
	return mdContext->digest_;
}

void ZnkMD5_init( ZnkMD5_CTX* mdContext );
void ZnkMD5_update( ZnkMD5_CTX* mdContext, const uint8_t* data, unsigned int data_size );
void ZnkMD5_final( ZnkMD5_CTX* mdContext );

Znk_INLINE void
ZnkMD5_getDigest_byData( ZnkMD5_CTX* mdContext, const uint8_t* data, size_t data_size )
{
    ZnkMD5_init( mdContext );
    ZnkMD5_update( mdContext, data, data_size );
    ZnkMD5_final( mdContext );
}

void
ZnkMD5_getDigest_byFile( ZnkMD5_CTX* mdContext, const char* file_path );


/**
 * @brief
 *   MD5のCheckSumデータ.
 *   一般にMD5 CheckSum と呼ばれる 32 文字のASCII文字列である.
 *   ZnkMD5_getCheckSum関数でこれを取得した場合、常にNULL終端される.
 *   即ちユーザはbuf_をC文字列として参照してよい.
 */
typedef struct {
	/* null終端分用として+1してある */
	char buf_[ ZnkMD5_DIGEST_SIZE*2+1 ];
}ZnkMD5CheckSum;

Znk_INLINE void
ZnkMD5CheckSum_get_byContext( ZnkMD5CheckSum* checksum, const ZnkMD5_CTX* mdContext )
{
	const char* xdigit = "0123456789abcdef";
	size_t idx;
	char* p = checksum->buf_;
	for( idx=0; idx<sizeof(mdContext->digest_); ++idx ){
		*p++ = xdigit[ mdContext->digest_[ idx ] >> 4 ];
		*p++ = xdigit[ mdContext->digest_[ idx ] & 0xf ];
	}
	*p = '\0';
}
Znk_INLINE const char*
ZnkMD5CheckSum_cstr( const ZnkMD5CheckSum* checksum ){
	return checksum->buf_;
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
