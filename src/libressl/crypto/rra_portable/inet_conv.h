/*
 * Wrapper around inet_ntoa, inet_aton, and inet_ntop.
 *
 * Copyright 2008 Board of Trustees, Leland Stanford Jr. University
 * Copyright (c) 2004, 2005, 2006, 2007
 *     by Internet Systems Consortium, Inc. ("ISC")
 * Copyright (c) 1991, 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001,
 *     2002, 2003 by The Internet Software Consortium and Rich Salz
 *
 * This code is derived from software contributed to the Internet Software
 * Consortium by Rich Salz.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified by Zenkaku( https://github.com/mr-moai-2016/znk_project ).
 */

#ifndef RRA_PORTABLE_INET_CONV_H
#define RRA_PORTABLE_INET_CONV_H 1

#include <rra_portable/macros.h>

#include <errno.h>
#include <sys/types.h>

/* BSDI needs <netinet/in.h> before <arpa/inet.h>. */
#ifdef _WIN32
# include <winsock2.h>
# include <ws2tcpip.h>
#else
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <sys/socket.h>
#endif

BEGIN_DECLS

/*
 * Provide prototypes for inet_aton and inet_ntop if not prototyped in the
 * system header files since they're occasionally available without proper
 * prototypes.
 */
int rra_inet_aton( const char*, struct in_addr* );

const char* rra_inet_ntoa( const struct in_addr in, char* buf, size_t buf_size );

char*       rra_inet_ntop(int af, const void *src, char *buf, size_t size);
int         rra_inet_pton( int af, const char* src, void* dst );

END_DECLS

#endif /* !RRA_PORTABLE_INET_CONV_H */
