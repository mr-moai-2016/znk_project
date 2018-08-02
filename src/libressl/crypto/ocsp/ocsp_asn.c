/* $OpenBSD: ocsp_asn.c,v 1.9 2016/11/04 18:35:30 jsing Exp $ */
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
#include <openssl/asn1.h>
#include <openssl/asn1t.h>
#include <openssl/ocsp.h>

static const ASN1_TEMPLATE OCSP_SIGNATURE_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_SIGNATURE, signatureAlgorithm) ),
		Znk_DOTINIT( field_name, "signatureAlgorithm" ),
		Znk_DOTINIT( item, &X509_ALGOR_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_SIGNATURE, signature) ),
		Znk_DOTINIT( field_name, "signature" ),
		Znk_DOTINIT( item, &ASN1_BIT_STRING_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_SIGNATURE, certs) ),
		Znk_DOTINIT( field_name, "certs" ),
		Znk_DOTINIT( item, &X509_it ),
	},
};

const ASN1_ITEM OCSP_SIGNATURE_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, OCSP_SIGNATURE_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_SIGNATURE_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_SIGNATURE) ),
	Znk_DOTINIT( sname, "OCSP_SIGNATURE" ),
};


OCSP_SIGNATURE *
d2i_OCSP_SIGNATURE(OCSP_SIGNATURE **a, const unsigned char **in, long len)
{
	return (OCSP_SIGNATURE *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_SIGNATURE_it);
}

int
i2d_OCSP_SIGNATURE(OCSP_SIGNATURE *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_SIGNATURE_it);
}

OCSP_SIGNATURE *
OCSP_SIGNATURE_new(void)
{
	return (OCSP_SIGNATURE *)ASN1_item_new(&OCSP_SIGNATURE_it);
}

void
OCSP_SIGNATURE_free(OCSP_SIGNATURE *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_SIGNATURE_it);
}

static const ASN1_TEMPLATE OCSP_CERTID_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_CERTID, hashAlgorithm) ),
		Znk_DOTINIT( field_name, "hashAlgorithm" ),
		Znk_DOTINIT( item, &X509_ALGOR_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_CERTID, issuerNameHash) ),
		Znk_DOTINIT( field_name, "issuerNameHash" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_CERTID, issuerKeyHash) ),
		Znk_DOTINIT( field_name, "issuerKeyHash" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_CERTID, serialNumber) ),
		Znk_DOTINIT( field_name, "serialNumber" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
};

const ASN1_ITEM OCSP_CERTID_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, OCSP_CERTID_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_CERTID_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_CERTID) ),
	Znk_DOTINIT( sname, "OCSP_CERTID" ),
};


OCSP_CERTID *
d2i_OCSP_CERTID(OCSP_CERTID **a, const unsigned char **in, long len)
{
	return (OCSP_CERTID *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_CERTID_it);
}

int
i2d_OCSP_CERTID(OCSP_CERTID *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_CERTID_it);
}

OCSP_CERTID *
OCSP_CERTID_new(void)
{
	return (OCSP_CERTID *)ASN1_item_new(&OCSP_CERTID_it);
}

void
OCSP_CERTID_free(OCSP_CERTID *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_CERTID_it);
}

static const ASN1_TEMPLATE OCSP_ONEREQ_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_ONEREQ, reqCert) ),
		Znk_DOTINIT( field_name, "reqCert" ),
		Znk_DOTINIT( item, &OCSP_CERTID_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_ONEREQ, singleRequestExtensions) ),
		Znk_DOTINIT( field_name, "singleRequestExtensions" ),
		Znk_DOTINIT( item, &X509_EXTENSION_it ),
	},
};

const ASN1_ITEM OCSP_ONEREQ_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, OCSP_ONEREQ_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_ONEREQ_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_ONEREQ) ),
	Znk_DOTINIT( sname, "OCSP_ONEREQ" ),
};


OCSP_ONEREQ *
d2i_OCSP_ONEREQ(OCSP_ONEREQ **a, const unsigned char **in, long len)
{
	return (OCSP_ONEREQ *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_ONEREQ_it);
}

