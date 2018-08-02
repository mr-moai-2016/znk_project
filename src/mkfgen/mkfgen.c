#include <Znk_dir.h>
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <Znk_str_ary.h>
#include <Znk_str_ex.h>
#include <Znk_bird.h>
#include <Znk_str_fio.h>
#include <Znk_myf.h>
#include <Znk_missing_libc.h>
#include <Znk_str_path.h>
#include <string.h>

#include "Mkf_install.h"
#include "Mkf_include.h"
#include "Mkf_seek.h"
#include "Mkf_src_depend.h"
#include "Mkf_lib_depend.h"
#include "Mkf_android.h"
#include "Mkf_sset.h"

static const char*
getDSP( bool is_dos )
{
	if( is_dos ){
		return "\\";
	}
	return "/";
}
static const char*
getSfxRuleLeftDSP( const char* mkf_id )
{
	if( ZnkS_eq( mkf_id, "vc" ) ){
		return "\\";
	} else if( ZnkS_eq( mkf_id, "mingw" ) || ZnkS_eq( mkf_id, "android" ) ){
		return "\\\\";
	}
	return "/";
}
static const char*
getNL( const char* mkf_id )
{
	if(  ZnkS_eq( mkf_id, "vc" )
	  || ZnkS_eq( mkf_id, "mingw" )
	  || ZnkS_eq( mkf_id, "android" )
	 ){
		return "\r\n";
	}
	return "\n";
}
#if 0
static bool
isDOS( const char* mkf_id )
{
	if( ZnkS_eq( mkf_id, "vc" ) || ZnkS_eq( mkf_id, "mingw" ) ){
		return true;
	}
	return false;
}
#endif
static void
addDepLibs( ZnkStr dep_libs, const char* mkf_id, const char* val )
{
	if( ZnkS_eq( mkf_id, "vc" ) ){
		ZnkStr_addf( dep_libs, "%s.lib ", val );
	} else {
		ZnkStr_addf( dep_libs, "-l%s ", val );
	}
}

static bool
isSrcFileExt( const char* ext )
{
	if(  ZnkS_eq( ext, "c" )
	  || ZnkS_eq( ext, "cpp" )
	  || ZnkS_eq( ext, "cxx" )
	){
		return true;
	}
	return false;
}
static bool
isHdrFileExt( const char* ext )
{
	if(  ZnkS_eq( ext, "h" )
	  || ZnkS_eq( ext, "hpp" )
	  || ZnkS_eq( ext, "hxx" )
	){
		return true;
	}
	return false;
}
static bool
isRcFileExt( const char* ext )
{
	if(  ZnkS_eq( ext, "rc" )
	){
		return true;
	}
	return false;
}

