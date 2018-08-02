/* $OpenBSD: t1_clnt.c,v 1.24 2017/01/26 05:31:25 jsing Exp $ */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#include <stdio.h>

#include "ssl_locl.h"

#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/objects.h>

static const SSL_METHOD_INTERNAL TLS_client_method_internal_data = {
	Znk_DOTINIT( version, TLS1_2_VERSION ),
	Znk_DOTINIT( min_version, TLS1_VERSION ),
	Znk_DOTINIT( max_version, TLS1_2_VERSION ),
	Znk_DOTINIT( ssl_new, tls1_new ),
	Znk_DOTINIT( ssl_clear, tls1_clear ),
	Znk_DOTINIT( ssl_free, tls1_free ),
	Znk_DOTINIT( ssl_accept, ssl_undefined_function ),
	Znk_DOTINIT( ssl_connect, ssl3_connect ),
	Znk_DOTINIT( ssl_read, ssl3_read ),
	Znk_DOTINIT( ssl_peek, ssl3_peek ),
	Znk_DOTINIT( ssl_write, ssl3_write ),
	Znk_DOTINIT( ssl_shutdown, ssl3_shutdown ),
	Znk_DOTINIT( ssl_renegotiate, ssl_undefined_function ),
	Znk_DOTINIT( ssl_renegotiate_check, ssl_ok ),
	Znk_DOTINIT( ssl_get_message, ssl3_get_message ),
	Znk_DOTINIT( ssl_read_bytes, ssl3_read_bytes ),
	Znk_DOTINIT( ssl_write_bytes, ssl3_write_bytes ),
	Znk_DOTINIT( ssl_pending, ssl3_pending ),
	Znk_DOTINIT( get_ssl_method, tls1_get_client_method ),
	Znk_DOTINIT( get_timeout, tls1_default_timeout ),
	Znk_DOTINIT( ssl_version, ssl_undefined_void_function ),
	Znk_DOTINIT( ssl3_enc, &TLSv1_2_enc_data ),
};

static const SSL_METHOD TLS_client_method_data = {
	Znk_DOTINIT( ssl_dispatch_alert, ssl3_dispatch_alert ),
	Znk_DOTINIT( num_ciphers, ssl3_num_ciphers ),
	Znk_DOTINIT( get_cipher, ssl3_get_cipher ),
	Znk_DOTINIT( get_cipher_by_char, ssl3_get_cipher_by_char ),
	Znk_DOTINIT( put_cipher_by_char, ssl3_put_cipher_by_char ),
	Znk_DOTINIT( internal, &TLS_client_method_internal_data ),
};

static const SSL_METHOD_INTERNAL TLSv1_client_method_internal_data = {
	Znk_DOTINIT( version, TLS1_VERSION ),
	Znk_DOTINIT( min_version, TLS1_VERSION ),
	Znk_DOTINIT( max_version, TLS1_VERSION ),
	Znk_DOTINIT( ssl_new, tls1_new ),
	Znk_DOTINIT( ssl_clear, tls1_clear ),
	Znk_DOTINIT( ssl_free, tls1_free ),
	Znk_DOTINIT( ssl_accept, ssl_undefined_function ),
	Znk_DOTINIT( ssl_connect, ssl3_connect ),
	Znk_DOTINIT( ssl_read, ssl3_read ),
	Znk_DOTINIT( ssl_peek, ssl3_peek ),
	Znk_DOTINIT( ssl_write, ssl3_write ),
	Znk_DOTINIT( ssl_shutdown, ssl3_shutdown ),
	Znk_DOTINIT( ssl_renegotiate, ssl3_renegotiate ),
	Znk_DOTINIT( ssl_renegotiate_check, ssl3_renegotiate_check ),
	Znk_DOTINIT( ssl_get_message, ssl3_get_message ),
	Znk_DOTINIT( ssl_read_bytes, ssl3_read_bytes ),
	Znk_DOTINIT( ssl_write_bytes, ssl3_write_bytes ),
	Znk_DOTINIT( ssl_pending, ssl3_pending ),
	Znk_DOTINIT( get_ssl_method, tls1_get_client_method ),
	Znk_DOTINIT( get_timeout, tls1_default_timeout ),
	Znk_DOTINIT( ssl_version, ssl_undefined_void_function ),
	Znk_DOTINIT( ssl3_enc, &TLSv1_enc_data ),
};

static const SSL_METHOD TLSv1_client_method_data = {
	Znk_DOTINIT( ssl_dispatch_alert, ssl3_dispatch_alert ),
	Znk_DOTINIT( num_ciphers, ssl3_num_ciphers ),
	Znk_DOTINIT( get_cipher, ssl3_get_cipher ),
	Znk_DOTINIT( get_cipher_by_char, ssl3_get_cipher_by_char ),
	Znk_DOTINIT( put_cipher_by_char, ssl3_put_cipher_by_char ),
	Znk_DOTINIT( internal, &TLSv1_client_method_internal_data ),
};

