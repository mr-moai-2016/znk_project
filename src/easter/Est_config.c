#include <Est_config.h>
#include <Est_base.h>
#include <Est_hint_manager.h>
#include <Est_unid.h>

#include <Znk_htp_util.h>
#include <Znk_bird.h>
#include <Znk_dir.h>
#include <Znk_dir_recursive.h>
#include <Znk_missing_libc.h>
#include <Znk_str_ex.h>
#include <Znk_str_fio.h>
#include <Znk_str_ptn.h>
#include <Znk_str_path.h>
#include <Znk_mem_find.h>
#include <Znk_net_ip.h>
#include <Znk_nset.h>
#include <Znk_process.h>
#include <Znk_date.h>
#include <Znk_envvar.h>

#include <Rano_log.h>
#include <Rano_module.h>
#include <Rano_module_ary.h>
#include <Rano_file_info.h>
#include <Rano_conf_util.h>
#include <time.h>
#include <ctype.h>

static char          st_moai_authentic_key[ 256 ] = "";
static ZnkStr        st_moai_dir = NULL;

static char          st_acceptable_host[ 256 ] = "ANY";
static char          st_filters_dir[ 256 ] = "filters";

static ZnkMyf        st_easter_myf;
static ZnkMyf        st_tags_myf;
static ZnkMyf        st_target_myf;
static ZnkMyf        st_hosts_myf;
static ZnkMyf        st_input_hiddens_myf;
static RanoModuleAry st_mod_ary;

static ZnkStrAry  st_allow_js_hosts = NULL;

static ZnkStr     st_direct_img_link = NULL;
static ZnkStr     st_auto_link = NULL;

static ZnkStr     st_profile_dir = NULL;
static ZnkStr     st_favorite_dir = NULL;
static ZnkStr     st_stockbox_dir = NULL;
static ZnkStr     st_userbox_dir  = NULL;
static ZnkStr     st_topics_dir  = NULL;
static char       st_xhr_dmz[ 256 ] = "";
static uint16_t   st_moai_port = 8124;
static uint16_t   st_xhr_dmz_port = 8125;
static char       st_parent_proxy[ 4096 ] = "NONE";
static size_t     st_count = 0;
static size_t     st_show_file_num = 10;
static size_t     st_thumb_size = 64;
static size_t     st_line_file_num = 4;
static size_t     st_line_tag_num  = 4;
static size_t     st_preview_max_width  = 860;
static size_t     st_preview_max_height = 820;
static size_t     st_cache_days_ago   = 7;
static size_t     st_dustbox_days_ago = 10;
static char       st_explicit_referer[ 4096 ] = "";

const char*
EstConfig_getTmpDirPID( bool with_end_dsp )
{
	static char st_tmpdir[ 256 ] = "tmp/pid";
	static char st_tmpdir_dsp[ 256 ] = "tmp/pid/";
	static bool st_initialized = false;
	if( !st_initialized ){
		//unsigned int pid = ZnkProcess_getCurrentProcessId();
		size_t pid = st_count;
		st_initialized = true;
		Znk_snprintf( st_tmpdir,     sizeof(st_tmpdir),     "tmp/pid/%zu",  pid );
		Znk_snprintf( st_tmpdir_dsp, sizeof(st_tmpdir_dsp), "tmp/pid/%zu/", pid );
	}
	return with_end_dsp ? st_tmpdir_dsp : st_tmpdir;
}
const char*
EstConfig_getTmpDirCommon( bool with_end_dsp )
{
	static char st_tmpdir[ 256 ] = "tmp/common";
	static char st_tmpdir_dsp[ 256 ] = "tmp/common/";
	return with_end_dsp ? st_tmpdir_dsp : st_tmpdir;
}
size_t
EstConfig_getSessionCount( void )
{
	return st_count;
}
size_t
EstConfig_getShowFileNum( void )
{
	return st_show_file_num;
}
size_t
EstConfig_getThumbSize( void )
{
	return st_thumb_size;
}
size_t
EstConfig_getLineFileNum( void )
{
	return st_line_file_num;
}
size_t
EstConfig_getLineTagNum( void )
{
	return st_line_tag_num;
}
size_t
EstConfig_getPreviewMaxWidth( void )
{
	return st_preview_max_width;
}
size_t
EstConfig_getPreviewMaxHeight( void )
{
	return st_preview_max_height;
}
size_t
EstConfig_getCacheDaysAgo( void )
{
	return st_cache_days_ago;
}
size_t
EstConfig_getDustboxDaysAgo( void )
{
	return st_dustbox_days_ago;
}
const char*
EstConfig_getExplicitReferer( void )
{
	return st_explicit_referer;
}

