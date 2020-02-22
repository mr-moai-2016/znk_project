#include "Est_search_manager.h"
#include "Est_config.h"
#include "Est_base.h"
#include "Est_sqi.h"
#include "Est_recentory.h"
#include "Est_box.h"

#include <Rano_log.h>
#include <Rano_cgi_util.h>
#include <Rano_conf_util.h>
#include <Rano_htp_boy.h>

#include <Znk_dir.h>
#include <stdio.h>

static char st_moai_authentic_key[ 256 ] = "";

static void
removeDirModestly( const char* path, ZnkStr ermsg, size_t days_ago, size_t sec_ago )
{
	if( ZnkDir_getType( path ) == ZnkDirType_e_Directory ){
		/***
		 * ディレクトリ内の古いファイルから消していく.
		 */
		EstBase_removeOldFile( path, ermsg, days_ago, sec_ago );
		/***
		 * ディレクトリ内が空ならディレクトリそのものも消す
		 */
		ZnkDir_rmdir( path );
	}
}

int main(int argc, char **argv)
{
	RanoCGIEVar* evar = RanoCGIEVar_create();
	EstSQI sqy = NULL;
	ZnkVarpAry sqy_vars = ZnkVarpAry_create( true );
	ZnkStr ermsg = ZnkStr_new( "" );
	ZnkStr EstSM_result = ZnkStr_new( "" );
	const char* authentic_key = "";
	const char* moai_dir = NULL;
	size_t count = 0;

	if( RanoConfUtil_rano_app_initiate( ".", false, ermsg ) ){
		static const bool keep_open = true;
		static const bool additional = false;
		const char* tmpdir_common = RanoHtpBoy_getTmpDirCommon();
		char logfile_path[ 256 ] = "";
		Znk_snprintf( logfile_path, sizeof(logfile_path), "%s/easter_maintainer.log", tmpdir_common );
		RanoLog_open( logfile_path, keep_open, additional );
	} else {
		/* log自体を生成できないため、とりあえず無報告で終わる */
		goto FUNC_END;
	}

	/* 認証 */
	if( argc > 1 ){
		authentic_key = argv[ 1 ];
	}
	moai_dir = RanoConfUtil_moai_dir( NULL );
	if( moai_dir == NULL ){
		RanoLog_printf( "moai_dir is not found.\n" );
		goto FUNC_END;
	}
	EstConfig_loadAuthenticKey( st_moai_authentic_key, sizeof(st_moai_authentic_key), moai_dir );
	if( !ZnkS_eq( authentic_key, st_moai_authentic_key ) ){
		RanoLog_printf( "Not authenticated.\n" );
		goto FUNC_END;
	}
	RanoLog_printf( "OK. authenticated.\n" );
	EstConfig_initiate( evar, moai_dir, count );

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
		EstBase_moveOldDir( "cachebox", "dustbox", ermsg, days_ago, sec_ago );
	}
	/* days_ago より前のファイルをdustboxから完全削除する. */
	{
		const size_t days_ago = EstConfig_getDustboxDaysAgo();
		const size_t sec_ago  = 0;
		EstBase_removeOldFile( "dustbox", ermsg, days_ago, sec_ago );
	}
	/* days_ago より前のファイルをtmp/searchedから完全削除する. */
	{
		const char* topics_dir = EstConfig_topics_dir();
		const size_t days_ago = 14;
		const size_t sec_ago  = 0;
		char searched_dir[ 256 ] = "";
		Znk_snprintf( searched_dir, sizeof(searched_dir), "%s/tmp/searched", topics_dir );
		EstBase_removeOldFile( searched_dir, ermsg, days_ago, sec_ago );
	}
	/***
	 * days_ago より前のファイルをtmp/pidから完全削除する.
	 * tmp/pid内は放っておくと数万というファイル数になり得るので古いものは定期的な削除が必要.
	 * days_agoの値はかなり小さめでもよい(Easterプロセスが稼動する時間は長くないため).
	 */
	{
		const size_t days_ago = 2;
		const size_t sec_ago  = 0;
		EstBase_removeOldFile( "tmp/pid", ermsg, days_ago, sec_ago );
	}
	/***
	 * ログファイルの保持数を減少させるため、RanoCGIUtil_rano_app_init_log の仕様を変更する.
	 * 旧仕様では log_0 から log_9 までを保持していたが、新仕様ではこれを log_0 から log_4 までとする.
	 * これに伴い、旧仕様までにあったlog_5からlog_9が残存している可能性があるのでこれを削除する.
	 */
	{
		const size_t days_ago = 3; /* 3日の猶予期間で段階的に削除、新仕様へ移行させる. */
		const size_t sec_ago  = 0;
		removeDirModestly( "tmp/log_5", ermsg, days_ago, sec_ago );
		removeDirModestly( "tmp/log_6", ermsg, days_ago, sec_ago );
		removeDirModestly( "tmp/log_7", ermsg, days_ago, sec_ago );
		removeDirModestly( "tmp/log_8", ermsg, days_ago, sec_ago );
		removeDirModestly( "tmp/log_9", ermsg, days_ago, sec_ago );
	}
	/***
	 * easter_maintainer.cgi は以前は cache_task.cgi という名前であったが、
	 * その当時に使われていたファイルがまだ残っている場合は削除しておく.
	 */
	{
		ZnkDir_deleteFile_byForce( "tmp/cache_task.log" );
		ZnkDir_deleteFile_byForce( "tmp/cache_task_lasttime.dat" );
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
		EstSearchManager_searchInBox( sqy, ermsg, searched_key, force_fsys_scan );
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
	EstConfig_finalize();

	RanoLog_close();
	RanoConfUtil_moai_dir_finalize();
	RanoConfUtil_rano_app_finalize();
	ZnkStr_delete( ermsg );
	return 0;
}
