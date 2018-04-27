#include <Est_search_manager.h>
#include <Est_boxmap_viewer.h>
#include <Est_ui.h>
#include <Est_box_ui.h>
#include <Est_record.h>
#include <Est_config.h>
#include <Est_assort.h>
#include <Est_assort_ui.h>
#include <Est_hint_manager.h>
#include <Est_base.h>
#include <Est_finf.h>
#include <Est_box_base.h>
#include <Est_unid.h>

#include <Rano_file_info.h>

#include <Znk_htp_util.h>
#include <Znk_dir.h>
#include <Znk_dir_recursive.h>
#include <Znk_missing_libc.h>
#include <Znk_date.h>
#include <Znk_mem_find.h>
#include <Znk_str_fio.h>
#include <Znk_thread.h>
#include <Znk_str_ex.h>
#include <Znk_stdc.h>

#include <stdio.h>
#include <time.h>

#define IS_OK( val ) (bool)( (val) != NULL )

Znk_INLINE double
Time_getCurrentSec( void ){ return ( 1.0 / CLOCKS_PER_SEC ) * clock(); }

static void
issuedSearchedKey( char* issued_searched_key, size_t issued_searched_key_size )
{
	EstUNID_issue( issued_searched_key, issued_searched_key_size );
}
static void
getSearchedBoxShowFilename( const char* topics_dir, char* show_filename, size_t show_filename_size, const char* issued_searched_key, bool from_topic )
{
	if( from_topic ){
		Znk_snprintf( show_filename, show_filename_size, "%s/searched_%s.myf", topics_dir, issued_searched_key );
	} else {
		Znk_snprintf( show_filename, show_filename_size, "%s/tmp/searched/searched_%s.myf", topics_dir, issued_searched_key );
	}
}

static size_t 
findValidPath( const char* path )
{
	static const char* dir_table[] = {
		"cachebox",
		"dustbox",
		"favorite",
		"stockbox",
		"userbox",
	};
	const char* dir;
	size_t idx;
	for( idx=0; idx<Znk_NARY(dir_table); ++idx ){
		dir = dir_table[ idx ];
		if( ZnkS_eq( path, dir ) ){
			return idx;
		}
		if( ZnkS_isBegin( path, dir ) ){
			if( path[ Znk_strlen(dir) ] == '/' ){
				return idx;
			}
		}
	}
	return Znk_NPOS;
}

#if 0
static bool
grepFile( const char* file_path, ZnkStr keyword, const size_t* keyword_occ_tbl )
{
	ZnkFile fp = Znk_fopen( file_path, "rb" );
	bool result = false;
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		size_t pos = Znk_NPOS;
		while( ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			pos = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(line), ZnkStr_leng(line),
					(uint8_t*)ZnkStr_cstr(keyword), ZnkStr_leng(keyword), 1, keyword_occ_tbl );
			if( pos != Znk_NPOS ){
				result = true;
				break;
			}
		}
		Znk_fclose( fp );
		ZnkStr_delete( line );
	}
	return result;
}
#endif

typedef struct {
	size_t      new_idx_;
	EstSQI      sqi_;
	ZnkVarpAry  finf_list_;
	ZnkStr      msg_;
	ZnkMyf      search_base_;
	const char* search_key_;
	const char* src_dir_;
	const char* box_vname_;
} SearchInfo;

static ZnkFile
fopenStateDat( const char* search_key )
{
	char file_path[ 256 ] = "";
	ZnkDir_mkdirPath( "publicbox/state", Znk_NPOS, '/', NULL );
	Znk_snprintf( file_path, sizeof(file_path), "publicbox/state/%s.dat", search_key );
	return Znk_fopen( file_path, "wb" );
}

static size_t st_dir_max = 0;
static size_t st_dir_count = 0;
static size_t st_file_count = 0;

static bool count_onEnterDir( ZnkDirRecursive recur, const char* file_path, void* arg, size_t local_err_num )
{
	++st_dir_max;
	{
		SearchInfo* search_info = Znk_force_ptr_cast( SearchInfo*, arg );
		ZnkFile fp = fopenStateDat( search_info->search_key_ );
		if( fp ){
			Znk_fprintf( fp, "Counting : directory %zu", st_dir_max );
			Znk_fclose( fp );
		}
	}
	return true;
}
#if 0
static bool search_onEnterDir( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	++st_dir_count;
	{
		SearchInfo* search_info = Znk_force_ptr_cast( SearchInfo*, arg );
		ZnkFile fp = fopenStateDat( search_info->search_key_ );
		if( fp ){
			Znk_fprintf( fp, "Searching : file=%zu : directory=%zu/%zu", st_file_count, st_dir_count, st_dir_max );
			Znk_fclose( fp );
		}
	}
	return true;
}

static bool
isFileExt( const ZnkStrAry file_ext, const char* query_ext )
{
	const size_t size = ZnkStrAry_size( file_ext );
	if( size ){
		const char* ext = NULL;
		size_t idx;
		for( idx=0; idx<size; ++idx ){
			ext = ZnkStrAry_at_cstr( file_ext, idx );
			if( ZnkS_eqCase( ext, query_ext ) ){
				/* found */
				return true;
			}
		}
		/* not found */
		return false;
	}
	/* この場合に限り、全ての拡張子が対象 */
	return true;
}
#endif