int
i2d_OCSP_ONEREQ(OCSP_ONEREQ *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_ONEREQ_it);
}

OCSP_ONEREQ *
OCSP_ONEREQ_new(void)
{
	return (OCSP_ONEREQ *)ASN1_item_new(&OCSP_ONEREQ_it);
}

void
OCSP_ONEREQ_free(OCSP_ONEREQ *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_ONEREQ_it);
}

static const ASN1_TEMPLATE OCSP_REQINFO_seq_tt[] = {
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_REQINFO, version) ),
		Znk_DOTINIT( field_name, "version" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 1 ),
		Znk_DOTINIT( offset, offsetof(OCSP_REQINFO, requestorName) ),
		Znk_DOTINIT( field_name, "requestorName" ),
		Znk_DOTINIT( item, &GENERAL_NAME_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SEQUENCE_OF ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_REQINFO, requestList) ),
		Znk_DOTINIT( field_name, "requestList" ),
		Znk_DOTINIT( item, &OCSP_ONEREQ_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 2 ),
		Znk_DOTINIT( offset, offsetof(OCSP_REQINFO, requestExtensions) ),
		Znk_DOTINIT( field_name, "requestExtensions" ),
		Znk_DOTINIT( item, &X509_EXTENSION_it ),
	},
};

const ASN1_ITEM OCSP_REQINFO_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, OCSP_REQINFO_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_REQINFO_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_REQINFO) ),
	Znk_DOTINIT( sname, "OCSP_REQINFO" ),
};


OCSP_REQINFO *
d2i_OCSP_REQINFO(OCSP_REQINFO **a, const unsigned char **in, long len)
{
	return (OCSP_REQINFO *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_REQINFO_it);
}

int
i2d_OCSP_REQINFO(OCSP_REQINFO *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_REQINFO_it);
}

OCSP_REQINFO *
OCSP_REQINFO_new(void)
{
	return (OCSP_REQINFO *)ASN1_item_new(&OCSP_REQINFO_it);
}

void
OCSP_REQINFO_free(OCSP_REQINFO *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_REQINFO_it);
}

static const ASN1_TEMPLATE OCSP_REQUEST_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_REQUEST, tbsRequest) ),
		Znk_DOTINIT( field_name, "tbsRequest" ),
		Znk_DOTINIT( item, &OCSP_REQINFO_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_REQUEST, optionalSignature) ),
		Znk_DOTINIT( field_name, "optionalSignature" ),
		Znk_DOTINIT( item, &OCSP_SIGNATURE_it ),
	},
};

const ASN1_ITEM OCSP_REQUEST_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, OCSP_REQUEST_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_REQUEST_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_REQUEST) ),
	Znk_DOTINIT( sname, "OCSP_REQUEST" ),
};

OCSP_REQUEST *
d2i_OCSP_REQUEST(OCSP_REQUEST **a, const unsigned char **in, long len)
{
	return (OCSP_REQUEST *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_REQUEST_it);
}

int
i2d_OCSP_REQUEST(OCSP_REQUEST *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_REQUEST_it);
}

OCSP_REQUEST *
d2i_OCSP_REQUEST_bio(BIO *bp, OCSP_REQUEST **a)
{
	return ASN1_item_d2i_bio(&OCSP_REQUEST_it, bp, a);
}

int
i2d_OCSP_REQUEST_bio(BIO *bp, OCSP_REQUEST *a)
{
	return ASN1_item_i2d_bio(&OCSP_REQUEST_it, bp, a);
}

OCSP_REQUEST *
OCSP_REQUEST_new(void)
{
	return (OCSP_REQUEST *)ASN1_item_new(&OCSP_REQUEST_it);
}

void
OCSP_REQUEST_free(OCSP_REQUEST *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_REQUEST_it);
}

/* OCSP_RESPONSE templates */

static const ASN1_TEMPLATE OCSP_RESPBYTES_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_RESPBYTES, responseType) ),
		Znk_DOTINIT( field_name, "responseType" ),
		Znk_DOTINIT( item, &ASN1_OBJECT_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_RESPBYTES, response) ),
		Znk_DOTINIT( field_name, "response" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
	},
};

