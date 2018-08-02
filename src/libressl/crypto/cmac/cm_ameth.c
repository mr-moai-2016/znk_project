/* $OpenBSD: cm_ameth.c,v 1.7 2014/07/12 16:03:37 miod Exp $ */
/* Written by Dr Stephen N Henson (steve@openssl.org) for the OpenSSL
 * project 2010.
 */
/* ====================================================================
 * Copyright (c) 2010 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    licensing@OpenSSL.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 */

#include <stdio.h>

#include <openssl/cmac.h>
#include <openssl/evp.h>

#include "asn1_locl.h"

/* CMAC "ASN1" method. This is just here to indicate the
 * maximum CMAC output length and to free up a CMAC
 * key.
 */

static int
cmac_size(const EVP_PKEY *pkey)
{
	return EVP_MAX_BLOCK_LENGTH;
}

static void
cmac_key_free(EVP_PKEY *pkey)
{
	CMAC_CTX *cmctx = (CMAC_CTX *)pkey->pkey.ptr;

	CMAC_CTX_free(cmctx);
}

const EVP_PKEY_ASN1_METHOD cmac_asn1_meth = {
	Znk_DOTINIT( pkey_id,      EVP_PKEY_CMAC ),
	Znk_DOTINIT( pkey_base_id, EVP_PKEY_CMAC ),
	Znk_DOTINIT( pkey_flags,   0 ),

	Znk_DOTINIT( pem_str, "CMAC" ),
	Znk_DOTINIT( info,    "OpenSSL CMAC method" ),

	Znk_DOTINIT( pub_decode,  NULL ),
	Znk_DOTINIT( pub_encode,  NULL ),
	Znk_DOTINIT( pub_cmp,     NULL ),
	Znk_DOTINIT( pub_print,   NULL ),
	Znk_DOTINIT( priv_decode, NULL ),
	Znk_DOTINIT( priv_encode, NULL ),
	Znk_DOTINIT( priv_print,  NULL ),

	Znk_DOTINIT( pkey_size, cmac_size ),
	Znk_DOTINIT( pkey_bits, NULL ),

	Znk_DOTINIT( param_decode,  NULL ),
	Znk_DOTINIT( param_encode,  NULL ),
	Znk_DOTINIT( param_missing, NULL ),
	Znk_DOTINIT( param_copy,    NULL ),
	Znk_DOTINIT( param_cmp,     NULL ),
	Znk_DOTINIT( param_print,   NULL ),
	Znk_DOTINIT( sig_print,     NULL ),

	Znk_DOTINIT( pkey_free, cmac_key_free ),
	Znk_DOTINIT( pkey_ctrl, NULL ),

	Znk_DOTINIT( old_priv_decode, NULL ),
	Znk_DOTINIT( old_priv_encode, NULL ),
	Znk_DOTINIT( item_verify, NULL ),
	Znk_DOTINIT( item_sign,   NULL ),
};