static bool
isEqExt( const char* s1, size_t s1_leng, const char* s2, size_t s2_leng )
{
	Znk_UNUSED( s1_leng );
	Znk_UNUSED( s2_leng );
	return ZnkS_eqCase( s1, s2 );
}
static void
searchInMyf( SearchInfo* search_info, EstSQI sqi, ZnkMyf myf, const char* cur_dir, const ZnkStrAry query_file_ext )
{
	char vpath[ 256 ] = "";
	size_t num_of_sec = ZnkMyf_numOfSection( myf );
	size_t sec_idx;
	ZnkMyfSection sec = NULL;
	const char*   sec_name = NULL;
	ZnkStrAry tags_list = ZnkStrAry_create( true );
	const ZnkStrAry sqi_tags = EstSQI_tags( sqi );
	const ZnkDate   sqi_date = EstSQI_date( sqi );
	const size_t    sqi_size = EstSQI_size( sqi );
	const char*     sqi_keyword = EstSQI_keyword( sqi );
	const size_t    sqi_keyword_leng = Znk_strlen( sqi_keyword );
	const size_t    qtg_size = ZnkStrAry_size( sqi_tags );
	const size_t    sqi_tags_num_min = EstSQI_tags_num_min( sqi );
	const size_t    sqi_tags_num_max = EstSQI_tags_num_max( sqi );
	const bool      sqi_tags_method_or = EstSQI_is_tags_method_or( sqi );
	ZnkStr comment = ZnkStr_new( "" );
	ZnkOccTable_D( keyword_occ_tbl );

	/* 明らかな不適合 */
	if( qtg_size == 0 && sqi_tags_method_or ){
		/***
		 * タグ指定が空でOR検索の場合、それは何にもマッチしないことを示す.
		 */
		goto FUNC_END;
	}

	ZnkMem_getLOccTable_forBMS( keyword_occ_tbl, (uint8_t*)sqi_keyword, sqi_keyword_leng );

	for( sec_idx=0; sec_idx<num_of_sec; ++sec_idx ){
		sec      = ZnkMyf_atSection( myf, sec_idx );
		sec_name = ZnkMyfSection_name( sec );
		if( ZnkS_eq( sec_name, "sqi_vars" ) ){
			/* SQI-Section */
		} else {
			ZnkVarpAry vars = ZnkMyfSection_vars( sec );
			ZnkVarp var = NULL;
			const char* file_tags   = "";
			const char* access_time_str = NULL;
			const char* file_size_str   = NULL;
			ZnkDate access_date = { 0 };
			uint64_t file_size64u = 0;

			if( query_file_ext ){
				const char* file_ext = ZnkS_get_extension( sec_name, '.' );
				const size_t matched_idx = ZnkStrAry_find_isMatch( query_file_ext, 0, file_ext, Znk_NPOS, isEqExt );
				if( matched_idx == Znk_NPOS ){
					continue; /* 不適合 */
				}
			}
		
			var = ZnkVarpAry_find_byName_literal( vars, "file_path", false );
			if( var ){
				ZnkS_copy( vpath, sizeof(vpath), ZnkVar_cstr( var ), ZnkVar_str_leng( var ) );
			} else {
				Znk_snprintf( vpath, sizeof(vpath), "%s/%s/%s", search_info->box_vname_, cur_dir+Znk_strlen(search_info->src_dir_)+1, sec_name );
			}
		
			/* file_tagsによる篩 */
#if 0
			if( qtg_size ){
				var = ZnkVarpAry_find_byName_literal( vars, "file_tags", false );
				if( var ){
					file_tags = ZnkVar_cstr( var );
					if( ZnkS_empty( file_tags ) ){
						continue; /* 不適合 */
					} else {
						bool found = false;
						ZnkStrAry_clear( tags_list );
						ZnkStrEx_addSplitC( tags_list,
								file_tags, Znk_NPOS,
								' ', false, 4 );
						found = EstSQI_isMatchTags( sqi, tags_list );
						if( !found ){
							continue; /* 不適合 */
						}
					}
				}
			} else {
				var = ZnkVarpAry_find_byName_literal( vars, "file_tags", false );
				if( var ){
					file_tags = ZnkVar_cstr( var );
				}
			}
#else
			if( qtg_size == 0 ){
				size_t tags_list_num = 0;
				/***
				 * 高速化のためにこの場合は特別に処理.
				 * この場合のOR検索の可能性は除しているためここは必ずAND検索である.
				 * このときすべてのタグ指定にマッチする.
				 */
				var = ZnkVarpAry_find_byName_literal( vars, "file_tags", false );
				ZnkStrAry_clear( tags_list );
				if( var ){
					file_tags = ZnkVar_cstr( var );
					if( ZnkS_empty( file_tags ) ){
						tags_list_num = 0;
					} else {
						ZnkStrEx_addSplitC( tags_list,
								file_tags, Znk_NPOS,
								' ', false, 4 );
						tags_list_num = ZnkStrAry_size( tags_list );
					}
				} else {
					/* tag指定の個数が0のケースとみなす */
					tags_list_num = 0;
				}
				if( tags_list_num < sqi_tags_num_min || tags_list_num > sqi_tags_num_max ){
					continue; /* 不適合 */
				}
			} else {
				/***
				 * 完全なスキャン.
				 */
				bool found = false;
				var = ZnkVarpAry_find_byName_literal( vars, "file_tags", false );
				ZnkStrAry_clear( tags_list );
				if( var ){
					file_tags = ZnkVar_cstr( var );
					if( !ZnkS_empty( file_tags ) ){
						ZnkStrEx_addSplitC( tags_list,
								file_tags, Znk_NPOS,
								' ', false, 4 );
					}
				} else {
					/* tag指定の個数が0のケースとみなす */
				}
				found = EstSQI_isMatchTags( sqi, tags_list );
				if( !found ){
					continue; /* 不適合 */
				}
			}
#endif
		
			if( ZnkDate_year( &sqi_date ) != 0 ){
				var = ZnkVarpAry_find_byName_literal( vars, "access_time", false );
				if( var == NULL ){
					var = ZnkVarpAry_find_byName_literal( vars, "file_time", false );
				}
				if( var ){
					access_time_str = ZnkVar_cstr( var );
					if( ZnkDate_scanStr( &access_date, access_time_str, ZnkDateStr_e_Std ) ){
						if( !EstSQI_isMatchDate( sqi, &access_date ) ){
							continue; /* 不適合 */
						}
						/* 合格 */
					}
				}
			}
		
			if( sqi_size != Znk_NPOS ){
				var = ZnkVarpAry_find_byName_literal( vars, "file_size", false );
				if( var ){
					file_size_str = ZnkVar_cstr(var);
					if( ZnkS_getU64U( &file_size64u, ZnkVar_cstr(var) ) ){
						if( !EstSQI_isMatchSize( sqi, file_size64u ) ){
							continue; /* 不適合 */
						}
					}
					/* 合格 */
				}
			}
		
			/* コメントの取得(必須) */
			{
				var = ZnkVarpAry_find_byName_literal( vars, "comment", false );
				if( var ){
					ZnkStr_set( comment, ZnkVar_cstr( var ) );
				} else {
					ZnkStr_clear( comment );
				}
			}
			/* Keywordによるコメントの篩 */
			if( sqi_keyword_leng ){
				if( ZnkStr_empty(comment) ){
					continue; /* 不適合 */
				} else if( sqi_keyword_leng == 1 && sqi_keyword[ 0 ] == '*' ){
					/* 合格 */
				} else {
					const size_t pos = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(comment), ZnkStr_leng(comment),
							(uint8_t*)sqi_keyword, sqi_keyword_leng, 1, keyword_occ_tbl );
					if( pos == Znk_NPOS ){
						continue; /* 不適合 */
					}
				}
				/* 合格 */
			}
		
			/* 追加 */
			{
				if( access_time_str == NULL ){
					var = ZnkVarpAry_find_byName_literal( vars, "access_time", false );
					if( var == NULL ){
						var = ZnkVarpAry_find_byName_literal( vars, "file_time", false );
					}
					if( var ){
						access_time_str = ZnkVar_cstr( var );
						if( ZnkDate_scanStr( &access_date, access_time_str, ZnkDateStr_e_Std ) ){
						}
					}
				}
				if( file_size_str == NULL ){
					var = ZnkVarpAry_find_byName_literal( vars, "file_size", false );
					if( var ){
						file_size_str = ZnkVar_cstr(var);
						if( ZnkS_getU64U( &file_size64u, ZnkVar_cstr(var) ) ){
						}
					}
				}
				EstFInfList_add( search_info->finf_list_,
						sec_name, vpath, file_tags, &access_date, (size_t)file_size64u, ZnkStr_cstr(comment) );
				++search_info->new_idx_;
			}
		}
	}

FUNC_END:
	ZnkStrAry_destroy( tags_list );
	ZnkStr_delete( comment );
}

static bool search_onExitDir( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	return true;
}