const ASN1_ITEM OCSP_RESPBYTES_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, OCSP_RESPBYTES_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_RESPBYTES_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_RESPBYTES) ),
	Znk_DOTINIT( sname, "OCSP_RESPBYTES" ),
};


OCSP_RESPBYTES *
d2i_OCSP_RESPBYTES(OCSP_RESPBYTES **a, const unsigned char **in, long len)
{
	return (OCSP_RESPBYTES *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_RESPBYTES_it);
}

int
i2d_OCSP_RESPBYTES(OCSP_RESPBYTES *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_RESPBYTES_it);
}

OCSP_RESPBYTES *
OCSP_RESPBYTES_new(void)
{
	return (OCSP_RESPBYTES *)ASN1_item_new(&OCSP_RESPBYTES_it);
}

void
OCSP_RESPBYTES_free(OCSP_RESPBYTES *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_RESPBYTES_it);
}

static const ASN1_TEMPLATE OCSP_RESPONSE_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_RESPONSE, responseStatus) ),
		Znk_DOTINIT( field_name, "responseStatus" ),
		Znk_DOTINIT( item, &ASN1_ENUMERATED_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_RESPONSE, responseBytes) ),
		Znk_DOTINIT( field_name, "responseBytes" ),
		Znk_DOTINIT( item, &OCSP_RESPBYTES_it ),
	},
};

const ASN1_ITEM OCSP_RESPONSE_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, OCSP_RESPONSE_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_RESPONSE_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_RESPONSE) ),
	Znk_DOTINIT( sname, "OCSP_RESPONSE" ),
};


OCSP_RESPONSE *
d2i_OCSP_RESPONSE(OCSP_RESPONSE **a, const unsigned char **in, long len)
{
	return (OCSP_RESPONSE *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_RESPONSE_it);
}

int
i2d_OCSP_RESPONSE(OCSP_RESPONSE *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_RESPONSE_it);
}

OCSP_RESPONSE *
d2i_OCSP_RESPONSE_bio(BIO *bp, OCSP_RESPONSE **a)
{
	return ASN1_item_d2i_bio(&OCSP_RESPONSE_it, bp, a);
}

int
i2d_OCSP_RESPONSE_bio(BIO *bp, OCSP_RESPONSE *a)
{
	return ASN1_item_i2d_bio(&OCSP_RESPONSE_it, bp, a);
}

OCSP_RESPONSE *
OCSP_RESPONSE_new(void)
{
	return (OCSP_RESPONSE *)ASN1_item_new(&OCSP_RESPONSE_it);
}

void
OCSP_RESPONSE_free(OCSP_RESPONSE *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_RESPONSE_it);
}

static const ASN1_TEMPLATE OCSP_RESPID_ch_tt[] = {
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT ),
		Znk_DOTINIT( tag, 1 ),
		Znk_DOTINIT( offset, offsetof(OCSP_RESPID, value.byName) ),
		Znk_DOTINIT( field_name, "value.byName" ),
		Znk_DOTINIT( item, &X509_NAME_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT ),
		Znk_DOTINIT( tag, 2 ),
		Znk_DOTINIT( offset, offsetof(OCSP_RESPID, value.byKey) ),
		Znk_DOTINIT( field_name, "value.byKey" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
	},
};

const ASN1_ITEM OCSP_RESPID_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_CHOICE ),
	Znk_DOTINIT( utype, offsetof(OCSP_RESPID, type) ),
	Znk_DOTINIT( templates, OCSP_RESPID_ch_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_RESPID_ch_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_RESPID) ),
	Znk_DOTINIT( sname, "OCSP_RESPID" ),
};


OCSP_RESPID *
d2i_OCSP_RESPID(OCSP_RESPID **a, const unsigned char **in, long len)
{
	return (OCSP_RESPID *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_RESPID_it);
}

int
i2d_OCSP_RESPID(OCSP_RESPID *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_RESPID_it);
}

