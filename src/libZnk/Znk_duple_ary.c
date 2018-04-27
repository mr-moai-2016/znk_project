#include "Znk_duple_ary.h"
#include "Znk_bfr.h"
#include "Znk_stdc.h"

#include "Znk_s_base.h"
#include "Znk_base.h"
#include "Znk_str.h"
#include "Znk_obj_ary.h"


struct ZnkDupleAryImpl {
	ZnkDupleKeyFuncs key_funcs_;
	ZnkDupleValFuncs val_funcs_;
	ZnkObjAry        entry_list_;
};

typedef struct ZnkDuplePrivateImpl* ZnkDuplePrivate;
struct ZnkDuplePrivateImpl {
	/* 以下公開メンバ(ZnkDuple_tagと完全一致させること) */
	const void* val_;

	/* 以下非公開メンバ */
	const void* ikey_;
};

Znk_INLINE void
I_deallocDupleValPrivate( ZnkDuplePrivate entry )
{
	Znk_free( entry );
}

Znk_INLINE void
deleteEntry( ZnkDupleAry dupa, ZnkDuplePrivate entry )
{
	if( entry ){
		dupa->key_funcs_.delete_ikey_( (void*)entry->ikey_ );
		entry->ikey_ = NULL;
		dupa->val_funcs_.delete_val_( (void*)entry->val_ );
		entry->val_  = NULL;
		I_deallocDupleValPrivate( entry );
	}
}


ZnkDupleAry
ZnkDupleAry_create(
		const ZnkDupleKeyFuncs* key_funcs,
		const ZnkDupleValFuncs* val_funcs )
{
	ZnkDupleAry dupa = (ZnkDupleAry)Znk_malloc( sizeof(struct ZnkDupleAryImpl) );

	if( key_funcs == NULL ){ key_funcs = ZnkDupleKeyFuncs_get_uval(); }
	if( val_funcs == NULL ){ val_funcs = ZnkDupleValFuncs_get_uval(); }
	dupa->key_funcs_   = *key_funcs;
	dupa->val_funcs_   = *val_funcs;

	dupa->entry_list_ = ZnkObjAry_create( NULL );
	return dupa;
}
void
ZnkDupleAry_destroy( ZnkDupleAry dupa )
{
	if( dupa ){
		ZnkObjAry_destroy( dupa->entry_list_ );
		Znk_free( dupa );
	}
}

size_t
ZnkDupleAry_size( const ZnkDupleAry dupa )
{
	return ZnkObjAry_size( dupa->entry_list_ );
}

ZnkDuple
ZnkDupleAry_atElem( ZnkDupleAry dupa, size_t idx )
{
	return (ZnkDuple)ZnkObjAry_at( dupa->entry_list_, idx );
}

Znk_INLINE ZnkDuplePrivate
I_allocDupleValPrivate( void )
{
	return (ZnkDuplePrivate)Znk_malloc( sizeof(struct ZnkDuplePrivateImpl) );
}

const void*
ZnkDuple_ikey_unsafe( const ZnkDuple elem )
{
	return ((ZnkDuplePrivate)elem)->ikey_;
}


/***
 * 存在する場合、該当するZnkDupleが返される.
 * 存在しない場合、NULLが返される.
 */
ZnkDuple
ZnkDupleAry_findElem( ZnkDupleAry dupa, const void* key )
{
	ZnkDuplePrivate entry;
	const size_t size = ZnkObjAry_size( dupa->entry_list_ );
	size_t idx;
	ZnkObj* ary_ptr = ZnkObjAry_ary_ptr( dupa->entry_list_ );
	if( ary_ptr ){
		for( idx=0; idx<size; ++idx ){
			entry = (ZnkDuplePrivate)ary_ptr[ idx ];
			if( entry ){
				if( dupa->key_funcs_.eq_(entry->ikey_, key) ){
					return (ZnkDuple)entry;
				}
			}
		}
	}
	return NULL;
}

