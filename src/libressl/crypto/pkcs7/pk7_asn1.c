/* $OpenBSD: pk7_asn1.c,v 1.12 2015/07/25 15:33:06 jsing Exp $ */
/* Written by Dr Stephen N Henson (steve@openssl.org) for the OpenSSL
 * project 2000.
 */
/* ====================================================================
 * Copyright (c) 2000 The OpenSSL Project.  All rights reserved.
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
#include <openssl/pkcs7.h>
#include <openssl/x509.h>

/* PKCS#7 ASN1 module */

/* This is the ANY DEFINED BY table for the top level PKCS#7 structure */

static const ASN1_TEMPLATE p7default_tt = {
	Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
	Znk_DOTINIT( tag, 0 ),
	Znk_DOTINIT( offset, offsetof(PKCS7, d.other) ),
	Znk_DOTINIT( field_name, "d.other" ),
	Znk_DOTINIT( item, &ASN1_ANY_it ),
};

static const ASN1_ADB_TABLE PKCS7_adbtbl[] = {
	{
		Znk_DOTINIT( value, NID_pkcs7_data ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL | ASN1_TFLG_NDEF ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS7, d.data) ),
			Znk_DOTINIT( field_name, "d.data" ),
			Znk_DOTINIT( item, &ASN1_OCTET_STRING_NDEF_it ),
		},
	
	},
	{
		Znk_DOTINIT( value, NID_pkcs7_signed ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL | ASN1_TFLG_NDEF ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS7, d.sign) ),
			Znk_DOTINIT( field_name, "d.sign" ),
			Znk_DOTINIT( item, &PKCS7_SIGNED_it ),
		},
	
	},
	{
		Znk_DOTINIT( value, NID_pkcs7_enveloped ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL | ASN1_TFLG_NDEF ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS7, d.enveloped) ),
			Znk_DOTINIT( field_name, "d.enveloped" ),
			Znk_DOTINIT( item, &PKCS7_ENVELOPE_it ),
		},
	
	},
	{
		Znk_DOTINIT( value, NID_pkcs7_signedAndEnveloped ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL | ASN1_TFLG_NDEF ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS7, d.signed_and_enveloped) ),
			Znk_DOTINIT( field_name, "d.signed_and_enveloped" ),
			Znk_DOTINIT( item, &PKCS7_SIGN_ENVELOPE_it ),
		},
	
	},
	{
		Znk_DOTINIT( value, NID_pkcs7_digest ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL | ASN1_TFLG_NDEF ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS7, d.digest) ),
			Znk_DOTINIT( field_name, "d.digest" ),
			Znk_DOTINIT( item, &PKCS7_DIGEST_it ),
		},
	
	},
	{
		Znk_DOTINIT( value, NID_pkcs7_encrypted ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL | ASN1_TFLG_NDEF ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS7, d.encrypted) ),
			Znk_DOTINIT( field_name, "d.encrypted" ),
			Znk_DOTINIT( item, &PKCS7_ENCRYPT_it ),
		},
	
	},
};

static const ASN1_ADB PKCS7_adb = {
	Znk_DOTINIT( flags, 0 ),
	Znk_DOTINIT( offset, offsetof(PKCS7, type) ),
	Znk_DOTINIT( app_items, 0 ),
	Znk_DOTINIT( tbl, PKCS7_adbtbl ),
	Znk_DOTINIT( tblcount, sizeof(PKCS7_adbtbl) / sizeof(ASN1_ADB_TABLE) ),
	Znk_DOTINIT( default_tt, &p7default_tt ),
	Znk_DOTINIT( null_tt, NULL ),
};

