#ifndef INCLUDE_GUARD__Znk_var_h__
#define INCLUDE_GUARD__Znk_var_h__

#include <Znk_prim.h>
#include <assert.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkVar_tag {
	ZnkStr    name_;
	ZnkStr    filename_;
	uintptr_t type_;
	ZnkPrim   prim_;
} ZnkVar;

#define ZnkVar_D( name ) ZnkVar name = { 0,}
Znk_INLINE void ZnkVar_set_null( ZnkVar* var ){
	ZnkVar_D( zero ); *var = zero;
}

typedef ZnkVar* ZnkVarp;

Znk_INLINE void
ZnkVar_compose( ZnkVar* var, const char* name, const char* filename, int var_type, ZnkPrimType prim_type )
{
	var->name_     = ZnkStr_new( name );
	var->filename_ = ZnkStr_new( filename );
	var->type_     = var_type;
	ZnkPrim_compose( &var->prim_, prim_type );
}
Znk_INLINE void
ZnkVar_dispose( ZnkVar* var )
{
	ZnkStr_delete( var->name_ );
	ZnkStr_delete( var->filename_ );
	var->type_ = 0xDEADBEAF;
	ZnkPrim_dispose( &var->prim_ );
}

Znk_INLINE void
ZnkVar_set_val_Bfr( ZnkVar* var, const uint8_t* data, size_t data_size )
{
	ZnkPrim_compose( &var->prim_, ZnkPrim_e_Bfr );
	ZnkBfr_set_dfr( var->prim_.u_.bfr_, data, data_size );
}
Znk_INLINE uint8_t*
ZnkVar_data( ZnkVar* var )
{
	assert( ZnkPrim_type( &var->prim_ ) == ZnkPrim_e_Bfr );
	return ZnkBfr_data( var->prim_.u_.bfr_ );
}
Znk_INLINE size_t
ZnkVar_data_size( ZnkVar* var )
{
	assert( ZnkPrim_type( &var->prim_ ) == ZnkPrim_e_Bfr );
	return ZnkBfr_size( var->prim_.u_.bfr_ );
}

Znk_INLINE void
ZnkVar_set_val_Str( ZnkVar* var, const char* data, size_t data_size )
{
	ZnkPrim_compose( &var->prim_, ZnkPrim_e_Str );
	ZnkStr_assign( var->prim_.u_.str_, 0, data, data_size );
}
Znk_INLINE const char*
ZnkVar_name_cstr( ZnkVar* var )
{
	return ZnkStr_cstr( var->name_ );
}
Znk_INLINE const char*
ZnkVar_cstr( ZnkVar* var )
{
	assert( ZnkPrim_type( &var->prim_ ) == ZnkPrim_e_Str );
	return ZnkStr_cstr( var->prim_.u_.str_ );
}
Znk_INLINE ZnkStr
ZnkVar_str( ZnkVar* var )
{
	assert( ZnkPrim_type( &var->prim_ ) == ZnkPrim_e_Str );
	return var->prim_.u_.str_;
}
Znk_INLINE size_t
ZnkVar_str_leng( ZnkVar* var )
{
	assert( ZnkPrim_type( &var->prim_ ) == ZnkPrim_e_Str );
	return ZnkStr_leng( var->prim_.u_.str_ );
}


ZnkVarp
ZnkVarp_create( const char* name, const char* filename, int var_type, ZnkPrimType prim_type );
void
ZnkVarp_destroy( ZnkVarp varp );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
