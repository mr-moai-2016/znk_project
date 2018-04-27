#include "Rano_conf_util.h"
#include <Znk_envvar.h>

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