static bool search_onEnterDir_forInfoList( ZnkDirRecursive recur, const char* cur_dir, void* arg, size_t local_err_num )
{
	bool result = false;
	SearchInfo* search_info = Znk_force_ptr_cast( SearchInfo*, arg );
	char file_path[ 256 ] = "";
	EstSQI sqi = search_info->sqi_;
	ZnkMyf myf = ZnkMyf_create();

	Znk_snprintf( file_path, sizeof(file_path), "%s/info_list.myf", cur_dir );
	if( !ZnkMyf_load( myf, file_path ) ){
		result = true; /* info_list.myfがない中間ディレクトリでもエラーとはしない */
		goto FUNC_END;
	}

	searchInMyf( search_info, sqi, myf, cur_dir, NULL ); /* 拡張子は既に絞られているのでNULLを指定 */
	result = true; 

FUNC_END:
	ZnkMyf_destroy( myf );
	++st_dir_count;
	{
		ZnkFile fp = fopenStateDat( search_info->search_key_ );
		if( fp ){
			Znk_fprintf( fp, "Searching : file=%zu : directory=%zu/%zu", st_file_count, st_dir_count, st_dir_max );
			Znk_fclose( fp );
		}
	}

	return result;
}


static bool
searchInMD5InfoList( SearchInfo* search_info, ZnkStr msg, ZnkStr fsys_path, const char* searched_key )
{
	bool   is_err_ignore = false;
	ZnkDirRecursive recur = NULL;

	recur = ZnkDirRecursive_create( is_err_ignore,
			count_onEnterDir, search_info,
			NULL, NULL,
			NULL, NULL );
	if( ZnkDirRecursive_traverse( recur, ZnkStr_cstr(fsys_path), msg ) ){}

	ZnkDirRecursive_config_onEnterDir(  recur, search_onEnterDir_forInfoList, search_info );
	ZnkDirRecursive_config_processFile( recur, NULL, NULL );
	ZnkDirRecursive_config_onExitDir(   recur, search_onExitDir, search_info );
	if( ZnkDirRecursive_traverse( recur, ZnkStr_cstr(fsys_path), msg ) ){}

	ZnkDirRecursive_destroy( recur );

#if 1
	{
		ZnkFile fp = fopenStateDat( searched_key );
		if( fp ){
			Znk_fprintf( fp, "Saving result" );
			Znk_fclose( fp );
		}
	}
#endif
	return true;
}

static bool
searchInSearchedMyf( SearchInfo* search_info, const char* myf_file_path, const ZnkStrAry file_ext )
{
	bool result = false;
	EstSQI sqi = search_info->sqi_;
	ZnkMyf myf = ZnkMyf_create();
	const char* cur_dir = search_info->src_dir_;

	if( !ZnkMyf_load( myf, myf_file_path ) ){
		goto FUNC_END;
	}

	searchInMyf( search_info, sqi, myf, cur_dir, file_ext );
	result = true; 

FUNC_END:
	ZnkMyf_destroy( myf );
	{
		ZnkFile fp = fopenStateDat( search_info->search_key_ );
		if( fp ){
			Znk_fprintf( fp, "Searching : %s", myf_file_path );
			Znk_fclose( fp );
		}
	}

	return result;
}


void
EstSearchManager_searchInBox( EstSQI sqi, ZnkStr msg, const char* searched_key, bool force_fsys_scan, const char* searched_dir )
{
	const char* elem_name = NULL;
	const char* search_paths = EstSQI_search_path(sqi);
	ZnkVarpAry  finf_list = EstFInfList_create();
	ZnkMyf search_base = ZnkMyf_create();
	char base_myf_path[ 256 ] = "";
	char ans_myf_path[ 256 ] = "";

	double begin_time = Time_getCurrentSec();
	double end_time = begin_time;

	EstBoxDirType dir_type = EstBoxDir_e_Unknown;
	ZnkStr fsys_path = ZnkStr_new( "" );
	const char* box_vname    = NULL;
	const char* box_fsys_dir = NULL;
	ZnkStr sqi_result_msg = EstSQI_result_msg( sqi );

	ZnkStrAry elem_tkns = ZnkStrAry_create( true );
	size_t elem_idx;
	size_t elem_tkns_size = 0;
	ZnkStr uxs_elem_name = ZnkStr_new( "" );

	ZnkStrEx_addSplitC( elem_tkns,
			search_paths, Znk_NPOS,
			' ', false, 4 );
	elem_tkns_size = ZnkStrAry_size( elem_tkns );

	Znk_snprintf( ans_myf_path,  sizeof(ans_myf_path),  "%s/searched_%s.myf", searched_dir, searched_key );
	ZnkDir_mkdirPath( searched_dir, Znk_NPOS, '/', NULL );

	for( elem_idx=0; elem_idx<elem_tkns_size; ++elem_idx ){
		ZnkStr_set( uxs_elem_name, ZnkStrAry_at_cstr( elem_tkns, elem_idx ) );
	
		/***
		 * Security check
		 */
		ZnkHtpURL_negateHtmlTagEffection( uxs_elem_name ); /* for XSS */
		ZnkStr_chompC( uxs_elem_name, Znk_NPOS, '/' );
		elem_name = ZnkStr_cstr( uxs_elem_name );
	
		dir_type = EstBoxBase_getDirType( elem_name );
		if( dir_type == EstBoxDir_e_Unknown ){
			ZnkStr_addf( sqi_result_msg, "Invalid path [%s].", elem_name );
			continue;
			//goto FUNC_END;
		} else {
			const char* p = Znk_strchr( elem_name, '/' );
			box_vname    = EstBoxBase_getBoxVName( dir_type );
			box_fsys_dir = EstBoxBase_getBoxFsysDir( dir_type );
			if( p ){
				ZnkStr_setf( fsys_path, "%s/%s", box_fsys_dir, p+1 );
			} else {
				ZnkStr_setf( fsys_path, "%s", box_fsys_dir );
			}
		}
	
		Znk_snprintf( base_myf_path, sizeof(base_myf_path), "%s/finf_list.myf", box_fsys_dir );
		if( ZnkDir_getType( base_myf_path ) != ZnkDirType_e_File ){
			/* searched baseが存在しない場合はファイルシステムスキャンを行わざるを得ない. */
			force_fsys_scan = true;
		}
		ZnkStr_addf( sqi_result_msg, "base_myf_path=[%s].", base_myf_path );
	
		switch( dir_type ){
		case EstBoxDir_e_Favorite:
		case EstBoxDir_e_Stockbox:
		{
			SearchInfo search_info = { 0 };
			const ZnkStrAry file_ext = EstSQI_file_ext( sqi );
			const size_t ext_num = ZnkStrAry_size( file_ext );
			size_t ext_idx;
			search_info.new_idx_ = 0;
			search_info.finf_list_ = finf_list;
			search_info.msg_ = msg;
			search_info.sqi_ = sqi;
			search_info.search_base_ = search_base;
			search_info.search_key_  = searched_key;
			search_info.src_dir_     = box_fsys_dir;
			search_info.box_vname_   = box_vname;
		
			if( force_fsys_scan ){
				if( ext_num ){
					/* 与えられた拡張子フォルダのみをスキャン */
					for( ext_idx=0; ext_idx<ext_num; ++ext_idx ){
						const char* ext = ZnkStrAry_at_cstr( file_ext, ext_idx );
						ZnkStr_setf( fsys_path, "%s/%s", box_fsys_dir, ext );
						searchInMD5InfoList( &search_info, msg, fsys_path, searched_key );
					}
				} else {
					/* 全拡張子を完全スキャン */
					searchInMD5InfoList( &search_info, msg, fsys_path, searched_key );
				}
			} else {
				/* searched base内をスキャン(超高速) */
				if( !searchInSearchedMyf( &search_info, base_myf_path, ext_num ? file_ext : NULL ) ){
					ZnkStr_addf( sqi_result_msg, "searchInSearchedMyf is failure." );
				}
			}
			break;
		}
		default:
		{
			const ZnkStrAry file_ext = EstSQI_file_ext( sqi );
			const size_t ext_num = ZnkStrAry_size( file_ext );
			SearchInfo search_info = { 0 };
			search_info.new_idx_ = 0;
			search_info.finf_list_ = finf_list;
			search_info.msg_ = msg;
			search_info.sqi_ = sqi;
			search_info.search_base_ = search_base;
			search_info.search_key_  = searched_key;
			search_info.src_dir_     = box_fsys_dir;
			search_info.box_vname_   = box_vname;
		
			if( force_fsys_scan ){
				searchInMD5InfoList( &search_info, msg, fsys_path, searched_key );
			} else {
				/* searched base内をスキャン(超高速) */
				if( !searchInSearchedMyf( &search_info, base_myf_path, ext_num ? file_ext : NULL ) ){
					ZnkStr_addf( sqi_result_msg, "searchInSearchedMyf is failure." );
				}
			}
			break;
		}
		}
	}

//FUNC_END:
	end_time = Time_getCurrentSec();
	ZnkStr_addf( sqi_result_msg, " Time:[%lf]", end_time-begin_time );
	{
		const bool is_marge_tags = false;
		const bool is_modesty    = false;
		EstFInfList_save( finf_list, sqi, ans_myf_path, is_marge_tags, is_modesty );
	}
	{
		ZnkFile fp = fopenStateDat( searched_key );
		if( fp ){
			Znk_fprintf( fp, "end" );
			Znk_fclose( fp );
		}
	}
	ZnkMyf_destroy( search_base );
	EstFInfList_destroy( finf_list );
	ZnkStr_delete( uxs_elem_name );
	ZnkStr_delete( fsys_path );
	ZnkStrAry_destroy( elem_tkns );
	return;
}

