#ifndef INCLUDE_GUARD__Mkf_install_h__
#define INCLUDE_GUARD__Mkf_install_h__

#include <Znk_str.h>
#include <Znk_myf.h>

Znk_EXTERN_C_BEGIN

typedef struct MkfInstall_tag* MkfInstall;
struct MkfInstall_tag {
	ZnkStr install_dir_;
	ZnkStr install_exec_dir_;
	ZnkStr install_dlib_dir_;
	ZnkStr install_slib_dir_;
	ZnkStr install_data_dir_;
	ZnkStrAry install_exec_list_;
	ZnkStrAry install_dlib_list_;
	ZnkStrAry install_slib_list_;
	ZnkStrAry install_data_list_;
};

void
MkfInstall_compose( MkfInstall inst, const ZnkMyf conf_myf );

void
MkfInstall_dispose( MkfInstall inst );

void
MkfInstall_clearList_atMkfGenerate( MkfInstall inst );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
