/* $OpenBSD: v3_cpols.c,v 1.25 2017/01/29 17:49:23 beck Exp $ */
/* Written by Dr Stephen N Henson (steve@openssl.org) for the OpenSSL
 * project 1999.
 */
/* ====================================================================
 * Copyright (c) 1999-2004 The OpenSSL Project.  All rights reserved.
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
#include <string.h>

#include <openssl/asn1.h>
#include <openssl/asn1t.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/x509v3.h>

#include "pcy_int.h"

/* Certificate policies extension support: this one is a bit complex... */

static int i2r_certpol(X509V3_EXT_METHOD *method, STACK_OF(POLICYINFO) *pol,
    BIO *out, int indent);
static STACK_OF(POLICYINFO) *r2i_certpol(X509V3_EXT_METHOD *method,
    X509V3_CTX *ctx, char *value);
static void print_qualifiers(BIO *out, STACK_OF(POLICYQUALINFO) *quals,
    int indent);
static void print_notice(BIO *out, USERNOTICE *notice, int indent);
static POLICYINFO *policy_section(X509V3_CTX *ctx,
    STACK_OF(CONF_VALUE) *polstrs, int ia5org);
static POLICYQUALINFO *notice_section(X509V3_CTX *ctx,
    STACK_OF(CONF_VALUE) *unot, int ia5org);
static int nref_nos(STACK_OF(ASN1_INTEGER) *nnums, STACK_OF(CONF_VALUE) *nos);

const X509V3_EXT_METHOD v3_cpols = {
	Znk_DOTINIT( ext_nid, NID_certificate_policies ),
	Znk_DOTINIT( ext_flags, 0 ),
	Znk_DOTINIT( it, &CERTIFICATEPOLICIES_it ),
	Znk_DOTINIT( ext_new, NULL ),
	Znk_DOTINIT( ext_free, NULL ),
	Znk_DOTINIT( d2i, NULL ),
	Znk_DOTINIT( i2d, NULL ),
	Znk_DOTINIT( i2s, NULL ),
	Znk_DOTINIT( s2i, NULL ),
	Znk_DOTINIT( i2v, NULL ),
	Znk_DOTINIT( v2i, NULL ),
	Znk_DOTINIT( i2r, (X509V3_EXT_I2R)i2r_certpol ),
	Znk_DOTINIT( r2i, (X509V3_EXT_R2I)r2i_certpol ),
	Znk_DOTINIT( usr_data, NULL ),
};

static const ASN1_TEMPLATE CERTIFICATEPOLICIES_item_tt = {
	Znk_DOTINIT( flags, ASN1_TFLG_SEQUENCE_OF ),
	Znk_DOTINIT( tag, 0 ),
	Znk_DOTINIT( offset, 0 ),
	Znk_DOTINIT( field_name, "CERTIFICATEPOLICIES" ),
	Znk_DOTINIT( item, &POLICYINFO_it ),
};

const ASN1_ITEM CERTIFICATEPOLICIES_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_PRIMITIVE ),
	Znk_DOTINIT( utype, -1 ),
	Znk_DOTINIT( templates, &CERTIFICATEPOLICIES_item_tt ),
	Znk_DOTINIT( tcount, 0 ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, 0 ),
	Znk_DOTINIT( sname, "CERTIFICATEPOLICIES" ),
};


CERTIFICATEPOLICIES *
d2i_CERTIFICATEPOLICIES(CERTIFICATEPOLICIES **a, const unsigned char **in, long len)
{
	return (CERTIFICATEPOLICIES *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &CERTIFICATEPOLICIES_it);
}

int
i2d_CERTIFICATEPOLICIES(CERTIFICATEPOLICIES *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &CERTIFICATEPOLICIES_it);
}

CERTIFICATEPOLICIES *
CERTIFICATEPOLICIES_new(void)
{
	return (CERTIFICATEPOLICIES *)ASN1_item_new(&CERTIFICATEPOLICIES_it);
}

void
CERTIFICATEPOLICIES_free(CERTIFICATEPOLICIES *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &CERTIFICATEPOLICIES_it);
}

