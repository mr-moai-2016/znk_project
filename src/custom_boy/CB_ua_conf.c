#include <CB_ua_conf.h>
#include <CB_wgt_prim_db.h>

#include <Rano_log.h>

#include <Znk_obj_ary.h>
#include <Znk_str.h>
#include <Znk_myf.h>
#include <Znk_bird.h>
#include <Znk_missing_libc.h>

#include <stdlib.h>
#include <time.h>
#include <string.h>

struct RandStrImpl {
	ZnkObjAry obj_ary_;
};

#if 0
static void
report_wpary( CBWgtPrimAry wpary )
{
	const size_t size = CBWgtPrimAry_size( wpary );
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		CBWgtPrim wp = CBWgtPrimAry_at( wpary, idx );
		if( wp ){
			ZnkPrim* prim = CBWgtPrim_prim( wp );
			double   rate = CBWgtPrim_rate( wp );
			Znk_printf_e( "[%s][%1.3f]\n", ZnkPrim_cstr(prim), rate );
		}
	}
}
#endif

#if 0
static bool
process_func( const ZnkBird info,
		ZnkStr dst,
		const  ZnkStr src,
		const  char* key,
		size_t begin_idx, /* srcã‚É‚¨‚¯‚ékey‚ÌŠJŽnˆÊ’u‚ðhint‚Æ‚µ‚Ä—^‚¦‚é */
		void*  func_arg )
{
	/***
	 * ˆê‚Â‚Ìkey‚É‚Â‚«ˆê“x‚µ‚©select‚µ‚È‚¢‚æ‚¤‚É‚µ‚½‚¢.
	 * ‚½‚Æ‚¦‚Îversion”Ô†‚È‚Ç‚ð–ˆ‰ñƒ‰ƒ“ƒ_ƒ€‚É‚µ‚Ä‚Í‚Ü‚¸‚¢‚Ì‚Å.
	 * ˆê“xselect‚µ‚½‚à‚Ì‚ð•Ê“rZnkBird‚É“o˜^‚µ‚Ä‚¨‚­.
	 */
	if( ZnkBird_exist( info, key ) ){
		const char* val = ZnkBird_at( info, key );
		RanoLog_printf( "key0=[%s]\n", key );
		ZnkStr_add( dst, val );
	} else {
		ZnkStr key_str = ZnkStr_new( key );
		CBWgtPrimDB wpdb = Znk_force_ptr_cast( CBWgtPrimDB, func_arg );
		CBWgtPrimAry wpary = CBWgtPrimDB_findAry( wpdb, ZnkStr_cstr(key_str) );
		ZnkPrim* prim = CBWgtPrimAry_select( wpary );
		ZnkStr tmp = ZnkStr_new( "" );
		RanoLog_printf( "key1=[%s]\n", ZnkStr_cstr(key_str) );
		ZnkBird_extend( info, tmp, ZnkPrim_str(prim), 4 );
		RanoLog_printf( "key2=[%s] tmp=[%s]\n", ZnkStr_cstr(key_str), ZnkStr_cstr(tmp) );
		ZnkStr_add( dst, ZnkStr_cstr(tmp) );
		ZnkBird_regist( info, ZnkStr_cstr(key_str), ZnkStr_cstr(tmp) );
		ZnkStr_delete( tmp );
		ZnkStr_delete( key_str );
	}
	return true;
}
#endif

static int
getMajorVer( ZnkBird bird, const char* key )
{
	const char* val = ZnkBird_at( bird, key );
	const char* p = strchr( val, '.' );
	int major_ver = 0;
	if( p ){
		char buf[ 256 ] = "";
		ZnkS_copy( buf, sizeof(buf), val, p-val );
		ZnkS_getIntD( &major_ver, buf );
	} else {
		ZnkS_getIntD( &major_ver, val );
	}
	return major_ver;
}
static int
getMajorVer_byEmbededPtn( ZnkBird bird, const char* key, const char* ptn )
{
	int major_ver = 15;
	const char* val = ZnkBird_at( bird, key );
	const char* opr = strstr( val, ptn );
	if( opr ){
		const char* p;
		val = opr += Znk_strlen(ptn);
		p = strchr( val, '.' );
		if( p ){
			char buf[ 256 ] = "";
			ZnkS_copy( buf, sizeof(buf), val, p-val );
			ZnkS_getIntD( &major_ver, buf );
		} else {
			ZnkS_getIntD( &major_ver, val );
		}
	}
	return major_ver;
}

Znk_INLINE bool
findPtn( const char** p, const char* val, const char* ptn )
{
	*p = strstr( val, ptn );
	return (bool)( *p != NULL );
}

