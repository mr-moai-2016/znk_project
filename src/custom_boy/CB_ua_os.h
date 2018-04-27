#ifndef INCLUDE_GUARD__CB_ua_os_h__
#define INCLUDE_GUARD__CB_ua_os_h__

#include <Znk_s_base.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 CBUAOS_e_windows
	,CBUAOS_e_linux
	,CBUAOS_e_mac
	,CBUAOS_e_android
	,CBUAOS_e_iphone
	,CBUAOS_e_Unknown
} CBUAOS;

Znk_INLINE const char*
CBUAOS_getCStr( CBUAOS uaos )
{
	const char* str = Znk_TO_STR( CBUAOS_e_Unknown );
	switch( uaos ){
	case Znk_BIND_STR( : str=, CBUAOS_e_windows ); break;
	case Znk_BIND_STR( : str=, CBUAOS_e_linux );   break;
	case Znk_BIND_STR( : str=, CBUAOS_e_mac );     break;
	case Znk_BIND_STR( : str=, CBUAOS_e_android ); break;
	case Znk_BIND_STR( : str=, CBUAOS_e_iphone );  break;
	default: break;
	}
	return str + Znk_strlen_literal( "CBUAOS_e_" );
}

Znk_INLINE CBUAOS
CBUAOS_getType_fromCStr( const char* os_cstr )
{
	switch( os_cstr[0] ){
	case 'w':
		if( ZnkS_eq( os_cstr, "windows" ) ){
			return CBUAOS_e_windows;
		}
		break;
	case 'l':
		if( ZnkS_eq( os_cstr, "linux" ) ){
			return CBUAOS_e_linux;
		}
		break;
	case 'm':
		if( ZnkS_eq( os_cstr, "mac" ) ){
			return CBUAOS_e_mac;
		}
		break;
	case 'a':
		if( ZnkS_eq( os_cstr, "android" ) ){
			return CBUAOS_e_android;
		}
		break;
	case 'i':
		if( ZnkS_eq( os_cstr, "iphone" ) ){
			return CBUAOS_e_iphone;
		}
		break;
	default:
		break;
	}
	return CBUAOS_e_Unknown;
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