/* PKCS#7 streaming support */
static int
pk7_cb(int operation, ASN1_VALUE **pval, const ASN1_ITEM *it, void *exarg)
{
	ASN1_STREAM_ARG *sarg = exarg;
	PKCS7 **pp7 = (PKCS7 **)pval;

	switch (operation) {
	case ASN1_OP_STREAM_PRE:
		if (PKCS7_stream(&sarg->boundary, *pp7) <= 0)
			return 0;

	case ASN1_OP_DETACHED_PRE:
		sarg->ndef_bio = PKCS7_dataInit(*pp7, sarg->out);
		if (!sarg->ndef_bio)
			return 0;
		break;

	case ASN1_OP_STREAM_POST:
	case ASN1_OP_DETACHED_POST:
		if (PKCS7_dataFinal(*pp7, sarg->ndef_bio) <= 0)
			return 0;
		break;
	}
	return 1;
}

static const ASN1_AUX PKCS7_aux = {
	Znk_DOTINIT( app_data, NULL ),
	Znk_DOTINIT( flags, 0 ),
	Znk_DOTINIT( ref_offset, 0 ),
	Znk_DOTINIT( ref_lock, 0 ),
	Znk_DOTINIT( asn1_cb, pk7_cb ),
	Znk_DOTINIT( enc_offset, 0 ),
};
static const ASN1_TEMPLATE PKCS7_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7, type) ),
		Znk_DOTINIT( field_name, "type" ),
		Znk_DOTINIT( item, &ASN1_OBJECT_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_ADB_OID ),
		Znk_DOTINIT( tag, -1 ),
		Znk_DOTINIT( offset, 0 ),
		Znk_DOTINIT( field_name, "PKCS7" ),
		Znk_DOTINIT( item, (const ASN1_ITEM *)&PKCS7_adb ),
	},
};

const ASN1_ITEM PKCS7_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_NDEF_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, PKCS7_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(PKCS7_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, &PKCS7_aux ),
	Znk_DOTINIT( size, sizeof(PKCS7) ),
	Znk_DOTINIT( sname, "PKCS7" ),
};


PKCS7 *
d2i_PKCS7(PKCS7 **a, const unsigned char **in, long len)
{
	return (PKCS7 *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_it);
}

int
i2d_PKCS7(PKCS7 *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_it);
}

PKCS7 *
PKCS7_new(void)
{
	return (PKCS7 *)ASN1_item_new(&PKCS7_it);
}

void
PKCS7_free(PKCS7 *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_it);
}

int
i2d_PKCS7_NDEF(PKCS7 *a, unsigned char **out)
{
	return ASN1_item_ndef_i2d((ASN1_VALUE *)a, out, &PKCS7_it);
}

PKCS7 *
PKCS7_dup(PKCS7 *x)
{
	return ASN1_item_dup(&PKCS7_it, x);
}

static const ASN1_TEMPLATE PKCS7_SIGNED_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGNED, version) ),
		Znk_DOTINIT( field_name, "version" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SET_OF ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGNED, md_algs) ),
		Znk_DOTINIT( field_name, "md_algs" ),
		Znk_DOTINIT( item, &X509_ALGOR_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGNED, contents) ),
		Znk_DOTINIT( field_name, "contents" ),
		Znk_DOTINIT( item, &PKCS7_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT | ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGNED, cert) ),
		Znk_DOTINIT( field_name, "cert" ),
		Znk_DOTINIT( item, &X509_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT | ASN1_TFLG_SET_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 1 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGNED, crl) ),
		Znk_DOTINIT( field_name, "crl" ),
		Znk_DOTINIT( item, &X509_CRL_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SET_OF ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGNED, signer_info) ),
		Znk_DOTINIT( field_name, "signer_info" ),
		Znk_DOTINIT( item, &PKCS7_SIGNER_INFO_it ),
	},
};

const ASN1_ITEM PKCS7_SIGNED_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_NDEF_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, PKCS7_SIGNED_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(PKCS7_SIGNED_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(PKCS7_SIGNED) ),
	Znk_DOTINIT( sname, "PKCS7_SIGNED" ),
};