static const ASN1_TEMPLATE POLICYINFO_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(POLICYINFO, policyid) ),
		Znk_DOTINIT( field_name, "policyid" ),
		Znk_DOTINIT( item, &ASN1_OBJECT_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SEQUENCE_OF | ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(POLICYINFO, qualifiers) ),
		Znk_DOTINIT( field_name, "qualifiers" ),
		Znk_DOTINIT( item, &POLICYQUALINFO_it ),
	},
};

const ASN1_ITEM POLICYINFO_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, POLICYINFO_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(POLICYINFO_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(POLICYINFO) ),
	Znk_DOTINIT( sname, "POLICYINFO" ),
};


POLICYINFO *
d2i_POLICYINFO(POLICYINFO **a, const unsigned char **in, long len)
{
	return (POLICYINFO *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &POLICYINFO_it);
}

int
i2d_POLICYINFO(POLICYINFO *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &POLICYINFO_it);
}

POLICYINFO *
POLICYINFO_new(void)
{
	return (POLICYINFO *)ASN1_item_new(&POLICYINFO_it);
}

void
POLICYINFO_free(POLICYINFO *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &POLICYINFO_it);
}

static const ASN1_TEMPLATE policydefault_tt = {
	Znk_DOTINIT( flags, 0 ),
	Znk_DOTINIT( tag, 0 ),
	Znk_DOTINIT( offset, offsetof(POLICYQUALINFO, d.other) ),
	Znk_DOTINIT( field_name, "d.other" ),
	Znk_DOTINIT( item, &ASN1_ANY_it ),
};

static const ASN1_ADB_TABLE POLICYQUALINFO_adbtbl[] = {
	{
		Znk_DOTINIT( value, NID_id_qt_cps ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, 0 ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(POLICYQUALINFO, d.cpsuri) ),
			Znk_DOTINIT( field_name, "d.cpsuri" ),
			Znk_DOTINIT( item, &ASN1_IA5STRING_it ),
		},
	
	},
	{
		Znk_DOTINIT( value, NID_id_qt_unotice ),
		Znk_DOTINIT_GROUP( tt ) {
			Znk_DOTINIT( flags, 0 ),
			Znk_DOTINIT( tag, 0 ),
			Znk_DOTINIT( offset, offsetof(POLICYQUALINFO, d.usernotice) ),
			Znk_DOTINIT( field_name, "d.usernotice" ),
			Znk_DOTINIT( item, &USERNOTICE_it ),
		},
	
	},
};

static const ASN1_ADB POLICYQUALINFO_adb = {
	Znk_DOTINIT( flags, 0 ),
	Znk_DOTINIT( offset, offsetof(POLICYQUALINFO, pqualid) ),
	Znk_DOTINIT( app_items, 0 ),
	Znk_DOTINIT( tbl, POLICYQUALINFO_adbtbl ),
	Znk_DOTINIT( tblcount, sizeof(POLICYQUALINFO_adbtbl) / sizeof(ASN1_ADB_TABLE) ),
	Znk_DOTINIT( default_tt, &policydefault_tt ),
	Znk_DOTINIT( null_tt, NULL ),
};

static const ASN1_TEMPLATE POLICYQUALINFO_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(POLICYQUALINFO, pqualid) ),
		Znk_DOTINIT( field_name, "pqualid" ),
		Znk_DOTINIT( item, &ASN1_OBJECT_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_ADB_OID ),
		Znk_DOTINIT( tag, -1 ),
		Znk_DOTINIT( offset, 0 ),
		Znk_DOTINIT( field_name, "POLICYQUALINFO" ),
		Znk_DOTINIT( item, (const ASN1_ITEM *)&POLICYQUALINFO_adb ),
	},
};

const ASN1_ITEM POLICYQUALINFO_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, POLICYQUALINFO_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(POLICYQUALINFO_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(POLICYQUALINFO) ),
	Znk_DOTINIT( sname, "POLICYQUALINFO" ),
};


POLICYQUALINFO *
d2i_POLICYQUALINFO(POLICYQUALINFO **a, const unsigned char **in, long len)
{
	return (POLICYQUALINFO *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &POLICYQUALINFO_it);
}

int
i2d_POLICYQUALINFO(POLICYQUALINFO *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &POLICYQUALINFO_it);
}

