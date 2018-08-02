/*
 * Public domain.
 * Win32 explicit_bzero compatibility shim.
 */

#include <windows.h>
#include <string.h>

void
explicit_bzero(void *buf, size_t len)
{
#if 0
	//SecureZeroMemory(buf, len);
#else
	{
		volatile char* p = (volatile char*)buf;
		int i;
		for( i=0; i<len; ++i ){ *p++ = 0; }
	}
#endif
}
