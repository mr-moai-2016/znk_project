#ifndef INCLUDE_GUARD__Rano_vtag_util_h__
#define INCLUDE_GUARD__Rano_vtag_util_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

bool
RanoVTagUtil_getVTagStr( ZnkStr str, const char* vtag_path );

void
RanoVTagUtil_getNewParentVer( ZnkStr new_parent_ver, const char* patch_basename );

bool
RanoVTagUtil_getPlatformID( ZnkStr platform_id, const char* app_name, const char* curdir_tail );

bool
RanoVTagUtil_getPatchDir( ZnkStr patch_dir, const char* app_name, bool with_platform_id, const char* curdir_tail, const char* tmp_dir );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
