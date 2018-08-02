/**********************************************************************
 *                          gost_keytrans.c                           *
 *             Copyright (c) 2005-2006 Cryptocom LTD                  *
 *         This file is distributed under the same license as OpenSSL *
 *                                                                    *
 *   ASN1 structure definition for GOST key transport                 *
 *          Requires OpenSSL 0.9.9 for compilation                    *
 **********************************************************************/

#include <openssl/opensslconf.h>

#ifndef OPENSSL_NO_GOST
#include <openssl/asn1t.h>
#include <openssl/x509.h>
#include <openssl/gost.h>

#include "gost_locl.h"
#include "gost_asn1.h"

static const ASN1_TEMPLATE GOST_KEY_TRANSPORT_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(GOST_KEY_TRANSPORT, key_info) ),
		Znk_DOTINIT( field_name, "key_info" ),
		Znk_DOTINIT( item, &GOST_KEY_INFO_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(GOST_KEY_TRANSPORT, key_agreement_info) ),
		Znk_DOTINIT( field_name, "key_agreement_info" ),
		Znk_DOTINIT( item, &GOST_KEY_AGREEMENT_INFO_it ),
	},
};

const ASN1_ITEM GOST_KEY_TRANSPORT_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_NDEF_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, GOST_KEY_TRANSPORT_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(GOST_KEY_TRANSPORT_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(GOST_KEY_TRANSPORT) ),
	Znk_DOTINIT( sname, "GOST_KEY_TRANSPORT" ),
};

GOST_KEY_TRANSPORT *
d2i_GOST_KEY_TRANSPORT(GOST_KEY_TRANSPORT **a, const unsigned char **in, long len)
{
	return (GOST_KEY_TRANSPORT *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &GOST_KEY_TRANSPORT_it);
}

int
i2d_GOST_KEY_TRANSPORT(GOST_KEY_TRANSPORT *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &GOST_KEY_TRANSPORT_it);
}

GOST_KEY_TRANSPORT *
GOST_KEY_TRANSPORT_new(void)
{
	return (GOST_KEY_TRANSPORT *)ASN1_item_new(&GOST_KEY_TRANSPORT_it);
}

void
GOST_KEY_TRANSPORT_free(GOST_KEY_TRANSPORT *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &GOST_KEY_TRANSPORT_it);
}

static const ASN1_TEMPLATE GOST_KEY_INFO_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(GOST_KEY_INFO, encrypted_key) ),
		Znk_DOTINIT( field_name, "encrypted_key" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(GOST_KEY_INFO, imit) ),
		Znk_DOTINIT( field_name, "imit" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
	},
};

const ASN1_ITEM GOST_KEY_INFO_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_NDEF_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, GOST_KEY_INFO_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(GOST_KEY_INFO_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(GOST_KEY_INFO) ),
	Znk_DOTINIT( sname, "GOST_KEY_INFO" ),
};

GOST_KEY_INFO *
d2i_GOST_KEY_INFO(GOST_KEY_INFO **a, const unsigned char **in, long len)
{
	return (GOST_KEY_INFO *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &GOST_KEY_INFO_it);
}

int
i2d_GOST_KEY_INFO(GOST_KEY_INFO *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &GOST_KEY_INFO_it);
}

GOST_KEY_INFO *
GOST_KEY_INFO_new(void)
{
	return (GOST_KEY_INFO *)ASN1_item_new(&GOST_KEY_INFO_it);
}

void
GOST_KEY_INFO_free(GOST_KEY_INFO *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &GOST_KEY_INFO_it);
}

static const ASN1_TEMPLATE GOST_KEY_AGREEMENT_INFO_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(GOST_KEY_AGREEMENT_INFO, cipher) ),
		Znk_DOTINIT( field_name, "cipher" ),
		Znk_DOTINIT( item, &ASN1_OBJECT_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_IMPLICIT | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(GOST_KEY_AGREEMENT_INFO, ephem_key) ),
		Znk_DOTINIT( field_name, "ephem_key" ),
		Znk_DOTINIT( item, &X509_PUBKEY_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(GOST_KEY_AGREEMENT_INFO, eph_iv) ),
		Znk_DOTINIT( field_name, "eph_iv" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
	},
};

const ASN1_ITEM GOST_KEY_AGREEMENT_INFO_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_NDEF_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, GOST_KEY_AGREEMENT_INFO_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(GOST_KEY_AGREEMENT_INFO_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(GOST_KEY_AGREEMENT_INFO) ),
	Znk_DOTINIT( sname, "GOST_KEY_AGREEMENT_INFO" ),
};

