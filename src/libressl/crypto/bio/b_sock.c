/* $OpenBSD: b_sock.c,v 1.69 2018/02/07 00:52:05 bluhm Exp $ */
/*
 * Copyright (c) 2017 Bob Beck <beck@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/err.h>

#include <rra_portable/getaddrinfo.h>
#include <rra_portable/getnameinfo.h>

int
BIO_get_host_ip(const char *str, unsigned char *ip)
{
	struct rra_addrinfo *res = NULL;
	struct rra_addrinfo hints = { 0 };
	uint32_t *iap = (in_addr_t *)ip;
	int error;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (str == NULL) {
		ERR_asprintf_error_data("NULL host provided");
		return (0);
	}

	if ((error = rra_getaddrinfo(str, NULL, &hints, &res)) != 0) {
		BIOerror(BIO_R_BAD_HOSTNAME_LOOKUP);
		ERR_asprintf_error_data("rra_getaddrinfo: host='%s' : %s'", str,
		    gai_strerror(error));
		return (0);
	}
	*iap = (uint32_t)(((struct sockaddr_in *)(res->ai_addr))->sin_addr.s_addr);
	rra_freeaddrinfo(res);
	return (1);
}

int
BIO_get_port(const char *str, unsigned short *port_ptr)
{
	struct rra_addrinfo *res = NULL;
	struct rra_addrinfo hints = { 0 };
	int error;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (str == NULL) {
		BIOerror(BIO_R_NO_PORT_SPECIFIED);
		return (0);
	}

	if ((error = rra_getaddrinfo(NULL, str, &hints, &res)) != 0) {
		ERR_asprintf_error_data("rra_getaddrinfo: service='%s' : %s'", str,
		    gai_strerror(error));
		return (0);
	}
	*port_ptr = ntohs(((struct sockaddr_in *)(res->ai_addr))->sin_port);
	rra_freeaddrinfo(res);
	return (1);
}

int
BIO_sock_error(int sock)
{
	socklen_t len;
	int err;

	len = sizeof(err);
	if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len) != 0)
		return (1);
	return (err);
}

struct hostent *
BIO_gethostbyname(const char *name)
{
	return gethostbyname(name);
}

int
BIO_socket_ioctl(int fd, long type, void *arg)
{
	int ret;

	ret = ioctl(fd, type, arg);
	if (ret < 0)
		SYSerror(errno);
	return (ret);
}

int
BIO_get_accept_socket(char *host, int bind_mode)
{
	struct rra_addrinfo hints = { 0 };
	struct rra_addrinfo *res = NULL;
	char *h, *p, *str = NULL;
	int error, ret = 0, s = -1;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (host == NULL || (str = strdup(host)) == NULL)
		return (-1);
	p = NULL;
	h = str;
	if ((p = strrchr(str, ':')) == NULL) {
		/* A string without a colon is treated as a port. */
		p = str;
		h = NULL;
	} else {
		*p++ = '\0';
		if (*p == '\0') {
			BIOerror(BIO_R_NO_PORT_SPECIFIED);
			goto err;
		}
		if (*h == '\0' || strcmp(h, "*") == 0)
			h = NULL;
	}

	if ((error = rra_getaddrinfo(h, p, &hints, &res)) != 0) {
		ERR_asprintf_error_data("rra_getaddrinfo: '%s:%s': %s'", h, p,
		    gai_strerror(error));
		goto err;
	}
	if (h == NULL) {
		struct sockaddr_in *sin = (struct sockaddr_in *)res->ai_addr;
		sin->sin_addr.s_addr = INADDR_ANY;
	}

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == -1) {
		SYSerror(errno);
		ERR_asprintf_error_data("host='%s'", host);
		BIOerror(BIO_R_UNABLE_TO_CREATE_SOCKET);
		goto err;
	}
	if (bind_mode == BIO_BIND_REUSEADDR) {
		int i = 1;

		ret = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
		bind_mode = BIO_BIND_NORMAL;
	}
	if (bind(s, res->ai_addr, res->ai_addrlen) == -1) {
		SYSerror(errno);
		ERR_asprintf_error_data("host='%s'", host);
		BIOerror(BIO_R_UNABLE_TO_BIND_SOCKET);
		goto err;
	}
	if (listen(s, SOMAXCONN) == -1) {
		SYSerror(errno);
		ERR_asprintf_error_data("host='%s'", host);
		BIOerror(BIO_R_UNABLE_TO_LISTEN_SOCKET);
		goto err;
	}
	ret = 1;

err:
	free(str);
	if (res != NULL)
		rra_freeaddrinfo(res);
	if ((ret == 0) && (s != -1)) {
		close(s);
		s = -1;
	}
	return (s);
}

int
BIO_accept(int sock, char **addr)
{
	char   h[NI_MAXHOST], s[NI_MAXSERV];
	struct sockaddr_in sin;
	socklen_t sin_len = sizeof(sin);
	int ret = -1;

	if (addr == NULL)
		goto end;

	ret = accept(sock, (struct sockaddr *)&sin, &sin_len);
	if (ret == -1) {
		if (BIO_sock_should_retry(ret))
			return -2;
		SYSerror(errno);
		BIOerror(BIO_R_ACCEPT_ERROR);
		goto end;
	}
	/* XXX Crazy API. Can't be helped */
	if (*addr != NULL) {
		free(*addr);
		*addr = NULL;
	}

	if (sin.sin_family != AF_INET)
		goto end;

	if (rra_getnameinfo((struct sockaddr *)&sin, sin_len, h, sizeof(h),
		s, sizeof(s), NI_NUMERICHOST|NI_NUMERICSERV) != 0)
		goto end;

	if ((asprintf(addr, "%s:%s", h, s)) == -1) {
		BIOerror(ERR_R_MALLOC_FAILURE);
		*addr = NULL;
		goto end;
	}
end:
	return (ret);
}

int
BIO_set_tcp_ndelay(int s, int on)
{
	return (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) == 0);
}
