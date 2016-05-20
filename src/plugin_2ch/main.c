#include <Moai_plugin_dev.h>
#include <Znk_stdc.h>
#include <Znk_def_util.h>
#include <Znk_cookie.h>
#include <Znk_htp_hdrs.h>
#include <Znk_str_ptn.h>
#include <string.h>

Znk_INLINE ZnkVarp
refPostVar( ZnkMyf myf, const char* var_name )
{
	return ZnkMyf_refVar( myf, "post_vars", var_name );
}
Znk_INLINE ZnkVarp
refCookieVar( ZnkMyf myf, const char* var_name )
{
	return ZnkMyf_refVar( myf, "cookie_vars", var_name );
}

bool on_post( ZnkMyf myf )
{
	ZnkVarp USERS_yuki = refPostVar( myf, "USERS_yuki" );
	ZnkVarp yuki       = refCookieVar( myf, "yuki" );

	/***
	 * myfにUSERS_yukiが定義されていない場合はこの処理は無視
	 */
	if( USERS_yuki ){
		/***
		 * 初投稿においては、yukiがcookie上に定義されていてはいけない.
		 * yukiが定義されていない状態で現在の __cfduid の値をまず一旦サーバ側へ
		 * 渡す必要がある.
		 * Initiationを行った直後は、USERS_yukiの値は空となっている.
		 * そのため、以下の処理で所望の処理を実現できる.
		 */
		if( yuki ){
			ZnkVar_set_val_Str( yuki, ZnkVar_cstr(USERS_yuki), ZnkVar_str_leng(USERS_yuki) );
			ZnkVar_set_val_Str( USERS_yuki, "akari", 5 );
		}
	}
	return true;
}

static size_t
replaceStr( ZnkStr str, size_t begin,
		const char* old_ptn, size_t old_ptn_leng,
		const char* new_ptn, size_t new_ptn_leng,
		size_t seek_depth )
{
	return ZnkStrPtn_replace( str, begin,
			(uint8_t*)old_ptn, old_ptn_leng,
			(uint8_t*)new_ptn, new_ptn_leng,
			seek_depth );
}

bool on_response( ZnkMyf myf,
		ZnkVarpAry hdr_vars, ZnkStr text, const char* req_urp )
{
	if( text ){
		const char* old_ptn;
		const char* new_ptn;

		old_ptn = "</div><div class=\"post\" id=";
		new_ptn = "</div>\n<div class=\"post\" id=";
		replaceStr( text, 0,
				old_ptn, Znk_NPOS,
				new_ptn, Znk_NPOS,
				Znk_NPOS );

		old_ptn = "</div><form method=\"POST\"";
		new_ptn = "</div>\n<form method=\"POST\"";
		replaceStr( text, 0,
				old_ptn, Znk_NPOS,
				new_ptn, Znk_NPOS,
				Znk_NPOS );
	}
	return true;
}