ZnkStr
EstConfig_getMoaiDir( void )
{
	const char dsp = '/';
	size_t depth = 5;
	ZnkStr moai_dir = ZnkStr_new( "../" );
	if( !ZnkStrPath_searchParentDir( moai_dir, depth, "target.myf", ZnkDirType_e_File, dsp ) ){
		RanoLog_printf( "Easter : Searching moai_dir is failure. target.myf does not found.\n" );
		ZnkStr_delete( moai_dir );
		return NULL;
	}
	return moai_dir;
}
bool
EstConfig_loadAuthenticKey( char* authentic_key_buf, size_t authentic_key_buf_size, const char* moai_dir )
{
	char path[ 256 ] = "";
	ZnkFile fp = NULL;
	Znk_snprintf( path, sizeof(path), "%sauthentic_key.dat", moai_dir );
	fp = Znk_fopen( path, "rb" );
	if( fp ){
		Znk_fgets( authentic_key_buf, authentic_key_buf_size, fp );
		Znk_fclose( fp );
		return true;
	}
	return false;
}

static bool
getRealPath( ZnkStr ans, ZnkVarpAry vars, const char* conf_varname, size_t default_idx, size_t envvar_idx, ZnkBird bird )
{
	ZnkVarp varp = ZnkVarpAry_findObj_byName( vars, conf_varname, Znk_NPOS, false );
	if( varp ){
		/* found */
		ZnkStrAry sda = ZnkVar_str_ary( varp );
		RanoConfUtil_initByStrAry_withEnvVar( ans, sda, default_idx, envvar_idx,
				0, Znk_NPOS );
		ZnkBird_extend_self( bird, ans, ZnkStr_leng(ans) );
		return true;
	}
	return false;
}

