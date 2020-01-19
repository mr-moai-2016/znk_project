#include "test.h"

#include <Rano_log.h>
#include <Rano_cgi_util.h>
#include <Rano_conf_util.h>

#include <Znk_dir.h>
#include <Znk_stdc.h>
#include <Znk_thread.h>
#include <Znk_zlib.h>
#include <Znk_s_base.h>
#include <Znk_str_ex.h>

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
		ZnkStr_addf( msg, "Error : ZnkDir_openDir( path=[%s] ) failure.\n", path );
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

static bool
makeFileSymbolicLink( const char* real_path, const char* filename, const char* here_dir, ZnkStr cmd, ZnkStr msg )
{
	bool   result    = true;

	ZnkStr_setf( cmd, "rm %s/%s", here_dir, filename );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

	if( ZnkDir_getType( real_path ) != ZnkDirType_e_File ){
		result = false;
		ZnkStr_addf( msg, "Error : Cannot execute ln command. real_path=[%s] does not exist.\n", real_path );
		goto FUNC_END;
	}

	/* real_path exist */
	ZnkStr_setf( cmd, "ln -s %s %s/%s",
			real_path,
			here_dir, filename );
	system( ZnkStr_cstr(cmd) );
	ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

FUNC_END:
	return result;
}
static bool
makeFileSymbolicLink_forLib(
		const char* private_path_cstr, const char* filename, const char* here_dir, ZnkStr cmd, ZnkStr msg )
{
	bool   result    = true;
	ZnkStr real_path = ZnkStr_newf( "%s/../lib/%s", private_path_cstr, filename );

	result = makeFileSymbolicLink( ZnkStr_cstr(real_path), filename, here_dir, cmd, msg );

	ZnkStr_delete( real_path );
	return result;
}
static bool
makeFileSymbolicLink_forBin(
		const char* private_path_cstr, const char* filename, const char* here_dir, const char* abi_str, ZnkStr cmd, ZnkStr msg )
{
	bool   result    = true;
	ZnkStr real_path = ZnkStr_newf( "%s/bin/%s/%s", private_path_cstr, abi_str, filename );

	result = makeFileSymbolicLink( ZnkStr_cstr(real_path), filename, here_dir, cmd, msg );

	ZnkStr_delete( real_path );
	return result;
}
static bool
makeFileSymbolicLink_forPlugin(
		const char* private_path_cstr, const char* filename, const char* here_dir, const char* abi_str, ZnkStr cmd, ZnkStr msg )
{
	bool   result    = true;
	ZnkStr real_path = ZnkStr_newf( "%s/bin/%s/plugins/%s", private_path_cstr, abi_str, filename );

	result = makeFileSymbolicLink( ZnkStr_cstr(real_path), filename, here_dir, cmd, msg );

	ZnkStr_delete( real_path );
	return result;
}

static bool
convertFullVer_toParentVer( ZnkStr parent_ver, const char* full_ver )
{
	bool      result = false;
	ZnkStrAry tkns = ZnkStrAry_create( true );
	size_t    tkns_size = 0;

	ZnkStrEx_addSplitC( tkns, full_ver, Znk_NPOS, '.', false, 3 );
	tkns_size = ZnkStrAry_size( tkns );
	if( tkns_size >= 2 ){
		ZnkStr_setf( parent_ver, "%s.%s", ZnkStrAry_at_cstr(tkns,0), ZnkStrAry_at_cstr(tkns,1) );
		result = true;
	} else if( tkns_size == 1 ){
		ZnkStr_setf( parent_ver, "%s", ZnkStrAry_at_cstr(tkns,0) );
		result = true;
	} else {
		result = false;
	}
	ZnkStrAry_destroy( tkns );
	return result;
}