Znk_INLINE ZnkDuplePrivate
pushEntry( ZnkDupleAry dupa, const void* key, const void* val )
{
	ZnkDuplePrivate entry = I_allocDupleValPrivate();
	entry->val_  = dupa->val_funcs_.copy_val_( val );
	entry->ikey_ = dupa->key_funcs_.make_ikey_( key );
	ZnkObjAry_push_bk( dupa->entry_list_, (ZnkObj)entry );
	return entry;
}


/***
 * is_newly:
 * 新規登録であった場合はtrue, そうではなかった場合は false を返す.
 *
 * 指定されたキーが既に存在する場合、is_update が true のときは上書きを行う.
 * そうでない場合は何も行わない.
 * 指定されたキーが存在しない場合、is_updateの指定は意味をもたない.
 */
ZnkDuple
ZnkDupleAry_regist( ZnkDupleAry dupa, const void* key, const void* val, bool is_update, bool* is_newly )
{
	ZnkDuplePrivate new_elem;
	ZnkDuplePrivate entry;
	const size_t size = ZnkObjAry_size( dupa->entry_list_ );
	size_t idx;
	ZnkObj* ary_ptr = ZnkObjAry_ary_ptr( dupa->entry_list_ );
	for( idx=0; idx<size; ++idx ){
		entry = (ZnkDuplePrivate)ary_ptr[ idx ];
		if( dupa->key_funcs_.eq_(entry->ikey_, key) ){
			/* すでに存在する. 上書き更新 or 無視 */
			if( is_update ){
				entry->val_ = dupa->val_funcs_.copy_val_( val );
			}
			if( is_newly ){ *is_newly = false; }
			return (ZnkDuple)entry;
		}
	}
	/* まだ存在しない. 新規登録 */
	new_elem = pushEntry( dupa, key, val );
	if( is_newly ){ *is_newly = true; }
	return (ZnkDuple)new_elem;
}

void
ZnkDupleAry_swap( ZnkDupleAry dupa1, ZnkDupleAry dupa2 )
{
	Znk_SWAP( struct ZnkDupleAryImpl, *dupa1, *dupa2 );
}
void
ZnkDupleAry_copy( ZnkDupleAry dst, const ZnkDupleAry src )
{
	dst->key_funcs_     = src->key_funcs_;
	dst->val_funcs_     = src->val_funcs_;

	/* table-copy */
	{
		ZnkDuplePrivate dst_entry;
		ZnkDuplePrivate src_entry;
		const size_t dst_table_size = ZnkObjAry_size( dst->entry_list_ );
		const size_t src_table_size = ZnkObjAry_size( src->entry_list_ );
		size_t idx;

		/***
		 * dst_table_size > src_table_sizeの場合、src_table_size以上の部分の
		 * 要素を解放する.
		 */
		for( idx=src_table_size; idx<dst_table_size; ++idx ){
			dst_entry = (ZnkDuplePrivate)ZnkObjAry_at( dst->entry_list_, idx );
			deleteEntry( dst, dst_entry );
			ZnkObjAry_set( dst->entry_list_, idx, NULL );
		}

		ZnkObjAry_resize( dst->entry_list_, src_table_size );

		/***
		 * dst_table_size < src_table_sizeの場合、dst_table_size以上の部分の
		 * 要素を確保する.
		 */
		for( idx=dst_table_size; idx<src_table_size; ++idx ){
			dst_entry = I_allocDupleValPrivate();
			ZnkObjAry_set( dst->entry_list_, idx, (ZnkObj)dst_entry );
		}
		for( idx=0; idx<src_table_size; ++idx ){
			src_entry = (ZnkDuplePrivate)ZnkObjAry_at( src->entry_list_, idx );
			dst_entry = (ZnkDuplePrivate)ZnkObjAry_at( dst->entry_list_, idx );
			if( src_entry == NULL ){
				if( dst_entry ){
					deleteEntry( dst, dst_entry );
					ZnkObjAry_set( dst->entry_list_, idx, NULL );
				}
			} else {
				if( dst_entry == NULL ){
					dst_entry = I_allocDupleValPrivate();
					ZnkObjAry_set( dst->entry_list_, idx, (ZnkObj)dst_entry );
				}
				/* copy */
				src->val_funcs_ = dst->val_funcs_;
				src->key_funcs_ = dst->key_funcs_;
				dst_entry->val_  = src->val_funcs_.copy_val_(  src_entry->val_  );
				dst_entry->ikey_ = src->key_funcs_.copy_ikey_( src_entry->ikey_ );
			}
		}
	}
}