PKCS7_SIGNED *
d2i_PKCS7_SIGNED(PKCS7_SIGNED **a, const unsigned char **in, long len)
{
	return (PKCS7_SIGNED *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_SIGNED_it);
}

int
i2d_PKCS7_SIGNED(PKCS7_SIGNED *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_SIGNED_it);
}

PKCS7_SIGNED *
PKCS7_SIGNED_new(void)
{
	return (PKCS7_SIGNED *)ASN1_item_new(&PKCS7_SIGNED_it);
}

void
PKCS7_SIGNED_free(PKCS7_SIGNED *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_SIGNED_it);
}

/* Minor tweak to operation: free up EVP_PKEY */
static int
si_cb(int operation, ASN1_VALUE **pval, const ASN1_ITEM *it, void *exarg)
{
	if (operation == ASN1_OP_FREE_POST) {
		PKCS7_SIGNER_INFO *si = (PKCS7_SIGNER_INFO *)*pval;
		EVP_PKEY_free(si->pkey);
	}
	return 1;
}

static const ASN1_AUX PKCS7_SIGNER_INFO_aux = {
	Znk_DOTINIT( app_data, NULL ),
	Znk_DOTINIT( flags, 0 ),
	Znk_DOTINIT( ref_offset, 0 ),
	Znk_DOTINIT( ref_lock, 0 ),
	Znk_DOTINIT( asn1_cb, si_cb ),
	Znk_DOTINIT( enc_offset, 0 ),
};
static const ASN1_TEMPLATE PKCS7_SIGNER_INFO_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGNER_INFO, version) ),
		Znk_DOTINIT( field_name, "version" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGNER_INFO, issuer_and_serial) ),
		Znk_DOTINIT( field_name, "issuer_and_serial" ),
		Znk_DOTINIT( item, &PKCS7_ISSUER_AND_SERIAL_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGNER_INFO, digest_alg) ),
		Znk_DOTINIT( field_name, "digest_alg" ),
		Znk_DOTINIT( item, &X509_ALGOR_it ),
	},
	/* NB this should be a SET OF but we use a SEQUENCE OF so the
	 * original order * is retained when the structure is reencoded.
	 * Since the attributes are implicitly tagged this will not affect
	 * the encoding.
	 */
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT | ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGNER_INFO, auth_attr) ),
		Znk_DOTINIT( field_name, "auth_attr" ),
		Znk_DOTINIT( item, &X509_ATTRIBUTE_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGNER_INFO, digest_enc_alg) ),
		Znk_DOTINIT( field_name, "digest_enc_alg" ),
		Znk_DOTINIT( item, &X509_ALGOR_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGNER_INFO, enc_digest) ),
		Znk_DOTINIT( field_name, "enc_digest" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT | ASN1_TFLG_SET_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 1 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGNER_INFO, unauth_attr) ),
		Znk_DOTINIT( field_name, "unauth_attr" ),
		Znk_DOTINIT( item, &X509_ATTRIBUTE_it ),
	},
};

const ASN1_ITEM PKCS7_SIGNER_INFO_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, PKCS7_SIGNER_INFO_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(PKCS7_SIGNER_INFO_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, &PKCS7_SIGNER_INFO_aux ),
	Znk_DOTINIT( size, sizeof(PKCS7_SIGNER_INFO) ),
	Znk_DOTINIT( sname, "PKCS7_SIGNER_INFO" ),
};


PKCS7_SIGNER_INFO *
d2i_PKCS7_SIGNER_INFO(PKCS7_SIGNER_INFO **a, const unsigned char **in, long len)
{
	return (PKCS7_SIGNER_INFO *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_SIGNER_INFO_it);
}

int
i2d_PKCS7_SIGNER_INFO(PKCS7_SIGNER_INFO *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_SIGNER_INFO_it);
}

PKCS7_SIGNER_INFO *
PKCS7_SIGNER_INFO_new(void)
{
	return (PKCS7_SIGNER_INFO *)ASN1_item_new(&PKCS7_SIGNER_INFO_it);
}

