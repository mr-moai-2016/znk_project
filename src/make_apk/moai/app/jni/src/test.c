#include "test.h"

#include <Rano_log.h>
#include <Rano_cgi_util.h>
#include <Rano_conf_util.h>

#include <Znk_dir.h>
#include <Znk_stdc.h>
#include <Znk_thread.h>
#include <Znk_zlib.h>
#include <Znk_s_base.h>

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <Moai_server.h>
#include <Moai_server_info.h>

#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <sys/stat.h>


static bool
chmodDir( const char* path, ZnkStr cmd, ZnkStr msg, const char* ext, int mode )
{
#if defined(Znk_TARGET_UNIX)
	int err_count = 0;
	const char* name;
	ZnkDirId id = ZnkDir_openDir( path );
	if( id == NULL ){
		return false;
	}
	while( true ){
		name = ZnkDir_readDir( id );
		if( name == NULL ){
			break;
		}
		if( ZnkS_eq( ext, "*" ) || ZnkS_eqCase( ZnkS_get_extension( name, '.' ), ext ) ){
			ZnkStr_setf( cmd, "%s/%s", path, name );
			if( chmod( ZnkStr_cstr(cmd) , mode ) < 0 ){
				if( msg ){
					ZnkStr_addf( msg, "Error : %s\n", ZnkStr_cstr(cmd) );
				}
				err_count += 1;
			}
		}
	}
	ZnkDir_closeDir( id );
	return (bool)( err_count == 0 );
#else
	return true;
#endif
}

static void
makeLibSymbolicLink( const char* private_path_cstr, const char* filename, const char* here_dir, ZnkStr cmd, ZnkStr msg )
{
	ZnkStr_setf( cmd, "rm %s/%s", here_dir, filename );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

	ZnkStr_setf( cmd, "ln -s %s/../lib/%s %s/%s",
			private_path_cstr, filename,
			here_dir, filename );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );
}
static void
makeBinSymbolicLink( const char* private_path_cstr, const char* filename, const char* here_dir, const char* abi_str, ZnkStr cmd, ZnkStr msg )
{
	ZnkStr_setf( cmd, "rm %s/%s", here_dir, filename );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

	ZnkStr_setf( cmd, "ln -s %s/bin/%s/%s %s/%s",
			private_path_cstr, abi_str, filename,
			here_dir, filename );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );
}


