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
