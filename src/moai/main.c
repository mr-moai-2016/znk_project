/***
 * Moai-Engine Ver2.1
 * -- CipherAgent --
 */
#include "Moai_server.h"

#include <Rano_vtag_util.h>

#include <Znk_zlib.h>
#include <Znk_stdc.h>
#include <Znk_bfr.h>
#include <Znk_s_base.h>
#include <Znk_htp_util.h>
#include <Znk_process.h>
#include <Znk_dir.h>

#include <stdio.h>
#include <stdlib.h>

#if defined(Znk_TARGET_WINDOWS)
#  include <windows.h>
#endif

Znk_INLINE void
dumpFile_toBase64( const char* in_filename, const char* out_filename )
{
	ZnkFile fp = Znk_fopen( in_filename, "rb" );
	if( fp ){
		ZnkBfr bfr = ZnkBfr_create_null();
		ZnkStr str = ZnkStr_new( "" );
		uint8_t buf[ 4096 ];
		size_t read_size = 0;
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

		fp = Znk_fopen( out_filename, "wb" );
		if( fp ){
			Znk_fwrite( (uint8_t*)ZnkStr_cstr(str), ZnkStr_leng(str), fp );
			Znk_fclose( fp );
		}
		ZnkStr_delete( str );
	}
}

int main( int argc, char** argv )
{
	int           result              = EXIT_FAILURE;
	MoaiRASResult ras_result          = MoaiRASResult_e_Ignored;
	bool          first_initiate      = false;
	bool          enable_parent_proxy = true;

	/***
	 * TODO:
	 * 異なるポート番号にてmoaiを多重起動させるという用途は許容すべきかと思われる.
	 * 従ってこれはカレントにあるconfig.myfにあるmoaiポートに接続して、存在を確認し
	 * 弾く方が多分望ましい.
	 */
#if defined(Znk_TARGET_WINDOWS)
	HANDLE hmtx = CreateMutex( NULL, FALSE, "Global\\Moai_MultiRunGuardian" );
	if( hmtx == NULL ){
		Znk_printf_e("Moai : Cannot create MultiRunGuardian. Press any key...\n" );
		getchar();
	} else if( GetLastError() == ERROR_ALREADY_EXISTS ){
		/* Already running */
		Znk_printf_e("Moai : Already running. Press any key...\n" );
		getchar();
		goto FUNC_END;
	}
#endif

	if( !ZnkZlib_initiate() ){
		Znk_printf_e( "ZnkZlib_initiate : failure.\n" );
		getchar();
		result = EXIT_FAILURE;
		goto FUNC_END;
	}

	if( argc > 1 ){
		const char* option = argv[ 1 ];
		if( ZnkS_eq( option, "--disable_init" ) ){
			first_initiate = false;
		} else if( ZnkS_eq( option, "--initiate" ) ){
			first_initiate = true;
		} else if( ZnkS_eq( option, "--disable_parent_proxy" ) ){
			enable_parent_proxy = false;
		}
	}

	//dumpFile_toBase64( "index.png", "flrv_canv_b64.txt" );

	while( true ){
		ras_result = MoaiServer_main( first_initiate, enable_parent_proxy );
		if( ras_result != MoaiRASResult_e_RestartServer ){
			break;
		}
	}

	/***
	 * 代替プロセス birdman に制御を移す.
	 * birdman では主にMoai本体のアップグレードに伴うバージョンのチェック、
	 * zipファイルのダウンロードと解凍、ファイルのコピーを行い、
	 * それが完了したらMoaiプロセスを復活させる.
	 * (このとき moai 本体をも書き換える状況に対応するため、moaiプロセスを一旦終了する必要がある)
	 */
	if( ras_result == MoaiRASResult_e_RestartProcess ){
		ZnkStr curdir_save = ZnkStr_new( "" );
		ZnkDir_getCurrentDir( curdir_save );
		ZnkDir_changeCurrentDir( "birdman" );
		{
			ZnkStr birdman_path = ZnkStr_new( "" );
			ZnkStr ermsg = ZnkStr_new( "" );
			if( RanoVTagUtil_getPatchDir( birdman_path, "moai", true, "birdman", "tmp/" ) ){
				/* この辺りの不統一性は美しくないがとりあえず */
				ZnkStr_add( birdman_path, "/birdman/birdman.exe" );
				if( ZnkDir_getType( ZnkStr_cstr(birdman_path) ) == ZnkDirType_e_File ){
					if( ZnkDir_copyFile_byForce( ZnkStr_cstr(birdman_path), "birdman.exe", ermsg ) ){
					}
				}
				ZnkStr_add( birdman_path, "/birdman/birdman" );
				if( ZnkDir_getType( ZnkStr_cstr(birdman_path) ) == ZnkDirType_e_File ){
					if( ZnkDir_copyFile_byForce( ZnkStr_cstr(birdman_path), "birdman", ermsg ) ){
						system( "chmod 755 birdman" );
					}
				}
			}
			ZnkStr_delete( birdman_path );
			ZnkStr_delete( ermsg );
		}
		ZnkDir_changeCurrentDir( ZnkStr_cstr(curdir_save) );
		ZnkStr_delete( curdir_save );

		/* authentic_key.datの内容の継続を要求する */
		{
			ZnkFile fp = Znk_fopen( "__authentic_key_reuse__", "wb" );
			if( fp ){
				Znk_fputs( "__authentic_key_reuse__", fp );
				Znk_fflush( fp );
				Znk_fclose( fp );
			}
		}
		goto FUNC_END;
	}

	if( ras_result != MoaiRASResult_e_CriticalError ){
		result = EXIT_SUCCESS;
	}

FUNC_END:
#if defined(Znk_TARGET_WINDOWS)
	if( hmtx ){
		CloseHandle( hmtx );
	}
#endif

	/* このプロセスそのものを入れ替える(子プロセスを起動するのではない) */
	if( ras_result == MoaiRASResult_e_RestartProcess ){
		const char* birdman[] = { "birdman", "upgrade_apply" };
		ZnkDir_changeCurrentDir( "birdman" );
		ZnkProcess_execRestart( 2, birdman, ZnkProcessConsole_e_Detached );
	}

	return result;
}