POLICYQUALINFO *
POLICYQUALINFO_new(void)
{
	return (POLICYQUALINFO *)ASN1_item_new(&POLICYQUALINFO_it);
}

void
POLICYQUALINFO_free(POLICYQUALINFO *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &POLICYQUALINFO_it);
}

static const ASN1_TEMPLATE USERNOTICE_seq_tt[] = {
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(USERNOTICE, noticeref) ),
		Znk_DOTINIT( field_name, "noticeref" ),
		Znk_DOTINIT( item, &NOTICEREF_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_OPTIONAL ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(USERNOTICE, exptext) ),
		Znk_DOTINIT( field_name, "exptext" ),
		Znk_DOTINIT( item, &DISPLAYTEXT_it ),
	},
};

const ASN1_ITEM USERNOTICE_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, USERNOTICE_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(USERNOTICE_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(USERNOTICE) ),
	Znk_DOTINIT( sname, "USERNOTICE" ),
};


USERNOTICE *
d2i_USERNOTICE(USERNOTICE **a, const unsigned char **in, long len)
{
	return (USERNOTICE *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &USERNOTICE_it);
}

int
i2d_USERNOTICE(USERNOTICE *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &USERNOTICE_it);
}

USERNOTICE *
USERNOTICE_new(void)
{
	return (USERNOTICE *)ASN1_item_new(&USERNOTICE_it);
}

void
USERNOTICE_free(USERNOTICE *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &USERNOTICE_it);
}

static const ASN1_TEMPLATE NOTICEREF_seq_tt[] = {
	{
		Znk_DOTINIT( flags, 0 ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(NOTICEREF, organization) ),
		Znk_DOTINIT( field_name, "organization" ),
		Znk_DOTINIT( item, &DISPLAYTEXT_it ),
	},
	{
		Znk_DOTINIT( flags, ASN1_TFLG_SEQUENCE_OF ),
		Znk_DOTINIT( tag, 0 ),
		Znk_DOTINIT( offset, offsetof(NOTICEREF, noticenos) ),
		Znk_DOTINIT( field_name, "noticenos" ),
		Znk_DOTINIT( item, &ASN1_INTEGER_it ),
	},
};

const ASN1_ITEM NOTICEREF_it = {
	Znk_DOTINIT( itype, ASN1_ITYPE_SEQUENCE ),
	Znk_DOTINIT( utype, V_ASN1_SEQUENCE ),
	Znk_DOTINIT( templates, NOTICEREF_seq_tt ),
	Znk_DOTINIT( tcount, sizeof(NOTICEREF_seq_tt) / sizeof(ASN1_TEMPLATE) ),
	Znk_DOTINIT( funcs, NULL ),
	Znk_DOTINIT( size, sizeof(NOTICEREF) ),
	Znk_DOTINIT( sname, "NOTICEREF" ),
};


NOTICEREF *
d2i_NOTICEREF(NOTICEREF **a, const unsigned char **in, long len)
{
	return (NOTICEREF *)ASN1_item_d2i((ASN1_VALUE **)a, in, len,
	    &NOTICEREF_it);
}

int
i2d_NOTICEREF(NOTICEREF *a, unsigned char **out)
{
	return ASN1_item_i2d((ASN1_VALUE *)a, out, &NOTICEREF_it);
}

NOTICEREF *
NOTICEREF_new(void)
{
	return (NOTICEREF *)ASN1_item_new(&NOTICEREF_it);
}

void
NOTICEREF_free(NOTICEREF *a)
{
	ASN1_item_free((ASN1_VALUE *)a, &NOTICEREF_it);
}