static bool
getEnableTagList_fromTagsStr( ZnkStrAry tags_list, const char* tags_str )
{
	ZnkStrAry_clear( tags_list );
	ZnkStrEx_addSplitC( tags_list,
			tags_str, Znk_NPOS,
			' ', false, 4 );
	return true;
}

static void
makeEditUI( ZnkStr EstSM_edit_ui, ZnkVarpAry sqi_vars, const char* EstSM_topdir, const char* searched_key, const char* current_category_id )
{
	static const char* comment = NULL;
	ZnkVarp pv;
	const char* EstSM_file_ext = "";
	const char* EstSM_tags     = "";
	const char* EstSM_tags_method = "and";
	const char* EstSM_tags_num = "";
	const char* EstSM_keyword  = "";
	const char* EstSM_time     = "";
	const char* EstSM_time_method = "old";
	const char* EstSM_size     = "";
	const char* EstSM_size_method = "big";

	ZnkStr_addf( EstSM_edit_ui, "<a class=MstyElemLink href=\"javascript:EjsAssort_submitCommand( document.fm_main, 'search', 'search_run' )\">" );
	ZnkStr_addf( EstSM_edit_ui, "下記で検索</a><br>\n" );

	if( ZnkS_empty(EstSM_topdir) ){
		pv = ZnkVarpAry_find_byName_literal( sqi_vars, "EstSM_topdir", false );
		if( pv ){
			EstSM_topdir = ZnkVar_cstr( pv );
		}
	}
	ZnkStr_addf( EstSM_edit_ui, "<input class=MstyInputField type=text name=EstSM_topdir placeholder=\"検索場所\" value=\"%s\" size=50><br>",
			EstSM_topdir );

	pv = ZnkVarpAry_find_byName_literal( sqi_vars, "EstSM_file_ext", false );
	if( pv ){
		EstSM_file_ext = ZnkVar_cstr( pv );
	}
	ZnkStr_addf( EstSM_edit_ui, "<input class=MstyInputField type=text name=EstSM_file_ext placeholder=\"拡張子\" value=\"%s\" size=20><br>",
			EstSM_file_ext );

	/* keyword */
	pv = ZnkVarpAry_find_byName_literal( sqi_vars, "EstSM_keyword", false );
	if( pv ){
		EstSM_keyword = ZnkVar_cstr( pv );
	}
	ZnkStr_addf( EstSM_edit_ui, "<input class=MstyInputField type=text name=EstSM_keyword placeholder=\"コメント内文字列\" value=\"%s\" size=50><br>",
			EstSM_keyword );

	pv = ZnkVarpAry_find_byName_literal( sqi_vars, "EstSM_time", false );
	if( pv ){
		EstSM_time = ZnkVar_cstr( pv );
	}
	ZnkStr_addf( EstSM_edit_ui, "<input class=MstyInputField type=text name=EstSM_time placeholder=\"時刻\"   value=\"%s\" size=20> &nbsp;",
			EstSM_time );

	pv = ZnkVarpAry_find_byName_literal( sqi_vars, "EstSM_time_method", false );
	if( pv ){
		EstSM_time_method = ZnkVar_cstr( pv );
	}
	ZnkStr_addf( EstSM_edit_ui, "<select name=\"EstSM_time_method\" >"
			"<option value=\"old\" %s>指定した時刻より古い</option>"
			"<option value=\"new\" %s>指定した時刻より新しい</option>"
			"<option value=\"eq\"  %s>指定した時刻と等しい</option>"
			"</select><br>\n",
			ZnkS_eq(EstSM_time_method,"old") ? "selected" : "",
			ZnkS_eq(EstSM_time_method,"new") ? "selected" : "",
			ZnkS_eq(EstSM_time_method,"eq")  ? "selected" : "" );

	pv = ZnkVarpAry_find_byName_literal( sqi_vars, "EstSM_size", false );
	if( pv ){
		EstSM_size = ZnkVar_cstr( pv );
	}
	ZnkStr_addf( EstSM_edit_ui, "<input class=MstyInputField type=text name=EstSM_size placeholder=\"サイズ\" value=\"%s\" size=20> &nbsp;",
			EstSM_size );

	pv = ZnkVarpAry_find_byName_literal( sqi_vars, "EstSM_size_method", false );
	if( pv ){
		EstSM_size_method = ZnkVar_cstr( pv );
	}
	ZnkStr_addf( EstSM_edit_ui, "<select name=\"EstSM_size_method\" >"
			"<option value=\"big\"   %s>指定したサイズより大きい</option>"
			"<option value=\"small\" %s>指定したサイズより小さい</option>"
			"<option value=\"eq\"    %s>指定したサイズと等しい</option>"
			"</select><br>\n",
			ZnkS_eq(EstSM_size_method,"big")   ? "selected" : "",
			ZnkS_eq(EstSM_size_method,"small") ? "selected" : "",
			ZnkS_eq(EstSM_size_method,"eq")    ? "selected" : "" );

	{
		ZnkStrAry enable_tag_list = ZnkStrAry_create( true );
		/* tags */
		pv = ZnkVarpAry_find_byName_literal( sqi_vars, "EstSM_tags", false );
		if( pv ){
			EstSM_tags = ZnkVar_cstr( pv );
		}
		ZnkStr_addf( EstSM_edit_ui, "<input class=MstyInputField type=text name=EstSM_tags placeholder=\"タグ\" value=\"\" size=50 disable> &nbsp;" );
		getEnableTagList_fromTagsStr( enable_tag_list, EstSM_tags );

		/* tags_method */
		pv = ZnkVarpAry_find_byName_literal( sqi_vars, "EstSM_tags_method", false );
		if( pv ){
			EstSM_tags_method = ZnkVar_cstr( pv );
		}
		ZnkStr_addf( EstSM_edit_ui, "<select name=\"EstSM_tags_method\" >"
				"<option value=\"and\" %s>AND検索</option>"
				"<option value=\"or\" %s>OR検索</option>"
				"</select><br>\n",
				ZnkS_eq(EstSM_tags_method,"and") ? "selected" : "",
				ZnkS_eq(EstSM_tags_method,"or") ? "selected" : "" );

		/* tags_num */
		pv = ZnkVarpAry_find_byName_literal( sqi_vars, "EstSM_tags_num", false );
		if( pv ){
			EstSM_tags_num = ZnkVar_cstr( pv );
		}
		ZnkStr_addf( EstSM_edit_ui,
				"<input class=MstyInputField type=text name=EstSM_tags_num placeholder=\"タグの個数\" value=\"%s\" size=10 disable><br>\n",
				EstSM_tags_num );
	
		{
			ZnkStr arg = ZnkStr_newf( "searched_key=%s", searched_key );
			EstAssortUI_makeCategorySelectBar( EstSM_edit_ui, current_category_id, NULL,
					"search", "edit", ZnkStr_cstr(arg), "" );
			ZnkStr_delete( arg );
		}
		EstAssortUI_makeTagsView( EstSM_edit_ui, enable_tag_list, current_category_id, "", comment, false );
		ZnkStrAry_destroy( enable_tag_list );
	}
	
	ZnkStr_addf( EstSM_edit_ui, "<a class=MstyElemLink href=\"javascript:EjsAssort_submitCommand( document.fm_main, 'search', 'search_run' )\">" );
	ZnkStr_addf( EstSM_edit_ui, "上記で検索</a><br>\n" );
}

