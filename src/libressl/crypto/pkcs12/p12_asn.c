/* $OpenBSD: p12_asn.c,v 1.9 2015/07/25 17:08:40 jsing Exp $ */
/* Written by Dr Stephen N Henson (steve@openssl.org) for the OpenSSL
 * project 1999.
 */
/* ====================================================================
 * Copyright (c) 1999 The OpenSSL Project.  All rights reserved.
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
#include <openssl/pkcs12.h>

/* PKCS#12 ASN1 module */

static const ASN1_TEMPLATE PKCS12_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS12, version) ),
		Znk_DOTINIT( field_name, "version" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS12, authsafes) ),
		Znk_DOTINIT( field_name, "authsafes" ),
		Znk_DOTINIT( item, &PKCS7_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS12, mac) ),
		Znk_DOTINIT( field_name, "mac" ),
		Znk_DOTINIT( item, &PKCS12_MAC_DATA_it ),
	},
};

const ASN1_ITEM PKCS12_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, PKCS12_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(PKCS12_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(PKCS12) ),
	Znk_DOTINIT( sname, "PKCS12" ),
};


PKCS12 *
d2i_PKCS12(PKCS12 **a, const unsigned char **in, long len)
{
	return (PKCS12 *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS12_it);
}

int
i2d_PKCS12(PKCS12 *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS12_it);
}

PKCS12 *
PKCS12_new(void)
{
	return (PKCS12 *)ASN1_item_new(&PKCS12_it);
}

void
PKCS12_free(PKCS12 *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS12_it);
}

static const ASN1_TEMPLATE PKCS12_MAC_DATA_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS12_MAC_DATA, dinfo) ),
		Znk_DOTINIT( field_name, "dinfo" ),
		Znk_DOTINIT( item, &X509_SIG_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS12_MAC_DATA, salt) ),
		Znk_DOTINIT( field_name, "salt" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS12_MAC_DATA, iter) ),
		Znk_DOTINIT( field_name, "iter" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
};

const ASN1_ITEM PKCS12_MAC_DATA_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, PKCS12_MAC_DATA_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(PKCS12_MAC_DATA_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(PKCS12_MAC_DATA) ),
	Znk_DOTINIT( sname, "PKCS12_MAC_DATA" ),
};


PKCS12_MAC_DATA *
d2i_PKCS12_MAC_DATA(PKCS12_MAC_DATA **a, const unsigned char **in, long len)
{
	return (PKCS12_MAC_DATA *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS12_MAC_DATA_it);
}

int
i2d_PKCS12_MAC_DATA(PKCS12_MAC_DATA *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS12_MAC_DATA_it);
}

PKCS12_MAC_DATA *
PKCS12_MAC_DATA_new(void)
{
	return (PKCS12_MAC_DATA *)ASN1_item_new(&PKCS12_MAC_DATA_it);
}

void
PKCS12_MAC_DATA_free(PKCS12_MAC_DATA *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS12_MAC_DATA_it);
}

static const ASN1_TEMPLATE bag_default_tt = {
	Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT ),
	Znk_DOTINIT( tag, 0 ),
	Znk_DOTINIT( offset, offsetof(PKCS12_BAGS, value.other) ),
	Znk_DOTINIT( field_name, "value.other" ),
	Znk_DOTINIT( item, &ASN1_ANY_it ),
};

static const ASN1_ADB_TABLE PKCS12_BAGS_adbtbl[] = {
	{
		Znk_DOTINIT( value, NID_x509Certificate ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS12_BAGS, value.x509cert) ),
			Znk_DOTINIT( field_name, "value.x509cert" ),
			Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
		},
	
	},
	{
		Znk_DOTINIT( value, NID_x509Crl ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS12_BAGS, value.x509crl) ),
			Znk_DOTINIT( field_name, "value.x509crl" ),
			Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
		},
	
	},
	{
		Znk_DOTINIT( value, NID_sdsiCertificate ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS12_BAGS, value.sdsicert) ),
			Znk_DOTINIT( field_name, "value.sdsicert" ),
			Znk_DOTINIT( item, &ASN1_IA5STRING_it ),
		},
	
	},
};

