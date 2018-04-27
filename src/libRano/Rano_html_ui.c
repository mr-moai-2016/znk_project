#include <Znk_str.h>
#include <Znk_str_ary.h>

void
RanoHtmlUI_makeSelectBox( ZnkStr html,
		const char* varname, const char* current_val, bool is_enable,
		ZnkStrAry val_list, ZnkStrAry name_list )
{
	const char* readonly_str = is_enable ? "" : "disabled=true";
	const size_t size = ZnkStrAry_size( val_list );
	size_t idx;
	bool is_selected = false;
	const char* val  = "";
	const char* name = "";

	ZnkStr_addf2( html, "<select name=\"%s\" %s>\n", varname, readonly_str );

	for( idx=0; idx<size; ++idx ){
		val  = ZnkStrAry_at_cstr( val_list, idx );
		name = name_list ? ZnkStrAry_at_cstr( name_list, idx ) : val;
		is_selected = ZnkS_eq( val, current_val );
		ZnkStr_addf2( html, "<option value=\"%s\" %s>%s</option>\n", val, is_selected ? " selected" : "", name );
	}
	ZnkStr_add( html, "</select>\n" );
}


