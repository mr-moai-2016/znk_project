#include <Est_finf.h>
#include <Est_record.h>

#include <Rano_sset.h>
#include <Rano_file_info.h>

#include <Znk_varp_ary.h>
#include <Znk_stdc.h>
#include <Znk_prim.h>
#include <Znk_myf.h>
#include <Znk_algo_vec.h>
#include <Znk_dir.h>
#include <Znk_md5.h>
#include <Znk_missing_libc.h>

struct EstFInfImpl {
	ZnkStr  id_;
	ZnkStr  file_path_;
	ZnkStr  file_tags_;
	ZnkDate access_time_;
	size_t  file_size_;
	ZnkStr  comment_;
};

static EstFInf
createEstFInf( const char* id, const char* file_path, const char* file_tags, const ZnkDate* access_time, size_t file_size, const char* comment )
{
	EstFInf finf = Znk_malloc( sizeof( struct EstFInfImpl ) );
	finf->id_        = ZnkStr_new( id );
	finf->file_path_ = ZnkStr_new( file_path );
	finf->file_tags_ = ZnkStr_new( file_tags );
	finf->access_time_ = *access_time;
	finf->file_size_ = file_size;
	finf->comment_ = ZnkStr_new( comment ? comment : "" );
	return finf;
}
static void
destroyEstFInf( EstFInf finf )
{
	if( finf ){
		ZnkStr_delete( finf->id_ );
		ZnkStr_delete( finf->file_path_ );
		ZnkStr_delete( finf->file_tags_ );
		ZnkStr_delete( finf->comment_ );
		Znk_free( finf );
	}
}

static void
deleteElem( void* elem )
{
	destroyEstFInf( (EstFInf)elem );
}

void
EstFInfList_clear( ZnkVarpAry finf_list )
{
	ZnkVarpAry_clear( finf_list );
}

Znk_INLINE EstFInf
I_atEstFInf( const ZnkVarpAry finf_list, size_t idx )
{
	ZnkVarp var = ZnkVarpAry_at( finf_list, idx );
	ZnkPrimp prim = ZnkVar_prim( var );
	return (EstFInf)ZnkPrim_ptr( prim );
}

size_t
EstFInfList_find( ZnkVarpAry finf_list, const char* query_id )
{
	const size_t size = EstFInfList_size( finf_list );
	size_t idx;
	EstFInf finf = NULL;
	for( idx=0; idx<size; ++idx ){
		finf = I_atEstFInf( finf_list, idx );
		if( ZnkS_eq( query_id, EstFInf_id(finf) ) ){
			/* found */
			return idx;
		}
	}
	return Znk_NPOS;
}

EstFInf
EstFInfList_add( ZnkVarpAry finf_list, const char* id,
		const char* file_path, const char* file_tags, const ZnkDate* access_time, size_t file_size, const char* comment )
{
	EstFInf finf = createEstFInf( id, file_path, file_tags, access_time, file_size, comment );
	ZnkVarp var  = ZnkVarp_create( id, "", 0, ZnkPrim_e_Ptr, deleteElem );
	ZnkPrimp prim = ZnkVar_prim( var );
	ZnkPrim_set_ptr( prim, finf );
	ZnkVarpAry_push_bk( finf_list, var );
	return finf;
}
EstFInf
EstFInfList_regist( ZnkVarpAry finf_list, const char* id,
		const char* file_path, const char* file_tags, const ZnkDate* access_time, size_t file_size, const char* comment,
		bool is_marge_tags )
{
	const size_t idx = EstFInfList_find( finf_list, id );
	EstFInf finf = NULL;
	if( idx == Znk_NPOS ){
		finf = EstFInfList_add( finf_list, id, file_path, file_tags, access_time, file_size, comment );
	} else {
		finf = EstFInfList_at( finf_list, idx );
		ZnkStr_set( finf->file_path_, file_path );
		if( is_marge_tags ){
			RanoSSet_addSeq( finf->file_tags_, file_tags );
		} else {
			ZnkStr_set( finf->file_tags_, file_tags );
		}
		EstFInf_set_access_time( finf, access_time );
		finf->file_size_ = file_size;
		ZnkStr_set( finf->comment_, comment );
	}
	return finf;
}

