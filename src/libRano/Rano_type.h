#ifndef INCLUDE_GUARD__Rano_type_h__
#define INCLUDE_GUARD__Rano_type_h__

#include <Znk_stdc.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 RanoHtpType_e_None=0
	,RanoHtpType_e_Request
	,RanoHtpType_e_Response
	,RanoHtpType_e_NotHttpStart
} RanoHtpType;

typedef enum {
	 RanoText_Unknown=0
	,RanoText_HTML
	,RanoText_JS
	,RanoText_CSS
	,RanoText_Plain
	,RanoText_Image
	,RanoText_Video
	,RanoText_Binary
} RanoTextType;

Znk_INLINE const char*
RanoTextType_getCStr( RanoTextType type )
{
	const char* str = Znk_TO_STR( RanoText_Unknown );
	switch( type ){
	case Znk_BIND_STR( : str=, RanoText_HTML );   break;
	case Znk_BIND_STR( : str=, RanoText_JS );     break;
	case Znk_BIND_STR( : str=, RanoText_CSS );    break;
	case Znk_BIND_STR( : str=, RanoText_Binary ); break;
	default: break;
	}
	return str + Znk_strlen_literal( "RanoText_" );
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
