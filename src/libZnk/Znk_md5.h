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

/* Data structure for MD5 (Message-Digest) computation */
typedef struct {
	uint32_t i__[2];      /* Private : number of _bits_ handled mod 2^64 */
	uint32_t buf__[4];    /* Private : scratch buffer */
	uint8_t  in__[64];    /* Private : input buffer */
	uint8_t  digest_[16]; /* Public  : actual digest after MD5Final call */
}ZnkMD5_CTX;

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

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