void
PKCS7_SIGNER_INFO_free(PKCS7_SIGNER_INFO *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_SIGNER_INFO_it);
}

static const ASN1_TEMPLATE PKCS7_ISSUER_AND_SERIAL_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_ISSUER_AND_SERIAL, issuer) ),
		Znk_DOTINIT( field_name, "issuer" ),
		Znk_DOTINIT( item, &X509_NAME_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_ISSUER_AND_SERIAL, serial) ),
		Znk_DOTINIT( field_name, "serial" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
};

const ASN1_ITEM PKCS7_ISSUER_AND_SERIAL_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, PKCS7_ISSUER_AND_SERIAL_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(PKCS7_ISSUER_AND_SERIAL_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(PKCS7_ISSUER_AND_SERIAL) ),
	Znk_DOTINIT( sname, "PKCS7_ISSUER_AND_SERIAL" ),
};


PKCS7_ISSUER_AND_SERIAL *
d2i_PKCS7_ISSUER_AND_SERIAL(PKCS7_ISSUER_AND_SERIAL **a, const unsigned char **in, long len)
{
	return (PKCS7_ISSUER_AND_SERIAL *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_ISSUER_AND_SERIAL_it);
}

int
i2d_PKCS7_ISSUER_AND_SERIAL(PKCS7_ISSUER_AND_SERIAL *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_ISSUER_AND_SERIAL_it);
}

PKCS7_ISSUER_AND_SERIAL *
PKCS7_ISSUER_AND_SERIAL_new(void)
{
	return (PKCS7_ISSUER_AND_SERIAL *)ASN1_item_new(&PKCS7_ISSUER_AND_SERIAL_it);
}

void
PKCS7_ISSUER_AND_SERIAL_free(PKCS7_ISSUER_AND_SERIAL *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_ISSUER_AND_SERIAL_it);
}

static const ASN1_TEMPLATE PKCS7_ENVELOPE_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_ENVELOPE, version) ),
		Znk_DOTINIT( field_name, "version" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SET_OF ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_ENVELOPE, recipientinfo) ),
		Znk_DOTINIT( field_name, "recipientinfo" ),
		Znk_DOTINIT( item, &PKCS7_RECIP_INFO_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_ENVELOPE, enc_data) ),
		Znk_DOTINIT( field_name, "enc_data" ),
		Znk_DOTINIT( item, &PKCS7_ENC_CONTENT_it ),
	},
};

const ASN1_ITEM PKCS7_ENVELOPE_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_NDEF_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, PKCS7_ENVELOPE_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(PKCS7_ENVELOPE_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(PKCS7_ENVELOPE) ),
	Znk_DOTINIT( sname, "PKCS7_ENVELOPE" ),
};


PKCS7_ENVELOPE *
d2i_PKCS7_ENVELOPE(PKCS7_ENVELOPE **a, const unsigned char **in, long len)
{
	return (PKCS7_ENVELOPE *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_ENVELOPE_it);
}

int
i2d_PKCS7_ENVELOPE(PKCS7_ENVELOPE *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_ENVELOPE_it);
}

PKCS7_ENVELOPE *
PKCS7_ENVELOPE_new(void)
{
	return (PKCS7_ENVELOPE *)ASN1_item_new(&PKCS7_ENVELOPE_it);
}

void
PKCS7_ENVELOPE_free(PKCS7_ENVELOPE *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_ENVELOPE_it);
}

/* Minor tweak to operation: free up X509 */
static int
ri_cb(int operation, ASN1_VALUE **pval, const ASN1_ITEM *it, void *exarg)
{
	if (operation == ASN1_OP_FREE_POST) {
		PKCS7_RECIP_INFO *ri = (PKCS7_RECIP_INFO *)*pval;
		X509_free(ri->cert);
	}
	return 1;
}

