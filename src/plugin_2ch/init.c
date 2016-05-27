#include <Moai_plugin_dev.h>
#include <Znk_stdc.h>
#include <Znk_def_util.h>
#include <Znk_zlib.h>
#include <Znk_missing_libc.h>
#include <Znk_net_base.h>
#include <Znk_s_base.h>
#include <Znk_str.h>
#include <Znk_str_fio.h>
#include <Znk_str_ptn.h>
#include <Znk_cookie.h>
#include <Znk_myf.h>
#include <Znk_dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>
#include <limits.h>
#include <time.h>

static bool st_is_dos_path = false;

static unsigned int
getRandomUInt( void )
{
	return (unsigned int)( rand() * UINT_MAX );
}

static bool
decorateHeaderGET( const char* hostname, const char* request_uri,
		const char* ua, const char* referer, ZnkVarpAry cookie )
{
	const char* tmpdir = st_is_dos_path ? ".\\tmp\\" : "./tmp/";
	ZnkFile fp = NULL;
	char http_hdr_file[ 256 ] = "";
	Znk_snprintf( http_hdr_file, sizeof(http_hdr_file), "%shttp_hdr.dat", tmpdir );
	fp = ZnkF_fopen( http_hdr_file, "wb" );
	if( fp ){
		ZnkF_fprintf( fp, "GET %s HTTP/1.1\r\n", request_uri );
		ZnkF_fprintf( fp, "Host: %s\r\n", hostname );
		ZnkF_fprintf( fp, "User-Agent: %s\r\n", ua );
		//ZnkF_fprintf( fp, "Accept: */*\r\n" );
		ZnkF_fprintf( fp, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n" );
		ZnkF_fprintf( fp, "Accept-Language: ja,en-US;q=0.7,en;q=0.3\r\n" );
		ZnkF_fprintf( fp, "Accept-Encoding: gzip, deflate\r\n" );

		/* Referer */
		if( !ZnkS_empty(referer) ){
			ZnkF_fprintf( fp, "Referer: %s\r\n", referer );
		}
		/* Cookie */
		{
			const size_t size = ZnkVarpAry_size( cookie );
			if( size ){
				size_t idx;
				ZnkVarp varp;
				ZnkF_fprintf( fp, "Cookie: " );
				for( idx=0; idx<size; ++idx ){
					varp = ZnkVarpAry_at( cookie, idx );
					ZnkF_fprintf( fp, "%s=%s", ZnkStr_cstr(varp->name_), ZnkVar_cstr(varp) );
					if( idx < size-1 ){
						ZnkF_fprintf( fp, "; " );
					}
				}
				ZnkF_fprintf( fp, "\r\n" );
			}
		}

		ZnkF_fprintf( fp, "Connection: keep-alive\r\n" );
		ZnkF_fprintf( fp, "\r\n" );
		ZnkF_fclose( fp );
		return true;
	}
	return false;
}

static bool
getCfduid( const char* srv_name,
		ZnkStr cfduid, const char* ua, ZnkVarpAry cookie,
		const char* parent_proxy )
{
	bool result = false;
	char result_file[ 256 ] = "";
	char cookie_file[ 256 ] = "";
	const char* exedir = st_is_dos_path ? ".\\"      : "./";
	const char* tmpdir = st_is_dos_path ? ".\\tmp\\" : "./tmp/";
	{
		char hostname[ 256 ];
		char req_uri[ 1024 ];
		char cmd[ 4096 ];
		char parent_cnct[ 4096 ] = "";
		int ret = EXIT_FAILURE;

		Znk_snprintf( hostname, sizeof(hostname), "%s.2ch.net", srv_name );
		if( ZnkS_empty( parent_proxy ) || ZnkS_eq( parent_proxy, "NONE" ) ){
			Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", hostname );
			Znk_snprintf( req_uri, sizeof(req_uri), "/" );
		} else {
			/* by proxy */
			Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", parent_proxy );
			/* ここでのhttp://は必須である(ないとうまくいかないproxyもある) */
			Znk_snprintf( req_uri, sizeof(req_uri), "http://%s/", hostname );
		}
		decorateHeaderGET( hostname, req_uri, ua, "", cookie );
		Znk_snprintf( result_file, sizeof(result_file), "%s2ch_cfduid.htm", tmpdir );
		Znk_snprintf( cmd, sizeof(cmd), "%shttp_decorator %s %s %scookie_2ch.txt %shttp_hdr.dat",
				exedir, parent_cnct, result_file, tmpdir, tmpdir );
		ret = system( cmd );
		if( ret == EXIT_FAILURE ){
			return false;
		}
	}
	{
		Znk_snprintf( cookie_file, sizeof(cookie_file), "%scookie_2ch.txt", tmpdir );
		if( ZnkCookie_load( cookie, cookie_file ) ){
			ZnkVarp varp = ZnkVarpAry_find_byName( cookie, "__cfduid", Znk_strlen_literal( "__cfduid" ), false );
			ZnkStr_set( cfduid, ZnkVar_cstr(varp) );
			result = true;
		}
	}
	return result;

}

static bool
loadLineAryData( ZnkStrAry line_ary, const char* filename )
{
	ZnkFile fp = ZnkF_fopen( filename, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		ZnkStrAry_clear( line_ary );

		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				break; /* eof : 読み込み完了 */
			}

			ZnkStr_chompNL( line ); /* 改行を除去 */
			if( ZnkStr_first( line ) == '#' ){
				continue; /* 行頭が#で始まる場合はskip */
			}
			/* 前方にあるwhite-spaceを除去 */
			while( ZnkStr_first( line ) == ' ' || ZnkStr_first( line ) == '\t' ){
				ZnkStr_cut_front_c( line );
			}
			if( ZnkStr_empty( line ) ){
				continue; /* 空行をskip */
			}

			/* この行を一つの値として文字列配列へと追加 */
			ZnkStrAry_push_bk_cstr( line_ary, ZnkStr_cstr(line), ZnkStr_leng(line) );
		}
		ZnkStr_delete( line );
		ZnkF_fclose( fp );
		return true;
	}
	return false;
}
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
Znk_INLINE ZnkVarp
refUserAgent( ZnkMyf myf )
{
	return ZnkMyf_refVar( myf, "header_vars", "User-Agent" );
}