#if 0
static size_t
remove_byFileList( ZnkVarpAry cache_file_list, ZnkStrAry assorted_filelist )
{
	size_t count = 0;
	size_t size = ZnkVarpAry_size( cache_file_list );
	size_t idx;
	ZnkVarp var;
	ZnkStrAry sda;
	const char* file_path = NULL;
	for( idx=size; idx; --idx ){
		var = ZnkVarpAry_at( cache_file_list, idx-1 );
		sda = ZnkVar_str_ary( var );
		file_path = ZnkStrAry_at_cstr(sda,0);
		if( ZnkStrAry_find( assorted_filelist, 0, file_path, Znk_NPOS ) != Znk_NPOS ){
			ZnkVarpAry_erase_byIdx( cache_file_list, idx-1 );
			++count;
		}
	}
	return count;
}
#endif

typedef enum {
	 Mode_e_SearchNew=0
	,Mode_e_ShowResult
	,Mode_e_Edit
	,Mode_e_Save
} EstSearchMode;

static bool
addNewTopic( const char* topics_dir, const char* query_searched_key, const char* topic_name, ZnkStr msg )
{
	ZnkMyf topic_list_myf = ZnkMyf_create();
	bool result = false;
	char topic_list_myf_path[ 256 ] = "";

	Znk_snprintf( topic_list_myf_path, sizeof(topic_list_myf_path), "%s/topic_list.myf", topics_dir );
	if( ZnkMyf_load( topic_list_myf, topic_list_myf_path ) ){
		ZnkMyfSection sec = NULL;
		ZnkVarp       var  = NULL;
		ZnkVarpAry    vars = NULL;
		char sec_name[ 256 ] = "";
		Znk_snprintf( sec_name, sizeof(sec_name), "topic_%s", query_searched_key );
		sec  = ZnkMyf_internSection( topic_list_myf, sec_name, ZnkMyfSection_e_Vars );
		vars = ZnkMyfSection_vars( sec );
		if( ZnkVarpAry_size( vars ) ){
			/* Update */
			var = ZnkVarpAry_findObj_byName_literal( vars, "type", false );
			if( var && ZnkS_eq( ZnkVar_cstr( var ), "searched" ) ){
				var = ZnkVarpAry_findObj_byName_literal( vars, "name", false );
				if( var ){
					ZnkVar_set_val_Str( var, topic_name, Znk_NPOS );
				}
				var = ZnkVarpAry_findObj_byName_literal( vars, "file", false );
				if( var ){
					ZnkStr_setf( ZnkVar_str( var ), "searched_%s.myf", query_searched_key );
				}
				ZnkStr_addf( msg, "sec_name=[%s] is updated.\n", sec_name );
			}
		} else {
			/* New */
			var = ZnkVarp_create( "type", "", 0, ZnkPrim_e_Str, NULL );
			ZnkVar_set_val_Str( var, "searched", Znk_NPOS );
			ZnkVarpAry_push_bk( vars, var );

			var = ZnkVarp_create( "name", "", 0, ZnkPrim_e_Str, NULL );
			ZnkVar_set_val_Str( var, topic_name, Znk_NPOS );
			ZnkVarpAry_push_bk( vars, var );

			var = ZnkVarp_create( "file", "", 0, ZnkPrim_e_Str, NULL );
			ZnkStr_setf( ZnkVar_str( var ), "searched_%s.myf", query_searched_key );
			ZnkVarpAry_push_bk( vars, var );
			ZnkStr_addf( msg, "sec_name=[%s] is added newly.\n", sec_name );
		}

		if( IS_OK( var = ZnkVarpAry_findObj_byName_literal( vars, "file", false ) ) ){
			const char* filename = ZnkVar_cstr( var );
			if( !ZnkS_empty( filename ) ){
				char src_path[ 256 ] = "";
				char dst_path[ 256 ] = "";
				ZnkDir_mkdirPath( topics_dir, Znk_NPOS, '/', NULL );
				Znk_snprintf( src_path, sizeof(src_path), "%s/tmp/searched/%s", topics_dir, filename );
				Znk_snprintf( dst_path, sizeof(dst_path), "%s/%s", topics_dir, filename );
				if( ZnkDir_copyFile_byForce( src_path, dst_path, msg ) ){
					if( ZnkDir_deleteFile_byForce( src_path ) ){
						ZnkStr_addf( msg, "OK. topic [%s] is moved from [%s] to [%s]\n", filename, src_path, dst_path );
					}
				}
			}
		}

		ZnkMyf_save( topic_list_myf, topic_list_myf_path );
		result = true;
	} else {
		ZnkStr_addf( msg, "Cannot load [%s]\n", topic_list_myf_path );
		result = false;
	}

	ZnkMyf_destroy( topic_list_myf );
	return result;
}