static bool
isInterestExt( const char* ext )
{
	return isSrcFileExt( ext ) || isHdrFileExt( ext ) || isRcFileExt( ext );
}

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
static const char*
getProductFileVarName( ZnkVarp varp )
{
	const char* type = getProductType( varp );
	if( ZnkS_eq( type, "exec" ) ){
		return "EXE_FILE";
	} else if( ZnkS_eq( type, "slib" ) ){
		return "SLIB_FILE";
	} else if( ZnkS_eq( type, "dlib" ) ){
		return "DLIB_FILE";
	}
	return "EXE_FILE";
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

static void
getLibFile( ZnkStr lib_file, ZnkStr template_lib_file, ZnkStr template_lib_ver_sfx, ZnkBird bird,
		const char* lib_pfx, const char* lib_name, const char* lib_dlver, const char* lib_sfx )
{
	ZnkStr_clear( lib_file );
	ZnkBird_regist( bird, "lib_pfx",    lib_pfx );
	ZnkBird_regist( bird, "lib_name",   lib_name );
	ZnkBird_regist( bird, "lib_dlver",  lib_dlver );
	ZnkBird_regist( bird, "lib_sfx",    lib_sfx );
	if( template_lib_ver_sfx && !ZnkS_empty(lib_dlver) ){
		ZnkStr lib_ver_sfx = ZnkStr_new( "" );
		ZnkBird_extend( bird, lib_ver_sfx,  ZnkStr_cstr(template_lib_ver_sfx), ZnkStr_leng( template_lib_ver_sfx ) * 2 );
		ZnkBird_regist( bird, "lib_ver_sfx", ZnkStr_cstr(lib_ver_sfx) );
		ZnkStr_delete( lib_ver_sfx );
	} else {
		ZnkBird_regist( bird, "lib_ver_sfx", "" );
	}
	ZnkBird_extend( bird, lib_file, ZnkStr_cstr(template_lib_file), ZnkStr_leng( template_lib_file ) * 2 );
}

static void
getObjsDefsProductFiles( ZnkStr ans, const char* defs_name, ZnkStrAry install_file_list, const char* nl )
{
	size_t size = ZnkStrAry_size( install_file_list );
	if( size ){
		size_t idx;
		const char* file_var_name;
		for( idx=0; idx<size; ++idx ){
			file_var_name = ZnkStrAry_at_cstr( install_file_list, idx );
			ZnkStr_addf( ans, "%s= \\%s", defs_name, nl );
			ZnkStr_addf( ans, "\t__mkg_sentinel_target__ \\%s", nl );
			ZnkStr_addf( ans, "\t%s \\%s",
					file_var_name, nl );
		}
		ZnkStr_add( ans, nl );
	}
}

static void
getObjsDefs( ZnkStr ans,
		ZnkStrAry list, ZnkVarpAry product_list, ZnkStrAry rc_list, ZnkStrAry sublibs_list, const ZnkStrAry runtime_list,
		ZnkStr template_lib_file, ZnkStr template_lib_ver_sfx, ZnkStr template_sublib_dir,
		const char* obj_sfx, const char* exe_sfx,
		const char* slib_pfx, const char* ilib_pfx, const char* dlib_pfx,
		const char* slib_sfx, const char* ilib_sfx, const char* dlib_sfx,
		const char* lib_dlver, MkfInstall mkf_inst, const char* mkf_id, bool is_dos,
		ZnkVarpAry slib_pfx_special_vary, ZnkVarpAry rlib_pfx_special_vary )
{
	char  out_dir[ 256 ] = "$O/";
	char  src_dir[ 256 ] = "$S/";
	size_t       idx;
	const size_t size = ZnkStrAry_size( list );
	const char*  name;
	const char*  ext;
	char         obj_basename[ 256 ] = "";
	const size_t main_size = ZnkVarpAry_size( product_list );
	size_t       main_idx;
	const char* dsp = getDSP( is_dos );
	const char* nl  = getNL( mkf_id );
	ZnkStr  ilib_file = ZnkStr_new( "" );
	ZnkStr  slib_file = ZnkStr_new( "" );
	ZnkStr  dlib_name = ZnkStr_new( "" );
	//ZnkStr  basename_mod = ZnkStr_new( "" );
	ZnkBird bird = ZnkBird_create( "$[", "]$" );

	Znk_snprintf( out_dir, sizeof(out_dir), "$O%s", dsp );
	Znk_snprintf( src_dir, sizeof(src_dir), "$S%s", dsp );

	ZnkStr_clear( ans );

	for( main_idx=0; main_idx<main_size; ++main_idx ){
		const ZnkVarp varp = ZnkVarpAry_at( product_list, main_idx );
		const char* main_name = getProductMainSrc( varp );
		const char* basename  = getProductName( varp );
		const char* file_varname = getProductFileVarName(varp);
		const char* product_type = getProductType( varp );
		const char* product_sfx  = ZnkS_eq( product_type, "exec" ) ? exe_sfx :
			ZnkS_eq( product_type, "slib" ) ? slib_sfx :
			ZnkS_eq( product_type, "dlib" ) ? dlib_sfx :
			ZnkS_eq( product_type, "ilib" ) ? ilib_sfx :
			exe_sfx ;

#if 0
		if( lib_name_special_vary ){
			ZnkVarp lib_name_var = ZnkVarpAry_findObj_byName( lib_name_special_vary,
					basename, Znk_strlen(basename), false );
			if( lib_name_var ){
				ZnkStr_set( basename_mod, ZnkVar_cstr(lib_name_var) );
				basename = ZnkStr_cstr(basename_mod);
			}
		}
#endif
		if( slib_pfx_special_vary ){
			ZnkVarp slib_pfx_var = ZnkVarpAry_findObj_byName( slib_pfx_special_vary,
					basename, Znk_strlen(basename), false );
			if( slib_pfx_var ){
				slib_pfx = ZnkVar_cstr(slib_pfx_var);
			}
		}
		if( rlib_pfx_special_vary ){
			ZnkVarp rlib_pfx_var = ZnkVarpAry_findObj_byName( rlib_pfx_special_vary,
					basename, Znk_strlen(basename), false );
			if( rlib_pfx_var ){
				ilib_pfx = ZnkVar_cstr(rlib_pfx_var);
				dlib_pfx = ZnkVar_cstr(rlib_pfx_var);
			}
		}

		/***
		 * BASENAME,EXE_FILE,DLIB_FILE,SLIB_FILE
		 */
		{
			ZnkStr_addf( ans, "BASENAME%zu=%s%s", main_idx, basename, nl );
			if( ZnkS_eq( product_type, "exec" ) ){
				/* EXE_FILE */
				if( ZnkS_empty( product_sfx ) ){
					ZnkStr_addf( ans, "%s%zu=$O%s%s%s",
							file_varname, main_idx, dsp, basename, nl );
				} else {
					ZnkStr_addf( ans, "%s%zu=$O%s%s.%s%s",
							file_varname, main_idx, dsp, basename, product_sfx, nl );
				}

			} else if( ZnkS_eq( product_type, "dlib" ) || ZnkS_eq( product_type, "ilib" ) ){

				/* DLIB_NAME (linuxにおけるsoname指定などで使用) */
				getLibFile( dlib_name, template_lib_file, template_lib_ver_sfx, bird,
						dlib_pfx, basename, lib_dlver, product_sfx );
				ZnkStr_addf( ans, "DLIB_NAME%zu=%s%s",
						main_idx, ZnkStr_cstr(dlib_name), nl );
				/* DLIB_FILE */
				ZnkStr_addf( ans, "%s%zu=$O%s$(DLIB_NAME%zu)%s",
						file_varname, main_idx, dsp, main_idx, nl );

				/* ILIB_FILE */
				getLibFile( ilib_file, template_lib_file, template_lib_ver_sfx, bird,
						ilib_pfx, basename, lib_dlver, ilib_sfx );
				ZnkStr_addf( ans, "ILIB_FILE%zu=$O%s%s%s",
						main_idx, dsp, ZnkStr_cstr(ilib_file), nl );

				/* SLIB_FILE */
				getLibFile( slib_file, template_lib_file, NULL, bird,
						slib_pfx, basename, lib_dlver, slib_sfx );
				ZnkStr_addf( ans, "SLIB_FILE%zu=$O%s%s%s",
						main_idx, dsp, ZnkStr_cstr(slib_file), nl );
			} else if( ZnkS_eq( product_type, "slib" ) ){
				/* SLIB_FILE */
				getLibFile( slib_file, template_lib_file, NULL, bird,
						slib_pfx, basename, lib_dlver, slib_sfx );
				ZnkStr_addf( ans, "SLIB_FILE%zu=$O%s%s%s",
						main_idx, dsp, ZnkStr_cstr(slib_file), nl );
			}
		}

		/***
		 * OBJS
		 */
		ZnkStr_addf( ans, "OBJS%zu=\\%s", main_idx, nl );
		for( idx=0; idx<size; ++idx ){
			name = ZnkStrAry_at_cstr( list, idx );
			ext  = ZnkS_get_extension( name, '.' );
			if( isSrcFileExt( ext ) ){
				if( !isExistMainSrc( product_list, name ) ){
					ZnkS_copy( obj_basename, sizeof(obj_basename), name, ext-1-name );
					ZnkStr_addf( ans, "\t%s%s.%s \\%s", out_dir, obj_basename, obj_sfx, nl );
				}
			}
		}

		{
			ext  = ZnkS_get_extension( main_name, '.' );
			if( ext ){
				ZnkS_copy( obj_basename, sizeof(obj_basename), main_name, ext-1-main_name );
				ZnkStr_addf( ans, "\t%s%s.%s \\%s", out_dir, obj_basename, obj_sfx, nl );
			} else {
				ZnkStr_addf( ans, "\t%s%s.%s \\%s", out_dir, main_name, obj_sfx, nl );
			}
			//ZnkStr_addf( ans, "\t%s%s.%s \\%s", out_dir, main_name, obj_sfx, nl );
		}
		ZnkStr_add( ans, nl );

	}

	/***
	 * SUB_LIBS
	 */
#if 0
	if( sublibs_list ){
		const size_t sublibs_size = ZnkStrAry_size( sublibs_list );
		ZnkStr    sublib_file = ZnkStr_new( "" );
		ZnkStrAry tkns = ZnkStrAry_create( true );
		ZnkStr_addf( ans, "SUB_LIBS=\\%s", nl );
		for( idx=0; idx<sublibs_size; ++idx ){
			const ZnkStr sublib = ZnkStrAry_at(sublibs_list,idx);
			ZnkStrEx_addSplitCSet( tkns,
					ZnkStr_cstr(sublib), ZnkStr_leng(sublib),
					" \t", 2,
					2 );
			ZnkBird_regist( bird, "obj_sfx",   obj_sfx );
			ZnkBird_regist( bird, "lib_pfx",   slib_pfx );
			ZnkBird_regist( bird, "slib_sfx",  slib_sfx );
			ZnkBird_regist( bird, "submkf_dir",  ZnkStrAry_at_cstr(tkns,0) );
			ZnkBird_regist( bird, "sublib_name", ZnkStrAry_at_cstr(tkns,1) );
			ZnkBird_extend( bird, sublib_file,  ZnkStr_cstr(template_sublib), ZnkStr_leng( template_sublib ) * 2 );
			ZnkStr_addf( ans, "\t%s \\%s", ZnkStr_cstr( sublib_file ), nl );
		}
		ZnkStr_add( ans, nl );
		ZnkStrAry_destroy( tkns );
		ZnkStr_delete( sublib_file );
	}
#else
	if( sublibs_list ){
		const size_t sublibs_size = ZnkStrAry_size( sublibs_list );
		ZnkStr       sublib_file  = ZnkStr_new( "" );
		ZnkStr       sublib_dir   = ZnkStr_new( "" );
		ZnkStrAry    tkns = ZnkStrAry_create( true );
		ZnkStr_addf( ans, "SUB_LIBS=\\%s", nl );
		for( idx=0; idx<sublibs_size; ++idx ){
			const ZnkStr sublib = ZnkStrAry_at(sublibs_list,idx);
			ZnkStrEx_addSplitCSet( tkns,
					ZnkStr_cstr(sublib), ZnkStr_leng(sublib),
					" \t", 2,
					2 );

			ZnkBird_regist( bird, "submkf_dir",  ZnkStrAry_at_cstr(tkns,0) );
			ZnkBird_extend( bird, sublib_dir,  ZnkStr_cstr( template_sublib_dir ), ZnkStr_leng( template_sublib_dir ) * 2 );

			getLibFile( sublib_file, template_lib_file, NULL, bird,
					slib_pfx, ZnkStrAry_at_cstr(tkns,1), lib_dlver, slib_sfx );

			ZnkStr_addf( ans, "\t%s/%s \\%s", ZnkStr_cstr( sublib_dir ), ZnkStr_cstr(sublib_file), nl );
		}
		ZnkStr_add( ans, nl );
		ZnkStrAry_destroy( tkns );
		ZnkStr_delete( sublib_dir );
		ZnkStr_delete( sublib_file );
	}
#endif
#if 0
	/***
	 * SUB_OBJS
	 */
	if( sublibs_list ){
		static const char* template_subobjs = "$[submkf_dir]$/$O/*.$[obj_sfx]$";
		const size_t sublibs_size = ZnkStrAry_size( sublibs_list );
		ZnkStr    sub_objs = ZnkStr_new( "" );
		ZnkStrAry tkns = ZnkStrAry_create( true );
		ZnkStr_addf( ans, "SUB_OBJS=\\%s", nl );
		for( idx=0; idx<sublibs_size; ++idx ){
			const ZnkStr sublib = ZnkStrAry_at(sublibs_list,idx);
			ZnkStrEx_addSplitCSet( tkns,
					ZnkStr_cstr(sublib), ZnkStr_leng(sublib),
					" \t", 2,
					2 );
			ZnkBird_regist( bird, "obj_sfx",   obj_sfx );
			ZnkBird_regist( bird, "submkf_dir",  ZnkStrAry_at_cstr(tkns,0) );
			ZnkBird_extend( bird, sub_objs,  template_subobjs, Znk_strlen( template_subobjs ) * 2 );
			ZnkStr_addf( ans, "\t%s \\%s", ZnkStr_cstr( sub_objs ), nl );
		}
		ZnkStr_add( ans, nl );
		ZnkStrAry_destroy( tkns );
		ZnkStr_delete( sub_objs );
	}
#else
	/***
	 * SUB_OBJS
	 */
	if( sublibs_list ){
		const size_t sublibs_size = ZnkStrAry_size( sublibs_list );
		ZnkStr       sublib_dir   = ZnkStr_new( "" );
		ZnkStrAry    tkns = ZnkStrAry_create( true );
		ZnkStr_addf( ans, "SUB_OBJS=\\%s", nl );
		for( idx=0; idx<sublibs_size; ++idx ){
			const ZnkStr sublib = ZnkStrAry_at(sublibs_list,idx);
			ZnkStrEx_addSplitCSet( tkns,
					ZnkStr_cstr(sublib), ZnkStr_leng(sublib),
					" \t", 2,
					2 );

			ZnkBird_regist( bird, "submkf_dir",  ZnkStrAry_at_cstr(tkns,0) );
			ZnkBird_extend( bird, sublib_dir,  ZnkStr_cstr( template_sublib_dir ), ZnkStr_leng( template_sublib_dir ) * 2 );

			ZnkStr_addf( ans, "\t%s/*.%s \\%s", ZnkStr_cstr( sublib_dir ), obj_sfx, nl );
		}
		ZnkStr_add( ans, nl );
		ZnkStrAry_destroy( tkns );
		ZnkStr_delete( sublib_dir );
	}
#endif
	/***
	 * SUB_OBJS_ECHO
	 */
	if( sublibs_list ){
		const size_t sublibs_size = ZnkStrAry_size( sublibs_list );
		ZnkStr       sublib_dir   = ZnkStr_new( "" );
		ZnkStrAry    tkns = ZnkStrAry_create( true );
		ZnkStr_addf( ans, "SUB_OBJS_ECHO=\\%s", nl );
		for( idx=0; idx<sublibs_size; ++idx ){
			const ZnkStr sublib = ZnkStrAry_at(sublibs_list,idx);
			ZnkStrEx_addSplitCSet( tkns,
					ZnkStr_cstr(sublib), ZnkStr_leng(sublib),
					" \t", 2,
					2 );

			ZnkBird_regist( bird, "submkf_dir",  ZnkStrAry_at_cstr(tkns,0) );
			ZnkBird_extend( bird, sublib_dir,  ZnkStr_cstr( template_sublib_dir ), ZnkStr_leng( template_sublib_dir ) * 2 );

			ZnkStr_addf( ans, "\t%s/{[objs]} \\%s", ZnkStr_cstr( sublib_dir ), nl );
		}
		ZnkStr_add( ans, nl );
		ZnkStrAry_destroy( tkns );
		ZnkStr_delete( sublib_dir );
	}

	/***
	 * RES_FILE,RC_FILE
	 */
	/***
	 * rc_listを自動的に補填する.
	 */
	for( idx=0; idx<size; ++idx ){
		name = ZnkStrAry_at_cstr( list, idx );
		ext  = ZnkS_get_extension( name, '.' );
		if( isRcFileExt( ext ) ){
			ZnkS_copy( obj_basename, sizeof(obj_basename), name, ext-1-name );
			if( ZnkStrAry_find( rc_list, 0, obj_basename, Znk_strlen(obj_basename) ) == Znk_NPOS ){
				ZnkStrAry_push_bk_cstr( rc_list, obj_basename, Znk_NPOS );
			}
		}
	}
	{
		const size_t rc_size = ZnkStrAry_size( rc_list );
		size_t rc_idx;
		for( rc_idx=0; rc_idx<rc_size; ++rc_idx ){
			const char* rc_base = ZnkStrAry_at_cstr( rc_list, rc_idx );
			ZnkStr_addf( ans, "RES_FILE%zu=%s%s.res%s", rc_idx, out_dir, rc_base, nl );
			ZnkStr_addf( ans, "RC_FILE%zu =%s%s.rc%s",  rc_idx, src_dir, rc_base, nl );
		}
	}

	/***
	 * PRODUCT_FILES, RUNTIME_FILES
	 */
	{
		ZnkStr runtime_files = ZnkStr_new( "" );
		ZnkVarp product = NULL;
		const char* runtime = NULL;
		size_t size;
		const char* product_type = "";

		size = ZnkVarpAry_size( product_list );
		for( idx=0; idx<size; ++idx ){
			product = ZnkVarpAry_at( product_list, idx );
			product_type = getProductType( product );
			if( ZnkS_eq( product_type, "exec" ) ){
				ZnkStrAry_push_bk_snprintf( mkf_inst->install_exec_list_, Znk_NPOS,
						"$(%s%zu)", getProductFileVarName( product ), idx );
			} else if( ZnkS_eq( product_type, "dlib" ) || ZnkS_eq( product_type, "rlib" ) ){
				ZnkStrAry_push_bk_snprintf( mkf_inst->install_dlib_list_, Znk_NPOS,
						"$(%s%zu)", getProductFileVarName( product ), idx );
				/***
				 * 特別措置:
				 * install_slib_dir として install_dlib_dirとは異なるパスが指定されていた場合のみ、
				 * slibのインストールを有効にする.
				 */
				if( !ZnkStr_eq(mkf_inst->install_slib_dir_, ZnkStr_cstr(mkf_inst->install_dlib_dir_)) ){
					ZnkStrAry_push_bk_snprintf( mkf_inst->install_slib_list_, Znk_NPOS,
							"$(SLIB_FILE%zu)", idx );
				}
			} else if( ZnkS_eq( product_type, "slib" ) ){
				ZnkStrAry_push_bk_snprintf( mkf_inst->install_slib_list_, Znk_NPOS,
						"$(%s%zu)", getProductFileVarName( product ), idx );
			} else {
			}
		}

		size = ZnkStrAry_size( runtime_list );
		ZnkStr_addf( runtime_files, "\t__mkg_sentinel_target__ \\%s", nl );
		for( idx=0; idx<size; ++idx ){
			runtime = ZnkStrAry_at_cstr( runtime_list, idx );
			ZnkStr_addf( runtime_files, "\t%s \\%s", runtime, nl );
		}

		getObjsDefsProductFiles( ans, "PRODUCT_EXECS", mkf_inst->install_exec_list_, nl );
		getObjsDefsProductFiles( ans, "PRODUCT_DLIBS", mkf_inst->install_dlib_list_, nl );
		getObjsDefsProductFiles( ans, "PRODUCT_SLIBS", mkf_inst->install_slib_list_, nl );

		ZnkStr_addf( ans, "RUNTIME_FILES= \\%s", nl );
		ZnkStr_add( ans, ZnkStr_cstr(runtime_files) );
		ZnkStr_add( ans, nl );

		ZnkStr_delete( runtime_files );
	}

	ZnkBird_destroy( bird );
	ZnkStr_delete( ilib_file );
	ZnkStr_delete( slib_file );
	ZnkStr_delete( dlib_name );
	//ZnkStr_delete( basename_mod );
}

static void
getSuffixRule( ZnkStr ans, ZnkMyf tmpl_myf, const char* mkf_id, ZnkStrAry dir_list,
		ZnkStrAry src_suffix_list, bool is_dos )
{
	ZnkStrAry lines = ZnkMyf_find_lines( tmpl_myf, "suffix_rule" );
	size_t  size = ZnkStrAry_size( lines );
	size_t  idx;
	ZnkStr  rule_src = ZnkStr_new( "" );
	ZnkBird rule_bird = ZnkBird_create( "$[", "]$" );
	const char* nl = getNL( mkf_id );

	for( idx=0; idx<size; ++idx ){
		ZnkStr_addf( rule_src, "%s%s", ZnkStrAry_at_cstr( lines, idx ), nl );
	}

	{
		const char* src_sfx;
		size_t       dir_idx;
		const size_t dir_list_size = ZnkStrAry_size( dir_list );
		/* とりあえず */
		const char* src_dsp = getDSP( is_dos );
		const char* obj_dsp = getSfxRuleLeftDSP( mkf_id );

		ZnkStr_clear( ans );

		for( dir_idx=0; dir_idx<dir_list_size; ++dir_idx ){
			const char* dir = ZnkStrAry_at_cstr( dir_list, dir_idx );
			char src_dir[ 256 ] = "";
			char obj_dir[ 256 ] = "";

			if( ZnkS_eq( dir, "." ) ){
				ZnkS_copy( src_dir, sizeof(src_dir), "$S", Znk_NPOS );
				ZnkS_copy( obj_dir, sizeof(obj_dir), "$O", Znk_NPOS );
			} else {
				Znk_snprintf( src_dir, sizeof(src_dir), "$S%s%s", src_dsp, dir );
				Znk_snprintf( obj_dir, sizeof(obj_dir), "$O%s%s", obj_dsp, dir );
			}

			size = ZnkStrAry_size( src_suffix_list );
			for( idx=0; idx<size; ++idx ){
				src_sfx = ZnkStrAry_at_cstr( src_suffix_list, idx );
				ZnkBird_regist( rule_bird, "src_dir", src_dir );
				ZnkBird_regist( rule_bird, "obj_dir", obj_dir );
				ZnkBird_regist( rule_bird, "src_sfx", src_sfx );
				ZnkBird_extend( rule_bird, ans, ZnkStr_cstr(rule_src), ZnkStr_leng( rule_src ) * 2 );
				ZnkStr_add( ans, nl );
			}
		}
	}

	ZnkBird_destroy( rule_bird );
	ZnkStr_delete( rule_src );
}

static void
getAllTargetRule( ZnkStr ans, ZnkStrAry dir_list, const ZnkStrAry submkf_list, const ZnkVarpAry product_list, const char* mkf_id, bool is_dos )
{
	size_t       idx;
	const size_t dir_list_size     = ZnkStrAry_size( dir_list );
	const size_t submkf_size      = ZnkStrAry_size( submkf_list );
	const size_t product_list_size = ZnkVarpAry_size( product_list );
	const char* obj_dsp = getDSP( is_dos );
	const char* nl      = getNL( mkf_id );

	ZnkStr_clear( ans );
	ZnkStr_add( ans, "all: " );

	for( idx=0; idx<dir_list_size; ++idx ){
		const char* dir = ZnkStrAry_at_cstr( dir_list, idx );
		char obj_dir[ 256 ] = "";

		if( ZnkS_eq( dir, "." ) ){
			ZnkS_copy( obj_dir, sizeof(obj_dir), "$O", Znk_NPOS );
		} else {
			Znk_snprintf( obj_dir, sizeof(obj_dir), "$O%s%s", obj_dsp, dir );
		}

		ZnkStr_addf( ans, "%s ", obj_dir );
	}

	for( idx=0; idx<submkf_size; ++idx ){
		const char* submkf_dir = ZnkStrAry_at_cstr( submkf_list, idx );
		ZnkStr_addf( ans, "submkf_%s ", submkf_dir );
	}

	for( idx=0; idx<product_list_size; ++idx ){
		ZnkVarp     varp = ZnkVarpAry_at( product_list, idx );
		ZnkStr_addf( ans, "$(%s%zu) ", getProductFileVarName(varp), idx );

		/***
		 * dlibであった場合はslibもall-targetに追加.
		 * dll系の環境ではgsl.myfで必要となるため、相当するslibは自動的に作られるが
		 * so系の環境ではトリガーが存在しないため、all-targetで明示的に指定しておく必要がある.
		 */
		{
			const char* type = getProductType( varp );
			if( ZnkS_eq( type, "dlib" ) ){
				ZnkStr_addf( ans, "$(SLIB_FILE%zu) ", idx );
			}
		}
	}
	ZnkStr_add( ans, nl );
}

static void
getMkdirRule( ZnkStr ans, ZnkMyf tmpl_myf, const char* mkf_id, ZnkStrAry dir_list, bool is_dos )
{
	ZnkStrAry lines = ZnkMyf_find_lines( tmpl_myf, "mkdir_rule" );
	size_t  size = ZnkStrAry_size( lines );
	size_t  idx;
	ZnkStr  rule_src = ZnkStr_new( "" );
	ZnkBird rule_bird = ZnkBird_create( "$[", "]$" );
	const char* nl = getNL( mkf_id );

	for( idx=0; idx<size; ++idx ){
		ZnkStr_addf( rule_src, "%s%s", ZnkStrAry_at_cstr( lines, idx ), nl );
	}

	{
		size_t       dir_idx;
		const size_t dir_list_size = ZnkStrAry_size( dir_list );
		/* とりあえず */
		const char* obj_dsp = getDSP( is_dos );

		ZnkStr_clear( ans );

		for( dir_idx=0; dir_idx<dir_list_size; ++dir_idx ){
			const char* dir = ZnkStrAry_at_cstr( dir_list, dir_idx );
			char obj_dir[ 256 ] = "";

			if( ZnkS_eq( dir, "." ) ){
				ZnkS_copy( obj_dir, sizeof(obj_dir), "$O", Znk_NPOS );
			} else {
				Znk_snprintf( obj_dir, sizeof(obj_dir), "$O%s%s", obj_dsp, dir );
			}

			ZnkBird_regist( rule_bird, "obj_dir", obj_dir );
			ZnkBird_extend( rule_bird, ans, ZnkStr_cstr(rule_src), ZnkStr_leng( rule_src ) * 2 );
			ZnkStr_add( ans, nl );
		}
	}

	ZnkBird_destroy( rule_bird );
	ZnkStr_delete( rule_src );
}

static void
getProductFilesRule( ZnkStr ans, ZnkMyf tmpl_myf, const char* mkf_id,
		ZnkVarpAry product_list, ZnkStrAry rc_list, ZnkStr dep_libs,
		const char* ilib_sfx,
		const char* resfile_additional, const char* gslconv )
{
	size_t size;
	size_t idx;
	const char* nl = getNL( mkf_id );

	ZnkStrAry dlib_lines     = ZnkMyf_find_lines( tmpl_myf, "dlibfile_rule" );
	ZnkStr    dlib_rule_src  = ZnkStr_new( "" );
	ZnkBird   dlib_rule_bird = ZnkBird_create( "$[", "]$" );

	ZnkStrAry slib_lines     = ZnkMyf_find_lines( tmpl_myf, "slibfile_rule" );
	ZnkStr    slib_rule_src  = ZnkStr_new( "" );
	ZnkBird   slib_rule_bird = ZnkBird_create( "$[", "]$" );

	ZnkStrAry exec_lines     = ZnkMyf_find_lines( tmpl_myf, "exefile_rule" );
	ZnkStr    exec_rule_src  = ZnkStr_new( "" );
	ZnkBird   exec_rule_bird = ZnkBird_create( "$[", "]$" );

	ZnkStrAry gsl_lines      = ZnkMyf_find_lines( tmpl_myf, "gsl_rule" );
	ZnkStr    gsl_rule_src   = ZnkStr_new( "" );
	ZnkBird   gsl_rule_bird  = ZnkBird_create( "$[", "]$" );

	ZnkStr    slib_file = ZnkStr_new( "" );
	ZnkStr    ilib_file = ZnkStr_new( "" );
	ZnkStr    def_file = ZnkStr_new( "" );
	ZnkStr    dlib_name = ZnkStr_new( "" );

	size = ZnkStrAry_size( dlib_lines );
	for( idx=0; idx<size; ++idx ){
		ZnkStr_addf( dlib_rule_src, "%s%s", ZnkStrAry_at_cstr( dlib_lines, idx ), nl );
	}
	size = ZnkStrAry_size( slib_lines );
	for( idx=0; idx<size; ++idx ){
		ZnkStr_addf( slib_rule_src, "%s%s", ZnkStrAry_at_cstr( slib_lines, idx ), nl );
	}
	size = ZnkStrAry_size( exec_lines );
	for( idx=0; idx<size; ++idx ){
		ZnkStr_addf( exec_rule_src, "%s%s", ZnkStrAry_at_cstr( exec_lines, idx ), nl );
	}
	size = ZnkStrAry_size( gsl_lines );
	for( idx=0; idx<size; ++idx ){
		ZnkStr_addf( gsl_rule_src, "%s%s", ZnkStrAry_at_cstr( gsl_lines, idx ), nl );
	}

	{
		size_t main_idx;
		size_t rc_idx;
		size_t main_list_size = ZnkVarpAry_size( product_list );
		ZnkStr res_file = ZnkStr_new( "" );
		char   obj_basename[ 256 ] = "";

		ZnkStr_clear( ans );

		for( main_idx=0; main_idx<main_list_size; ++main_idx ){
			char product_file[ 256 ] = "";
			char objs[ 256 ] = "";
			ZnkVarp varp = ZnkVarpAry_at( product_list, main_idx );
			const char* type      = getProductType( varp );
			const char* main_name = getProductMainSrc( varp );
			const char* basename  = getProductName( varp );
			const char* file_varname = getProductFileVarName( varp );

			Znk_snprintf( product_file, sizeof(product_file), "$(%s%zu)", file_varname, main_idx );
			Znk_snprintf( objs,     sizeof(objs),     "$(OBJS%zu)",     main_idx );

			rc_idx = ZnkStrAry_find( rc_list, 0, basename, Znk_strlen(basename) );
			if( rc_idx == Znk_NPOS ){
				const char* ext  = ZnkS_get_extension( main_name, '.' );
				if( ext ){
					ZnkS_copy( obj_basename, sizeof(obj_basename), main_name, ext-1-main_name );
					rc_idx = ZnkStrAry_find( rc_list, 0, obj_basename, Znk_strlen(obj_basename) );
				} else {
					rc_idx = ZnkStrAry_find( rc_list, 0, main_name, Znk_strlen(main_name) );
				}
			}
			if( rc_idx != Znk_NPOS ){
				ZnkStr_setf( res_file, "$(RES_FILE%zu)", rc_idx );
				if( !ZnkS_empty( resfile_additional ) ){
					ZnkStr_addf( res_file, " %s", resfile_additional );
				}
			} else {
				ZnkStr_clear( res_file );
				if( !ZnkS_empty( resfile_additional ) ){
					ZnkStr_set( res_file, resfile_additional );
				}
			}

			if( ZnkS_eq( type, "exec" ) ){
				ZnkBird_regist( exec_rule_bird, "exe_file", product_file );
				ZnkBird_regist( exec_rule_bird, "objs",     objs );
				ZnkBird_regist( exec_rule_bird, "res_file", ZnkStr_cstr(res_file) );
				ZnkBird_regist( exec_rule_bird, "dep_libs", ZnkStr_cstr(dep_libs) );
				ZnkBird_extend( exec_rule_bird, ans, ZnkStr_cstr(exec_rule_src), ZnkStr_leng( exec_rule_src ) * 2 );
			} else if( ZnkS_eq( type, "slib" ) ){
				ZnkBird_regist( slib_rule_bird, "slib_file", product_file );
				ZnkBird_regist( slib_rule_bird, "objs",      objs );
				ZnkBird_regist( slib_rule_bird, "res_file",  ZnkStr_cstr(res_file) );
				ZnkBird_regist( slib_rule_bird, "dep_libs",  ZnkStr_cstr(dep_libs) );
				ZnkBird_extend( slib_rule_bird, ans, ZnkStr_cstr(slib_rule_src), ZnkStr_leng( slib_rule_src ) * 2 );
			} else if( ZnkS_eq( type, "dlib" ) || ZnkS_eq( type, "ilib" ) ){
				ZnkStr_setf( ilib_file, "$(ILIB_FILE%zu)", main_idx );
				ZnkStr_setf( dlib_name, "$(DLIB_NAME%zu)", main_idx );

				ZnkStr_set( def_file,  "gsl.def" );

				ZnkStr_setf( slib_file, "$(SLIB_FILE%zu)", main_idx );

				ZnkBird_regist( slib_rule_bird, "slib_file", ZnkStr_cstr(slib_file) );
				ZnkBird_regist( slib_rule_bird, "objs",      objs );
				ZnkBird_regist( slib_rule_bird, "res_file",  ZnkStr_cstr(res_file) );
				ZnkBird_regist( slib_rule_bird, "dep_libs",  ZnkStr_cstr(dep_libs) );
				ZnkBird_extend( slib_rule_bird, ans, ZnkStr_cstr(slib_rule_src), ZnkStr_leng( slib_rule_src ) * 2 );
				ZnkStr_add( ans, nl );

				ZnkBird_regist( gsl_rule_bird, "slib_file", ZnkStr_cstr(slib_file) );
				ZnkBird_regist( gsl_rule_bird, "gslconv",   gslconv );
				ZnkBird_extend( gsl_rule_bird, ans, ZnkStr_cstr(gsl_rule_src), ZnkStr_leng( gsl_rule_src ) * 2 );
				ZnkStr_add( ans, nl );

				ZnkBird_regist( dlib_rule_bird, "slib_file", ZnkStr_cstr(slib_file) );
				ZnkBird_regist( dlib_rule_bird, "dlib_file", product_file );
				ZnkBird_regist( dlib_rule_bird, "objs",      objs );
				ZnkBird_regist( dlib_rule_bird, "res_file",  ZnkStr_cstr(res_file) );
				ZnkBird_regist( dlib_rule_bird, "dep_libs",  ZnkStr_cstr(dep_libs) );
				ZnkBird_regist( dlib_rule_bird, "ilib_file", ZnkStr_cstr(ilib_file) );
				ZnkBird_regist( dlib_rule_bird, "dlib_name", ZnkStr_cstr(dlib_name) );
				ZnkBird_regist( dlib_rule_bird, "def_file",  ZnkStr_cstr(def_file) );
				ZnkBird_extend( dlib_rule_bird, ans, ZnkStr_cstr(dlib_rule_src), ZnkStr_leng( dlib_rule_src ) * 2 );
			}
			ZnkStr_add( ans, nl );
		}
		ZnkStr_delete( res_file );
	}

	ZnkStr_delete( slib_file );
	ZnkStr_delete( ilib_file );
	ZnkStr_delete( def_file );
	ZnkStr_delete( dlib_name );

	ZnkBird_destroy( dlib_rule_bird );
	ZnkStr_delete(   dlib_rule_src );
	ZnkBird_destroy( slib_rule_bird );
	ZnkStr_delete(   slib_rule_src );
	ZnkBird_destroy( exec_rule_bird );
	ZnkStr_delete(   exec_rule_src );
	ZnkBird_destroy( gsl_rule_bird );
	ZnkStr_delete(   gsl_rule_src );
}

static void
getRcRule( ZnkStr ans, ZnkMyf tmpl_myf, const char* mkf_id, ZnkStrAry rc_list )
{
	ZnkStrAry lines = ZnkMyf_find_lines( tmpl_myf, "rc_rule" );
	size_t  size = ZnkStrAry_size( lines );
	size_t  idx;
	ZnkStr  rule_src = ZnkStr_new( "" );
	ZnkBird rule_bird = ZnkBird_create( "$[", "]$" );
	const char* nl = getNL( mkf_id );

	for( idx=0; idx<size; ++idx ){
		ZnkStr_addf( rule_src, "%s%s", ZnkStrAry_at_cstr( lines, idx ), nl );
	}

	{
		size_t   rc_idx;

		size = ZnkStrAry_size( rc_list );
		ZnkStr_clear( ans );
		for( rc_idx=0; rc_idx<size; ++rc_idx ){
			char res_file[ 256 ] = "";
			char rc_file[ 256 ] = "";

			Znk_snprintf( res_file, sizeof(res_file), "$(RES_FILE%zu)", rc_idx );
			Znk_snprintf( rc_file,  sizeof(rc_file),  "$(RC_FILE%zu)",  rc_idx );

			ZnkBird_regist( rule_bird, "res_file", res_file );
			ZnkBird_regist( rule_bird, "rc_file",  rc_file );
			ZnkBird_extend( rule_bird, ans, ZnkStr_cstr(rule_src), ZnkStr_leng( rule_src ) * 2 );
			ZnkStr_add( ans, nl );
		}
	}

	ZnkBird_destroy( rule_bird );
	ZnkStr_delete( rule_src );
}

static void
getSubmkfRule( ZnkStr ans, ZnkMyf tmpl_myf, const char* mkf_id, const ZnkStrAry submkf_list, const char* nl )
{
	ZnkStr_clear( ans );
	if( submkf_list ){
		ZnkStrAry lines = ZnkMyf_find_lines( tmpl_myf, "submkf_rule" );
		size_t  size = ZnkStrAry_size( lines );
		size_t  idx;
		ZnkStr  rule_src = ZnkStr_new( "" );
		ZnkBird rule_bird = ZnkBird_create( "$[", "]$" );
		for( idx=0; idx<size; ++idx ){
			ZnkStr_addf( rule_src, "%s%s", ZnkStrAry_at_cstr( lines, idx ), nl );
		}
	
		{
			size_t   submkf_idx;
	
			size = ZnkStrAry_size( submkf_list );
			for( submkf_idx=0; submkf_idx<size; ++submkf_idx ){
				const char* submkf_dir = ZnkStrAry_at_cstr( submkf_list, submkf_idx );
				ZnkBird_regist( rule_bird, "submkf_dir", submkf_dir );
				ZnkBird_regist( rule_bird, "mkf_id",  mkf_id );
				ZnkBird_extend( rule_bird, ans, ZnkStr_cstr(rule_src), ZnkStr_leng( rule_src ) * 2 );
				ZnkStr_add( ans, nl );
			}
		}
	
		ZnkBird_destroy( rule_bird );
		ZnkStr_delete( rule_src );
	}
}
static void
getCleanRule( ZnkStr ans, ZnkMyf tmpl_myf, const char* mkf_id, const ZnkStrAry submkf_list, const char* nl )
{
	ZnkStrAry lines;
	size_t    size;
	size_t    idx;
	ZnkStr    rule_src = ZnkStr_new( "" );
	ZnkBird   rule_bird = ZnkBird_create( "$[", "]$" );

	ZnkStr_clear( ans );
	ZnkStr_addf( ans, "clean:%s", nl );

	lines = ZnkMyf_find_lines( tmpl_myf, "clean_here_rule" );
	size  = ZnkStrAry_size( lines );
	for( idx=0; idx<size; ++idx ){
		ZnkStr_addf( rule_src, "%s%s", ZnkStrAry_at_cstr( lines, idx ), nl );
	}

	ZnkBird_regist( rule_bird, "mkf_id", mkf_id );
	ZnkBird_extend( rule_bird, ans, ZnkStr_cstr(rule_src), ZnkStr_leng( rule_src ) * 2 );

	if( submkf_list ){

		ZnkStr_clear( rule_src );
		lines = ZnkMyf_find_lines( tmpl_myf, "clean_sub_rule" );
		size  = ZnkStrAry_size( lines );
		for( idx=0; idx<size; ++idx ){
			ZnkStr_addf( rule_src, "%s%s", ZnkStrAry_at_cstr( lines, idx ), nl );
		}
	
		{
			size_t   submkf_idx;
			size = ZnkStrAry_size( submkf_list );
			for( submkf_idx=0; submkf_idx<size; ++submkf_idx ){
				const char* submkf_dir = ZnkStrAry_at_cstr( submkf_list, submkf_idx );
				ZnkBird_regist( rule_bird, "submkf_dir", submkf_dir );
				ZnkBird_extend( rule_bird, ans, ZnkStr_cstr(rule_src), ZnkStr_leng( rule_src ) * 2 );
				ZnkStr_add( ans, nl );
			}
		}
	
	}

	ZnkBird_destroy( rule_bird );
	ZnkStr_delete( rule_src );
}

static void
getDepLib( ZnkStr dep_libs, ZnkStrAry runtime_list, const char* dependency_lib,
		ZnkStr template_lib_file, ZnkStr template_runtime_path, ZnkStr template_lib_ver_sfx, ZnkStr template_libs_path,
		const char* slib_pfx, const char* ilib_pfx, const char* dlib_pfx,
		const char* slib_sfx, const char* ilib_sfx, const char* dlib_sfx,
		const char* mkf_id, const char* machine,
		ZnkVarpAry slib_pfx_special_vary, ZnkVarpAry rlib_pfx_special_vary )
{
	ZnkBird bird = ZnkBird_create( "$[", "]$" );
	ZnkStr dep_lib_type = ZnkStr_new( "" );
	ZnkStr lib_name  = ZnkStr_new( "" );
	ZnkStr lib_dlver = ZnkStr_new( "" );
	ZnkStr lib_dir   = ZnkStr_new( "" );
	ZnkStr lib_file  = ZnkStr_new( "" );
	ZnkStr runtime_file = ZnkStr_new( "" );
	bool   is_at_notation = false;

	MkfLibDepend_parseDependencyLib( dependency_lib,
			dep_lib_type,
			lib_name,
			lib_dlver,
			lib_dir,
			&is_at_notation );

#if 0
	if( lib_name_special_vary ){
		ZnkVarp lib_name_var = ZnkVarpAry_findObj_byName( lib_name_special_vary,
				ZnkStr_cstr(lib_name), ZnkStr_leng(lib_name), false );
		if( lib_name_var ){
			ZnkStr_set( lib_name, ZnkVar_cstr(lib_name_var) );
		}
	}
#endif
	if( slib_pfx_special_vary ){
		ZnkVarp slib_pfx_var = ZnkVarpAry_findObj_byName( slib_pfx_special_vary,
				ZnkStr_cstr(lib_name), ZnkStr_leng(lib_name), false );
		if( slib_pfx_var ){
			slib_pfx = ZnkVar_cstr(slib_pfx_var);
		}
	}
	if( rlib_pfx_special_vary ){
		ZnkVarp rlib_pfx_var = ZnkVarpAry_findObj_byName( rlib_pfx_special_vary,
				ZnkStr_cstr(lib_name), ZnkStr_leng(lib_name), false );
		if( rlib_pfx_var ){
			ilib_pfx = ZnkVar_cstr(rlib_pfx_var);
			dlib_pfx = ZnkVar_cstr(rlib_pfx_var);
		}
	}

	if( ZnkStr_eq(dep_lib_type, "slib" ) ){
		getLibFile( lib_file, template_lib_file, NULL, bird,
				slib_pfx, ZnkStr_cstr( lib_name ), ZnkStr_cstr( lib_dlver ), slib_sfx );
	
		ZnkBird_regist( bird, "lib_dir",  ZnkStr_cstr( lib_dir ) );
		ZnkBird_regist( bird, "lib_file", ZnkStr_cstr(lib_file) );
		if( is_at_notation ){
			ZnkBird_extend( bird, dep_libs, ZnkStr_cstr(template_libs_path), ZnkStr_leng( template_libs_path ) * 2 );
		} else {
			ZnkBird_extend( bird, dep_libs, "$[lib_dir]$/$[lib_file]$", 64 ); 
		}
	} else {
		getLibFile( lib_file, template_lib_file, template_lib_ver_sfx, bird,
				ilib_pfx, ZnkStr_cstr( lib_name ), ZnkStr_cstr( lib_dlver ), ilib_sfx );
	
		ZnkBird_regist( bird, "lib_dir",  ZnkStr_cstr( lib_dir ) );
		ZnkBird_regist( bird, "lib_file", ZnkStr_cstr(lib_file) );
		if( is_at_notation ){
			ZnkBird_extend( bird, dep_libs, ZnkStr_cstr(template_libs_path), ZnkStr_leng( template_libs_path ) * 2 );
		} else {
			ZnkBird_extend( bird, dep_libs, "$[lib_dir]$/$[lib_file]$", 64 ); 
		}
	
		getLibFile( lib_file, template_lib_file, template_lib_ver_sfx, bird,
				dlib_pfx, ZnkStr_cstr( lib_name ), ZnkStr_cstr( lib_dlver ), dlib_sfx );
		ZnkBird_regist( bird, "machine", machine );
		ZnkBird_regist( bird, "lib_file", ZnkStr_cstr(lib_file) );
	
		ZnkBird_extend( bird, runtime_file, ZnkStr_cstr(template_runtime_path), ZnkStr_leng( template_runtime_path ) * 2 );
		ZnkStrAry_push_bk_cstr( runtime_list, ZnkStr_cstr( runtime_file ), ZnkStr_leng( runtime_file ) );
	}

	ZnkStr_delete( dep_lib_type );
	ZnkStr_delete( lib_name );
	ZnkStr_delete( lib_dlver );
	ZnkStr_delete( lib_dir );
	ZnkStr_delete( lib_file );
	ZnkStr_delete( runtime_file );
	ZnkBird_destroy( bird );
}

static void
getLinkingLibsSpecial( ZnkStr dep_libs, ZnkVarp linking_libs_special, const char* mkf_id )
{
	if( linking_libs_special ){
		ZnkStrAry  tkns = ZnkStrAry_create( true );
		size_t size;
		size_t idx;
	
		ZnkStrEx_addSplitCSet( tkns,
				ZnkVar_cstr(linking_libs_special), ZnkVar_str_leng(linking_libs_special),
				" \t", 2,
				8 );
	
		size = ZnkStrAry_size( tkns );

		if( size ){
			ZnkStr_add( dep_libs, " " );
		}

		for( idx=0; idx<size; ++idx ){
			const char* val = ZnkStrAry_at_cstr( tkns, idx );
			addDepLibs( dep_libs, mkf_id, val );
		}
		ZnkStrAry_destroy( tkns );
	}
}
static void
getDependencyLibsSpecial( ZnkStr dep_libs, ZnkVarp dependency_libs_special, const char* mkf_id )
{
	if( dependency_libs_special ){
		ZnkStr_addf( dep_libs, " %s", ZnkVar_cstr(dependency_libs_special) );
	}
}

static void
getInstallFileRule( ZnkStr rule, ZnkMyf tmpl_myf, const char* mkf_id, ZnkStrAry install_file_list, ZnkStrAry explicit_dir_list,
		char dsp, const char* target_name, const char* install_dir, bool is_suppress_rule_right, bool is_dos )
{
	size_t size = 0;
	size_t idx;
	ZnkStr    install_file = NULL;
	ZnkStr    install_file_dir = ZnkStr_new( "" );
	ZnkStrAry install_file_dir_list = ZnkStrAry_create( true );
	ZnkStr    install_files = ZnkStr_new( "" );
	ZnkStr    copy_cmd = ZnkStr_new( "" );
	const char* p;
	const char* nl = getNL( mkf_id );
	const char* explicit_dir = NULL;

	ZnkStr_clear( rule );
	ZnkStr_add( rule, target_name );
	ZnkStr_add( rule, ":" );
	if( !is_suppress_rule_right ){
		size = ZnkStrAry_size( install_file_list );
		for( idx=0; idx<size; ++idx ){
			install_file = ZnkStrAry_at( install_file_list, idx );
			ZnkStrPath_replaceDSP( install_file, dsp );
			ZnkStr_addf( rule, " %s", ZnkStr_cstr(install_file) );
		}
	}
	ZnkStr_add( rule, nl );

	size = ZnkStrAry_size( install_file_list );
	for( idx=0; idx<size; ++idx ){
		install_file = ZnkStrAry_at( install_file_list, idx );
		ZnkStrPath_replaceDSP( install_file, dsp );

		ZnkStr_setf( install_file_dir, "%s", install_dir );
		ZnkStrPath_replaceDSP( install_file_dir, dsp );

		explicit_dir = explicit_dir_list ? ZnkStrAry_at_cstr( explicit_dir_list, idx ) : "";
		p = strrchr( ZnkStr_cstr(install_file), dsp );
		if( p ){
			if( ZnkS_empty(explicit_dir) ){
				ZnkStr_add_c( install_file_dir, dsp );
				ZnkStr_append( install_file_dir, ZnkStr_cstr(install_file), p-ZnkStr_cstr(install_file) );
			} else {
				ZnkStr_add_c( install_file_dir, dsp );
				ZnkStr_append( install_file_dir, explicit_dir, Znk_NPOS );
			}
		} else {
			if( ZnkS_empty(explicit_dir) ){
				/* none */
			} else {
				ZnkStr_add_c( install_file_dir, dsp );
				ZnkStr_append( install_file_dir, explicit_dir, Znk_NPOS );
			}
		}

		if( ZnkStrAry_find( install_file_dir_list, 0,
					ZnkStr_cstr(install_file_dir), ZnkStr_leng(install_file_dir) ) == Znk_NPOS )
		{
			ZnkStrAry_push_bk_cstr( install_file_dir_list,
					ZnkStr_cstr(install_file_dir), ZnkStr_leng(install_file_dir) );
		}
		if( is_dos ){
			ZnkStr_addf( copy_cmd, "\t@if exist \"%s\" @$(CP) /F \"%s\" %s\\ $(CP_END)",
					ZnkStr_cstr(install_file), ZnkStr_cstr(install_file), ZnkStr_cstr(install_file_dir) );
		} else {
			/* echo抑止の@は付けない方がむしろわかりやすい */
#if 0
			ZnkStr_addf( copy_cmd, "\tif test -e \"%s\" ; then $(CP) \"%s\" %s/ ; fi",
					ZnkStr_cstr(install_file), ZnkStr_cstr(install_file), ZnkStr_cstr(install_file_dir) );
#else
			/* shにおけるワイルドカード指定に対応. この場合は test -e だと判別できないのでまず forで展開する.  */
			ZnkStr_addf( copy_cmd,
					"\tfor tgt in %s ; do if test -e \"$$tgt\" ; then $(CP) \"$$tgt\" %s/ ; fi ; done",
					ZnkStr_cstr(install_file), ZnkStr_cstr(install_file_dir) );
#endif
		}
		ZnkStr_add( copy_cmd, nl );
	}

	size = ZnkStrAry_size( install_file_dir_list );
	for( idx=0; idx<size; ++idx ){
		ZnkStr dir = ZnkStrAry_at( install_file_dir_list, idx );
		if( is_dos ){
			ZnkStr_addf( rule, "\t@if not exist %s @mkdir %s ",
					ZnkStr_cstr(dir), ZnkStr_cstr(dir) );
		} else {
			/* echo抑止の@は付けない方がむしろわかりやすい */
			ZnkStr_addf( rule, "\tmkdir -p %s ",
					ZnkStr_cstr(dir) );
		}
		ZnkStr_add( rule, nl );
	}

	ZnkStr_add( rule, ZnkStr_cstr(copy_cmd) );

	ZnkStr_delete( install_file_dir );
	ZnkStrAry_destroy( install_file_dir_list );
	ZnkStr_delete( install_files );
	ZnkStr_delete( copy_cmd );
}

static void
getInstallFileAndExplicitDirList( ZnkStrAry install_data_list, ZnkStrAry install_file_list, ZnkStrAry explicit_dir_list )
{
	const size_t size = ZnkStrAry_size( install_data_list );
	size_t idx;
	ZnkStrAry tkns = ZnkStrAry_create( true );
	ZnkStr install_data = NULL;
	for( idx=0; idx<size; ++idx ){
		ZnkStrAry_clear( tkns );
		install_data = ZnkStrAry_at( install_data_list, idx );
		ZnkStrEx_addSplitCSet( tkns,
				ZnkStr_cstr(install_data), ZnkStr_leng(install_data),
				" \t", 2,
				2 );
		if( ZnkStrAry_size(tkns) >= 2 ){
			ZnkStrAry_push_bk_cstr( install_file_list, ZnkStrAry_at_cstr(tkns,0), Znk_NPOS );
			ZnkStrAry_push_bk_cstr( explicit_dir_list, ZnkStrAry_at_cstr(tkns,1), Znk_NPOS );
		} else {
			ZnkStrAry_push_bk_cstr( install_file_list, ZnkStrAry_at_cstr(tkns,0), Znk_NPOS );
			ZnkStrAry_push_bk_cstr( explicit_dir_list, "", 0 );
		}
	}
	ZnkStrAry_destroy( tkns );
}
static void
getInstallDataRule( ZnkStr rule, ZnkMyf tmpl_myf, const char* mkf_id, ZnkStrAry install_data_list,
		const MkfInstall mkf_inst, char dsp, bool is_dos )
{
	ZnkStrAry install_file_list = ZnkStrAry_create( true );
	ZnkStrAry explicit_dir_list = ZnkStrAry_create( true );
	getInstallFileAndExplicitDirList( install_data_list, install_file_list, explicit_dir_list );

	getInstallFileRule( rule, tmpl_myf, mkf_id, install_file_list, explicit_dir_list,
			dsp, "install_data", ZnkStr_cstr(mkf_inst->install_data_dir_), true, is_dos );

	ZnkStrAry_destroy( explicit_dir_list );
	ZnkStrAry_destroy( install_file_list );
}
static void
getInstallExecRule( ZnkStr rule, ZnkMyf tmpl_myf, const char* mkf_id, ZnkStrAry install_exec_list,
		const MkfInstall mkf_inst, char dsp, bool is_dos, bool is_runtime_install )
{
	const char* nl = getNL( mkf_id );

	getInstallFileRule( rule, tmpl_myf, mkf_id, install_exec_list, NULL,
			dsp, "install_exec", ZnkStr_cstr(mkf_inst->install_exec_dir_), false, is_dos );

	ZnkStrPath_replaceDSP( mkf_inst->install_exec_dir_, dsp );
	if( is_runtime_install ){
		if( is_dos ){
			ZnkStr_addf( rule,
					"\t@for %%%%a in ( $(RUNTIME_FILES) ) do @if exist \"%%%%a\" @$(CP) /F \"%%%%a\" %s\\ $(CP_END)",
					ZnkStr_cstr(mkf_inst->install_exec_dir_) );
		} else {
			ZnkStr_addf( rule,
					"\t@for tgt in $(RUNTIME_FILES) ; do if test -e \"$$tgt\" ; then $(CP) \"$$tgt\" %s/ ; fi ; done",
					ZnkStr_cstr(mkf_inst->install_exec_dir_) );
		}
		ZnkStr_add( rule, nl );
	}
}
static void
getInstallDlibRule( ZnkStr rule, ZnkMyf tmpl_myf, const char* mkf_id, ZnkStrAry install_dlib_list,
		const MkfInstall mkf_inst, char dsp, bool is_dos )
{
	getInstallFileRule( rule, tmpl_myf, mkf_id, install_dlib_list, NULL,
			dsp, "install_dlib", ZnkStr_cstr(mkf_inst->install_dlib_dir_), false, is_dos );
}
static void
getInstallSlibRule( ZnkStr rule, ZnkMyf tmpl_myf, const char* mkf_id, ZnkStrAry install_slib_list,
		const MkfInstall mkf_inst, char dsp, bool is_dos )
{
	getInstallFileRule( rule, tmpl_myf, mkf_id, install_slib_list, NULL,
			dsp, "install_slib", ZnkStr_cstr(mkf_inst->install_slib_dir_), false, is_dos );
}
static void
getInstallRule2( ZnkStr rule, const MkfInstall mkf_inst, const char* nl )
{
	ZnkStr_clear( rule );
	ZnkStr_add( rule, "install: all " );
	if( ZnkStrAry_size( mkf_inst->install_slib_list_ ) ){
		ZnkStr_add( rule, "install_slib " );
	}
	if( ZnkStrAry_size( mkf_inst->install_dlib_list_ ) ){
		ZnkStr_add( rule, "install_dlib " );
	}
	if( ZnkStrAry_size( mkf_inst->install_exec_list_ ) ){
		ZnkStr_add( rule, "install_exec " );
	}
	if( ZnkStrAry_size( mkf_inst->install_data_list_ ) ){
		ZnkStr_add( rule, "install_data " );
	}
	ZnkStr_add( rule, nl );
	ZnkStr_add( rule, nl );
}

static ZnkStrAry
createSubMkfList( ZnkStrAry sublibs_list )
{
	const size_t size = ZnkStrAry_size( sublibs_list );
	size_t idx;
	ZnkStrAry tkns        = ZnkStrAry_create( true );
	ZnkStrAry submkf_list = ZnkStrAry_create( true );
	for( idx=0; idx<size; ++idx ){
		const ZnkStr sublib = ZnkStrAry_at( sublibs_list, idx );
		ZnkStrAry_clear( tkns );
		ZnkStrEx_addSplitCSet( tkns,
				ZnkStr_cstr(sublib), ZnkStr_leng(sublib),
				" \t", 2,
				2 );
		ZnkStrAry_push_bk_cstr( submkf_list, ZnkStrAry_at_cstr( tkns, 0 ), Znk_NPOS );
	}
	ZnkStrAry_destroy( tkns );
	return submkf_list;
}

static void
convertNL( ZnkStr str, bool is_dos )
{
	/* DOS => UNIX */
	/* DOSの改行にしたい場合も一旦UNIXの改行に変換しておく.
	 * (さもないと改行コードが混在している場合にうまくいかない) */
	ZnkStrEx_replace_BF( str, 0,
			"\r\n", 2,
			"\n", 1,
			Znk_NPOS, Znk_NPOS );
	if( is_dos ){
		/* UNIX => DOS */
		ZnkStrEx_replace_BF( str, 0,
				"\n", 1,
				"\r\n", 2,
				Znk_NPOS, Znk_NPOS );
	}
}

static void
getLibNameSpecialVAry( ZnkVarpAry lib_name_special_vary, const char* lib_name_special )
{
	ZnkStrAry tkns = ZnkStrAry_create( true );
	ZnkStr    name = ZnkStr_new ( "" );
	size_t  size = 0;
	size_t  idx;
	ZnkVarp varp = NULL;

	ZnkStrEx_addSplitCSet( tkns,
			lib_name_special, Znk_strlen(lib_name_special),
			" \t\r\n", 4,
			4 );
	size = ZnkStrAry_size( tkns );
	for( idx=0; idx<size; ++idx ){
		const char* p = ZnkStrAry_at_cstr( tkns, idx );
		const char* q = Znk_strchr( p, ':' );
		if( q ){
			ZnkStr_assign( name, 0, p, q-p );
			varp = ZnkVarp_create( ZnkStr_cstr(name), "", 0, ZnkPrim_e_Str, NULL );
			ZnkVar_set_val_Str( varp, q+1, Znk_NPOS );
			ZnkVarpAry_push_bk( lib_name_special_vary, varp );
		}
	}

	ZnkStrAry_destroy( tkns );
	ZnkStr_delete( name );
}

static void
generateMkf( ZnkVarpAry info,
		ZnkStrAry list, ZnkStrAry dir_list, const char* mkf_id, ZnkStrAry src_suffix_list,
		ZnkStrAry include_paths_common, ZnkStrAry dependency_libs_common, ZnkStrAry runtime_additional, bool is_runtime_install,
		const char* my_libs_root, const char* template_dir,
		ZnkVarpAry product_list, ZnkStrAry rc_list, ZnkStrAry sublibs_list,
		const char* lib_dlver, MkfInstall mkf_inst, const char* special_exe_sfx, const char* special_lib_pfx,
		const char* resfile_additional, const char* gslconv, bool is_include_makefile_version )
{
	char path[ 256 ];
	ZnkFile fp   = NULL;
	ZnkStr  text = ZnkStr_new( "" );
	ZnkStr  line = ZnkStr_new( "" );
	ZnkBird bird = ZnkBird_create( "$[", "]$" );
	ZnkMyf tmpl_myf = ZnkMyf_create();
	ZnkStrAry runtime_list = ZnkStrAry_create( true );
	const char* machine = "$(MACHINE)";
	const char* nl = getNL( mkf_id );
	bool is_dos = true;
	ZnkStrAry submkf_list = createSubMkfList( sublibs_list );

	if( template_dir == NULL || ZnkS_empty( template_dir ) ){
		template_dir = ".";
	}

	Znk_snprintf( path, sizeof(path), "%s/Makefile_%s.template.mak", template_dir, mkf_id );
	fp = Znk_fopen( path, "rb" );
	if( fp ){
		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				break;
			}
			ZnkStr_add( text, ZnkStr_cstr(line) );
		}
		Znk_fclose( fp );
	}

	Znk_snprintf( path, sizeof(path), "%s/template_%s.myf", template_dir, mkf_id );
	if( !ZnkMyf_load( tmpl_myf, path ) ){
		Znk_printf_e( "Cannot open [%s]\n", path );
	} else {
		ZnkStr rule = ZnkStr_new( "" );
		ZnkStr include_flags = ZnkStr_new( "" );
		ZnkStr dep_libs      = ZnkStr_new( "" );
		ZnkVarpAry tm_conf = ZnkMyf_find_vars( tmpl_myf, "config" );
		ZnkStr template_include_path = ZnkStr_new( "" );
		ZnkStr template_lib_ver_sfx  = ZnkStr_new( "" );
		ZnkStr template_lib_file     = ZnkStr_new( "" );
		ZnkStr template_libs_path    = ZnkStr_new( "" );
		ZnkStr template_runtime_path = ZnkStr_new( "" );
		ZnkStr template_sublib_dir   = ZnkStr_new( "" );


		{
			ZnkVarp varp;
			const char* obj_sfx  = NULL;
			const char* exe_sfx  = NULL;
			const char* slib_pfx = "";
			const char* ilib_pfx = "";
			const char* dlib_pfx = "";
			const char* slib_sfx = "";
			const char* ilib_sfx = "";
			const char* dlib_sfx = "";
			const char* dsp = NULL;
			ZnkStr dep_template_lib_file = NULL;
			ZnkStr new_template_lib_file = NULL;
			ZnkStr my_libs_root_for_shell = ZnkStr_new( my_libs_root );
			ZnkStr gslconv_for_shell      = ZnkStr_new( gslconv );
			ZnkVarpAry lib_name_special_vary = NULL;
			ZnkVarpAry slib_pfx_special_vary = NULL;
			ZnkVarpAry rlib_pfx_special_vary = NULL;

			varp = ZnkVarpAry_find_byName_literal( tm_conf, "obj_sfx", false );
			if( varp ){ obj_sfx = ZnkVar_cstr( varp ); }
			varp = ZnkVarpAry_find_byName_literal( tm_conf, "exe_sfx", false );
			if( varp ){ exe_sfx = ZnkVar_cstr( varp ); }

			varp = ZnkVarpAry_find_byName_literal( tm_conf, "slib_pfx", false );
			if( varp ){ slib_pfx = ZnkVar_cstr( varp ); }
			varp = ZnkVarpAry_find_byName_literal( tm_conf, "ilib_pfx", false );
			if( varp ){ ilib_pfx = ZnkVar_cstr( varp ); }
			varp = ZnkVarpAry_find_byName_literal( tm_conf, "dlib_pfx", false );
			if( varp ){ dlib_pfx = ZnkVar_cstr( varp ); }

			varp = ZnkVarpAry_find_byName_literal( tm_conf, "slib_sfx", false );
			if( varp ){ slib_sfx = ZnkVar_cstr( varp ); }
			varp = ZnkVarpAry_find_byName_literal( tm_conf, "ilib_sfx", false );
			if( varp ){ ilib_sfx = ZnkVar_cstr( varp ); }
			varp = ZnkVarpAry_find_byName_literal( tm_conf, "dlib_sfx", false );
			if( varp ){ dlib_sfx = ZnkVar_cstr( varp ); }

			if( special_exe_sfx ){
				exe_sfx = special_exe_sfx;
			}

			slib_pfx = special_lib_pfx ? special_lib_pfx : slib_pfx;
			ilib_pfx = special_lib_pfx ? special_lib_pfx : ilib_pfx;
			dlib_pfx = special_lib_pfx ? special_lib_pfx : dlib_pfx;

			varp = ZnkVarpAry_find_byName_literal( tm_conf, "template_include_path", false );
			if( varp ){ ZnkStr_set( template_include_path, ZnkVar_cstr( varp ) ); }

			varp = ZnkVarpAry_find_byName_literal( tm_conf, "template_lib_ver_sfx", false );
			if( varp ){ ZnkStr_set( template_lib_ver_sfx, ZnkVar_cstr( varp ) ); }

			varp = ZnkVarpAry_find_byName_literal( tm_conf, "template_lib_file", false );
			if( varp ){ ZnkStr_set( template_lib_file, ZnkVar_cstr( varp ) ); }
			dep_template_lib_file = template_lib_file;
			if( info ){
				ZnkVarp template_lib_file_special = ZnkVarpAry_find_byName_literal( info, "template_lib_file_special", false );
				new_template_lib_file = template_lib_file_special ? ZnkVar_str(template_lib_file_special) : template_lib_file;
			} else {
				new_template_lib_file = template_lib_file;
			}

			varp = ZnkVarpAry_find_byName_literal( tm_conf, "template_libs_path", false );
			if( varp ){ ZnkStr_set( template_libs_path, ZnkVar_cstr( varp ) ); }

			varp = ZnkVarpAry_find_byName_literal( tm_conf, "template_runtime_path", false );
			if( varp ){ ZnkStr_set( template_runtime_path, ZnkVar_cstr( varp ) ); }

			varp = ZnkVarpAry_find_byName_literal( tm_conf, "template_sublib_dir", false );
			if( varp ){ ZnkStr_set( template_sublib_dir, ZnkVar_cstr( varp ) ); }

			{
				ZnkVarp shell = ZnkVarpAry_find_byName_literal( tm_conf, "shell", false );
				if( shell ){
					is_dos = ZnkS_eq( ZnkVar_cstr(shell), "dos" );
				} else {
					is_dos = true;
				}
			}

			dsp = getDSP( is_dos );

			/* replaceDSP : my_libs_root */
			{
				ZnkStrPath_replaceDSP( my_libs_root_for_shell, dsp[0] );
				ZnkBird_regist( bird, "my_libs_root", ZnkStr_cstr(my_libs_root_for_shell) );
			}

			/* replaceDSP : gslconv */
			{
				ZnkStrPath_replaceDSP( gslconv_for_shell, dsp[0] );
			}

			{
				size_t idx;
				size_t size = ZnkStrAry_size( include_paths_common );
				for( idx=0; idx<size; ++idx ){
					const char* include_path = ZnkStrAry_at_cstr( include_paths_common, idx );
					MkfInclude_getIncludeFlag( include_flags, include_path, template_include_path, nl );
				}
				ZnkBird_regist( bird, "include_flags", ZnkStr_cstr(include_flags) );
			}

			if( is_include_makefile_version ){
				ZnkBird_regist( bird, "include_makefile_version", "include Makefile_version.mak" );
			} else {
				ZnkBird_regist( bird, "include_makefile_version", "# include Makefile_version.mak" );
			}

			if( info ){
				ZnkVarp lib_name_special = ZnkVarpAry_findObj_byName_literal( info, "lib_name_special", false );
				if( lib_name_special ){
					lib_name_special_vary = ZnkVarpAry_create( true );
					getLibNameSpecialVAry( lib_name_special_vary, ZnkVar_cstr(lib_name_special) );
				}
			}
			if( info ){
				ZnkVarp slib_pfx_special = ZnkVarpAry_findObj_byName_literal( info, "slib_pfx_special", false );
				ZnkVarp rlib_pfx_special = ZnkVarpAry_findObj_byName_literal( info, "rlib_pfx_special", false );
				if( slib_pfx_special ){
					slib_pfx_special_vary = ZnkVarpAry_create( true );
					getLibNameSpecialVAry( slib_pfx_special_vary, ZnkVar_cstr(slib_pfx_special) );
				}
				if( rlib_pfx_special ){
					rlib_pfx_special_vary = ZnkVarpAry_create( true );
					getLibNameSpecialVAry( rlib_pfx_special_vary, ZnkVar_cstr(rlib_pfx_special) );
				}
			}
			{
				size_t idx;
				size_t size = ZnkStrAry_size( dependency_libs_common );
				for( idx=0; idx<size; ++idx ){
					const char* dependency_lib = ZnkStrAry_at_cstr( dependency_libs_common, idx );
					getDepLib( dep_libs, runtime_list, dependency_lib,
							dep_template_lib_file, template_runtime_path, template_lib_ver_sfx, template_libs_path,
							slib_pfx, ilib_pfx, dlib_pfx,
							slib_sfx, ilib_sfx, dlib_sfx,
							mkf_id, machine, slib_pfx_special_vary, rlib_pfx_special_vary );
					if( idx != size-1 ){
						ZnkStr_add( dep_libs, " " );
					}
				}
			}
			/***
			 * 特殊ランタイムの追加.
			 * (たとえばサードパーティでビルド済みなDLLなどを特別に追加したい場合.
			 */
			{
				size_t idx;
				const size_t size = runtime_additional ? ZnkStrAry_size( runtime_additional ) : 0;
				ZnkStr dummy = ZnkStr_new( "" );
				for( idx=0; idx<size; ++idx ){
					const char* dependency_lib = ZnkStrAry_at_cstr( runtime_additional, idx );
					getDepLib( dummy, runtime_list, dependency_lib,
							dep_template_lib_file, template_runtime_path, template_lib_ver_sfx, template_libs_path,
							slib_pfx, ilib_pfx, dlib_pfx,
							slib_sfx, ilib_sfx, dlib_sfx,
							mkf_id, machine, slib_pfx_special_vary, rlib_pfx_special_vary );
					if( idx != size-1 ){
						ZnkStr_add( dep_libs, " " );
					}
				}
				ZnkStr_delete( dummy );
			}

			ZnkBird_regist( bird, "compiler_option_special", "" );
			if( info ){
				ZnkVarp compiler_option_special = ZnkVarpAry_find_byName_literal( info, "compiler_option_special", false );
				if( compiler_option_special ){
					convertNL( ZnkVar_str( compiler_option_special ), is_dos );
					ZnkBird_regist( bird, "compiler_option_special", ZnkVar_cstr(compiler_option_special) );
				}
				{
					ZnkVarp linking_libs_special = ZnkVarpAry_find_byName_literal( info, "linking_libs_special", false );
					getLinkingLibsSpecial( dep_libs, linking_libs_special, mkf_id );
				}
				{
					ZnkVarp dependency_libs_special = ZnkVarpAry_find_byName_literal( info, "dependency_libs_special", false );
					getDependencyLibsSpecial( dep_libs, dependency_libs_special, mkf_id );
				}
			}

			getSuffixRule( rule, tmpl_myf, mkf_id, dir_list, src_suffix_list, is_dos );
			ZnkBird_regist( bird, "suffix_rule", ZnkStr_cstr(rule) );
	
			getAllTargetRule( rule, dir_list, submkf_list, product_list, mkf_id, is_dos );
			ZnkBird_regist( bird, "all_target_rule", ZnkStr_cstr(rule) );
	
			getMkdirRule( rule, tmpl_myf, mkf_id, dir_list, is_dos );
			ZnkBird_regist( bird, "mkdir_rule", ZnkStr_cstr(rule) );


			getObjsDefs( rule,
					list, product_list, rc_list, sublibs_list, runtime_list,
					new_template_lib_file, template_lib_ver_sfx, template_sublib_dir,
					obj_sfx, exe_sfx,
					slib_pfx, ilib_pfx, dlib_pfx, slib_sfx, ilib_sfx, dlib_sfx,
					lib_dlver, mkf_inst, mkf_id, is_dos,
					slib_pfx_special_vary, rlib_pfx_special_vary );
			ZnkBird_regist( bird, "objs_defs", ZnkStr_cstr(rule) );

			getProductFilesRule( rule, tmpl_myf, mkf_id,
					product_list, rc_list, dep_libs,
					ilib_sfx,
					resfile_additional, ZnkStr_cstr(gslconv_for_shell) );
			ZnkBird_regist( bird, "product_files_rule", ZnkStr_cstr(rule) );

			getRcRule( rule, tmpl_myf, mkf_id, rc_list );
			ZnkBird_regist( bird, "rc_rule", ZnkStr_cstr(rule) );

			getSubmkfRule( rule, tmpl_myf, mkf_id, submkf_list, nl );
			ZnkBird_regist( bird, "submkf_rule", ZnkStr_cstr(rule) );


			getInstallDataRule( rule, tmpl_myf, mkf_id, mkf_inst->install_data_list_, mkf_inst, dsp[0], is_dos );
			ZnkBird_regist( bird, "install_data_rule", ZnkStr_cstr(rule) );

			getInstallExecRule( rule, tmpl_myf, mkf_id, mkf_inst->install_exec_list_, mkf_inst, dsp[0], is_dos, is_runtime_install );
			ZnkBird_regist( bird, "install_exec_rule", ZnkStr_cstr(rule) );

			getInstallDlibRule( rule, tmpl_myf, mkf_id, mkf_inst->install_dlib_list_, mkf_inst, dsp[0], is_dos );
			ZnkBird_regist( bird, "install_dlib_rule", ZnkStr_cstr(rule) );

			getInstallSlibRule( rule, tmpl_myf, mkf_id, mkf_inst->install_slib_list_, mkf_inst, dsp[0], is_dos );
			ZnkBird_regist( bird, "install_slib_rule", ZnkStr_cstr(rule) );

			getInstallRule2( rule, mkf_inst, nl );
			ZnkBird_regist( bird, "install_rule", ZnkStr_cstr(rule) );

			getCleanRule( rule, tmpl_myf, mkf_id, submkf_list, nl );
			ZnkBird_regist( bird, "clean_rule", ZnkStr_cstr(rule) );

			ZnkStr_delete( include_flags );
			ZnkStr_delete( dep_libs );
			ZnkStr_delete( rule );

		
			ZnkBird_extend_self( bird, text, ZnkStr_leng( text ) * 2 );
		
			ZnkStr_delete( line );
			ZnkBird_destroy( bird );

			ZnkStr_delete( template_include_path );
			ZnkStr_delete( template_lib_ver_sfx );
			ZnkStr_delete( template_lib_file );
			ZnkStr_delete( template_libs_path );
			ZnkStr_delete( template_runtime_path );
			ZnkStr_delete( template_sublib_dir );
			ZnkStr_delete( my_libs_root_for_shell );
			ZnkStr_delete( gslconv_for_shell );
			ZnkVarpAry_destroy( lib_name_special_vary );
			ZnkVarpAry_destroy( slib_pfx_special_vary );
			ZnkVarpAry_destroy( rlib_pfx_special_vary );

			ZnkStr_addf( text, "# Src and Headers Dependency%s", nl );
			MkfSrcDepend_get( text, list, obj_sfx, nl, isSrcFileExt );
		}

	
		Znk_snprintf( path, sizeof(path), "Makefile_%s.mak", mkf_id );
		fp = Znk_fopen( path, "wb" );
		if( fp ){
			Znk_fputs( ZnkStr_cstr(text), fp );
			Znk_fclose( fp );
		}
	}

	ZnkStr_delete( text );
	ZnkMyf_destroy( tmpl_myf );
	ZnkStrAry_destroy( runtime_list );
	ZnkStrAry_destroy( submkf_list );
}

