#ifndef INCLUDE_GUARD__Mkf_android_h__
#define INCLUDE_GUARD__Mkf_android_h__

#include <Mkf_seek.h>
#include <Znk_myf.h>
#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

bool
MkfAndroid_generate( const ZnkMyf conf_myf, ZnkVarpAry product_list, MkfSeekFuncT_isInterestExt isSrcFileExt,
		const ZnkStrAry include_paths_common,
		const ZnkStrAry dependency_libs_common, const ZnkStrAry runtime_additional,
		const ZnkStrAry sublibs_list );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
