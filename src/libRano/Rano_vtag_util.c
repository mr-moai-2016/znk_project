#include <Rano_vtag_util.h>
#include <Znk_s_base.h>
#include <Znk_dir.h>
#include <Znk_str_path.h>
#include <Znk_str_fio.h>
#include <Znk_missing_libc.h>

bool
RanoVTagUtil_getVTagStr( ZnkStr str, const char* vtag_path )
{
	bool result = false;
	ZnkFile fp = Znk_fopen( vtag_path, "rb" );
	if( fp ){
		if( ZnkStrFIO_fgets( str, 0, 4096, fp ) ){
			ZnkStr_chompNL( str );
			result = true;
		}
		Znk_fclose( fp );
	}
	return result;
}

void
RanoVTagUtil_getNewParentVer( ZnkStr new_parent_ver, const char* patch_basename )
{
	const char* p = patch_basename; /* moai-v2.0-patch-v2.0.2 etc. */
	const char* q = NULL;
	while( *p && *p != '-' ){ ++p; }
	++p;

	/* old parent version : skip */
	while( *p && *p != '-' ){ ++p; }
	++p;

	/* patch : skip */
	while( *p && *p != '-' ){ ++p; }
	++p;

	/* new version */
	q = p;
	while( *p && *p != '.' ){ ++p; }
	++p;
	while( *p && *p != '.' ){ ++p; }

	ZnkStr_assign( new_parent_ver, 0, q, p-q );
}

static const char*
getPlatformID_fromCompilerIntrinsic( void )
{
#if defined( __CYGWIN__ )
#  if Znk_CPU_BIT == 64
	return "cygwin64";
#  else
	return "cygwin32";
#  endif

#elif defined( __ANDROID__ )
	/* ‚Æ‚è‚ ‚¦‚¸ */
	return "android-armeabi";

#elif defined( Znk_TARGET_WINDOWS )
#  if Znk_CPU_BIT == 64
	return "win64";
#  else
	return "win32";
#  endif

#elif defined( Znk_TARGET_UNIX )
#  if Znk_CPU_BIT == 64
	return "linux64";
#  else
	return "linux32";
#  endif

#else
	return NULL;
#endif
}

bool
RanoVTagUtil_getPlatformID( ZnkStr platform_id, const char* app_name, const char* curdir_tail )
{
	ZnkStr curdir = ZnkStr_new( "" );
	size_t tail_pos = 0;
	bool result = false;
	ZnkDir_getCurrentDir( curdir );
	tail_pos = ZnkStrPath_getTailPos( ZnkStr_cstr(curdir) );
	if( ZnkS_eq( ZnkStr_cstr(curdir) + tail_pos, curdir_tail ) ){
		ZnkStr_releng( curdir, tail_pos );
		ZnkStr_cut_back_c( curdir );
		tail_pos = ZnkStrPath_getTailPos( ZnkStr_cstr(curdir) );
		if( ZnkS_isBegin( ZnkStr_cstr(curdir) + tail_pos, app_name ) ){
			/* found */
			const char* p = ZnkStr_cstr(curdir) + tail_pos + Znk_strlen(app_name);
			if( *p == '-' ){ ++p; }
			p = Znk_strchr( p, '-' ); /* skip version part */
			if( p ){
				++p;
				ZnkStr_set( platform_id, p );
				result = true;
			}
		} else {
			const char* id = getPlatformID_fromCompilerIntrinsic();
			if( id ){
				ZnkStr_set( platform_id, id );
				result = true;
			}
		}
	}
	ZnkStr_delete( curdir );
	return result;
}

bool
RanoVTagUtil_getPatchDir( ZnkStr patch_dir, const char* app_name, bool with_platform_id, const char* curdir_tail, const char* tmp_dir )
{
	bool    result = false;
	ZnkStr  patch_basename = ZnkStr_new( "" );
	char    patch_basenam_dat_path[ 256 ] = "";
	ZnkFile fp = NULL;

	Znk_snprintf( patch_basenam_dat_path, sizeof(patch_basenam_dat_path), "%spatch_basename.dat", tmp_dir );
	fp = Znk_fopen( patch_basenam_dat_path, "rb" );
	if( fp ){
		if( ZnkStrFIO_fgets( patch_basename, 0, 4096, fp ) ){
			ZnkStr_chompNL( patch_basename );
			if( with_platform_id ){
				ZnkStr platform_id       = ZnkStr_new( "" );
				RanoVTagUtil_getPlatformID( platform_id, app_name, curdir_tail );
				ZnkStr_addf( patch_dir, "%s%s/%s-%s",
						tmp_dir, ZnkStr_cstr(patch_basename), app_name, ZnkStr_cstr(platform_id) );
				ZnkStr_delete( platform_id );
			} else {
				ZnkStr_addf( patch_dir, "%s%s/%s",
						tmp_dir, ZnkStr_cstr(patch_basename), app_name );
			}
			result = true;
		}
		Znk_fclose( fp );
	}
	ZnkStr_delete( patch_basename );
	return result;
}

