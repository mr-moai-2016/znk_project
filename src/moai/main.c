#include "Moai_server.h"
#include "Moai_parent_proxy.h"
#include <Znk_zlib.h>
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(Znk_TARGET_WINDOWS)
#  include <windows.h>
#endif

int main(int argc, char **argv)
{
	int result = EXIT_FAILURE;
	MoaiRASResult ras_result = MoaiRASResult_e_Ignored;
	bool first_initiate = false;
	bool enable_parent_proxy = true;

	/***
	 * TODO:
	 * 異なるポート番号にてmoaiを多重起動させるという用途は許容すべきかと思われる.
	 * 従ってこれはカレントにあるconfig.myfにあるmoaiポートに接続して、存在を確認し
	 * 弾く方が多分望ましい.
	 */
#if defined(Znk_TARGET_WINDOWS)
	HANDLE hmtx = CreateMutex( NULL, FALSE, "Global\\Moai_MultiRunGuardian" );
	if( hmtx == NULL ){
		ZnkF_printf_e("Moai : Cannot create MultiRunGuardian. Press any key...\n" );
		getchar();
	} else if( GetLastError() == ERROR_ALREADY_EXISTS ){
		/* Already running */
		ZnkF_printf_e("Moai : Already running. Press any key...\n" );
		getchar();
		goto FUNC_END;
	}
#endif

	if( !ZnkZlib_initiate() ){
		ZnkF_printf_e( "ZnkZlib_initiate : failure.\n" );
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
