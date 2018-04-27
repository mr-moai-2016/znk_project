#ifndef INCLUDE_GUARD__Est_parser_h__
#define INCLUDE_GUARD__Est_parser_h__

#include <Znk_varp_ary.h>
#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

typedef int (*EstParserProcessFuncT)( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend );

typedef int (*EstParserPlaneTxtProcessFuncT)( ZnkStr planetxt, void* arg );

bool
EstParser_invokeHtmlTagEx( ZnkStr text,
		const EstParserProcessFuncT tag_event_handler, void* tag_event_arg,
		const EstParserPlaneTxtProcessFuncT planetxt_event_handler, void* planetxt_event_arg,
		ZnkStr msg );

Znk_INLINE const char*
EstHtmlAttr_name( const ZnkVarp attr ) {
	return ZnkVar_name_cstr(attr);
}
Znk_INLINE const char*
EstHtmlAttr_val( const ZnkVarp attr ) {
	return ZnkVar_cstr(attr);
}
Znk_INLINE ZnkStr
EstHtmlAttr_str( const ZnkVarp attr ) {
	return ZnkVar_str(attr);
}
Znk_INLINE char
EstHtmlAttr_quote_char( const ZnkVarp attr ) {
	return ZnkStr_first( ZnkVar_misc(attr) );
}
Znk_INLINE void
EstHtmlAttr_set_quote_char( ZnkVarp attr, char qch ) {
	ZnkStr_clear( ZnkVar_misc(attr) );
	ZnkStr_add_c( ZnkVar_misc(attr), qch );
}
Znk_INLINE void
EstHtmlAttr_val_clear( const ZnkVarp attr ) {
	ZnkStr str = ZnkVar_str( attr );
	ZnkStr_clear( str );
}
Znk_INLINE void
EstHtmlAttr_val_set( const ZnkVarp attr, const char* val ) {
	ZnkStr str = ZnkVar_str( attr );
	ZnkStr_set( str, val );
}
Znk_INLINE bool
EstHtmlAttr_isLogical( const ZnkVarp attr ){
	return (bool)( ZnkVar_prim_type( attr ) == ZnkPrim_e_Int );
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
