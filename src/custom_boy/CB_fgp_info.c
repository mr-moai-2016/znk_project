#include <CB_fgp_info.h>
#include <CB_finger.h>
#include <CB_wgt_prim.h>
#include <CB_config.h>

#include <Rano_hash.h>

#include <Znk_myf.h>

CBFgpInfo
CBFgpInfo_create( void )
{
	CBFgpInfo fgp_info = Znk_malloc( sizeof(struct CBFgpInfo_tag) );
	fgp_info->Fgp_userAgent_        = ZnkStr_new( "" );
	fgp_info->Fgp_language_         = ZnkStr_new( "" );
	fgp_info->Fgp_colorDepth_       = ZnkStr_new( "" );
	fgp_info->Fgp_ScreenResolution_ = ZnkStr_new( "" );
	fgp_info->Fgp_TimezoneOffset_   = ZnkStr_new( "" );
	fgp_info->Fgp_SessionStorage_   = ZnkStr_new( "" );
	fgp_info->Fgp_LocalStorage_     = ZnkStr_new( "" );
	fgp_info->Fgp_IndexedDB_        = ZnkStr_new( "" );
	fgp_info->Fgp_BodyAddBehavior_  = ZnkStr_new( "" );
	fgp_info->Fgp_OpenDatabase_     = ZnkStr_new( "" );
	fgp_info->Fgp_CpuClass_         = ZnkStr_new( "" );
	fgp_info->Fgp_Platform_         = ZnkStr_new( "" );
	fgp_info->Fgp_doNotTrack_       = ZnkStr_new( "" );
	fgp_info->Fgp_PluginsString_    = ZnkStr_new( "" );
	return fgp_info;
}
void
CBFgpInfo_destroy( CBFgpInfo fgp_info )
{
	if( fgp_info ){
		ZnkStr_delete( fgp_info->Fgp_userAgent_ );
		ZnkStr_delete( fgp_info->Fgp_language_ );
		ZnkStr_delete( fgp_info->Fgp_colorDepth_ );
		ZnkStr_delete( fgp_info->Fgp_ScreenResolution_ );
		ZnkStr_delete( fgp_info->Fgp_TimezoneOffset_ );
		ZnkStr_delete( fgp_info->Fgp_SessionStorage_ );
		ZnkStr_delete( fgp_info->Fgp_LocalStorage_ );
		ZnkStr_delete( fgp_info->Fgp_IndexedDB_ );
		ZnkStr_delete( fgp_info->Fgp_BodyAddBehavior_ );
		ZnkStr_delete( fgp_info->Fgp_OpenDatabase_ );
		ZnkStr_delete( fgp_info->Fgp_CpuClass_ );
		ZnkStr_delete( fgp_info->Fgp_Platform_ );
		ZnkStr_delete( fgp_info->Fgp_doNotTrack_ );
		ZnkStr_delete( fgp_info->Fgp_PluginsString_ );
		Znk_free( fgp_info );
	}
}

