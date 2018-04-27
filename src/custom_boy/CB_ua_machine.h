#ifndef INCLUDE_GUARD__CB_ua_machine_h__
#define INCLUDE_GUARD__CB_ua_machine_h__

#include <Znk_s_base.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 CBUAMachine_e_x86
	,CBUAMachine_e_x64
	,CBUAMachine_e_WOW64
	,CBUAMachine_e_MacIntel
	,CBUAMachine_e_MacPPC
	,CBUAMachine_e_android
	,CBUAMachine_e_iphone
	,CBUAMachine_e_Unknown
} CBUAMachine;

Znk_INLINE const char*
CBUAMachine_getCStr( CBUAMachine uamach )
{
	const char* str = Znk_TO_STR( CBUAMachine_e_Unknown );
	switch( uamach ){
	case Znk_BIND_STR( : str=, CBUAMachine_e_x86 );      break;
	case Znk_BIND_STR( : str=, CBUAMachine_e_x64 );      break;
	case Znk_BIND_STR( : str=, CBUAMachine_e_WOW64 );    break;
	case Znk_BIND_STR( : str=, CBUAMachine_e_MacIntel ); break;
	case Znk_BIND_STR( : str=, CBUAMachine_e_MacPPC );   break;
	case Znk_BIND_STR( : str=, CBUAMachine_e_android );  break;
	case Znk_BIND_STR( : str=, CBUAMachine_e_iphone );   break;
	default: break;
	}
	return str + Znk_strlen_literal( "CBUAMachine_e_" );
}

Znk_INLINE CBUAMachine
CBUAMachine_getType_fromCStr( const char* machine_cstr )
{
	switch( machine_cstr[0] ){
	case 'x':
		if( ZnkS_eq( machine_cstr, "x86" ) ){
			return CBUAMachine_e_x86;
		}
		if( ZnkS_eq( machine_cstr, "x64" ) ){
			return CBUAMachine_e_x64;
		}
		break;
	case 'W':
		if( ZnkS_eq( machine_cstr, "WOW64" ) ){
			return CBUAMachine_e_WOW64;
		}
		break;
	case 'M':
		if( ZnkS_eq( machine_cstr, "MacIntel" ) ){
			return CBUAMachine_e_MacIntel;
		}
		if( ZnkS_eq( machine_cstr, "MacPPC" ) ){
			return CBUAMachine_e_MacPPC;
		}
		break;
	case 'a':
		if( ZnkS_eq( machine_cstr, "android" ) ){
			return CBUAMachine_e_android;
		}
		break;
	case 'i':
		if( ZnkS_eq( machine_cstr, "iphone" ) ){
			return CBUAMachine_e_iphone;
		}
		break;
	default:
		break;
	}
	return CBUAMachine_e_Unknown;
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