static void
getMD5CheckSum( ZnkMD5CheckSum* checksum, const char* file_path )
{
	ZnkMD5_CTX md5ctx = {{ 0 }};
	ZnkMD5_getDigest_byFile( &md5ctx, file_path );
	ZnkMD5CheckSum_get_byContext( checksum, &md5ctx );
}

EstFInf
EstFInfList_addFileByMD5( ZnkVarpAry finf_list, const char* box_vname, const char* fsys_path, const char* file_tags, const char* comment )
{
	EstFInf finf = NULL;
	if( ZnkDir_getType( fsys_path ) == ZnkDirType_e_File ){
		char fileid[ 256 ] = "";
		char vpath[ 256 ] = "";
		ZnkMD5CheckSum checksum = {{ 0 }};
		const char* md5_of_file = "";
		char subdir_xx[ 16 ] = "";
		const char* ext = ZnkS_get_extension( fsys_path, '.' );
		getMD5CheckSum( &checksum, fsys_path );
		md5_of_file = ZnkMD5CheckSum_cstr( &checksum );
		ZnkS_copy( subdir_xx, sizeof(subdir_xx), md5_of_file, 2 );
		if( ZnkS_empty( ext ) ){ ext = "dat"; }
		Znk_snprintf( fileid, sizeof(fileid), "%s.%s", md5_of_file, ext );
		Znk_snprintf( vpath, sizeof(vpath), "%s/%s/%s/%s.%s", box_vname, ext, subdir_xx, md5_of_file, ext );
		{
			ZnkDate access_time = { 0 };
			uint64_t file_size = 0;
			RanoFileInfo_getLastUpdatedTime( fsys_path, &access_time );
			RanoFileInfo_getFileSize( fsys_path, &file_size );
			finf = EstFInfList_add( finf_list, fileid,
					vpath, file_tags, &access_time, file_size, comment );
		}
	}
	return finf;
}
EstFInf
EstFInfList_registFileByMD5( ZnkVarpAry finf_list, const char* box_vname, const char* fsys_path,
		const char* file_tags, const char* comment, bool is_marge_tags )
{
	EstFInf finf = NULL;
	if( ZnkDir_getType( fsys_path ) == ZnkDirType_e_File ){
		char fileid[ 256 ] = "";
		char vpath[ 256 ] = "";
		ZnkMD5CheckSum checksum = {{ 0 }};
		const char* md5_of_file = "";
		char subdir_xx[ 16 ] = "";
		const char* ext = ZnkS_get_extension( fsys_path, '.' );
		getMD5CheckSum( &checksum, fsys_path );
		md5_of_file = ZnkMD5CheckSum_cstr( &checksum );
		ZnkS_copy( subdir_xx, sizeof(subdir_xx), md5_of_file, 2 );
		if( ZnkS_empty( ext ) ){ ext = "dat"; }
		Znk_snprintf( fileid, sizeof(fileid), "%s.%s", md5_of_file, ext );
		Znk_snprintf( vpath, sizeof(vpath), "%s/%s/%s/%s.%s", box_vname, ext, subdir_xx, md5_of_file, ext );
		{
			ZnkDate access_time = { 0 };
			uint64_t file_size = 0;
			RanoFileInfo_getLastUpdatedTime( fsys_path, &access_time );
			RanoFileInfo_getFileSize( fsys_path, &file_size );
			finf = EstFInfList_regist( finf_list, fileid,
					vpath, file_tags, &access_time, file_size, comment, is_marge_tags );
		}
	}
	return finf;
}

