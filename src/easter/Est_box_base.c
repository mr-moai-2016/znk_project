#include <Est_box_base.h>
#include <Est_config.h>
#include <Est_base.h>

#include <Znk_s_base.h>
#include <Znk_htp_util.h>
#include <Znk_str_ex.h>

static const char* st_box_vname_table[] = {
	"favorite",
	"cachebox",
	"dustbox",
	"stockbox",
	"userbox",
};

EstBoxDirType
EstBoxBase_getDirType( const char* path )
{
	const char* dir;
	size_t idx;
	for( idx=0; idx<Znk_NARY(st_box_vname_table); ++idx ){
		dir = st_box_vname_table[ idx ];
		if( ZnkS_eq( path, dir ) ){
			return (EstBoxDirType)( idx + 1 );
		}
		if( ZnkS_isBegin( path, dir ) ){
			if( path[ Znk_strlen(dir) ] == '/' ){
				return (EstBoxDirType)( idx + 1 );
			}
		}
	}
	return EstBoxDir_e_Unknown;
}

const char*
EstBoxBase_getBoxVName( EstBoxDirType box_dir_type )
{
	if( box_dir_type == EstBoxDir_e_Unknown ){
		return NULL;
	}
	return st_box_vname_table[ (int)( box_dir_type-1 ) ];
}
const char*
EstBoxBase_getBoxFsysDir( EstBoxDirType box_dir_type )
{
	switch( box_dir_type ){
	case EstBoxDir_e_Favorite:
		return EstConfig_favorite_dir();
	case EstBoxDir_e_Cachebox:
		return "cachebox";
	case EstBoxDir_e_Dustbox:
		return "dustbox";
	case EstBoxDir_e_Stockbox:
		return EstConfig_stockbox_dir();
	case EstBoxDir_e_Userbox:
		return EstConfig_userbox_dir();
	default:
		break;
	}
	return NULL;
}

EstBoxDirType
EstBoxBase_convertBoxDir_toFSysDir( ZnkStr fsys_path, const char* src_box_dir, size_t* ans_box_path_offset, ZnkStr msg )
{
	EstBoxDirType dir_type = EstBoxBase_getDirType( src_box_dir );
	const char* box_fsys_dir = NULL;
	size_t box_path_offset = 0;

	ZnkStr_set( fsys_path, src_box_dir );
	EstBase_unescape_forMbcFSysPath( fsys_path );
	ZnkHtpURL_sanitizeReqUrpDir( fsys_path, false );

	switch( dir_type ){
	case EstBoxDir_e_Unknown:
		if( msg ){
			ZnkStr_addf( msg, "Invalid path [%s].", src_box_dir );
		}
		return dir_type;
	case EstBoxDir_e_Cachebox:
		/* src_box_dirをそのまま使用. */
		box_path_offset = Znk_strlen_literal( "cachebox/" );
		break;
	case EstBoxDir_e_Dustbox:
		/* src_box_dirをそのまま使用. */
		box_path_offset = Znk_strlen_literal( "dustbox/" );
		break;
	case EstBoxDir_e_Favorite:
		box_fsys_dir = EstConfig_favorite_dir();
		box_path_offset = Znk_strlen_literal( "favorite/" );
		break;
	case EstBoxDir_e_Stockbox:
		box_fsys_dir = EstConfig_stockbox_dir();
		box_path_offset = Znk_strlen_literal( "stockbox/" );
		break;
	case EstBoxDir_e_Userbox:
		box_fsys_dir = EstConfig_userbox_dir();
		box_path_offset = Znk_strlen_literal( "userbox/" );
		break;
	default:
		break;
	}

	if( ans_box_path_offset ){ *ans_box_path_offset = box_path_offset; }

	if( box_fsys_dir ){
		const char* p = ZnkStrEx_strchr( fsys_path, '/' );
		if( p ){
			ZnkStr_replace( fsys_path, 0, box_path_offset-1, box_fsys_dir, Znk_NPOS );
		} else {
			ZnkStr_set( fsys_path, box_fsys_dir );
		}
	}
	return dir_type;
}