bool
EstConfig_initiate( RanoCGIEVar* evar, const char* moai_dir, size_t count )
{
	char path_target[ 256 ]  = "";
	char path_filters[ 256 ] = "";
	char path_plugins[ 256 ] = "";
	bool result = false;

	st_count = count;

	/***
	 * 1秒間に10000プロセス以上のeasterが起動していることはまずあり得ないので
	 * 最大でも4桁あれば補助識別子としては十分.
	 * 複数のeasterディレクトリがインストールされている場合、count値は各easter毎が保持する.
	 * 従ってあまり意味がないことであるが仮にそれらが同時に起動している場合、
	 * それらの値は理論上は重複し得るが、意図して狙いでもしない限り極低い確率である.
	 * ましてやそれらが同じ時刻(秒単位)に一つのeaster設定ファイルにアクセスするような状況は
	 * 実用上ありえないとみなして差し支えなかろう.
	 */
	EstUNID_setSessionCode_byNumber( count % 10000 );

	if( st_easter_myf == NULL ){
		st_easter_myf = ZnkMyf_create();
	}
	if( st_tags_myf == NULL ){
		st_tags_myf = ZnkMyf_create();
	}
	if( st_target_myf == NULL ){
		st_target_myf = ZnkMyf_create();
	}
	if( st_hosts_myf == NULL ){
		st_hosts_myf = ZnkMyf_create();
	}

	if( st_moai_dir == NULL ){
		st_moai_dir = ZnkStr_new( moai_dir );
	} else {
		ZnkStr_set( st_moai_dir, moai_dir );
	}

	/* Moai_AuthenticKey */
	EstConfig_loadAuthenticKey( st_moai_authentic_key, sizeof(st_moai_authentic_key), ZnkStr_cstr(st_moai_dir) );

	/***
	 * filters_dir and profile_dir setting.
	 * This must be relative directory from st_moai_dir
	 */
	{
		char filename[ 1024 ] = "";
		ZnkMyf config = ZnkMyf_create();
		Znk_snprintf( filename, sizeof(filename), "%sconfig.myf", ZnkStr_cstr(st_moai_dir) );

		st_profile_dir = ZnkStr_newf( "%s/moai_profile", ZnkStr_cstr(st_moai_dir) );

		if( ZnkMyf_load( config, filename ) ){
			ZnkVarpAry vars = ZnkMyf_find_vars( config, "config" );
			if( vars ){
				ZnkVarp var = NULL;

				var = ZnkVarpAry_findObj_byName_literal( vars, "acceptable_host", false );
				if( var ){
					ZnkS_copy( st_acceptable_host, sizeof(st_acceptable_host), ZnkVar_cstr(var), Znk_NPOS );
				}

				var = ZnkVarpAry_findObj_byName_literal( vars, "filters_dir", false );
				if( var ){
					ZnkS_copy( st_filters_dir, sizeof(st_filters_dir), ZnkVar_cstr(var), Znk_NPOS );
				}

				var = ZnkVarpAry_findObj_byName_literal( vars, "profile_dir", false );
				if( var ){
					const char* profile_dir = ZnkVar_cstr(var);
					bool is_relative = true;
					if( ZnkS_empty(profile_dir) ){
						profile_dir = "../moai_profile";
					} else {
						if( profile_dir[ 0 ] == '/' ){
							/* UNIX fullpath */
							is_relative = false;
						} else if( profile_dir[ 1 ] == ':' && isalpha( profile_dir[ 0 ] ) ){
							/* Windows fullpath */
							is_relative = false;
						}
					}
					if( is_relative ){
						ZnkStr_setf( st_profile_dir, "%s/%s", ZnkStr_cstr(st_moai_dir), profile_dir );
					} else {
						ZnkStr_setf( st_profile_dir, "%s", profile_dir );
					}
				}

				var = ZnkVarpAry_findObj_byName_literal( vars, "parent_proxy", false );
				ZnkS_copy( st_parent_proxy, sizeof(st_parent_proxy), ZnkVar_cstr(var), Znk_NPOS );
				if( ZnkS_eq(st_parent_proxy,":0") ){
					ZnkS_copy( st_parent_proxy, sizeof(st_parent_proxy), "NONE", Znk_NPOS );
				}

				var = ZnkVarpAry_findObj_byName_literal( vars, "moai_port", false );
				if( var ){
					const char* moai_port_str = ZnkVar_cstr( var );
					ZnkS_getU16U( &st_moai_port, moai_port_str );
				}

				var = ZnkVarpAry_findObj_byName_literal( vars, "xhr_dmz_port", false );
				if( var ){
					const char* xhr_dmz_port_str = ZnkVar_cstr( var );
					ZnkS_getU16U( &st_xhr_dmz_port, xhr_dmz_port_str );
				}
			}
		}
		ZnkMyf_destroy( config );

		{
			char* envvar_val = ZnkEnvVar_get( "MOAI_PROFILE_DIR" );
			if( envvar_val ){
				/* defined in OS */
				ZnkStr_set( st_profile_dir, envvar_val );
			}
			ZnkEnvVar_free( envvar_val );
		}
		{
			const char* profile_dir = EstConfig_profile_dir();
			ZnkDir_mkdirPath( profile_dir, Znk_NPOS, '/', NULL );
		}

	}

	/***
	 * profile family directories.
	 */
	{
		char filename[ 1024 ] = "";
		ZnkMyf config_cgi = ZnkMyf_create();
		Znk_snprintf( filename, sizeof(filename), "%sconfig_cgi.myf", ZnkStr_cstr(st_moai_dir) );
		if( ZnkMyf_load( config_cgi, filename ) ){
			ZnkVarpAry vars = ZnkMyf_find_vars( config_cgi, "fsys_map_list" );
			if( vars ){
				ZnkBird bird = ZnkBird_create( "#[", "]#" );
				ZnkBird_regist( bird, "profile_dir", ZnkStr_cstr(st_profile_dir) );

				st_favorite_dir = ZnkStr_new( "" );
				getRealPath( st_favorite_dir, vars, "favorite", 1, 2, bird );
				if( ZnkStr_empty(st_favorite_dir) ){
					ZnkStr_setf( st_favorite_dir, "%s/favorite", ZnkStr_cstr(st_profile_dir) );
				}

				st_stockbox_dir = ZnkStr_new( "" );
				getRealPath( st_stockbox_dir, vars, "stockbox", 1, 2, bird );
				if( ZnkStr_empty(st_stockbox_dir) ){
					ZnkStr_setf( st_stockbox_dir, "%s/stockbox", ZnkStr_cstr(st_profile_dir) );
				}

				st_userbox_dir = ZnkStr_new( "" );
				getRealPath( st_userbox_dir, vars, "userbox", 1, 2, bird );
				if( ZnkStr_empty(st_userbox_dir) ){
					ZnkStr_setf( st_userbox_dir, "%s/userbox", ZnkStr_cstr(st_profile_dir) );
				}

				ZnkBird_destroy( bird );
			}
		}
		ZnkMyf_destroy( config_cgi );

		if( ZnkStr_empty(st_profile_dir) ){
			st_topics_dir = ZnkStr_new( "topics" );
		} else {
			st_topics_dir = ZnkStr_newf( "%s/topics", ZnkStr_cstr(st_profile_dir) );
		}
	}

	if( ZnkDir_getType( "easter.myf" ) != ZnkDirType_e_File ){ 
		ZnkDir_copyFile_byForce( "default/easter.myf", "easter.myf", NULL );
	}
	if( ZnkDir_getType( "input_hiddens.myf" ) != ZnkDirType_e_File ){ 
		ZnkDir_copyFile_byForce( "default/input_hiddens.myf", "input_hiddens.myf", NULL );
	}

	/* load easter.myf */
	result = ZnkMyf_load( st_easter_myf, "easter.myf" );
	if( result ){
		ZnkVarpAry vars;
		ZnkVarp    varp;

		vars = ZnkMyf_find_vars( st_easter_myf, "config" );

		varp = ZnkVarpAry_findObj_byName_literal( vars, "direct_img_link", false );
		st_direct_img_link = ZnkVar_str(varp);

		varp = ZnkVarpAry_findObj_byName_literal( vars, "auto_link", false );
		st_auto_link = ZnkVar_str(varp);


		st_allow_js_hosts = ZnkMyf_find_lines( st_easter_myf, "allow_js_hosts" );

		varp = ZnkVarpAry_findObj_byName_literal( vars, "show_file_num", false );
		if( varp ){
			ZnkS_getSzU( &st_show_file_num, ZnkVar_cstr(varp) );
		}

		varp = ZnkVarpAry_findObj_byName_literal( vars, "thumb_size", false );
		if( varp ){
			ZnkS_getSzU( &st_thumb_size, ZnkVar_cstr(varp) );
		}

		varp = ZnkVarpAry_findObj_byName_literal( vars, "line_file_num", false );
		if( varp ){
			ZnkS_getSzU( &st_line_file_num, ZnkVar_cstr(varp) );
		}

		varp = ZnkVarpAry_findObj_byName_literal( vars, "line_tag_num", false );
		if( varp ){
			ZnkS_getSzU( &st_line_tag_num, ZnkVar_cstr(varp) );
		}

		varp = ZnkVarpAry_findObj_byName_literal( vars, "preview_max_width", false );
		if( varp ){
			ZnkS_getSzU( &st_preview_max_width, ZnkVar_cstr(varp) );
		}
		varp = ZnkVarpAry_findObj_byName_literal( vars, "preview_max_height", false );
		if( varp ){
			ZnkS_getSzU( &st_preview_max_height, ZnkVar_cstr(varp) );
		}

		varp = ZnkVarpAry_findObj_byName_literal( vars, "cache_days_ago", false );
		if( varp ){
			ZnkS_getSzU( &st_cache_days_ago, ZnkVar_cstr(varp) );
		}

		varp = ZnkVarpAry_findObj_byName_literal( vars, "dustbox_days_ago", false );
		if( varp ){
			ZnkS_getSzU( &st_dustbox_days_ago, ZnkVar_cstr(varp) );
		}
	}
	{
		char topic_list_myf_path[ 256 ] = "";
		const char* topics_dir = EstConfig_topics_dir();
		Znk_snprintf( topic_list_myf_path, sizeof(topic_list_myf_path), "%s/topic_list.myf", topics_dir );
		if( ZnkDir_getType( topic_list_myf_path ) != ZnkDirType_e_File ){ 
			ZnkDir_copyFile_byForce( "default/topic_list.myf", topic_list_myf_path, NULL );
		}
	}

	/* mkdir all directory in moai_profile if not exist. */
	{
		const char* favorite_dir = EstConfig_favorite_dir();
		const char* stockbox_dir = EstConfig_stockbox_dir();
		const char* userbox_dir  = EstConfig_userbox_dir();
		const char* topics_dir   = EstConfig_topics_dir();
		ZnkDir_mkdirPath( favorite_dir, Znk_NPOS, '/', NULL );
		ZnkDir_mkdirPath( stockbox_dir, Znk_NPOS, '/', NULL );
		ZnkDir_mkdirPath( userbox_dir,  Znk_NPOS, '/', NULL );
		ZnkDir_mkdirPath( topics_dir,   Znk_NPOS, '/', NULL );
	}

	/* load tags.myf */
	{
		char tags_myf_path[ 256 ] = "tags.myf";
		Znk_snprintf( tags_myf_path, sizeof(tags_myf_path), "%s/tags.myf", EstConfig_profile_dir() );
		if( ZnkDir_getType( tags_myf_path ) != ZnkDirType_e_File ){ 
			ZnkDir_copyFile_byForce( "default/tags.myf", tags_myf_path, NULL );
		}
		result = ZnkMyf_load( st_tags_myf, tags_myf_path );
	}

	/* load target.myf */
	Znk_snprintf( path_target, sizeof(path_target), "%starget.myf", ZnkStr_cstr(st_moai_dir) );
	result = ZnkMyf_load( st_target_myf, path_target );

	/* load hosts.myf */
	{
		char filename[ 1024 ] = "";
		Znk_snprintf( filename, sizeof(filename), "%shosts.myf", ZnkStr_cstr(st_moai_dir) );
		if( !ZnkMyf_load( st_hosts_myf, filename ) ){
			RanoLog_printf( "Easter : loading hosts.myf is failure.\n" );
		}
	}

	/* load hint.myf */
	EstHint_initiate();

	st_mod_ary = RanoModuleAry_create( true );
	Znk_snprintf( path_filters, sizeof(path_filters), "%s%s", moai_dir, st_filters_dir );
	Znk_snprintf( path_plugins, sizeof(path_plugins), "%splugins", moai_dir );
	RanoModuleAry_loadAllModules( st_mod_ary, st_target_myf, path_filters, path_plugins );

	if( ZnkS_empty( st_xhr_dmz ) ){
		if( ZnkS_eq( st_acceptable_host, "LOOPBACK" ) ){
			Znk_snprintf( st_xhr_dmz, sizeof(st_xhr_dmz), "127.0.0.1:%u", st_xhr_dmz_port );
		} else {
			uint32_t private_ip = 0;
			/***
			 * 最新のPrivateIPを自動取得する.
			 */
			if( ZnkNetIP_getPrivateIP32( &private_ip ) ){
				char private_ipstr[ 64 ] = "";
				ZnkNetIP_getIPStr_fromU32( private_ip, private_ipstr, sizeof(private_ipstr) );
				Znk_snprintf( st_xhr_dmz, sizeof(st_xhr_dmz), "%s:%u", private_ipstr, st_xhr_dmz_port );
			} else {
				Znk_snprintf( st_xhr_dmz, sizeof(st_xhr_dmz), "127.0.0.1:%u", st_xhr_dmz_port );
			}
		}
	}

	return result;
}

