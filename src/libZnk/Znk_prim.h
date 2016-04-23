#ifndef INCLUDE_GUARD__Znk_prim_h__
#define INCLUDE_GUARD__Znk_prim_h__

#include <Znk_bfr.h>
#include <Znk_str.h>
#include <Znk_bfr_dary.h>
#include <Znk_str_dary.h>

Znk_EXTERN_C_BEGIN

typedef void* ZnkPrimDAry; /* dummy */

typedef enum {
	 ZnkPrim_e_None = 0
	,ZnkPrim_e_Int
	,ZnkPrim_e_Real
	,ZnkPrim_e_Ptr
	,ZnkPrim_e_Bfr
	,ZnkPrim_e_BfrDAry
	,ZnkPrim_e_Str
	,ZnkPrim_e_StrDAry
	,ZnkPrim_e_PrimDAry
} ZnkPrimType;

/***
 * 異コンパイラ間でのポータブルなDLLを作成時、
 * 構造体のAlignmentを調整するために使用する.
 * sizeof( void* ) <= 8 のとき、この共用体のsizeofの値は 8
 * sizeof( void* ) >  8 のとき、この共用体のsizeofの値は sizeof( void* ) となる.
 */
typedef union ZnkAlign64PtrU_tag {
	uint64_t u64_;
	void*    ptr_;
} ZnkAlign64PtrU;

/***
 * 厳密なCの仕様に合致しないが、sizeof( double ) == 8 と仮定する.
 * (これが成立しない奇特な環境は想定しない)
 * これにより、この共用体のsizeofの値は sizeof( ZnkAlign64PtrU ) と同じとみなせる.
 */
typedef union ZnkPrimU_tag {
	int32_t     i32_;
	int64_t     i64_;
	double      r64_;
	void*       ptr_; /* general object */
	ZnkBfr      bfr_;
	ZnkBfrDAry  bda_;
	ZnkStr      str_;
	ZnkStrDAry  sda_;
	ZnkPrimDAry pda_; /* reserve */
} ZnkPrimU;

typedef struct ZnkPrim_tag {
	ZnkAlign64PtrU type__;    /* must not access directly */
	ZnkPrimU       u_;
	ZnkAlign64PtrU deleter__; /* must not access directly */
} ZnkPrim;

#define ZnkPrim_D( name ) ZnkPrim name = { { 0 }, { 0 }, { 0 } }
Znk_INLINE void ZnkPrim_set_null( ZnkPrim* prim ){
	ZnkPrim_D( zero ); *prim = zero;
}

Znk_INLINE ZnkPrimType
ZnkPrim_type( ZnkPrim* prim )
{
	return (ZnkPrimType)prim->type__.u64_;
}
Znk_INLINE void
ZnkPrim_set_deleter( ZnkPrim* prim, const ZnkElemDeleterFunc deleter )
{
	prim->deleter__.ptr_ = Znk_force_ptr_cast( void*, deleter );
}
Znk_INLINE ZnkElemDeleterFunc
ZnkPrim_get_deleter( ZnkPrim* prim )
{
	return (ZnkElemDeleterFunc)prim->deleter__.ptr_;
}

Znk_INLINE void
ZnkPrim_dispose( ZnkPrim* prim );

Znk_INLINE void
ZnkPrim_compose( ZnkPrim* prim, ZnkPrimType type )
{
	const bool elem_responsibility = true;
	ZnkPrim_dispose( prim );

	prim->type__.u64_ = (uint64_t)type;
	switch( type ){
	case ZnkPrim_e_Bfr:
		prim->u_.bfr_ = ZnkBfr_create_null();
		break;
	case ZnkPrim_e_BfrDAry:
		prim->u_.bda_ = ZnkBfrDAry_create( elem_responsibility );
		break;
	case ZnkPrim_e_Str:
		prim->u_.str_ = ZnkStr_new( "" );
		break;
	case ZnkPrim_e_StrDAry:
		prim->u_.sda_ = ZnkStrDAry_create( elem_responsibility );
		break;
	case ZnkPrim_e_PrimDAry:
		//ZnkPrimDAry_create();
		break;
	default:
		/* none */
		break;
	}
}
Znk_INLINE void
ZnkPrim_dispose( ZnkPrim* prim )
{
	ZnkPrimType type = ZnkPrim_type( prim );
	switch( type ){
	case ZnkPrim_e_Ptr:
	{
		ZnkElemDeleterFunc deleter = ZnkPrim_get_deleter( prim );
		if( deleter ){
			deleter( prim->u_.ptr_ );
		}
		break;
	}
	case ZnkPrim_e_Bfr:
		ZnkBfr_destroy( prim->u_.bfr_ );
		break;
	case ZnkPrim_e_BfrDAry:
		ZnkBfrDAry_destroy( prim->u_.bda_ );
		break;
	case ZnkPrim_e_Str:
		ZnkStr_destroy( prim->u_.str_ );
		break;
	case ZnkPrim_e_StrDAry:
		ZnkStrDAry_destroy( prim->u_.sda_ );
		break;
	case ZnkPrim_e_PrimDAry:
		//ZnkPrimDAry_destroy( prim->u_.pda_ );
		break;
	default:
		/* none */
		break;
	}
	ZnkPrim_set_null( prim );
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
