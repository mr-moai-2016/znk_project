#include "Est_search_manager.h"
#include "Est_config.h"
#include "Est_base.h"
#include "Est_sqi.h"
#include "Est_recentory.h"
#include "Est_box.h"
#include <Rano_log.h>
#include <Rano_cgi_util.h>
#include <Znk_dir.h>
#include <stdio.h>

static char st_moai_authentic_key[ 256 ] = "";


int main(int argc, char **argv)
{
	RanoCGIEVar* evar = RanoCGIEVar_create();
	EstSQI sqy = NULL;
	ZnkVarpAry sqy_vars = ZnkVarpAry_create( true );
	ZnkStr msg = ZnkStr_new( "" );
	ZnkStr EstSM_result = ZnkStr_new( "" );
	const char* authentic_key = "";
	ZnkStr moai_dir = NULL;
	size_t count = 0;

	{
		static const bool keep_open = true;
		bool additional = false;
		ZnkDir_mkdirPath( "./tmp", Znk_NPOS, '/', NULL );
		RanoLog_open( "./tmp/log_task.log", keep_open, additional );
	}

	/* 認証 */
	if( argc > 1 ){
		authentic_key = argv[ 1 ];
	}
	moai_dir = EstConfig_getMoaiDir();
	if( moai_dir == NULL ){
		goto FUNC_END;
	}
	EstConfig_loadAuthenticKey( st_moai_authentic_key, sizeof(st_moai_authentic_key), ZnkStr_cstr(moai_dir) );
	if( !ZnkS_eq( authentic_key, st_moai_authentic_key ) ){
		RanoLog_printf( "Not authenticated.\n" );
		goto FUNC_END;
	}
	RanoLog_printf( "OK. authenticated.\n" );
	EstConfig_initiate( evar, ZnkStr_cstr(moai_dir), count );

	{
		const bool is_marge_tags = false;
		EstRecentory recent = EstRecentory_create();
		ZnkFile fp = Znk_fopen( "__recentory_tasking__.lock", "wb" );
		Znk_fclose( fp );
		EstRecentory_load( recent, "recentory.myf" );
		EstRecentory_removeOld( recent );
		EstRecentory_save( recent, "recentory.myf", is_marge_tags );
		ZnkDir_deleteFile_byForce( "__recentory_tasking__.lock" );
		EstRecentory_destroy( recent );
	}

	/* days_ago より前のファイルをcacheboxからdustboxへ移動する */
	{
		const size_t days_ago = EstConfig_getCacheDaysAgo();
		const size_t sec_ago  = 0;
		EstBase_moveOldDir( "cachebox", "dustbox", msg, days_ago, sec_ago );
	}
	/* days_ago より前のファイルをdustboxから完全削除する. */
	{
		const size_t days_ago = EstConfig_getDustboxDaysAgo();
		const size_t sec_ago  = 0;
		EstBase_removeOldFile( "dustbox", msg, days_ago, sec_ago );
	}
	/* days_ago より前のファイルをtmp/searchedから完全削除する. */
	{
		const char* topics_dir = EstConfig_topics_dir();
		const size_t days_ago = 14;
		const size_t sec_ago  = 0;
		char searched_dir[ 256 ] = "";
		Znk_snprintf( searched_dir, sizeof(searched_dir), "%s/tmp/searched", topics_dir );
		EstBase_removeOldFile( searched_dir, msg, days_ago, sec_ago );
	}

#if 0
	/* 完全スキャンをsearched_favorite.myfへ保存 */
	RanoLog_printf( "favorite fsys_dir=[%s].\n", EstConfig_favorite_dir() );
	{	
		static const bool force_fsys_scan = true;
		searched_key = "favorite";
		EstSQIVars_makeVars( sqy_vars,
				"favorite", "",
				"", "",
				"",
				"", "",
				"", "" );
		sqy = EstSQI_create( sqy_vars );
		EstSearchManager_searchInBox( sqy, msg, searched_key, force_fsys_scan );
	}
#endif

	{
		const char* favorite_dir = EstConfig_favorite_dir();
		EstBox_makeFInfList( favorite_dir, "favorite" );
	}
	{
		const char* stockbox_dir = EstConfig_stockbox_dir();
		EstBox_makeFInfList( stockbox_dir, "stockbox" );
	}

FUNC_END:

	EstSQI_destroy( sqy );
	ZnkVarpAry_destroy( sqy_vars );
	ZnkStr_delete( EstSM_result );
	RanoCGIEVar_destroy( evar );
	ZnkStr_delete( moai_dir );
	EstConfig_finalize();

	RanoLog_close();
	return 0;
}
