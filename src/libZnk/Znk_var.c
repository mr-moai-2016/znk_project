#include "Znk_var.h"
#include "Znk_stdc.h"

ZnkVarp
ZnkVarp_create( const char* name, const char* misc, int var_type, ZnkPrimType prim_type, ZnkElemDeleterFunc elem_deleter )
{
	ZnkVarp varp = (ZnkVarp)Znk_alloc0( sizeof( struct ZnkVar_tag ) );
	ZnkVar_compose( varp, name, misc, var_type, prim_type, elem_deleter );
	return varp;
}
void
ZnkVarp_destroy( ZnkVarp varp )
{
	if( varp ){
		ZnkVar_dispose( varp );
		Znk_free( varp );
	}
}