static void
generateMkfVersion( const char* makefile_version_landmark )
{
	if( ZnkDir_getType( "Makefile_version.mak" ) != ZnkDirType_e_File ){
		ZnkFile fp = Znk_fopen( "Makefile_version.mak", "wb" );
		if( fp ){
			const size_t depth = 5;
			ZnkStr       include_mkver_dir = ZnkStr_new( "../" );
			if( ZnkStrPath_searchParentDir( include_mkver_dir, depth, "Makefile_version.mak", ZnkDirType_e_File, '/' ) ){
				Znk_fprintf( fp, "include %sMakefile_version.mak", ZnkStr_cstr(include_mkver_dir) );
			} else {
				ZnkStr_set( include_mkver_dir, "../" ); /* reset */
				if( ZnkStrPath_searchParentDir( include_mkver_dir, depth, makefile_version_landmark, ZnkDirType_e_Directory, '/' ) ){
					Znk_fprintf( fp, "include %s%s/Makefile_version.mak", ZnkStr_cstr(include_mkver_dir), makefile_version_landmark );
				}
			}
			Znk_fclose( fp );
			ZnkStr_delete( include_mkver_dir );
		}
	}
}

/**
 * src_ary を except_ary を除き dst_aryへ追加.
 * except_aryがNULLの場合はそのまま追加.
 */