static const ASN1_AUX PKCS7_RECIP_INFO_aux = {
	Znk_DOTINIT( app_data, NULL ),
	Znk_DOTINIT( flags, 0 ),
	Znk_DOTINIT( ref_offset, 0 ),
	Znk_DOTINIT( ref_lock, 0 ),
	Znk_DOTINIT( asn1_cb, ri_cb ),
	Znk_DOTINIT( enc_offset, 0 ),
};
static const ASN1_TEMPLATE PKCS7_RECIP_INFO_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_RECIP_INFO, version) ),
		Znk_DOTINIT( field_name, "version" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_RECIP_INFO, issuer_and_serial) ),
		Znk_DOTINIT( field_name, "issuer_and_serial" ),
		Znk_DOTINIT( item, &PKCS7_ISSUER_AND_SERIAL_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_RECIP_INFO, key_enc_algor) ),
		Znk_DOTINIT( field_name, "key_enc_algor" ),
		Znk_DOTINIT( item, &X509_ALGOR_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_RECIP_INFO, enc_key) ),
		Znk_DOTINIT( field_name, "enc_key" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
	},
};

const ASN1_ITEM PKCS7_RECIP_INFO_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, PKCS7_RECIP_INFO_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(PKCS7_RECIP_INFO_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, &PKCS7_RECIP_INFO_aux ),
	Znk_DOTINIT( size, sizeof(PKCS7_RECIP_INFO) ),
	Znk_DOTINIT( sname, "PKCS7_RECIP_INFO" ),
};


PKCS7_RECIP_INFO *
d2i_PKCS7_RECIP_INFO(PKCS7_RECIP_INFO **a, const unsigned char **in, long len)
{
	return (PKCS7_RECIP_INFO *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_RECIP_INFO_it);
}

int
i2d_PKCS7_RECIP_INFO(PKCS7_RECIP_INFO *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_RECIP_INFO_it);
}

PKCS7_RECIP_INFO *
PKCS7_RECIP_INFO_new(void)
{
	return (PKCS7_RECIP_INFO *)ASN1_item_new(&PKCS7_RECIP_INFO_it);
}

void
PKCS7_RECIP_INFO_free(PKCS7_RECIP_INFO *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_RECIP_INFO_it);
}

static const ASN1_TEMPLATE PKCS7_ENC_CONTENT_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_ENC_CONTENT, content_type) ),
		Znk_DOTINIT( field_name, "content_type" ),
		Znk_DOTINIT( item, &ASN1_OBJECT_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_ENC_CONTENT, algorithm) ),
		Znk_DOTINIT( field_name, "algorithm" ),
		Znk_DOTINIT( item, &X509_ALGOR_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_ENC_CONTENT, enc_data) ),
		Znk_DOTINIT( field_name, "enc_data" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_NDEF_it ),
	},
};

const ASN1_ITEM PKCS7_ENC_CONTENT_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_NDEF_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, PKCS7_ENC_CONTENT_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(PKCS7_ENC_CONTENT_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(PKCS7_ENC_CONTENT) ),
	Znk_DOTINIT( sname, "PKCS7_ENC_CONTENT" ),
};


PKCS7_ENC_CONTENT *
d2i_PKCS7_ENC_CONTENT(PKCS7_ENC_CONTENT **a, const unsigned char **in, long len)
{
	return (PKCS7_ENC_CONTENT *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_ENC_CONTENT_it);
}

int
i2d_PKCS7_ENC_CONTENT(PKCS7_ENC_CONTENT *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_ENC_CONTENT_it);
}

PKCS7_ENC_CONTENT *
PKCS7_ENC_CONTENT_new(void)
{
	return (PKCS7_ENC_CONTENT *)ASN1_item_new(&PKCS7_ENC_CONTENT_it);
}

void
PKCS7_ENC_CONTENT_free(PKCS7_ENC_CONTENT *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_ENC_CONTENT_it);
}

