#include "Rano_conf_util.h"
#include "Rano_htp_boy.h"

#include <Znk_envvar.h>
#include <Znk_dir.h>
#include <Znk_str_path.h>

#include <ctype.h>

#if defined(Znk_TARGET_WINDOWS)
static bool st_is_dos_path = true;
#else
static bool st_is_dos_path = false;
#endif

bool
RanoConfUtil_initByStrAry_withEnvVar( ZnkStr ans, ZnkStrAry sda, size_t default_idx, size_t envvar_idx,
		size_t dst_begin, size_t dst_size )
{
	bool use_envvar = false;
	const char* val = ZnkStrAry_at_cstr( sda, default_idx );
	const char* envvar_name = NULL;
	char*       envvar_val  = NULL;
	if( ZnkStrAry_size( sda ) >= envvar_idx+1 ){
		envvar_name = ZnkStrAry_at_cstr( sda, envvar_idx );
		envvar_val  = ZnkEnvVar_get( envvar_name );
		if( envvar_val ){
			/* defined in OS */
			val = envvar_val;
			use_envvar = true;
		}
	}
	if( dst_begin == 0 && dst_size == Znk_NPOS ){
		ZnkStr_set( ans, val );
	} else {
		ZnkStr_replace( ans, dst_begin, dst_size, val, Znk_NPOS );
	}
	ZnkEnvVar_free( envvar_val );
	return use_envvar;
}

void
RanoConfUtil_setupConfigDir( ZnkStr ans, const char* given_dir, const char* base_dir, const char* when_empty_dir )
{
	bool is_relative = true;
	if( ZnkS_empty(given_dir) ){
		given_dir = when_empty_dir;
	} else {
		if( given_dir[ 0 ] == '/' ){
			/* UNIX fullpath */
			is_relative = false;
		} else if( given_dir[ 1 ] == ':' && isalpha( given_dir[ 0 ] ) ){
			/* Windows fullpath */
			is_relative = false;
		}
	}
	if( is_relative ){
		ZnkStr_setf( ans, "%s/%s", base_dir, given_dir );
	} else {
		ZnkStr_setf( ans, "%s", given_dir );
	}
}

static ZnkMyf st_rano_app_myf = NULL;

ZnkMyf
RanoConfUtil_rano_app_myf( void )
{
	if( st_rano_app_myf == NULL ){
		st_rano_app_myf = ZnkMyf_create();
		if( !ZnkMyf_load( st_rano_app_myf, "rano_app.myf" ) ){
			ZnkMyf_destroy( st_rano_app_myf );
			st_rano_app_myf = NULL;
		}
	}
	return st_rano_app_myf;
}
bool
RanoConfUtil_rano_app_initiate( const char* base_dir, bool is_binary_mode, ZnkStr ermsg )
{
	bool result = true;
	ZnkMyf rano_app_myf = RanoConfUtil_rano_app_myf();

	/***
	 * for Windows : change stdin to text or binary-mode.
	 */
	Znk_Internal_setMode( 0, is_binary_mode );
	/***
	 * for Windows : change stdout to text or binary-mode.
	 */
	Znk_Internal_setMode( 1, is_binary_mode );

	if( rano_app_myf ){
		ZnkVarpAry vars = ZnkMyf_find_vars( rano_app_myf, "config" );
		if( vars ){
			ZnkStr dir_str = ZnkStr_new( "" );
			ZnkVarp var;

			/***
			 * logの出力先、Httpリソースのキャッシュの一時保存など
			 */
			var = ZnkVarpAry_find_byName_literal( vars, "tmp_dir", false );
			if( var ){
				RanoConfUtil_setupConfigDir( dir_str, ZnkVar_cstr(var), base_dir, "tmp" );
				RanoHtpBoy_setTmpDirCommon( ZnkStr_cstr(dir_str) );
				if( !ZnkDir_mkdirPath( ZnkStr_cstr(dir_str), Znk_NPOS, '/', ermsg ) ){
					result = false;
				}
			}
			ZnkStr_delete( dir_str );
		}
	} else {
		if( ermsg ){
			ZnkStr_addf( ermsg, "rano_app.myf cannot load." );
		}
		result = false;
	}
	return result;
}
void
RanoConfUtil_rano_app_finalize( void )
{
	ZnkMyf_destroy( st_rano_app_myf );
	st_rano_app_myf = NULL;
}


static ZnkStr st_moai_dir = NULL;

bool
RanoConfUtil_moai_dir_initiate( ZnkStr ermsg )
{
	if( st_moai_dir == NULL ){
		const char dsp = st_is_dos_path ? '\\' : '/';
		static const size_t depth = 5;
		st_moai_dir = ZnkStr_new( "../" );
		if( !ZnkStrPath_searchParentDir( st_moai_dir, depth, "target.myf", ZnkDirType_e_File, dsp ) ){
			if( ermsg ){
				ZnkStr_add( ermsg, "Searching moai_dir is failure. target.myf does not found." );
			}
			return false;
		}
	}
	return true;
}
void
RanoConfUtil_moai_dir_finalize( void )
{
	ZnkStr_delete( st_moai_dir );
}
const char*
RanoConfUtil_moai_dir( ZnkStr ermsg )
{
	if( RanoConfUtil_moai_dir_initiate( ermsg ) ){
		return ZnkStr_cstr( st_moai_dir );
	}
	return NULL;
}
