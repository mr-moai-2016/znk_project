#include <CB_config.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <Znk_str.h>
#include <Znk_str_path.h>

#if defined(Znk_TARGET_WINDOWS)
static bool st_is_dos_path = true;
#else
static bool st_is_dos_path = false;
#endif

static CBConfigInfo st_cb_config_info_futaba = {
	"futaba/ua_state.myf",
	"futaba_send.myf",
	"futaba/futaba_send_save.myf",
};
static CBConfigInfo st_cb_config_info_5ch = {
	"5ch/ua_state.myf",
	"5ch_send.myf",
	"5ch/5ch_send_save.myf",
};

static char st_negotiating_target[ 256 ] = "futaba";


CBConfigInfo*
CBConfig_theInfo( void )
{
	if( ZnkS_eq( st_negotiating_target, "futaba" ) ){
		return &st_cb_config_info_futaba;
	}
	if( ZnkS_eq( st_negotiating_target, "5ch" ) ){
		return &st_cb_config_info_5ch;
	}
	return NULL;
}

void
CBConfig_setNegotiatingTarget( const char* target )
{
	Znk_snprintf( st_negotiating_target, sizeof(st_negotiating_target), "%s", target );
}
const char*
CBConfig_theNegotiatingTarget( void )
{
	return st_negotiating_target;
}
const char*
CBConfig_theCBSrc( void )
{
	if( ZnkS_eq( st_negotiating_target, "futaba" ) ){
		return "may.2chan.net/b/futaba.php%3fmode%3dcat"; 
	}
	if( ZnkS_eq( st_negotiating_target, "5ch" ) ){
		return "hanabi.5ch.net"; 
	}
	return NULL;
}


const char*
CBConfig_hint_base_Snp( void )
{
	static const char* st_hint_base_Snp = "/moai2.0/customboy_hints_Snippet.html#";
	return st_hint_base_Snp;
}
const char*
CBConfig_hint_base_PV( void )
{
	static const char* st_hint_base_PV = "/moai2.0/customboy_hints_PostVars.html#";
	return st_hint_base_PV;
}


static ZnkStr st_moai_dir = NULL;

bool
CBConfig_moai_dir_initiate( ZnkStr ermsg )
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
CBConfig_moai_dir_finalize( void )
{
	ZnkStr_delete( st_moai_dir );
}
const char*
CBConfig_moai_dir( void )
{
	CBConfig_moai_dir_initiate( NULL );
	return ZnkStr_cstr( st_moai_dir );
}

static ZnkMyf st_custom_boy_myf = NULL;

ZnkMyf
CBConfig_custom_boy_myf( void )
{
	if( st_custom_boy_myf == NULL ){
		st_custom_boy_myf = ZnkMyf_create();
		if( !ZnkMyf_load( st_custom_boy_myf, "custom_boy.myf" ) ){
			ZnkMyf_destroy( st_custom_boy_myf );
			st_custom_boy_myf = NULL;
		}
	}
	return st_custom_boy_myf;
}
void
CBConfig_custom_boy_myf_destroy( void )
{
	ZnkMyf_destroy( st_custom_boy_myf );
	st_custom_boy_myf = NULL;
}


