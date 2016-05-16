#include <Znk_s_atom.h>
#include <Znk_str_ary.h>
#include <Znk_stdc.h>
#include <assert.h>

typedef struct { int dummy_; }* StrDB;

static StrDB
VStrDB_create( void )
{
	StrDB strdb = (StrDB)ZnkStrAry_create( true );
	return strdb;
}
static void
VStrDB_destroy( StrDB strdb )
{
	if( strdb ){
		ZnkStrAry_destroy( (ZnkStrAry)strdb );
	}
}
static ZnkSAtomId
VStrDB_intern( StrDB strdb, const char* key )
{
	const ZnkStrAry vstr = (ZnkStrAry)strdb;
	const size_t leng = Znk_strlen( key );
	const size_t id = ZnkStrAry_find( vstr, 0, key, leng );
	if( id == Znk_NPOS ){
		ZnkStrAry_push_bk_cstr( vstr, key, leng );
		return ZnkStrAry_size( vstr ) - 1;
	}
	return (ZnkSAtomId)id;
}
static const char*
VStrDB_cstr( StrDB strdb, ZnkSAtomId id )
{
	return ZnkStrAry_at_cstr( (ZnkStrAry)strdb, id );
}

typedef void        (*AtomDBFuncT_destroy)( StrDB );
typedef ZnkSAtomId  (*AtomDBFuncT_intern)( StrDB, const char* );
typedef const char* (*AtomDBFuncT_cstr)( StrDB, ZnkSAtomId );

struct ZnkSAtomDBImpl {
	StrDB strdb_;
	AtomDBFuncT_destroy destroy_;
	AtomDBFuncT_intern  intern_;
	AtomDBFuncT_cstr    cstr_;
};

ZnkSAtomDB
ZnkSAtomDB_create( void )
{
	ZnkSAtomDB atomdb = (ZnkSAtomDB) Znk_malloc( sizeof( struct ZnkSAtomDBImpl ) ); 
	atomdb->strdb_   = VStrDB_create();
	atomdb->destroy_ = VStrDB_destroy;
	atomdb->intern_  = VStrDB_intern;
	atomdb->cstr_    = VStrDB_cstr;
	return atomdb;
}
void
ZnkSAtomDB_destroy( ZnkSAtomDB atomdb )
{
	if( atomdb ){
		atomdb->destroy_( atomdb->strdb_ );
		Znk_free( atomdb );
	}
}
ZnkSAtomId
ZnkSAtomDB_intern( ZnkSAtomDB atomdb, const char* key )
{
	assert( atomdb != NULL );
	return atomdb->intern_( atomdb->strdb_, key );
}
const char*
ZnkSAtomDB_cstr( ZnkSAtomDB atomdb, ZnkSAtomId id )
{
	assert( atomdb != NULL );
	return atomdb->cstr_( atomdb->strdb_, id );
}
void
ZnkSAtomDB_swap( ZnkSAtomDB atomdb1, ZnkSAtomDB atomdb2 )
{
	Znk_SWAP( StrDB, atomdb1->strdb_, atomdb2->strdb_ );
	Znk_SWAP( AtomDBFuncT_destroy, atomdb1->destroy_, atomdb2->destroy_ );
	Znk_SWAP( AtomDBFuncT_intern,  atomdb1->intern_,  atomdb2->intern_ );
	Znk_SWAP( AtomDBFuncT_cstr,    atomdb1->cstr_,    atomdb2->cstr_ );
}