ZnkVarpAry
EstFInfList_create( void )
{
	return ZnkVarpAry_create( true );
}
void
EstFInfList_destroy( ZnkVarpAry finf_list )
{
	if( finf_list ){
		ZnkVarpAry_destroy( finf_list );
	}
}
size_t
EstFInfList_size( const ZnkVarpAry finf_list )
{
	return ZnkVarpAry_size( finf_list );
}
EstFInf
EstFInfList_at( const ZnkVarpAry finf_list, size_t idx )
{
	return I_atEstFInf( finf_list, idx );
}
void
EstFInfList_shiftToLast( ZnkVarpAry finf_list, size_t target_idx )
{
	const size_t size = ZnkVarpAry_size( finf_list );
	ZnkVarp* ary_ptr = ZnkVarpAry_ary_ptr( finf_list );
	size_t idx;
	for( idx=target_idx; idx<size-1; ++idx ){
		/* target_idx”Ô–Ú‚Ì—v‘f(pointer)‚ðÅŒã”ö‚Ö‚¸‚ç‚· */
		Znk_SWAP( ZnkVarp, ary_ptr[ idx+0 ], ary_ptr[ idx+1 ] );
	}
}
bool
EstFInfList_erase_byIdx( ZnkVarpAry finf_list, size_t target_idx )
{
	return ZnkVarpAry_erase_byIdx( finf_list, target_idx );
}
size_t
EstFInfList_findIdx_byName( ZnkVarpAry finf_list, const char* query_name )
{
	return ZnkVarpAry_findIdx_byName( finf_list, query_name, Znk_NPOS, false );
}

const char*
EstFInf_id( const EstFInf finf )
{
	return ZnkStr_cstr( finf->id_ );
}
const char*
EstFInf_file_path( const EstFInf finf )
{
	return ZnkStr_cstr( finf->file_path_ );
}
const char*
EstFInf_file_tags( const EstFInf finf )
{
	return ZnkStr_cstr( finf->file_tags_ );
}
void
EstFInf_access_time( const EstFInf finf, ZnkDate* access_time )
{
	*access_time = finf->access_time_;
}
void
EstFInf_set_access_time( EstFInf finf, const ZnkDate* access_time )
{
	finf->access_time_ = *access_time;
}
size_t
EstFInf_file_size( const EstFInf finf )
{
	return finf->file_size_;
}
const char*
EstFInf_comment( const EstFInf finf )
{
	return ZnkStr_cstr( finf->comment_ );
}
bool
EstFInfList_load( ZnkVarpAry finf_list, ZnkVarpAry sqi_vars, const char* filename )
{
	size_t idx;
	ZnkMyf myf = ZnkMyf_create();
	bool   result = false;

	result = ZnkMyf_load( myf, filename );
	if( result ){
		size_t num_of_sec = ZnkMyf_numOfSection( myf );
		size_t sec_idx;
		for( sec_idx=0; sec_idx<num_of_sec; ++sec_idx ){
			ZnkMyfSection sec = ZnkMyf_atSection( myf, sec_idx );
			const char* sec_name = ZnkMyfSection_name( sec );
			if( ZnkS_eq( sec_name, "sqi_vars" ) ){
				if( sqi_vars ){
					/* SQI-Section */
					ZnkVarpAry   vars = ZnkMyfSection_vars( sec );
					const size_t size = ZnkVarpAry_size( vars );
					for( idx=0; idx<size; ++idx ){
						ZnkVarp src_var = ZnkVarpAry_at( vars, idx );
						ZnkVarp dst_var = ZnkVarp_create( ZnkVar_name_cstr(src_var), "", 0, ZnkPrim_e_Str, NULL );
						ZnkVar_set_val_Str( dst_var, ZnkVar_cstr( src_var ), ZnkVar_str_leng( src_var ) );
						ZnkVarpAry_push_bk( sqi_vars, dst_var );
					}
				} else {
					/* ‚±‚Ìê‡’Pƒ‚Ésqi_vars‚ð“Ç‚Ý”ò‚Î‚· */
				}
			} else {
				const char* file_path = NULL;
				const char* file_tags = NULL;
				ZnkDate     access_time = { 0 };
				size_t      file_size = 0;
				const char* comment   = NULL;

				ZnkVarpAry   vars = ZnkMyfSection_vars( sec );
				const size_t size = ZnkVarpAry_size( vars );
				for( idx=0; idx<size; ++idx ){
					ZnkVarp var = ZnkVarpAry_at( vars, idx );
					const char* name = ZnkVar_name_cstr( var );
					if( ZnkS_eq( name, "file_path" ) ){
						file_path = ZnkVar_cstr( var );
					} else if( ZnkS_eq( name, "file_tags" ) ){
						file_tags = ZnkVar_cstr( var );
					} else if( ZnkS_eq( name, "access_time" ) ){
						ZnkDate_scanStr( &access_time, ZnkVar_cstr( var ), ZnkDateStr_e_Std );
					} else if( ZnkS_eq( name, "file_size" ) ){
						if( !ZnkS_getSzU( &file_size, ZnkVar_cstr( var ) ) ){
							file_size = Znk_NPOS;
						}
					} else if( ZnkS_eq( name, "comment" ) ){
						comment = ZnkVar_cstr( var );
					}
				}

				EstFInfList_add( finf_list, sec_name, file_path, file_tags, &access_time, file_size, comment );

			}
		}
	}

	ZnkMyf_destroy( myf );
	return result;
}

