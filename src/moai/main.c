#include "Moai_server.h"

#include <Znk_zlib.h>
#include <Znk_stdc.h>
#include <Znk_bfr.h>
#include <Znk_s_base.h>
#include <Znk_htp_util.h>
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

int main(int argc, char **argv)
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
	if( ras_result != MoaiRASResult_e_CriticalError ){
		result = EXIT_SUCCESS;
	}

FUNC_END:
#if defined(Znk_TARGET_WINDOWS)
	if( hmtx ){
		CloseHandle( hmtx );
	}
#endif

	return result;
}