/***
 * ここでは全elemのdeleteを行う.
 */
void
ZnkDupleAry_clear( ZnkDupleAry dupa )
{
	size_t idx=0;
	const size_t size = ZnkObjAry_size( dupa->entry_list_ );
	ZnkDuplePrivate entry;

	for( idx=0; idx<size; ++idx ){
		entry = (ZnkDuplePrivate)ZnkObjAry_at( dupa->entry_list_, idx );
		if( entry == NULL ){ continue; }
		deleteEntry( dupa, entry );
	}
	ZnkObjAry_resize( dupa->entry_list_, 0 );
}

/***
 * 要素ZnkObjがNULLである場合は詰める.
 */
static void
doCompaction( ZnkObjAry ary )
{
	ZnkObj obj;
	const size_t size = ZnkObjAry_size( ary );
	size_t  idx;
	size_t  nul_idx = Znk_NPOS;
	ZnkObj* ary_ptr = ZnkObjAry_ary_ptr( ary );
	for( idx=0; idx<size; ++idx ){
		obj = ary_ptr[ idx ];
		if( obj ){
			if( nul_idx != Znk_NPOS ){
				ary_ptr[ nul_idx ] = obj;
				++nul_idx;
			}
		} else {
			if( nul_idx == Znk_NPOS ){
				nul_idx = idx;
			}
		}
	}
	if( nul_idx != Znk_NPOS ){
		ZnkObjAry_resize( ary, nul_idx );
	}
}

bool
ZnkDupleAry_erase( ZnkDupleAry dupa, const void* key )
{
	ZnkDuplePrivate entry;
	const size_t size = ZnkObjAry_size( dupa->entry_list_ );
	size_t idx;
	bool erased = false;
	ZnkObj* ary_ptr = ZnkObjAry_ary_ptr( dupa->entry_list_ );
	for( idx=0; idx<size; ++idx ){
		entry = (ZnkDuplePrivate)ary_ptr[ idx ];
		if( dupa->key_funcs_.eq_(entry->ikey_, key) ){
			/* found */
			deleteEntry( dupa, entry );
			ZnkObjAry_set( dupa->entry_list_, idx, NULL );
			erased = true;
			break;
		}
	}
	if( erased ){
		doCompaction( dupa->entry_list_ );
	}
	return erased;
}

/*****************************************************************************/
/* Well-Use DupleAryFuncs */

static bool
uval_eq( const void* ikey, const void* key ) {
	return (bool)( ikey == key );
}
static const void*
uval_make_ikey( const void* key ) {
	return key;
}
static const void*
uval_copy_ikey( const void* ikey ) {
	return ikey;
}
static void
uval_delete_ikey( void* ikey ) {
	return; /* none */
}
const ZnkDupleKeyFuncs*
ZnkDupleKeyFuncs_get_uval( void )
{
	static ZnkDupleKeyFuncs st_key_funcs = {
		uval_eq,
		uval_make_ikey,
		uval_copy_ikey,
		uval_delete_ikey,
	};
	return &st_key_funcs;
}