static
STACK_OF(POLICYINFO) *r2i_certpol(X509V3_EXT_METHOD *method, X509V3_CTX *ctx,
    char *value)
{
	STACK_OF(POLICYINFO) *pols = NULL;
	char *pstr;
	POLICYINFO *pol;
	ASN1_OBJECT *pobj;
	STACK_OF(CONF_VALUE) *vals;
	CONF_VALUE *cnf;
	int i, ia5org;

	pols = sk_POLICYINFO_new_null();
	if (pols == NULL) {
		X509V3error(ERR_R_MALLOC_FAILURE);
		return NULL;
	}
	vals = X509V3_parse_list(value);
	if (vals == NULL) {
		X509V3error(ERR_R_X509V3_LIB);
		goto err;
	}
	ia5org = 0;
	for (i = 0; i < sk_CONF_VALUE_num(vals); i++) {
		cnf = sk_CONF_VALUE_value(vals, i);
		if (cnf->value || !cnf->name) {
			X509V3error(X509V3_R_INVALID_POLICY_IDENTIFIER);
			X509V3_conf_err(cnf);
			goto err;
		}
		pstr = cnf->name;
		if (!strcmp(pstr, "ia5org")) {
			ia5org = 1;
			continue;
		} else if (*pstr == '@') {
			STACK_OF(CONF_VALUE) *polsect;
			polsect = X509V3_get_section(ctx, pstr + 1);
			if (!polsect) {
				X509V3error(X509V3_R_INVALID_SECTION);
				X509V3_conf_err(cnf);
				goto err;
			}
			pol = policy_section(ctx, polsect, ia5org);
			X509V3_section_free(ctx, polsect);
			if (!pol)
				goto err;
		} else {
			if (!(pobj = OBJ_txt2obj(cnf->name, 0))) {
				X509V3error(X509V3_R_INVALID_OBJECT_IDENTIFIER);
				X509V3_conf_err(cnf);
				goto err;
			}
			pol = POLICYINFO_new();
			pol->policyid = pobj;
		}
		if (!sk_POLICYINFO_push(pols, pol)){
			POLICYINFO_free(pol);
			X509V3error(ERR_R_MALLOC_FAILURE);
			goto err;
		}
	}
	sk_CONF_VALUE_pop_free(vals, X509V3_conf_free);
	return pols;

err:
	sk_CONF_VALUE_pop_free(vals, X509V3_conf_free);
	sk_POLICYINFO_pop_free(pols, POLICYINFO_free);
	return NULL;
}

static POLICYINFO *
policy_section(X509V3_CTX *ctx, STACK_OF(CONF_VALUE) *polstrs, int ia5org)
{
	int i;
	CONF_VALUE *cnf;
	POLICYINFO *pol;
	POLICYQUALINFO *nqual = NULL;

	if ((pol = POLICYINFO_new()) == NULL)
		goto merr;
	for (i = 0; i < sk_CONF_VALUE_num(polstrs); i++) {
		cnf = sk_CONF_VALUE_value(polstrs, i);
		if (strcmp(cnf->name, "policyIdentifier") == 0) {
			ASN1_OBJECT *pobj;

			if ((pobj = OBJ_txt2obj(cnf->value, 0)) == NULL) {
				X509V3error(X509V3_R_INVALID_OBJECT_IDENTIFIER);
				X509V3_conf_err(cnf);
				goto err;
			}
			pol->policyid = pobj;
		} else if (name_cmp(cnf->name, "CPS") == 0) {
			if ((nqual = POLICYQUALINFO_new()) == NULL)
				goto merr;
			nqual->pqualid = OBJ_nid2obj(NID_id_qt_cps);
			nqual->d.cpsuri = ASN1_IA5STRING_new();
			if (nqual->d.cpsuri == NULL)
				goto merr;
			if (ASN1_STRING_set(nqual->d.cpsuri, cnf->value,
			    strlen(cnf->value)) == 0)
				goto merr;

			if (pol->qualifiers == NULL) {
				pol->qualifiers = sk_POLICYQUALINFO_new_null();
				if (pol->qualifiers == NULL)
					goto merr;
			}
			if (sk_POLICYQUALINFO_push(pol->qualifiers, nqual) == 0)
				goto merr;
			nqual = NULL;
		} else if (name_cmp(cnf->name, "userNotice") == 0) {
			STACK_OF(CONF_VALUE) *unot;
			POLICYQUALINFO *qual;

			if (*cnf->value != '@') {
				X509V3error(X509V3_R_EXPECTED_A_SECTION_NAME);
				X509V3_conf_err(cnf);
				goto err;
			}
			unot = X509V3_get_section(ctx, cnf->value + 1);
			if (unot == NULL) {
				X509V3error(X509V3_R_INVALID_SECTION);
				X509V3_conf_err(cnf);
				goto err;
			}
			qual = notice_section(ctx, unot, ia5org);
			X509V3_section_free(ctx, unot);
			if (qual == NULL)
				goto err;

			if (pol->qualifiers == NULL) {
				pol->qualifiers = sk_POLICYQUALINFO_new_null();
				if (pol->qualifiers == NULL)
					goto merr;
			}
			if (sk_POLICYQUALINFO_push(pol->qualifiers, qual) == 0)
				goto merr;
		} else {
			X509V3error(X509V3_R_INVALID_OPTION);
			X509V3_conf_err(cnf);
			goto err;
		}
	}
	if (pol->policyid == NULL) {
		X509V3error(X509V3_R_NO_POLICY_IDENTIFIER);
		goto err;
	}

	return pol;

merr:
	X509V3error(ERR_R_MALLOC_FAILURE);

err:
	POLICYQUALINFO_free(nqual);
	POLICYINFO_free(pol);
	return NULL;
}

