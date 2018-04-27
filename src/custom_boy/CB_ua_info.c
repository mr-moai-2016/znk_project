#include <CB_ua_info.h>

#include <Znk_myf.h>
#include <Znk_missing_libc.h>

bool
CBUAInfo_load( CBUAInfo ua_info, const char* filename )
{
	ZnkMyf myf = ZnkMyf_create();
	bool result = false;

	result = ZnkMyf_load( myf, filename );
	if( result ){
		ZnkVarp varp = NULL;
		ZnkVarpAry vars;
		vars = ZnkMyf_find_vars( myf, "UA" );

		varp = ZnkVarpAry_find_byName_literal( vars, "UA_browser", false );
		if( varp ){
			ua_info->browser_ = CBUABrowser_getType_fromCStr( ZnkVar_cstr(varp) );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "UA_major_ver", false );
		if( varp ){
			ZnkS_getSzU( &ua_info->major_ver_, ZnkVar_cstr(varp) );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "UA_minor_ver", false );
		if( varp ){
			ZnkS_getSzU( &ua_info->minor_ver_, ZnkVar_cstr(varp) );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "UA_os", false );
		if( varp ){
			ua_info->os_ = CBUAOS_getType_fromCStr( ZnkVar_cstr(varp) );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "UA_machine", false );
		if( varp ){
			ua_info->machine_ = CBUAMachine_getType_fromCStr( ZnkVar_cstr(varp) );
		}

	}

	ZnkMyf_destroy( myf );

	return result;
}
bool
CBUAInfo_load2( CBUAInfo ua_info, const char* filename, const char* sec_name )
{
	ZnkMyf myf = ZnkMyf_create();
	bool result = false;

	result = ZnkMyf_load( myf, filename );
	if( result ){
		ZnkVarp varp = NULL;
		ZnkVarpAry vars;
		vars = ZnkMyf_find_vars( myf, sec_name );

		varp = ZnkVarpAry_find_byName_literal( vars, "UA_browser", false );
		if( varp ){
			ua_info->browser_ = CBUABrowser_getType_fromCStr( ZnkVar_cstr(varp) );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "UA_major_ver", false );
		if( varp ){
			ZnkS_getSzU( &ua_info->major_ver_, ZnkVar_cstr(varp) );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "UA_minor_ver", false );
		if( varp ){
			ZnkS_getSzU( &ua_info->minor_ver_, ZnkVar_cstr(varp) );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "UA_os", false );
		if( varp ){
			ua_info->os_ = CBUAOS_getType_fromCStr( ZnkVar_cstr(varp) );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "UA_machine", false );
		if( varp ){
			ua_info->machine_ = CBUAMachine_getType_fromCStr( ZnkVar_cstr(varp) );
		}

	}

	ZnkMyf_destroy( myf );

	return result;
}
bool
CBUAInfo_save( const CBUAInfo ua_info, const char* filename )
{
	ZnkMyf myf = ZnkMyf_create();
	bool result = false;

	result = ZnkMyf_load( myf, filename );
	if( result ){
		ZnkVarp varp = NULL;
		ZnkVarpAry vars;
		char buf[ 256 ];
		vars = ZnkMyf_find_vars( myf, "UA" );

		varp = ZnkVarpAry_find_byName_literal( vars, "UA_browser", false );
		if( varp ){
			ZnkVar_set_val_Str( varp, CBUABrowser_getCStr( ua_info->browser_ ), Znk_NPOS );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "UA_minor_ver", false );
		if( varp ){
			Znk_snprintf( buf, sizeof(buf), "%zu", ua_info->minor_ver_ );
			ZnkVar_set_val_Str( varp, buf, Znk_NPOS );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "UA_major_ver", false );
		if( varp ){
			Znk_snprintf( buf, sizeof(buf), "%zu", ua_info->major_ver_ );
			ZnkVar_set_val_Str( varp, buf, Znk_NPOS );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "UA_os", false );
		if( varp ){
			ZnkVar_set_val_Str( varp, CBUAOS_getCStr( ua_info->os_ ), Znk_NPOS );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "UA_machine", false );
		if( varp ){
			ZnkVar_set_val_Str( varp, CBUAMachine_getCStr( ua_info->machine_ ), Znk_NPOS );
		}

		result = ZnkMyf_save( myf, "ua_state.myf" );
	}

	ZnkMyf_destroy( myf );

	return result;
}

