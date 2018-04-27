#include <Est_recentory.h>
#include <Est_search_manager.h>
#include <Est_finf.h>
#include <Est_unid.h>

#include <Rano_file_info.h>

#include <Znk_varp_ary.h>
#include <Znk_missing_libc.h>
#include <Znk_str_ex.h>
#include <Znk_date.h>
#include <Znk_dir.h>
#include <Znk_thread.h>

struct EstRecentoryImpl {
	ZnkVarpAry finf_list_;
	ZnkVarpAry sqi_vars_;
	ZnkStr     title_;
};

EstRecentory 
EstRecentory_create( void )
{
	EstRecentory recent = Znk_malloc( sizeof( struct EstRecentoryImpl ) ); 
	recent->finf_list_ = EstFInfList_create();
	recent->sqi_vars_  = ZnkVarpAry_create( true );
	recent->title_     = ZnkStr_new( "" );
	return recent;
}
void
EstRecentory_destroy( EstRecentory recent )
{
	if( recent ){
		EstFInfList_destroy( recent->finf_list_ );
		ZnkVarpAry_destroy( recent->sqi_vars_ );
		ZnkStr_delete( recent->title_ );
		Znk_free( recent );
	}
}

static bool
waitRecentoryTasking( void )
{
	size_t count = 3;
	while( count ){
		/* cache_taskが稼動中で、recentory.myfにアクセス中である場合 */
		if( ZnkDir_getType( "__recentory_tasking__.lock" ) == ZnkDirType_e_File ){
			ZnkThread_sleep( 500 );
		} else {
			break;
		}
		--count;
	}
	if( count == 0 ){
		/* おそらくなんらかの理由でrecentory_tasking.lockが消されず残っている */
		ZnkDir_deleteFile_byForce( "__recentory_tasking__.lock" );
		return false;
	}
	return true;
}

bool
EstRecentory_load( EstRecentory recent, const char* filename )
{
	bool result = false;
	waitRecentoryTasking();
	result = EstFInfList_load( recent->finf_list_, recent->sqi_vars_, filename );
	return result;
}
bool
EstRecentory_save( EstRecentory recent, const char* filename, bool is_marge_tags )
{
	bool result = false;
	bool is_modesty = false;
	EstSQI sqi = EstSQI_create( recent->sqi_vars_ );
	waitRecentoryTasking();
	result = EstFInfList_save( recent->finf_list_, sqi, filename, is_marge_tags, is_modesty );
	EstSQI_destroy( sqi );
	return result;
}

ZnkStr
EstRecentory_title( EstRecentory recent )
{
	return recent->title_;
}
ZnkVarpAry
EstRecentory_finf_list( EstRecentory recent )
{
	return recent->finf_list_;
}

static size_t
findObj_byVPath( EstRecentory recent, const char* query_vpath )
{
	ZnkVarpAry ary = recent->finf_list_;
	const size_t size = EstFInfList_size( ary );
	size_t idx;
	EstFInf finf = NULL;
	const char* vpath = NULL;
	for( idx=0; idx<size; ++idx ){
		finf  = EstFInfList_at( ary, idx );
		vpath = EstFInf_file_path( finf );
		if( ZnkS_eq( vpath, query_vpath ) ){
			/* found */
			return idx;
		}
	}
	/* not found */
	return Znk_NPOS;
}

void
EstRecentory_add2( EstRecentory recent, const char* filename, const char* dst_box, const char* fsys_path,
		const char* src_vpath, ZnkStr msg )
{
	ZnkStr    dst_vpath = ZnkStr_newf( "%s/%s", dst_box, filename );
	ZnkDate   access_time = { 0 };
	size_t    dst_idx = findObj_byVPath( recent, ZnkStr_cstr(dst_vpath) );

	ZnkDate_getCurrent( &access_time );

	if( dst_idx == Znk_NPOS ){
		/* New */
		const char* file_tags = "";
		uint64_t    file_size = 0;
		const char* comment = "";
		char name[ 256 ] = "";
		char id[ 256 ] = "";
	
		EstUNID_issue( id, sizeof(id) );
		Znk_snprintf( name, sizeof(name), "recentory_%s", id );
		RanoFileInfo_getFileSize( fsys_path, &file_size );
	
		EstFInfList_add( recent->finf_list_, name,
				ZnkStr_cstr(dst_vpath),
				file_tags,
				&access_time,
				(size_t)file_size,
				comment );
	
	} else {
		/* Update */
		EstFInf finf = EstFInfList_at( recent->finf_list_, dst_idx );
		/* update only access_time_str */
		EstFInf_set_access_time( finf, &access_time );
		EstFInfList_shiftToLast( recent->finf_list_, dst_idx );
	}

	if( src_vpath && !ZnkStr_eq( dst_vpath, src_vpath ) ){
		size_t src_idx = findObj_byVPath( recent, src_vpath );
		if( src_idx != Znk_NPOS ){
			EstFInfList_erase_byIdx( recent->finf_list_, src_idx );
		}
	}

	if( msg ){
		ZnkStr_addf( msg, "  access_time=[" );
		ZnkDate_getStr( msg, Znk_NPOS, &access_time, ZnkDateStr_e_Std );
		ZnkStr_addf( msg, "].\n" );
	}
	ZnkStr_delete( dst_vpath );
}

size_t
EstRecentory_removeOld( EstRecentory recent )
{
	const size_t size = EstFInfList_size( recent->finf_list_ );
	size_t  count = 0;
	size_t  idx;
	EstFInf finf;
	ZnkDate access_time = { 0 };
	ZnkDate current = { 0 };
	ZnkDate threshold = { 0 };
	unsigned int ndays_ago = 7;

	ZnkDate_getCurrent( &current );
	ZnkDate_getNDaysAgo( &threshold, &current, ndays_ago );

	for( idx=size; idx; --idx ){
		finf = EstFInfList_at( recent->finf_list_, idx-1 );
		EstFInf_access_time( finf, &access_time );
		if( ZnkDate_compareDay( &access_time, &threshold ) < 0 ){
			EstFInfList_erase_byIdx( recent->finf_list_, idx-1 );
			++count;
		}
	}
	return count;
}

