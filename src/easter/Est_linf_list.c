#include <Est_linf_list.h>
#include <Est_base.h>

#include <Rano_file_info.h>

#include <Znk_varp_ary.h>
#include <Znk_stdc.h>
#include <Znk_prim.h>
#include <Znk_myf.h>
#include <Znk_algo_vec.h>
#include <Znk_dir.h>
#include <Znk_missing_libc.h>

struct EstLInfImpl {
	ZnkStr  id_;
	ZnkStr  url_;
	ZnkStr  comment_;
};

static EstLInf
createEstLInf( const char* id, const char* url, const char* comment )
{
	EstLInf linf = Znk_malloc( sizeof( struct EstLInfImpl ) );
	linf->id_        = ZnkStr_new( id );
	linf->url_ = ZnkStr_new( url );
	linf->comment_ = ZnkStr_new( comment ? comment : "" );
	return linf;
}
static void
destroyEstLInf( EstLInf linf )
{
	if( linf ){
		ZnkStr_delete( linf->id_ );
		ZnkStr_delete( linf->url_ );
		ZnkStr_delete( linf->comment_ );
		Znk_free( linf );
	}
}

static void
deleteElem( void* elem )
{
	destroyEstLInf( (EstLInf)elem );
}

void
EstLInfList_clear( ZnkVarpAry linf_list )
{
	ZnkVarpAry_clear( linf_list );
}

Znk_INLINE EstLInf
I_atEstLInf( const ZnkVarpAry linf_list, size_t idx )
{
	ZnkVarp var = ZnkVarpAry_at( linf_list, idx );
	ZnkPrimp prim = ZnkVar_prim( var );
	return (EstLInf)ZnkPrim_ptr( prim );
}

size_t
EstLInfList_find( ZnkVarpAry linf_list, const char* query_id )
{
	const size_t size = EstLInfList_size( linf_list );
	size_t idx;
	EstLInf linf = NULL;
	for( idx=0; idx<size; ++idx ){
		linf = I_atEstLInf( linf_list, idx );
		if( ZnkS_eq( query_id, EstLInf_id(linf) ) ){
			/* found */
			return idx;
		}
	}
	return Znk_NPOS;
}

EstLInf
EstLInfList_add( ZnkVarpAry linf_list, const char* id, const char* url, const char* comment )
{
	EstLInf linf = createEstLInf( id, url, comment );
	ZnkVarp var  = ZnkVarp_create( id, "", 0, ZnkPrim_e_Ptr, deleteElem );
	ZnkPrimp prim = ZnkVar_prim( var );
	ZnkPrim_set_ptr( prim, linf );
	ZnkVarpAry_push_bk( linf_list, var );
	return linf;
}
EstLInf
EstLInfList_regist( ZnkVarpAry linf_list, const char* id, const char* url, const char* comment )
{
	const size_t idx = EstLInfList_find( linf_list, id );
	EstLInf linf = NULL;
	if( idx == Znk_NPOS ){
		linf = EstLInfList_add( linf_list, id, url, comment );
	} else {
		linf = EstLInfList_at( linf_list, idx );
		ZnkStr_set( linf->url_, url );
		ZnkStr_set( linf->comment_, comment );
	}
	return linf;
}

ZnkVarpAry
EstLInfList_create( void )
{
	return ZnkVarpAry_create( true );
}
void
EstLInfList_destroy( ZnkVarpAry linf_list )
{
	if( linf_list ){
		ZnkVarpAry_destroy( linf_list );
	}
}
size_t
EstLInfList_size( const ZnkVarpAry linf_list )
{
	return ZnkVarpAry_size( linf_list );
}
EstLInf
EstLInfList_at( const ZnkVarpAry linf_list, size_t idx )
{
	return I_atEstLInf( linf_list, idx );
}
void
EstLInfList_shiftToLast( ZnkVarpAry linf_list, size_t target_idx )
{
	const size_t size = ZnkVarpAry_size( linf_list );
	ZnkVarp* ary_ptr = ZnkVarpAry_ary_ptr( linf_list );
	size_t idx;
	for( idx=target_idx; idx<size-1; ++idx ){
		/* target_idx”Ô–Ú‚Ì—v‘f(pointer)‚ðÅŒã”ö‚Ö‚¸‚ç‚· */
		Znk_SWAP( ZnkVarp, ary_ptr[ idx+0 ], ary_ptr[ idx+1 ] );
	}
}
bool
EstLInfList_erase_byIdx( ZnkVarpAry linf_list, size_t target_idx )
{
	return ZnkVarpAry_erase_byIdx( linf_list, target_idx );
}
size_t
EstLInfList_findIdx_byName( ZnkVarpAry linf_list, const char* query_name )
{
	return ZnkVarpAry_findIdx_byName( linf_list, query_name, Znk_NPOS, false );
}

