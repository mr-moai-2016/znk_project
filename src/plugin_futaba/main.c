#include <Moai_plugin_dev.h>
#include <Znk_stdc.h>
#include <Znk_def_util.h>
#include <Znk_cookie.h>
#include <Znk_htp_hdrs.h>
#include <string.h>

static ZnkVarp
refPostVar( ZnkMyf myf, const char* var_name )
{
	return ZnkMyf_refVar( myf, "post_vars", var_name );
}
static ZnkVarp
refCookieVar( ZnkMyf myf, const char* var_name )
{
	return ZnkMyf_refVar( myf, "cookie_vars", var_name );
}

/***
 * myfのUSERS_passwordは、ユーザからの明示的な削除キー指定をシミュレーションするための
 * ものである. pwdはpwdcの値を元に自動設定される場合があり、ユーザからの明示的な指定か
 * 否かの区別がつかないため、このような変数を別途導入してある.
 */
static void
update_pwd( ZnkVarp pwd, ZnkVarp pwdc, ZnkVarp USERS_password )
{
	if( pwd ){
		if( USERS_password && !ZnkS_empty(ZnkVar_cstr(USERS_password)) ){
			/***
			 * この場合、以後pwdcもUSERS_passwordの値として発行されるはずである.
			 */
			ZnkVar_set_val_Str( pwd, ZnkVar_cstr(USERS_password),
					Znk_MIN(ZnkVar_str_leng(USERS_password),8) );
		} else if( pwdc ){
			/***
			 * pwdcがcookie_vars上において設定されているなら、この値を採用.
			 */
			ZnkVar_set_val_Str( pwd, ZnkVar_cstr(pwdc), ZnkVar_str_leng(pwdc) );
		} else {
			/***
			 * pwdcがcookie_vars上において設定されていないなら、空値とする.
			 * この場合、以後pwdcがふたばから適当な値として自動発行発行されるはずである.
			 */
			ZnkVar_set_val_Str( pwd, "", 0 );
		}
	}
}

bool on_post_before( ZnkMyf myf )
{
	ZnkVarp USERS_futabapt = refPostVar( myf, "USERS_futabapt" );
	ZnkVarp USERS_password = refPostVar( myf, "USERS_password" );
	ZnkVarp pthb = refPostVar( myf, "pthb" );
	ZnkVarp pthc = refPostVar( myf, "pthc" );
	ZnkVarp pwd  = refPostVar( myf, "pwd" );
	ZnkVarp pwdc = refCookieVar( myf, "pwdc" );

	ZnkF_printf_e( "plugin_futaba : on_post_before\n" );
	/***
	 * myfにUSERS_futabaptが定義されていない場合は
	 * localStorageが無効である環境と同挙動となる.
	 */
	if( USERS_futabapt ){
		/***
		 * ptfk関数のエミュレート.
		 * オリジナルでは右辺が空のとき、左辺への代入を行わないようにしてあるが、
		 * ここでは無条件で更新する. 左辺の古い値を残してはならないためである.
		 */
		if( pthb ){
			ZnkVar_set_val_Str( pthb, ZnkVar_cstr(USERS_futabapt), ZnkVar_str_leng(USERS_futabapt) );
		}
		if( pthc ){
			ZnkVar_set_val_Str( USERS_futabapt, ZnkVar_cstr(pthc), ZnkVar_str_leng(pthc) );
		}
	}

	/***
	 * これによりブラウザ上にあるpwdの値は遮断され、
	 * myf上にある値が替わりに採用される.
	 */
	update_pwd( pwd, pwdc, USERS_password );
	return true;
}
bool on_response_hdr( ZnkMyf myf, ZnkVarpAry hdr_vars )
{
	ZnkVarp set_cookie = ZnkHtpHdrs_find_literal( hdr_vars, "Set-Cookie" );

	/***
	 * set_cookieの解析し、myf上におけるcookie_varsを更新する.
	 */
	if( set_cookie ){
		const size_t val_size = ZnkHtpHdrs_val_size( set_cookie );
		size_t       val_idx  = 0;
		ZnkVarpAry   cok_vars = ZnkMyf_find_vars( myf, "cookie_vars" );

		for( val_idx=0; val_idx<val_size; ++val_idx ){
			const char* p = ZnkHtpHdrs_val_cstr( set_cookie, val_idx );
			const char* e = p + strlen(p);
			const char* q = memchr( p, ';', (size_t)(e-p) );
			if( q == NULL ){
				q = e;
			}
			ZnkCookie_regist_byAssignmentStatement( cok_vars, p, (size_t)(q-p) );
		}
		return true;
	}
	return false;
}

