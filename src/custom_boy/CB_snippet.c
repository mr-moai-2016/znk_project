#include <CB_snippet.h>

/**
 * Written and Analyzed by K.Yakisoba.H (2017.3)
 */

bool
Snippet_Snp_LtIE6( const CBUAInfo info )
{
	return (bool)( info->browser_ == CBUABrowser_e_ie && info->major_ver_ <= 6 );
}
bool
Snippet_Snp_LtIE7( const CBUAInfo info )
{
	return (bool)( info->browser_ == CBUABrowser_e_ie && info->major_ver_ <= 7 );
}
bool
Snippet_Snp_LtIE8( const CBUAInfo info )
{
	return (bool)( info->browser_ == CBUABrowser_e_ie && info->major_ver_ <= 8 );
}
bool
Snippet_Snp_IE9( const CBUAInfo info )
{
	return (bool)( info->browser_ == CBUABrowser_e_ie && info->major_ver_ == 9 );
}
bool
Snippet_Snp_IE10( const CBUAInfo info )
{
	return (bool)( info->browser_ == CBUABrowser_e_ie && info->major_ver_ == 10 );
}
bool
Snippet_Snp_IE11( const CBUAInfo info )
{
	return (bool)( info->browser_ == CBUABrowser_e_ie && info->major_ver_ == 11 );
}
bool
Snippet_Snp_Trident( const CBUAInfo info )
{
	return (bool)( info->browser_ == CBUABrowser_e_ie );
}
bool
Snippet_Snp_Edge( const CBUAInfo info )
{
	return (bool)( info->browser_ == CBUABrowser_e_edge );
}
bool
Snippet_Snp_Gecko( const CBUAInfo info )
{
	return (bool)( info->browser_ == CBUABrowser_e_firefox );
}
bool
Snippet_Snp_MozillaSidebar( const CBUAInfo info )
{
	return (bool)( info->browser_ == CBUABrowser_e_firefox );
}
bool
Snippet_Snp_NavigatorOnline( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
	case CBUABrowser_e_chrome:
	case CBUABrowser_e_ie:
		return true;
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_SessionStorage( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		return (bool)( info->major_ver_ >= 2 );
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 5 );
	case CBUABrowser_e_ie:
		return (bool)( info->major_ver_ >= 8 );
	case CBUABrowser_e_opera:
		if( info->major_ver_ >= 11 ){
			return true;
		}
		if( info->os_ != CBUAOS_e_iphone && info->os_ != CBUAOS_e_android ){
			return (bool)( info->major_ver_ == 10 && info->minor_ver_ >= 50 );
		}
		return false;
	case CBUABrowser_e_safari:
		return (bool)( info->major_ver_ >= 4 );
	case CBUABrowser_e_android_std:
		return (bool)( ( info->major_ver_ >= 3 ) || ( info->major_ver_ >= 2 && info->minor_ver_ >= 1 ) );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_LocalStorage( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		return (bool)( ( info->major_ver_ >= 4 ) || ( info->major_ver_ >= 3 && info->minor_ver_ >= 5 ) );
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 4 );
	case CBUABrowser_e_ie:
		return (bool)( info->major_ver_ >= 8 );
	case CBUABrowser_e_opera:
		if( info->major_ver_ >= 11 ){
			return true;
		}
		if( info->os_ != CBUAOS_e_iphone && info->os_ != CBUAOS_e_android ){
			return (bool)( info->major_ver_ == 10 && info->minor_ver_ >= 50 );
		}
		return false;
	case CBUABrowser_e_safari:
		return (bool)( info->major_ver_ >= 4 );
	case CBUABrowser_e_android_std:
		return (bool)( ( info->major_ver_ >= 3 ) || ( info->major_ver_ >= 2 && info->minor_ver_ >= 1 ) );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_FunctionX5( const CBUAInfo info )
{
	return (bool)( info->browser_ == CBUABrowser_e_firefox && info->major_ver_ == 3 );
}
bool
Snippet_Snp_DocCurrentScript( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		return (bool)( info->major_ver_ >= 4 );
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 29 );
	case CBUABrowser_e_opera:
		return (bool)( info->major_ver_ >= 16 );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_EventSource( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		return (bool)( info->major_ver_ >= 6 );
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 9 );
	case CBUABrowser_e_opera:
		return (bool)( info->major_ver_ >= 11 );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_Crypto( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		return (bool)( info->major_ver_ >= 21 );
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 11 );
	case CBUABrowser_e_opera:
		return (bool)( info->major_ver_ >= 15 );
	case CBUABrowser_e_ie:
		return (bool)( info->major_ver_ >= 11 );
	case CBUABrowser_e_safari:
		return (bool)( info->major_ver_ >= 3 ); /* 3.1+ */
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_PerformanceNow( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		return (bool)( info->major_ver_ >= 15 );
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 20 );
	case CBUABrowser_e_opera:
		return (bool)( info->major_ver_ >= 15 );
	case CBUABrowser_e_ie:
		return (bool)( info->major_ver_ >= 10 );
	case CBUABrowser_e_safari:
		return (bool)( info->major_ver_ >= 8 );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_AudioContext( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		return (bool)( info->major_ver_ >= 25 );
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 10 );
	case CBUABrowser_e_opera:
		return (bool)( info->major_ver_ >= 15 );
	case CBUABrowser_e_safari:
		return (bool)( info->major_ver_ >= 6 );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_IndexedDB( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		if( info->os_ == CBUAOS_e_iphone || info->os_ == CBUAOS_e_android ){
			return (bool)( info->major_ver_ >= 22 );
		} else {
			return (bool)( info->major_ver_ >= 10 );
		}
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 23 );
	case CBUABrowser_e_opera:
		if( info->os_ == CBUAOS_e_iphone || info->os_ == CBUAOS_e_android ){
			return (bool)( info->major_ver_ >= 22 );
		} else {
			return (bool)( info->major_ver_ >= 15 );
		}
	case CBUABrowser_e_ie:
		return (bool)( info->major_ver_ >= 10 );
	case CBUABrowser_e_safari:
		if( info->os_ == CBUAOS_e_iphone ){
			return (bool)( info->major_ver_ >= 8 );
		} else {
			return (bool)( ( info->major_ver_ >= 8 ) || ( info->major_ver_ >= 7 && info->minor_ver_ >= 1 ) );
		}
	case CBUABrowser_e_edge:
		return true;
	case CBUABrowser_e_android_std:
		return (bool)( ( info->major_ver_ >= 5 ) || ( info->major_ver_ >= 4 && info->minor_ver_ >= 4 ) );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_WindowStyles( const CBUAInfo info )
{
	/* ? */
	return false;
}
bool
Snippet_Snp_SendBeacon( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		return (bool)( info->major_ver_ >= 31 );
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 39 );
	case CBUABrowser_e_opera:
		return (bool)( info->major_ver_ >= 26 );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_GetGamepads( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		return (bool)( info->major_ver_ >= 29 );
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 21 );
	case CBUABrowser_e_opera:
		return (bool)( info->major_ver_ >= 15 );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_NavLanguages( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		return (bool)( info->major_ver_ >= 32 );
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 32 );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_NavMediaDevices( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		return (bool)( info->major_ver_ >= 36 );
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 51 );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_WinCaches( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		return (bool)( info->major_ver_ >= 39 );
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 43 );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_CreateImageBitmap( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		return (bool)( info->major_ver_ >= 42 );
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 50 );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_Onstorage( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		return (bool)( info->major_ver_ >= 45 );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_NavGetBattery( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 39 );
	case CBUABrowser_e_opera:
		return (bool)( info->major_ver_ >= 25 );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_Presto( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_ie:
		/* ieでtrueとなる場合もある(過去にoperaをインストールしたことがあるような場合か?) */
		return false;
	case CBUABrowser_e_opera:
		/* 12.16- */
		return (bool)( ( info->major_ver_ <= 11 ) || ( info->major_ver_ <= 12 && info->minor_ver_ <= 16 ) );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_LtChrome14( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_chrome:
		return (bool)( info->major_ver_ >= 14 );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_ChromiumInFutaba( const CBUAInfo info )
{
	switch( info->browser_ ){
	case CBUABrowser_e_chrome:
		return true;
	case CBUABrowser_e_opera:
		return (bool)( info->major_ver_ >= 14 );
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_Touch( const CBUAInfo info )
{
	switch( info->os_ ){
	case CBUAOS_e_android:
	case CBUAOS_e_iphone:
		return true;
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_FacileMobile( const CBUAInfo info )
{
	switch( info->os_ ){
	case CBUAOS_e_android:
	case CBUAOS_e_iphone:
		return true;
	default:
		break;
	}
	return false;
}
bool
Snippet_Snp_Webkit( const CBUAInfo info )
{
	return false;
}