JNIEXPORT jboolean JNICALL
Znk_JNI_INTERFACE( init )( JNIEnv* env, jobject obj, jstring private_path, jstring abi_jstr )
{
	jboolean result    = JNI_TRUE;
	ZnkStr  cmd        = ZnkStr_new( "" );
	ZnkStr  msg        = ZnkStr_new( "" );
	ZnkStr  path       = ZnkStr_new( "" );
	ZnkStr  libname    = ZnkStr_new( "" );
	ZnkStr  parent_ver = ZnkStr_new( "" );
	const char* private_path_cstr = (*env)->GetStringUTFChars( env, private_path, NULL );
	const char* private_ext_path_cstr = "/sdcard/Android/data/znkproject.moai/files";
	const char* abi_cstr = (*env)->GetStringUTFChars( env, abi_jstr, NULL );
	bool result_of_section = false;

	MoaiServerInfo_setCurrentDirRequest( private_path_cstr );
	ZnkDir_changeCurrentDir( private_path_cstr );

	do {
		ZnkStr_setf( path, "%s/moai/tmp", private_ext_path_cstr );
		ZnkDir_mkdirPath( ZnkStr_cstr(path), Znk_NPOS, '/', NULL );

		/***
		 * assets/plugins 内が空である場合、java内でのcopyFromAssets が実行されない.
		 * しかし、plugins内のsoファイルは後でシンボリックリンクを張る必要があると考えられる.
		 * そのため、ここで確実にmkdirしておく.
		 */
		ZnkStr_setf( path, "%s/plugins", private_path_cstr );
		ZnkDir_mkdirPath( ZnkStr_cstr(path), Znk_NPOS, '/', NULL );

		ZnkStr_setf( path, "%s/moai/tmp", private_ext_path_cstr );
		chmodDir( ZnkStr_cstr(path), cmd, msg, "*", 0775 );

		RanoLog_close();
		ZnkStr_setf( path, "%s/moai/tmp/moai_jni.log", private_ext_path_cstr );
		RanoLog_open( ZnkStr_cstr(path), false, false );
		RanoLog_printf( "MoaiJni : init.\n" );

		ZnkStr_setf( path, "%s/cgis/cgi_developers/protected", private_path_cstr );
		if( !chmodDir( ZnkStr_cstr(path), cmd, msg, "*", 0775 ) ){
			break;
		}

		ZnkStr_setf( path, "%s/plugins", private_path_cstr );
		if( !chmodDir( ZnkStr_cstr(path), cmd, msg, "so", 0775 ) ){
			break;
		}

		ZnkStr_setf( path, "%s/bin/%s", private_path_cstr, abi_cstr );
		if( !chmodDir( ZnkStr_cstr(path), cmd, msg, "cgi", 0775 ) ){
			break;
		}

		ZnkStr_setf( path, "%s/bin/%s/cgi_developers/protected", private_path_cstr, abi_cstr );
		if( !chmodDir( ZnkStr_cstr(path), cmd, msg, "cgi", 0775 ) ){
			break;
		}

		result_of_section = true;
	} while( false );
	if( result_of_section ){
		RanoLog_printf( "MoaiJni : setup basic directoris done.\n" );
	} else {
		RanoLog_printf( "MoaiJni : setup basic directoris failure.\n" );
		RanoLog_printf( "msg :\n" );
		RanoLog_printf( "%s\n", ZnkStr_cstr(msg) );
		result = JNI_FALSE;
	}


	result_of_section = false;
	do {
		const char* here_dir = NULL;
		const char* full_ver = MoaiServerInfo_version( true );
		convertFullVer_toParentVer( parent_ver, full_ver );


		/* libs */
		here_dir = private_path_cstr;

		/* 古いバージョンのシンボリックリンクが残っている可能性があるため確実に削除. */
		ZnkStr_setf( cmd, "rm %s/*.so", private_path_cstr );
		system( ZnkStr_cstr(cmd) );
		ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

		ZnkStr_setf( libname, "libZnk-%s.so", ZnkStr_cstr(parent_ver) );
		if( !makeFileSymbolicLink_forLib( private_path_cstr, ZnkStr_cstr(libname),  private_path_cstr, cmd, msg ) ){
			break;
		}

		ZnkStr_setf( libname, "libRano-%s.so", ZnkStr_cstr(parent_ver) );
		if( !makeFileSymbolicLink_forLib( private_path_cstr, ZnkStr_cstr(libname),  private_path_cstr, cmd, msg ) ){
			break;
		}

		ZnkStr_setf( libname, "libtls-17.so" );
		if( !makeFileSymbolicLink_forLib( private_path_cstr, ZnkStr_cstr(libname),  private_path_cstr, cmd, msg ) ){
			break;
		}


		/* easter */
		ZnkStr_setf( path, "%s/cgis/easter", private_path_cstr );
		here_dir = ZnkStr_cstr(path);

		ZnkStr_setf( libname, "cache_task.cgi" );
		if( !makeFileSymbolicLink_forBin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}

		ZnkStr_setf( libname, "easter.cgi" );
		if( !makeFileSymbolicLink_forBin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}


		/* custom_boy */
		ZnkStr_setf( path, "%s/cgis/custom_boy", private_path_cstr );
		here_dir = ZnkStr_cstr(path);

		ZnkStr_setf( libname, "custom_boy.cgi" );
		if( !makeFileSymbolicLink_forBin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}


		/* proxy_finder */
		ZnkStr_setf( path, "%s/cgis/proxy_finder", private_path_cstr );
		here_dir = ZnkStr_cstr(path);

		ZnkStr_setf( libname, "proxy_finder.cgi" );
		if( !makeFileSymbolicLink_forBin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}


		/* cgi_developers */
		ZnkStr_setf( path, "%s/cgis", private_path_cstr );
		here_dir = ZnkStr_cstr(path);

		ZnkStr_setf( libname, "cgi_developers/protected/evar1.cgi" );
		if( !makeFileSymbolicLink_forBin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}
		ZnkStr_setf( libname, "cgi_developers/protected/evar2.cgi" );
		if( !makeFileSymbolicLink_forBin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}
		ZnkStr_setf( libname, "cgi_developers/protected/hello.cgi" );
		if( !makeFileSymbolicLink_forBin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}
		ZnkStr_setf( libname, "cgi_developers/protected/hello_template.cgi" );
		if( !makeFileSymbolicLink_forBin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}
		ZnkStr_setf( libname, "cgi_developers/protected/post1.cgi" );
		if( !makeFileSymbolicLink_forBin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}
		ZnkStr_setf( libname, "cgi_developers/protected/post2.cgi" );
		if( !makeFileSymbolicLink_forBin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}
		ZnkStr_setf( libname, "cgi_developers/protected/progress.cgi" );
		if( !makeFileSymbolicLink_forBin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}
		ZnkStr_setf( libname, "cgi_developers/protected/query_string1.cgi" );
		if( !makeFileSymbolicLink_forBin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}
		ZnkStr_setf( libname, "cgi_developers/protected/query_string2.cgi" );
		if( !makeFileSymbolicLink_forBin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}
		ZnkStr_setf( libname, "cgi_developers/protected/transfer_chunked.cgi" );
		if( !makeFileSymbolicLink_forBin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}


		/* plugins */
		ZnkStr_setf( path, "%s/plugins", private_path_cstr );
		here_dir = ZnkStr_cstr(path);

		/* 古いバージョンのシンボリックリンクが残っている可能性があるため確実に削除. */
		ZnkStr_setf( cmd, "rm %s/*.so", here_dir );
		system( ZnkStr_cstr(cmd) );
		ZnkStr_addf( msg, "%s\n", ZnkStr_cstr(cmd) );

		ZnkStr_setf( libname, "futaba.so" );
		if( !makeFileSymbolicLink_forPlugin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}

		ZnkStr_setf( libname, "5ch.so" );
		if( !makeFileSymbolicLink_forPlugin( private_path_cstr, ZnkStr_cstr(libname),  here_dir, abi_cstr, cmd, msg ) ){
			break;
		}

		result_of_section = true;
	} while( false );
	if( result_of_section ){
		RanoLog_printf( "MoaiJni : ln libs done.\n" );
	} else {
		RanoLog_printf( "MoaiJni : ln libs failure.\n" );
		RanoLog_printf( "msg :\n" );
		RanoLog_printf( "%s\n", ZnkStr_cstr(msg) );
		result = JNI_FALSE;
	}

	result_of_section = false;
	do {
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
		result_of_section = true;
	} while( false );
	if( result_of_section ){
		RanoLog_printf( "MoaiJni : ls done.\n" );
	} else {
		RanoLog_printf( "MoaiJni : ls failure.\n" );
		RanoLog_printf( "msg :\n" );
		RanoLog_printf( "%s\n", ZnkStr_cstr(msg) );
		result = JNI_FALSE;
	}

	if( result == JNI_TRUE ){
		RanoLog_printf( "MoaiJni : init success.\n" );
	} else {
		RanoLog_printf( "MoaiJni : init failure.\n" );
	}

	ZnkStr_delete( path );
	ZnkStr_delete( cmd );
	ZnkStr_delete( msg );
	ZnkStr_delete( libname );
	ZnkStr_delete( parent_ver );
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
