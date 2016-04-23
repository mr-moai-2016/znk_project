#include <Znk_bfr_dary.h>

static void
deleteElem( void* elem ){
	ZnkBfr_destroy( (ZnkBfr)elem );
}
ZnkBfrDAry
ZnkBfrDAry_create( bool elem_responsibility )
{
	ZnkElemDeleterFunc deleter = elem_responsibility ? deleteElem : NULL;
	return (ZnkBfrDAry)ZnkObjDAry_create( deleter );
}
