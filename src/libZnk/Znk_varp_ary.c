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

size_t
ZnkVarpAry_findIdx_byName( ZnkVarpAry ary,
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
				return idx;
			}
		}
	} else {
		for( idx=0; idx<size; ++idx ){
			varp = ZnkVarpAry_at( ary, idx );
			if(  ZnkStr_leng(varp->name_) == query_name_leng
			  && ZnkS_eqEx( ZnkStr_cstr(varp->name_), query_name, query_name_leng ) )
			{
				/* found */
				return idx;
			}
		}
	}
	/* not found */
	return Znk_NPOS;
}
size_t
ZnkVarpAry_findIdx_byStrVal( ZnkVarpAry ary,
		const char* query_val, size_t query_val_leng )
{
	const size_t size = ZnkVarpAry_size( ary );
	size_t  idx;
	ZnkVarp varp = NULL;
	if( query_val_leng == Znk_NPOS ){
		query_val_leng = Znk_strlen( query_val );
	}
	for( idx=0; idx<size; ++idx ){
		varp = ZnkVarpAry_at( ary, idx );
		if(  ZnkVar_str_leng(varp) == query_val_leng
		  && ZnkS_eqEx( ZnkVar_cstr(varp), query_val, query_val_leng ) )
		{
			/* found */
			return idx;
		}
	}
	/* not found */
	return Znk_NPOS;
}