static bool
getTopicName( const char* topics_dir, const char* query_searched_key, ZnkStr topic_name, ZnkStr msg )
{
	ZnkMyf topic_list_myf = ZnkMyf_create();
	bool result = false;
	char topic_list_myf_path[ 256 ] = "";

	Znk_snprintf( topic_list_myf_path, sizeof(topic_list_myf_path), "%s/topic_list.myf", topics_dir );
	if( ZnkMyf_load( topic_list_myf, topic_list_myf_path ) ){
		ZnkMyfSection sec = NULL;
		ZnkVarp       var  = NULL;
		ZnkVarpAry    vars = NULL;
		char sec_name[ 256 ] = "";
		Znk_snprintf( sec_name, sizeof(sec_name), "topic_%s", query_searched_key );
		sec  = ZnkMyf_findSection( topic_list_myf, sec_name, ZnkMyfSection_e_Vars );
		if( sec ){
			vars = ZnkMyfSection_vars( sec );
			var = ZnkVarpAry_findObj_byName_literal( vars, "name", false );
			if( var ){
				ZnkStr_set( topic_name, ZnkVar_cstr(var) );
				result = true;
			}
		}
	} else {
		ZnkStr_addf( msg, "Cannot load [%s]\n", topic_list_myf_path );
		result = false;
	}
	ZnkMyf_destroy( topic_list_myf );
	return result;
}

static void
makeAssortAndEditUI( ZnkStr assort_ui, ZnkStr tag_editor_ui, ZnkVarpAry post_vars, const char* comment )
{
	ZnkStrAry enable_tag_list = ZnkStrAry_create( true );
	ZnkVarp pv = ZnkVarpAry_find_byName_literal( post_vars, "EstSM_tags", false );
	ZnkStr tag_editor_msg = ZnkStr_new( "" );
	const char* tags = NULL;
	if( pv ){
		tags = ZnkVar_cstr( pv );
	}
	if( tags ){
		getEnableTagList_fromTagsStr( enable_tag_list, tags );
	}
	//EstAssortUI_make( assort_ui, "", enable_tag_list, comment );
	//EstAssortUI_makeTagEditor( tag_editor_ui, ZnkStr_cstr(tag_editor_msg) );
	EstAssortUI_makeTagsView( assort_ui, enable_tag_list, "category_all", "", comment, false );
	ZnkStrAry_destroy( enable_tag_list );
	ZnkStr_delete( tag_editor_msg );
}

