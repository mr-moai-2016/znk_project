#include "Mkf_install.h"

void
MkfInstall_compose( MkfInstall inst, const ZnkMyf conf_myf )
{
	ZnkVarp varp;
	const ZnkVarpAry vars = ZnkMyf_find_vars( conf_myf, "config" );

	/* base */
	varp = ZnkVarpAry_find_byName_literal( vars, "install_dir", false );
	if( varp ){
		inst->install_dir_ = ZnkStr_new( ZnkVar_cstr( varp ) );
	} else {
		inst->install_dir_ = ZnkStr_new( "out_put/test_install" );
	}

	/* exec, dlib, slib, data */
	varp = ZnkVarpAry_find_byName_literal( vars, "install_exec_dir", false );
	if( varp ){
		inst->install_exec_dir_ = ZnkStr_new( ZnkVar_cstr( varp ) );
	} else {
		inst->install_exec_dir_ = ZnkStr_new( ZnkStr_cstr(inst->install_dir_) );
	}
	varp = ZnkVarpAry_find_byName_literal( vars, "install_dlib_dir", false );
	if( varp ){
		inst->install_dlib_dir_ = ZnkStr_new( ZnkVar_cstr( varp ) );
	} else {
		inst->install_dlib_dir_ = ZnkStr_new( ZnkStr_cstr(inst->install_dir_) );
	}
	varp = ZnkVarpAry_find_byName_literal( vars, "install_slib_dir", false );
	if( varp ){
		inst->install_slib_dir_ = ZnkStr_new( ZnkVar_cstr( varp ) );
	} else {
		inst->install_slib_dir_ = ZnkStr_new( ZnkStr_cstr(inst->install_dir_) );
	}
	varp = ZnkVarpAry_find_byName_literal( vars, "install_data_dir", false );
	if( varp ){
		inst->install_data_dir_ = ZnkStr_new( ZnkVar_cstr( varp ) );
	} else {
		inst->install_data_dir_ = ZnkStr_new( ZnkStr_cstr(inst->install_dir_) );
	}

	inst->install_data_list_ = ZnkStrAry_create( true );
	ZnkStrAry_copy( inst->install_data_list_, ZnkMyf_find_lines( conf_myf, "install_data_list" ) );

	/* 以下はmkf_id毎に異なる値が設定されるため、それぞれのMakefile毎にclearしてから渡す必要がある */
	inst->install_exec_list_ = ZnkStrAry_create( true );
	inst->install_dlib_list_ = ZnkStrAry_create( true );
	inst->install_slib_list_ = ZnkStrAry_create( true );
}

void
MkfInstall_dispose( MkfInstall inst )
{
	ZnkStr_delete( inst->install_dir_      );
	ZnkStr_delete( inst->install_exec_dir_ );
	ZnkStr_delete( inst->install_dlib_dir_ );
	ZnkStr_delete( inst->install_slib_dir_ );
	ZnkStr_delete( inst->install_data_dir_ );
	ZnkStrAry_destroy( inst->install_exec_list_ );
	ZnkStrAry_destroy( inst->install_dlib_list_ );
	ZnkStrAry_destroy( inst->install_slib_list_ );
	ZnkStrAry_destroy( inst->install_data_list_ );
}

void
MkfInstall_clearList_atMkfGenerate( MkfInstall inst )
{
	ZnkStrAry_clear( inst->install_exec_list_ );
	ZnkStrAry_clear( inst->install_dlib_list_ );
	ZnkStrAry_clear( inst->install_slib_list_ );
}

