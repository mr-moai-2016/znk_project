#ifndef INCLUDE_GUARD__Moai_type_h__
#define INCLUDE_GUARD__Moai_type_h__

#include <Znk_stdc.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 MoaiRASResult_e_OK=0
	,MoaiRASResult_e_Ignored
	,MoaiRASResult_e_CriticalError
	,MoaiRASResult_e_RestartServer
} MoaiRASResult;

#if 0
typedef enum {
	 MoaiHtpType_e_None=0
	,MoaiHtpType_e_Request
	,MoaiHtpType_e_Response
	,MoaiHtpType_e_NotHttpStart
} MoaiHtpType;

typedef enum {
	 MoaiText_Unknown=0
	,MoaiText_HTML
	,MoaiText_JS
	,MoaiText_CSS
	,MoaiText_Binary
} MoaiTextType;

Znk_INLINE const char*
MoaiTextType_getCStr( MoaiTextType type )
{
	const char* str = Znk_TO_STR( MoaiText_Unknown );
	switch( type ){
	case Znk_BIND_STR( : str=, MoaiText_HTML );   break;
	case Znk_BIND_STR( : str=, MoaiText_JS );     break;
	case Znk_BIND_STR( : str=, MoaiText_CSS );    break;
	case Znk_BIND_STR( : str=, MoaiText_Binary ); break;
	default: break;
	}
	return str + Znk_strlen_literal( "MoaiText_" );
}
#endif

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