#if 0
static bool
isOldFile( const char* file_path, ZnkStr msg )
{
	ZnkDate current = { 0 };
	ZnkDate date = { 0 };
	ZnkDate threshold = { 0 };
	long diff_sec = 0;
	bool is_old = false;

	ZnkDate_getCurrent( &current );
	ZnkDate_getNDaysAgo( &threshold, &current, 1 );
	RanoFileInfo_getLastUpdatedTime( file_path, &date );

	if( ZnkDate_compareDay( &date, &threshold ) >= 0 ){
		diff_sec = ZnkDate_diffSecond( &current, &date, 1 );
		if( msg ){
			ZnkStr_addf( msg, "Est_config : diff_sec=[%ld]\n", diff_sec );
		}
		if( diff_sec >= 600 ){
			is_old = true;
		}
	} else {
		if( msg ){
			ZnkStr_addf( msg, "Est_config : compareDay is negative.\n" );
		}
		is_old = true;
	}
	return is_old;
}
#endif


void
EstConfig_finalize( void )
{
	ZnkMyf_destroy( st_easter_myf );
	ZnkMyf_destroy( st_target_myf );
	ZnkMyf_destroy( st_hosts_myf );
	RanoModuleAry_destroy( st_mod_ary );

	EstBase_removeOldFile( "tmp/pid", NULL, 0, 600 );
	EstBase_removeOldFile( "publicbox/state", NULL, 0, 3600 );
}