void
EstSearchManager_main( RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr msg, const char* authentic_key )
{
	ZnkBird               bird = ZnkBird_create( "#[", "]#" );
	const char*           template_html_file = "templates/search_manager.html";
	RanoModule            mod = NULL;
	struct ZnkHtpHdrs_tag htp_hdrs = { 0 };
	ZnkStr                pst_str  = NULL;
	bool                  is_unescape_val  = true;
	bool                  is_authenticated = false;
	bool                  from_topic       = false;
	ZnkVarp               cmd_var  = NULL;
	ZnkVarp               mode_var = NULL;
	ZnkVarp               varp = NULL;
	ZnkStr EstSM_edit_ui      = ZnkStr_new( "" );
	ZnkStr EstSM_result       = ZnkStr_new( "" );
	ZnkStr assort_ui          = ZnkStr_new( "" );
	ZnkStr tag_editor_ui      = ZnkStr_new( "" );
	ZnkStr EstCM_img_url_list = ZnkStr_new( "" );
	EstSQI sqi = NULL;
	char issued_searched_key[ 256 ] = "0";
	char result_searched_key[ 256 ] = "new";
	EstSearchMode es_mode = Mode_e_ShowResult;
	char show_filename[ 256 ] = "";
	const char* current_category_id = "category_all";
	const char* topics_dir = EstConfig_topics_dir();
	char searched_dir[ 256 ] = "";

	Znk_snprintf( searched_dir, sizeof(searched_dir), "%s/tmp/searched", topics_dir );

	st_dir_count = 0;
	st_file_count = 0;

	RanoCGIUtil_getPostedFormData( evar, post_vars, mod, &htp_hdrs, pst_str, NULL, is_unescape_val );
	if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "Moai_AuthenticKey", false ) )
	  && ZnkS_eq( authentic_key, ZnkVar_cstr(varp) ) ){
		is_authenticated = true;
	} else {
	}
	ZnkStr_addf( msg, "is_authenticated=[%d]\n", is_authenticated );
	ZnkBird_regist( bird, "Moai_AuthenticKey", authentic_key );


	if( is_authenticated && IS_OK( cmd_var = ZnkVarpAry_find_byName_literal( post_vars, "command", false ) )){
		ZnkStr_addf( msg, "cmd_var is [%s]\n", ZnkVar_cstr(cmd_var) );
		if( ZnkS_eq( ZnkVar_cstr(cmd_var), "search_run" ) ){
			es_mode = Mode_e_SearchNew;
		} else if( ZnkS_eq( ZnkVar_cstr(cmd_var), "result" ) ){
			mode_var = ZnkVarpAry_find_byName_literal( post_vars, "mode", false );
			if( mode_var ){
				ZnkStr_addf( msg, "mode_var is [%s]\n", ZnkVar_cstr(mode_var) );
				if( ZnkS_eq( ZnkVar_cstr(mode_var), "run_new" ) ){
					es_mode = Mode_e_SearchNew;
				} else {
					es_mode = Mode_e_ShowResult;
				}
			} else {
				ZnkStr_addf( msg, "mode_var is NULL\n" );
			}
		} else if( ZnkS_eq( ZnkVar_cstr(cmd_var), "edit" ) ){
			mode_var = ZnkVarpAry_find_byName_literal( post_vars, "mode", false );
			if( mode_var ){
				ZnkStr_addf( msg, "mode_var is [%s]\n", ZnkVar_cstr(mode_var) );
				if( ZnkS_eq( ZnkVar_cstr(mode_var), "run_new" ) ){
					es_mode = Mode_e_SearchNew;
				} else if( ZnkS_eq( ZnkVar_cstr(mode_var), "from_topic" ) ){
					es_mode = Mode_e_Edit;
					from_topic = true;
				} else {
					es_mode = Mode_e_Edit;
				}
			} else {
				ZnkStr_addf( msg, "mode_var is NULL\n" );
				es_mode = Mode_e_Edit;
			}
		} else if( ZnkS_eq( ZnkVar_cstr(cmd_var), "save" ) ){
			es_mode = Mode_e_Save;
		}
	} else {
		ZnkStr_addf( msg, "cmd_var is NULL\n" );
		es_mode = Mode_e_ShowResult;
	}

	if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "searched_key", false ) )){
		if( ZnkS_eq( ZnkVar_cstr(varp), "recentory" ) ){
			/* Recentory */
			issuedSearchedKey( issued_searched_key, sizeof(issued_searched_key) );
			Znk_snprintf( result_searched_key, sizeof(result_searched_key), "%s", ZnkVar_cstr(varp) );
			Znk_snprintf( show_filename, sizeof(show_filename), "recentory.myf" );
		} else if ( ZnkS_eq( ZnkVar_cstr(varp), "stockbox" )
		         || ZnkS_eq( ZnkVar_cstr(varp), "favorite" )
		){
			/* Special Box */
			EstBoxDirType box_dir_type = EstBoxBase_getDirType( ZnkVar_cstr(varp) );
			const char*   box_fsys_dir = EstBoxBase_getBoxFsysDir( box_dir_type );
			issuedSearchedKey( issued_searched_key, sizeof(issued_searched_key) );
			Znk_snprintf( result_searched_key, sizeof(result_searched_key), "%s", ZnkVar_cstr(varp) );
			Znk_snprintf( show_filename, sizeof(show_filename), "%s/finf_list.myf", box_fsys_dir );
		} else {
			/* General Number */
			Znk_snprintf( issued_searched_key, sizeof(issued_searched_key), "%s", ZnkVar_cstr(varp) );
			Znk_snprintf( result_searched_key, sizeof(result_searched_key), "%s", ZnkVar_cstr(varp) );
			getSearchedBoxShowFilename( topics_dir, show_filename, sizeof(show_filename), issued_searched_key, from_topic );
		}
	} else {
		/* General Number Newly */
		issuedSearchedKey( issued_searched_key, sizeof(issued_searched_key) );
		Znk_snprintf( result_searched_key, sizeof(result_searched_key), "new" );
		Znk_snprintf( show_filename, sizeof(show_filename), "%s/searched_%s.myf", searched_dir, issued_searched_key );
	}

	{
		ZnkFile fp = fopenStateDat( issued_searched_key );
		if( fp ){
			Znk_fprintf( fp, "waiting" );
			Znk_fclose( fp );
		}
	}
	ZnkStr_addf( msg, "issued_searched_key=[%s]\n", issued_searched_key );
	ZnkStr_addf( msg, "result_searched_key=[%s]\n", result_searched_key );
	ZnkStr_addf( msg, "show_filename=[%s]\n", show_filename );
	ZnkStr_addf( msg, "es_mode=[%d]\n", es_mode );

	{
		varp = ZnkVarpAry_findObj_byName_literal( post_vars, "category_key", false );
		if( varp ){
			current_category_id = ZnkVar_cstr( varp );
		}
	}

	switch( es_mode ){
	case Mode_e_SearchNew:
	{
		ZnkStr_addf( msg, "Mode_e_SearchNew.\n" );
		makeEditUI( EstSM_edit_ui, post_vars, "", result_searched_key, current_category_id );
		sqi = EstSQI_create( post_vars );
		break;
	}
	case Mode_e_Edit:
	{
		ZnkVarpAry  finf_list = EstFInfList_create();
		ZnkVarpAry sqi_vars   = ZnkVarpAry_create( true );
		const char* sm_topbox = "";
		template_html_file = "templates/search_manager.html";

		ZnkStr_addf( msg, "Mode_e_Edit.\n" );

		if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "cache_path", false ) )){
			sm_topbox = ZnkVar_cstr(varp);
		}

		if( EstFInfList_load( finf_list, sqi_vars, show_filename ) ){
			ZnkStr query_string_base = ZnkStr_new( "" );
			ZnkVarp varp = NULL;
			size_t show_file_num = EstConfig_getShowFileNum();
			size_t begin_idx = 0;
			size_t end_idx   = show_file_num;
			size_t finf_list_size = EstFInfList_size( finf_list );

			if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_cache_begin", false ) )){
				ZnkS_getSzU( &begin_idx, ZnkVar_cstr(varp) );
				ZnkStr_addf( msg, "est_cache_begin=[%zu]\n", begin_idx );
			}
			if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_cache_end", false ) )){
				ZnkS_getSzU( &end_idx, ZnkVar_cstr(varp) );
				ZnkStr_addf( msg, "est_cache_end=[%zu]\n", end_idx );
			}

			ZnkStr_setf( query_string_base, "est_manager=search&amp;command=result&amp;searched_key=%s", result_searched_key );

			{
				size_t finf_idx;
				for( finf_idx=0; finf_idx<finf_list_size; ++finf_idx ){
					EstFInf finf = EstFInfList_at( finf_list, finf_idx );
					const char* file_path = EstFInf_file_path( finf );
					EstAssort_addImgURLList( EstCM_img_url_list, finf_idx, begin_idx, end_idx, file_path );
				}
			}

			ZnkStr_addf( EstSM_result, "<hr>" );
			EstUI_showPageSwitcher( EstSM_result, ZnkStr_cstr(query_string_base),
					finf_list_size,
					show_file_num, begin_idx, authentic_key, "PageSwitcher" );

			ZnkStr_add( EstSM_result, "<table><tr><td valign=top>\n" );

			EstBoxUI_make_forSearchResult( EstSM_result, finf_list,
					begin_idx, end_idx, authentic_key );

			ZnkStr_add( EstSM_result, "</td><td valign=top><span id=preview></span></td></tr></table>\n" );

			ZnkStr_addf( EstSM_result, "%zu 件ヒットしました.<br>", finf_list_size );
			{
				ZnkVarp var = ZnkVarpAry_find_byName_literal( sqi_vars, "EstSM_result", false );
				if( var ){
					ZnkStr_addf( EstSM_result, "SQI result msg = [%s].<br>", ZnkVar_cstr(var) );
				}
			}
			ZnkStr_delete( query_string_base );
		} else {
			ZnkStr_add( EstSM_result, "Cannot load finf_list." );
		}

		if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "category_key", false ) )){
			ZnkStr_addf( msg, "category_key=[%s]\n", ZnkVar_cstr(varp) );
		}
		makeEditUI( EstSM_edit_ui, sqi_vars, sm_topbox, result_searched_key, current_category_id );

		ZnkVarpAry_destroy( sqi_vars );
		EstFInfList_destroy( finf_list );
		break;
	}
	case Mode_e_ShowResult:
	{
		ZnkVarpAry  finf_list = EstFInfList_create();
		ZnkVarpAry  sqi_vars  = ZnkVarpAry_create( true );
		//const char* sm_topbox = "";
		const char* comment = "";
		template_html_file = "templates/search_manager.html";
		ZnkStr_addf( msg, "Mode_e_ShowResult.\n" );

#if 0
		if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "cache_path", false ) )){
			sm_topbox = ZnkVar_cstr(varp);
		}
