#include <CB_finger.h>
#include <CB_snippet.h>
#include <CB_wgt_prim.h>
#include <CB_ua_conf.h>

#include <Znk_s_base.h>
#include <Znk_str.h>
#include <Znk_stdc.h>
#include <Znk_missing_libc.h>
#include <Znk_str_fio.h>
#include <Znk_myf.h>
#include <Znk_htp_util.h>

#include <stdlib.h>
#include <limits.h>

static unsigned int
getRandomUInt( void )
{
	unsigned int r = (unsigned int)( ( rand() / (double)RAND_MAX ) * UINT_MAX );
	return r;
}



static void
CBFinger_loadREData( const char* RE_key, ZnkStr str )
{
	char in_filename[ 4096 ] = "";
	ZnkFile fp = NULL;

	Znk_snprintf( in_filename, sizeof(in_filename), "RE/%s.dat", RE_key );
	fp = Znk_fopen( in_filename, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		ZnkStr_add( str, "data:image/png;base64," );
		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				break;
			}
			ZnkStr_chompNL( line );
			if( ZnkStr_empty( line ) ){
				continue;
			}
			ZnkStr_add( str, ZnkStr_cstr(line) );
		}
		Znk_fclose( fp );
	}
}
static void
CBFinger_loadREPng( const char* file_path, ZnkStr str )
{
	ZnkFile fp = NULL;

	fp = Znk_fopen( file_path, "rb" );
	if( fp ){
		ZnkBfr bfr = ZnkBfr_create_null();
		uint8_t buf[ 4096 ];
		size_t read_size = 0;
		ZnkStr_add( str, "data:image/png;base64," );
		while( true ){
			read_size = Znk_fread( buf, sizeof(buf), fp );
			if( read_size == 0 ){
				break;
			}
			ZnkBfr_append_dfr( bfr, buf, read_size );
		}
		Znk_fclose( fp );
		ZnkHtpB64_encode( str, ZnkBfr_data(bfr), ZnkBfr_size(bfr) );
		ZnkBfr_destroy( bfr );
	}
}
bool
CBFinger_saveREPng( const char* filepath, ZnkStr str )
{
	bool result = false;
	ZnkFile fp = NULL;
	if( ZnkStr_isBegin( str, "data:image/png;base64," ) ){
		const char* bs64_str = ZnkStr_cstr( str ) + Znk_strlen_literal( "data:image/png;base64," );
		fp = Znk_fopen( filepath, "wb" );
		if( fp ){
			ZnkBfr bfr = ZnkBfr_create_null();
			uint8_t* data = NULL;
			size_t   data_size = 0;
			size_t   write_size = 0;
			ZnkHtpB64_decode( bfr, bs64_str, Znk_NPOS );
			data       = ZnkBfr_data( bfr );
			data_size  = ZnkBfr_size( bfr );
			write_size = Znk_fwrite( data, data_size, fp );
			result = (bool)( write_size == data_size );
			Znk_fclose( fp );
			ZnkBfr_destroy( bfr );
		}
	}
	return result;
}