const char*
EstConfig_findTargetName( const char* line )
{
	const size_t  size = ZnkMyf_numOfSection( st_target_myf );
	size_t        idx;
	ZnkMyfSection sec;
	ZnkStrAry     lines;
	for( idx=0; idx<size; ++idx ){
		sec   = ZnkMyf_atSection( st_target_myf, idx );
		lines = ZnkMyfSection_lines( sec ); 
		if( ZnkStrAry_find_isMatch( lines, 0, line, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
			/* found */
			return ZnkMyfSection_name( sec );
		}
	}
	/* not found */
	return NULL;
}
RanoModule
EstConfig_findModule( const char* query_target )
{
	const size_t mod_size = RanoModuleAry_size( st_mod_ary );
	size_t mod_idx;
	RanoModule mod = NULL;
	for( mod_idx=0; mod_idx<mod_size; ++mod_idx ){
		mod = RanoModuleAry_at( st_mod_ary, mod_idx );
		if( ZnkS_eq( RanoModule_target_name( mod ), query_target ) ){
			return mod;
		}
	}
	return NULL;
}
const char*
EstConfig_getTargetAndModule( RanoModule* modp, const char* hostname )
{
	const char* target = NULL;
	/***
	 * est_target指定は廃止.
	 * セキュリティ強化のため、findTargetNameで取得できないhostnameは
	 * targetの値をNULLとしてページの内容を取得不可とすべき.
	 */
	target = EstBase_findTargetName( st_target_myf, hostname );
	if( target ){
		if( modp ){
			*modp = EstConfig_findModule( target );
		}
	}
	return target;
}

bool
EstConfig_isDirectImgLink( void )
{
	return ZnkStr_eq( st_direct_img_link, "on" );
}
bool
EstConfig_isAutoLink( void )
{
	return ZnkStr_eq( st_auto_link, "on" );
}

ZnkVarpAry
EstConfig_filterOnclickJSCall( void )
{
	return ZnkMyf_find_vars( st_easter_myf, "filter_onclick_jscall" );
}

#define IS_OK( val ) (bool)( (val) != NULL )

const char*
EstConfig_XhrDMZ( void )
{
	return st_xhr_dmz;
}
const char*
EstConfig_XhrAuthHost( void )
{
	static char st_xhr_authhost[ 256 ] = "";
	if( ZnkS_empty( st_xhr_authhost ) ){
		Znk_snprintf( st_xhr_authhost, sizeof(st_xhr_authhost), "127.0.0.1:%zu", st_moai_port );
	}
	return st_xhr_authhost;
}
bool
EstConfig_isXhrAuthHost( const RanoCGIEVar* evar )
{
	const char* server_port = evar->server_port_;
	uint16_t server_port_u16 = 0;
	ZnkS_getU16U( &server_port_u16, server_port );
	return (bool)( st_moai_port == server_port_u16 );
}

const char*
EstConfig_authenticKey( void )
{
	return st_moai_authentic_key;
}
const char*
EstConfig_moai_dir( void )
{
	return ZnkStr_cstr( st_moai_dir );
}
const char*
EstConfig_filter_dir( void )
{
	return st_filters_dir;
}
const char*
EstConfig_parent_proxy( void )
{
	return st_parent_proxy;
}
const char*
EstConfig_profile_dir( void )
{
	if( st_profile_dir ){
		return ZnkStr_cstr( st_profile_dir );
	}
	return "moai_profile";
}
const char*
EstConfig_favorite_dir( void )
{
	if( st_favorite_dir ){
		return ZnkStr_cstr( st_favorite_dir );
	}
	return "favorite";
}
const char*
EstConfig_stockbox_dir( void )
{
	if( st_stockbox_dir ){
		return ZnkStr_cstr( st_stockbox_dir );
	}
	return "stockbox";
}
const char*
EstConfig_userbox_dir( void )
{
	if( st_userbox_dir ){
		return ZnkStr_cstr( st_userbox_dir );
	}
	return "userbox";
}
const char*
EstConfig_topics_dir( void )
{
	if( st_topics_dir ){
		return ZnkStr_cstr( st_topics_dir );
	}
	return "topics";
}

ZnkStrAry
EstConfig_AllowJSHosts( void )
{
	return st_allow_js_hosts;
}
ZnkMyf
EstConfig_tags_myf( void )
{
	return st_tags_myf;
}
ZnkMyf
EstConfig_target_myf( void )
{
	return st_target_myf;
}
ZnkMyf
EstConfig_hosts_myf( void )
{
	return st_hosts_myf;
}
void
EstConfig_saveEasterMyf( void )
{
	ZnkMyf_save( st_easter_myf, "easter.myf" );
}
void
EstConfig_saveTagsMyf( void )
{
	char tags_myf_path[ 256 ] = "tags.myf";
	Znk_snprintf( tags_myf_path, sizeof(tags_myf_path), "%s/tags.myf", EstConfig_profile_dir() );
	ZnkMyf_save( st_tags_myf, tags_myf_path );
}

/***
 * SetCookieを検出した場合のみ強制的にfilters/send_<target>.myfのcookie_varsを
 * その値で更新する.
 */
void
EstConfig_readRecvHdrSetCookie( RanoModule mod, const char* target )
{
	char filename[ 1024 ]  = "";
	char result_dir[ 256 ] = "";
	ZnkFile fp = NULL;
	const char* tmpdir_pid = EstConfig_getTmpDirPID( false );

	if( mod == NULL ){
		return;
	}

	Znk_snprintf( result_dir, sizeof(result_dir), "./%s/%s", tmpdir_pid, target );
	ZnkDir_mkdirPath( result_dir, Znk_NPOS, '/', NULL );

	Znk_snprintf( filename, sizeof(filename), "%s/recv_hdr.dat", result_dir );
	fp = Znk_fopen( filename, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		ZnkVarpAry vars = ZnkVarpAry_create( true );
		ZnkMyf ftr_send;

		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				break;
			}
			ZnkStr_chompNL( line );
			ZnkHtpHdrs_regist_byLine( vars, ZnkStr_cstr(line), ZnkStr_leng(line) );
		}
		Znk_fclose( fp );
		ZnkStr_delete( line );

		ftr_send = RanoModule_ftrSend( mod );
		RanoFltr_updateCookieFilter_bySetCookie( vars, ftr_send );
		/***
		 * 次回起動時にも状態を引き継ぐため、saveが必要.
		 * TODO : Lock/Unlockが必要.
		 */
		{
			char        filename[ 256 ];
			const char* target_name = RanoModule_target_name( mod );
			Znk_snprintf( filename, sizeof(filename), "%s%s/%s_send.myf", ZnkStr_cstr(st_moai_dir), st_filters_dir, target_name );
			if( !ZnkMyf_save( ftr_send, filename ) ){
				RanoLog_printf( "  Response : Cannot save %s\n", filename );
			}
		}

		ZnkVarpAry_destroy( vars );
	}
}
bool
EstConfig_isInputHiddensPostVarNames( const char* target, const char* query_varname )
{
	ZnkStrAry lines = NULL;

	if( st_input_hiddens_myf == NULL ){
		st_input_hiddens_myf = ZnkMyf_create();
		if( !ZnkMyf_load( st_input_hiddens_myf, "input_hiddens.myf" ) ){
			RanoLog_printf( "Easter : loading input_hiddens.myf is failure.\n" );
			ZnkMyf_destroy( st_input_hiddens_myf );
			st_input_hiddens_myf = NULL;
			return false;
		}
	}

	lines = ZnkMyf_find_lines( st_input_hiddens_myf, target );
	if( lines ){
		const char* val = NULL;
		size_t idx;
		size_t size = ZnkStrAry_size( lines );
		for( idx=0; idx<size; ++idx ){
			val = ZnkStrAry_at_cstr( lines, idx );
			if( ZnkS_eq( val, query_varname ) ){
				/* found */
				return true;
			}
		}
	}
	/* not found */
	return false;
}

