#ifndef INCLUDE_GUARD__Znk_prim_h__
#define INCLUDE_GUARD__Znk_prim_h__

#include <Znk_bfr.h>
#include <Znk_str.h>
#include <Znk_bfr_ary.h>
#include <Znk_str_ary.h>
#include <assert.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkPrimpAry );
typedef struct ZnkPrim_tag* ZnkPrimp;

typedef enum {
	 ZnkPrim_e_None = 0
	,ZnkPrim_e_Int
	,ZnkPrim_e_Real
	,ZnkPrim_e_Ptr
	,ZnkPrim_e_Bfr
	,ZnkPrim_e_BfrAry
	,ZnkPrim_e_Str
	,ZnkPrim_e_StrAry
	,ZnkPrim_e_ObjAry
	,ZnkPrim_e_PrimAry
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
	ZnkBfrAry   bda_;
	ZnkStr      str_;
	ZnkStrAry   sda_;
	ZnkObjAry   oda_;
	ZnkPrimpAry pda_; /* prim ary */
} ZnkPrimU;

typedef struct ZnkPrim_tag {
	ZnkAlign64PtrU type__;    /* must not access directly */
	ZnkPrimU       u_;
	ZnkAlign64PtrU deleter__; /* must not access directly */
} ZnkPrim;

#define ZnkPrim_D( name ) ZnkPrim name = { { 0 }, { 0 }, { 0 } }
Znk_INLINE void ZnkPrim_set_null( ZnkPrimp prim ){
	ZnkPrim_D( zero ); *prim = zero;
}

Znk_INLINE ZnkPrimType
ZnkPrim_type( ZnkPrimp prim )
{
	return (ZnkPrimType)prim->type__.u64_;
}
Znk_INLINE int32_t
ZnkPrim_i32( ZnkPrimp prim )
{
	assert( ZnkPrim_type(prim) == ZnkPrim_e_Int );
	return prim->u_.i32_;
}
Znk_INLINE int64_t
ZnkPrim_i64( ZnkPrimp prim )
{
	assert( ZnkPrim_type(prim) == ZnkPrim_e_Int );
	return prim->u_.i64_;
}
Znk_INLINE double
ZnkPrim_real( ZnkPrimp prim )
{
	assert( ZnkPrim_type(prim) == ZnkPrim_e_Real );
	return prim->u_.r64_;
}
Znk_INLINE void*
ZnkPrim_ptr( ZnkPrimp prim )
{
	assert( ZnkPrim_type(prim) == ZnkPrim_e_Ptr );
	return prim->u_.ptr_;
}
Znk_INLINE ZnkStr
ZnkPrim_str( ZnkPrimp prim )
{
	assert( ZnkPrim_type(prim) == ZnkPrim_e_Str );
	return prim->u_.str_;
}
Znk_INLINE const char*
ZnkPrim_cstr( ZnkPrimp prim )
{
	assert( ZnkPrim_type(prim) == ZnkPrim_e_Str );
	return ZnkStr_cstr( prim->u_.str_ );
}
Znk_INLINE ZnkStrAry
ZnkPrim_strAry( ZnkPrimp prim )
{
	assert( ZnkPrim_type(prim) == ZnkPrim_e_StrAry );
	return prim->u_.sda_;
}
Znk_INLINE ZnkBfr
ZnkPrim_bfr( ZnkPrimp prim )
{
	assert( ZnkPrim_type(prim) == ZnkPrim_e_Bfr );
	return prim->u_.bfr_;
}
Znk_INLINE ZnkBfrAry
ZnkPrim_bfrAry( ZnkPrimp prim )
{
	assert( ZnkPrim_type(prim) == ZnkPrim_e_BfrAry );
	return prim->u_.bda_;
}
Znk_INLINE ZnkObjAry
ZnkPrim_objAry( ZnkPrimp prim )
{
	assert( ZnkPrim_type(prim) == ZnkPrim_e_ObjAry );
	return prim->u_.oda_;
}
Znk_INLINE ZnkPrimpAry
ZnkPrim_primAry( ZnkPrimp prim )
{
	assert( ZnkPrim_type(prim) == ZnkPrim_e_PrimAry );
	return prim->u_.pda_;
}

Znk_INLINE void
ZnkPrim_set_ptr( ZnkPrimp prim, void* ptr )
{
	prim->u_.ptr_ = ptr;
}
Znk_INLINE ZnkElemDeleterFunc
ZnkPrim_get_deleter( ZnkPrimp prim )
{
	return (ZnkElemDeleterFunc)prim->deleter__.ptr_;
}

void
ZnkPrim_dispose( ZnkPrimp prim );

/**
 * @param elem_deleter:
 *  ZnkPrim_e_Ptr と ZnkPrim_e_ObjAry の場合のみ意味を持つ.
 *  それ以外は指定しても単に無視される.
 */
void
ZnkPrim_compose( ZnkPrimp prim, ZnkPrimType type, ZnkElemDeleterFunc elem_deleter );

Znk_INLINE void
ZnkPrim_composePtr( ZnkPrimp prim, void* ptr, ZnkElemDeleterFunc elem_deleter )
{
	ZnkPrim_compose( prim, ZnkPrim_e_Ptr, elem_deleter );
	ZnkPrim_set_ptr( prim, ptr );
}

ZnkPrimp
ZnkPrimp_create( ZnkPrimType prim_type, ZnkElemDeleterFunc elem_deleter );

ZnkPrimp
ZnkPrimp_createPtr( void* ptr, ZnkElemDeleterFunc elem_deleter );

void
ZnkPrimp_destroy( ZnkPrimp primp );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
