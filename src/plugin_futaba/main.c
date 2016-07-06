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
	/***
	 * dst_ptuaが以下のケースの場合、値を変更してはならない.
	 *
	 *   空値
	 *   整数
	 */
	return ZnkVar_str_leng(dst_ptua) == 0 || isInteger(ZnkVar_cstr(dst_ptua));
}

bool on_post( ZnkMyf ftr_send, ZnkVarpAry hdr_vars, ZnkVarpAry post_vars )
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
		ZnkVarp     real_ua      = ZnkHtpHdrs_find( hdr_vars, "User-Agent", Znk_strlen_literal("User-Agent") );
		const char* real_ua_cstr = ZnkHtpHdrs_val_cstr( real_ua, 0 );
		/* まず簡易なケースを弾く */
		if( !isUntouchable_ptua( dst_ptua ) ){
#if 0
			/***
			 * 現在ふたばではptuaの実値をtransientな仕様とすることで偽装をかわす戦略をとっている.
			 * そのため、単純な代入が通用しない.
			 */
			ZnkVar_set_val_Str( dst_ptua, ZnkVar_cstr(USERS_ptua), ZnkVar_str_leng(USERS_ptua) );
#else
			/* とりあえずbrute force algorismで十分であろう */
			ZnkStr dst_ptua_str = ZnkVar_str( dst_ptua );
			ZnkStrEx_replace_BF( dst_ptua_str, 0,
					real_ua_cstr, strlen(real_ua_cstr),
					ZnkVar_cstr(USERS_ptua), ZnkVar_str_leng(USERS_ptua),
					Znk_NPOS );
#endif
		}
	}

	/***
	 * これによりブラウザ上にあるpwdの値は遮断され、
	 * ftr_send上にある値が替わりに採用される.
	 */
	update_pwd( pwd, pwdc, USERS_password );
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


typedef int (*StrProcessFuncT)( ZnkStr str, void* arg );

/***
 * 完成.
 * 次期メジャーバージョンリリース1.2にて
 * libZnkへ追加予定.
 */
static void
ZnkQuoteParser_invoke( ZnkStr text,
		const char* begin_ptn, const char* end_ptn, const char* escape_ptn,
		const StrProcessFuncT event_handler, void* event_arg )
{
	ZnkSRef sref_begin;
	ZnkSRef sref_end;
	ZnkSRef sref_escape;
	size_t  occ_tbl_begin[ 256 ];
	size_t  occ_tbl_end[ 256 ];
	size_t  occ_tbl_escape[ 256 ];
	size_t  curp = 0;
	size_t  begp = 0;
	size_t  endp = 0;
	size_t  escp = 0;
	size_t  text_leng = ZnkStr_leng( text );
	ZnkStr  tmp = ZnkStr_new( "" );

	sref_begin.cstr_  = begin_ptn;
	sref_begin.leng_  = strlen( begin_ptn );
	sref_end.cstr_    = end_ptn;
	sref_end.leng_    = strlen( end_ptn );
	ZnkMem_getLOccTable_forBMS( occ_tbl_begin,  (uint8_t*)sref_begin.cstr_,  sref_begin.leng_ );
	ZnkMem_getLOccTable_forBMS( occ_tbl_end,    (uint8_t*)sref_end.cstr_,    sref_end.leng_ );

	sref_escape.cstr_ = escape_ptn;
	if( sref_escape.cstr_ ){
		sref_escape.leng_ = strlen( escape_ptn );
		ZnkMem_getLOccTable_forBMS( occ_tbl_escape, (uint8_t*)sref_escape.cstr_, sref_escape.leng_ );
	}

	while( true ){
		text_leng = ZnkStr_leng( text );
		if( curp >= text_leng ){
			break;
		}

		/* lexical-scan phase1 */
		begp = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(text)+curp, text_leng-curp,
				(uint8_t*)sref_begin.cstr_, sref_begin.leng_, 1, occ_tbl_begin );
		if( begp == Znk_NPOS ){
			break;
		}
		begp += curp + sref_begin.leng_;

		/* lexical-scan phase2 */
		curp = begp;
		while( true ){
			endp = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(text)+curp, text_leng-curp,
					(uint8_t*)sref_end.cstr_, sref_end.leng_, 1, occ_tbl_end );
			if( sref_escape.cstr_ ){
				escp = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(text)+curp, text_leng-curp,
						(uint8_t*)sref_escape.cstr_, sref_escape.leng_, 1, occ_tbl_escape );
			} else {
				escp = Znk_NPOS;
			}
			if( endp == Znk_NPOS ){
				endp = text_leng;
			} else {
				if( escp != Znk_NPOS && escp + sref_escape.leng_ == endp ){
					/* Escape sequence is recognized. Try again. */
					curp += endp + sref_end.leng_;
					continue;
				}
				endp += curp;
			}
			break;
		}

		ZnkStr_assign( tmp, 0, ZnkStr_cstr( text ) + begp, endp-begp );

		/* invoke event callback. */
		event_handler( tmp, event_arg );

		/* replace by the result */
		ZnkBfr_replace( text, begp, endp-begp, (uint8_t*)ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );

		curp = begp + ZnkStr_leng(tmp) + sref_end.leng_;
	}

	ZnkStr_delete( tmp );
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
			Znk_NPOS );
	return 0;
}
static int
processQuote( ZnkStr str, void* arg )
{
	ZnkQuoteParser_invoke( str, "\"", "\"", "\\",
			replaceStr_BMS, arg );
	ZnkQuoteParser_invoke( str, "\'", "\'", "\\",
			replaceStr_BMS, arg );
	return 0;
}

bool on_response( ZnkMyf ftr_send,
		ZnkVarpAry hdr_vars, ZnkStr text, const char* req_urp )
{
	if( text ){
		const char* old_ptn = "＠ふたば</span>";
		char new_ptn[ 4096 ];
		Znk_snprintf( new_ptn, sizeof(new_ptn), "＠ふたば via Moai[%s]</span>", req_urp );
		replaceStr( text, 0,
				old_ptn, Znk_NPOS,
				new_ptn, Znk_NPOS,
				Znk_NPOS );

		/***
		 * Hi all,
		 *
		 * First, we search miscellaneous range, and replace evil patterns to innocences
		 * if it exists in the range.  We use Boyer-Moore Sunday algorism for performance.
		 * Second, adopt your myf filter to the text normally.
		 *
		 * Regard.
		 */

		/* boiiinize */
		{
			struct PtnInfo ptn_info = { "iframe", "zenkakuboiiin" };
			ZnkQuoteParser_invoke( text, "<!--", "-->", NULL,
					processQuote, &ptn_info );
			ZnkQuoteParser_invoke( text, "<script", "</script", NULL,
					processQuote, &ptn_info );
		}

		/* dayoonize */
		{
			struct PtnInfo ptn_info = { "iframe", "zenkakudayoon" };
			ZnkQuoteParser_invoke( text, "<!--", "-->", NULL,
					replaceStr_BMS, &ptn_info );
			ZnkQuoteParser_invoke( text, "<script", "</script", NULL,
					replaceStr_BMS, &ptn_info );
		}
	
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
		{
			struct PtnInfo ptn_info = { "zenkakuboiiin", "iframe" };
			ZnkQuoteParser_invoke( text, "<script", "</script", "/*",
					replaceStr_BMS, &ptn_info );
		}

	}
	return true;
}