GOST_KEY_AGREEMENT_INFO *
d2i_GOST_KEY_AGREEMENT_INFO(GOST_KEY_AGREEMENT_INFO **a, const unsigned char **in, long len)
{
	return (GOST_KEY_AGREEMENT_INFO *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &GOST_KEY_AGREEMENT_INFO_it);
}

int
i2d_GOST_KEY_AGREEMENT_INFO(GOST_KEY_AGREEMENT_INFO *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &GOST_KEY_AGREEMENT_INFO_it);
}

GOST_KEY_AGREEMENT_INFO *
GOST_KEY_AGREEMENT_INFO_new(void)
{
	return (GOST_KEY_AGREEMENT_INFO *)ASN1_item_new(&GOST_KEY_AGREEMENT_INFO_it);
}

void
GOST_KEY_AGREEMENT_INFO_free(GOST_KEY_AGREEMENT_INFO *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &GOST_KEY_AGREEMENT_INFO_it);
}


static const ASN1_TEMPLATE GOST_KEY_PARAMS_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(GOST_KEY_PARAMS, key_params) ),
		Znk_DOTINIT( field_name, "key_params" ),
		Znk_DOTINIT( item, &ASN1_OBJECT_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(GOST_KEY_PARAMS, hash_params) ),
		Znk_DOTINIT( field_name, "hash_params" ),
		Znk_DOTINIT( item, &ASN1_OBJECT_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(GOST_KEY_PARAMS, cipher_params) ),
		Znk_DOTINIT( field_name, "cipher_params" ),
		Znk_DOTINIT( item, &ASN1_OBJECT_it ),
	},
};

const ASN1_ITEM GOST_KEY_PARAMS_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_NDEF_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, GOST_KEY_PARAMS_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(GOST_KEY_PARAMS_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(GOST_KEY_PARAMS) ),
	Znk_DOTINIT( sname, "GOST_KEY_PARAMS" ),
};

GOST_KEY_PARAMS *
d2i_GOST_KEY_PARAMS(GOST_KEY_PARAMS **a, const unsigned char **in, long len)
{
	return (GOST_KEY_PARAMS *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &GOST_KEY_PARAMS_it);
}

int
i2d_GOST_KEY_PARAMS(GOST_KEY_PARAMS *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &GOST_KEY_PARAMS_it);
}

GOST_KEY_PARAMS *
GOST_KEY_PARAMS_new(void)
{
	return (GOST_KEY_PARAMS *)ASN1_item_new(&GOST_KEY_PARAMS_it);
}

void
GOST_KEY_PARAMS_free(GOST_KEY_PARAMS *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &GOST_KEY_PARAMS_it);
}

static const ASN1_TEMPLATE GOST_CIPHER_PARAMS_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(GOST_CIPHER_PARAMS, iv) ),
		Znk_DOTINIT( field_name, "iv" ),
		Znk_DOTINIT( item, &ASN1_OCTET_STRING_it ),
	},
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(GOST_CIPHER_PARAMS, enc_param_set) ),
		Znk_DOTINIT( field_name, "enc_param_set" ),
		Znk_DOTINIT( item, &ASN1_OBJECT_it ),
	},
};

const ASN1_ITEM GOST_CIPHER_PARAMS_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_NDEF_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, GOST_CIPHER_PARAMS_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(GOST_CIPHER_PARAMS_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(GOST_CIPHER_PARAMS) ),
	Znk_DOTINIT( sname, "GOST_CIPHER_PARAMS" ),
};

GOST_CIPHER_PARAMS *
d2i_GOST_CIPHER_PARAMS(GOST_CIPHER_PARAMS **a, const unsigned char **in, long len)
{
	return (GOST_CIPHER_PARAMS *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &GOST_CIPHER_PARAMS_it);
}

int
i2d_GOST_CIPHER_PARAMS(GOST_CIPHER_PARAMS *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &GOST_CIPHER_PARAMS_it);
}

GOST_CIPHER_PARAMS *
GOST_CIPHER_PARAMS_new(void)
{
	return (GOST_CIPHER_PARAMS *)ASN1_item_new(&GOST_CIPHER_PARAMS_it);
}

void
GOST_CIPHER_PARAMS_free(GOST_CIPHER_PARAMS *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &GOST_CIPHER_PARAMS_it);
}

#endif
