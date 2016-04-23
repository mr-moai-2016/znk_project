#include "Znk_var.h"
#include "Znk_stdc.h"

ZnkVarp
ZnkVarp_create( const char* name, const char* filename, int var_type, ZnkPrimType prim_type )
{
	ZnkVarp varp = (ZnkVarp)Znk_alloc0( sizeof( struct ZnkVar_tag ) );
	ZnkVar_compose( varp, name, filename, var_type, prim_type );
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
