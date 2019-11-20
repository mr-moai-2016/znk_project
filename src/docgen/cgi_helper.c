#include "cgi_helper.h"

#include <Znk_varp_ary.h>
#include <Znk_str_path.h>
#include <Znk_missing_libc.h>
#include <Znk_htp_util.h>

#if defined(Znk_TARGET_WINDOWS)
static bool st_is_dos_path = true;
#else
static bool st_is_dos_path = false;
#endif

const char*
CGIHelper_theMoaiDir( ZnkStr ermsg )
{
	static ZnkStr st_moai_dir = NULL;
	if( st_moai_dir == NULL ){
		const char dsp = st_is_dos_path ? '\\' : '/';
		static const size_t depth = 5;
		st_moai_dir = ZnkStr_new( "../" );
		if( !ZnkStrPath_searchParentDir( st_moai_dir, depth, "target.myf", ZnkDirType_e_File, dsp ) ){
			if( ermsg ){
				ZnkStr_add( ermsg, "Error : Searching moai_dir is failure. target.myf does not found." );
			}
			return NULL;
		}
	}
	return ZnkStr_cstr( st_moai_dir );
}

const char*
CGIHelper_loadAuthenticKey( ZnkStr ermsg )
{
	static char st_moai_authentic_key[ 256 ] = "";
	const char* moai_dir = CGIHelper_theMoaiDir( ermsg );
	char        path[ 256 ] = "";
	ZnkFile     fp = NULL;

	Znk_snprintf( path, sizeof(path), "%sauthentic_key.dat", moai_dir );
	fp = Znk_fopen( path, "rb" );
	if( fp ){
		Znk_fgets( st_moai_authentic_key, sizeof(st_moai_authentic_key), fp );
		Znk_fclose( fp );
		return st_moai_authentic_key;
	}
	return NULL;
}

bool
CGIHelper_isAuthenticated( ZnkVarpAry post_vars, const char* authentic_key )
{
	ZnkVarp varp = ZnkVarpAry_findObj_byName_literal( post_vars, "Moai_AuthenticKey", false );
	if( varp ){
		/* post_varsにMoai_AuthenticKeyが与えられており、かつ authentic_keyに等しい場合のみ認証 */
		if( ZnkS_eq( authentic_key, ZnkVar_cstr( varp ) ) ){
			return true;
		}
	}
	return false;
}

bool
CGIHelper_initEsscential( ZnkVarpAry post_vars,
		const char** ans_moai_dir, const char** ans_authentic_key, bool* ans_is_authenticated,
		ZnkStr ermsg )
{
	bool        result = false;
	const char* moai_dir = NULL;
	const char* authentic_key = NULL;
	bool        is_authenticated = false;

	/***
	 * moaiディレクトリの自動検出.
	 */
	moai_dir = CGIHelper_theMoaiDir( ermsg );
	if( moai_dir == NULL ){
		/* error */
		if( ermsg ){
			ZnkStr_addf( ermsg, "Error : moai_dir is not found.\n" );
		}
		goto FUNC_END;
	}
	if( ermsg ){
		ZnkStr_addf( ermsg, "Report : moai_dir is detected : [%s].\n", moai_dir );
	}

	/***
	 * authentic_key のロード.
	 * (このキーの実体は、moaiが起動している場合必ずmoai_dir直下に存在します)
	 */
	authentic_key = CGIHelper_loadAuthenticKey( ermsg );
	if( authentic_key == NULL ){
		/* error */
		if( ermsg ){
			ZnkStr_addf( ermsg, "Error : Moai authentic_key cannot load.\n" );
		}
		goto FUNC_END;
	}

	/***
	 * Moai認証.
	 *
	 * 万一ユーザの意図しない何らかのスクリプトが裏で実行された場合、
	 * CGIの仕様上、正規のUIを介さなくてもリクエストを発行することができてしまいます.
	 * リクエストに含まれるMoai_AuthenticKeyの値を調べることで、このリクエストが確かに
	 * このCGIアプリケーションの正規のUIを介してユーザ自らが発行したものであることを実証します.
	 *
	 * 特にファイルを読み書きしたり、何らかのシステムのリソースへアクセスするなど、
	 * セキュリティをより堅固な状態で実行したい処理に関しては、
	 * この実証に合格した場合のみ実行を許可することを強くお勧めします.
	 */
	is_authenticated = CGIHelper_isAuthenticated( post_vars, authentic_key );

	result = true;
FUNC_END:
	if( ans_moai_dir         ){ *ans_moai_dir = moai_dir; }
	if( ans_authentic_key    ){ *ans_authentic_key = authentic_key; }
	if( ans_is_authenticated ){ *ans_is_authenticated = is_authenticated; }

	return result;
}

bool
CGIHelper_printTemplateHTML( RanoCGIEVar* evar, ZnkBird bird, const char* template_html_file, const char* authentic_key, ZnkStr ermsg )
{
	if( authentic_key ){
		ZnkBird_regist( bird, "Moai_AuthenticKey", authentic_key );
	}

	ZnkHtpURL_negateHtmlTagEffection( ermsg ); /* for XSS */
	RanoCGIUtil_replaceNLtoHtmlBR( ermsg );
	ZnkBird_regist( bird, "ermsg", ZnkStr_cstr(ermsg) );
	return RanoCGIUtil_printTemplateHTML( evar, bird, template_html_file );
}