static const ASN1_TEMPLATE PKCS7_SIGN_ENVELOPE_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGN_ENVELOPE, version) ),
		Znk_DOTINIT( field_name, "version" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SET_OF ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGN_ENVELOPE, recipientinfo) ),
		Znk_DOTINIT( field_name, "recipientinfo" ),
		Znk_DOTINIT( item, &PKCS7_RECIP_INFO_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SET_OF ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGN_ENVELOPE, md_algs) ),
		Znk_DOTINIT( field_name, "md_algs" ),
		Znk_DOTINIT( item, &X509_ALGOR_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGN_ENVELOPE, enc_data) ),
		Znk_DOTINIT( field_name, "enc_data" ),
		Znk_DOTINIT( item, &PKCS7_ENC_CONTENT_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT | ASN1_TFLG_SET_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGN_ENVELOPE, cert) ),
		Znk_DOTINIT( field_name, "cert" ),
		Znk_DOTINIT( item, &X509_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT | ASN1_TFLG_SET_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 1 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGN_ENVELOPE, crl) ),
		Znk_DOTINIT( field_name, "crl" ),
		Znk_DOTINIT( item, &X509_CRL_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SET_OF ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_SIGN_ENVELOPE, signer_info) ),
		Znk_DOTINIT( field_name, "signer_info" ),
		Znk_DOTINIT( item, &PKCS7_SIGNER_INFO_it ),
	},
};

const ASN1_ITEM PKCS7_SIGN_ENVELOPE_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_NDEF_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, PKCS7_SIGN_ENVELOPE_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(PKCS7_SIGN_ENVELOPE_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(PKCS7_SIGN_ENVELOPE) ),
	Znk_DOTINIT( sname, "PKCS7_SIGN_ENVELOPE" ),
};


PKCS7_SIGN_ENVELOPE *
d2i_PKCS7_SIGN_ENVELOPE(PKCS7_SIGN_ENVELOPE **a, const unsigned char **in, long len)
{
	return (PKCS7_SIGN_ENVELOPE *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_SIGN_ENVELOPE_it);
}

int
i2d_PKCS7_SIGN_ENVELOPE(PKCS7_SIGN_ENVELOPE *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_SIGN_ENVELOPE_it);
}

PKCS7_SIGN_ENVELOPE *
PKCS7_SIGN_ENVELOPE_new(void)
{
	return (PKCS7_SIGN_ENVELOPE *)ASN1_item_new(&PKCS7_SIGN_ENVELOPE_it);
}

void
PKCS7_SIGN_ENVELOPE_free(PKCS7_SIGN_ENVELOPE *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_SIGN_ENVELOPE_it);
}

static const ASN1_TEMPLATE PKCS7_ENCRYPT_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_ENCRYPT, version) ),
		Znk_DOTINIT( field_name, "version" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_ENCRYPT, enc_data) ),
		Znk_DOTINIT( field_name, "enc_data" ),
		Znk_DOTINIT( item, &PKCS7_ENC_CONTENT_it ),
	},
};

const ASN1_ITEM PKCS7_ENCRYPT_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_NDEF_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, PKCS7_ENCRYPT_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(PKCS7_ENCRYPT_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(PKCS7_ENCRYPT) ),
	Znk_DOTINIT( sname, "PKCS7_ENCRYPT" ),
};


PKCS7_ENCRYPT *
d2i_PKCS7_ENCRYPT(PKCS7_ENCRYPT **a, const unsigned char **in, long len)
{
	return (PKCS7_ENCRYPT *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_ENCRYPT_it);
}

int
i2d_PKCS7_ENCRYPT(PKCS7_ENCRYPT *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_ENCRYPT_it);
}

PKCS7_ENCRYPT *
PKCS7_ENCRYPT_new(void)
{
	return (PKCS7_ENCRYPT *)ASN1_item_new(&PKCS7_ENCRYPT_it);
}

