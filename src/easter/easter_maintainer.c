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
		 * �f�B���N�g�����̌Â��t�@�C����������Ă���.
		 */
		EstBase_removeOldFile( path, ermsg, days_ago, sec_ago );
		/***
		 * �f�B���N�g��������Ȃ�f�B���N�g�����̂��̂�����
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
		/* log���̂𐶐��ł��Ȃ����߁A�Ƃ肠�������񍐂ŏI��� */
		goto FUNC_END;
	}

	/* �F�� */
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

	/* days_ago ���O�̃t�@�C����cachebox����dustbox�ֈړ����� */
	{
		const size_t days_ago = EstConfig_getCacheDaysAgo();
		const size_t sec_ago  = 0;
		EstBase_moveOldDir( "cachebox", "dustbox", ermsg, days_ago, sec_ago );
	}
	/* days_ago ���O�̃t�@�C����dustbox���犮�S�폜����. */
	{
		const size_t days_ago = EstConfig_getDustboxDaysAgo();
		const size_t sec_ago  = 0;
		EstBase_removeOldFile( "dustbox", ermsg, days_ago, sec_ago );
	}
	/* days_ago ���O�̃t�@�C����tmp/searched���犮�S�폜����. */
	{
		const char* topics_dir = EstConfig_topics_dir();
		const size_t days_ago = 14;
		const size_t sec_ago  = 0;
		char searched_dir[ 256 ] = "";
		Znk_snprintf( searched_dir, sizeof(searched_dir), "%s/tmp/searched", topics_dir );
		EstBase_removeOldFile( searched_dir, ermsg, days_ago, sec_ago );
	}
	/***
	 * days_ago ���O�̃t�@�C����tmp/pid���犮�S�폜����.
	 * tmp/pid���͕����Ă����Ɛ����Ƃ����t�@�C�����ɂȂ蓾��̂ŌÂ����̂͒���I�ȍ폜���K�v.
	 * days_ago�̒l�͂��Ȃ菬���߂ł��悢(Easter�v���Z�X���ғ����鎞�Ԃ͒����Ȃ�����).
	 */
	{
		const size_t days_ago = 2;
		const size_t sec_ago  = 0;
		EstBase_removeOldFile( "tmp/pid", ermsg, days_ago, sec_ago );
	}
	/***
	 * ���O�t�@�C���̕ێ��������������邽�߁ARanoCGIUtil_rano_app_init_log �̎d�l��ύX����.
	 * ���d�l�ł� log_0 ���� log_9 �܂ł�ێ����Ă������A�V�d�l�ł͂���� log_0 ���� log_4 �܂łƂ���.
	 * ����ɔ����A���d�l�܂łɂ�����log_5����log_9���c�����Ă���\��������̂ł�����폜����.
	 */
	{
		const size_t days_ago = 3; /* 3���̗P�\���ԂŒi�K�I�ɍ폜�A�V�d�l�ֈڍs������. */
		const size_t sec_ago  = 0;
		removeDirModestly( "tmp/log_5", ermsg, days_ago, sec_ago );
		removeDirModestly( "tmp/log_6", ermsg, days_ago, sec_ago );
		removeDirModestly( "tmp/log_7", ermsg, days_ago, sec_ago );
		removeDirModestly( "tmp/log_8", ermsg, days_ago, sec_ago );
		removeDirModestly( "tmp/log_9", ermsg, days_ago, sec_ago );
	}
	/***
	 * easter_maintainer.cgi �͈ȑO�� cache_task.cgi �Ƃ������O�ł��������A
	 * ���̓����Ɏg���Ă����t�@�C�����܂��c���Ă���ꍇ�͍폜���Ă���.
	 */
	{
		ZnkDir_deleteFile_byForce( "tmp/cache_task.log" );
		ZnkDir_deleteFile_byForce( "tmp/cache_task_lasttime.dat" );
	}

#if 0
	/* ���S�X�L������searched_favorite.myf�֕ۑ� */
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
