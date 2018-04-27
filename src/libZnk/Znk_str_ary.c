#include <Znk_str_ary.h>
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <Znk_algo_vec.h>

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
void
ZnkStrAry_resize( ZnkStrAry ary, size_t size, const char* init_val )
{
	const size_t old_size = ZnkStrAry_size( ary );
	if( old_size > size ){
		ZnkObjAry_M_RESIZE( ary, size );
	} else if( old_size < size ){
		size_t idx;
		ZnkObjAry_M_RESIZE( ary, size );
		if( init_val ){
			for( idx=old_size; idx<size; ++idx ){
				ZnkStrAry_set( ary, idx, ZnkStr_new( init_val ) );
			}
		}
	}
}
int
ZnkStrAry_push_bk_snprintf( ZnkStrAry ary, size_t size, const char* fmt, ... )
{
	ZnkStr zkstr;
	int ret_len;
	va_list ap;
	const size_t old_num = ZnkStrAry_size( ary );

	ZnkStrAry_resize( ary, old_num + 1, "" );
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

	ZnkStrAry_resize( ary, old_num + 1, "" );
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

static int compareStr( const void* vec1, size_t vec1_idx, const void* vec2, size_t vec2_idx )
{
	ZnkStrAry ary1 = Znk_force_ptr_cast( ZnkStrAry, vec1 );
	ZnkStrAry ary2 = Znk_force_ptr_cast( ZnkStrAry, vec2 );
	ZnkStr str1 = ZnkStrAry_at( ary1, vec1_idx );
	ZnkStr str2 = ZnkStrAry_at( ary2, vec2_idx );
	return Znk_strcmp( ZnkStr_cstr(str1), ZnkStr_cstr(str2) );
}
static void swapStr( void* vec1, size_t vec1_idx, void* vec2, size_t vec2_idx )
{
	ZnkStrAry ary1 = Znk_force_ptr_cast( ZnkStrAry, vec1 );
	ZnkStrAry ary2 = Znk_force_ptr_cast( ZnkStrAry, vec2 );
	ZnkStr str1 = ZnkStrAry_at( ary1, vec1_idx );
	ZnkStr str2 = ZnkStrAry_at( ary2, vec2_idx );
	ZnkStr_swap( str1, str2 );
}

void
ZnkStrAry_sort( ZnkStrAry ary )
{
	ZnkAlgoVec_quicksort( ary, 0, ZnkStrAry_size(ary),
			compareStr, swapStr );
}

