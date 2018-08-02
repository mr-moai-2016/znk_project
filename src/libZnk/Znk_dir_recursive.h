#ifndef INCLUDE_GUARD__Znk_dir_recursive_h__
#define INCLUDE_GUARD__Znk_dir_recursive_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkDirRecursiveImpl* ZnkDirRecursive;
typedef bool (*ZnkDirRecursiveFuncT)( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num );

typedef struct ZnkDirRecursiveFuncArg_tag {
	ZnkDirRecursiveFuncT func_;
	void*                arg_;
} ZnkDirRecursiveFuncArg;

typedef enum {
	 ZnkDirRecursiveFnca_e_onEnterDir=0
	,ZnkDirRecursiveFnca_e_processFile
	,ZnkDirRecursiveFnca_e_onExitDir
	,ZnkDirRecursiveFnca_e_isIgnoreDir
} ZnkDirRecursiveFncaType;

ZnkDirRecursive
ZnkDirRecursive_create( bool is_err_ignore,
		const ZnkDirRecursiveFuncT func_onEnterDir,  void* arg_onEnterDir,
		const ZnkDirRecursiveFuncT func_processFile, void* arg_processFile,
		const ZnkDirRecursiveFuncT func_onExitDir,   void* arg_onExitDir );
void
ZnkDirRecursive_destroy( ZnkDirRecursive recur );

ZnkDirRecursiveFuncArg*
ZnkDirRecursive_configFnca( ZnkDirRecursive recur, ZnkDirRecursiveFncaType fnca_type );

Znk_INLINE void
ZnkDirRecursive_config_onEnterDir( ZnkDirRecursive recur,
		const ZnkDirRecursiveFuncT func, void* arg )
{
	ZnkDirRecursiveFuncArg* fnca = ZnkDirRecursive_configFnca( recur, ZnkDirRecursiveFnca_e_onEnterDir );
	Znk_FUNCARG_INIT( *fnca, func, arg );
}
Znk_INLINE void
ZnkDirRecursive_config_processFile( ZnkDirRecursive recur,
		const ZnkDirRecursiveFuncT func, void* arg )
{
	ZnkDirRecursiveFuncArg* fnca = ZnkDirRecursive_configFnca( recur, ZnkDirRecursiveFnca_e_processFile );
	Znk_FUNCARG_INIT( *fnca, func, arg );
}
Znk_INLINE void
ZnkDirRecursive_config_onExitDir( ZnkDirRecursive recur,
		const ZnkDirRecursiveFuncT func, void* arg )
{
	ZnkDirRecursiveFuncArg* fnca = ZnkDirRecursive_configFnca( recur, ZnkDirRecursiveFnca_e_onExitDir );
	Znk_FUNCARG_INIT( *fnca, func, arg );
}
Znk_INLINE void
ZnkDirRecursive_config_isIgnoreDir( ZnkDirRecursive recur,
		const ZnkDirRecursiveFuncT func, void* arg )
{
	ZnkDirRecursiveFuncArg* fnca = ZnkDirRecursive_configFnca( recur, ZnkDirRecursiveFnca_e_isIgnoreDir );
	Znk_FUNCARG_INIT( *fnca, func, arg );
}

void
ZnkDirRecursive_setErrorIgnore( ZnkDirRecursive recur, bool is_err_ignore );

bool
ZnkDirRecursive_traverse( ZnkDirRecursive recur, const char* top_dir, ZnkStr ermsg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
