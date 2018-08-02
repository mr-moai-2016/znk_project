/* $OpenBSD: m_wp.c,v 1.8 2014/07/13 09:30:02 miod Exp $ */

#include <stdio.h>

#include <openssl/opensslconf.h>

#ifndef OPENSSL_NO_WHIRLPOOL

#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/whrlpool.h>

static int
init(EVP_MD_CTX *ctx)
{
	return WHIRLPOOL_Init(ctx->md_data);
}

static int
update(EVP_MD_CTX *ctx, const void *data, size_t count)
{
	return WHIRLPOOL_Update(ctx->md_data, data, count);
}

static int
final(EVP_MD_CTX *ctx, unsigned char *md)
{
	return WHIRLPOOL_Final(md, ctx->md_data);
}

static const EVP_MD whirlpool_md = {
	Znk_DOTINIT( type, NID_whirlpool ),
	Znk_DOTINIT( pkey_type, 0 ),
	Znk_DOTINIT( md_size, WHIRLPOOL_DIGEST_LENGTH ),
	Znk_DOTINIT( flags, 0 ),
	Znk_DOTINIT( init, init ),
	Znk_DOTINIT( update, update ),
	Znk_DOTINIT( final, final ),
	Znk_DOTINIT( copy, NULL ),
	Znk_DOTINIT( cleanup, NULL ),
	Znk_DOTINIT( sign, NULL ),
	Znk_DOTINIT( verify, NULL ),
	Znk_DOTINIT_GROUP( required_pkey_type ) {
		0, 0, 0, 0,
	},
	Znk_DOTINIT( block_size, WHIRLPOOL_BBLOCK / 8 ),
	Znk_DOTINIT( ctx_size, sizeof(EVP_MD *) + sizeof(WHIRLPOOL_CTX) ),
};

const EVP_MD *
EVP_whirlpool(void)
{
	return (&whirlpool_md);
}
#endif