static bool
shuffleMyfFilter( ZnkMyf myf, const char* srv_name,
		const char* parent_proxy, ZnkStr result_msg )
{
	bool result = false;

	ZnkStrAry  line_ary = ZnkStrAry_create( true );
	ZnkVarpAry cookie   = NULL;
	ZnkVarp    varp     = NULL;
	ZnkStr     cfduid   = ZnkStr_new( "" );
	/***
	 * user_agent.txtの読み込みに失敗した場合は以下の値が使われる.
	 */
	const char*  ua = "2ch";
	unsigned int random_uval1;
	//unsigned int random_uval2;

	srand((unsigned) time(NULL));

	cookie = ZnkVarpAry_create( true );

	ZnkF_printf_e( "shuffleMyfFilter : parent_proxy=[%s]", parent_proxy );
	/***
	 * user_agent.txtに列挙されているUser-Agentからランダムに一つ選び、
	 * それをHttpヘッダにセットする.
	 * user_agent.txtには我々が自由に値を記述することができるが
	 * 実際に有り得る値を記述しておくことをお勧めする.
	 *
	 * デフォルトで付属しているuser_agent.txtは、ネット上に転がっている値を拾い集め
	 * 私が適当にでっち上げたものである.
	 * これを修正することなくそのまま使ってもよいが、新しいブラウザが登場するたびに
	 * 段々とその情報は古くなるし、できればあなた独自のプライベートな user_agent.txtを
	 * 生成しておくほうがなにかと確実ではある.
	 * 手動で好みのリストを作成していってもよいし、 巷にあるUser-Agentジェネレータを
	 * 使うのも一つの手である.
	 * (一応、USERSの次期バージョンではそのようなジェネレータの追加も予定してはいる).
	 *
	 * cfduidコード取得のためにtopページをGETするのであるが、この際にUser-Agentの
	 * 情報も送られる. この値を2chがチェックしている可能性も否定はできない.
	 * よって、それより前にUser-AgentのRandomizeを完了して置く必要があることに
	 * 注意する.
	 */
	varp = refUserAgent( myf );
	if( varp ){
		size_t size;
		size_t idx;
		loadLineAryData( line_ary, "user_agent.txt" );
		size = ZnkStrAry_size( line_ary );
		if( size ){
			random_uval1 = getRandomUInt();
			idx = random_uval1 % size;
			ua = ZnkStrAry_at_cstr( line_ary, idx );
		}
		ZnkVar_set_val_Str( varp, ua, Znk_strlen(ua) );
	}

	/***
	 * 現在では2ch内のページをGETすると、その応答時のSet-Cookieによって
	 * Cookieに__cfduid値が設定されるようである.
	 *
	 * とりあえず、ここではCookie __cfduid値の取得を目当てに一度topにアクセスする.
	 * 尚、__cfduidの値は全サーバで共通である. 換言すればサーバ毎に値を保持する必要はない.
	 */
	if( !getCfduid( srv_name, cfduid, ua, cookie, parent_proxy ) ){
		ZnkStr_set( result_msg, "Cannot get __cfduid" );
		goto FUNC_END;
	}

	varp = refCookieVar( myf, "__cfduid" );
	if( varp ){
		ZnkVar_set_val_Str( varp, ZnkStr_cstr(cfduid), ZnkStr_leng(cfduid) );
	}

#if 0
	/***
	 * この内部変数は初投稿の際にCookieにおけるyukiの値を隠蔽するために使われる.
	 * ここでは空に設定しておく必要がある.
	 */
	varp = refPostVar( myf, "USERS_yuki" );
	if( varp ){
		ZnkVar_set_val_Str( varp, "", 0 );
	}
#else
	varp = refCookieVar( myf, "yuki" );
	if( varp ){
		ZnkVar_set_val_Str( varp, "", 0 );
	}
#endif

	/***
	 * PON, HAPの内容を一応消去.
	 * しかしこのロジックにはまだ問題と改良の余地がある.
	 */
	varp = refCookieVar( myf, "PON" );
	if( varp ){
		ZnkVar_set_val_Str( varp, "", 0 );
	}
	varp = refCookieVar( myf, "HAP" );
	if( varp ){
		ZnkVar_set_val_Str( varp, "", 0 );
	}

	result = true;
FUNC_END:
	ZnkStr_delete( cfduid );
	ZnkStrAry_destroy( line_ary );

	return result;
}