#endif

		if( EstFInfList_load( finf_list, sqi_vars, show_filename ) ){
			ZnkStr query_string_base = ZnkStr_new( "" );
			ZnkVarp varp = NULL;
			size_t show_file_num = EstConfig_getShowFileNum();
			size_t begin_idx = 0;
			size_t end_idx   = show_file_num;
			size_t finf_list_size = EstFInfList_size( finf_list );

			if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_cache_begin", false ) )){
				ZnkS_getSzU( &begin_idx, ZnkVar_cstr(varp) );
				ZnkStr_addf( msg, "est_cache_begin=[%zu]\n", begin_idx );
			}
			if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "est_cache_end", false ) )){
				ZnkS_getSzU( &end_idx, ZnkVar_cstr(varp) );
				ZnkStr_addf( msg, "est_cache_end=[%zu]\n", end_idx );
			}

			ZnkStr_setf( query_string_base, "est_manager=search&amp;command=result&amp;searched_key=%s", result_searched_key );

			{
				size_t finf_idx;
				for( finf_idx=0; finf_idx<finf_list_size; ++finf_idx ){
					EstFInf finf = EstFInfList_at( finf_list, finf_idx );
					const char* file_path = EstFInf_file_path( finf );
					EstAssort_addImgURLList( EstCM_img_url_list, finf_idx, begin_idx, end_idx, file_path );
				}
			}

			{
				ZnkStr topic_name = ZnkStr_new( "無名" );
				getTopicName( topics_dir, result_searched_key, topic_name, msg );
				ZnkStr_addf( EstSM_edit_ui, "<br>\n" );
				ZnkStr_addf( EstSM_edit_ui, "<input class=MstyInputField type=text name=topic_name placeholder=\"トピック名\" value=\"%s\" size=50>",
						ZnkStr_cstr(topic_name) );
				ZnkStr_addf( EstSM_edit_ui, " <a class=MstyElemLink href=\"javascript:EjsAssort_submitCommand( document.fm_main, 'search', 'save' )\">" );
				ZnkStr_addf( EstSM_edit_ui, "この名前でトピックを保存</a> \n" );
				ZnkStr_delete( topic_name );

				ZnkStr_addf( EstSM_edit_ui, "<a class=MstyElemLink href=\"/easter?est_manager=search&amp;command=edit&amp;searched_key=%s&amp;Moai_AuthenticKey=%s\" >検索して内容を更新</a> &nbsp;\n",
						result_searched_key, authentic_key );
			}

			ZnkStr_addf( EstSM_result, "<hr>" );
			EstUI_showPageSwitcher( EstSM_result, ZnkStr_cstr(query_string_base),
					finf_list_size,
					show_file_num, begin_idx, authentic_key, "PageSwitcher" );

			ZnkStr_add( EstSM_result, "<table><tr><td valign=top>\n" );

			EstBoxUI_make_forSearchResult( EstSM_result, finf_list,
					begin_idx, end_idx, authentic_key );

			ZnkStr_add( EstSM_result, "</td><td valign=top><span id=preview></span></td></tr></table>\n" );

			ZnkStr_addf( EstSM_result, "%zu 件ヒットしました.<br>", finf_list_size );
			{
				ZnkVarp var = ZnkVarpAry_find_byName_literal( sqi_vars, "EstSM_result", false );
				if( var ){
					ZnkStr_addf( EstSM_result, "SQI result msg = [%s].<br>", ZnkVar_cstr(var) );
				}
			}
			ZnkStr_delete( query_string_base );
		} else {
			ZnkStr_add( EstSM_result, "Cannot load finf_list." );
		}

		makeAssortAndEditUI( assort_ui, tag_editor_ui, post_vars, comment );

		ZnkVarpAry_destroy( sqi_vars );
		EstFInfList_destroy( finf_list );
		break;
	}
	case Mode_e_Save:
	{
		ZnkVarp varp = NULL;
		const char* topic_name = "";
		template_html_file = "templates/command_complete.html";
		ZnkStr_addf( msg, "Mode_e_Save.\n" );
		if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "topic_name", false ) )){
			ZnkStr result_view = ZnkStr_new( "" );
			topic_name = ZnkVar_cstr(varp);
			if( addNewTopic( topics_dir, result_searched_key, topic_name, msg ) ){
				ZnkStr_addf( result_view, "トピック[%s]を保存しました.", topic_name );
			} else {
				ZnkStr_addf( result_view, "トピック[%s]の保存に失敗しました.", topic_name );
			}
			ZnkBird_regist( bird, "result_view",  ZnkStr_cstr(result_view) );
			ZnkStr_delete( result_view );
		}
		ZnkStr_addf( msg, "command=[%s]\n", ZnkVar_cstr(cmd_var) );
		ZnkStr_addf( msg, "topic_name=[%s]\n", topic_name );
		break;
	}
	default:
		ZnkStr_addf( msg, "Mode : default.\n" );
		makeEditUI( EstSM_edit_ui, post_vars, "", result_searched_key, current_category_id );
		ZnkStr_addf( EstSM_result, "command=[%s]\n", ZnkVar_cstr(cmd_var) );
		break;
	}

	/***
	 * Security check
	 */
	if( sqi ){
		ZnkStrAry elem_tkns = ZnkStrAry_create( true );
		size_t elem_idx;
		size_t elem_tkns_size = 0;
		ZnkStr uxs_elem_name = ZnkStr_new( "" );
		const char* elem_name = NULL;
		size_t path_idx = Znk_NPOS;
		const char* search_paths = EstSQI_search_path(sqi);

		ZnkStrEx_addSplitC( elem_tkns,
				search_paths, Znk_NPOS,
				' ', false, 4 );
		elem_tkns_size = ZnkStrAry_size( elem_tkns );

		for( elem_idx=0; elem_idx<elem_tkns_size; ++elem_idx ){
			ZnkStr_set( uxs_elem_name, ZnkStrAry_at_cstr( elem_tkns, elem_idx ) );
			ZnkHtpURL_negateHtmlTagEffection( uxs_elem_name ); /* for XSS */
			ZnkStr_chompC( uxs_elem_name, Znk_NPOS, '/' );
			elem_name = ZnkStr_cstr( uxs_elem_name );
			path_idx = findValidPath( elem_name );
			if( path_idx == Znk_NPOS ){
				ZnkStr_addf( msg, "Invalid path[%s].\n", elem_name );
				es_mode = Mode_e_ShowResult;
				break;
			}
		}
		ZnkStr_delete( uxs_elem_name );
		ZnkStrAry_destroy( elem_tkns );
	}

	{
		ZnkStr hint_table = EstHint_getHintTable( "search_manager" );
		if( hint_table ){
			ZnkBird_regist( bird, "hint_table", ZnkStr_cstr(hint_table) );
		} else {
			ZnkBird_regist( bird, "hint_table", "" );
		}
	}

	ZnkHtpURL_negateHtmlTagEffection( msg ); /* for XSS */
	{
		ZnkSRef old_ptn = { 0 };
		ZnkSRef new_ptn = { 0 };
		ZnkSRef_set_literal( &old_ptn, "\n" );
		ZnkSRef_set_literal( &new_ptn, "<br>\n" );
		ZnkStrEx_replace_BF( msg, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}

	ZnkBird_regist( bird, "searched_key",  issued_searched_key );
	ZnkBird_regist( bird, "msg",           ZnkStr_cstr(msg) );
	ZnkBird_regist( bird, "EstCM_img_url_list", ZnkStr_cstr(EstCM_img_url_list) );
	ZnkBird_regist( bird, "EstSM_edit_ui",   ZnkStr_cstr( EstSM_edit_ui ) );
	ZnkBird_regist( bird, "EstSM_result",    ZnkStr_cstr( EstSM_result ) );
	ZnkBird_regist( bird, "assort_ui",       ZnkStr_cstr( assort_ui ) );
	ZnkBird_regist( bird, "tag_editor_ui",   ZnkStr_cstr( assort_ui ) );
	RanoCGIUtil_printTemplateHTML( evar, bird, template_html_file );

	if( es_mode == Mode_e_SearchNew ){
		Znk_fflush( Znk_stdout() );
		Znk_fclose( Znk_stdout() );
		EstSearchManager_searchInBox( sqi, msg, issued_searched_key, false, searched_dir );
	}

	ZnkBird_destroy( bird );
	EstSQI_destroy( sqi );
	ZnkStr_delete( EstSM_edit_ui );
	ZnkStr_delete( EstSM_result );
	ZnkStr_delete( EstCM_img_url_list );
	ZnkStr_delete( assort_ui );
	ZnkStr_delete( tag_editor_ui );
}

