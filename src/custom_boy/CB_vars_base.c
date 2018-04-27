#include <CB_vars_base.h>

void
CBVarsBase_addNewStr_toVars( ZnkVarpAry vars, const char* var_name, const char* val, size_t val_leng )
{
	ZnkVarp varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
	ZnkStr_assign( varp->name_, 0, var_name, Znk_NPOS );
	ZnkVar_set_val_Str( varp, val, val_leng );
	ZnkVarpAry_push_bk( vars, varp );
}
void
CBVarsBase_registStr_toVars( ZnkVarpAry vars, const char* var_name, const char* val, size_t val_leng )
{
	ZnkVarp vp = ZnkVarpAry_find_byName( vars, var_name, Znk_NPOS, false );
	if( vp ){
		ZnkVar_set_val_Str( vp, val, val_leng );
	} else {
		/* êVãKí«â¡ */
		CBVarsBase_addNewStr_toVars( vars, var_name, val, val_leng );
	}
}
