#ifndef INCLUDE_GUARD__Mkf_lib_depend_h__
#define INCLUDE_GUARD__Mkf_lib_depend_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

bool
MkfLibDepend_parseDependencyLib( const char* dependency_lib,
		ZnkStr dep_lib_type,
		ZnkStr lib_name,
		ZnkStr lib_dlver,
		ZnkStr lib_dir,
		bool* is_at_notation );

void
MkfLibDepend_parseSubLib( const char* sublib,
		ZnkStr submkf_dir,
		ZnkStr sublib_name );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
