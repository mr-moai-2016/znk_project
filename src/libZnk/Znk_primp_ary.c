#include <Znk_primp_ary.h>
#include <Znk_s_base.h>

static void
deleteElem( void* elem ){
	ZnkPrimp_destroy( (ZnkPrimp)elem );
}
ZnkPrimpAry
ZnkPrimpAry_create( bool elem_responsibility )
{
	ZnkElemDeleterFunc deleter = elem_responsibility ? deleteElem : NULL;
	return (ZnkPrimpAry)ZnkObjAry_create( deleter );
}

