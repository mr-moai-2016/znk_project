#include <Znk_nset.h>
#include <Znk_vpod.h>
#include <Znk_stdc.h>

ZnkNSet32
ZnkNSet32_create( void )
{
	ZnkBfr nset = ZnkBfr_create_null();
	return (ZnkNSet32)nset;
}
void
ZnkNSet32_destroy( ZnkNSet32 nset )
{
	ZnkBfr_destroy( (ZnkBfr)nset );
}
uint32_t
ZnkNSet32_at( const ZnkNSet32 nset, size_t idx )
{
	return ZnkVUInt32_at( (ZnkBfr)nset, idx );
}
size_t
ZnkNSet32_size( const ZnkNSet32 nset )
{
	return ZnkVUInt32_size( (ZnkBfr)nset );
}
size_t
ZnkNSet32_find( ZnkNSet32 nset, uint32_t query_val )
{
	const size_t size = ZnkVUInt32_size( (ZnkBfr)nset );
	size_t   idx;
	uint32_t val;
	for( idx=0; idx<size; ++idx ){
		val = ZnkNSet32_at( nset, idx );
		if( val == query_val ){
			return idx;
		}
	}
	return Znk_NPOS;
}
void
ZnkNSet32_push( ZnkNSet32 nset, uint32_t val )
{
	if( ZnkNSet32_find( nset, val ) == Znk_NPOS ){
		ZnkVUInt32_push_bk( (ZnkBfr)nset, val );
	}
}
void
ZnkNSet32_erase( ZnkNSet32 nset, uint32_t val )
{
	const size_t idx = ZnkNSet32_find( nset, val );
	if( idx != Znk_NPOS ){
		ZnkBfr_erase( (ZnkBfr)nset, idx*sizeof(uint32_t), sizeof(uint32_t) );
	}
}
void
ZnkNSet32_copy( ZnkNSet32 dst, const ZnkNSet32 src )
{
	ZnkBfr_copy( (ZnkBfr)dst, (ZnkBfr)src );
}
bool
ZnkNSet32_eq( const ZnkNSet32 nset1, const ZnkNSet32 nset2 )
{
	const size_t size1 = ZnkNSet32_size( nset1 );
	const size_t size2 = ZnkNSet32_size( nset2 );
	if( size1 == size2 ){
		size_t idx;
		uint32_t val1;
		uint32_t val2;
		for( idx=0; idx<size1; ++idx ){
			val1 = ZnkNSet32_at( nset1, idx );
			val2 = ZnkNSet32_at( nset2, idx );
			if( val1 != val2 ){
				return false;
			}
		}
		return true;
	}
	return false;
}
void
ZnkNSet32_print_e( const ZnkNSet32 nset )
{
	const size_t size = ZnkNSet32_size( nset );
	size_t idx;
	Znk_printf_e("{");
	for( idx=0; idx<size; ++idx ){
		Znk_printf_e("%I32u ", ZnkNSet32_at(nset,idx) );
	}
	Znk_printf_e("}\n");
}