static void
getPlatform( ZnkBird bird, const char* key, CBUAOS* uaos, CBUAMachine* uamach )
{
	const char* val = ZnkBird_at( bird, key );
	const char* p = NULL;

	if(  findPtn( &p, val, "Windows" )
	  || findPtn( &p, val, "Win98" )
	  || findPtn( &p, val, "Win 9x" ) )
	{
		*uaos   = CBUAOS_e_windows;
		if( findPtn( &p, val, "WOW64" ) ){
			*uamach = CBUAMachine_e_WOW64;
		} else if( findPtn( &p, val, "Win64" ) ){
			*uamach = CBUAMachine_e_x64;
		} else {
			*uamach = CBUAMachine_e_x86;
		}
	} else if( findPtn( &p, val, "Android" ) ){
		*uaos   = CBUAOS_e_android;
		*uamach = CBUAMachine_e_android;
	} else if( findPtn( &p, val, "iPhone" ) || findPtn( &p, val, "iPad" ) ){
		*uaos   = CBUAOS_e_iphone;
		*uamach = CBUAMachine_e_iphone;
	} else if( findPtn( &p, val, "Linux" ) ){
		*uaos   = CBUAOS_e_linux;
		if( findPtn( &p, val, "x86_64" ) ){
			*uamach = CBUAMachine_e_x64;
		} else {
			*uamach = CBUAMachine_e_x86;
		}
	} else if( findPtn( &p, val, "Mac OS X" ) ){
		*uaos   = CBUAOS_e_mac;
		if( findPtn( &p, val, "PPC" ) ){
			*uamach = CBUAMachine_e_MacPPC;
		} else {
			*uamach = CBUAMachine_e_MacIntel;
		}
	} else if( findPtn( &p, val, "Macintosh" ) ){
		*uaos   = CBUAOS_e_mac;
		if( findPtn( &p, val, "PPC" ) ){
			*uamach = CBUAMachine_e_MacPPC;
		} else {
			*uamach = CBUAMachine_e_MacIntel;
		}
	}
}

bool
CBUAConf_getUA( ZnkStr ua_str, const char* browser, int* major_ver, CBUAOS* uaos, CBUAMachine* uamach, CBUABrowser* uabrowser )
{
	CBWgtPrimDB wpdb = CBWgtPrimDB_create();
	ZnkBird   bird = ZnkBird_create( "$[", "]$" );
	char conffile[ 256 ];
	bool result = false;

	Znk_snprintf( conffile, sizeof(conffile), "UA/ua_config_%s.myf", browser );
	if( !CBWgtPrimDB_load( wpdb, conffile ) ){
		goto FUNC_END;
	}

	CBWgtPrimDB_exec( wpdb, bird, ua_str, browser );

	if( ZnkS_eq( browser, "firefox" ) ){
		*major_ver = 0;
		if( ZnkBird_exist( bird, "gt4_version" ) ){
			*major_ver = getMajorVer( bird, "gt4_version" );
		} else if( ZnkBird_exist( bird, "firefox_3" ) ){
			*major_ver = 3;
		} else {
		}
		if( ZnkBird_exist( bird, "platform_3" ) ){
			getPlatform( bird, "platform_3", uaos, uamach );
		} else {
			getPlatform( bird, "platform", uaos, uamach );
		}
		*uabrowser = CBUABrowser_e_firefox;
	} else if( ZnkS_eq( browser, "ie" ) ){
		if( ZnkBird_exist( bird, "ie_7" ) ){
			getPlatform( bird, "platform_ie_7", uaos, uamach );
			*major_ver = 7;
		} else if( ZnkBird_exist( bird, "ie_8" ) ){
			getPlatform( bird, "platform_ie_8", uaos, uamach );
			*major_ver = 8;
		} else if( ZnkBird_exist( bird, "ie_9" ) ){
			getPlatform( bird, "platform_ie_9", uaos, uamach );
			*major_ver = 9;
		} else if( ZnkBird_exist( bird, "ie_10" ) ){
			getPlatform( bird, "platform_ie_10", uaos, uamach );
			*major_ver = 10;
		} else if( ZnkBird_exist( bird, "ie_11" ) ){
			getPlatform( bird, "platform_ie_11", uaos, uamach );
			*major_ver = 11;
		}
		*uabrowser = CBUABrowser_e_ie;
	} else if( ZnkS_eq( browser, "chrome" ) ){
		*major_ver = 0;
		if( ZnkBird_exist( bird, "version_gt27" ) ){
			*major_ver = getMajorVer( bird, "version_gt27" );
		} else if( ZnkBird_exist( bird, "chrome_lt26" ) ){
			*major_ver = getMajorVer_byEmbededPtn( bird, "chrome_lt26", "Chrome/" );
		} else {
		}
		getPlatform( bird, "platform", uaos, uamach );
		*uabrowser = CBUABrowser_e_chrome;
	} else if( ZnkS_eq( browser, "opera" ) ){
		*major_ver = 0;
		if( ZnkBird_exist( bird, "opera_gt15" ) ){
			*major_ver = getMajorVer_byEmbededPtn( bird, "opera_gt15", "OPR/" );
			getPlatform( bird, "platform_chr", uaos, uamach );
		} else if( ZnkBird_exist( bird, "opera_presto" ) ){
			*major_ver = getMajorVer_byEmbededPtn( bird, "opera_presto", "Version/" );
			getPlatform( bird, "platform_presto", uaos, uamach );
		} else {
			*major_ver = 9;
		}
		*uabrowser = CBUABrowser_e_opera;
	} else if( ZnkS_eq( browser, "safari" ) ){
		*major_ver = getMajorVer_byEmbededPtn( bird, "safari", "Version/" );
		getPlatform( bird, "platform", uaos, uamach );
		*uabrowser = CBUABrowser_e_safari;
	} else if( ZnkS_eq( browser, "android" ) ){
		*major_ver = 0;
		if( ZnkBird_exist( bird, "chr_gt27_ver" ) ){
			*major_ver = getMajorVer( bird, "chr_gt27_ver" );
			getPlatform( bird, "platform", uaos, uamach );
			*uabrowser = CBUABrowser_e_chrome;
		} else if( ZnkBird_exist( bird, "fx_ver" ) ){
			*major_ver = getMajorVer( bird, "fx_ver" );
			getPlatform( bird, "platform_firefox", uaos, uamach );
			*uabrowser = CBUABrowser_e_firefox;
		} else if( ZnkBird_exist( bird, "android_std_chr_lt26" ) ){
			*major_ver = getMajorVer_byEmbededPtn( bird, "android_std_chr_lt26", "Chrome/" );
			getPlatform( bird, "platform", uaos, uamach );
			*uabrowser = CBUABrowser_e_chrome;
		} else if( ZnkBird_exist( bird, "android_std_old" ) ){
			*major_ver = getMajorVer_byEmbededPtn( bird, "android_std_old", "Version/" );
			getPlatform( bird, "platform", uaos, uamach );
			*uabrowser = CBUABrowser_e_android_std; /* ‚Æ‚è‚ ‚¦‚¸ */
		} else if( ZnkBird_exist( bird, "android_opera_presto" ) ){
			*major_ver = getMajorVer_byEmbededPtn( bird, "android_opera_presto", "Version/" );
			getPlatform( bird, "platform_opera_presto", uaos, uamach );
			*uabrowser = CBUABrowser_e_opera;
		} else if( ZnkBird_exist( bird, "android_opera_chr" ) ){
			*major_ver = getMajorVer_byEmbededPtn( bird, "android_opera_chr", "OPR/" );
			getPlatform( bird, "platform_opera_chr", uaos, uamach );
			*uabrowser = CBUABrowser_e_opera;
		} else {
			*uabrowser = CBUABrowser_e_Unknown;
		}
	} else if( ZnkS_eq( browser, "iphone" ) ){
		*major_ver = getMajorVer_byEmbededPtn( bird, "iphone", "Version/" );
		getPlatform( bird, "platform", uaos, uamach );
		*uabrowser = CBUABrowser_e_safari;
	}

	result = true;
FUNC_END:
	ZnkBird_destroy( bird );
	CBWgtPrimDB_destroy( wpdb );
	return result;
}

