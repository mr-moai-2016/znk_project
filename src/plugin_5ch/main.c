#include <Moai_plugin_dev.h>
#include <Znk_stdc.h>
#include <Znk_def_util.h>
#include <Znk_cookie.h>
#include <Znk_htp_hdrs.h>
#include <Znk_str_ptn.h>
#include <Znk_str_ex.h>
#include <Znk_mem_find.h>
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

MoaiPluginState
on_post( ZnkMyf ftr_send, ZnkVarpAry hdr_vars, ZnkVarpAry post_vars )
{
	bool referer_with_bbs_key = false;
	ZnkVarp yuki = refCookieVar( ftr_send, "yuki" );
	if( yuki ){
		Znk_printf_e( "on_post yuki=[%s]\n", ZnkVar_cstr(yuki) );
		if( ZnkVar_str_leng(yuki) == 0 ){
			/***
			 * 初回投稿と考えられる.
			 * このとき投稿の確認画面が表示されるが、その部分においてPOST変数MESSAGEの内容は
			 * " 文字が &quot; へサニタイズされないため、これをサニタイズしておく必要がある.
			 * 尚、初回投稿以外の通常の投稿では " は &quot;にサニタイズされるため、この必要はなく、
			 * また同時に &quot; は &amp;quot; にサニタイズされてしまうため、この置き換えをしてはならない.
			 */
			ZnkVarp message = ZnkVarpAry_find_byName( post_vars, "MESSAGE", Znk_strlen_literal("MESSAGE"), false );
			if( message ){
				ZnkStr message_str = ZnkVar_str( message );
				Znk_printf_e( "message before=[%s]\n", ZnkStr_cstr(message_str) );
				ZnkStrEx_replace_BF( message_str, 0,
						"\"", 1,
						"&quot;", Znk_strlen_literal("&quot;"), 
						Znk_NPOS, Znk_NPOS );
				Znk_printf_e( "message after=[%s]\n", ZnkStr_cstr(message_str) );
			}
		}
	}

	/***
	 * Referer修正.
	 *
	 * 現在、5chではPOST時のRefererは以下のようでなければならない.
	 * http://<hostname>/<cgi_script>
	 * 例. http://lavender.5ch.net/test/read.cgi
	 * http://も必須で、これがないと「Refererが変です」エラーとなる.
	 * 
	 * <cgi_script>以降は空でもよいし板の情報まで含まれていてもよい.
	 * たとえば、http://<hostname>/<cgi_script>/<bbs>/<key>/l50
	 * などとなっていてもよい.
	 *
	 * Moaiをローカルプロキシモードで使った場合、Referer値はブラウザの設定した値をそのまま引き継ぐため、
	 * <bbs>/<key>/l50 などまで含む値となっている.
	 * 一方、EasterからのPostの場合、現状ではReferer値は http://<hostname>/<cgi_script>
	 * が設定されるようになっている.
	 *
	 * 現状ではこのままでもどちらでも問題ないが、もし5chの仕様が変わって<bbs>/<key>の部分まで
	 * 必須となった場合は、以下のadd_bbs_keyを有効にすればEasterでもこれに対応できる.
	 */
	if( referer_with_bbs_key ){
		ZnkVarp bbs  = ZnkVarpAry_find_byName_literal( post_vars, "bbs", false );
		ZnkVarp key  = ZnkVarpAry_find_byName_literal( post_vars, "key", false );
		if( bbs && key ){
			ZnkVarp varp = ZnkHtpHdrs_find_literal( hdr_vars, "Referer" );
			if( varp ){
				ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
				/* 既に<bbs>/<key>が付加されているような場合を弾くため、末尾が.cgiであるかをチェックする. */
				if( ZnkStr_isEnd( val, ".cgi" ) ){
					ZnkStr_addf( val, "/%s/%s/l50", ZnkVar_cstr(bbs), ZnkVar_cstr(key) );
				}
			}
		}
	}
	return true;
}

static int
landmarkMoai( ZnkStr str, void* arg )
{
	ZnkStr_add( str, " <font size=\"-1\" color=\"#800080\">via Moai</font>" );
	return 1;
}

MoaiPluginState
on_response( ZnkMyf ftr_send,
		ZnkVarpAry hdr_vars, ZnkStr text, const char* req_urp )
{
	if( text ){
		ZnkSRef old_ptn = { 0 };
		ZnkSRef new_ptn = { 0 };
		size_t  old_occ[ 256 ] = { 0 };

		ZnkSRef_set_literal( &old_ptn, "</div><div class=\"post\" id=" );
		ZnkSRef_set_literal( &new_ptn, "</div>\n<div class=\"post\" id=" );
		ZnkMem_getLOccTable_forBMS( old_occ, (uint8_t*)old_ptn.cstr_, old_ptn.leng_ );
		ZnkStrEx_replace_BMS( text, 0,
				old_ptn.cstr_, old_ptn.leng_, old_occ,
				new_ptn.cstr_, new_ptn.leng_,
				Znk_NPOS, Znk_NPOS );

		ZnkSRef_set_literal( &old_ptn, "</div><form method=\"POST\"" );
		ZnkSRef_set_literal( &new_ptn, "</div>\n<form method=\"POST\"" );
		ZnkMem_getLOccTable_forBMS( old_occ, (uint8_t*)old_ptn.cstr_, old_ptn.leng_ );
		ZnkStrEx_replace_BMS( text, 0,
				old_ptn.cstr_, old_ptn.leng_, old_occ,
				new_ptn.cstr_, new_ptn.leng_,
				Znk_NPOS, Znk_NPOS );

		/* 5ch : via Moai landmarking */
		ZnkStrPtn_invokeInQuote( text,
				"<h1 class=\"title\">", "\n",
				NULL, NULL,
				landmarkMoai, NULL, false );

		/* 5ch smart phone : via Moai landmarking */
		ZnkSRef_set_literal( &old_ptn, "<div id=\"title\" class=\"threadview_response_title\"></div>" );
		ZnkSRef_set_literal( &new_ptn, "<span id=\"title\" class=\"threadview_response_title\"></span> <font size=\"-1\" color=\"#800080\">via Moai</font>" );
		ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 

	}
	return true;
}

