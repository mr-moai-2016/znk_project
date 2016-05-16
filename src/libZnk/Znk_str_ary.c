#include <Znk_str_ary.h>
#include <Znk_stdc.h>
#include <Znk_s_base.h>

static void
deleteElem( void* elem ){
	ZnkStr_destroy( (ZnkStr)elem );
}
ZnkStrAry
ZnkStrAry_create( bool elem_responsibility )
{
	ZnkElemDeleterFunc deleter = elem_responsibility ? deleteElem : NULL;
	return (ZnkStrAry)ZnkObjAry_create( deleter );
}
int
ZnkStrAry_push_bk_snprintf( ZnkStrAry ary, size_t size, const char* fmt, ... )
{
	ZnkStr zkstr;
	int ret_len;
	va_list ap;
	const size_t old_num = ZnkStrAry_size( ary );

	ZnkStrAry_resize( ary, old_num + 1 );
	ZnkStrAry_set( ary, old_num, ZnkStr_new( "" ) );
	zkstr = ZnkStrAry_at( ary, old_num );

	va_start(ap, fmt);
	ret_len = ZnkStr_vsnprintf( zkstr, 0, size, fmt, ap );
	va_end(ap);
	return ret_len;
}

void
ZnkStrAry_push_bk_cstr( ZnkStrAry ary, const char* cstr, size_t cstr_leng )
{
	ZnkStr zkstr;
	const size_t old_num = ZnkStrAry_size( ary );

	ZnkStrAry_resize( ary, old_num + 1 );
	ZnkStrAry_set( ary, old_num, ZnkStr_new( "" ) );
	zkstr = ZnkStrAry_at( ary, old_num );

	ZnkStr_assign( zkstr, 0, cstr, cstr_leng );
}
size_t
ZnkStrAry_find( ZnkStrAry ary, size_t pos, const char* query, size_t query_leng )
{
	const size_t size = ZnkStrAry_size( ary );
	if( size ){
		size_t idx;
		ZnkStr str;
		if( pos == Znk_NPOS ){ pos = size - 1; }
		Znk_setStrLen_ifNPos( &query_leng, query );
		for( idx=pos; idx<size; ++idx ){
			str = ZnkStrAry_at( ary, idx );
			if( ZnkStr_eqEx( str, 0, query, query_leng ) ){
				return idx;
			}
		}
	}
	return Znk_NPOS;
}
size_t
ZnkStrAry_find_isMatch( ZnkStrAry ary, size_t pos, const char* query, size_t query_leng,
		ZnkS_FuncT_IsMatch is_match_func )
{
	const size_t size = ZnkStrAry_size( ary );
	if( size ){
		size_t idx;
		ZnkStr str;
		if( pos == Znk_NPOS ){ pos = size - 1; }
		Znk_setStrLen_ifNPos( &query_leng, query );
		for( idx=pos; idx<size; ++idx ){
			str = ZnkStrAry_at( ary, idx );
			if( is_match_func( ZnkStr_cstr(str), ZnkStr_leng(str), query, query_leng ) ){
				/* found */
				return idx;
			}
		}
	}
	/* not found */
	return Znk_NPOS;
}