bool
CBUAInfo_save2( const CBUAInfo ua_info, const char* filename, const char* sec_name )
{
	ZnkMyf myf = ZnkMyf_create();
	bool result = false;

	result = ZnkMyf_load( myf, filename );
	if( result ){
		ZnkVarp varp = NULL;
		ZnkVarpAry vars;
		char buf[ 256 ];
		vars = ZnkMyf_find_vars( myf, sec_name );

		varp = ZnkVarpAry_find_byName_literal( vars, "UA_browser", false );
		if( varp ){
			ZnkVar_set_val_Str( varp, CBUABrowser_getCStr( ua_info->browser_ ), Znk_NPOS );
		} else {
			/* V‹K’Ç‰Á */
			varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
			ZnkStr_assign( varp->name_, 0, "UA_browser", Znk_NPOS );
			ZnkVar_set_val_Str( varp, CBUABrowser_getCStr( ua_info->browser_ ), Znk_NPOS );
			ZnkVarpAry_push_bk( vars, varp );
		}

		varp = ZnkVarpAry_find_byName_literal( vars, "UA_minor_ver", false );
		if( varp ){
			Znk_snprintf( buf, sizeof(buf), "%zu", ua_info->minor_ver_ );
			ZnkVar_set_val_Str( varp, buf, Znk_NPOS );
		} else {
			/* V‹K’Ç‰Á */
			varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
			ZnkStr_assign( varp->name_, 0, "UA_minor_ver", Znk_NPOS );
			Znk_snprintf( buf, sizeof(buf), "%zu", ua_info->minor_ver_ );
			ZnkVar_set_val_Str( varp, buf, Znk_NPOS );
			ZnkVarpAry_push_bk( vars, varp );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "UA_major_ver", false );
		if( varp ){
			Znk_snprintf( buf, sizeof(buf), "%zu", ua_info->major_ver_ );
			ZnkVar_set_val_Str( varp, buf, Znk_NPOS );
		} else {
			/* V‹K’Ç‰Á */
			varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
			ZnkStr_assign( varp->name_, 0, "UA_major_ver", Znk_NPOS );
			Znk_snprintf( buf, sizeof(buf), "%zu", ua_info->major_ver_ );
			ZnkVar_set_val_Str( varp, buf, Znk_NPOS );
			ZnkVarpAry_push_bk( vars, varp );
		}

		varp = ZnkVarpAry_find_byName_literal( vars, "UA_os", false );
		if( varp ){
			ZnkVar_set_val_Str( varp, CBUAOS_getCStr( ua_info->os_ ), Znk_NPOS );
		} else {
			/* V‹K’Ç‰Á */
			varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
			ZnkStr_assign( varp->name_, 0, "UA_os", Znk_NPOS );
			ZnkVar_set_val_Str( varp, CBUAOS_getCStr( ua_info->os_ ), Znk_NPOS );
			ZnkVarpAry_push_bk( vars, varp );
		}

		varp = ZnkVarpAry_find_byName_literal( vars, "UA_machine", false );
		if( varp ){
			ZnkVar_set_val_Str( varp, CBUAMachine_getCStr( ua_info->machine_ ), Znk_NPOS );
		} else {
			/* V‹K’Ç‰Á */
			varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
			ZnkStr_assign( varp->name_, 0, "UA_machine", Znk_NPOS );
			ZnkVar_set_val_Str( varp, CBUAMachine_getCStr( ua_info->machine_ ), Znk_NPOS );
			ZnkVarpAry_push_bk( vars, varp );
		}

		result = ZnkMyf_save( myf, filename );
	}

	ZnkMyf_destroy( myf );

	return result;
}

bool
CBUAInfo_init_byCBVars( CBUAInfo ua_info, ZnkVarpAry cb_vars )
{
	bool result = false;
	ZnkVarp varp = NULL;

	varp = ZnkVarpAry_find_byName_literal( cb_vars, "UA_browser", false );
	if( varp ){
		ua_info->browser_ = CBUABrowser_getType_fromCStr( ZnkVar_cstr(varp) );
		result = true;
	}
	varp = ZnkVarpAry_find_byName_literal( cb_vars, "UA_major_ver", false );
	if( varp ){
		ZnkS_getSzU( &ua_info->major_ver_, ZnkVar_cstr(varp) );
		result = true;
	}
	varp = ZnkVarpAry_find_byName_literal( cb_vars, "UA_minor_ver", false );
	if( varp ){
		ZnkS_getSzU( &ua_info->minor_ver_, ZnkVar_cstr(varp) );
		result = true;
	}
	varp = ZnkVarpAry_find_byName_literal( cb_vars, "UA_os", false );
	if( varp ){
		ua_info->os_ = CBUAOS_getType_fromCStr( ZnkVar_cstr(varp) );
		result = true;
	}
	varp = ZnkVarpAry_find_byName_literal( cb_vars, "UA_machine", false );
	if( varp ){
		ua_info->machine_ = CBUAMachine_getType_fromCStr( ZnkVar_cstr(varp) );
		result = true;
	}
	return result;
}