bool
EstFInfList_save( ZnkVarpAry finf_list, EstSQI sqi, const char* filename, bool is_marge_tags, bool is_modesty )
{
	const size_t size = EstFInfList_size( finf_list );
	ZnkMyf  myf    = ZnkMyf_create();
	bool    result = false;
	size_t  idx;

	ZnkMyf_set_quote( myf, "['", "']" );

	/* SQI-Section */
	if( sqi ){
		ZnkVarpAry sqi_vars = ZnkMyf_intern_vars( myf, "sqi_vars" );
		EstSQI_convert_toSQIVars( sqi_vars, sqi );
	}

	for( idx=0; idx<size; ++idx ){
		EstFInf finf = EstFInfList_at( finf_list, idx );
		const char*  id        = EstFInf_id( finf );
		const char*  file_path = EstFInf_file_path( finf );
		const char*  file_tags = EstFInf_file_tags( finf );
		ZnkDate      access_time = { 0 };
		const size_t file_size = EstFInf_file_size( finf );
		const char*  comment   = EstFInf_comment( finf );

		EstFInf_access_time( finf, &access_time );
		EstRecord_regist( myf, id, file_path, file_tags, &access_time, file_size, comment, is_marge_tags, is_modesty );
	}

	result = ZnkMyf_save( myf, filename );
	ZnkMyf_destroy( myf );
	return result;
}

static int
func_compare( const void* vec1, size_t vec1_idx, const void* vec2, size_t vec2_idx )
{
	const ZnkVarpAry finf_list1 = Znk_force_ptr_cast( ZnkVarpAry, vec1 );
	const ZnkVarpAry finf_list2 = Znk_force_ptr_cast( ZnkVarpAry, vec2 );
	const EstFInf finf1 = EstFInfList_at( finf_list1, vec1_idx );
	const EstFInf finf2 = EstFInfList_at( finf_list2, vec2_idx );
	int compare_ret = 0;
	ZnkDate access_time1 = { 0 };
	ZnkDate access_time2 = { 0 };

	EstFInf_access_time( finf1, &access_time1 );
	EstFInf_access_time( finf2, &access_time2 );

	compare_ret = ZnkDate_compareDay( &access_time1, &access_time2 );
	if( compare_ret == 0 ){
		long diff_sec = ZnkDate_diffSecond( &access_time1, &access_time2, 0 );
		compare_ret = ( diff_sec > 0 ) ? 1 :
			( diff_sec < 0 ) ? -1 :
			0;
	}
	return compare_ret;
}
static void
func_swap( void* vec1, size_t vec1_idx, void* vec2, size_t vec2_idx )
{
	ZnkVarpAry finf_list1 = Znk_force_ptr_cast( ZnkVarpAry, vec1 );
	ZnkVarpAry finf_list2 = Znk_force_ptr_cast( ZnkVarpAry, vec2 );
	ZnkVarp* vars1 = ZnkVarpAry_ary_ptr( finf_list1 );
	ZnkVarp* vars2 = ZnkVarpAry_ary_ptr( finf_list2 );
	Znk_SWAP( ZnkVarp, vars1[ vec1_idx ], vars2[ vec2_idx ] );
}
void
EstFInfList_sort( ZnkVarpAry finf_list )
{
	ZnkAlgoVec_quicksort( finf_list, 0, EstFInfList_size( finf_list ),
			func_compare,
			func_swap );
}



