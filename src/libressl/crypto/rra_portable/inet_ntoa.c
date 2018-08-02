/* $Id: inet_ntoa.c 4022 2008-03-31 06:11:07Z rra $
 *
 * Replacement for a missing or broken inet_ntoa.
 *
 * Provides the same functionality as the standard library routine inet_ntoa
 * for those platforms that don't have it or where it doesn't work right (such
 * as on IRIX when using gcc to compile).
 *
 * Written by Russ Allbery <rra@stanford.edu>
 * This work is hereby placed in the public domain by its author.
 *
 * Modified by Zenkaku( https://github.com/mr-moai-2016/znk_project ).
 * Original inet_ntoa is not thread-safe since it uses static storage.
 * (inet_ntop should be used instead when available).
 * This function is modified at the interface for it.
 */

#include <rra_portable/inet_conv.h>
#include <rra_portable/snprintf.h>

const char*
rra_inet_ntoa( const struct in_addr in, char* buf, size_t buf_size )
{
	const unsigned char *p = (const unsigned char *) &in.s_addr;
	snprintf( buf, buf_size, "%u.%u.%u.%u",
			(unsigned int) (p[0] & 0xff), (unsigned int) (p[1] & 0xff),
			(unsigned int) (p[2] & 0xff), (unsigned int) (p[3] & 0xff));
	return buf;
}
