#ifndef INCLUDE_GUARD__Est_box_base_h__
#define INCLUDE_GUARD__Est_box_base_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 EstBoxDir_e_Unknown = 0
	,EstBoxDir_e_Favorite
	,EstBoxDir_e_Cachebox
	,EstBoxDir_e_Dustbox
	,EstBoxDir_e_Stockbox
	,EstBoxDir_e_Userbox
} EstBoxDirType;

EstBoxDirType
EstBoxBase_getDirType( const char* path );

const char*
EstBoxBase_getBoxVName( EstBoxDirType box_dir_type );

const char*
EstBoxBase_getBoxFsysDir( EstBoxDirType box_dir_type );

EstBoxDirType
EstBoxBase_convertBoxDir_toFSysDir( ZnkStr fsys_path, const char* src_box_dir, size_t* ans_box_path_offset, ZnkStr msg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