static void
addStrAry_withExcept( ZnkStrAry dst_ary, const ZnkStrAry src_ary, const ZnkStrAry except_ary )
{
	const size_t size = ZnkStrAry_size( src_ary );
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		const char*  src      = ZnkStrAry_at_cstr( src_ary, idx );
		const size_t src_leng = ZnkStrAry_at_leng( src_ary, idx );
		if( except_ary == NULL || ZnkStrAry_find( except_ary, 0, src, src_leng ) == Znk_NPOS ){
			ZnkStrAry_push_bk_cstr( dst_ary, src, src_leng );
		}
	}
}


int main(int argc, char **argv)
{
	ZnkMyf conf_myf = ZnkMyf_create();
	const char* myf_filename = "mkfgen.myf";

	if( !ZnkMyf_load( conf_myf, myf_filename ) ){
		Znk_printf_e( "Cannot open [%s]\n", myf_filename );
	} else {
		ZnkVarpAry vars  = NULL;
		ZnkVarpAry info  = NULL;
		ZnkVarpAry product_list = NULL;
		ZnkStrAry  product_list_sda = NULL;
		ZnkStrAry  lines = NULL;
		ZnkStrAry  include_paths_common   = NULL;
		ZnkStrAry  dependency_libs_common = NULL;
		ZnkStrAry  runtime_additional = NULL;
		ZnkStrAry  src_suffix_list = NULL;
		ZnkStrAry  rc_list = NULL;
		ZnkStrAry  sublibs_list = NULL;
		ZnkStrAry  tkns = NULL;
		ZnkVarp    varp;
		struct MkfInstall_tag mkf_inst = { 0 };
		const char* my_libs_root = "";
		const char* template_dir = "";
		const char* lib_dlver    = "";
		const char* special_exe_sfx = NULL;
		const char* special_lib_pfx = NULL;
		const char* resfile_additional = NULL;
		bool is_runtime_install = true;
		bool is_include_makefile_version = true;
		size_t size;
		size_t idx;
		ZnkStr info_name      = ZnkStr_new( "" );
		ZnkStr only_list_name = ZnkStr_new( "" );
		ZnkStrAry dir_list = ZnkStrAry_create( true );
		ZnkStrAry list     = ZnkStrAry_create( true );
		ZnkStrAry list_mdf = ZnkStrAry_create( true );
		ZnkStrAry ignore_list   = ZnkMyf_find_lines( conf_myf, "ignore_list" );
		ZnkStrAry only_list     = NULL;
		ZnkStrAry only_list_all = ZnkStrAry_create( true );
		const char* gslconv = "$(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe";
		ZnkStr makefile_version_landmark = ZnkStr_new( "libZnk" );

		MkfSeek_listDir( list, dir_list, ".", ignore_list, isInterestExt );

		product_list = ZnkVarpAry_create( true );
		tkns = ZnkStrAry_create( true );

		vars = ZnkMyf_find_vars( conf_myf, "config" );
		varp = ZnkVarpAry_find_byName_literal( vars, "my_libs_root", false );
		my_libs_root = ZnkVar_cstr( varp );

		varp = ZnkVarpAry_find_byName_literal( vars, "template_dir", false );
		template_dir = ZnkVar_cstr( varp );

		varp = ZnkVarpAry_find_byName_literal( vars, "lib_dlver", false );
		if( varp ){
			lib_dlver = ZnkVar_cstr( varp );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "special_exe_sfx", false );
		if( varp ){
			special_exe_sfx = ZnkVar_cstr( varp );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "special_lib_pfx", false );
		if( varp ){
			special_lib_pfx = ZnkVar_cstr( varp );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "resfile_additional", false );
		if( varp ){
			resfile_additional = ZnkVar_cstr( varp );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "runtime_install", false );
		if( varp ){
			is_runtime_install = ZnkS_eq( ZnkVar_cstr( varp ), "true" );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "include_makefile_version", false );
		if( varp ){
			is_include_makefile_version = ZnkS_eq( ZnkVar_cstr( varp ), "true" );
		}
		varp = ZnkVarpAry_find_byName_literal( vars, "makefile_version_landmark", false );
		if( varp ){
			ZnkStr_set( makefile_version_landmark, ZnkVar_cstr( varp ) );
		}
		/***
		 * gslconvの位置を特別に指定したい場合.
		 * gslconvがMKFSYS_DIRにインストールされている前の段階であるlibZnkビルド時での使用を想定しているものと考えてよい.
		 */
		varp = ZnkVarpAry_find_byName_literal( vars, "gslconv", false );
		if( varp ){
			//ZnkStr_set( gslconv, ZnkVar_cstr( varp ) );
			gslconv = ZnkVar_cstr( varp );
		}

		MkfInstall_compose( &mkf_inst, conf_myf );

		include_paths_common   = ZnkMyf_find_lines( conf_myf, "include_paths_common" );
		dependency_libs_common = ZnkMyf_find_lines( conf_myf, "dependency_libs_common" );
		runtime_additional     = ZnkMyf_find_lines( conf_myf, "runtime_additional" );

		src_suffix_list   = ZnkMyf_find_lines( conf_myf, "src_suffix_list" );
		rc_list           = ZnkMyf_find_lines( conf_myf, "rc_list" );
		product_list_sda  = ZnkMyf_find_lines( conf_myf, "product_list" );
		//submkf_list       = ZnkMyf_find_lines( conf_myf, "submkf_list" );
		sublibs_list      = ZnkMyf_find_lines( conf_myf, "sublibs_list" );

		size = ZnkStrAry_size( product_list_sda );
		for( idx=0; idx<size; ++idx ){
			const char* product = ZnkStrAry_at_cstr( product_list_sda, idx );
			const char* type = NULL;
			const char* name = NULL;
			const char* mainsrc = NULL;
			ZnkStrAry_clear( tkns );
			ZnkStrEx_addSplitCSet( tkns,
					product, Znk_strlen(product),
					" \t", 2,
					4 );
			type    = ZnkStrAry_at_cstr( tkns, 0 );
			name    = ZnkStrAry_at_cstr( tkns, 1 );
			mainsrc = ZnkStrAry_at_cstr( tkns, 2 );
			varp = ZnkVarp_create( name, "", 0, ZnkPrim_e_StrAry, NULL );
			ZnkStrAry_push_bk_cstr( varp->prim_.u_.sda_, type, Znk_NPOS );
			ZnkStrAry_push_bk_cstr( varp->prim_.u_.sda_, name, Znk_NPOS );
			ZnkStrAry_push_bk_cstr( varp->prim_.u_.sda_, mainsrc, Znk_NPOS );
			ZnkVarpAry_push_bk( product_list, varp );
		}

		lines = ZnkMyf_find_lines( conf_myf, "mkid_list" );
		size = ZnkStrAry_size( lines );

		/* init only_list_all */
		for( idx=0; idx<size; ++idx ){
			const char* mkf_id = ZnkStrAry_at_cstr( lines, idx );
			ZnkStr_setf( only_list_name, "only_list_%s", mkf_id );
			only_list = ZnkMyf_find_lines( conf_myf, ZnkStr_cstr(only_list_name) );
			if( only_list ){
				size_t only_idx;
				size_t only_size = ZnkStrAry_size( only_list );
				for( only_idx=0; only_idx<only_size; ++only_idx ){
					const char* only = ZnkStrAry_at_cstr( only_list, only_idx );
					MkfSSet_add( only_list_all, only );
				}
			}
		}

		for( idx=0; idx<size; ++idx ){
			const char* mkf_id = ZnkStrAry_at_cstr( lines, idx );

			ZnkStr_setf( info_name, "info_%s", mkf_id );
			info = ZnkMyf_find_vars( conf_myf, ZnkStr_cstr(info_name) );

			/* list から only_list_all を除き、list_mdfへコピー. */
			ZnkStrAry_clear( list_mdf );
			addStrAry_withExcept( list_mdf, list, only_list_all );

			/* list_mdfへ only_list を追加. */
			ZnkStr_setf( only_list_name, "only_list_%s", mkf_id );
			only_list = ZnkMyf_find_lines( conf_myf, ZnkStr_cstr(only_list_name) );
			if( only_list ){
				addStrAry_withExcept( list_mdf, only_list, NULL );
			}

			ZnkStr_setf( only_list_name, "only_list_%s", mkf_id );
			only_list = ZnkMyf_find_lines( conf_myf, ZnkStr_cstr(only_list_name) );

			MkfInstall_clearList_atMkfGenerate( &mkf_inst );
			if( ZnkS_eq( mkf_id, "android" ) ){
				MkfAndroid_generate( conf_myf, product_list, isSrcFileExt,
						include_paths_common,
						dependency_libs_common, runtime_additional,
						sublibs_list );
			}
			generateMkf( info, list_mdf, dir_list, mkf_id, src_suffix_list,
					include_paths_common, dependency_libs_common, runtime_additional, is_runtime_install,
					my_libs_root, template_dir, product_list, rc_list, sublibs_list,
					lib_dlver, &mkf_inst, special_exe_sfx, special_lib_pfx,
					resfile_additional, gslconv, is_include_makefile_version );

		}

		if( is_include_makefile_version ){
			generateMkfVersion( ZnkStr_cstr(makefile_version_landmark) );
		}

		ZnkVarpAry_destroy( product_list );
		ZnkStrAry_destroy( tkns );
		ZnkStr_delete( info_name );
		ZnkStr_delete( only_list_name );
		ZnkStr_delete( makefile_version_landmark );
		ZnkStrAry_destroy( list );
		ZnkStrAry_destroy( list_mdf );
		ZnkStrAry_destroy( only_list_all );
		ZnkStrAry_destroy( dir_list );
		MkfInstall_dispose( &mkf_inst );
	}

	ZnkMyf_destroy( conf_myf );
	return 0;
}