static const SSL_METHOD_INTERNAL TLSv1_1_client_method_internal_data = {
	Znk_DOTINIT( version, TLS1_1_VERSION ),
	Znk_DOTINIT( min_version, TLS1_1_VERSION ),
	Znk_DOTINIT( max_version, TLS1_1_VERSION ),
	Znk_DOTINIT( ssl_new, tls1_new ),
	Znk_DOTINIT( ssl_clear, tls1_clear ),
	Znk_DOTINIT( ssl_free, tls1_free ),
	Znk_DOTINIT( ssl_accept, ssl_undefined_function ),
	Znk_DOTINIT( ssl_connect, ssl3_connect ),
	Znk_DOTINIT( ssl_read, ssl3_read ),
	Znk_DOTINIT( ssl_peek, ssl3_peek ),
	Znk_DOTINIT( ssl_write, ssl3_write ),
	Znk_DOTINIT( ssl_shutdown, ssl3_shutdown ),
	Znk_DOTINIT( ssl_renegotiate, ssl3_renegotiate ),
	Znk_DOTINIT( ssl_renegotiate_check, ssl3_renegotiate_check ),
	Znk_DOTINIT( ssl_get_message, ssl3_get_message ),
	Znk_DOTINIT( ssl_read_bytes, ssl3_read_bytes ),
	Znk_DOTINIT( ssl_write_bytes, ssl3_write_bytes ),
	Znk_DOTINIT( ssl_pending, ssl3_pending ),
	Znk_DOTINIT( get_ssl_method, tls1_get_client_method ),
	Znk_DOTINIT( get_timeout, tls1_default_timeout ),
	Znk_DOTINIT( ssl_version, ssl_undefined_void_function ),
	Znk_DOTINIT( ssl3_enc, &TLSv1_1_enc_data ),
};

static const SSL_METHOD TLSv1_1_client_method_data = {
	Znk_DOTINIT( ssl_dispatch_alert, ssl3_dispatch_alert ),
	Znk_DOTINIT( num_ciphers, ssl3_num_ciphers ),
	Znk_DOTINIT( get_cipher, ssl3_get_cipher ),
	Znk_DOTINIT( get_cipher_by_char, ssl3_get_cipher_by_char ),
	Znk_DOTINIT( put_cipher_by_char, ssl3_put_cipher_by_char ),
	Znk_DOTINIT( internal, &TLSv1_1_client_method_internal_data ),
};

static const SSL_METHOD_INTERNAL TLSv1_2_client_method_internal_data = {
	Znk_DOTINIT( version, TLS1_2_VERSION ),
	Znk_DOTINIT( min_version, TLS1_2_VERSION ),
	Znk_DOTINIT( max_version, TLS1_2_VERSION ),
	Znk_DOTINIT( ssl_new, tls1_new ),
	Znk_DOTINIT( ssl_clear, tls1_clear ),
	Znk_DOTINIT( ssl_free, tls1_free ),
	Znk_DOTINIT( ssl_accept, ssl_undefined_function ),
	Znk_DOTINIT( ssl_connect, ssl3_connect ),
	Znk_DOTINIT( ssl_read, ssl3_read ),
	Znk_DOTINIT( ssl_peek, ssl3_peek ),
	Znk_DOTINIT( ssl_write, ssl3_write ),
	Znk_DOTINIT( ssl_shutdown, ssl3_shutdown ),
	Znk_DOTINIT( ssl_renegotiate, ssl3_renegotiate ),
	Znk_DOTINIT( ssl_renegotiate_check, ssl3_renegotiate_check ),
	Znk_DOTINIT( ssl_get_message, ssl3_get_message ),
	Znk_DOTINIT( ssl_read_bytes, ssl3_read_bytes ),
	Znk_DOTINIT( ssl_write_bytes, ssl3_write_bytes ),
	Znk_DOTINIT( ssl_pending, ssl3_pending ),
	Znk_DOTINIT( get_ssl_method, tls1_get_client_method ),
	Znk_DOTINIT( get_timeout, tls1_default_timeout ),
	Znk_DOTINIT( ssl_version, ssl_undefined_void_function ),
	Znk_DOTINIT( ssl3_enc, &TLSv1_2_enc_data ),
};

static const SSL_METHOD TLSv1_2_client_method_data = {
	Znk_DOTINIT( ssl_dispatch_alert, ssl3_dispatch_alert ),
	Znk_DOTINIT( num_ciphers, ssl3_num_ciphers ),
	Znk_DOTINIT( get_cipher, ssl3_get_cipher ),
	Znk_DOTINIT( get_cipher_by_char, ssl3_get_cipher_by_char ),
	Znk_DOTINIT( put_cipher_by_char, ssl3_put_cipher_by_char ),
	Znk_DOTINIT( internal, &TLSv1_2_client_method_internal_data ),
};

const SSL_METHOD *
tls1_get_client_method(int ver)
{
	if (ver == TLS1_2_VERSION)
		return (TLSv1_2_client_method());
	if (ver == TLS1_1_VERSION)
		return (TLSv1_1_client_method());
	if (ver == TLS1_VERSION)
		return (TLSv1_client_method());
	return (NULL);
}

const SSL_METHOD *
SSLv23_client_method(void)
{
	return (TLS_client_method());
}

const SSL_METHOD *
TLS_client_method(void)
{
	return (&TLS_client_method_data);
}

const SSL_METHOD *
TLSv1_client_method(void)
{
	return (&TLSv1_client_method_data);
}

const SSL_METHOD *
TLSv1_1_client_method(void)
{
	return (&TLSv1_1_client_method_data);
}

const SSL_METHOD *
TLSv1_2_client_method(void)
{
	return (&TLSv1_2_client_method_data);
}