OCSP_RESPID *
OCSP_RESPID_new(void)
{
	return (OCSP_RESPID *)ASN1_item_new(&OCSP_RESPID_it);
}

void
OCSP_RESPID_free(OCSP_RESPID *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_RESPID_it);
}

static const ASN1_TEMPLATE OCSP_REVOKEDINFO_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_REVOKEDINFO, revocationTime) ),
		Znk_DOTINIT( field_name, "revocationTime" ),
		Znk_DOTINIT( item, &ASN1_GENERALIZEDTIME_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_REVOKEDINFO, revocationReason) ),
		Znk_DOTINIT( field_name, "revocationReason" ),
		Znk_DOTINIT( item, &ASN1_ENUMERATED_it ),
	},
};

const ASN1_ITEM OCSP_REVOKEDINFO_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, OCSP_REVOKEDINFO_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_REVOKEDINFO_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_REVOKEDINFO) ),
	Znk_DOTINIT( sname, "OCSP_REVOKEDINFO" ),
};


OCSP_REVOKEDINFO *
d2i_OCSP_REVOKEDINFO(OCSP_REVOKEDINFO **a, const unsigned char **in, long len)
{
	return (OCSP_REVOKEDINFO *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_REVOKEDINFO_it);
}

int
i2d_OCSP_REVOKEDINFO(OCSP_REVOKEDINFO *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_REVOKEDINFO_it);
}

OCSP_REVOKEDINFO *
OCSP_REVOKEDINFO_new(void)
{
	return (OCSP_REVOKEDINFO *)ASN1_item_new(&OCSP_REVOKEDINFO_it);
}

void
OCSP_REVOKEDINFO_free(OCSP_REVOKEDINFO *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_REVOKEDINFO_it);
}

static const ASN1_TEMPLATE OCSP_CERTSTATUS_ch_tt[] = {
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_CERTSTATUS, value.good) ),
		Znk_DOTINIT( field_name, "value.good" ),
		Znk_DOTINIT( item, &ASN1_NULL_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT ),
		Znk_DOTINIT( tag, 1 ),
		Znk_DOTINIT( offset, offsetof(OCSP_CERTSTATUS, value.revoked) ),
		Znk_DOTINIT( field_name, "value.revoked" ),
		Znk_DOTINIT( item, &OCSP_REVOKEDINFO_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT ),
		Znk_DOTINIT( tag, 2 ),
		Znk_DOTINIT( offset, offsetof(OCSP_CERTSTATUS, value.unknown) ),
		Znk_DOTINIT( field_name, "value.unknown" ),
		Znk_DOTINIT( item, &ASN1_NULL_it ),
	},
};

const ASN1_ITEM OCSP_CERTSTATUS_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_CHOICE ),
	Znk_DOTINIT( utype, offsetof(OCSP_CERTSTATUS, type) ),
	Znk_DOTINIT( templates, OCSP_CERTSTATUS_ch_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_CERTSTATUS_ch_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_CERTSTATUS) ),
	Znk_DOTINIT( sname, "OCSP_CERTSTATUS" ),
};


OCSP_CERTSTATUS *
d2i_OCSP_CERTSTATUS(OCSP_CERTSTATUS **a, const unsigned char **in, long len)
{
	return (OCSP_CERTSTATUS *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_CERTSTATUS_it);
}

int
i2d_OCSP_CERTSTATUS(OCSP_CERTSTATUS *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_CERTSTATUS_it);
}

OCSP_CERTSTATUS *
OCSP_CERTSTATUS_new(void)
{
	return (OCSP_CERTSTATUS *)ASN1_item_new(&OCSP_CERTSTATUS_it);
}

void
OCSP_CERTSTATUS_free(OCSP_CERTSTATUS *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_CERTSTATUS_it);
}