static POLICYQUALINFO *
notice_section(X509V3_CTX *ctx, STACK_OF(CONF_VALUE) *unot, int ia5org)
{
	int i, ret;
	CONF_VALUE *cnf;
	USERNOTICE *not;
	POLICYQUALINFO *qual;

	if (!(qual = POLICYQUALINFO_new()))
		goto merr;
	qual->pqualid = OBJ_nid2obj(NID_id_qt_unotice);
	if (!(not = USERNOTICE_new()))
		goto merr;
	qual->d.usernotice = not;
	for (i = 0; i < sk_CONF_VALUE_num(unot); i++) {
		cnf = sk_CONF_VALUE_value(unot, i);
		if (!strcmp(cnf->name, "explicitText")) {
			if (not->exptext == NULL) {
				not->exptext = ASN1_VISIBLESTRING_new();
				if (not->exptext == NULL)
					goto merr;
			}
			if (!ASN1_STRING_set(not->exptext, cnf->value,
			    strlen(cnf->value)))
				goto merr;
		} else if (!strcmp(cnf->name, "organization")) {
			NOTICEREF *nref;
			if (!not->noticeref) {
				if (!(nref = NOTICEREF_new()))
					goto merr;
				not->noticeref = nref;
			} else
				nref = not->noticeref;
			if (ia5org)
				nref->organization->type = V_ASN1_IA5STRING;
			else
				nref->organization->type = V_ASN1_VISIBLESTRING;
			if (!ASN1_STRING_set(nref->organization, cnf->value,
			    strlen(cnf->value)))
				goto merr;
		} else if (!strcmp(cnf->name, "noticeNumbers")) {
			NOTICEREF *nref;
			STACK_OF(CONF_VALUE) *nos;
			if (!not->noticeref) {
				if (!(nref = NOTICEREF_new()))
					goto merr;
				not->noticeref = nref;
			} else
				nref = not->noticeref;
			nos = X509V3_parse_list(cnf->value);
			if (!nos || !sk_CONF_VALUE_num(nos)) {
				X509V3error(X509V3_R_INVALID_NUMBERS);
				X509V3_conf_err(cnf);
				if (nos != NULL)
					sk_CONF_VALUE_pop_free(nos,
					    X509V3_conf_free);
				goto err;
			}
			ret = nref_nos(nref->noticenos, nos);
			sk_CONF_VALUE_pop_free(nos, X509V3_conf_free);
			if (!ret)
				goto err;
		} else {
			X509V3error(X509V3_R_INVALID_OPTION);
			X509V3_conf_err(cnf);
			goto err;
		}
	}

	if (not->noticeref &&
	    (!not->noticeref->noticenos || !not->noticeref->organization)) {
		X509V3error(X509V3_R_NEED_ORGANIZATION_AND_NUMBERS);
		goto err;
	}

	return qual;

merr:
	X509V3error(ERR_R_MALLOC_FAILURE);

err:
	POLICYQUALINFO_free(qual);
	return NULL;
}