void
CBFinger_Fgp_language( ZnkStr str, const CBUAInfo info, const char* RE )
{
	switch( info->os_ ){
	case CBUAOS_e_android:
		ZnkStr_add( str, "ja-JP" );
		break;
	case CBUAOS_e_iphone:
		ZnkStr_add( str, "ja-jp" );
		break;
	case CBUAOS_e_mac:
		if( info->browser_ == CBUABrowser_e_firefox ){
			ZnkStr_add( str, "ja-JP-mac" );
		} else if( info->browser_ == CBUABrowser_e_safari ){
			ZnkStr_add( str, "ja-jp" );
		} else {
			/* TODO:確率的処理 */
			ZnkStr_add( str, "ja" );
		}
		break;
	default:
		/* TODO:確率的処理 */
		ZnkStr_add( str, "ja" );
		break;
	}
}
void
CBFinger_Fgp_colorDepth( ZnkStr str, const CBUAInfo info, const char* RE )
{
	ZnkStr_addf( str, "%zu", info->sc_depth_ );
}
void
CBFinger_Fgp_ScreenResolution( ZnkStr str, const CBUAInfo info, const char* RE )
{
	ZnkStr_addf( str, "%zux%zu", info->sc_width_, info->sc_height_ );
}
void
CBFinger_Fgp_TimezoneOffset( ZnkStr str, const CBUAInfo info, const char* RE )
{
	ZnkStr_add( str, "-540" );
}
void
CBFinger_Fgp_SessionStorage( ZnkStr str, const CBUAInfo info, const char* RE )
{
	if( Snippet_Snp_SessionStorage( info ) ){
		ZnkStr_add( str, "true" );
	} else {
		ZnkStr_add( str, "false" );
	}
}
void
CBFinger_Fgp_LocalStorage( ZnkStr str, const CBUAInfo info, const char* RE )
{
	if( Snippet_Snp_LocalStorage( info ) ){
		ZnkStr_add( str, "true" );
	} else {
		ZnkStr_add( str, "false" );
	}
}
void
CBFinger_Fgp_IndexedDB( ZnkStr str, const CBUAInfo info, const char* RE )
{
	if( Snippet_Snp_IndexedDB( info ) ){
		ZnkStr_add( str, "true" );
	} else {
		ZnkStr_add( str, "false" );
	}
}
void
CBFinger_Fgp_BodyAddBehavior( ZnkStr str, const CBUAInfo info, const char* RE )
{
	ZnkStr_add( str, "undefined" );
}
void
CBFinger_Fgp_OpenDatabase( ZnkStr str, const CBUAInfo info, const char* RE )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		ZnkStr_add( str, "undefined" );
		break;
	case CBUABrowser_e_chrome:
	case CBUABrowser_e_opera:
		ZnkStr_add( str, "function" );
		break;
	default:
		/* TODO:確率的処理 */
		ZnkStr_add( str, "function" );
		break;
	}
}
void
CBFinger_Fgp_CpuClass( ZnkStr str, const CBUAInfo info, const char* RE )
{
	/* これは実際大抵のケースでは空値となっているようにも思える */
	/* TODO:確率的処理が妥当か？ */
	ZnkStr_add( str, "" );
}
void
CBFinger_Fgp_Platform( ZnkStr str, const CBUAInfo info, const char* RE )
{
	switch( info->os_ ){
	case CBUAOS_e_windows:
		/* TODO:条件的確率的処理 */
		if( info->machine_ == CBUAMachine_e_x86 ){
			ZnkStr_add( str, "Win32" );
		} else {
			ZnkStr_add( str, "Win64" );
		}
		break;
	case CBUAOS_e_linux:
		/* TODO:条件的確率的処理 */
		if( info->machine_ == CBUAMachine_e_x86 ){
			ZnkStr_add( str, "Linux i686" );
		} else {
			ZnkStr_add( str, "Linux x86_64" );
		}
		break;
	case CBUAOS_e_android:
		ZnkStr_add( str, "Linux armv7l" );
		break;
	case CBUAOS_e_iphone:
		ZnkStr_add( str, "iPhone" );
		break;
	case CBUAOS_e_mac:
		if( info->machine_ == CBUAMachine_e_MacIntel ){
			ZnkStr_add( str, "MacIntel" );
		} else {
			ZnkStr_add( str, "PPC" );
		}
		break;
	default:
		ZnkStr_add( str, "" );
		break;
	}
}
void
CBFinger_Fgp_doNotTrack( ZnkStr str, const CBUAInfo info, const char* RE )
{
	switch( info->browser_ ){
	case CBUABrowser_e_firefox:
		/* TODO:条件的確率的処理 */
		ZnkStr_add( str, "unspecified" );
		//ZnkStr_add( str, "true" );
		break;
	default:
		ZnkStr_add( str, "" );
	}
}
void
CBFinger_Fgp_PluginsString( ZnkStr str, const CBUAInfo info, const char* RE )
{
#if 1
	const char* Chrome =
		"Widevine Content Decryption Module::"
		"Enables Widevine licenses for playback of HTML audio/video content. (version: 1.4.8.903)::"
		"application/x-ppapi-widevine-cdm~;Shockwave Flash::"
		"Shockwave Flash 22.0 r0::"
		"application/x-shockwave-flash~swf,application/futuresplash~spl;Chrome PDF Viewer::"
		"::"
		"application/pdf~;Native Client::"
		"::"
		"application/x-nacl~,application/x-pnacl~;Chrome PDF Viewer::"
		"Portable Document Format::"
		"application/x-google-chrome-pdf~pdf";

	const char* Opera =
		"Widevine Content Decryption Module::"
		"Enables Widevine licenses for playback of HTML audio/video content. (version: 1.4.8.866)::"
		"application/x-ppapi-widevine-cdm~;Chrome PDF Viewer::"
		"::"
		"application/pdf~;Shockwave Flash::"
		"Shockwave Flash 21.0 r0::"
		"application/x-shockwave-flash~swf,application/futuresplash~spl;Chrome PDF Viewer::"
		"Portable Document Format::"
		"application/x-google-chrome-pdf~pdf;MicrosoftR DRM::"
		"DRM Netscape Network Object::"
		"application/x-drm-v2~nip;Windows Media Player Plug-in Dynamic Link Library::"
		"Npdsplay dll::"
		"application/asx~*,video/x-ms-asf-plugin~*,application/x-mplayer2~*,video/x-ms-asf~asf,asx,*,video/x-ms-wm~wm,*,audio/x-ms-wma~wma,*,audio/x-ms-wax~wax,*,video/x-ms-wmv~wmv,*,video/x-ms-wvx~wvx,*;MicrosoftR DRM::"
		"DRM Store Netscape Plugin::"
		"application/x-drm~nip;Google Update::"
		"Google Update::"
		"application/x-vnd.google.update3webcontrol.3~,application/x-vnd.google.oneclickctrl.9~;Windows Presentation Foundation::"
		"Windows Presentation Foundation (WPF) plug-in for Mozilla browsers::"
		"application/x-ms-xbap~xbap,application/xaml+xml~xaml";

	const char* Firefox =
		"MicrosoftR DRM::"
		"DRM Netscape Network Object::"
		"application/x-drm-v2~nip;MicrosoftR DRM::"
		"DRM Store Netscape Plugin::"
		"application/x-drm~nip;Shockwave Flash::"
		"Shockwave Flash 16.0 r0::"
		"application/x-shockwave-flash~swf,application/futuresplash~spl";

	const char* IE = "";
#endif

	/* ほぼ完全自由文字列とみなしてよい */
	ZnkStr_addf( str, "%u", getRandomUInt() );
}
void
CBFinger_Fgp_Canvas( ZnkStr str, const char* RE )
{
	/* RE,ドライバ,グラフィックスカード,インストールフォントなどに依存 */
	ZnkStr file_path = ZnkStr_new( "" );
	CBUAConf_getREPath( file_path, RE );
	CBFinger_loadREPng( ZnkStr_cstr(file_path), str );
	ZnkStr_delete( file_path );
}

