#include <Moai_plugin_dev.h>
#include <Znk_stdc.h>
#include <Znk_def_util.h>
#include <Znk_cookie.h>
#include <Znk_htp_hdrs.h>
#include <Znk_s_base.h>
#include <Znk_str_ptn.h>
#include <Znk_str_ex.h>
#include <Znk_missing_libc.h>
#include <Znk_mem_find.h>
#include <string.h>
#include <ctype.h>

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
			 * この場合、以後pwdcがふたばから適当な値として自動発行されるはずである.
			 */
			ZnkVar_set_val_Str( pwd, "", 0 );
		}
	}
}

static bool
isInteger( const char* cstr )
{
	const char* p = cstr;
	while( *p ){
		if( !isdigit( *p ) ){
			return false;
		}
		++p;
	}
	return (bool)( p != cstr );
}
static bool
isUntouchable_ptua( ZnkVarp dst_ptua )
{
	/* 2016/10/07: 最新の仕様においてはここはfalseでよい */
	return false;
}

MoaiPluginState
on_post( ZnkMyf ftr_send, ZnkVarpAry hdr_vars, ZnkVarpAry post_vars )
{
	ZnkVarp USERS_futabapt = refPostVar( ftr_send, "USERS_futabapt" );
	ZnkVarp USERS_password = refPostVar( ftr_send, "USERS_password" );
	ZnkVarp USERS_ptua     = refPostVar( ftr_send, "USERS_ptua" );
	ZnkVarp pthb = refPostVar( ftr_send, "pthb" );
	ZnkVarp pthc = refPostVar( ftr_send, "pthc" );
	ZnkVarp pwd  = refPostVar( ftr_send, "pwd" );
	ZnkVarp pwdc = refCookieVar( ftr_send, "pwdc" );
	ZnkVarp dst_ptua = ZnkVarpAry_find_byName( post_vars, "ptua", Znk_strlen_literal("ptua"), false );

	/***
	 * ftr_sendにUSERS_futabaptが定義されていない場合は
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

	if( dst_ptua ){
		/* まず簡易なケースを弾く */
		if( !isUntouchable_ptua( dst_ptua ) ){
			/***
			 * ptuaの実値をtransientな仕様とすることで偽装をかわす可能性がある.
			 */
			ZnkVar_set_val_Str( dst_ptua, ZnkVar_cstr(USERS_ptua), ZnkVar_str_leng(USERS_ptua) );
		}
	}

	/***
	 * これによりブラウザ上にあるpwdの値は遮断され、
	 * ftr_send上にある値が替わりに採用される.
	 */
	update_pwd( pwd, pwdc, USERS_password );
	return true;
}


struct PtnInfo {
	const char* old_ptn_;
	const char* new_ptn_;
};
static int
replaceStr_BMS( ZnkStr str, void* arg )
{
	struct PtnInfo* ptn_info = Znk_force_ptr_cast( struct PtnInfo*, arg );
	const char* old_ptn = ptn_info->old_ptn_;
	const char* new_ptn = ptn_info->new_ptn_;
	ZnkSRef sref_old;
	ZnkSRef sref_new;
	size_t occ_tbl_target[ 256 ];
	sref_old.cstr_   = old_ptn;
	sref_old.leng_   = strlen( old_ptn );
	ZnkMem_getLOccTable_forBMS( occ_tbl_target, (uint8_t*)sref_old.cstr_,   sref_old.leng_ );
	sref_new.cstr_   = new_ptn;
	sref_new.leng_   = strlen( new_ptn );

	ZnkStrEx_replace_BMS( str, 0,
			sref_old.cstr_, sref_old.leng_, occ_tbl_target,
			sref_new.cstr_, sref_new.leng_,
			Znk_NPOS, Znk_NPOS );
	return 0;
}
static int
processQuote( ZnkStr str, void* arg )
{
	ZnkStrPtn_invokeInQuote( str, "\"", "\"", "\\", NULL,
			replaceStr_BMS, arg, false );
	ZnkStrPtn_invokeInQuote( str, "\'", "\'", "\\", NULL,
			replaceStr_BMS, arg, false );
	return 0;
}