JNIEXPORT jboolean JNICALL
Znk_JNI_INTERFACE( init )( JNIEnv* env, jobject obj, jstring private_path, jstring abi_jstr )
{
	jboolean result = JNI_TRUE;
	ZnkStr  cmd = ZnkStr_new( "" );
	ZnkStr  msg = ZnkStr_new( "" );
	ZnkStr path = ZnkStr_new( "" );
	const char* private_path_cstr = (*env)->GetStringUTFChars( env, private_path, NULL );
	const char* private_ext_path_cstr = "/sdcard/Android/data/znkproject.moai/files";
	const char* abi_cstr = (*env)->GetStringUTFChars( env, abi_jstr, NULL );

	MoaiServerInfo_setCurrentDirRequest( private_path_cstr );
	ZnkDir_changeCurrentDir( private_path_cstr );

	{
		ZnkStr_setf( path, "%s/moai/tmp", private_ext_path_cstr );
		ZnkDir_mkdirPath( ZnkStr_cstr(path), Znk_NPOS, '/', NULL );

		ZnkStr_setf( path, "%s/moai/tmp", private_ext_path_cstr );
		chmodDir( ZnkStr_cstr(path), cmd, msg, "*", 0775 );

		RanoLog_close();
		ZnkStr_setf( path, "%s/moai/tmp/moai_jni.log", private_ext_path_cstr );
		RanoLog_open( ZnkStr_cstr(path), false, false );
		RanoLog_printf( "MoaiJni : init.\n" );

		ZnkStr_setf( path, "%s/cgis/cgi_developers/protected", private_path_cstr );
		chmodDir( ZnkStr_cstr(path), cmd, msg, "*", 0775 );

		ZnkStr_setf( path, "%s/plugins", private_path_cstr );
		chmodDir( ZnkStr_cstr(path), cmd, msg, "so", 0775 );

		ZnkStr_setf( path, "%s/bin/%s", private_path_cstr, abi_cstr );
		chmodDir( ZnkStr_cstr(path), cmd, msg, "cgi", 0775 );

		ZnkStr_setf( path, "%s/bin/%s/cgi_developers/protected", private_path_cstr, abi_cstr );
		chmodDir( ZnkStr_cstr(path), cmd, msg, "cgi", 0775 );
	}

	RanoLog_printf( "MoaiJni : setup basic directoris done.\n" );

	{
		ZnkStr libname = ZnkStr_new( "" );
		//const char* version_str = "2.2";
		const char* version_str = MoaiServerInfo_version( true );

		/***
		 * 古いバージョンのシンボリックリンクが残っている可能性があるため
		 * 確実に消しておく.
		 */
		ZnkStr_setf( cmd, "rm %s/*.so", private_path_cstr );
		system( ZnkStr_cstr(cmd) );
		ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

		ZnkStr_setf( libname, "libZnk-%s.so", version_str );
		makeLibSymbolicLink( private_path_cstr, ZnkStr_cstr(libname),  private_path_cstr, cmd, msg );

		ZnkStr_setf( libname, "libRano-%s.so", version_str );
		makeLibSymbolicLink( private_path_cstr, ZnkStr_cstr(libname),  private_path_cstr, cmd, msg );

		ZnkStr_setf( libname, "libtls-17.so" );
		makeLibSymbolicLink( private_path_cstr, ZnkStr_cstr(libname),  private_path_cstr, cmd, msg );

		/* easter */
		ZnkStr_setf( path, "%s/cgis/easter", private_path_cstr );

		ZnkStr_setf( libname, "cache_task.cgi" );
		makeBinSymbolicLink( private_path_cstr, ZnkStr_cstr(libname),  ZnkStr_cstr(path), abi_cstr, cmd, msg );

		ZnkStr_setf( libname, "easter.cgi" );
		makeBinSymbolicLink( private_path_cstr, ZnkStr_cstr(libname),  ZnkStr_cstr(path), abi_cstr, cmd, msg );

		/* custom_boy */
		ZnkStr_setf( path, "%s/cgis/custom_boy", private_path_cstr );

		ZnkStr_setf( libname, "custom_boy.cgi" );
		makeBinSymbolicLink( private_path_cstr, ZnkStr_cstr(libname),  ZnkStr_cstr(path), abi_cstr, cmd, msg );

		/* cgi_developers */
		ZnkStr_setf( path, "%s/cgis", private_path_cstr );

		ZnkStr_setf( libname, "cgi_developers/protected/hello.cgi" );
		makeBinSymbolicLink( private_path_cstr, ZnkStr_cstr(libname),  ZnkStr_cstr(path), abi_cstr, cmd, msg );

		ZnkStr_delete( libname );
	}

	RanoLog_printf( "MoaiJni : ln libs done.\n" );

	ZnkStr_setf( cmd, "ls -al %s/cgis/easter > /sdcard/moai_profile/ls_easter.log", private_path_cstr );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

	ZnkStr_setf( cmd, "ls -al %s/cgis/custom_boy > /sdcard/moai_profile/ls_custom_boy.log", private_path_cstr );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

	ZnkStr_setf( cmd, "ls -al %s/cgis/cgi_developers/protected > /sdcard/moai_profile/ls_cgi_developers.log", private_path_cstr );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

	ZnkStr_setf( cmd, "ls -alR %s/bin > /sdcard/moai_profile/ls_bin.log", private_path_cstr );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

#if 0
	ZnkStr_setf( cmd, "ls -al %s/cgis/easter > /sdcard/moai_profile/ls_easter.log", private_path_cstr );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );


	ZnkStr_setf( cmd, "ls -al /storage/emulated/legacy > /sdcard/moai_profile/ls_sd.log" );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

	ZnkStr_setf( cmd, "ls -al /sdcard/moai_profile > /sdcard/moai_profile/ls_sd_profile.log" );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

	ZnkStr_setf( cmd, "ls -al /sdcard/Download > /sdcard/moai_profile/ls_download.log" );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

	ZnkStr_setf( cmd, "ls -al /sdcard/Download/moai_profile > /sdcard/moai_profile/ls_download_profile.log" );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

	ZnkStr_setf( cmd, "ls -al /sdcard/Android/data/mrmoai2016.helloworld > /sdcard/moai_profile/ls_mr.log" );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

	ZnkStr_setf( cmd, "ls -al /sdcard/Android/data/mrmoai2016.helloworld/files > /sdcard/moai_profile/ls_mr_files.log" );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );
#endif

	RanoLog_printf( "MoaiJni : init done.\n" );

	ZnkStr_delete( path );
	ZnkStr_delete( cmd );
	ZnkStr_delete( msg );
	return result;
}

JNIEXPORT jboolean JNICALL
Znk_JNI_INTERFACE( copyFromAssets )( JNIEnv* env, jclass clazz, jobject assetManager,
		jstring src_filename, jstring dst_dir, jstring dst_filename )
{
	jboolean result = JNI_TRUE;
	AAssetManager* mgr   = NULL;
	AAsset*        asset = NULL;
	const char*    src_utf8  = NULL;
	const char*    dst_utf8  = NULL;
	const char*    dst_dir_utf8  = NULL;
	ZnkStr         dst_path = ZnkStr_new( "" );

	// convert Java string to UTF-8
	src_utf8 = (*env)->GetStringUTFChars( env, src_filename, NULL );
	assert( src_utf8 );

	dst_dir_utf8 = (*env)->GetStringUTFChars( env, dst_dir, NULL );
	assert( dst_dir_utf8 );
	ZnkDir_mkdirPath( dst_dir_utf8, Znk_NPOS, '/', NULL );

	dst_utf8 = (*env)->GetStringUTFChars( env, dst_filename, NULL );
	assert( dst_utf8 );

	ZnkStr_setf( dst_path, "%s/%s", dst_dir_utf8, dst_utf8 );

	// use asset manager to open asset by src_filename
	mgr = AAssetManager_fromJava( env, assetManager );
	assert( mgr );
	asset = AAssetManager_open( mgr, src_utf8, AASSET_MODE_UNKNOWN );

	// release the Java string and UTF-8
	(*env)->ReleaseStringUTFChars( env, src_filename, src_utf8 );
	(*env)->ReleaseStringUTFChars( env, dst_dir,      dst_dir_utf8 );
	(*env)->ReleaseStringUTFChars( env, dst_filename, dst_utf8 );

	// the asset might not be found
	if( asset == NULL ){
		result = JNI_FALSE;
		goto FUNC_END;
	}

	{
		ZnkFile fp;
		uint8_t buf[ 4096 ] = { 0 };
		fp = Znk_fopen( ZnkStr_cstr(dst_path), "wb" );
		if( fp ){
			while( true ){
				int readed_size = AAsset_read( asset, buf, sizeof( buf ) );
				if( readed_size == 0 ){
					/* EOF */
					break;
				} else if( readed_size < 0 ){
					/* Error */
					result = JNI_FALSE;
					break;
				}
				Znk_fwrite( buf, readed_size, fp );
			}
			Znk_fclose( fp );
		}
	}

FUNC_END:
	ZnkStr_delete( dst_path );
	return result;
}

static void*
funcThread( void* arg )
{
	MoaiRASResult ras_result          = MoaiRASResult_e_Ignored;
	bool          first_initiate      = false;
	bool          enable_parent_proxy = true;

	RanoLog_printf( "Moai : thread loop start.\n" );
	while( true ){
		ras_result = MoaiServer_main( first_initiate, enable_parent_proxy );
		if( ras_result != MoaiRASResult_e_RestartServer ){
			RanoLog_printf( "Moai : thread loop break.\n" );
			break;
		}
	}
	return NULL;
}

JNIEXPORT jstring JNICALL
Znk_JNI_INTERFACE( mainLoop )( JNIEnv* env, jobject obj )
{
	jstring ans;
	ZnkStr ermsg = ZnkStr_new( "" );
	RanoLog_printf( "MoaiJni : mainLoop begin.\n" );
	size_t count = 0;

	if( RanoConfUtil_rano_app_initiate( ".", false, ermsg ) ){
		static const bool is_multi_dir = false;
		RanoLog_close();
		count = RanoCGIUtil_rano_app_init_log( "moai", is_multi_dir );
	} else {
		RanoLog_printf( "MoaiJni : [%s]\n", ZnkStr_cstr(ermsg) );
		goto FUNC_END;
	}

	if( !ZnkZlib_initiate() ){
		RanoLog_printf( "Moai : ZnkZlib_initiate : failure\n" );
		goto FUNC_END;
	}

	Znk_UNUSED( count );

	//__android_log_print(ANDROID_LOG_DEBUG,"Tag","mainLoop.");
	ZnkThread_create( funcThread, NULL );

FUNC_END:
	ans = (*env)->NewStringUTF( env, ZnkStr_cstr(ermsg) );
	ZnkStr_delete(ermsg);
	return ans;
}