#define SET_MEMBER( info, vars, key ) do{ \
		ZnkVarp varp = ZnkVarpAry_find_byName_literal( vars, #key, false ); \
		if( varp ){ \
			ZnkStr_set( info->key##_, ZnkVar_cstr( varp ) ); \
		} \
} while(0)

#define SET_VARS_ELEM( vars, info, key ) do{ \
		ZnkVarp varp = ZnkVarpAry_find_byName_literal( vars, #key, false ); \
		if( varp ){ \
			ZnkStr str = info->key##_; \
			ZnkVar_set_val_Str( varp, ZnkStr_cstr(str), ZnkStr_leng(str) ); \
		} \
} while(0)

void
CBFgpInfo_init_byCBVars( CBFgpInfo fgp_info, ZnkVarpAry cb_vars )
{
	SET_MEMBER( fgp_info, cb_vars, Fgp_userAgent );
	SET_MEMBER( fgp_info, cb_vars, Fgp_language );
	SET_MEMBER( fgp_info, cb_vars, Fgp_colorDepth );
	SET_MEMBER( fgp_info, cb_vars, Fgp_ScreenResolution );
	SET_MEMBER( fgp_info, cb_vars, Fgp_TimezoneOffset );
	SET_MEMBER( fgp_info, cb_vars, Fgp_SessionStorage );
	SET_MEMBER( fgp_info, cb_vars, Fgp_LocalStorage );
	SET_MEMBER( fgp_info, cb_vars, Fgp_IndexedDB );
	SET_MEMBER( fgp_info, cb_vars, Fgp_BodyAddBehavior );
	SET_MEMBER( fgp_info, cb_vars, Fgp_OpenDatabase );
	SET_MEMBER( fgp_info, cb_vars, Fgp_CpuClass );
	SET_MEMBER( fgp_info, cb_vars, Fgp_Platform );
	SET_MEMBER( fgp_info, cb_vars, Fgp_doNotTrack );
	SET_MEMBER( fgp_info, cb_vars, Fgp_PluginsString );
}
bool
CBFgpInfo_load( CBFgpInfo fgp_info, const char* filename, const char* sec_name )
{
	ZnkMyf myf = ZnkMyf_create();
	bool result = false;

	result = ZnkMyf_load( myf, filename );
	if( result ){
		ZnkVarpAry vars;
		vars = ZnkMyf_find_vars( myf, sec_name );
	
		SET_MEMBER( fgp_info, vars, Fgp_userAgent );
		SET_MEMBER( fgp_info, vars, Fgp_language );
		SET_MEMBER( fgp_info, vars, Fgp_colorDepth );
		SET_MEMBER( fgp_info, vars, Fgp_ScreenResolution );
		SET_MEMBER( fgp_info, vars, Fgp_TimezoneOffset );
		SET_MEMBER( fgp_info, vars, Fgp_SessionStorage );
		SET_MEMBER( fgp_info, vars, Fgp_LocalStorage );
		SET_MEMBER( fgp_info, vars, Fgp_IndexedDB );
		SET_MEMBER( fgp_info, vars, Fgp_BodyAddBehavior );
		SET_MEMBER( fgp_info, vars, Fgp_OpenDatabase );
		SET_MEMBER( fgp_info, vars, Fgp_CpuClass );
		SET_MEMBER( fgp_info, vars, Fgp_Platform );
		SET_MEMBER( fgp_info, vars, Fgp_doNotTrack );
		SET_MEMBER( fgp_info, vars, Fgp_PluginsString );
	}

	ZnkMyf_destroy( myf );

	return result;
}
bool
CBFgpInfo_save( const CBFgpInfo fgp_info, const char* filename, const char* sec_name )
{
	ZnkMyf myf = ZnkMyf_create();
	bool result = false;

	result = ZnkMyf_load( myf, filename );
	if( result ){
		ZnkVarpAry vars;
		vars = ZnkMyf_find_vars( myf, sec_name );
	
		SET_VARS_ELEM( vars, fgp_info, Fgp_userAgent );
		SET_VARS_ELEM( vars, fgp_info, Fgp_language );
		SET_VARS_ELEM( vars, fgp_info, Fgp_colorDepth );
		SET_VARS_ELEM( vars, fgp_info, Fgp_ScreenResolution );
		SET_VARS_ELEM( vars, fgp_info, Fgp_TimezoneOffset );
		SET_VARS_ELEM( vars, fgp_info, Fgp_SessionStorage );
		SET_VARS_ELEM( vars, fgp_info, Fgp_LocalStorage );
		SET_VARS_ELEM( vars, fgp_info, Fgp_IndexedDB );
		SET_VARS_ELEM( vars, fgp_info, Fgp_BodyAddBehavior );
		SET_VARS_ELEM( vars, fgp_info, Fgp_OpenDatabase );
		SET_VARS_ELEM( vars, fgp_info, Fgp_CpuClass );
		SET_VARS_ELEM( vars, fgp_info, Fgp_Platform );
		SET_VARS_ELEM( vars, fgp_info, Fgp_doNotTrack );
		SET_VARS_ELEM( vars, fgp_info, Fgp_PluginsString );

		result = ZnkMyf_save( myf, "ua_state.myf" );
	}

	ZnkMyf_destroy( myf );

	return result;
}

void
CBFgpInfo_proc_byStateMode( CBFgpInfo fgp_info, CBFgpStateMode state_mode )
{
	CBConfigInfo* info = CBConfig_theInfo(); 
	switch( state_mode ){
	case CBFgpStateMode_e_StockContext:
	case CBFgpStateMode_e_SaveFile:
		CBFgpInfo_save( fgp_info, info->ua_state_filename_, "FingerContext" );
		break;
	case CBFgpStateMode_e_CBGetting:
		/* none */
		break;
	default:
		CBFgpInfo_load( fgp_info, info->ua_state_filename_, "FingerContext" );
		break;
	}
}

static const char*
getRandamREKey( void )
{
	double rate = 0.0;
	static const char* st_table[] = {
		"webkit_mobile",
		"gecko_gdi",
		"gecko_x11",
		"blink",
		"webkit",
	};
	rate = CBWgtRand_getRandomReal( 0.0, (double)Znk_NARY(st_table) );
	return st_table[ (int)rate ];
}

void
CBFgpInfo_generate_byCBUAInfo( CBFgpInfo fgp_info, ZnkStr RE_key, const CBUAInfo ua_info, const double RE_random_factor )
{
	ZnkStr_set( RE_key, "webkit" );
	{
		double rate = CBWgtRand_getRandomReal( 0.0, 1.0 );
		switch( ua_info->browser_ ){
		case CBUABrowser_e_chrome:
			if( rate >= RE_random_factor ){
				if( ua_info->major_ver_ >= 28 ){
					ZnkStr_set( RE_key, "blink" );
				} else {
					ZnkStr_set( RE_key, "webkit" );
				}
			} else {
				ZnkStr_set( RE_key, getRandamREKey() );
			}
			break;
		case CBUABrowser_e_opera:
			if( rate >= RE_random_factor ){
				ZnkStr_set( RE_key, "blink" ); /* ‚Æ‚è‚ ‚¦‚¸ */
			} else {
				ZnkStr_set( RE_key, getRandamREKey() );
			}
			break;
		case CBUABrowser_e_firefox:
			if( rate >= RE_random_factor ){
				switch( ua_info->os_ ){
				case CBUAOS_e_windows:
					ZnkStr_set( RE_key, "gecko_gdi" );
					break;
				default:
					ZnkStr_set( RE_key, "gecko_x11" );
					break;
				}
			} else {
				ZnkStr_set( RE_key, getRandamREKey() );
			}
			break;
		default:
			if( rate >= RE_random_factor ){
				switch( ua_info->os_ ){
				case CBUAOS_e_android:
				case CBUAOS_e_iphone:
					ZnkStr_set( RE_key, "webkit_mobile" );
					break;
				default:
					break;
				}
			} else {
				ZnkStr_set( RE_key, getRandamREKey() );
			}
			break;
		}
	}

	ZnkStr_clear( fgp_info->Fgp_language_ );
	CBFinger_Fgp_language( fgp_info->Fgp_language_, ua_info, ZnkStr_cstr(RE_key) );

	ZnkStr_clear( fgp_info->Fgp_colorDepth_ );
	CBFinger_Fgp_colorDepth( fgp_info->Fgp_colorDepth_, ua_info, ZnkStr_cstr(RE_key) );

	ZnkStr_clear( fgp_info->Fgp_ScreenResolution_ );
	CBFinger_Fgp_ScreenResolution( fgp_info->Fgp_ScreenResolution_, ua_info, ZnkStr_cstr(RE_key) );

	ZnkStr_clear( fgp_info->Fgp_TimezoneOffset_ );
	CBFinger_Fgp_TimezoneOffset( fgp_info->Fgp_TimezoneOffset_, ua_info, ZnkStr_cstr(RE_key) );

	ZnkStr_clear( fgp_info->Fgp_SessionStorage_ );
	CBFinger_Fgp_SessionStorage( fgp_info->Fgp_SessionStorage_, ua_info, ZnkStr_cstr(RE_key) );

	ZnkStr_clear( fgp_info->Fgp_LocalStorage_ );
	CBFinger_Fgp_LocalStorage( fgp_info->Fgp_LocalStorage_, ua_info, ZnkStr_cstr(RE_key) );

	ZnkStr_clear( fgp_info->Fgp_IndexedDB_ );
	CBFinger_Fgp_IndexedDB( fgp_info->Fgp_IndexedDB_, ua_info, ZnkStr_cstr(RE_key) );

	ZnkStr_set( fgp_info->Fgp_BodyAddBehavior_, "undefined" );

	ZnkStr_clear( fgp_info->Fgp_OpenDatabase_ );
	CBFinger_Fgp_OpenDatabase( fgp_info->Fgp_OpenDatabase_, ua_info, ZnkStr_cstr(RE_key) );

	ZnkStr_clear( fgp_info->Fgp_CpuClass_ );
	CBFinger_Fgp_CpuClass( fgp_info->Fgp_CpuClass_, ua_info, ZnkStr_cstr(RE_key) );

	ZnkStr_clear( fgp_info->Fgp_Platform_ );
	CBFinger_Fgp_Platform( fgp_info->Fgp_Platform_, ua_info, ZnkStr_cstr(RE_key) );

	ZnkStr_clear( fgp_info->Fgp_doNotTrack_ );
	CBFinger_Fgp_doNotTrack( fgp_info->Fgp_doNotTrack_, ua_info, ZnkStr_cstr(RE_key) );

	ZnkStr_clear( fgp_info->Fgp_PluginsString_ );
	CBFinger_Fgp_PluginsString( fgp_info->Fgp_PluginsString_, ua_info, ZnkStr_cstr(RE_key) );
}

static size_t
addKeys( ZnkStr keys, const char* fgp_str, size_t count )
{
	if( count ){
		ZnkStr_add( keys, "###" );
	}
	ZnkStr_add( keys, fgp_str );
	return count + 1;
}
uint32_t
CBFgpInfo_calcHash( const CBFgpInfo fgp_info,
		bool with_UA, bool with_PluginStrings, bool with_ScreenResolution, const char* RE_key )
{
	uint32_t hash_i32 = 0;
	ZnkStr keys   = ZnkStr_new( "" );
	size_t count = 0;
	if( with_UA ){
		count = addKeys( keys, ZnkStr_cstr(fgp_info->Fgp_userAgent_), count );
	}

	count = addKeys( keys, ZnkStr_cstr(fgp_info->Fgp_language_), count );
	count = addKeys( keys, ZnkStr_cstr(fgp_info->Fgp_colorDepth_), count );

	if( with_ScreenResolution ){
		count = addKeys( keys, ZnkStr_cstr(fgp_info->Fgp_ScreenResolution_), count );
	}

	count = addKeys( keys, ZnkStr_cstr(fgp_info->Fgp_TimezoneOffset_), count );
	count = addKeys( keys, ZnkStr_cstr(fgp_info->Fgp_SessionStorage_), count );
	count = addKeys( keys, ZnkStr_cstr(fgp_info->Fgp_LocalStorage_), count );
	count = addKeys( keys, ZnkStr_cstr(fgp_info->Fgp_IndexedDB_), count );
	count = addKeys( keys, ZnkStr_cstr(fgp_info->Fgp_BodyAddBehavior_), count );
	count = addKeys( keys, ZnkStr_cstr(fgp_info->Fgp_OpenDatabase_), count );
	count = addKeys( keys, ZnkStr_cstr(fgp_info->Fgp_CpuClass_), count );
	count = addKeys( keys, ZnkStr_cstr(fgp_info->Fgp_Platform_), count );
	count = addKeys( keys, ZnkStr_cstr(fgp_info->Fgp_doNotTrack_), count );

	if( with_PluginStrings ){
		count = addKeys( keys, ZnkStr_cstr(fgp_info->Fgp_PluginsString_), count );
	}

	{
		ZnkStr fgp_str = ZnkStr_new( "" );
		CBFinger_Fgp_Canvas( fgp_str, RE_key );
		if( !ZnkStr_empty( fgp_str ) ){
			count = addKeys( keys, ZnkStr_cstr(fgp_str), count );
		}
		ZnkStr_delete( fgp_str );
	}

	hash_i32 = RanoHash_murmurhash3_32_gc( ZnkStr_cstr(keys), ZnkStr_leng(keys), 31 );

	ZnkStr_delete( keys );
	return hash_i32;
}