static const ASN1_ADB PKCS12_BAGS_adb = {
	Znk_DOTINIT( flags, 0 ),
	Znk_DOTINIT( offset, offsetof(PKCS12_BAGS, type) ),
	Znk_DOTINIT( app_items, 0 ),
	Znk_DOTINIT( tbl, PKCS12_BAGS_adbtbl ),
	Znk_DOTINIT( tblcount, sizeof(PKCS12_BAGS_adbtbl) / sizeof(ASN1_ADB_TABLE) ),
	Znk_DOTINIT( default_tt, &bag_default_tt ),
	Znk_DOTINIT( null_tt, NULL ),
};

static const ASN1_TEMPLATE PKCS12_BAGS_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS12_BAGS, type) ),
		Znk_DOTINIT( field_name, "type" ),
		Znk_DOTINIT( item, &ASN1_OBJECT_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_ADB_OID ),
		Znk_DOTINIT( tag, -1 ),
		Znk_DOTINIT( offset, 0 ),
		Znk_DOTINIT( field_name, "PKCS12_BAGS" ),
		Znk_DOTINIT( item, (const ASN1_ITEM *)&PKCS12_BAGS_adb ),
	},
};

const ASN1_ITEM PKCS12_BAGS_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, PKCS12_BAGS_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(PKCS12_BAGS_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(PKCS12_BAGS) ),
	Znk_DOTINIT( sname, "PKCS12_BAGS" ),
};


PKCS12_BAGS *
d2i_PKCS12_BAGS(PKCS12_BAGS **a, const unsigned char **in, long len)
{
	return (PKCS12_BAGS *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS12_BAGS_it);
}

int
i2d_PKCS12_BAGS(PKCS12_BAGS *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS12_BAGS_it);
}

PKCS12_BAGS *
PKCS12_BAGS_new(void)
{
	return (PKCS12_BAGS *)ASN1_item_new(&PKCS12_BAGS_it);
}

void
PKCS12_BAGS_free(PKCS12_BAGS *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS12_BAGS_it);
}

static const ASN1_TEMPLATE safebag_default_tt = {
	Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT ),
	Znk_DOTINIT( tag, 0 ),
	Znk_DOTINIT( offset, offsetof(PKCS12_SAFEBAG, value.other) ),
	Znk_DOTINIT( field_name, "value.other" ),
	Znk_DOTINIT( item, &ASN1_ANY_it ),
};

static const ASN1_ADB_TABLE PKCS12_SAFEBAG_adbtbl[] = {
	{
		Znk_DOTINIT( value, NID_keyBag ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS12_SAFEBAG, value.keybag) ),
			Znk_DOTINIT( field_name, "value.keybag" ),
			Znk_DOTINIT( item, &PKCS8_PRIV_KEY_INFO_it ),
		},
	
	},
	{
		Znk_DOTINIT( value, NID_pkcs8ShroudedKeyBag ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS12_SAFEBAG, value.shkeybag) ),
			Znk_DOTINIT( field_name, "value.shkeybag" ),
			Znk_DOTINIT( item, &X509_SIG_it ),
		},
	
	},
	{
		Znk_DOTINIT( value, NID_safeContentsBag ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT | ASN1_TFLG_SET_OF ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS12_SAFEBAG, value.safes) ),
			Znk_DOTINIT( field_name, "value.safes" ),
			Znk_DOTINIT( item, &PKCS12_SAFEBAG_it ),
		},
	},
	{
		Znk_DOTINIT( value, NID_certBag ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS12_SAFEBAG, value.bag) ),
			Znk_DOTINIT( field_name, "value.bag" ),
			Znk_DOTINIT( item, &PKCS12_BAGS_it ),
		},
	
	},
	{
		Znk_DOTINIT( value, NID_crlBag ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS12_SAFEBAG, value.bag) ),
			Znk_DOTINIT( field_name, "value.bag" ),
			Znk_DOTINIT( item, &PKCS12_BAGS_it ),
		},
	
	},
	{
		Znk_DOTINIT( value, NID_secretBag ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, ASN1_TFLG_EXPLICIT ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(PKCS12_SAFEBAG, value.bag) ),
			Znk_DOTINIT( field_name, "value.bag" ),
			Znk_DOTINIT( item, &PKCS12_BAGS_it ),
		},
	
	},
};