static const ASN1_TEMPLATE OCSP_SINGLERESP_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_SINGLERESP, certId) ),
		Znk_DOTINIT( field_name, "certId" ),
		Znk_DOTINIT( item, &OCSP_CERTID_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_SINGLERESP, certStatus) ),
		Znk_DOTINIT( field_name, "certStatus" ),
		Znk_DOTINIT( item, &OCSP_CERTSTATUS_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_SINGLERESP, thisUpdate) ),
		Znk_DOTINIT( field_name, "thisUpdate" ),
		Znk_DOTINIT( item, &ASN1_GENERALIZEDTIME_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_SINGLERESP, nextUpdate) ),
		Znk_DOTINIT( field_name, "nextUpdate" ),
		Znk_DOTINIT( item, &ASN1_GENERALIZEDTIME_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 1 ),
		Znk_DOTINIT( offset, offsetof(OCSP_SINGLERESP, singleExtensions) ),
		Znk_DOTINIT( field_name, "singleExtensions" ),
		Znk_DOTINIT( item, &X509_EXTENSION_it ),
	},
};

const ASN1_ITEM OCSP_SINGLERESP_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, OCSP_SINGLERESP_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_SINGLERESP_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_SINGLERESP) ),
	Znk_DOTINIT( sname, "OCSP_SINGLERESP" ),
};


OCSP_SINGLERESP *
d2i_OCSP_SINGLERESP(OCSP_SINGLERESP **a, const unsigned char **in, long len)
{
	return (OCSP_SINGLERESP *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_SINGLERESP_it);
}

int
i2d_OCSP_SINGLERESP(OCSP_SINGLERESP *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_SINGLERESP_it);
}

OCSP_SINGLERESP *
OCSP_SINGLERESP_new(void)
{
	return (OCSP_SINGLERESP *)ASN1_item_new(&OCSP_SINGLERESP_it);
}

void
OCSP_SINGLERESP_free(OCSP_SINGLERESP *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_SINGLERESP_it);
}

static const ASN1_TEMPLATE OCSP_RESPDATA_seq_tt[] = {
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_RESPDATA, version) ),
		Znk_DOTINIT( field_name, "version" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_RESPDATA, responderId) ),
		Znk_DOTINIT( field_name, "responderId" ),
		Znk_DOTINIT( item, &OCSP_RESPID_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_RESPDATA, producedAt) ),
		Znk_DOTINIT( field_name, "producedAt" ),
		Znk_DOTINIT( item, &ASN1_GENERALIZEDTIME_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SEQUENCE_OF ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_RESPDATA, responses) ),
		Znk_DOTINIT( field_name, "responses" ),
		Znk_DOTINIT( item, &OCSP_SINGLERESP_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 1 ),
		Znk_DOTINIT( offset, offsetof(OCSP_RESPDATA, responseExtensions) ),
		Znk_DOTINIT( field_name, "responseExtensions" ),
		Znk_DOTINIT( item, &X509_EXTENSION_it ),
	},
};

const ASN1_ITEM OCSP_RESPDATA_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, OCSP_RESPDATA_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_RESPDATA_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_RESPDATA) ),
	Znk_DOTINIT( sname, "OCSP_RESPDATA" ),
};


OCSP_RESPDATA *
d2i_OCSP_RESPDATA(OCSP_RESPDATA **a, const unsigned char **in, long len)
{
	return (OCSP_RESPDATA *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_RESPDATA_it);
}

int
i2d_OCSP_RESPDATA(OCSP_RESPDATA *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_RESPDATA_it);
}

OCSP_RESPDATA *
OCSP_RESPDATA_new(void)
{
	return (OCSP_RESPDATA *)ASN1_item_new(&OCSP_RESPDATA_it);
}

void
OCSP_RESPDATA_free(OCSP_RESPDATA *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_RESPDATA_it);
}

static const ASN1_TEMPLATE OCSP_BASICRESP_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_BASICRESP, tbsResponseData) ),
		Znk_DOTINIT( field_name, "tbsResponseData" ),
		Znk_DOTINIT( item, &OCSP_RESPDATA_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_BASICRESP, signatureAlgorithm) ),
		Znk_DOTINIT( field_name, "signatureAlgorithm" ),
		Znk_DOTINIT( item, &X509_ALGOR_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_BASICRESP, signature) ),
		Znk_DOTINIT( field_name, "signature" ),
		Znk_DOTINIT( item, &ASN1_BIT_STRING_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_BASICRESP, certs) ),
		Znk_DOTINIT( field_name, "certs" ),
		Znk_DOTINIT( item, &X509_it ),
	},
};

