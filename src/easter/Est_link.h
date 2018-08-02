#ifndef INCLUDE_GUARD__Est_link_h__
#define INCLUDE_GUARD__Est_link_h__

#include <Znk_varp_ary.h>
#include <Znk_str.h>
#include <Znk_myf.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 EstLinkXhr_e_ImplicitDMZ=0
	,EstLinkXhr_e_ImplicitAuth
	,EstLinkXhr_e_ExplicitDMZ
	,EstLinkXhr_e_ExplicitAuth
} EstLinkXhr;

typedef enum {
	 EstRequest_e_get=0
	,EstRequest_e_post
	,EstRequest_e_post_ex
	,EstRequest_e_head
	,EstRequest_e_reload
	,EstRequest_e_config
	,EstRequest_e_view
} EstRequest;

Znk_INLINE const char*
EstRequest_getCStr( EstRequest est_req )
{
	const char* str = Znk_TO_STR( EstRequest_e_get );
	switch( est_req ){
	case Znk_BIND_STR( : str=, EstRequest_e_get );     break;
	case Znk_BIND_STR( : str=, EstRequest_e_post );    break;
	case Znk_BIND_STR( : str=, EstRequest_e_post_ex ); break;
	case Znk_BIND_STR( : str=, EstRequest_e_head );    break;
	case Znk_BIND_STR( : str=, EstRequest_e_reload );  break;
	case Znk_BIND_STR( : str=, EstRequest_e_config );  break;
	case Znk_BIND_STR( : str=, EstRequest_e_view );    break;
	default: break;
	}
	return str + Znk_strlen_literal( "EstRequest_e_" );
}

struct EstLinkInfo {
	ZnkStr     base_href_;
	ZnkStr     hostname_;
	ZnkStr     hyperpost_url_;
	EstRequest est_req_;
	ZnkMyf     mtgt_;
	bool       img_link_direct_;
	bool       css_link_done_;
	bool       is_https_parent_;
};

typedef int (*EstLinkProcessFuncT)( ZnkVarpAry str, void* arg );

void
EstLink_invokeHtmlTag( ZnkStr str, const char* tag_beg,
		const EstLinkProcessFuncT event_handler, void* event_arg, bool check_with_upper_case );

const char*
EstLink_findTargetName( const ZnkMyf mtgt, const char* line );

int
EstLink_filterImg( ZnkStr str, struct EstLinkInfo* link_info );
int
EstLink_filterScript( ZnkStr str, struct EstLinkInfo* link_info );
int
EstLink_filterLink( ZnkStr str, struct EstLinkInfo* link_info, EstLinkXhr link_xhr );
//int
//EstLink_filterForm( ZnkStr str, struct EstLinkInfo* link_info );
bool
EstLink_filterOnclickJSCall( ZnkVarpAry varp_ary, ZnkVarpAry onclick_jscall );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