static const ASN1_ADB PKCS12_SAFEBAG_adb = {
	Znk_DOTINIT( flags, 0 ),
	Znk_DOTINIT( offset, offsetof(PKCS12_SAFEBAG, type) ),
	Znk_DOTINIT( app_items, 0 ),
	Znk_DOTINIT( tbl, PKCS12_SAFEBAG_adbtbl ),
	Znk_DOTINIT( tblcount, sizeof(PKCS12_SAFEBAG_adbtbl) / sizeof(ASN1_ADB_TABLE) ),
	Znk_DOTINIT( default_tt, &safebag_default_tt ),
	Znk_DOTINIT( null_tt, NULL ),
};

static const ASN1_TEMPLATE PKCS12_SAFEBAG_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS12_SAFEBAG, type) ),
		Znk_DOTINIT( field_name, "type" ),
		Znk_DOTINIT( item, &ASN1_OBJECT_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_ADB_OID ),
		Znk_DOTINIT( tag, -1 ),
		Znk_DOTINIT( offset, 0 ),
		Znk_DOTINIT( field_name, "PKCS12_SAFEBAG" ),
		Znk_DOTINIT( item, (const ASN1_ITEM *)&PKCS12_SAFEBAG_adb ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SET_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(PKCS12_SAFEBAG, attrib) ),
		Znk_DOTINIT( field_name, "attrib" ),
		Znk_DOTINIT( item, &X509_ATTRIBUTE_it ),
	},
};

const ASN1_ITEM PKCS12_SAFEBAG_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, PKCS12_SAFEBAG_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(PKCS12_SAFEBAG_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(PKCS12_SAFEBAG) ),
	Znk_DOTINIT( sname, "PKCS12_SAFEBAG" ),
};


PKCS12_SAFEBAG *
d2i_PKCS12_SAFEBAG(PKCS12_SAFEBAG **a, const unsigned char **in, long len)
{
	return (PKCS12_SAFEBAG *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &PKCS12_SAFEBAG_it);
}

int
i2d_PKCS12_SAFEBAG(PKCS12_SAFEBAG *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &PKCS12_SAFEBAG_it);
}

PKCS12_SAFEBAG *
PKCS12_SAFEBAG_new(void)
{
	return (PKCS12_SAFEBAG *)ASN1_item_new(&PKCS12_SAFEBAG_it);
}

void
PKCS12_SAFEBAG_free(PKCS12_SAFEBAG *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &PKCS12_SAFEBAG_it);
}

/* SEQUENCE OF SafeBag */
static const ASN1_TEMPLATE PKCS12_SAFEBAGS_item_tt = {
	Znk_DOTINIT( flags, ASN1_TFLG_SEQUENCE_OF ),
	Znk_DOTINIT( tag, 0 ),
	Znk_DOTINIT( offset, 0 ),
	Znk_DOTINIT( field_name, "PKCS12_SAFEBAGS" ),
	Znk_DOTINIT( item, &PKCS12_SAFEBAG_it ),
};

const ASN1_ITEM PKCS12_SAFEBAGS_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_PRIMITIVE ),
	Znk_DOTINIT( utype, -1 ),
	Znk_DOTINIT( templates, &PKCS12_SAFEBAGS_item_tt ),
	Znk_DOTINIT( tcount, 0 ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, 0 ),
	Znk_DOTINIT( sname, "PKCS12_SAFEBAGS" ),
};

/* Authsafes: SEQUENCE OF PKCS7 */
static const ASN1_TEMPLATE PKCS12_AUTHSAFES_item_tt = {
	Znk_DOTINIT( flags, ASN1_TFLG_SEQUENCE_OF ),
	Znk_DOTINIT( tag, 0 ),
	Znk_DOTINIT( offset, 0 ),
	Znk_DOTINIT( field_name, "PKCS12_AUTHSAFES" ),
	Znk_DOTINIT( item, &PKCS7_it ),
};

const ASN1_ITEM PKCS12_AUTHSAFES_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_PRIMITIVE ),
	Znk_DOTINIT( utype, -1 ),
	Znk_DOTINIT( templates, &PKCS12_AUTHSAFES_item_tt ),
	Znk_DOTINIT( tcount, 0 ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, 0 ),
	Znk_DOTINIT( sname, "PKCS12_AUTHSAFES" ),
};