const ASN1_ITEM OCSP_BASICRESP_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, OCSP_BASICRESP_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_BASICRESP_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_BASICRESP) ),
	Znk_DOTINIT( sname, "OCSP_BASICRESP" ),
};


OCSP_BASICRESP *
d2i_OCSP_BASICRESP(OCSP_BASICRESP **a, const unsigned char **in, long len)
{
	return (OCSP_BASICRESP *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_BASICRESP_it);
}

int
i2d_OCSP_BASICRESP(OCSP_BASICRESP *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_BASICRESP_it);
}

OCSP_BASICRESP *
OCSP_BASICRESP_new(void)
{
	return (OCSP_BASICRESP *)ASN1_item_new(&OCSP_BASICRESP_it);
}

void
OCSP_BASICRESP_free(OCSP_BASICRESP *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_BASICRESP_it);
}

static const ASN1_TEMPLATE OCSP_CRLID_seq_tt[] = {
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_CRLID, crlUrl) ),
		Znk_DOTINIT( field_name, "crlUrl" ),
		Znk_DOTINIT( item, &ASN1_IA5STRING_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 1 ),
		Znk_DOTINIT( offset, offsetof(OCSP_CRLID, crlNum) ),
		Znk_DOTINIT( field_name, "crlNum" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 2 ),
		Znk_DOTINIT( offset, offsetof(OCSP_CRLID, crlTime) ),
		Znk_DOTINIT( field_name, "crlTime" ),
		Znk_DOTINIT( item, &ASN1_GENERALIZEDTIME_it ),
	},
};

const ASN1_ITEM OCSP_CRLID_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, OCSP_CRLID_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_CRLID_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_CRLID) ),
	Znk_DOTINIT( sname, "OCSP_CRLID" ),
};


OCSP_CRLID *
d2i_OCSP_CRLID(OCSP_CRLID **a, const unsigned char **in, long len)
{
	return (OCSP_CRLID *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_CRLID_it);
}

int
i2d_OCSP_CRLID(OCSP_CRLID *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_CRLID_it);
}

OCSP_CRLID *
OCSP_CRLID_new(void)
{
	return (OCSP_CRLID *)ASN1_item_new(&OCSP_CRLID_it);
}

void
OCSP_CRLID_free(OCSP_CRLID *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_CRLID_it);
}

static const ASN1_TEMPLATE OCSP_SERVICELOC_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_SERVICELOC, issuer) ),
		Znk_DOTINIT( field_name, "issuer" ),
		Znk_DOTINIT( item, &X509_NAME_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(OCSP_SERVICELOC, locator) ),
		Znk_DOTINIT( field_name, "locator" ),
		Znk_DOTINIT( item, &ACCESS_DESCRIPTION_it ),
	},
};

const ASN1_ITEM OCSP_SERVICELOC_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, OCSP_SERVICELOC_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(OCSP_SERVICELOC_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(OCSP_SERVICELOC) ),
	Znk_DOTINIT( sname, "OCSP_SERVICELOC" ),
};


OCSP_SERVICELOC *
d2i_OCSP_SERVICELOC(OCSP_SERVICELOC **a, const unsigned char **in, long len)
{
	return (OCSP_SERVICELOC *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &OCSP_SERVICELOC_it);
}

int
i2d_OCSP_SERVICELOC(OCSP_SERVICELOC *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &OCSP_SERVICELOC_it);
}

OCSP_SERVICELOC *
OCSP_SERVICELOC_new(void)
{
	return (OCSP_SERVICELOC *)ASN1_item_new(&OCSP_SERVICELOC_it);
}

void
OCSP_SERVICELOC_free(OCSP_SERVICELOC *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &OCSP_SERVICELOC_it);
}