bool
CBUAConf_getScsz( ZnkStr scsz_str )
{
	CBWgtPrimDB wpdb = CBWgtPrimDB_create();
	ZnkBird   bird = ZnkBird_create( "$[", "]$" );
	const char* conffile = "UA/screen_size.myf";
	bool result = false;

	if( !CBWgtPrimDB_load( wpdb, conffile ) ){
		goto FUNC_END;
	}

	CBWgtPrimDB_exec( wpdb, bird, scsz_str, "scsz" );

	result = true;
FUNC_END:
	ZnkBird_destroy( bird );
	CBWgtPrimDB_destroy( wpdb );
	return result;
}

bool
CBUAConf_getBrowser_fromCategory( ZnkStr browser, const char* category )
{
	CBWgtPrimDB wpdb = CBWgtPrimDB_create();
	ZnkBird   bird = ZnkBird_create( "$[", "]$" );
	const char* conffile = "UA/ua_category.myf";
	bool result = false;

	if( !CBWgtPrimDB_load( wpdb, conffile ) ){
		goto FUNC_END;
	}

	CBWgtPrimDB_exec( wpdb, bird, browser, category );

	result = true;
FUNC_END:
	ZnkBird_destroy( bird );
	CBWgtPrimDB_destroy( wpdb );
	return result;
}

bool
CBUAConf_getREPath( ZnkStr ans_path, const char* RE_key )
{
	CBWgtPrimDB wpdb = CBWgtPrimDB_create();
	ZnkBird   bird = ZnkBird_create( "$[", "]$" );
	const char* conffile = "RE/RE_path.myf";
	bool result = false;

	if( !CBWgtPrimDB_load( wpdb, conffile ) ){
		goto FUNC_END;
	}

	CBWgtPrimDB_exec( wpdb, bird, ans_path, RE_key );

	result = true;
FUNC_END:
	ZnkBird_destroy( bird );
	CBWgtPrimDB_destroy( wpdb );
	return result;
}