static int
nref_nos(STACK_OF(ASN1_INTEGER) *nnums, STACK_OF(CONF_VALUE) *nos)
{
	CONF_VALUE *cnf;
	ASN1_INTEGER *aint;
	int i;

	for (i = 0; i < sk_CONF_VALUE_num(nos); i++) {
		cnf = sk_CONF_VALUE_value(nos, i);
		if (!(aint = s2i_ASN1_INTEGER(NULL, cnf->name))) {
			X509V3error(X509V3_R_INVALID_NUMBER);
			goto err;
		}
		if (!sk_ASN1_INTEGER_push(nnums, aint))
			goto merr;
	}
	return 1;

merr:
	X509V3error(ERR_R_MALLOC_FAILURE);

err:
	sk_ASN1_INTEGER_pop_free(nnums, ASN1_STRING_free);
	return 0;
}

static int
i2r_certpol(X509V3_EXT_METHOD *method, STACK_OF(POLICYINFO) *pol, BIO *out,
    int indent)
{
	int i;
	POLICYINFO *pinfo;

	/* First print out the policy OIDs */
	for (i = 0; i < sk_POLICYINFO_num(pol); i++) {
		pinfo = sk_POLICYINFO_value(pol, i);
		BIO_printf(out, "%*sPolicy: ", indent, "");
		i2a_ASN1_OBJECT(out, pinfo->policyid);
		BIO_puts(out, "\n");
		if (pinfo->qualifiers)
			print_qualifiers(out, pinfo->qualifiers, indent + 2);
	}
	return 1;
}

static void
print_qualifiers(BIO *out, STACK_OF(POLICYQUALINFO) *quals, int indent)
{
	POLICYQUALINFO *qualinfo;
	int i;

	for (i = 0; i < sk_POLICYQUALINFO_num(quals); i++) {
		qualinfo = sk_POLICYQUALINFO_value(quals, i);
		switch (OBJ_obj2nid(qualinfo->pqualid)) {
		case NID_id_qt_cps:
			BIO_printf(out, "%*sCPS: %s\n", indent, "",
			    qualinfo->d.cpsuri->data);
			break;

		case NID_id_qt_unotice:
			BIO_printf(out, "%*sUser Notice:\n", indent, "");
			print_notice(out, qualinfo->d.usernotice, indent + 2);
			break;

		default:
			BIO_printf(out, "%*sUnknown Qualifier: ",
			    indent + 2, "");

			i2a_ASN1_OBJECT(out, qualinfo->pqualid);
			BIO_puts(out, "\n");
			break;
		}
	}
}

static void
print_notice(BIO *out, USERNOTICE *notice, int indent)
{
	int i;

	if (notice->noticeref) {
		NOTICEREF *ref;
		ref = notice->noticeref;
		BIO_printf(out, "%*sOrganization: %s\n", indent, "",
		    ref->organization->data);
		BIO_printf(out, "%*sNumber%s: ", indent, "",
		    sk_ASN1_INTEGER_num(ref->noticenos) > 1 ? "s" : "");
		for (i = 0; i < sk_ASN1_INTEGER_num(ref->noticenos); i++) {
			ASN1_INTEGER *num;
			char *tmp;
			num = sk_ASN1_INTEGER_value(ref->noticenos, i);
			if (i)
				BIO_puts(out, ", ");
			tmp = i2s_ASN1_INTEGER(NULL, num);
			BIO_puts(out, tmp);
			free(tmp);
		}
		BIO_puts(out, "\n");
	}
	if (notice->exptext)
		BIO_printf(out, "%*sExplicit Text: %s\n", indent, "",
		    notice->exptext->data);
}

void
X509_POLICY_NODE_print(BIO *out, X509_POLICY_NODE *node, int indent)
{
	const X509_POLICY_DATA *dat = node->data;

	BIO_printf(out, "%*sPolicy: ", indent, "");

	i2a_ASN1_OBJECT(out, dat->valid_policy);
	BIO_puts(out, "\n");
	BIO_printf(out, "%*s%s\n", indent + 2, "",
	    node_data_critical(dat) ? "Critical" : "Non Critical");
	if (dat->qualifier_set)
		print_qualifiers(out, dat->qualifier_set, indent + 2);
	else
		BIO_printf(out, "%*sNo Qualifiers\n", indent + 2, "");
}
