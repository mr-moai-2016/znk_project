#include <Est_bat_operator.h>
#include <Znk_s_base.h>
#include <Znk_htp_util.h>
#include <Znk_dir.h>

static void
unescape_forMbcFSysPath( ZnkStr fsys_path, ZnkStr tmp )
{
	ZnkStr_clear( tmp );
	ZnkHtpURL_unescape_toStr( tmp, ZnkStr_cstr(fsys_path), ZnkStr_leng(fsys_path) );
	ZnkStr_swap( tmp, fsys_path );
}

size_t
EstBatOperator_processBox( const ZnkVarpAry given_vars, ZnkStr msg,
		EstBatOperator_ProcessFunc proc_dir_func, 
		EstBatOperator_ProcessFunc proc_file_func, 
		void* param )
{
	const size_t     given_vars_size = ZnkVarpAry_size( given_vars );
	const ZnkVarp*   given_vars_ptr  = ZnkVarpAry_ary_ptr( given_vars );
	ZnkVarp          given_varp      = NULL;
	const char*      vpath           = NULL;
	ZnkStr           unesc_vpath     = ZnkStr_new( "" );
	ZnkStr           fsys_path       = ZnkStr_new( "" );
	ZnkStr           tmp             = ZnkStr_new( "" );
	EstBoxDirType    ast_dir_type    = EstBoxDir_e_Unknown;
	size_t           box_path_offset = 0;
	size_t           processed_count = 0;
	size_t           idx;

	//bool             is_err_ignore = false;
	//ZnkDir_mkdirPath( "dustbox", Znk_NPOS, '/', NULL );

	for( idx=0; idx<given_vars_size; ++idx ){
		const char* var_name = NULL;
		given_varp = given_vars_ptr[ idx ];
		ZnkStr_clear( unesc_vpath );
		var_name = ZnkVar_name_cstr( given_varp );
		if( ZnkS_isBegin( var_name, "cache_dir" )){
			/* dir : cache_dir */
			vpath = ZnkVar_cstr( given_varp );
			ZnkHtpURL_unescape_toStr( unesc_vpath, vpath, Znk_strlen(vpath) );
			ZnkHtpURL_sanitizeReqUrpDir( unesc_vpath, true );
			ast_dir_type = EstBoxBase_convertBoxDir_toFSysDir( fsys_path, ZnkStr_cstr(unesc_vpath), &box_path_offset, msg );
			unescape_forMbcFSysPath( fsys_path, tmp );
			if( ast_dir_type == EstBoxDir_e_Unknown ){
				ZnkStr_addf( msg, "EstBatOperator_processBox : cache_dir : vpath=[%s] is invalid.\n", ZnkStr_cstr(unesc_vpath) );
			} else {
				if( ZnkDir_getType( ZnkStr_cstr(fsys_path) ) == ZnkDirType_e_Directory ){

					if( proc_dir_func &&
						proc_dir_func( ast_dir_type, ZnkStr_cstr(fsys_path), ZnkStr_cstr(unesc_vpath), box_path_offset, msg, param ) )
					{
						++processed_count;
					}
				} else {
					ZnkStr_addf( msg, "  verify : fsys_path=[%s] is not directory.\n", ZnkStr_cstr(fsys_path) );
				}
			}
		} else if( ZnkS_isBegin( var_name, "cache_file" ) || ZnkS_eq( var_name, "cache_path" ) || ZnkS_isBegin( var_name, "md5ext_" ) ){
			/* file : cache_file */
			vpath = ZnkVar_cstr( given_varp );
			ZnkHtpURL_unescape_toStr( unesc_vpath, vpath, Znk_strlen(vpath) );
			ZnkHtpURL_sanitizeReqUrpDir( unesc_vpath, true );
			ast_dir_type = EstBoxBase_convertBoxDir_toFSysDir( fsys_path, ZnkStr_cstr(unesc_vpath), &box_path_offset, msg );
			unescape_forMbcFSysPath( fsys_path, tmp );
			if( ast_dir_type == EstBoxDir_e_Unknown ){
				ZnkStr_addf( msg, "EstBatOperator_processBox : cache_file : vpath=[%s] is invalid.\n", ZnkStr_cstr(unesc_vpath) );
			} else {
				if( ZnkDir_getType( ZnkStr_cstr(fsys_path) ) == ZnkDirType_e_File ){
					if( proc_file_func &&
						proc_file_func( ast_dir_type, ZnkStr_cstr(fsys_path), ZnkStr_cstr(unesc_vpath), box_path_offset, msg, param ) )
					{
						++processed_count;
					}
				} else {
					ZnkStr_addf( msg, "  verify : ZnkStr_cstr(fsys_path)=[%s] is not file.\n", ZnkStr_cstr(fsys_path) );
				}
			}
		} else {
			/* others : –³ŠÖŒW‚Èpost•Ï”‚Å‚ ‚é‚½‚ßA‰½‚à‚¹‚¸ƒXƒ‹[‚µ‚È‚¯‚ê‚Î‚È‚ç‚È‚¢. */
		}
	}

	ZnkStr_delete( unesc_vpath );
	ZnkStr_delete( fsys_path );
	ZnkStr_delete( tmp );
	return processed_count;
}
