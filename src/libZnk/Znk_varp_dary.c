#include <Znk_varp_dary.h>
#include <Znk_s_base.h>

static void
deleteElem( void* elem ){
	ZnkVarp_destroy( (ZnkVarp)elem );
}
ZnkVarpDAry
ZnkVarpDAry_create( bool elem_responsibility )
{
	ZnkElemDeleterFunc deleter = elem_responsibility ? deleteElem : NULL;
	return (ZnkVarpDAry)ZnkObjDAry_create( deleter );
}
ZnkVarp
ZnkVarpDAry_find_byName( ZnkVarpDAry dary,
		const char* query_name, size_t query_name_leng, bool use_eqCase )
{
	const size_t size = ZnkVarpDAry_size( dary );
	size_t idx;
	ZnkVarp varp = NULL;
	if( query_name_leng == Znk_NPOS ){
		query_name_leng = Znk_strlen(query_name);
	}
	if( use_eqCase ){
		for( idx=0; idx<size; ++idx ){
			varp = ZnkVarpDAry_at( dary, idx );
			if( ZnkS_eqCaseEx( ZnkStr_cstr(varp->name_), query_name, query_name_leng ) ){
				/* found */
				return varp;
			}
		}
	} else {
		for( idx=0; idx<size; ++idx ){
			varp = ZnkVarpDAry_at( dary, idx );
			if( ZnkS_eqEx( ZnkStr_cstr(varp->name_), query_name, query_name_leng ) ){
				/* found */
				return varp;
			}
		}
	}
	/* not found */
	return NULL;
}

