#include <Mkf_sset.h>
#include <Znk_str_ary.h>

bool
MkfSSet_add( ZnkStrAry sset, const char* str )
{
	if( ZnkStrAry_find( sset, 0, str, Znk_NPOS ) == Znk_NPOS ){
		ZnkStrAry_push_bk_cstr( sset, str, Znk_NPOS );
		return true;
	}
	return false;
}

bool
MkfSSet_erase( ZnkStrAry sset, const char* str )
{
	const size_t idx = ZnkStrAry_find( sset, 0, str, Znk_NPOS );
	if( idx != Znk_NPOS ){
		ZnkStrAry_erase_byIdx( sset, idx );
		return true;
	}
	return false;
}
