#include "Znk_prim.h"
#include "Znk_primp_ary.h"

ZnkPrimp
ZnkPrimp_create( ZnkPrimType prim_type, ZnkElemDeleterFunc elem_deleter )
{
	ZnkPrimp primp = (ZnkPrimp)Znk_alloc0( sizeof( ZnkPrim ) );
	ZnkPrim_compose( primp, prim_type, elem_deleter );
	return primp;
}
ZnkPrimp
ZnkPrimp_createPtr( void* ptr, ZnkElemDeleterFunc elem_deleter )
{
	ZnkPrimp primp = (ZnkPrimp)Znk_alloc0( sizeof( ZnkPrim ) );
	ZnkPrim_composePtr( primp, ptr, elem_deleter );
	return primp;
}
void
ZnkPrimp_destroy( ZnkPrimp primp )
{
	if( primp ){
		ZnkPrim_dispose( primp );
		Znk_free( primp );
	}
}

void
ZnkPrim_dispose( ZnkPrimp prim )
{
	ZnkPrimType type = ZnkPrim_type( prim );
	switch( type ){
	case ZnkPrim_e_Ptr:
	{
		ZnkElemDeleterFunc elem_deleter = ZnkPrim_get_deleter( prim );
		if( elem_deleter ){
			elem_deleter( prim->u_.ptr_ );
		}
		break;
	}
	case ZnkPrim_e_Bfr:
		ZnkBfr_destroy( prim->u_.bfr_ );
		break;
	case ZnkPrim_e_BfrAry:
		ZnkBfrAry_destroy( prim->u_.bda_ );
		break;
	case ZnkPrim_e_Str:
		ZnkStr_destroy( prim->u_.str_ );
		break;
	case ZnkPrim_e_StrAry:
		ZnkStrAry_destroy( prim->u_.sda_ );
		break;
	case ZnkPrim_e_ObjAry:
		ZnkObjAry_destroy( prim->u_.oda_ );
		break;
	case ZnkPrim_e_PrimAry:
		ZnkPrimpAry_destroy( prim->u_.pda_ );
		break;
	default:
		/* none */
		break;
	}
	ZnkPrim_set_null( prim );
}

void
ZnkPrim_compose( ZnkPrimp prim, ZnkPrimType type, ZnkElemDeleterFunc elem_deleter )
{
	const bool elem_responsibility = true;
	ZnkPrim_dispose( prim );

	prim->type__.u64_ = (uint64_t)type;
	switch( type ){
	case ZnkPrim_e_Ptr:
		prim->u_.ptr_ = NULL;
		prim->deleter__.ptr_ = Znk_force_ptr_cast( void*, elem_deleter );
		break;
	case ZnkPrim_e_Bfr:
		prim->u_.bfr_ = ZnkBfr_create_null();
		break;
	case ZnkPrim_e_BfrAry:
		prim->u_.bda_ = ZnkBfrAry_create( elem_responsibility );
		break;
	case ZnkPrim_e_Str:
		prim->u_.str_ = ZnkStr_new( "" );
		break;
	case ZnkPrim_e_StrAry:
		prim->u_.sda_ = ZnkStrAry_create( elem_responsibility );
		break;
	case ZnkPrim_e_ObjAry:
		prim->u_.oda_ = ZnkObjAry_create( elem_deleter );
		break;
	case ZnkPrim_e_PrimAry:
		prim->u_.pda_ = ZnkPrimpAry_create( elem_responsibility );
		break;
	default:
		/* none */
		break;
	}
}

