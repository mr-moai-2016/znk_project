/***
 * 翻訳単位が空の場合警告を発するコンパイラが存在する(VCなど)
 * これはそのための対策.
 */
typedef int dummy_for_warning__translation_unit_is_empty__;

/***
 * dll作成の際、DllMainの定義が必須なコンパイラが存在する(DMCなど)
 * これはそのための対策.
 * ただし、既に他の部分でDllMainを定義している場合はこれをスキップすることもできる.
 * その場合はDLLMAIN_DEFINEDを用いること.
 */
#if !defined(DLLMAIN_DEFINED) && defined(__DMC__)

#  define WIN32_LEAN_AND_MEAN
#  include<windows.h>

#  ifdef __cplusplus
extern "C" {
#  endif

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
	switch(fdwReason){
	case DLL_PROCESS_ATTACH: break;
	case DLL_PROCESS_DETACH: break;
	case DLL_THREAD_ATTACH:  break;
	case DLL_THREAD_DETACH:  break;
	}
	return  TRUE;
}

#  ifdef __cplusplus
}
#  endif

#endif
