/*  $Id: getnameinfo.h 2915 2007-01-13 06:29:46Z rra $
**
**  Replacement implementation of getnameinfo.
**
**  Written by Russ Allbery <rra@stanford.edu>
**  This work is hereby placed in the public domain by its author.
**
**  This is an implementation of the getnameinfo function for systems that
**  lack it, so that code can use getnameinfo always.  It provides IPv4
**  support only; for IPv6 support, a native getnameinfo implemenation is
**  required.
**
**  This file should generally be included by way of rra_portable/socket.h rather
**  than directly.
*/

#ifndef RRA_PORTABLE_GETNAMEINFO_H
#define RRA_PORTABLE_GETNAMEINFO_H 1

#include <rra_portable/macros.h>

#include <sys/socket.h>

/* Constants for flags from RFC 3493, combined with binary or. */
#undef  NI_NOFQDN
#define NI_NOFQDN       0x0001

#undef  NI_NUMERICHOST
#define NI_NUMERICHOST  0x0002

#undef  NI_NAMEREQD
#define NI_NAMEREQD     0x0004

#undef  NI_NUMERICSERV
#define NI_NUMERICSERV  0x0008

#undef  NI_DGRAM
#define NI_DGRAM        0x0010

BEGIN_DECLS

int rra_getnameinfo(const struct sockaddr *sa, socklen_t salen,
                char *node, socklen_t nodelen,
                char *service, socklen_t servicelen, int flags);

END_DECLS

#endif /* !RRA_PORTABLE_GETNAMEINFO_H */
