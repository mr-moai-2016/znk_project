#include <Znk_dir.h>
#include <Znk_dir_recursive.h>
#include <Znk_missing_libc.h>
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <Znk_str_path.h>
#include <Znk_str_fio.h>
#include <Znk_str_ary.h>
#include <Znk_myf.h>

#include <Rano_file_info.h>
#include <Rano_dir_util.h>

static const char*
getOneArg( ZnkStr arg, const char* p )
{
	const char* q;
	p = Znk_strstr( p, "['" );
	if( p ){
		p += 2;
		q = Znk_strstr( p, "']" );
		if( q ){
			ZnkStr_assign( arg, 0, p, q-p );
			return q + 2;
		}
	}
	return NULL;
}

static bool
copyPatch( ZnkStrAry platform_deps, ZnkStrAry scr_common, ZnkStrAry scr_platform,
		const char* new_dir, const char* common_dir, const char* platform_dir, const char* under_path, ZnkStr ermsg )
{
	bool   result = false;
	bool   is_platform_dep = false;
	ZnkStr src_path = ZnkStr_new( "" );
	ZnkStr dst_path = ZnkStr_new( "" );

	if( platform_deps && ZnkStrAry_find_isMatch( platform_deps, 0, under_path, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
		is_platform_dep = true;
	}
	ZnkStr_setf( src_path, "%s/%s", new_dir, under_path );
	ZnkStr_setf( dst_path, "%s/%s", is_platform_dep ? platform_dir : common_dir, under_path );
	ZnkStrAry_push_bk_snprintf( is_platform_dep ? scr_platform : scr_common, Znk_NPOS, "copy ['%s']", under_path );
	if( !RanoDirUtil_mkdirOfTailContain( ZnkStr_cstr(dst_path), ermsg ) ){
		ZnkStr_addf( ermsg, "# Error : RanoDirUtil_mkdirOfTailContain : ['%s']\n", ZnkStr_cstr(dst_path) );
		goto FUNC_END;
	}
	if( !ZnkDir_copyFile_byForce( ZnkStr_cstr(src_path), ZnkStr_cstr(dst_path), ermsg ) ){
		ZnkStr_addf( ermsg, "# copy ['%s'] ['%s']\n",
				ZnkStr_cstr(src_path), ZnkStr_cstr(dst_path) );
		goto FUNC_END;
	}
	result = true;
FUNC_END:
	ZnkStr_delete(src_path);
	ZnkStr_delete(dst_path);
	return result;
}
static bool
cmdPatch( ZnkStrAry platform_deps, ZnkStrAry scr_common, ZnkStrAry scr_platform, const char* cmd, const char* under_path )
{
	bool   result = false;
	bool   is_platform_dep = false;

	if( platform_deps && ZnkStrAry_find_isMatch( platform_deps, 0, under_path, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
		is_platform_dep = true;
	}
	ZnkStrAry_push_bk_snprintf( is_platform_dep ? scr_platform : scr_common, Znk_NPOS, "%s ['%s']", cmd, under_path );
	return result;
}

size_t
Pat_make( const char* pmk_file, const char* common_dir, const char* platform_dir, const char* platform_id, ZnkMyf config, ZnkStr ermsg )
{
	size_t err_count = 0;
	ZnkFile fp = Znk_fopen( pmk_file, "rb" );
	if( fp ){
		ZnkMyf apl  = ZnkMyf_create();
		ZnkStr line = ZnkStr_new( "" );
		ZnkStr cmd  = ZnkStr_new( "" );
		ZnkStr arg1 = ZnkStr_new( "" );
		ZnkStrAry scr_common   = ZnkStrAry_create( true );
		ZnkStrAry scr_platform = ZnkStrAry_create( true );
		ZnkStr dst_path     = ZnkStr_new( "" );
		ZnkStr new_dir_str  = ZnkStr_new( "" );
		size_t cmd_leng = 0;
		size_t pos = 0;
		ZnkStrAry platform_deps = ZnkMyf_find_lines( config, "platform_deps" );
		while( ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			ZnkStr_chompNL( line );
			pos = ZnkS_lfind_arg( ZnkStr_cstr(line), 0, ZnkStr_leng(line),
					0, &cmd_leng,
					" \t", 2 );
			if( pos != Znk_NPOS ){
				const char* p = ZnkStr_cstr(line) + pos;
				ZnkStr_assign( cmd, 0, p, cmd_leng );
				p += cmd_leng;

				if( ZnkStr_eq( cmd, "set_new_dir" ) ){
					p = getOneArg( arg1, p );
					if( p ){
						ZnkStr_set( new_dir_str, ZnkStr_cstr(arg1) );
					}
				} else if( ZnkStr_eq( cmd, "add" ) ){
					p = getOneArg( arg1, p );
					if( p ){
						const char* new_dir    = ZnkStr_cstr(new_dir_str);
						const char* under_path = ZnkStr_cstr(arg1);
						if( !copyPatch( platform_deps, scr_common, scr_platform,
								new_dir, common_dir, platform_dir, under_path, ermsg ) )
						{
							++err_count;
						}
					}
				} else if( ZnkStr_eq( cmd, "mkdir" ) ){
					p = getOneArg( arg1, p );
					if( p ){
						const char* under_path = ZnkStr_cstr(arg1);
						cmdPatch( platform_deps, scr_common, scr_platform, ZnkStr_cstr(cmd), under_path );
					}
				} else if( ZnkStr_eq( cmd, "update" ) ){
					p = getOneArg( arg1, p );
					if( p ){
						const char* new_dir    = ZnkStr_cstr(new_dir_str);
						const char* under_path = ZnkStr_cstr(arg1);
						if( !copyPatch( platform_deps, scr_common, scr_platform,
								new_dir, common_dir, platform_dir, under_path, ermsg ) )
						{
							++err_count;
						}
					}
				} else if( ZnkStr_eq( cmd, "delete" ) ){
					p = getOneArg( arg1, p );
					if( p ){
						const char* under_path = ZnkStr_cstr(arg1);
						cmdPatch( platform_deps, scr_common, scr_platform, ZnkStr_cstr(cmd), under_path );
					}
				} else if( ZnkStr_eq( cmd, "rmdir" ) ){
					p = getOneArg( arg1, p );
					if( p ){
						const char* under_path = ZnkStr_cstr(arg1);
						cmdPatch( platform_deps, scr_common, scr_platform, ZnkStr_cstr(cmd), under_path );
					}
				}
				//while( *p == ' ' || *p == '\t' ){ ++p; } 
			}
		}

		if( !ZnkMyf_load( apl, "apply_list.myf" ) ){
			ZnkMyf_set_quote( apl, "['", "']" );
		}
		{
			ZnkStrAry common   = ZnkMyf_intern_lines( apl, "common" );
			ZnkStrAry platform = ZnkMyf_intern_lines( apl, platform_id );
			if( common ){
				ZnkStrAry_copy( common,   scr_common );
			}
			if( platform ){
				ZnkStrAry_copy( platform, scr_platform );
			}
		}
		ZnkMyf_save( apl, "apply_list.myf" );

		Znk_fclose( fp );
		ZnkStr_delete( cmd );
		ZnkStr_delete( arg1 );
		ZnkStr_delete( dst_path );
		ZnkStr_delete( new_dir_str );
		ZnkStr_delete( line );
		ZnkStrAry_destroy( scr_common );
		ZnkStrAry_destroy( scr_platform );
		ZnkMyf_destroy( apl );
	}
	return err_count;
}

static bool
getPathPart( const char* line, size_t* begin, size_t* end )
{
	const char* p1 = Znk_strchr( line, ' ' );
	const char* p2 = NULL;
	if( p1 ){
		p1 = Znk_strstr( p1, "['" );
		if( p1 ){
			p1 += 2;
			p2 = Znk_strstr( p1, "']" );
			if( p2 ){
				*begin = p1 - line;
				*end   = p2 - line;
				return true;
			}
		}
	}
	return false;
}

static bool
isMatchLine( const char* ptn, size_t ptn_leng, const char* query, size_t query_leng )
{
	size_t begin1 = 0;
	size_t end1   = 0;
	size_t begin2 = 0;
	size_t end2   = 0;

	if( !getPathPart( ptn, &begin1, &end1 ) ){
		return false;
	}
	if( !getPathPart( query, &begin2, &end2 ) ){
		return false;
	}
	if( end1-begin1 != end2-begin2 ){
		return false;
	}
	return ZnkS_eqEx( ptn+begin1, query+begin2, end1-begin1 );
}

static void
margeSection( ZnkMyf old_myf, ZnkMyf new_myf, const char* section_name )
{
	ZnkStrAry old_sec = ZnkMyf_find_lines( old_myf, section_name );
	ZnkStrAry new_sec = ZnkMyf_find_lines( new_myf, section_name );
	if( old_sec && new_sec ){
		ZnkStrAry new_add = ZnkStrAry_create( true );
		size_t    old_idx;
		size_t    new_idx;
		size_t    old_size = ZnkStrAry_size( old_sec );
		for( old_idx=0; old_idx<old_size; ++old_idx ){
			ZnkStr old_line = ZnkStrAry_at( old_sec, old_idx );
			new_idx = ZnkStrAry_find_isMatch( new_sec, 0, ZnkStr_cstr(old_line), ZnkStr_leng(old_line),
					isMatchLine );
			if( new_idx == Znk_NPOS ){
				ZnkStrAry_push_bk_cstr( new_add, ZnkStr_cstr(old_line), ZnkStr_leng(old_line) );
			}
		}
		ZnkStrAry_append( new_sec, new_add, 0, Znk_NPOS );
		ZnkStrAry_destroy( new_add );
	}
}

void
Pat_marge( const char* old_file, const char* new_file, const char* save_file )
{
	ZnkMyf old_myf = ZnkMyf_create();
	ZnkMyf new_myf = ZnkMyf_create();
	size_t sec_num;
	size_t sec_idx;

	ZnkMyf_load( old_myf, old_file );
	ZnkMyf_load( new_myf, new_file );

	sec_num = ZnkMyf_numOfSection( new_myf );
	for( sec_idx=0; sec_idx<sec_num; ++sec_idx ){
		ZnkMyfSection sec      = ZnkMyf_atSection( new_myf, sec_idx );
		const char*   sec_name = ZnkMyfSection_name( sec );
		margeSection( old_myf, new_myf, sec_name );
	}

	ZnkMyf_save( new_myf, save_file );

	ZnkMyf_destroy( old_myf );
	ZnkMyf_destroy( new_myf );
}


