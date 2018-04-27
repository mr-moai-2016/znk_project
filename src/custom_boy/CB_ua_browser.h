#ifndef INCLUDE_GUARD__CB_ua_browser_h__
#define INCLUDE_GUARD__CB_ua_browser_h__

#include <Znk_s_base.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 CBUABrowser_e_firefox
	,CBUABrowser_e_chrome
	,CBUABrowser_e_opera
	,CBUABrowser_e_ie
	,CBUABrowser_e_safari
	,CBUABrowser_e_edge
	,CBUABrowser_e_android_std
	,CBUABrowser_e_Unknown
} CBUABrowser;

Znk_INLINE const char*
CBUABrowser_getCStr( CBUABrowser browser )
{
	const char* str = Znk_TO_STR( CBUABrowser_e_Unknown );
	switch( browser ){
	case Znk_BIND_STR( : str=, CBUABrowser_e_firefox ); break;
	case Znk_BIND_STR( : str=, CBUABrowser_e_chrome );  break;
	case Znk_BIND_STR( : str=, CBUABrowser_e_opera );   break;
	case Znk_BIND_STR( : str=, CBUABrowser_e_ie );      break;
	case Znk_BIND_STR( : str=, CBUABrowser_e_safari );  break;
	case Znk_BIND_STR( : str=, CBUABrowser_e_edge );    break;
	case Znk_BIND_STR( : str=, CBUABrowser_e_android_std ); break;
	default: break;
	}
	return str + Znk_strlen_literal( "CBUABrowser_e_" );
}

Znk_INLINE CBUABrowser
CBUABrowser_getType_fromCStr( const char* browser_cstr )
{
	switch( browser_cstr[0] ){
	case 'f':
		if( ZnkS_eq( browser_cstr, "firefox" ) ){
			return CBUABrowser_e_firefox;
		}
		break;
	case 'c':
		if( ZnkS_eq( browser_cstr, "chrome" ) ){
			return CBUABrowser_e_chrome;
		}
		break;
	case 'o':
		if( ZnkS_eq( browser_cstr, "opera" ) ){
			return CBUABrowser_e_opera;
		}
		break;
	case 'i':
		if( ZnkS_eq( browser_cstr, "ie" ) ){
			return CBUABrowser_e_ie;
		}
		break;
	case 's':
		if( ZnkS_eq( browser_cstr, "safari" ) ){
			return CBUABrowser_e_safari;
		}
		break;
	case 'e':
		if( ZnkS_eq( browser_cstr, "edge" ) ){
			return CBUABrowser_e_edge;
		}
		break;
	case 'a':
		if( ZnkS_eq( browser_cstr, "android_std" ) ){
			return CBUABrowser_e_android_std;
		}
		break;
	default:
		break;
	}
	return CBUABrowser_e_Unknown;
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
