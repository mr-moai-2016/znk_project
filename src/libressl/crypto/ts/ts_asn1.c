/* $OpenBSD: ts_asn1.c,v 1.11 2017/01/29 17:49:23 beck Exp $ */
/* Written by Nils Larsch for the OpenSSL project 2004.
 */
/* ====================================================================
 * Copyright (c) 2006 The OpenSSL Project.  All rights reserved.
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

#include <openssl/opensslconf.h>

#include <openssl/ts.h>
#include <openssl/err.h>
#include <openssl/asn1t.h>

static const ASN1_TEMPLATE TS_MSG_IMPRINT_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_MSG_IMPRINT, hash_algo) ),
		Znk_DOTINIT( field_name, "hash_algo" ),
		Znk_DOTINIT( item, &X509_ALGOR_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_MSG_IMPRINT, hashed_msg) ),
		Znk_DOTINIT( field_name, "hashed_msg" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
	},
};

const ASN1_ITEM TS_MSG_IMPRINT_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, TS_MSG_IMPRINT_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(TS_MSG_IMPRINT_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(TS_MSG_IMPRINT) ),
	Znk_DOTINIT( sname, "TS_MSG_IMPRINT" ),
};


TS_MSG_IMPRINT *
d2i_TS_MSG_IMPRINT(TS_MSG_IMPRINT **a, const unsigned char **in, long len)
{
	return (TS_MSG_IMPRINT *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &TS_MSG_IMPRINT_it);
}

int
i2d_TS_MSG_IMPRINT(const TS_MSG_IMPRINT *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &TS_MSG_IMPRINT_it);
}

TS_MSG_IMPRINT *
TS_MSG_IMPRINT_new(void)
{
	return (TS_MSG_IMPRINT *)ASN1_item_new(&TS_MSG_IMPRINT_it);
}

void
TS_MSG_IMPRINT_free(TS_MSG_IMPRINT *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &TS_MSG_IMPRINT_it);
}

TS_MSG_IMPRINT *
TS_MSG_IMPRINT_dup(TS_MSG_IMPRINT *x)
{
	return ASN1_item_dup(&TS_MSG_IMPRINT_it, x);
}

#ifndef OPENSSL_NO_BIO
TS_MSG_IMPRINT *
d2i_TS_MSG_IMPRINT_bio(BIO *bp, TS_MSG_IMPRINT **a)
{
	return ASN1_item_d2i_bio(&TS_MSG_IMPRINT_it, bp, a);
}

int
i2d_TS_MSG_IMPRINT_bio(BIO *bp, TS_MSG_IMPRINT *a)
{
	return ASN1_item_i2d_bio(&TS_MSG_IMPRINT_it, bp, a);
}
#endif

TS_MSG_IMPRINT *
d2i_TS_MSG_IMPRINT_fp(FILE *fp, TS_MSG_IMPRINT **a)
{
	return ASN1_item_d2i_fp(&TS_MSG_IMPRINT_it, fp, a);
}

int
i2d_TS_MSG_IMPRINT_fp(FILE *fp, TS_MSG_IMPRINT *a)
{
	return ASN1_item_i2d_fp(&TS_MSG_IMPRINT_it, fp, a);
}

static const ASN1_TEMPLATE TS_REQ_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_REQ, version) ),
		Znk_DOTINIT( field_name, "version" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_REQ, msg_imprint) ),
		Znk_DOTINIT( field_name, "msg_imprint" ),
		Znk_DOTINIT( item, &TS_MSG_IMPRINT_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_REQ, policy_id) ),
		Znk_DOTINIT( field_name, "policy_id" ),
		Znk_DOTINIT( item, &ASN1_OBJECT_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_REQ, nonce) ),
		Znk_DOTINIT( field_name, "nonce" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_REQ, cert_req) ),
		Znk_DOTINIT( field_name, "cert_req" ),
		Znk_DOTINIT( item, &ASN1_FBOOLEAN_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT | ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_REQ, extensions) ),
		Znk_DOTINIT( field_name, "extensions" ),
		Znk_DOTINIT( item, &X509_EXTENSION_it ),
	},
};

const ASN1_ITEM TS_REQ_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, TS_REQ_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(TS_REQ_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(TS_REQ) ),
	Znk_DOTINIT( sname, "TS_REQ" ),
};


TS_REQ *
d2i_TS_REQ(TS_REQ **a, const unsigned char **in, long len)
{
	return (TS_REQ *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &TS_REQ_it);
}

int
i2d_TS_REQ(const TS_REQ *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &TS_REQ_it);
}

TS_REQ *
TS_REQ_new(void)
{
	return (TS_REQ *)ASN1_item_new(&TS_REQ_it);
}

void
TS_REQ_free(TS_REQ *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &TS_REQ_it);
}

TS_REQ *
TS_REQ_dup(TS_REQ *x)
{
	return ASN1_item_dup(&TS_REQ_it, x);
}

#ifndef OPENSSL_NO_BIO
TS_REQ *
d2i_TS_REQ_bio(BIO *bp, TS_REQ **a)
{
	return ASN1_item_d2i_bio(&TS_REQ_it, bp, a);
}

int
i2d_TS_REQ_bio(BIO *bp, TS_REQ *a)
{
	return ASN1_item_i2d_bio(&TS_REQ_it, bp, a);
}
#endif

TS_REQ *
d2i_TS_REQ_fp(FILE *fp, TS_REQ **a)
{
	return ASN1_item_d2i_fp(&TS_REQ_it, fp, a);
}

int
i2d_TS_REQ_fp(FILE *fp, TS_REQ *a)
{
	return ASN1_item_i2d_fp(&TS_REQ_it, fp, a);
}

static const ASN1_TEMPLATE TS_ACCURACY_seq_tt[] = {
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_ACCURACY, seconds) ),
		Znk_DOTINIT( field_name, "seconds" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_ACCURACY, millis) ),
		Znk_DOTINIT( field_name, "millis" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 1 ),
		Znk_DOTINIT( offset, offsetof(TS_ACCURACY, micros) ),
		Znk_DOTINIT( field_name, "micros" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
};

const ASN1_ITEM TS_ACCURACY_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, TS_ACCURACY_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(TS_ACCURACY_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(TS_ACCURACY) ),
	Znk_DOTINIT( sname, "TS_ACCURACY" ),
};


TS_ACCURACY *
d2i_TS_ACCURACY(TS_ACCURACY **a, const unsigned char **in, long len)
{
	return (TS_ACCURACY *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &TS_ACCURACY_it);
}

int
i2d_TS_ACCURACY(const TS_ACCURACY *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &TS_ACCURACY_it);
}

TS_ACCURACY *
TS_ACCURACY_new(void)
{
	return (TS_ACCURACY *)ASN1_item_new(&TS_ACCURACY_it);
}

void
TS_ACCURACY_free(TS_ACCURACY *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &TS_ACCURACY_it);
}

TS_ACCURACY *
TS_ACCURACY_dup(TS_ACCURACY *x)
{
	return ASN1_item_dup(&TS_ACCURACY_it, x);
}

static const ASN1_TEMPLATE TS_TST_INFO_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_TST_INFO, version) ),
		Znk_DOTINIT( field_name, "version" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_TST_INFO, policy_id) ),
		Znk_DOTINIT( field_name,"policy_id" ),
		Znk_DOTINIT( item, &ASN1_OBJECT_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_TST_INFO, msg_imprint) ),
		Znk_DOTINIT( field_name, "msg_imprint" ),
		Znk_DOTINIT( item, &TS_MSG_IMPRINT_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_TST_INFO, serial) ),
		Znk_DOTINIT( field_name, "serial" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_TST_INFO, time) ),
		Znk_DOTINIT( field_name, "time" ),
		Znk_DOTINIT( item, &ASN1_GENERALIZEDTIME_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_TST_INFO, accuracy) ),
		Znk_DOTINIT( field_name, "accuracy" ),
		Znk_DOTINIT( item, &TS_ACCURACY_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_TST_INFO, ordering) ),
		Znk_DOTINIT( field_name, "ordering" ),
		Znk_DOTINIT( item, &ASN1_FBOOLEAN_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_TST_INFO, nonce) ),
		Znk_DOTINIT( field_name, "nonce" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_TST_INFO, tsa) ),
		Znk_DOTINIT( field_name, "tsa" ),
		Znk_DOTINIT( item, &GENERAL_NAME_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT | ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 1 ),
		Znk_DOTINIT( offset, offsetof(TS_TST_INFO, extensions) ),
		Znk_DOTINIT( field_name, "extensions" ),
		Znk_DOTINIT( item, &X509_EXTENSION_it ),
	},
};

const ASN1_ITEM TS_TST_INFO_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, TS_TST_INFO_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(TS_TST_INFO_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(TS_TST_INFO) ),
	Znk_DOTINIT( sname, "TS_TST_INFO" ),
};


TS_TST_INFO *
d2i_TS_TST_INFO(TS_TST_INFO **a, const unsigned char **in, long len)
{
	return (TS_TST_INFO *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &TS_TST_INFO_it);
}

int
i2d_TS_TST_INFO(const TS_TST_INFO *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &TS_TST_INFO_it);
}

TS_TST_INFO *
TS_TST_INFO_new(void)
{
	return (TS_TST_INFO *)ASN1_item_new(&TS_TST_INFO_it);
}

void
TS_TST_INFO_free(TS_TST_INFO *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &TS_TST_INFO_it);
}

TS_TST_INFO *
TS_TST_INFO_dup(TS_TST_INFO *x)
{
	return ASN1_item_dup(&TS_TST_INFO_it, x);
}

#ifndef OPENSSL_NO_BIO
TS_TST_INFO *
d2i_TS_TST_INFO_bio(BIO *bp, TS_TST_INFO **a)
{
	return ASN1_item_d2i_bio(&TS_TST_INFO_it, bp, a);
}

int
i2d_TS_TST_INFO_bio(BIO *bp, TS_TST_INFO *a)
{
	return ASN1_item_i2d_bio(&TS_TST_INFO_it, bp, a);
}
#endif

TS_TST_INFO *
d2i_TS_TST_INFO_fp(FILE *fp, TS_TST_INFO **a)
{
	return ASN1_item_d2i_fp(&TS_TST_INFO_it, fp, a);
}

int
i2d_TS_TST_INFO_fp(FILE *fp, TS_TST_INFO *a)
{
	return ASN1_item_i2d_fp(&TS_TST_INFO_it, fp, a);
}

static const ASN1_TEMPLATE TS_STATUS_INFO_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_STATUS_INFO, status) ),
		Znk_DOTINIT( field_name, "status" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_STATUS_INFO, text) ),
		Znk_DOTINIT( field_name, "text" ),
		Znk_DOTINIT( item, &ASN1_UTF8STRING_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_STATUS_INFO, failure_info) ),
		Znk_DOTINIT( field_name, "failure_info" ),
		Znk_DOTINIT( item, &ASN1_BIT_STRING_it ),
	},
};

const ASN1_ITEM TS_STATUS_INFO_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, TS_STATUS_INFO_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(TS_STATUS_INFO_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(TS_STATUS_INFO) ),
	Znk_DOTINIT( sname, "TS_STATUS_INFO" ),
};


TS_STATUS_INFO *
d2i_TS_STATUS_INFO(TS_STATUS_INFO **a, const unsigned char **in, long len)
{
	return (TS_STATUS_INFO *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &TS_STATUS_INFO_it);
}

int
i2d_TS_STATUS_INFO(const TS_STATUS_INFO *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &TS_STATUS_INFO_it);
}

TS_STATUS_INFO *
TS_STATUS_INFO_new(void)
{
	return (TS_STATUS_INFO *)ASN1_item_new(&TS_STATUS_INFO_it);
}

void
TS_STATUS_INFO_free(TS_STATUS_INFO *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &TS_STATUS_INFO_it);
}

TS_STATUS_INFO *
TS_STATUS_INFO_dup(TS_STATUS_INFO *x)
{
	return ASN1_item_dup(&TS_STATUS_INFO_it, x);
}

static int
ts_resp_set_tst_info(TS_RESP *a)
{
	long    status;

	status = ASN1_INTEGER_get(a->status_info->status);

	if (a->token) {
		if (status != 0 && status != 1) {
			TSerror(TS_R_TOKEN_PRESENT);
			return 0;
		}
		if (a->tst_info != NULL)
			TS_TST_INFO_free(a->tst_info);
		a->tst_info = PKCS7_to_TS_TST_INFO(a->token);
		if (!a->tst_info) {
			TSerror(TS_R_PKCS7_TO_TS_TST_INFO_FAILED);
			return 0;
		}
	} else if (status == 0 || status == 1) {
		TSerror(TS_R_TOKEN_NOT_PRESENT);
		return 0;
	}

	return 1;
}

static int
ts_resp_cb(int op, ASN1_VALUE **pval, const ASN1_ITEM *it, void *exarg)
{
	TS_RESP *ts_resp = (TS_RESP *)*pval;

	if (op == ASN1_OP_NEW_POST) {
		ts_resp->tst_info = NULL;
	} else if (op == ASN1_OP_FREE_POST) {
		if (ts_resp->tst_info != NULL)
			TS_TST_INFO_free(ts_resp->tst_info);
	} else if (op == ASN1_OP_D2I_POST) {
		if (ts_resp_set_tst_info(ts_resp) == 0)
			return 0;
	}
	return 1;
}

static const ASN1_AUX TS_RESP_aux = {
	Znk_DOTINIT( app_data, NULL ),
	Znk_DOTINIT( flags, 0 ),
	Znk_DOTINIT( ref_offset, 0 ),
	Znk_DOTINIT( ref_lock, 0 ),
	Znk_DOTINIT( asn1_cb, ts_resp_cb ),
	Znk_DOTINIT( enc_offset, 0 ),
};
static const ASN1_TEMPLATE TS_RESP_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_RESP, status_info) ),
		Znk_DOTINIT( field_name, "status_info" ),
		Znk_DOTINIT( item, &TS_STATUS_INFO_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(TS_RESP, token) ),
		Znk_DOTINIT( field_name, "token" ),
		Znk_DOTINIT( item, &PKCS7_it ),
	},
};

const ASN1_ITEM TS_RESP_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, TS_RESP_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(TS_RESP_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, &TS_RESP_aux ),
	Znk_DOTINIT( size, sizeof(TS_RESP) ),
	Znk_DOTINIT( sname, "TS_RESP" ),
};


TS_RESP *
d2i_TS_RESP(TS_RESP **a, const unsigned char **in, long len)
{
	return (TS_RESP *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &TS_RESP_it);
}

int
i2d_TS_RESP(const TS_RESP *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &TS_RESP_it);
}

TS_RESP *
TS_RESP_new(void)
{
	return (TS_RESP *)ASN1_item_new(&TS_RESP_it);
}

void
TS_RESP_free(TS_RESP *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &TS_RESP_it);
}

TS_RESP *
TS_RESP_dup(TS_RESP *x)
{
	return ASN1_item_dup(&TS_RESP_it, x);
}

#ifndef OPENSSL_NO_BIO
TS_RESP *
d2i_TS_RESP_bio(BIO *bp, TS_RESP **a)
{
	return ASN1_item_d2i_bio(&TS_RESP_it, bp, a);
}

int
i2d_TS_RESP_bio(BIO *bp, TS_RESP *a)
{
	return ASN1_item_i2d_bio(&TS_RESP_it, bp, a);
}
#endif

TS_RESP *
d2i_TS_RESP_fp(FILE *fp, TS_RESP **a)
{
	return ASN1_item_d2i_fp(&TS_RESP_it, fp, a);
}

int
i2d_TS_RESP_fp(FILE *fp, TS_RESP *a)
{
	return ASN1_item_i2d_fp(&TS_RESP_it, fp, a);
}

static const ASN1_TEMPLATE ESS_ISSUER_SERIAL_seq_tt[] = {
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SEQUENCE_OF ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(ESS_ISSUER_SERIAL, issuer) ),
		Znk_DOTINIT( field_name, "issuer" ),
		Znk_DOTINIT( item, &GENERAL_NAME_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(ESS_ISSUER_SERIAL, serial) ),
		Znk_DOTINIT( field_name, "serial" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
};

const ASN1_ITEM ESS_ISSUER_SERIAL_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, ESS_ISSUER_SERIAL_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(ESS_ISSUER_SERIAL_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(ESS_ISSUER_SERIAL) ),
	Znk_DOTINIT( sname, "ESS_ISSUER_SERIAL" ),
};


ESS_ISSUER_SERIAL *
d2i_ESS_ISSUER_SERIAL(ESS_ISSUER_SERIAL **a, const unsigned char **in, long len)
{
	return (ESS_ISSUER_SERIAL *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &ESS_ISSUER_SERIAL_it);
}

int
i2d_ESS_ISSUER_SERIAL(const ESS_ISSUER_SERIAL *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &ESS_ISSUER_SERIAL_it);
}

ESS_ISSUER_SERIAL *
ESS_ISSUER_SERIAL_new(void)
{
	return (ESS_ISSUER_SERIAL *)ASN1_item_new(&ESS_ISSUER_SERIAL_it);
}

void
ESS_ISSUER_SERIAL_free(ESS_ISSUER_SERIAL *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &ESS_ISSUER_SERIAL_it);
}

ESS_ISSUER_SERIAL *
ESS_ISSUER_SERIAL_dup(ESS_ISSUER_SERIAL *x)
{
	return ASN1_item_dup(&ESS_ISSUER_SERIAL_it, x);
}

static const ASN1_TEMPLATE ESS_CERT_ID_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(ESS_CERT_ID, hash) ),
		Znk_DOTINIT( field_name, "hash" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(ESS_CERT_ID, issuer_serial) ),
		Znk_DOTINIT( field_name, "issuer_serial" ),
		Znk_DOTINIT( item, &ESS_ISSUER_SERIAL_it ),
	},
};

const ASN1_ITEM ESS_CERT_ID_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, ESS_CERT_ID_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(ESS_CERT_ID_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(ESS_CERT_ID) ),
	Znk_DOTINIT( sname, "ESS_CERT_ID" ),
};


ESS_CERT_ID *
d2i_ESS_CERT_ID(ESS_CERT_ID **a, const unsigned char **in, long len)
{
	return (ESS_CERT_ID *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &ESS_CERT_ID_it);
}

int
i2d_ESS_CERT_ID(const ESS_CERT_ID *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &ESS_CERT_ID_it);
}

ESS_CERT_ID *
ESS_CERT_ID_new(void)
{
	return (ESS_CERT_ID *)ASN1_item_new(&ESS_CERT_ID_it);
}

void
ESS_CERT_ID_free(ESS_CERT_ID *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &ESS_CERT_ID_it);
}

ESS_CERT_ID *
ESS_CERT_ID_dup(ESS_CERT_ID *x)
{
	return ASN1_item_dup(&ESS_CERT_ID_it, x);
}

static const ASN1_TEMPLATE ESS_SIGNING_CERT_seq_tt[] = {
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SEQUENCE_OF ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(ESS_SIGNING_CERT, cert_ids) ),
		Znk_DOTINIT( field_name, "cert_ids" ),
		Znk_DOTINIT( item, &ESS_CERT_ID_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(ESS_SIGNING_CERT, policy_info) ),
		Znk_DOTINIT( field_name, "policy_info" ),
		Znk_DOTINIT( item, &POLICYINFO_it ),
	},
};

const ASN1_ITEM ESS_SIGNING_CERT_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, ESS_SIGNING_CERT_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(ESS_SIGNING_CERT_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(ESS_SIGNING_CERT) ),
	Znk_DOTINIT( sname, "ESS_SIGNING_CERT" ),
};


ESS_SIGNING_CERT *
d2i_ESS_SIGNING_CERT(ESS_SIGNING_CERT **a, const unsigned char **in, long len)
{
	return (ESS_SIGNING_CERT *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &ESS_SIGNING_CERT_it);
}

int
i2d_ESS_SIGNING_CERT(const ESS_SIGNING_CERT *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &ESS_SIGNING_CERT_it);
}

ESS_SIGNING_CERT *
ESS_SIGNING_CERT_new(void)
{
	return (ESS_SIGNING_CERT *)ASN1_item_new(&ESS_SIGNING_CERT_it);
}

void
ESS_SIGNING_CERT_free(ESS_SIGNING_CERT *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &ESS_SIGNING_CERT_it);
}

ESS_SIGNING_CERT *
ESS_SIGNING_CERT_dup(ESS_SIGNING_CERT *x)
{
	return ASN1_item_dup(&ESS_SIGNING_CERT_it, x);
}

/* Getting encapsulated TS_TST_INFO object from PKCS7. */
TS_TST_INFO *
PKCS7_to_TS_TST_INFO(PKCS7 *token)
{
	PKCS7_SIGNED *pkcs7_signed;
	PKCS7 *enveloped;
	ASN1_TYPE *tst_info_wrapper;
	ASN1_OCTET_STRING *tst_info_der;
	const unsigned char *p;

	if (!PKCS7_type_is_signed(token)) {
		TSerror(TS_R_BAD_PKCS7_TYPE);
		return NULL;
	}

	/* Content must be present. */
	if (PKCS7_get_detached(token)) {
		TSerror(TS_R_DETACHED_CONTENT);
		return NULL;
	}

	/* We have a signed data with content. */
	pkcs7_signed = token->d.sign;
	enveloped = pkcs7_signed->contents;
	if (OBJ_obj2nid(enveloped->type) != NID_id_smime_ct_TSTInfo) {
		TSerror(TS_R_BAD_PKCS7_TYPE);
		return NULL;
	}

	/* We have a DER encoded TST_INFO as the signed data. */
	tst_info_wrapper = enveloped->d.other;
	if (tst_info_wrapper->type != V_ASN1_OCTET_STRING) {
		TSerror(TS_R_BAD_TYPE);
		return NULL;
	}

	/* We have the correct ASN1_OCTET_STRING type. */
	tst_info_der = tst_info_wrapper->value.octet_string;
	/* At last, decode the TST_INFO. */
	p = tst_info_der->data;
	return d2i_TS_TST_INFO(NULL, &p, tst_info_der->length);
}
