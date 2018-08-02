/* $OpenBSD: rsa_asn1.c,v 1.13 2016/12/30 15:47:07 jsing Exp $ */
/* Written by Dr Stephen N Henson (steve@openssl.org) for the OpenSSL
 * project 2000.
 */
/* ====================================================================
 * Copyright (c) 2000-2005 The OpenSSL Project.  All rights reserved.
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
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

#include <stdio.h>

#include <openssl/asn1t.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

/* Override the default free and new methods */
static int
rsa_cb(int operation, ASN1_VALUE **pval, const ASN1_ITEM *it, void *exarg)
{
	if (operation == ASN1_OP_NEW_PRE) {
		*pval = (ASN1_VALUE *)RSA_new();
		if (*pval)
			return 2;
		return 0;
	} else if (operation == ASN1_OP_FREE_PRE) {
		RSA_free((RSA *)*pval);
		*pval = NULL;
		return 2;
	}
	return 1;
}

static const ASN1_AUX RSAPrivateKey_aux = {
	Znk_DOTINIT( app_data, NULL ),
	Znk_DOTINIT( flags, 0 ),
	Znk_DOTINIT( ref_offset, 0 ),
	Znk_DOTINIT( ref_lock, 0 ),
	Znk_DOTINIT( asn1_cb, rsa_cb ),
	Znk_DOTINIT( enc_offset, 0 ),
};
static const ASN1_TEMPLATE RSAPrivateKey_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(RSA, version) ),
		Znk_DOTINIT( field_name, "version" ),
		Znk_DOTINIT( item, &LONG_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(RSA, n) ),
		Znk_DOTINIT( field_name, "n" ),
		Znk_DOTINIT( item, &BIGNUM_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(RSA, e) ),
		Znk_DOTINIT( field_name, "e" ),
		Znk_DOTINIT( item, &BIGNUM_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(RSA, d) ),
		Znk_DOTINIT( field_name, "d" ),
		Znk_DOTINIT( item, &BIGNUM_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(RSA, p) ),
		Znk_DOTINIT( field_name, "p" ),
		Znk_DOTINIT( item, &BIGNUM_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(RSA, q) ),
		Znk_DOTINIT( field_name, "q" ),
		Znk_DOTINIT( item, &BIGNUM_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(RSA, dmp1) ),
		Znk_DOTINIT( field_name, "dmp1" ),
		Znk_DOTINIT( item, &BIGNUM_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(RSA, dmq1) ),
		Znk_DOTINIT( field_name, "dmq1" ),
		Znk_DOTINIT( item, &BIGNUM_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(RSA, iqmp) ),
		Znk_DOTINIT( field_name, "iqmp" ),
		Znk_DOTINIT( item, &BIGNUM_it ),
	},
};

const ASN1_ITEM RSAPrivateKey_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, RSAPrivateKey_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(RSAPrivateKey_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, &RSAPrivateKey_aux ),
	Znk_DOTINIT( size, sizeof(RSA) ),
	Znk_DOTINIT( sname, "RSA" ),
};


static const ASN1_AUX RSAPublicKey_aux = {
	Znk_DOTINIT( app_data, NULL ),
	Znk_DOTINIT( flags, 0 ),
	Znk_DOTINIT( ref_offset, 0 ),
	Znk_DOTINIT( ref_lock, 0 ),
	Znk_DOTINIT( asn1_cb, rsa_cb ),
	Znk_DOTINIT( enc_offset, 0 ),
};
static const ASN1_TEMPLATE RSAPublicKey_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(RSA, n) ),
		Znk_DOTINIT( field_name, "n" ),
		Znk_DOTINIT( item, &BIGNUM_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(RSA, e) ),
		Znk_DOTINIT( field_name, "e" ),
		Znk_DOTINIT( item, &BIGNUM_it ),
	},
};

const ASN1_ITEM RSAPublicKey_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, RSAPublicKey_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(RSAPublicKey_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, &RSAPublicKey_aux ),
	Znk_DOTINIT( size, sizeof(RSA) ),
	Znk_DOTINIT( sname, "RSA" ),
};

static const ASN1_TEMPLATE RSA_PSS_PARAMS_seq_tt[] = {
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(RSA_PSS_PARAMS, hashAlgorithm) ),
		Znk_DOTINIT( field_name, "hashAlgorithm" ),
		Znk_DOTINIT( item, &X509_ALGOR_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 1 ),
		Znk_DOTINIT( offset, offsetof(RSA_PSS_PARAMS, maskGenAlgorithm) ),
		Znk_DOTINIT( field_name, "maskGenAlgorithm" ),
		Znk_DOTINIT( item, &X509_ALGOR_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 2 ),
		Znk_DOTINIT( offset, offsetof(RSA_PSS_PARAMS, saltLength) ),
		Znk_DOTINIT( field_name, "saltLength" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 3 ),
		Znk_DOTINIT( offset, offsetof(RSA_PSS_PARAMS, trailerField) ),
		Znk_DOTINIT( field_name, "trailerField" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
};

const ASN1_ITEM RSA_PSS_PARAMS_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, RSA_PSS_PARAMS_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(RSA_PSS_PARAMS_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(RSA_PSS_PARAMS) ),
	Znk_DOTINIT( sname, "RSA_PSS_PARAMS" ),
};


RSA_PSS_PARAMS *
d2i_RSA_PSS_PARAMS(RSA_PSS_PARAMS **a, const unsigned char **in, long len)
{
	return (RSA_PSS_PARAMS *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &RSA_PSS_PARAMS_it);
}

int
i2d_RSA_PSS_PARAMS(RSA_PSS_PARAMS *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &RSA_PSS_PARAMS_it);
}

RSA_PSS_PARAMS *
RSA_PSS_PARAMS_new(void)
{
	return (RSA_PSS_PARAMS *)ASN1_item_new(&RSA_PSS_PARAMS_it);
}

void
RSA_PSS_PARAMS_free(RSA_PSS_PARAMS *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &RSA_PSS_PARAMS_it);
}


RSA *
d2i_RSAPrivateKey(RSA **a, const unsigned char **in, long len)
{
	return (RSA *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &RSAPrivateKey_it);
}

int
i2d_RSAPrivateKey(const RSA *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &RSAPrivateKey_it);
}


RSA *
d2i_RSAPublicKey(RSA **a, const unsigned char **in, long len)
{
	return (RSA *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &RSAPublicKey_it);
}

int
i2d_RSAPublicKey(const RSA *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &RSAPublicKey_it);
}

RSA *
RSAPublicKey_dup(RSA *rsa)
{
	return ASN1_item_dup(&RSAPublicKey_it, rsa);
}

RSA *
RSAPrivateKey_dup(RSA *rsa)
{
	return ASN1_item_dup(&RSAPrivateKey_it, rsa);
}
