#include <Znk_str_dary.h>
#include <Znk_stdc.h>

static void
deleteElem( void* elem ){
	ZnkStr_destroy( (ZnkStr)elem );
}
ZnkStrDAry
ZnkStrDAry_create( bool elem_responsibility )
{
	ZnkElemDeleterFunc deleter = elem_responsibility ? deleteElem : NULL;
	return (ZnkStrDAry)ZnkObjDAry_create( deleter );
}
int
ZnkStrDAry_push_bk_snprintf( ZnkStrDAry dary, size_t size, const char* fmt, ... )
{
	ZnkStr zkstr;
	int ret_len;
	va_list ap;
	const size_t old_num = ZnkStrDAry_size( dary );

	ZnkStrDAry_resize( dary, old_num + 1 );
	zkstr = ZnkStrDAry_at( dary, old_num );
	va_start(ap, fmt);
	ret_len = ZnkStr_vsnprintf( zkstr, 0, size, fmt, ap );
	va_end(ap);
	return ret_len;
}

void
ZnkStrDAry_push_bk_cstr( ZnkStrDAry dary, const char* cstr, size_t cstr_leng )
{
	ZnkStr zkstr;
	const size_t old_num = ZnkStrDAry_size( dary );

	ZnkStrDAry_resize( dary, old_num + 1 );
	ZnkStrDAry_set( dary, old_num, ZnkStr_new( "" ) );
	zkstr = ZnkStrDAry_at( dary, old_num );
	ZnkStr_assign( zkstr, 0, cstr, cstr_leng );
}
size_t
ZnkStrDAry_find( ZnkStrDAry dary, size_t pos, const char* ptn, size_t ptn_leng )
{
	const size_t size = ZnkStrDAry_size( dary );
	if( size ){
		size_t idx;
		ZnkStr str;
		if( pos == Znk_NPOS ){ pos = size - 1; }
		ptn_leng = ( ptn_leng == Znk_NPOS ) ? Znk_strlen(ptn) : ptn_leng;
		for( idx=pos; idx<size; ++idx ){
			str = ZnkStrDAry_at( dary, idx );
			if( ZnkStr_eqEx( str, 0, ptn, ptn_leng ) ){
				return idx;
			}
		}
	}
	return Znk_NPOS;
}
