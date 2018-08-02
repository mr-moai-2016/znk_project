/* $Id: inet_ntop.c 4022 2008-03-31 06:11:07Z rra $
 *
 * Replacement for a missing inet_ntop.
 *
 * Provides an implementation of inet_ntop that only supports IPv4 addresses
 * for hosts that are missing it.  If you want IPv6 support, you need to have
 * a real inet_ntop function; this function is only provided so that code can
 * call inet_ntop unconditionally without needing to worry about whether the
 * host supports IPv6.
 *
 * Written by Russ Allbery <rra@stanford.edu>
 * This work is hereby placed in the public domain by its author.
 */

#include <rra_portable/inet_conv.h>
#include <rra_portable/snprintf.h>
#include <rra_portable/socket.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

#define ENABLE_IPV6 1

#define NS_INADDRSZ  4
#define NS_IN6ADDRSZ 16
#define NS_INT16SZ   2

/* This may already be defined by the system headers. */
#ifndef   INET_ADDRSTRLEN
#  define INET_ADDRSTRLEN 16
#endif

/* Systems old enough to not support inet_ntop may not have this either. */
#ifndef   EAFNOSUPPORT
#  define EAFNOSUPPORT EDOM
#endif

/*
 * Format an IPv4 address, more or less like inet_ntoa().
 *
 * Returns `dst' (as a const)
 * Note:
 *  - uses no statics
 *  - takes a unsigned char* not an in_addr as input
 */
static char *inet_ntop4 (const unsigned char *src, char *dst, size_t size)
{
	char tmp[sizeof "255.255.255.255"];
	size_t len;
	
	assert(size >= 16);
	
	tmp[0] = '\0';
	(void)snprintf(tmp, sizeof(tmp), "%d.%d.%d.%d",
	               ((int)((unsigned char)src[0])) & 0xff,
	               ((int)((unsigned char)src[1])) & 0xff,
	               ((int)((unsigned char)src[2])) & 0xff,
	               ((int)((unsigned char)src[3])) & 0xff);
	
	len = strlen(tmp);
	if(len == 0 || len >= size) {
		errno = ENOSPC;
		return (NULL);
	}
	strcpy(dst, tmp);
	return dst;
}

#ifdef ENABLE_IPV6
/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
static char *inet_ntop6 (const unsigned char *src, char *dst, size_t size)
{
	/*
	 * Note that int32_t and int16_t need only be "at least" large enough
	 * to contain a value of the specified size.  On some systems, like
	 * Crays, there is no such thing as an integer variable with 16 bits.
	 * Keep this in mind if you think this function should have been coded
	 * to use pointer overlays.  All the world's not a VAX.
	 */
	char tmp[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
	char *tp;
	struct {
		long base;
		long len;
	} best, cur;
	unsigned long words[NS_IN6ADDRSZ / NS_INT16SZ];
	int i;
	
	/* Preprocess:
	 *  Copy the input (bytewise) array into a wordwise array.
	 *  Find the longest run of 0x00's in src[] for :: shorthanding.
	 */
	memset(words, '\0', sizeof(words));
	for(i = 0; i < NS_IN6ADDRSZ; i++)
		words[i/2] |= (src[i] << ((1 - (i % 2)) << 3));
	
	best.base = -1;
	cur.base  = -1;
	best.len = 0;
	cur.len = 0;
	
	for(i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
		if(words[i] == 0) {
			if(cur.base == -1)
				cur.base = i, cur.len = 1;
			else
				cur.len++;
		}
		else if(cur.base != -1) {
			if(best.base == -1 || cur.len > best.len)
				best = cur;
			cur.base = -1;
		}
	}
	if((cur.base != -1) && (best.base == -1 || cur.len > best.len))
		best = cur;
	if(best.base != -1 && best.len < 2)
		best.base = -1;
	/* Format the result. */
	tp = tmp;
	for(i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
		/* Are we inside the best run of 0x00's? */
		if(best.base != -1 && i >= best.base && i < (best.base + best.len)) {
			if(i == best.base)
				*tp++ = ':';
			continue;
		}
		
		/* Are we following an initial run of 0x00s or any real hex?
		 */
		if(i != 0)
			*tp++ = ':';
		
		/* Is this address an encapsulated IPv4?
		 */
		if(i == 6 && best.base == 0 && (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
			if(!inet_ntop4(src + 12, tp, sizeof(tmp) - (tp - tmp))) {
				errno = ENOSPC;
				return (NULL);
			}
			tp += strlen(tp);
			break;
		}
		tp += snprintf(tp, 5, "%lx", words[i]);
	}
	
	/* Was it a trailing run of 0x00's?
	 */
	if(best.base != -1 && (best.base + best.len) == (NS_IN6ADDRSZ / NS_INT16SZ))
		*tp++ = ':';
	*tp++ = '\0';
	
	/* Check for overflow, copy, and we're done.
	 */
	if((size_t)(tp - tmp) > size) {
		errno = ENOSPC;
		return (NULL);
	}
	strcpy(dst, tmp);
	return dst;
}
#endif  /* ENABLE_IPV6 */

/*
 * Convert a network format address to presentation format.
 *
 * Returns pointer to presentation format address (`buf').
 * Returns NULL on error and errno set with the specific
 * error, EAFNOSUPPORT or ENOSPC.
 *
 * On Windows we store the error in the thread errno, not
 * in the winsock error code. This is to avoid losing the
 * actual last winsock error. So when this function returns
 * NULL, check errno not SOCKERRNO.
 */
//char* Curl_inet_ntop(int af, const void *src, char *buf, size_t size)
char*
rra_inet_ntop(int af, const void *src, char *buf, size_t size)
{
	switch(af) {
	case AF_INET:
		return inet_ntop4((const unsigned char *)src, buf, size);
#ifdef ENABLE_IPV6
	case AF_INET6:
		return inet_ntop6((const unsigned char *)src, buf, size);
#endif
	default:
		break;
		//errno = EAFNOSUPPORT;
	}
	return NULL;
}


#if 0
const char*
rra_inet_ntop( int af, const void* src, char* dst, socklen_t cnt )
{
	const unsigned char *p;
	if (af != AF_INET) {
		socket_set_errno(EAFNOSUPPORT);
		return NULL;
	}
	if (cnt < INET_ADDRSTRLEN) {
		errno = ENOSPC;
		return NULL;
	}
	p = src;
	snprintf( dst, cnt, "%u.%u.%u.%u",
			(unsigned int) (p[0] & 0xff), (unsigned int) (p[1] & 0xff),
			(unsigned int) (p[2] & 0xff), (unsigned int) (p[3] & 0xff));
	return dst;
}
#endif