void
PKCS7_ENCRYPT_free(PKCS7_ENCRYPT *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_ENCRYPT_it);
}

static const ASN1_TEMPLATE PKCS7_DIGEST_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_DIGEST, version) ),
		Znk_DOTINIT( field_name, "version" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_DIGEST, md) ),
		Znk_DOTINIT( field_name, "md" ),
		Znk_DOTINIT( item, &X509_ALGOR_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_DIGEST, contents) ),
		Znk_DOTINIT( field_name, "contents" ),
		Znk_DOTINIT( item, &PKCS7_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS7_DIGEST, digest) ),
		Znk_DOTINIT( field_name, "digest" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
	},
};

const ASN1_ITEM PKCS7_DIGEST_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_NDEF_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, PKCS7_DIGEST_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(PKCS7_DIGEST_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(PKCS7_DIGEST) ),
	Znk_DOTINIT( sname, "PKCS7_DIGEST" ),
};


PKCS7_DIGEST *
d2i_PKCS7_DIGEST(PKCS7_DIGEST **a, const unsigned char **in, long len)
{
	return (PKCS7_DIGEST *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS7_DIGEST_it);
}

int
i2d_PKCS7_DIGEST(PKCS7_DIGEST *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS7_DIGEST_it);
}

PKCS7_DIGEST *
PKCS7_DIGEST_new(void)
{
	return (PKCS7_DIGEST *)ASN1_item_new(&PKCS7_DIGEST_it);
}

void
PKCS7_DIGEST_free(PKCS7_DIGEST *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS7_DIGEST_it);
}

/* Specials for authenticated attributes */

/* When signing attributes we want to reorder them to match the sorted
 * encoding.
 */

static const ASN1_TEMPLATE PKCS7_ATTR_SIGN_item_tt = {
	Znk_DOTINIT( flags, ASN1_TFLG_SET_ORDER ),
	Znk_DOTINIT( tag, 0 ),
	Znk_DOTINIT( offset, 0 ),
	Znk_DOTINIT( field_name, "PKCS7_ATTRIBUTES" ),
	Znk_DOTINIT( item, &X509_ATTRIBUTE_it ),
};

const ASN1_ITEM PKCS7_ATTR_SIGN_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_PRIMITIVE ),
	Znk_DOTINIT( utype, -1 ),
	Znk_DOTINIT( templates, &PKCS7_ATTR_SIGN_item_tt ),
	Znk_DOTINIT( tcount, 0 ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, 0 ),
	Znk_DOTINIT( sname, "PKCS7_ATTR_SIGN" ),
};

/* When verifying attributes we need to use the received order. So
 * we use SEQUENCE OF and tag it to SET OF
 */

static const ASN1_TEMPLATE PKCS7_ATTR_VERIFY_item_tt = {
	Znk_DOTINIT( flags, ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_IMPTAG | ASN1_TFLG_UNIVERSAL ),
	Znk_DOTINIT( tag, V_ASN1_SET ),
	Znk_DOTINIT( offset, 0 ),
	Znk_DOTINIT( field_name, "PKCS7_ATTRIBUTES" ),
	Znk_DOTINIT( item, &X509_ATTRIBUTE_it ),
};

const ASN1_ITEM PKCS7_ATTR_VERIFY_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_PRIMITIVE ),
	Znk_DOTINIT( utype, -1 ),
	Znk_DOTINIT( templates, &PKCS7_ATTR_VERIFY_item_tt ),
	Znk_DOTINIT( tcount, 0 ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, 0 ),
	Znk_DOTINIT( sname, "PKCS7_ATTR_VERIFY" ),
};


int
PKCS7_print_ctx(BIO *out, PKCS7 *x, int indent, const ASN1_PCTX *pctx)
{
	return ASN1_item_print(out, (ASN1_VALUE *)x, indent,
	    &PKCS7_it, pctx);
}
