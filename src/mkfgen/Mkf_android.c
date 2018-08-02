#include "Mkf_android.h"
#include "Mkf_seek.h"
#include "Mkf_lib_depend.h"
#include "Mkf_include.h"
#include <Znk_stdc.h>
#include <Znk_dir.h>

static const char*
getProductType( ZnkVarp varp )
{
	return ZnkStrAry_at_cstr( varp->prim_.u_.sda_, 0 );
}
static const char*
getProductName( ZnkVarp varp )
{
	return ZnkStrAry_at_cstr( varp->prim_.u_.sda_, 1 );
}
static const char*
getProductMainSrc( ZnkVarp varp )
{
	return ZnkStrAry_at_cstr( varp->prim_.u_.sda_, 2 );
}

static bool
isExistMainSrc( ZnkVarpAry product_list, const char* mainsrc )
{
	const size_t size = ZnkVarpAry_size( product_list );
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		const ZnkVarp varp = ZnkVarpAry_at( product_list, idx );
		if( ZnkS_eq( getProductMainSrc( varp ), mainsrc ) ){
			/* found */
			return true;
		}
	}
	/* not found */
	return false;
}

bool
MkfAndroid_generate( const ZnkMyf conf_myf, ZnkVarpAry product_list, MkfSeekFuncT_isInterestExt isSrcFileExt,
		const ZnkStrAry include_paths_common,
		const ZnkStrAry dependency_libs_common, const ZnkStrAry runtime_additional,
		const ZnkStrAry sublibs_list )
{
	ZnkFile fp = NULL;
	size_t product_idx;
	size_t lib_idx;
	const size_t product_list_size = ZnkVarpAry_size( product_list );
	const size_t deplib_size = ZnkStrAry_size( dependency_libs_common );
	const size_t sublib_size = ZnkStrAry_size( sublibs_list );
	bool is_at_notation = false;

	/***
	 * TODO: 
	 * もう mkf_android_install.sh のようなシェルスクリプトを別途出力させるべき.
	 * またruntime_additionalについてもそのスクリプト内にのみ反映させるべき.
	 */

	ZnkDir_mkdirPath( "mkf_android/jni", Znk_NPOS, '/', NULL );

	fp = Znk_fopen( "mkf_android/jni/Android.mk", "wb" );

	if( fp == NULL ){
		return false;
	} else {
		ZnkStr dep_lib_type = ZnkStr_new( "" );
		ZnkStr lib_name  = ZnkStr_new( "" );
		ZnkStr lib_dlver = ZnkStr_new( "" );
		ZnkStr lib_dir   = ZnkStr_new( "" );
		ZnkStr submkf_dir  = ZnkStr_new( "" );
		ZnkStr sublib_name = ZnkStr_new( "" );
		ZnkStr include_flags = ZnkStr_new( "" );
		ZnkStr template_include_path = ZnkStr_new( "-I$(MY_LIBS_ROOT)/$[lib_dir]$ \\" );
	
		Znk_fprintf( fp, "# It should be ndk root directory(which contains jni directory.)\n" );
		Znk_fprintf( fp, "LOCAL_PATH := $(call my-dir)/..\n" );
		Znk_fprintf( fp, "S:=..\n" );
		Znk_fprintf( fp, "\n" );

		Znk_fprintf( fp, "ifndef MY_LIBS_ROOT\n" );
		Znk_fprintf( fp, "  MY_LIBS_ROOT := ../..\n" );
		Znk_fprintf( fp, "endif\n" );
		Znk_fprintf( fp, "\n" );
	
		for( lib_idx=0; lib_idx<deplib_size; ++lib_idx ){
			const char* deplib = ZnkStrAry_at_cstr( dependency_libs_common, lib_idx );
			MkfLibDepend_parseDependencyLib( deplib, dep_lib_type, lib_name, lib_dlver, lib_dir, &is_at_notation );
			if( ZnkStr_eq( dep_lib_type, "slib" ) ){
				Znk_fprintf( fp, "# Declaring a prebuilt external-static-library module\n" );
				Znk_fprintf( fp, "include $(CLEAR_VARS)\n" );
				Znk_fprintf( fp, "LOCAL_MODULE := %s\n", ZnkStr_cstr(lib_dir) );
				Znk_fprintf( fp, "LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/%s/mkf_android/libs/$(TARGET_ARCH_ABI)/%s.a\n",
						ZnkStr_cstr(lib_dir), ZnkStr_cstr(lib_dir) );
				Znk_fprintf( fp, "include $(PREBUILT_STATIC_LIBRARY)\n" );
				Znk_fprintf( fp, "\n" );
			} else {
				Znk_fprintf( fp, "# Declaring a prebuilt external-dynamic-library module\n" );
				Znk_fprintf( fp, "include $(CLEAR_VARS)\n" );
				Znk_fprintf( fp, "LOCAL_MODULE := %s\n", ZnkStr_cstr(lib_dir) );
				Znk_fprintf( fp, "LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/%s/mkf_android/libs/$(TARGET_ARCH_ABI)/%s.so\n",
						ZnkStr_cstr(lib_dir), ZnkStr_cstr(lib_dir) );
				Znk_fprintf( fp, "include $(PREBUILT_SHARED_LIBRARY)\n" );
				Znk_fprintf( fp, "\n" );
			}
		}
	
		for( lib_idx=0; lib_idx<sublib_size; ++lib_idx ){
			const char* sublib = ZnkStrAry_at_cstr( sublibs_list, lib_idx );
			MkfLibDepend_parseSubLib( sublib, submkf_dir, sublib_name );

			Znk_fprintf( fp, "# Declaring a prebuilt sub-static-library module\n" );
			Znk_fprintf( fp, "include $(CLEAR_VARS)\n" );
			Znk_fprintf( fp, "LOCAL_MODULE := %s\n", ZnkStr_cstr(sublib_name) );
			Znk_fprintf( fp, "LOCAL_SRC_FILES := $S/%s/mkf_android/obj/local/$(TARGET_ARCH_ABI)/lib%s.a\n",
					ZnkStr_cstr(submkf_dir), ZnkStr_cstr(sublib_name) );
			Znk_fprintf( fp, "include $(PREBUILT_STATIC_LIBRARY)\n" );
			Znk_fprintf( fp, "\n" );
		}
	
		{
			size_t inc_idx;
			size_t inc_size = ZnkStrAry_size( include_paths_common );
			for( inc_idx=0; inc_idx<inc_size; ++inc_idx ){
				const char* include_path = ZnkStrAry_at_cstr( include_paths_common, inc_idx );
				MkfInclude_getIncludeFlag( include_flags, include_path, template_include_path, "\n" );
			}
		}

		for( product_idx=0; product_idx<product_list_size; ++product_idx ){
			ZnkVarp varp = ZnkVarpAry_at( product_list, product_idx );
			const char* product_type = getProductType( varp );
			const char* module_name  = getProductName( varp );
			const char* main_src     = getProductMainSrc( varp );
	
			Znk_fprintf( fp, "include $(CLEAR_VARS)\n" );
			Znk_fprintf( fp, "LOCAL_MODULE := %s\n", module_name );
			Znk_fprintf( fp, "LOCAL_CFLAGS += -I.. \\\n" );
			Znk_fprintf( fp, "%s", ZnkStr_cstr(include_flags) );
			Znk_fprintf( fp, "\n" );

			{
				ZnkStrAry dir_list = ZnkStrAry_create( true );
				ZnkStrAry list = ZnkStrAry_create( true );
				ZnkStrAry ignore_list = ZnkMyf_find_lines( conf_myf, "ignore_list" );
				size_t size;
				size_t idx;
				const char* name;
				MkfSeek_listDir( list, dir_list, ".", ignore_list, isSrcFileExt );
		
				size = ZnkStrAry_size( list );
				Znk_fprintf( fp, "LOCAL_SRC_FILES := \\\n" );
				for( idx=0; idx<size; ++idx ){
					name = ZnkStrAry_at_cstr( list, idx );

					{
						if( !isExistMainSrc( product_list, name ) ){
							Znk_fprintf( fp, "\t$S/%s \\\n", name );
						}
					}
				}
				Znk_fprintf( fp, "\t$S/%s \\\n", main_src );
				Znk_fprintf( fp, "\n" );
			}
	
			if( sublib_size ){
				Znk_fprintf( fp, "# Refer to prebuilt sub-static-libraries\n" );
				Znk_fprintf( fp, "LOCAL_STATIC_LIBRARIES :=" );
				for( lib_idx=0; lib_idx<sublib_size; ++lib_idx ){
					const char* sublib = ZnkStrAry_at_cstr( sublibs_list, lib_idx );
					MkfLibDepend_parseSubLib( sublib, submkf_dir, sublib_name );
					Znk_fprintf( fp, " %s", ZnkStr_cstr(sublib_name) );
				}
				for( lib_idx=0; lib_idx<deplib_size; ++lib_idx ){
					const char* deplib = ZnkStrAry_at_cstr( dependency_libs_common, lib_idx );
					MkfLibDepend_parseDependencyLib( deplib, dep_lib_type, lib_name, lib_dlver, lib_dir, &is_at_notation );
					if( ZnkStr_eq( dep_lib_type, "slib" ) ){
						Znk_fprintf( fp, " %s", ZnkStr_cstr(lib_dir) );
					}
				}
				Znk_fprintf( fp, "\n" );
			}
			if( deplib_size ){
				Znk_fprintf( fp, "# Refer to prebuilt external-dynamic-libraries\n" );
				Znk_fprintf( fp, "LOCAL_SHARED_LIBRARIES :=" );
				for( lib_idx=0; lib_idx<deplib_size; ++lib_idx ){
					const char* deplib = ZnkStrAry_at_cstr( dependency_libs_common, lib_idx );
					MkfLibDepend_parseDependencyLib( deplib, dep_lib_type, lib_name, lib_dlver, lib_dir, &is_at_notation );
					if( ZnkStr_eq( dep_lib_type, "rlib" ) || ZnkStr_eq( dep_lib_type, "dlib" ) ){
						Znk_fprintf( fp, " %s", ZnkStr_cstr(lib_dir) );
					}
				}
				Znk_fprintf( fp, "\n" );
			}
	
			if( ZnkS_eq( product_type, "dlib" ) ){
				Znk_fprintf( fp, "include $(BUILD_SHARED_LIBRARY)\n" );
			} else if( ZnkS_eq( product_type, "slib" ) ){
				Znk_fprintf( fp, "include $(BUILD_STATIC_LIBRARY)\n" );
			} else if( ZnkS_eq( product_type, "exec" ) ){
				Znk_fprintf( fp, "include $(BUILD_EXECUTABLE)\n" );
			}
			Znk_fprintf( fp, "\n" );
		}

		ZnkStr_delete( dep_lib_type  );
		ZnkStr_delete( lib_name  );
		ZnkStr_delete( lib_dlver );
		ZnkStr_delete( lib_dir   );
		ZnkStr_delete( submkf_dir  );
		ZnkStr_delete( sublib_name );
		ZnkStr_delete( include_flags );
		ZnkStr_delete( template_include_path );
	
		Znk_fclose( fp );
	}

	/* Application.mk */
	fp = Znk_fopen( "mkf_android/jni/Application.mk", "wb" );
	if( fp == NULL ){
		return false;
	} else {
		Znk_fprintf( fp, "APP_PLATFORM := android-8\n" );
		Znk_fprintf( fp, "APP_ABI      := armeabi armeabi-v7a x86\n" );
		Znk_fprintf( fp, "APP_LDFLAGS  := -fPIE\n" );
		Znk_fprintf( fp, "APP_OPTIM    := release\n" );
		Znk_fclose( fp );
	}

	return true;
}
