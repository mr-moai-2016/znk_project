#include <Znk_bfr_ary.h>

static void
deleteElem( void* elem ){
	ZnkBfr_destroy( (ZnkBfr)elem );
}
ZnkBfrAry
ZnkBfrAry_create( bool elem_responsibility )
{
	ZnkElemDeleterFunc deleter = elem_responsibility ? deleteElem : NULL;
	return (ZnkBfrAry)ZnkObjAry_create( deleter );
}
