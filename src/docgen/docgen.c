#include <Doc_html.h>

#include <Rano_dir_util.h>

#include <Znk_envvar.h>
#include <Znk_s_base.h>
#include <Znk_dir.h>

static bool
is_processFile_func( const char* file_path, void* arg )
{
	const char* ext = ZnkS_get_extension( file_path, '.' );
	if( ZnkS_eqCase( ext, "jpg" ) ){
		return true;
	}
	if( ZnkS_eqCase( ext, "png" ) ){
		return true;
	}
	if( ZnkS_eqCase( ext, "gif" ) ){
		return true;
	}
	return false;
}
static bool
filterFile_func( const char* in_file_path, const char* out_file_path, void* arg )
{
	return ZnkDir_copyFile_byForce( in_file_path, out_file_path, NULL );
}

int my_doc( void )
{
	char*  MOAI_PROFILE_DIR = ZnkEnvVar_get( "MOAI_PROFILE_DIR" );
	char*  EST_MYDOC_DIR    = ZnkEnvVar_get( "EST_MYDOC_DIR" );
	ZnkStr moai_profile_dir = ZnkStr_new( "" );
	ZnkStr est_mydoc_dir    = ZnkStr_new( "" );

	if( ZnkS_empty( MOAI_PROFILE_DIR ) ){
		ZnkStr_set( moai_profile_dir, "../moai_profile" );
	} else {
		ZnkStr_set( moai_profile_dir, MOAI_PROFILE_DIR );
	}

	if( ZnkS_empty( EST_MYDOC_DIR ) ){
		ZnkStr_setf( est_mydoc_dir, "%s/mydoc", ZnkStr_cstr(moai_profile_dir) );
	} else {
		ZnkStr_set( est_mydoc_dir, EST_MYDOC_DIR );
	}

	{
		ZnkStr imgs_dir = ZnkStr_newf( "%s/imgs", ZnkStr_cstr(est_mydoc_dir) );
		const char* src_dir = "../../zdoc/mydoc_in/imgs";

		ZnkDir_mkdirPath( ZnkStr_cstr(imgs_dir),  ZnkStr_leng(imgs_dir), '/', NULL );
		RanoDirUtil_filterDir( src_dir, ZnkStr_cstr(imgs_dir),
				"my_doc", NULL,
				is_processFile_func,  NULL,
				filterFile_func,      NULL,
				NULL, NULL );

		ZnkStr_delete( imgs_dir );

	}
	{
		const char* src_dir = "../../zdoc/mydoc_in";
		DocHtml_make( src_dir, ZnkStr_cstr(est_mydoc_dir), NULL );
	}

	ZnkEnvVar_free( MOAI_PROFILE_DIR );
	ZnkEnvVar_free( EST_MYDOC_DIR );
	ZnkStr_delete(  moai_profile_dir );
	ZnkStr_delete(  est_mydoc_dir );
	return 0;
}

