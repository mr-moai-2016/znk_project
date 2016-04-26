/***
 * Virtual USERS : Virtual User-agent Screen-size and Everything Randomize System 
 */
#include <Znk_zlib.h>
#include <Znk_stdc.h>
#include <Znk_missing_libc.h>
#include <Znk_net_base.h>
#include <Znk_auto_ptr.hpp>
#include <Znk_s_base.h>
#include <Znk_str.h>
#include <Znk_str_fio.h>
#include <Znk_str_ptn.h>
#include <Znk_cookie.h>
#include <Znk_myf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>
#include <limits.h>
#include <time.h>


typedef enum {
	 FtbSrvType_e_Unknown=0
	,FtbSrvType_e_may
	,FtbSrvType_e_jun
	,FtbSrvType_e_dec
	,FtbSrvType_e_img
	,FtbSrvType_e_cgi
} SrvType;

static bool st_is_dos_path = false;

static unsigned int
getRandomUInt( void )
{
	return (unsigned int)( rand() * UINT_MAX );
}

static bool
decorateHeaderGET( const char* hostname, const char* request_uri,
		const char* ua, const char* referer, ZnkVarpDAry cookie )
{
	ZnkFile fp = ZnkF_fopen( "http_hdr.dat", "wb" );
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
			const size_t size = ZnkVarpDAry_size( cookie );
			if( size ){
				size_t idx;
				ZnkVarp varp;
				ZnkF_fprintf( fp, "Cookie: " );
				for( idx=0; idx<size; ++idx ){
					varp = ZnkVarpDAry_at( cookie, idx );
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
getCatalog( const char* srv_name, const char* board_id,
		ZnkStr posttime, const char* ua, ZnkVarpDAry cookie, ZnkStr thre_uri,
		const char* cnct_hostname, const char* cnct_port )
{
	bool result = false;
	{
		char hostname[ 256 ];
		char req_uri[ 1024 ];
		char cmd[ 4096 ];
		const char sep = st_is_dos_path ? '\\' : '/';

		Znk_snprintf( hostname, sizeof(hostname), "%s.2chan.net", srv_name );
		if( ZnkS_empty( cnct_port ) ){ cnct_port = "80"; }
		if( ZnkS_empty( cnct_hostname ) ){
			cnct_hostname = hostname;
			Znk_snprintf( req_uri, sizeof(req_uri), "/%s/futaba.php?mode=cat", board_id );
		} else {
			/* by proxy */
			/* ここでのhttp://は必須である(ないとうまくいかないproxyもある) */
			Znk_snprintf( req_uri, sizeof(req_uri), "http://%s/%s/futaba.php?mode=cat", hostname, board_id );
		}
		decorateHeaderGET( hostname, req_uri, ua, "", cookie );
		Znk_snprintf( cmd, sizeof(cmd), ".%chttp_decorator %s %s catalog.htm http_hdr.dat",
				sep, cnct_hostname, cnct_port );
		system( cmd );
	}
	{
		Znk_AUTO_PTR( ZnkFile, fp, ZnkF_fopen( "catalog.htm", "rb" ), ZnkF_fclose );
		if( fp ){
			Znk_AUTO_PTR( ZnkStr, line, ZnkStr_new( "" ), ZnkStr_delete );
			while( true ){
				if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
					/* eof */
					break;
				}
				if( strstr( ZnkStr_cstr(line), "<a href='res/" ) ){
					ZnkF_printf_e( "*** line=[%s]\n", ZnkStr_cstr(line) );
					if( ZnkStrPtn_getBetween( thre_uri, (uint8_t*)ZnkStr_cstr(line), ZnkStr_leng(line), "<a href='", "' " ) ){
						/* lineはもう不要なのでこれを一時バッファとして用いる */
						ZnkStr_sprintf( line, 0, "/%s/%s", board_id, ZnkStr_cstr(thre_uri) );
						ZnkStr_swap( line, thre_uri );
						break;
					}
				}
			}
		} else {
			return false;
		}
	}
	{
		if( ZnkCookie_load( cookie, "cookie.txt" ) ){
			ZnkVarp varp = ZnkVarpDAry_find_byName( cookie, "posttime", Znk_strlen_literal( "posttime" ), false );
			ZnkStr_set( posttime, ZnkVar_cstr(varp) );
			result = true;
		}
	}
	return result;

}

static bool
getCaco( const char* srv_name, ZnkStr caco, const char* ua, ZnkVarpDAry cookie, ZnkStr thre_uri,
		const char* cnct_hostname, const char* cnct_port )
{
	bool result = false;
	{
		char hostname[ 256 ];
		char req_uri[ 1024 ];
		char cmd[ 4096 ];
		char referer[ 4096 ];
		const char sep = st_is_dos_path ? '\\' : '/';

		Znk_snprintf( hostname, sizeof(hostname), "%s.2chan.net", srv_name );
		Znk_snprintf( referer, sizeof(referer), "http://%s%s", hostname, ZnkStr_cstr(thre_uri) );
		if( ZnkS_empty( cnct_port ) ){ cnct_port = "80"; }
		if( ZnkS_empty( cnct_hostname ) ){
			cnct_hostname = hostname;
			Znk_snprintf( req_uri, sizeof(req_uri), "/bin/cachemt7.php" );
		} else {
			/* by proxy */
			/* ここでのhttp://は必須である(ないとうまくいかないproxyもある) */
			Znk_snprintf( req_uri, sizeof(req_uri), "http://%s/bin/cachemt7.php", hostname );
		}
		decorateHeaderGET( hostname, req_uri, ua, referer, cookie );
		Znk_snprintf( cmd, sizeof(cmd), ".%chttp_decorator %s %s caco.txt http_hdr.dat",
				sep, cnct_hostname, cnct_port );

		system( cmd );
	}

	{
		Znk_AUTO_PTR( ZnkFile, fp, ZnkF_fopen( "caco.txt", "rb" ), ZnkF_fclose );
		if( fp ){
			Znk_AUTO_PTR( ZnkStr, line, ZnkStr_new( "" ), ZnkStr_delete );
			ZnkStrFIO_fgets( line, 0, 4096, fp );
			ZnkStrPtn_getBetween( caco, (uint8_t*)ZnkStr_cstr(line), ZnkStr_leng(line), "\"", "\"" );
			result = true;
		}
	}
	return result;

}

static bool
loadLineAryData( ZnkStrDAry line_ary, const char* filename )
{
	Znk_AUTO_PTR( ZnkFile, fp, ZnkF_fopen( filename, "rb" ), ZnkF_fclose );
	if( fp ){
		Znk_AUTO_PTR( ZnkStr, line, ZnkStr_new( "" ), ZnkStr_delete );
		ZnkStrDAry_clear( line_ary );

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
			ZnkStrDAry_push_bk_cstr( line_ary, ZnkStr_cstr(line), ZnkStr_leng(line) );
		}
		return true;
	}
	return false;
}

static uint32_t
murmurhash3_32_gc( const char* key, size_t key_length, uint32_t seed )
{
	static const uint32_t c1 = 0xcc9e2d51;
	static const uint32_t c2 = 0x1b873593;
	const size_t remainder = key_length & 3; /* key_length % 4 */
	const size_t bytes = key_length - remainder;

	uint32_t h1 = seed;
	uint32_t i = 0;
	uint32_t h1b;
	uint32_t k1;
	
	while( i < bytes ){
		/***
		 * ISO/IEC 9899/1990 6.3 によれば、Cにおいて副作用完了点(Sequence point)とは
		 * すべての副作用が完了したことが保証されている点である. これは式の本当の終わり、
		 * 実引数の評価完了後の関数の呼出し時、&&、||、三項演算子?:、カンマ演算子において
		 * 存在する. これら以外での演算子で区切られた範囲では、その範囲内で変数xを
		 * 複数回更新した場合の、その範囲内で変数xを複数回更新した場合のxの評価順序は
		 * 未定義である. またたとえそのような範囲においてxの更新が一回のみであっても、
		 * x更新前の値が、x更新後の値を決定するためだけにアクセスされている場合(x = x+1など)を除き、
		 * xの変更と参照が同時に行われている場合(a[x]=x++など)の動作は未定義となる.
		 * 
		 * 従ってオリジナルのJavascriptにあるような ++i を一つの式内で複数使う記述はCではNGであり
		 * ( | 演算子で区分されてはいるが、これは上記の条件を満たす演算子ではないことに注意 )、
		 * 例えば以下のように後でまとめてインクリメントする必要がある.
		 */
		k1 =
			( (key[ i   ] & 0xff)      ) |
			( (key[ i+1 ] & 0xff) << 8 ) |
			( (key[ i+2 ] & 0xff) << 16) |
			( (key[ i+3 ] & 0xff) << 24);
		i += 4;
		k1  = ((((k1 & 0xffff) * c1) + ((((k1 >> 16) * c1) & 0xffff) << 16))) & 0xffffffff;
		k1  = (k1 << 15) | (k1 >> 17);
		k1  = ((((k1 & 0xffff) * c2) + ((((k1 >> 16) * c2) & 0xffff) << 16))) & 0xffffffff;
		h1 ^= k1;
		h1  = (h1 << 13) | (h1 >> 19);
		h1b = ((((h1 & 0xffff) * 5) + ((((h1 >> 16) * 5) & 0xffff) << 16))) & 0xffffffff;
		h1  = (((h1b & 0xffff) + 0x6b64) + ((((h1b >> 16) + 0xe654) & 0xffff) << 16));
	}

	k1 = 0;
	
	switch( remainder ){
	case 3: k1 ^= (key[ i + 2 ] & 0xff) << 16; /* fall-through */
	case 2: k1 ^= (key[ i + 1 ] & 0xff) << 8;  /* fall-through */
	case 1: k1 ^= (key[ i     ] & 0xff);
		k1  = (((k1 & 0xffff) * c1) + ((((k1 >> 16) * c1) & 0xffff) << 16)) & 0xffffffff;
		k1  = (k1 << 15) | (k1 >> 17);
		k1  = (((k1 & 0xffff) * c2) + ((((k1 >> 16) * c2) & 0xffff) << 16)) & 0xffffffff;
		h1 ^= k1;
		break;
	default:
		break;
	}
	
	h1 ^= key_length;
	
	h1 ^= h1 >> 16;
	h1  = (((h1 & 0xffff) * 0x85ebca6b) + ((((h1 >> 16) * 0x85ebca6b) & 0xffff) << 16)) & 0xffffffff;
	h1 ^= h1 >> 13;
	h1  = ((((h1 & 0xffff) * 0xc2b2ae35) + ((((h1 >> 16) * 0xc2b2ae35) & 0xffff) << 16))) & 0xffffffff;
	h1 ^= h1 >> 16;
	
	return h1 >> 0;
}
static void
setMurmurhash32ToVar( ZnkVarp varp, const char* src_key )
{
	uint32_t val;
	char buf[ 256 ];

	val = murmurhash3_32_gc( src_key, Znk_strlen(src_key), 31 );
	Znk_snprintf( buf, sizeof(buf), "%u", val );
	ZnkVar_set_val_Str( varp, buf, Znk_strlen(buf) );
}

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
static ZnkVarp
refUserAgent( ZnkMyf myf )
{
	return ZnkMyf_refVar( myf, "header_vars", "User-Agent" );
}

static bool
shuffleMyfFilter( ZnkMyf myf, const char* srv_name, const char* board_id,
		const char* cnct_hostname, const char* cnct_port )
{
	bool result = false;

	ZnkStr       caco     = ZnkStr_new( "" );
	ZnkStr       posttime = ZnkStr_new( "" );
	ZnkStr       thre_uri = ZnkStr_new( "" );
	ZnkStrDAry   line_ary = ZnkStrDAry_create( true );

	ZnkVarp      varp = NULL;
	ZnkVarpDAry  cookie = NULL;
	/***
	 * user_agent.txtの読み込みに失敗した場合は以下の値が使われる.
	 */
	const char*  ua   = "Firesexy";
	unsigned int random_uval1;
	unsigned int random_uval2;

	srand((unsigned) time(NULL));

	cookie = ZnkVarpDAry_create( true );

	ZnkF_printf_e( "shuffleMyfFilter : cnct_hostname=[%s] cnct_port=[%s]\n", cnct_hostname, cnct_port );
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
	 * cacoコード取得のためにcachemt7.phpをGETするのであるが、この際にUser-Agentの
	 * 情報も送られる. この値をcachemt7.phpがチェックしている可能性も否定はできない.
	 * よって、それより前にUser-AgentのRandomizeを完了して置く必要があることに
	 * 注意する.
	 */
	varp = refUserAgent( myf );
	if( varp ){
		size_t size;
		size_t idx;
		loadLineAryData( line_ary, "user_agent.txt" );
		size = ZnkStrDAry_size( line_ary );
		if( size ){
			random_uval1 = getRandomUInt();
			idx = random_uval1 % size;
			ua = ZnkStrDAry_at_cstr( line_ary, idx );
		}
		ZnkVar_set_val_Str( varp, ua, Znk_strlen(ua) );
	}

	/***
	 * 現在ではcatalogをGETすると、その応答時のSet-Cookieによって
	 * Cookieにposttime値が設定されるようである.
	 * (あるいは、catalogに一回もアクセスしていないかCookieが無効かによって
	 * posttime値が設定されていない状態で初回のPOSTを行うと、
	 * 「Cookieを有効にしてください」メッセージが返されるが、その応答時の
	 * Set-Cookieによってもこの値が設定される.
	 *
	 * とりあえず、ここではCookie posttime値の取得を目当てに一度catalogにアクセスする.
	 * おそらく多くのユーザが最初にアクセスするのはcatalogであろうからこれは妥当な選択と思われる.
	 * 尚、posttimeの値は全サーバで共通である. 換言すればサーバ毎に値を保持する必要はない.
	 */
	if( !getCatalog( srv_name, board_id, posttime, ua, cookie, thre_uri, cnct_hostname, cnct_port ) ){
		ZnkF_printf_e( "gen_filter : Error : Cannot get catalog.\n" );
		goto FUNC_END;
	}
	
	/***
	 * ここで一度HTTP通信(http_decoratorを用いる)を行い、cachemt7.phpの内容を得る.
	 *
	 * あなたの使っているブラウザでcachemt7.phpに直接アクセスしたとき
	 * もしこの内容が毎回変わらないのであれば、それは２回目以降の通信では
	 * キャッシュを参照しているためである.
	 *
	 * 一方、http_decoratorによるHTTP通信はキャッシングされない.
	 * つまり毎回違った内容となる.
	 */
	if( !getCaco( srv_name, caco, ua, cookie, thre_uri, cnct_hostname, cnct_port ) ){
		ZnkF_printf_e( "gen_filter : Error : Cannot get caco code.\n" );
		goto FUNC_END;
	}

	/***
	 * この変数は現在ではcacoコードの値がそのまま格納されるようである.
	 */
	varp = refPostVar( myf, "pthc" );
	if( varp ){
		ZnkVar_set_val_Str( varp, ZnkStr_cstr(caco), ZnkStr_leng(caco) );
	}

	/***
	 * この変数は現状では、古いcacoコードの値を出来るだけ残すことを狙いとして
	 * いるものと思われる. 実際、Cookieやキャッシュをクリアしたとしても
	 * localStorage(futabapt)の値が消去されていない場合は、古いcaco値がここに
	 * 格納される仕組みとなっている.
	 * おそらく、pthcの値が変わりpthbには以前のcacoコード値がそのまま残っていた場合に
	 * futaba.php側でのユーザ特定アルゴリズムはpthcよりpthbの値を優先するものと思われる.
	 *
	 * 従って、pthbへ我々が設定すべき値は空値か現在のpthcの値ということになるが、
	 * とりあえずこのshuffle処理においては空値を設定しておけばよい.
	 */
	varp = refPostVar( myf, "pthb" );
	if( varp ){
		ZnkVar_set_val_Str( varp, "", 0 );
	}

	/***
	 * この変数は実際にはPOST変数群の中に存在せず、localStorageにこの名前で存在する.
	 * これは初回のPOST時にcacoコード値によって初期化される.
	 * そして２回目のPOSTにおいて参照され、pthb値として使われる.
	 * ここでは、on_post_before の呼び出しによってこの挙動を実現するものとする.
	 *
	 * とりあえずこのshuffle処理においてはlocalStorageの完全消去をシミュレートすればよい.
	 * つまり空値を設定しておけばよい.
	 */
	varp = refPostVar( myf, "USERS_futabapt" );
	if( varp ){
		ZnkVar_set_val_Str( varp, "", 0 );
	}

	/***
	 * 一般にhash値から元文字列を復元するのは不可能である.
	 * 従って、以下のsetMurmurhash32ToVarの第２引数として与える元文字列は、
	 * 我々が自由に決めてもかまわない.
	 *
	 * ここではその元文字列としてとりあえずランダムな整数を使っている.
	 * しかしこれ一つだけではfutaba.phpがこれに対策するシナリオも考えられなくはない.
	 * そこで万全を期すために、ランダムな値を二つ用いる.
	 * このランダムな２値を持って、我々のマシン環境のダミー値とする.
	 */
	random_uval1 = getRandomUInt();
	random_uval2 = getRandomUInt();
	varp = refPostVar( myf, "flrv" );
	if( varp ){
		char src_key[ 4096 ];
		Znk_snprintf( src_key, sizeof(src_key), "%s###%u###%u", ua, random_uval1, random_uval2 );
		setMurmurhash32ToVar( varp, src_key );
	}

	/***
	 * 最近導入されたUser-Agent部を含まないfingerprint値である.
	 * User-Agentの値を変えた場合、flrv値は変わり、flvv値は変わらない.
	 * このことからUser-Agent偽装をある程度検出することが狙いと思われる.
	 *
	 * flrvと同様にランダム化すればよいが、異なる値となるよう注意する.
	 * (この両者が同じ値となる確率はゼロではないにしても天文学的に低く、
	 * つまり事実上明らかに異常であり、このことはfutaba.php側でも瞬時に
	 * 判定可能である)
	 * 以下では仕様に忠実にkey文字列にUser-Agent部のみを含めていない.
	 */
	varp = refPostVar( myf, "flvv" );
	if( varp ){
		char src_key[ 4096 ];
		Znk_snprintf( src_key, sizeof(src_key), "%u###%u", random_uval1, random_uval2 );
		setMurmurhash32ToVar( varp, src_key );
	}

	/***
	 * screen_size.txtに列挙されているサイズからランダムに一つ選び、
	 * さらにdepthをランダムに選択した上でscszを作成する.
	 * screen_size.txtには我々が自由に値を記述することができるが
	 * 実際に有り得る値を記述しておくことをお勧めする.
	 *
	 * デフォルトで付属しているscreen_size.txtを修正することなくそのまま使っても
	 * なんら問題なかろう. 大多数のユーザはここに記載されている解像度のいずれかを
	 * 使用しているはずである(それでももし過不足を感じるなら自由に削除/追加すればよい).
	 */
	varp = refPostVar( myf, "scsz" );
	if( varp ){
		char scsz[ 256 ];
		const char* wh = "640x480";
		size_t size;
		/***
		 * 32bitである場合がおそらく統計的に一番高いであろうから、
		 * 以下では確率が高くなるよう多めに設定してある.
		 */
		static const int depth_tbl[] = { 16, 24, 24, 32, 32, 32, 32 };
		int depth;

		/* get wh */
		loadLineAryData( line_ary, "screen_size.txt" );
		size = ZnkStrDAry_size( line_ary );
		if( size ){
			size_t idx;
			random_uval1 = getRandomUInt();
			idx = random_uval1 % size;
			wh = ZnkStrDAry_at_cstr( line_ary, idx );
		}

		/* get depth */
		random_uval2 = getRandomUInt();
		depth = depth_tbl[ random_uval2 % Znk_NARY(depth_tbl) ]; 

		/* set scsz */
		Znk_snprintf( scsz, sizeof(scsz), "%sx%d", wh, depth );
		ZnkVar_set_val_Str( varp, scsz, Znk_strlen(scsz) );
	}

	varp = refCookieVar( myf, "posttime" );
	if( varp ){
		ZnkVar_set_val_Str( varp, ZnkStr_cstr(posttime), ZnkStr_leng(posttime) );
	}

	result = true;
FUNC_END:
	ZnkStr_delete( caco );
	ZnkStr_delete( posttime );
	ZnkStr_delete( thre_uri );
	ZnkStrDAry_destroy( line_ary );

	return result;
}

static bool
initiallyShuffle( const char* proxy_hostname, const char* proxy_port )
{
	/***
	 * 最初に参照するサーバ名と板ID名を指定.
	 * 例えば may と b の組み合わせなら二次元裏Mayを意味する.
	 */
	const char* srv_name = "may";
	const char* board_id = "b";

	Znk_AUTO_PTR( ZnkMyf, myf, ZnkMyf_create(), ZnkMyf_destroy );

	if( !ZnkMyf_load( myf, "filters/futaba_send.myf" ) ){
		ZnkF_printf_e( "gen_filter : Error : Cannot load futaba_send.myf.\n" );
		return false;
	}
	if( !shuffleMyfFilter( myf, srv_name, board_id, proxy_hostname, proxy_port ) ){
		ZnkF_printf_e( "gen_filter : Error : fail to process filter\n" );
		return false;
	}
	if( !ZnkMyf_save( myf, "filters/futaba_send.myf" ) ){
		ZnkF_printf_e( "gen_filter : Error : Cannot save futaba_send.myf.\n" );
		return false;
	}
	return true;
}

int main(int argc, char **argv)
{
	int result;
	const char* proxy_hostname = "";
	const char* proxy_port = "";

#if defined(Znk_TARGET_WINDOWS)
	st_is_dos_path = true;
#else
	st_is_dos_path = false;
#endif

	if( argc > 2 ){
		proxy_hostname = argv[ 1 ];
		proxy_port     = argv[ 2 ];
	} else if( argc == 2 ){
		proxy_hostname = argv[ 1 ];
	}

	/***
	 * Initially shuffleを実行.
	 */
	ZnkF_printf_e( "\n" );
	ZnkF_printf_e( "USERS : Starting Initially shuffle.\n\n" );
	ZnkF_printf_e( "  proxy_hostname=[%s] proxy_port=[%s]\n", proxy_hostname, proxy_port );
	if( !initiallyShuffle( proxy_hostname, proxy_port ) ){
		return EXIT_FAILURE;
	}

	/***
	 * ローカルプロキシMoaiを起動.
	 */
	ZnkF_printf_e( "\n" );
	ZnkF_printf_e( "USERS : Starting Moai.\n\n" );
	{
		char cmd[ 4096 ];
		const char* app = st_is_dos_path ? ".\\moai" : "./moai";
		if( !ZnkS_empty( proxy_port ) ){
			Znk_snprintf( cmd, sizeof(cmd), "%s %s %s", app, proxy_hostname, proxy_port );
		} else if( !ZnkS_empty( proxy_hostname ) ){
			Znk_snprintf( cmd, sizeof(cmd), "%s %s", app, proxy_hostname );
		} else {
			Znk_snprintf( cmd, sizeof(cmd), "%s", app );
		}
		result = system( cmd );
		if( result == EXIT_FAILURE ){
			ZnkF_printf_e( "USERS : moai exit failure.\n\n" );
			getchar();
		}
	}

	return EXIT_SUCCESS;
}