static const char*
skipProtocolPrefix( const char* src )
{
	if( Znk_strncmp( src, "http://", Znk_strlen_literal("http://") ) == 0 ){
		src += Znk_strlen_literal("http://");
	} else if( Znk_strncmp( src, "https://", Znk_strlen_literal("https://") ) == 0 ){
		src += Znk_strlen_literal("https://");
	} else if( Znk_strncmp( src, "file://", Znk_strlen_literal("file://") ) == 0 ){
		src += Znk_strlen_literal("file://");
	}
	return src;
}

MoaiPluginState
on_response( ZnkMyf ftr_send,
		ZnkVarpAry hdr_vars, ZnkStr text, const char* url )
{
	/***
	 * TODO:
	 * ここでMoaiインストール後、初回起動時におけるsendフィルタのVirtualUSERSのStep2処理を
	 * 自動的に行う処理を入れるべき.
	 *
	 * 次のようにする.
	 * 1. Moai起動時に通信の伴わないStep1処理は終らせておく.
	 *    単純なGETでは当然ながらPOST変数の参照は発生せず、UserAgentのシャッフルのみでよいため.
	 *
	 * 2. on_response において ftr_sendで js=offの場合は まだStep2処理を未実行とみなし、
	 *    これを実行し、初POSTに備える.
	 */
	ZnkVarp js = refPostVar( ftr_send, "js" );
	if( js && !ZnkS_eq( ZnkVar_cstr( js ), "on" ) ){
	}

	if( text ){
		{
			ZnkSRef old_ptn = { 0 };
			ZnkSRef new_ptn = { 0 };
			size_t  old_occ[ 256 ] = { 0 };

			ZnkSRef_set_literal( &old_ptn, "＠ふたば</span>" );
			ZnkSRef_set_literal( &new_ptn, "＠ふたば <font size=\"-1\" color=\"#800080\">via Moai</font></span>" );
			ZnkMem_getLOccTable_forBMS( old_occ, (uint8_t*)old_ptn.cstr_,   old_ptn.leng_ );
			ZnkStrEx_replace_BMS( text, 0,
					old_ptn.cstr_, old_ptn.leng_, old_occ,
					new_ptn.cstr_, new_ptn.leng_,
					Znk_NPOS, Znk_NPOS );
		}

		/* MoaiをLocalProxyとして使うなら、以下のfilterはとりあえずまだ必要である */
		/* boiiinize */
		{
			struct PtnInfo ptn_info = { "iframe", "zenkakuboiiin" };
			ZnkStrPtn_invokeInQuote( text, "<!--", "-->", NULL, NULL,
					processQuote, &ptn_info, false );
			ZnkStrPtn_invokeInQuote( text, "<script", "</script", NULL, NULL,
					processQuote, &ptn_info, false );
		}

		/* dayoonize */
		ZnkStrPtn_replaceInQuote( text, "<!--", "-->", NULL, NULL,
				"<iframe", "<zenkakudayoon", false, Znk_NPOS );
		ZnkStrPtn_replaceInQuote( text, "<script", "</script", NULL, NULL,
				"<iframe", "<zenkakudayoon", false, Znk_NPOS );

		ZnkStrPtn_replaceInQuote( text, "<!--", "-->", NULL, NULL,
				"</iframe", "</zenkakudayoon", false, Znk_NPOS );
		ZnkStrPtn_replaceInQuote( text, "<script", "</script", NULL, NULL,
				"</iframe", "</zenkakudayoon", false, Znk_NPOS );
	
		/* disable iframe */
		{
			struct PtnInfo ptn_info = { "<iframe", "<!-- iframe" };
			replaceStr_BMS( text, &ptn_info );
		}
		{
			struct PtnInfo ptn_info = { "</iframe", "</iframe --><noscript></noscript" };
			replaceStr_BMS( text, &ptn_info );
		}
	
		/* de-boiiinize */
		ZnkStrPtn_replaceInQuote( text, "<script", "</script", "/*", "*/",
				"zenkakuboiiin", "iframe", false, Znk_NPOS );

	}
	return true;
}

