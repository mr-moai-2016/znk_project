#include <Znk_varp_ary.h>
#include <Znk_s_base.h>

static void
deleteElem( void* elem ){
	ZnkVarp_destroy( (ZnkVarp)elem );
}
ZnkVarpAry
ZnkVarpAry_create( bool elem_responsibility )
{
	ZnkElemDeleterFunc deleter = elem_responsibility ? deleteElem : NULL;
	return (ZnkVarpAry)ZnkObjAry_create( deleter );
}
ZnkVarp
ZnkVarpAry_find_byName( ZnkVarpAry ary,
		const char* query_name, size_t query_name_leng, bool use_eqCase )
{
	const size_t size = ZnkVarpAry_size( ary );
	size_t idx;
	ZnkVarp varp = NULL;
	if( query_name_leng == Znk_NPOS ){
		query_name_leng = Znk_strlen(query_name);
	}
	if( use_eqCase ){
		for( idx=0; idx<size; ++idx ){
			varp = ZnkVarpAry_at( ary, idx );
			if(  ZnkStr_leng(varp->name_) == query_name_leng
			  && ZnkS_eqCaseEx( ZnkStr_cstr(varp->name_), query_name, query_name_leng ) )
			{
				/* found */
				return varp;
			}
		}
	} else {
		for( idx=0; idx<size; ++idx ){
			varp = ZnkVarpAry_at( ary, idx );
			if(  ZnkStr_leng(varp->name_) == query_name_leng
			  && ZnkS_eqEx( ZnkStr_cstr(varp->name_), query_name, query_name_leng ) )
			{
				/* found */
				return varp;
			}
		}
	}
	/* not found */
	return NULL;
}

