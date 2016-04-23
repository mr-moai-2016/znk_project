#include "Znk_dlink.h"

#ifdef Znk_TARGET_WINDOWS
#  include <windows.h>
#else
#  include <dlfcn.h>
#endif


ZnkDLinkHandler
ZnkDLink_open( const char* lib_name )
{
#ifdef Znk_TARGET_WINDOWS
	return (ZnkDLinkHandler)LoadLibrary(lib_name);
#else
	return dlopen( lib_name, RTLD_LAZY );
#endif
}

void*
ZnkDLink_getFunc( ZnkDLinkHandler handle, const char* func_name )
{
#ifdef Znk_TARGET_WINDOWS
	return (void*)GetProcAddress( (HINSTANCE)handle, func_name );
#else
	return dlsym( handle, func_name );
#endif
}

void
ZnkDLink_close( ZnkDLinkHandler handle )
{
#ifdef Znk_TARGET_WINDOWS
	FreeLibrary( (HINSTANCE)handle );
#else
	dlclose( handle );
#endif
}

