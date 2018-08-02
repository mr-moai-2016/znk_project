/* $Id: getaddrinfo.h 4022 2008-03-31 06:11:07Z rra $
 *
 * Replacement implementation of getaddrinfo.
 *
 * This is an implementation of the getaddrinfo family of functions for
 * systems that lack it, so that code can use getaddrinfo always.  It provides
 * IPv4 support only; for IPv6 support, a native getaddrinfo implemenation is
 * required.
 *
 * This file should generally be included by way of rra_portable/socket.h rather
 * than directly.
 *
 * Written by Russ Allbery <rra@stanford.edu>
 * This work is hereby placed in the public domain by its author.
 */

#ifndef RRA_PORTABLE_GETADDRINFO_H
#define RRA_PORTABLE_GETADDRINFO_H 1

#include <rra_portable/macros.h>

#include <sys/socket.h>

/* The struct returned by getaddrinfo, from RFC 3493. */
struct rra_addrinfo {
    int ai_flags;               /* AI_PASSIVE, AI_CANONNAME, .. */
    int ai_family;              /* AF_xxx */
    int ai_socktype;            /* SOCK_xxx */
    int ai_protocol;            /* 0 or IPPROTO_xxx for IPv4 and IPv6 */
    socklen_t ai_addrlen;       /* Length of ai_addr */
    char *ai_canonname;         /* Canonical name for nodename */
    struct sockaddr *ai_addr;   /* Binary address */
    struct rra_addrinfo *ai_next;   /* Next structure in linked list */
};

/* Constants for ai_flags from RFC 3493, combined with binary or. */
#undef  AI_PASSIVE
#define AI_PASSIVE      0x0001

#undef  AI_CANONNAME
#define AI_CANONNAME    0x0002

#undef  AI_NUMERICHOST
#define AI_NUMERICHOST  0x0004

#undef  AI_NUMERICSERV
#define AI_NUMERICSERV  0x0008

#undef  AI_V4MAPPED
#define AI_V4MAPPED     0x0010

#undef  AI_ALL
#define AI_ALL          0x0020

#undef  AI_ADDRCONFIG
#define AI_ADDRCONFIG   0x0040

/* Error return codes from RFC 3493. */
#undef  EAI_AGAIN
#define EAI_AGAIN       1       /* Temporary name resolution failure */

#undef  EAI_BADFLAGS
#define EAI_BADFLAGS    2       /* Invalid value in ai_flags parameter */

#undef  EAI_FAIL
#define EAI_FAIL        3       /* Permanent name resolution failure */

#undef  EAI_FAMILY
#define EAI_FAMILY      4       /* Address family not recognized */

#undef  EAI_MEMORY
#define EAI_MEMORY      5       /* Memory allocation failure */

#undef  EAI_NONAME
#define EAI_NONAME      6       /* nodename or servname unknown */

#undef  EAI_SERVICE
#define EAI_SERVICE     7       /* Service not recognized for socket type */

#undef  EAI_SOCKTYPE
#define EAI_SOCKTYPE    8       /* Socket type not recognized */

#undef  EAI_SYSTEM
#define EAI_SYSTEM      9       /* System error occurred, see errno */

#undef  EAI_OVERFLOW
#define EAI_OVERFLOW    10      /* An argument buffer overflowed */

BEGIN_DECLS

/* Function prototypes. */
int rra_getaddrinfo(const char *nodename, const char *servname,
                const struct rra_addrinfo *hints, struct rra_addrinfo **res);
void rra_freeaddrinfo(struct rra_addrinfo *ai);

END_DECLS

#endif /* !RRA_PORTABLE_GETADDRINFO_H */