bool
initiate( ZnkMyf ftr_send, const char* parent_proxy, ZnkStr result_msg )
{
	/***
	 * 最初に参照するサーバ名を指定.
	 */
	const char* srv_name = "hanabi";

#if defined(Znk_TARGET_WINDOWS)
	st_is_dos_path = true;
#else
	st_is_dos_path = false;
#endif

	ZnkDir_mkdirPath( "./tmp", Znk_NPOS, '/' );
	if( !shuffleMyfFilter( ftr_send, srv_name, parent_proxy, result_msg ) ){
		return false;
	}
	if( !ZnkMyf_save( ftr_send, "filters/2ch_send.myf" ) ){
		ZnkStr_set( result_msg, "Cannot save 2ch_send.myf" );
		return false;
	}
	ZnkStr_set( result_msg, "Virtual USERS done. Your 2ch_send.myf is initiated successfully." );
	{
		ZnkVarp varp;
		varp = refUserAgent( ftr_send );
		if( varp ){
			ZnkStr_addf( result_msg, "\nUser-Agent=[%s]", ZnkVar_cstr(varp) );
		}
		varp = refCookieVar( ftr_send, "__cfduid" );
		if( varp ){
			ZnkStr_addf( result_msg, "\n__cfduid=[%s]", ZnkVar_cstr(varp) );
		}
	}
	return true;
}

