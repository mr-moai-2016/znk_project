#ifndef INCLUDE_GUARD__Znk_dlhlp_h__
#define INCLUDE_GUARD__Znk_dlhlp_h__

#include <Znk_dlink.h>
#include <Znk_stdc.h>

Znk_EXTERN_C_BEGIN

#if defined(Znk_TARGET_WINDOWS)
#  define Znk_DLHLP_DLIB_NAME( name ) #name ".dll"
#else
#  define Znk_DLHLP_DLIB_NAME( name ) #name ".so"
#endif


#define Znk_DLHLP_GET_FUNC( func_name ) do{ \
	st_module.func_name##_ = (FuncType_##func_name) ZnkDLink_getFunc( st_module.handler_, #func_name ); \
} while( 0 )

#define Znk_DLHLP_THE_MODULE_BEGIN( module_struct_name ) \
	static module_struct_name st_module = { NULL, }; \
	if( st_module.handler_ == NULL && auto_load ){ \
		st_module.handler_ = ZnkDLink_open( dlib_name ); \
		if( st_module.handler_ ){

#define Znk_DLHLP_THE_MODULE_END \
			return &st_module; \
		} \
		return NULL; \
	} \
	return &st_module;

#define Znk_DLHLP_DECLARE_UNLOAD_MODULE( unload_func, module_struct_name ) \
	void unload_func( void ){ \
		module_struct_name* module = the##module_struct_name( false, "" ); \
		if( module == NULL ){ return; } \
		Znk_DELETE_PTR( module->handler_, ZnkDLink_close, NULL ); \
		Znk_memset( module, 0, sizeof( module_struct_name ) ); \
	}


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