static bool
updateSizeVar( ZnkBird bird, ZnkVarpAry post_vars, ZnkVarpAry em_vars,
		const char* var_key, size_t* dst_var, size_t min_val, size_t max_val,
		const char* var_ui_name, ZnkStr ermsg )
{
	bool result = false;
	ZnkVarp varp = NULL;
	if( IS_OK( varp = ZnkVarpAry_findObj_byName( post_vars, var_key, Znk_NPOS, false ) ) ){
		size_t tmp = Znk_NPOS;
		if( ZnkS_getSzU( &tmp, ZnkVar_cstr(varp) ) ){
			if( tmp >= min_val && tmp <= max_val ){
				ZnkVarp em_varp = ZnkVarpAry_findObj_byName( em_vars, var_key, Znk_NPOS, false );
				*dst_var = tmp;
				ZnkStr_copy( ZnkVar_str(em_varp), ZnkVar_str(varp) );
				ZnkBird_regist( bird, var_key, ZnkVar_cstr(varp) );
				result = true;
			}
		}
	}
	if( !result ){
		ZnkStr_addf( ermsg, "無効な値の指定があります : %s <br>この値は %zu 以上 %zu 以下でなければなりません.<br>",
				var_ui_name, min_val, max_val );
	}
	return result;
}

void
EstConfigManager_main( RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr msg, const char* authentic_key )
{
	const char* template_html_file = "templates/config_manager.html";
	struct ZnkHtpHdrs_tag htp_hdrs = { 0 };
	ZnkBird    bird = ZnkBird_create( "#[", "]#" );
	ZnkVarp    cmd = NULL;
	RanoModule mod = NULL;
	ZnkStr     pst_str = ZnkStr_new( "" );
	ZnkStr     ermsg   = ZnkStr_new( "" );
	bool       is_authenticated = false;
	bool       is_unescape_val = false;
	ZnkVarp    varp;

	RanoCGIUtil_getPostedFormData( evar, post_vars, mod, &htp_hdrs, pst_str, NULL, is_unescape_val );

	if( IS_OK( varp = ZnkVarpAry_findObj_byName_literal( post_vars, "Moai_AuthenticKey", false ) )
	  && ZnkS_eq( authentic_key, ZnkVar_cstr(varp) ) ){
		is_authenticated = true;
	} else {
	}
	ZnkStr_addf( msg, "is_authenticated=[%d]\n", is_authenticated );

	if( EstConfig_isDirectImgLink() ){
		ZnkBird_regist( bird, "direct_img_link", "checked" );
	} else {
		ZnkBird_regist( bird, "direct_img_link", "" );
	}
	if( EstConfig_isAutoLink() ){
		ZnkBird_regist( bird, "auto_link", "checked" );
	} else {
		ZnkBird_regist( bird, "auto_link", "" );
	}

	{
		char str[ 256 ] = "";
		Znk_snprintf( str, sizeof(str), "%zu", st_show_file_num );
		ZnkBird_regist( bird, "show_file_num", str );

		Znk_snprintf( str, sizeof(str), "%zu", st_thumb_size );
		ZnkBird_regist( bird, "thumb_size", str );

		Znk_snprintf( str, sizeof(str), "%zu", st_line_file_num );
		ZnkBird_regist( bird, "line_file_num", str );

		Znk_snprintf( str, sizeof(str), "%zu", st_line_tag_num );
		ZnkBird_regist( bird, "line_tag_num", str );

		Znk_snprintf( str, sizeof(str), "%zu", st_preview_max_width );
		ZnkBird_regist( bird, "preview_max_width", str );

		Znk_snprintf( str, sizeof(str), "%zu", st_preview_max_height );
		ZnkBird_regist( bird, "preview_max_height", str );

		Znk_snprintf( str, sizeof(str), "%zu", st_cache_days_ago );
		ZnkBird_regist( bird, "cache_days_ago", str );

		Znk_snprintf( str, sizeof(str), "%zu", st_dustbox_days_ago );
		ZnkBird_regist( bird, "dustbox_days_ago", str );
	}

	if( IS_OK( cmd = ZnkVarpAry_findObj_byName_literal( post_vars, "command", false ) )){
		if( ZnkS_eq( ZnkVar_cstr(cmd), "update" ) ){
			ZnkVarpAry em_vars = ZnkMyf_find_vars( st_easter_myf, "config" );

			/* direct_img_link */
			if( IS_OK( varp = ZnkVarpAry_findObj_byName_literal( post_vars, "est_direct_img_link", false ) )){
				ZnkStr_addf( msg, "est_direct_img_link=on\n" );
				ZnkStr_set( st_direct_img_link, "on" );
				ZnkBird_regist( bird, "direct_img_link", "checked" );
			} else {
				ZnkStr_addf( msg, "est_direct_img_link=off\n" );
				ZnkStr_set( st_direct_img_link, "off" );
				ZnkBird_regist( bird, "direct_img_link", "" );
			}

			/* auto_link */
			if( IS_OK( varp = ZnkVarpAry_findObj_byName_literal( post_vars, "est_auto_link", false ) )){
				ZnkStr_addf( msg, "est_auto_link=on\n" );
				ZnkStr_set( st_auto_link, "on" );
				ZnkBird_regist( bird, "auto_link", "checked" );
			} else {
				ZnkStr_addf( msg, "est_auto_link=off\n" );
				ZnkStr_set( st_auto_link, "off" );
				ZnkBird_regist( bird, "auto_link", "" );
			}

			/* show_file_num */
			updateSizeVar( bird, post_vars, em_vars,
					"show_file_num", &st_show_file_num, 1, 200,
					"1ページあたりのサムネ数", ermsg );
			ZnkStr_addf( msg, "show_file_num=[%zu]\n", st_show_file_num );

			/* thumb_size */
			updateSizeVar( bird, post_vars, em_vars,
					"thumb_size", &st_thumb_size, 32, 1024,
					"サムネサイズ", ermsg );
			ZnkStr_addf( msg, "thumb_size=[%zu]\n", st_thumb_size );

			/* line_file_num */
			updateSizeVar( bird, post_vars, em_vars,
					"line_file_num", &st_line_file_num, 1, 30,
					"サムネ列数", ermsg );
			ZnkStr_addf( msg, "line_file_num=[%zu]\n", st_line_file_num );

			/* line_tag_num */
			updateSizeVar( bird, post_vars, em_vars,
					"line_tag_num", &st_line_tag_num, 1, 30,
					"タグ一覧の列数", ermsg );
			ZnkStr_addf( msg, "line_tag_num=[%zu]\n", st_line_tag_num );

			/* preview_max_width */
			updateSizeVar( bird, post_vars, em_vars,
					"preview_max_width", &st_preview_max_width, 30, 10000,
					"プレビューの最大幅(pixel)", ermsg );
			ZnkStr_addf( msg, "preview_max_width=[%zu]\n", st_preview_max_width );

			/* preview_max_height */
			updateSizeVar( bird, post_vars, em_vars,
					"preview_max_height", &st_preview_max_height, 30, 10000,
					"プレビューの最大高(pixel)", ermsg );
			ZnkStr_addf( msg, "preview_max_height=[%zu]\n", st_preview_max_height );

			/* cache_days_ago */
			updateSizeVar( bird, post_vars, em_vars,
					"cache_days_ago", &st_cache_days_ago, 1, 30,
					"キャッシュの保存日数", ermsg );
			ZnkStr_addf( msg, "cache_days_ago=[%zu]\n", st_cache_days_ago );

			/* dustbox_days_ago */
			updateSizeVar( bird, post_vars, em_vars,
					"dustbox_days_ago", &st_dustbox_days_ago, 1, 30,
					"ダストボックスの保存日数", ermsg );
			ZnkStr_addf( msg, "dustbox_days_ago=[%zu]\n", st_dustbox_days_ago );

			ZnkMyf_save( st_easter_myf, "easter.myf" );
		} else {
			ZnkStr_addf( msg, "unknown command=[%s].", ZnkVar_cstr(cmd) );
		}
	} else {
		ZnkStr_addf( msg, "command is not found." );
	}

	ZnkBird_regist( bird, "Moai_AuthenticKey", authentic_key );
	ZnkBird_regist( bird, "ermsg", ZnkStr_cstr(ermsg) );
	ZnkHtpURL_negateHtmlTagEffection( msg ); /* for XSS */
	{
		ZnkSRef old_ptn = { 0 };
		ZnkSRef new_ptn = { 0 };
		ZnkSRef_set_literal( &old_ptn, "\n" );
		ZnkSRef_set_literal( &new_ptn, "<br>\n" );
		ZnkStrEx_replace_BF( msg, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}
	{
		ZnkStr hint_table = EstHint_getHintTable( "config" );
		if( hint_table ){
			ZnkBird_regist( bird, "hint_table", ZnkStr_cstr(hint_table) );
		} else {
			ZnkBird_regist( bird, "hint_table", "" );
		}
	}
	ZnkBird_regist( bird, "msg",   ZnkStr_cstr(msg) );
	RanoCGIUtil_printTemplateHTML( evar, bird, template_html_file );
	ZnkBird_destroy( bird );
	ZnkStr_delete( pst_str );
	ZnkStr_delete( ermsg );
}