static bool
cstr_eq( const void* ikey, const void* key ) {
	return ZnkS_eq( (const char*)ikey, (const char*)key );
}
static const void*
cstr_make_ikey( const void* key ) {
	return (void*)key;
}
static const void*
cstr_copy_ikey( const void* ikey ) {
	return (void*)ikey;
}
static void
cstr_delete_ikey( void* ikey ) {
	return; /* none */
}
const ZnkDupleKeyFuncs*
ZnkDupleKeyFuncs_get_cstr( void )
{
	static ZnkDupleKeyFuncs st_key_funcs = {
		cstr_eq,
		cstr_make_ikey,
		cstr_copy_ikey,
		cstr_delete_ikey,
	};
	return &st_key_funcs;
}

static bool
sdup_eq( const void* ikey, const void* key ) {
	return ZnkS_eq( (const char*)ikey, (const char*)key );
}
static const void*
sdup_make_ikey( const void* key ) {
	return (void*)Znk_strdup( (const char*)key );
}
static const void*
sdup_copy_ikey( const void* ikey ) {
	return (void*)Znk_strdup( (const char*)ikey );
}
static void
sdup_delete_ikey( void* ikey ) {
	Znk_free( ikey );
}
const ZnkDupleKeyFuncs*
ZnkDupleKeyFuncs_get_strdup( void )
{
	static ZnkDupleKeyFuncs st_key_funcs = {
		sdup_eq,
		sdup_make_ikey,
		sdup_copy_ikey,
		sdup_delete_ikey,
	};
	return &st_key_funcs;
}

static bool
c1st_eq( const void* ikey, const void* key ) {
	return ZnkStr_eq( (ZnkStr)ikey, (const char*)key );
}
static const void*
c1st_make_ikey( const void* key ) {
	return (const void*)ZnkStr_new( (const char*)key );
}
static const void*
c1st_copy_ikey( const void* ikey ) {
	return (const void*)ZnkStr_new( ZnkStr_cstr((ZnkStr)ikey) );
}
static void
c1st_delete_ikey( void* ikey ) {
	ZnkStr_delete( (ZnkStr)ikey );
}
const ZnkDupleKeyFuncs*
ZnkDupleKeyFuncs_get_ZnkStr( void )
{
	static ZnkDupleKeyFuncs st_key_funcs = {
		c1st_eq,
		c1st_make_ikey,
		c1st_copy_ikey,
		c1st_delete_ikey,
	};
	return &st_key_funcs;
}

static const void*
uval_copy_val( const void* val ) {
	return val;
}
static void
uval_delete_val( void* val ) {
	return; /* none */
}
const ZnkDupleValFuncs*
ZnkDupleValFuncs_get_uval( void )
{
	static ZnkDupleValFuncs st_val_funcs = {
		uval_copy_val,
		uval_delete_val,
	};
	return &st_val_funcs;
}

/* endof Well-Use DupleAryFuncs */
/*****************************************************************************/

#if 0
static void
report( ZnkDupleAry mpt )
{
	ZnkDuple elem;
	size_t size = ZnkDupleAry_size( mpt );
	size_t idx;
	ZnkF_printf_e( "report:\n" );
	for( idx=0; idx<size; ++idx ){
		ZnkDuple elem = ZnkDupleAry_atElem( mpt, idx );
		const char* key = (char*)ZnkDuple_ikey_unsafe( elem );
		//elem = ZnkDupleAry_findElem( mpt, key );
		ZnkF_printf_e( "key=[%s] val=[%d]\n", key, (int)ZnkDuple_val( elem ) );
	}
}
int main(int argc, char **argv)
{
	ZnkDupleAry mpt = ZnkDupleAry_create( ZnkDupleKeyFuncs_get_cstr(), ZnkDupleValFuncs_get_uval() );
	ZnkDupleAry_regist( mpt, "apple", (void*)10, true, NULL );
	ZnkDupleAry_regist( mpt, "orange", (void*)20, true, NULL );
	ZnkDupleAry_regist( mpt, "grape", (void*)30, true, NULL );
	report( mpt );
	ZnkDupleAry_erase( mpt, "orange" );
	report( mpt );
	ZnkDupleAry_clear( mpt );
	report( mpt );
	ZnkDupleAry_destroy( mpt );
	getchar();
	return 0;
}
#endif