const char*
EstLInf_id( const EstLInf linf )
{
	return ZnkStr_cstr( linf->id_ );
}
const char*
EstLInf_url( const EstLInf linf )
{
	return ZnkStr_cstr( linf->url_ );
}
const char*
EstLInf_comment( const EstLInf linf )
{
	return ZnkStr_cstr( linf->comment_ );
}
void
EstLInf_set_url( const EstLInf linf, const char* url )
{
	ZnkStr_set( linf->url_, url );
}
void
EstLInf_set_comment( const EstLInf linf, const char* comment )
{
	ZnkStr_set( linf->comment_, comment );
}
bool
EstLInfList_load( ZnkVarpAry linf_list, const char* filename )
{
	ZnkMyf myf = ZnkMyf_create();
	bool   result = false;

	result = ZnkMyf_load( myf, filename );
	if( result ){
		size_t num_of_sec = ZnkMyf_numOfSection( myf );
		size_t sec_idx;
		for( sec_idx=0; sec_idx<num_of_sec; ++sec_idx ){
			ZnkMyfSection sec = ZnkMyf_atSection( myf, sec_idx );
			ZnkVarpAry vars = ZnkMyfSection_vars( sec );
			const char* sec_name = ZnkMyfSection_name( sec );
			if( ZnkS_eq( sec_name, "config" ) ){
				/* configc-Section */
				ZnkVarp var = ZnkVarpAry_findObj_byName( vars, "fmt_version", Znk_NPOS, false );
				Znk_UNUSED( var );
			} else {
				const char* url     = NULL;
				const char* comment = NULL;
				ZnkVarp var = NULL;

				var = ZnkVarpAry_findObj_byName( vars, "url", Znk_NPOS, false );
				if( var ){
					url = ZnkVar_cstr( var );
				}

				var = ZnkVarpAry_findObj_byName( vars, "name", Znk_NPOS, false );
				if( var ){
					comment = ZnkVar_cstr( var );
				}

				EstLInfList_add( linf_list, sec_name, url, comment );
			}
		}
	}

	ZnkMyf_destroy( myf );
	return result;
}

bool
EstLInfList_save( ZnkVarpAry linf_list, const char* filename )
{
	const size_t size = EstLInfList_size( linf_list );
	ZnkMyf  myf    = ZnkMyf_create();
	bool    result = false;
	size_t  idx;

	ZnkMyf_set_quote( myf, "['", "']" );

	{
		ZnkVarpAry config = ZnkMyf_intern_vars( myf, "config" );
		ZnkVarp var  = ZnkVarp_create( "fmt_version", "", 0, ZnkPrim_e_Str, deleteElem );
		ZnkVar_set_val_Str( var, "1", 1 );
		ZnkVarpAry_push_bk( config, var );
	}

	for( idx=0; idx<size; ++idx ){
		EstLInf linf = EstLInfList_at( linf_list, idx );
		const char*  id      = EstLInf_id( linf );
		const char*  url     = EstLInf_url( linf );
		const char*  comment = EstLInf_comment( linf );
		ZnkVarpAry vars = ZnkMyf_intern_vars( myf, id );
		ZnkStr     str_dst = NULL;
		str_dst = EstBase_registStrToVars( vars, "url", url );
		str_dst = EstBase_registStrToVars( vars, "name", comment );
		EstBase_escapeToAmpForm( str_dst );
	}

	result = ZnkMyf_save( myf, filename );
	ZnkMyf_destroy( myf );
	return result;
}


