/***
 * MoaiCGI Engine (introduced by Ver2.0)
 */
#include <Moai_cgi.h>
#include <Moai_io_base.h>
#include <Moai_server_info.h>
#include <Moai_connection.h>

#include <Rano_log.h>

#include <Znk_envvar.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <Znk_str.h>
#include <Znk_str_ex.h>
#include <Znk_htp_hdrs.h>
#include <Znk_mem_find.h>
#include <Znk_net_ip.h>
#include <Znk_dir.h>
#include <Znk_thread.h>
#include <time.h>

static size_t st_cgi_count = 0;
static size_t st_cgi_count_max = 16;

static void
formatTimeString( char* buf, size_t buf_size, time_t clck )
{
	struct 	tm *timePtr;
	if( clck == 0 ) clck = time(NULL);
	timePtr = gmtime( &clck );
	//strftime( buf, buf_size, "%a, %d %b %Y %T GMT", timePtr );
	strftime( buf, buf_size, "%a, %d %b %Y %H:%M:%S GMT", timePtr );
}
static int
sendHtpHdrOfCGI_withContentLength( ZnkStr hdr_str, ZnkSocket sock, size_t content_length )
{
	int ret = 0;
	char timeBuf[ 128 ];
	ZnkStr ans = ZnkStr_new( "" );

	formatTimeString( timeBuf, sizeof(timeBuf), 0 );

	if( ZnkStr_empty( hdr_str ) ){
		/* HTTPヘッダの指定がなされていないCGIスクリプトへの特別措置 */
		ZnkStr_addf( ans,
				"HTTP/1.1 200 OK\r\n"
				"Date: %s\r\n"
				"Content-Type: text/html\r\n",
				timeBuf );
		ZnkStr_addf( ans,
				"Content-Length: %d\r\n",
				content_length );
		ZnkStr_add( ans, "\r\n" );
	} else {
		/* Generally type. */
		ZnkStr_addf( ans,
				"HTTP/1.1 200 OK\r\n"
				"Date: %s\r\n",
				timeBuf );
		ZnkStr_addf( ans,
				"Content-Length: %d\r\n",
				content_length );
		ZnkStr_add( ans,
				ZnkStr_cstr( hdr_str ) );
	}

	ret = ZnkSocket_send_cstr( sock, ZnkStr_cstr(ans) );
	ZnkStr_delete( ans );
	return ret;
}
static int
sendHtpHdrOfCGI_forChunked( ZnkStr hdr_str, ZnkSocket sock )
{
	int ret = 0;
	char timeBuf[ 128 ];
	ZnkStr ans = ZnkStr_new( "" );

	formatTimeString( timeBuf, sizeof(timeBuf), 0 );

	/***
	 * 注意: Transfer-Encoding chunked は HTTP 1.1 からサポートされる機能である.
	 * 従って以下のHTTP/1.1 を替わりにHTTP/1.0と書くことはできない.
	 * (もしHTTP/1.0と書いた場合、ブラウザは正しくchunkedデータを処理しない)
	 */
	if( ZnkStr_empty( hdr_str ) ){
		/* HTTPヘッダの指定がなされていないCGIスクリプトへの特別措置 */
		ZnkStr_addf( ans,
				"HTTP/1.1 200 OK\r\n"
				"Date: %s\r\n"
				"Server: Moai/2.0\r\n"
    			"Keep-Alive: timeout=5, max=100\r\n"
				"Connection: Keep-Alive\r\n"
				"Content-Type: text/html\r\n",
				timeBuf );
		ZnkStr_add( ans,
				"Transfer-Encoding: chunked\r\n" );
		ZnkStr_add( ans, "\r\n" );
	} else {
		/* Generally type. */
		ZnkStr_addf( ans,
				"HTTP/1.1 200 OK\r\n"
				"Date: %s\r\n"
				"Server: Moai/2.0\r\n"
    			"Keep-Alive: timeout=5, max=100\r\n"
				"Connection: Keep-Alive\r\n",
				timeBuf );
		ZnkStr_add( ans,
				"Transfer-Encoding: chunked\r\n" );
		ZnkStr_add( ans,
				ZnkStr_cstr( hdr_str ) );
	}

	ret = ZnkSocket_send_cstr( sock, ZnkStr_cstr(ans) );
	ZnkStr_delete( ans );
	return ret;
}

static int
sendResponseOfCGI( ZnkBfr bfr, ZnkSocket sock )
{
	static bool   st_initialized = false;
	static size_t st_occ_tbl[256] = {0};
	int ret = 0;
	if( ZnkBfr_size(bfr) ){
		char timeBuf[ 128 ];
		size_t pos;
		ZnkStr ans = ZnkStr_new( "" );
	
		if( !st_initialized ){
			ZnkMem_getLOccTable_forBMS( st_occ_tbl, (uint8_t*)"\r\n\r\n", 4 );
			st_initialized = true;
		}
	
		formatTimeString( timeBuf, sizeof(timeBuf), 0 );
	
		pos = ZnkMem_lfind_data_BMS( ZnkBfr_data(bfr), ZnkBfr_size(bfr)-1,
				(uint8_t*)"\r\n\r\n", 4, 1, st_occ_tbl );
		if( pos == Znk_NPOS ){
			/***
			 * Content-Type is not specified.
			 * We take this as text/html.
			 */
			ZnkStr_addf( ans,
					"HTTP/1.1 200 OK\r\n"
					"Date: %s\r\n"
					"Content-Type: text/html\r\n",
					timeBuf );
			ZnkStr_addf( ans,
					"Content-Length: %d\r\n",
					ZnkBfr_size(bfr)-1 );
			ZnkStr_add( ans, "\r\n" );
			ZnkStr_append( ans, (char*)ZnkBfr_data(bfr), ZnkBfr_size(bfr)-1 );

			ret = ZnkSocket_send_cstr( sock, ZnkStr_cstr(ans) );
		} else {
			/***
			 * Generally type.
			 */
			ZnkStr_addf( ans,
					"HTTP/1.1 200 OK\r\n"
					"Date: %s\r\n",
					timeBuf );
			ZnkStr_append( ans, (char*)ZnkBfr_data(bfr), pos+2 );
			ZnkStr_addf( ans,
					"Content-Length: %d\r\n",
					ZnkBfr_size(bfr)-1-pos-4 );
			ZnkStr_add( ans, "\r\n" );
			ZnkStr_append( ans, (char*)ZnkBfr_data(bfr)+pos+4, ZnkBfr_size(bfr)-1-pos-4 );

			ret = ZnkSocket_send( sock, (uint8_t*)ZnkStr_cstr(ans), ZnkStr_leng(ans) );
		}
		ZnkStr_delete( ans );
	}
	return ret;
}

/***
 * \r\r\n => \r\n 変換をしなければならない.
 * しかし全文を無条件に置換する方法ではダメである.
 * (画像などのバイナリにたまたまそのようなパターンが含まれている場合
 * 元データが破壊されてしまう)
 * そこでここではまず厳密にHTTPヘッダを解釈し、ヘッダにおける改行と
 * chunkによる改行のみその変換を行うようにしなければならない.
 *
 * CGIスクリプトがTextモードで標準出力している場合、\nの自動変換によって
 * \r\nは\r\r\nなどとなる.
 * これを救済するため、改行コードに関するこの調整が必要である.
 */

/***
 * endには必ずヘッダ境界の開始位置を指定.
 */
static bool
checkValidHeader2( ZnkBfr bfr, size_t end )
{
	const uint8_t* data = ZnkBfr_data( bfr );
	size_t idx   = 0;
	int    u8    = 0;
	int    state = 0;
	for( idx=0; idx<end; ++idx ){
		u8 = data[ idx ];
		switch( u8 ){
		case '\n':
			if( state != 1 ){
				/* 異常: :がまだ登場していないにも関わらず改行が発生 */
				return false;
			}
			state = 0;
			continue;
		case '\r':
		case '\t':
			continue;
		case ':':
			state = 1;
			continue;
		default:
			break;
		}
		if( u8 < 0x20 && u8 >= 0x80 ){
			/* この範囲にある文字コードが来た場合は異常とする */
			return false;
		}
	}
	/***
	 * 少なくとも : が登場した後に終了とならなければならない
	 */
	return (bool)( state == 1 );
}
static size_t
seekHdrTerminate( ZnkBfr bfr )
{
	static bool   st_initialized = false;
	static size_t st_occ_tbl_rrnrrn[256] = {0};
	static size_t st_occ_tbl_rnrn[256]   = {0};
	static const uint8_t* rrnrrn = (uint8_t*)"\r\r\n\r\r\n";
	static const uint8_t* rnrn   = (uint8_t*)"\r\n\r\n";
	size_t pos1 = Znk_NPOS;
	size_t pos2 = Znk_NPOS;

	if( !st_initialized ){
		ZnkMem_getLOccTable_forBMS( st_occ_tbl_rrnrrn, rrnrrn, 6 );
		ZnkMem_getLOccTable_forBMS( st_occ_tbl_rnrn,   rnrn,   4 );
		st_initialized = true;
	}
	pos1 = ZnkMem_lfind_data_BMS( ZnkBfr_data(bfr), ZnkBfr_size(bfr),
			rrnrrn, 6, 1, st_occ_tbl_rrnrrn );
	pos2 = ZnkMem_lfind_data_BMS( ZnkBfr_data(bfr), ZnkBfr_size(bfr),
			rnrn, 4, 1, st_occ_tbl_rnrn );

	if( pos1 == Znk_NPOS && pos2 == Znk_NPOS ){
		/***
		 * ヘッダ部は存在しない.
		 */
		return Znk_NPOS;
	} else if( pos1 < pos2 ){
		/***
		 * pos1 を ヘッダ部終端と仮定しcheckValidHeaderで最終テスト.
		 */
		if( checkValidHeader2( bfr, pos1 ) ){
			return pos1 + 6;
		}
	} else {
		/***
		 * pos2 を ヘッダ部終端と仮定しcheckValidHeaderで最終テスト.
		 */
		if( checkValidHeader2( bfr, pos2 ) ){
			return pos2 + 4;
		}
	}
	/***
	 * rrnrrn または rnrn は見つかったものの checkValidHeader のテストに合格しなかった.
	 * たまたまそのようなパターンを含むバイナリであるとみなす.
	 */
	return Znk_NPOS;
}
static void
getHdrStrSanitizedRRN( ZnkStr hdr_str, ZnkBfr bfr, size_t hdr_end )
{
	static bool   st_initialized = false;
	static size_t st_occ_tbl_rrn[256] = {0};
	static const uint8_t* rrn = (uint8_t*)"\r\r\n";

	if( !st_initialized ){
		ZnkMem_getLOccTable_forBMS( st_occ_tbl_rrn, rrn, 3 );
		st_initialized = true;
	}

	/***
	 * 少なくとも bfr内における hdr_endまでの範囲は通常の文字列が格納されていると考えてよい.
	 */
	ZnkStr_assign( hdr_str, 0, (char*)ZnkBfr_data( bfr ), hdr_end );

	ZnkStrEx_replace_BMS( hdr_str, 0,
			(const char*)rrn, 3, st_occ_tbl_rrn,
			"\r\n", 2,
			Znk_NPOS, Znk_NPOS );

}

static void
setupServerNameAndServerPort( bool is_xhr_dmz )
{

	if( ZnkS_eq( MoaiServerInfo_acceptable_host(), "ANY" ) ){ 
		const uint32_t private_ip = MoaiServerInfo_private_ip();
		char private_ipstr[ 64 ] = "";
		ZnkNetIP_getIPStr_fromU32( private_ip, private_ipstr, sizeof(private_ipstr) );
		ZnkEnvVar_set( "SERVER_NAME", private_ipstr );
	} else {
		/* Anyway, we set as LOOPBACK */
		ZnkEnvVar_set( "SERVER_NAME", "127.0.0.1" );
	}

	{
		const uint16_t port = is_xhr_dmz ?
			MoaiServerInfo_XhrDMZPort() : MoaiServerInfo_port();
		char port_buf[ 64 ] = "";
		Znk_snprintf( port_buf, sizeof(port_buf), "%u", port );
		ZnkEnvVar_set( "SERVER_PORT", port_buf );
	}
}

/***
 * CGI environment variable list :
 * 厳密にはこれがすべてではない.
 *
 * REQUEST_METHOD :
 *   GET, POST, HEAD などの HTTP Request文字列.
 * QUERY_STRING :
 *   URLにおける「?」文字以降の部分.
 * SCRIPT_NAME :
 *   CGIスクリプトの名前.
 *
 * GATEWAY_INTERFACE :
 *   ゲートウェイプロトコル名称(例えばCGI/1.1)
 * AUTH_TYPE :
 *   認証方式(例えば MD5, Basicなど).
 *
 * REMOTE_IDENT :
 *   クライアント側のユーザーID
 * REMOTE_USER :
 *   クライアント側のユーザー名
 *
 * HTTP_FORWARDED :
 *   この要求をフォワードしたプロキシサーバーの情報.
 * HTTP_REFERER :
 *   いわゆるRefererである.
 * HTTP_X_FORWARDED_FOR :
 *   この要求をフォワードしたプロキシサーバーのIPアドレス.
 */
static void
setupEnv_forGET( ZnkVarpAry hdr_vars, ZnkStr query_str, RanoModule mod, bool is_xhr_dmz )
{
	/* for PHP */
	{
		ZnkEnvVar_set( "REDIRECT_STATUS", "1" );
	}

	/* Phase-1 : Base Info */
	{
		static const char* content_type = "application/x-www-form-urlencoded";
	
		ZnkEnvVar_set( "REQUEST_METHOD", "GET" );
		ZnkEnvVar_set( "CONTENT_TYPE", content_type );
		{
			char content_length_buf[ 256 ] = "";
			Znk_snprintf( content_length_buf, sizeof(content_length_buf), "%zu", ZnkStr_leng(query_str) );
			ZnkEnvVar_set( "CONTENT_LENGTH", content_length_buf );
		}
		ZnkEnvVar_set( "QUERY_STRING", ZnkStr_cstr(query_str) );
#if 0
		/* TODO */
		ZnkEnvVar_set( "SCRIPT_NAME", "cgi_test.cgi" );
#endif
	}

	/* Phase-2 : Seaver Info */
	{
		setupServerNameAndServerPort( is_xhr_dmz );
		ZnkEnvVar_set( "SERVER_PROTOCOL", "HTTP/1.1" );
		if( is_xhr_dmz ){
			ZnkEnvVar_set( "SERVER_SOFTWARE", "Moai XhrDMZ" );
		} else {
			ZnkEnvVar_set( "SERVER_SOFTWARE", "Moai WebServer" );
		}

		ZnkEnvVar_set( "REMOTE_ADDR", "127.0.0.1" );
		ZnkEnvVar_set( "REMOTE_HOST", "127.0.0.1" );
	}

	/* Phase-3 : User-Agent Info */
	{
		ZnkVarp varp = NULL;
		if( mod ){
			const bool is_all_replace = MoaiServerInfo_isAllReplaceCookie();
			ZnkVarpAry extra_vars     = MoaiServerInfo_refCookieExtraVars();
			RanoModule_filtHtpHeader(  mod, hdr_vars );
			RanoModule_filtCookieVars( mod, hdr_vars, is_all_replace, extra_vars );
		}
		varp = ZnkHtpHdrs_find_literal( hdr_vars, "Cookie" );
		if( varp ){
			ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
			ZnkEnvVar_set( "HTTP_COOKIE", ZnkStr_cstr(val) );
			{
				size_t idx;
				size_t size = ZnkHtpHdrs_val_size( varp );
				RanoLog_printf( "MoaiCGI : HTTP_COOKIE debug\n" );
				for( idx=0; idx<size; ++idx ){
					val = ZnkHtpHdrs_val( varp, 0 );
					RanoLog_printf( "MoaiCGI : cookie_val[%zu]=[%s]\n", idx, ZnkStr_cstr(val) );
				}
			}
		}
		varp = ZnkHtpHdrs_find_literal( hdr_vars, "User-Agent" );
		if( varp ){
			ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
			ZnkEnvVar_set( "HTTP_USER_AGENT", ZnkStr_cstr(val) );
		}
		varp = ZnkHtpHdrs_find_literal( hdr_vars, "Accept" );
		if( varp ){
			ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
			ZnkEnvVar_set( "HTTP_ACCEPT", ZnkStr_cstr(val) );
		}
	}
}

static void
setupEnv_forPOST( ZnkVarpAry hdr_vars, size_t content_length, ZnkStr query_str, RanoModule mod, bool is_xhr_dmz )
{
	/* for PHP */
	{
		ZnkEnvVar_set( "REDIRECT_STATUS", "1" );
	}

	/* Phase-1 : Base Info */
	{
		const char* content_type = NULL;

		ZnkEnvVar_set( "REQUEST_METHOD", "POST" );
		content_type = ZnkHtpHdrs_scanContentType( hdr_vars, NULL );
		if( content_type == NULL ){
			/***
			 * Content-Type が存在しない.
			 * このようなPOSTはどう扱うべきか?
			 * 多くの場合、本来エラーとすべきと思われるが、
			 * ここではとりあえず application/x-www-form-urlencoded として取り扱う.
			 */
			content_type = "application/x-www-form-urlencoded";
		}
	
		ZnkEnvVar_set( "CONTENT_TYPE", content_type );
		{
			char content_length_buf[ 256 ];
			Znk_snprintf( content_length_buf, sizeof(content_length_buf), "%zu", content_length );
			ZnkEnvVar_set( "CONTENT_LENGTH", content_length_buf );
		}
		ZnkEnvVar_set( "QUERY_STRING", ZnkStr_cstr(query_str) );

#if 0
		/* TODO */
		ZnkEnvVar_set( "SCRIPT_NAME", "cgi_test.cgi" );
#endif
	}

	/* Phase-2 : Seaver Info */
	{
		setupServerNameAndServerPort( is_xhr_dmz );
		ZnkEnvVar_set( "SERVER_PROTOCOL", "HTTP/1.1" );
		if( is_xhr_dmz ){
			ZnkEnvVar_set( "SERVER_SOFTWARE", "Moai XhrDMZ" );
		} else {
			ZnkEnvVar_set( "SERVER_SOFTWARE", "Moai WebServer" );
		}

		/* TODO */
		ZnkEnvVar_set( "REMOTE_ADDR", "127.0.0.1" );
		ZnkEnvVar_set( "REMOTE_HOST", "127.0.0.1" );
#if 0
		ZnkEnvVar_set( "REMOTE_PORT", "0" );
#endif

	}

	/* Phase-3 : User-Agent Info */
	{
		ZnkVarp varp = NULL;
		if( mod ){
			const bool is_all_replace = MoaiServerInfo_isAllReplaceCookie();
			ZnkVarpAry extra_vars     = MoaiServerInfo_refCookieExtraVars();
			RanoModule_filtHtpHeader(  mod, hdr_vars );
			RanoModule_filtCookieVars( mod, hdr_vars, is_all_replace, extra_vars );
		}
		varp = ZnkHtpHdrs_find_literal( hdr_vars, "Cookie" );
		if( varp ){
			ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
			ZnkEnvVar_set( "HTTP_COOKIE", ZnkStr_cstr(val) );
		}
		varp = ZnkHtpHdrs_find_literal( hdr_vars, "User-Agent" );
		if( varp ){
			ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
			ZnkEnvVar_set( "HTTP_USER_AGENT", ZnkStr_cstr(val) );
		}
		varp = ZnkHtpHdrs_find_literal( hdr_vars, "Accept" );
		if( varp ){
			ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
			ZnkEnvVar_set( "HTTP_ACCEPT", ZnkStr_cstr(val) );
		}
	}
}

static void
sendChunk( ZnkSocket sock, ZnkBfr bfr )
{
	char chunk_size_line[ 256 ] = "";
	Znk_snprintf( chunk_size_line, sizeof(chunk_size_line), "%zx\r\n", ZnkBfr_size(bfr) );
	ZnkSocket_send_cstr( sock, chunk_size_line );
	ZnkSocket_send( sock, ZnkBfr_data(bfr), ZnkBfr_size(bfr) );
	ZnkSocket_send_cstr( sock, "\r\n" );
	ZnkBfr_clear( bfr );
}


static void
appointCGISockAsExile( ZnkSocket sock )
{
	MoaiConnection mcn = MoaiConnection_find_byISock( sock );
	if( mcn ){
		ZnkSocket O_sock = MoaiConnection_O_sock( mcn );
		if( O_sock == sock ){
			/***
			 * sock を Exile に戻す.
			 * 自動抹殺の対象とする.
			 */
			mcn->O_sock_ = ZnkSocket_INVALID;
		}
	}
}


#if defined(Znk_TARGET_WINDOWS)
#  include<windows.h>

typedef struct {
	HANDLE    hChildProcess_;
	HANDLE    hOutputRead_;
	HANDLE    hInputWrite_;
	ZnkSocket sock_;
} ChildProcessInfo;

/**
 * Displays the error number and corresponding message.
 */
static void
printError( char* pszAPI )
{
	const int error_code = GetLastError();
	if( error_code != 0 ){
		LPVOID lpvMessageBuffer;
		
		FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
				NULL, error_code,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&lpvMessageBuffer, 0, NULL);
		
		RanoLog_printf( "MoaiCGI : Error: %s : error_code=[%d] message=[%s].\n",
				pszAPI, error_code, (char *)lpvMessageBuffer );
		
		LocalFree(lpvMessageBuffer);
	}
}

static BOOL
closeHandle( HANDLE handle ){

	if( handle && handle != INVALID_HANDLE_VALUE ){
		return CloseHandle( handle );
	}
	return FALSE;
}

static void
ReadAndHandleOutput_forWin32( HANDLE hPipeRead, ZnkSocket sock, bool use_chunked_mode )
{
	uint8_t ubuf[ 4096 ] = { 0 };
	DWORD   nBytesRead = 0;
	ZnkBfr  bfr = ZnkBfr_create_null();
	BOOL    result;
	size_t  hdr_end = Znk_NPOS;

	/***
	 * use_chunked_mode ではReadFileが成功したら
	 * HTTPとしては chunked 形式として即ソケットへ送る.
	 * ただし最初のヘッダ部だけはまず完全に読み込む.
	 */
	RanoLog_printf( "MoaiCGI : ReadAndHandleOutput_forWin32 : begin.\n" );
	while( true ){
		result = ReadFile( hPipeRead, ubuf, sizeof(ubuf), &nBytesRead, NULL );
		if( !result || nBytesRead == 0 ){
			if( GetLastError() == ERROR_BROKEN_PIPE ){
				/***
				 * pipe done - normal exit path.
				 */
				break;
			} else {
				printError("ReadFile"); // Something bad happened.
				break;
			}
		}
		if( ubuf[ nBytesRead-1 ] == '\0' ){
			RanoLog_printf( "MoaiCGI : ReadAndHandleOutput_forWin32 : null-terminate.\n" );
		}
		ZnkBfr_append_dfr( bfr, ubuf, nBytesRead );

		hdr_end = seekHdrTerminate( bfr );
		if( hdr_end != Znk_NPOS ){
			break;
		}
	}

	if( hdr_end == Znk_NPOS ){
		/***
		 * 最初のヘッダ部が存在しないパターン.
		 * broken pipe により上のループを終了しているはずであり、
		 * 既にbody部も含めすべて読み終わっているはずである.
		 */
		sendResponseOfCGI( bfr, sock );

	} else {
		ZnkStr hdr_str = ZnkStr_new( "" );

		/***
		 * 最初のヘッダ部内の RRN というパターンをRNに置き換えたものを取得しておく.
		 */
		getHdrStrSanitizedRRN( hdr_str, bfr, hdr_end );

		/***
		 * ヘッダ部はもはや不要なので消去しておく.
		 */
		ZnkBfr_erase( bfr, 0, hdr_end );

		if( use_chunked_mode ){

			/***
			 * hdr_strを送信.
			 */
			sendHtpHdrOfCGI_forChunked( hdr_str, sock );
			/***
			 * 最初のchunkを送信.
			 */
			if( ZnkBfr_size( bfr ) ){
				sendChunk( sock, bfr );
			}

			while( true ){
				result = ReadFile( hPipeRead, ubuf, sizeof(ubuf), &nBytesRead, NULL );
				if( !result || nBytesRead == 0 ){
					if( GetLastError() == ERROR_BROKEN_PIPE ){
						/***
						 * pipe done - normal exit path.
						 */
						break;
					} else {
						printError("ReadFile"); // Something bad happened.
						break;
					}
				}
				/* chunkの送信. */
				ZnkBfr_append_dfr( bfr, ubuf, nBytesRead );
				if( ZnkBfr_size( bfr ) ){
					sendChunk( sock, bfr );
				}
			}
			/***
			 * 終端chunk
			 * 0のみからなる行と空行を送る.
			 * 従って、0 の後ろには \r\n を二つ置かなければならないことに注意する.
			 * (ややこしい感じがするが、これが 0 byteのchunkを送るということである).
			 */
			ZnkSocket_send( sock, (uint8_t*)"0\r\n\r\n", 5 );
		} else {
			size_t content_length = 0;

			/***
			 * 残りを完全に読んでから送信する.
			 */
			while( true ){
				result = ReadFile( hPipeRead, ubuf, sizeof(ubuf), &nBytesRead, NULL );
				if( !result || nBytesRead == 0 ){
					if( GetLastError() == ERROR_BROKEN_PIPE ){
						/***
						 * pipe done - normal exit path.
						 */
						break;
					} else {
						printError("ReadFile"); // Something bad happened.
						break;
					}
				}
				ZnkBfr_append_dfr( bfr, ubuf, nBytesRead );
			}

			/***
			 * bodyの全サイズが確定.
			 */
			content_length = ZnkBfr_size( bfr );
			/***
			 * hdr_strを送信.
			 */
			sendHtpHdrOfCGI_withContentLength( hdr_str, sock, content_length );
			/***
			 * body部( 一般にバイナリの可能性もある )を送信.
			 */
			ZnkSocket_send( sock, ZnkBfr_data(bfr), ZnkBfr_size(bfr) );
		}
		
		ZnkStr_delete( hdr_str );
	}

	ZnkBfr_destroy( bfr );
	RanoLog_printf( "MoaiCGI : ReadAndHandleOutput_forWin32 : end.\n" );
}

static int
waitForCGIEnd_forWin32( HANDLE hChildProcess, HANDLE hOutputRead, HANDLE hInputWrite, ZnkSocket sock )
{
	int result = 0;
	DWORD r = 0;
	/**
	 * 標準出力の捕捉はメインスレッドでの無限ループで行う.
	 */
	ReadAndHandleOutput_forWin32( hOutputRead, sock, true );
	if( !closeHandle(hOutputRead) ){
		//printError("closeHandle:hOutputRead");
	}

	if( !closeHandle(hInputWrite) ){
		printError("closeHandle:hInputWrite");
	}

	r = WaitForSingleObject( hChildProcess, INFINITE );
	switch( r ){
	case WAIT_FAILED:
		printError("WaitForSingleObject");
		result = -1;
		break;
	case WAIT_ABANDONED:
		RanoLog_printf("MoaiCGI : wait result=WAIT_ABANDONED\n");
		result = -1;
		break;
	case WAIT_OBJECT_0: //正常終了
		RanoLog_printf("MoaiCGI : wait result=WAIT_OBJECT_0\n");
		result = 0;
		break;
	case WAIT_TIMEOUT:
		RanoLog_printf("MoaiCGI : wait result=WAIT_TIMEOUT\n");
		result = -1;
		break;
	default:
		RanoLog_printf("MoaiCGI : wait result=[%d]\n", r);
		result = -1;
		break;
	}

	if( !closeHandle(hChildProcess) ){
		//printError("closeHandle:hChildProcess");
	}

	--st_cgi_count;
	return result;
}

static void*
waitForCGIEndThread( void* arg )
{
	int result = 0;
	ChildProcessInfo* cpinfo = Znk_force_ptr_cast( ChildProcessInfo*, arg );
	HANDLE hChildProcess = cpinfo->hChildProcess_;
	HANDLE hOutputRead   = cpinfo->hOutputRead_;
	HANDLE hInputWrite   = cpinfo->hInputWrite_;
	ZnkSocket sock       = cpinfo->sock_;

	RanoLog_printf( "MoaiCGI : waitForCGIEndThread begin.\n" );

	Znk_free( cpinfo );

	result = waitForCGIEnd_forWin32( hChildProcess, hOutputRead, hInputWrite, sock );

	appointCGISockAsExile( sock );

	RanoLog_printf( "MoaiCGI : waitForCGIEndThread end.\n" );
	return Znk_force_ptr_cast( void*, result );
}

/**
 * Sets up STARTUPINFO structure, and launches redirected child.
 */
static HANDLE
PrepAndLaunchRedirectedChild( const char* cmd, const char* curdir_new, HANDLE hChildStdOut, HANDLE hChildStdIn, HANDLE hChildStdErr )
{
	HANDLE hChildProcess = NULL;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL result;
	ZnkStr curdir_save = ZnkStr_new( "" );
	
	/***
	 * Set up the start up info struct.
	 */
	ZeroMemory(&si,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdOutput = hChildStdOut;
	si.hStdInput  = hChildStdIn;
	si.hStdError  = hChildStdErr;

	/***
	 * Use this if you want to hide the child:
	 */
	// si.wShowWindow = SW_HIDE;
	/*
	 * Note that dwFlags must include STARTF_USESHOWWINDOW if you want to use the wShowWindow flags.
	 ***/

	/***
	 * CreateProcessの第5引数にTRUEを指定すれば、ハンドルは作成したプロセスに継承される.
	 * 第6引数に CREATE_NO_WINDOW を指定すると、作成したプロセスでコンソールが示されなくなる.
	 *
	 * Launch the process that you want to redirect (in this case, Child.exe).
	 * Make sure Child.exe is in the same directory as redirect.c launch redirect
	 * from a command line to prevent location confusion.
	 */
	ZnkDir_getCurrentDir( curdir_save );
	ZnkDir_changeCurrentDir( curdir_new );
	result = CreateProcess( NULL, (char*)cmd,
			//NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi );
			NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi );
			//NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, curdir_new, &si, &pi );
	ZnkDir_changeCurrentDir( ZnkStr_cstr(curdir_save) );
	RanoLog_printf( "MoaiCGI : Report : curdir_new=[%s]\n", curdir_new ? curdir_new : "NULL" );
	if( !result ){
	   printError( "CreateProcess" );
	}

	/***
	 * Set global child process handle to cause threads to exit.
	 */
	hChildProcess = pi.hProcess;

	/***
	 * Close any unnecessary handles.
	 */
	if( !closeHandle(pi.hThread) ){
		printError( "closeHandle:pi.hThread" );
	}
	ZnkStr_delete( curdir_save );

	return hChildProcess;
}

static int
runCGIProcess_forWin32( const char* cmd, const char* curdir_new, ZnkSocket sock,
		uint8_t* data, size_t data_size,
		bool enable_std_input )
{
	int result = 0;
	HANDLE hOutputRead    = NULL;
	HANDLE hOutputReadTmp = NULL;
	HANDLE hOutputWrite   = NULL;
	HANDLE hInputRead     = NULL;
	HANDLE hInputWrite    = NULL;
	HANDLE hInputWriteTmp = NULL;
	HANDLE hErrorWrite    = NULL;
	HANDLE hChildProcess  = NULL;
	SECURITY_ATTRIBUTES sa = { 0 };

	/***
	 * Set up the security attributes struct.
	 */
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	/***
	 * Create the child output pipe.
	 */
	if( !CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0) ){
		printError("CreatePipe:StdOut");
	}


	/***
	 * 以下は思惑通りにいかない.
	 * 調査はとりあえず保留.
	 */
#if 0
	/***
	 * とりあえずstderrへの出力の場合はCGIへは反映させないようにする.
	 * ただし単にここを無効にしただけでは、Moai 側のコンソールに出力する
	 * などといったことは出来ない.
	 */
	/***
	 * Create a duplicate of the output write handle for the std error write handle.
	 * This is necessary in case the child application closes one of its std output handles.
	 */
	if( !DuplicateHandle(
				GetCurrentProcess(), hOutputWrite,
				GetCurrentProcess(), &hErrorWrite,
				0, TRUE,
				DUPLICATE_SAME_ACCESS ) )
	{
		printError("DuplicateHandle:hOutputWrite");
	}
	{
		HANDLE hStdError = GetStdHandle( STD_ERROR_HANDLE );
		if( hStdError != INVALID_HANDLE_VALUE ){
			if( !DuplicateHandle(
						GetCurrentProcess(), hStdError,
						GetCurrentProcess(), &hErrorWrite,
						0, TRUE,
						DUPLICATE_SAME_ACCESS ) )
			{
				printError("DuplicateHandle:hErrorWrite");
			}
		}
	}
#endif


	/***
	 * Create the child input pipe.
	 */
	if( enable_std_input ){
		if( !CreatePipe(&hInputRead,&hInputWriteTmp,&sa,0) ){
			printError("CreatePipe:StdIn");
		}
	}

	/***
	 * Create new output read handle and the input write handles.
	 * Set the Properties to FALSE. Otherwise, the child inherits the properties and,
	 * as a result, non-closeable handles to the pipes are created.
	 */
	if( !DuplicateHandle(
				GetCurrentProcess(), hOutputReadTmp,
				GetCurrentProcess(), &hOutputRead, // Address of new handle.
				0, FALSE,                          // Make it uninheritable.
				DUPLICATE_SAME_ACCESS ) )
	{
		printError("DupliateHandle:hOutputReadTmp");
	}

	if( hInputWriteTmp ){
		if (!DuplicateHandle(
					GetCurrentProcess(), hInputWriteTmp,
					GetCurrentProcess(), &hInputWrite, // Address of new handle.
					0, FALSE,                          // Make it uninheritable.
					DUPLICATE_SAME_ACCESS))
		{
			printError("DupliateHandle");
		}
	}

	/***
	 * Close inheritable copies of the handles you do not want to be inherited.
	 */
	if (!closeHandle(hOutputReadTmp)){
		//printError("closeHandle:hOutputReadTmp");
	}
	if (!closeHandle(hInputWriteTmp)){
		//printError("closeHandle:hInputWriteTmp");
	}

	hChildProcess = PrepAndLaunchRedirectedChild( cmd, curdir_new, hOutputWrite, hInputRead, hErrorWrite );

	/***
	 * Close pipe handles (do not continue to modify the parent).
	 * You need to make sure that no handles to the write end of the output pipe are maintained
	 * in this process or else the pipe will not close when the child process exits
	 * and the ReadFile will hang.
	 */
	if( !closeHandle(hOutputWrite) ){
		//printError("closeHandle:hOutputWrite");
	}
	if( !closeHandle(hErrorWrite) ){
		//printError("closeHandle:hErrorWrite");
	}
	if( !closeHandle(hInputRead) ){
		//printError("closeHandle:hInputRead");
	}
	

	if( hInputWrite ){
		DWORD nBytesWrote = 0;
		if( !WriteFile( hInputWrite, data, data_size, &nBytesWrote, NULL) ){
			if( GetLastError() == ERROR_NO_DATA ){
				/* Pipe was closed (normal exit path). */
			} else {
				printError("WriteFile");
			}
		}
		/***
		 * 最後のterminate nulは必要.
		 * CGI側でfgetcなどでのstdinの入力待ちが存在する場合に
		 * それをこれで確実に終らせなければならない.
		 */
		if( !WriteFile( hInputWrite, "\0", 1, &nBytesWrote, NULL) ){
			if( GetLastError() == ERROR_NO_DATA ){
				/* Pipe was closed (normal exit path). */
			} else {
				printError("WriteFile");
			}
		}
	}

	++st_cgi_count;
	if( st_cgi_count < st_cgi_count_max ){
		MoaiConnection mcn = NULL;
		ChildProcessInfo* cpinfo = Znk_malloc( sizeof(ChildProcessInfo) );
		cpinfo->sock_          = sock;
		cpinfo->hOutputRead_   = hOutputRead;
		cpinfo->hInputWrite_   = hInputWrite;
		cpinfo->hChildProcess_ = hChildProcess; 

		mcn = MoaiConnection_find_byISock( sock );
		if( mcn ){
			ZnkSocket O_sock = MoaiConnection_O_sock( mcn );
			if( O_sock == ZnkSocket_INVALID ){
				/***
				 * Exile sockとして自動抹殺されてしまうのを防止するため、
				 * O_sock に ZnkSocket_INVALID 以外の値を強制的に代入しておく.
				 * sock と同じ値でよいだろう.
				 */
				mcn->O_sock_ = sock;
			}
		}

		/* 子プロセスの終了待ちスレッドを一つ立ち上げる.
		 * cpinfo の解放は waitForCGIEndThread 内で行う. */
		ZnkThread_create( waitForCGIEndThread, cpinfo );
	} else {
		/***
		 * CGI同時起動数がMAXとなった場合はMainスレッドで終了を待つ.
		 *
		 * 待ち行列に入れてた方がよいという考え方もあるが、そもそも最初に登録された連中が
		 * そこまで時間の掛かる処理であるというなら、それらの終了を待つより最新の要求を
		 * さっさと裁くべきという考え方もできる.
		 * 一番よいのは登録されたすべてのスレッドからもっとも早く終了したものを捕捉することで
		 * Windows ならWaitForMultipleObjectsExを使えば実現できそうだがとりあえず現段階ではメインスレッドで待つ仕様にしておく.
		 */
		result = waitForCGIEnd_forWin32( hChildProcess, hOutputRead, hInputWrite, sock );
	}

	return result;
}

#elif defined(Znk_TARGET_UNIX)
#  include <stdio.h>
#  include <unistd.h>
#  include <signal.h>
#  include <stdlib.h>
#  include <sys/types.h> /* for waitpid */
#  include <sys/wait.h>  /* for waitpid */

#define R (0)
#define W (1)

typedef struct {
	pid_t     child_process_pid_;
	int       fd_OutputRead_;
	int       fd_InputWrite_;
	ZnkSocket sock_;
} ChildProcessInfo;

static void
ReadAndHandleOutput_forUNIX( int fd_OutputRead, ZnkSocket sock, bool use_chunked_mode )
{
	uint8_t ubuf[ 4096 ] = { 0 };
	long    readed_size = 0;
	ZnkBfr  bfr = ZnkBfr_create_null();
	//bool    result;
	size_t  hdr_end = Znk_NPOS;

	/***
	 * use_chunked_mode ではReadFileが成功したら
	 * HTTPとしては chunked 形式として即ソケットへ送る.
	 * ただし最初のヘッダ部だけはまず完全に読み込む.
	 */
	RanoLog_printf( "MoaiCGI : ReadAndHandleOutput_forUNIX : begin.\n" );
	while( true ){
		ubuf[ 0 ] = '\0';
		readed_size = read( fd_OutputRead, ubuf, sizeof(ubuf)-1 );
		if( readed_size <= 0 ){
			/***
			 * pipe done - normal exit path.
			 */
			break;
		}
		if( ubuf[ readed_size-1 ] == '\0' ){
			RanoLog_printf( "MoaiCGI : ReadAndHandleOutput_forUNIX : null-terminate.\n" );
		}
		ZnkBfr_append_dfr( bfr, ubuf, readed_size );

		hdr_end = seekHdrTerminate( bfr );
		if( hdr_end != Znk_NPOS ){
			break;
		}
	}

	if( hdr_end == Znk_NPOS ){
		/***
		 * 最初のヘッダ部が存在しないパターン.
		 * broken pipe により上のループを終了しているはずであり、
		 * 既にbody部も含めすべて読み終わっているはずである.
		 */
		sendResponseOfCGI( bfr, sock );

	} else {
		ZnkStr hdr_str = ZnkStr_new( "" );

		/***
		 * 最初のヘッダ部内の RRN というパターンをRNに置き換えたものを取得しておく.
		 */
		getHdrStrSanitizedRRN( hdr_str, bfr, hdr_end );

		/***
		 * ヘッダ部はもはや不要なので消去しておく.
		 */
		ZnkBfr_erase( bfr, 0, hdr_end );

		if( use_chunked_mode ){

			/***
			 * hdr_strを送信.
			 */
			sendHtpHdrOfCGI_forChunked( hdr_str, sock );
			/***
			 * 最初のchunkを送信.
			 */
			if( ZnkBfr_size( bfr ) ){
				sendChunk( sock, bfr );
			}

			while( true ){
				ubuf[ 0 ] = '\0';
				readed_size = read( fd_OutputRead, ubuf, sizeof(ubuf)-1 );
				if( readed_size <= 0 ){
					/***
					 * pipe done - normal exit path.
					 */
					break;
				}
				/* chunkの送信. */
				ZnkBfr_append_dfr( bfr, ubuf, readed_size );
				if( ZnkBfr_size( bfr ) ){
					sendChunk( sock, bfr );
				}
			}
			/***
			 * 終端chunk
			 * 0のみからなる行と空行を送る.
			 * 従って、0 の後ろには \r\n を二つ置かなければならないことに注意する.
			 * (ややこしい感じがするが、これが 0 byteのchunkを送るということである).
			 */
			ZnkSocket_send( sock, (uint8_t*)"0\r\n\r\n", 5 );
		} else {
			size_t content_length = 0;

			/***
			 * 残りを完全に読んでから送信する.
			 */
			while( true ){
				ubuf[ 0 ] = '\0';
				readed_size = read( fd_OutputRead, ubuf, sizeof(ubuf)-1 );
				if( readed_size <= 0 ){
					/***
					 * pipe done - normal exit path.
					 */
					break;
				}
				ZnkBfr_append_dfr( bfr, ubuf, readed_size );
			}

			/***
			 * bodyの全サイズが確定.
			 */
			content_length = ZnkBfr_size( bfr );
			/***
			 * hdr_strを送信.
			 */
			sendHtpHdrOfCGI_withContentLength( hdr_str, sock, content_length );
			/***
			 * body部( 一般にバイナリの可能性もある )を送信.
			 */
			ZnkSocket_send( sock, ZnkBfr_data(bfr), ZnkBfr_size(bfr) );
		}
		
		ZnkStr_delete( hdr_str );
	}

	ZnkBfr_destroy( bfr );
	RanoLog_printf( "MoaiCGI : ReadAndHandleOutput_forUNIX : end.\n" );
}

static int
waitForCGIEnd_forUNIX( pid_t child_process_pid, int fd_OutputRead, int fd_InputWrite, ZnkSocket sock )
{
	int result  = 0;
	int state   = 0;
	int options = 0;
	/**
	 * 標準出力の捕捉はメインスレッドでの無限ループで行う.
	 */
	ReadAndHandleOutput_forUNIX( fd_OutputRead, sock, true );

	close( fd_OutputRead );
	close( fd_InputWrite );

	result = waitpid( child_process_pid, &state, options );

	--st_cgi_count;
	return result;
}

static void*
waitForCGIEndThread( void* arg )
{
	int result = 0;
	ChildProcessInfo* cpinfo = Znk_force_ptr_cast( ChildProcessInfo*, arg );
	pid_t child_process_pid = cpinfo->child_process_pid_;
	int   fd_OutputRead     = cpinfo->fd_OutputRead_;
	int   fd_InputWrite     = cpinfo->fd_InputWrite_;
	ZnkSocket sock          = cpinfo->sock_;

	RanoLog_printf( "MoaiCGI : waitForCGIEndThread begin.\n" );

	Znk_free( cpinfo );

	result = waitForCGIEnd_forUNIX( child_process_pid, fd_OutputRead, fd_InputWrite, sock );

	appointCGISockAsExile( sock );

	RanoLog_printf( "MoaiCGI : waitForCGIEndThread end.\n" );
	return Znk_force_ptr_cast( void*, result );
}

static ZnkStrAry
makeCmdList( const char* cmd, const char*** cmd_argv_and )
{
	ZnkStrAry    cmd_list = ZnkStrAry_create( true );
	const char** cmd_argv = NULL;
	size_t       cmd_size = 0;
	size_t       idx;

	ZnkStrEx_addSplitCSet( cmd_list,
			cmd, Znk_strlen(cmd),
			" \t", 2,
			4 );
	cmd_size = ZnkStrAry_size(cmd_list);
	cmd_argv = Znk_malloc( sizeof(char*) * ( cmd_size + 1 ) );

	for( idx=0; idx<cmd_size; ++idx ){
		cmd_argv[ idx ] = ZnkStrAry_at_cstr(cmd_list, idx);
	}
	cmd_argv[ cmd_size ] = NULL;

	*cmd_argv_and = cmd_argv;
	return cmd_list;
}

int
popen2( const char* cmd_file, const char** cmd_argv, const char* curdir_new, int* fd_r, int* fd_w )
{
	int pipe_c2p[ 2 ];
	int pipe_p2c[ 2 ];
	int pid;

	/* Create two of pipes. */
	if( pipe( pipe_c2p ) < 0 ){
		perror( "popen2" );
		return -1;
	}
	if( pipe(pipe_p2c) < 0 ){
		perror( "popen2" );
		close( pipe_c2p[R] );
		close( pipe_c2p[W] );
		return -1;
	}

	/* Invoke processs */
	if( ( pid=fork() ) < 0 ){
		perror( "popen2" );
		close( pipe_c2p[R] );
		close( pipe_c2p[W] );
		close( pipe_p2c[R] );
		close( pipe_p2c[W] );
		return -1;
	}
	if( pid == 0 ){
		/* I'm Child */
		close( pipe_p2c[W] );
		close( pipe_c2p[R] );
		dup2( pipe_p2c[R], 0 ); /* Parent2Childで子側からは読みとなる、つまりこのパイプの出口. それを標準入力へ上書きコピー. */
		dup2( pipe_c2p[W], 1 ); /* Child2Parentで子側からは書きとなる、つまりこのパイプの入口. それを標準出力へ上書きコピー. */
		close( pipe_p2c[R] );
		close( pipe_c2p[W] );
		ZnkDir_changeCurrentDir( curdir_new );
		if( execvp( cmd_file, Znk_force_ptr_cast( char* const*, cmd_argv ) ) < 0 ){
			perror( "popen2:execvp" );
			close( pipe_p2c[R] );
			close( pipe_c2p[W] );
			exit( EXIT_FAILURE );
		}
	}
	/* I'm Parent */
	close( pipe_p2c[R] );
	close( pipe_c2p[W] );
	*fd_w = pipe_p2c[W]; /* Parent2Childで親側からは書きとなる、つまりこのパイプの入口 */
	*fd_r = pipe_c2p[R]; /* Child2Parentで親側からは読みとなる、つまりこのパイプの出口 */
	
	return pid;
}

static int
runCGIProcess_forUNIX( const char* cmd, const char* curdir_new, ZnkSocket sock,
		uint8_t* data, size_t data_size,
		bool is_write_terminate_nul )
{
	int   fd_r = -1;
	int   fd_w = -1;
	pid_t child_process_pid = -1;
	int   result = 0;

	const char** cmd_argv = NULL;
	ZnkStrAry    cmd_list = makeCmdList( cmd, &cmd_argv );

	child_process_pid = popen2( cmd_argv[0], cmd_argv, curdir_new, &fd_r, &fd_w );

	/***
	 * 最後のterminate nulは必要.
	 * CGI側でfgetcなどでのstdinの入力待ちが存在する場合に
	 * それをこれで確実に終らせなければならない.
	 */
	write( fd_w, data, data_size );
	if( is_write_terminate_nul ){
		write( fd_w, "\0", 1 );
	}

	++st_cgi_count;
	if( st_cgi_count < st_cgi_count_max ){
		MoaiConnection mcn = NULL;
		ChildProcessInfo* cpinfo = Znk_malloc( sizeof(ChildProcessInfo) );
		cpinfo->sock_          = sock;
		cpinfo->fd_OutputRead_ = fd_r;
		cpinfo->fd_InputWrite_ = fd_w;
		cpinfo->child_process_pid_ = child_process_pid;

		mcn = MoaiConnection_find_byISock( sock );
		if( mcn ){
			ZnkSocket O_sock = MoaiConnection_O_sock( mcn );
			if( O_sock == ZnkSocket_INVALID ){
				/***
				 * Exile sockとして自動抹殺されてしまうのを防止するため、
				 * O_sock に ZnkSocket_INVALID 以外の値を強制的に代入しておく.
				 * sock と同じ値でよいだろう.
				 */
				mcn->O_sock_ = sock;
			}
		}

		/* 子プロセスの終了待ちスレッドを一つ立ち上げる.
		 * cpinfo の解放は waitForCGIEndThread 内で行う. */
		ZnkThread_create( waitForCGIEndThread, cpinfo );
	} else {
		/***
		 * CGI同時起動数がMAXとなった場合はMainスレッドで終了を待つ.
		 *
		 * 待ち行列に入れてた方がよいという考え方もあるが、そもそも最初に登録された連中が
		 * そこまで時間の掛かる処理であるというなら、それらの終了を待つより最新の要求を
		 * さっさと裁くべきという考え方もできる.
		 * 一番よいのは登録されたすべてのスレッドからもっとも早く終了したものを捕捉することで
		 * Windows ならWaitForMultipleObjectsExを使えば実現できそうだがとりあえず現段階ではメインスレッドで待つ仕様にしておく.
		 */
		result = waitForCGIEnd_forUNIX( child_process_pid, fd_r, fd_w, sock );
	}


	Znk_free( cmd_argv );
	ZnkStrAry_destroy( cmd_list );
	return result;
}

#else

#  error "Not Supported"

#endif



int
MoaiCGI_runGet( const char* cmd, const char* curdir_new, ZnkSocket sock, RanoModule mod,
		const size_t hdr_size, ZnkStrAry hdr1st, ZnkVarpAry hdr_vars,
		size_t content_length, ZnkBfr stream, ZnkStr query_str, bool is_xhr_dmz )
{

#if defined(Znk_TARGET_WINDOWS)
	static const bool enable_std_input = false;
	setupEnv_forGET( hdr_vars, query_str, mod, is_xhr_dmz );
	return runCGIProcess_forWin32( cmd, curdir_new, sock,
			ZnkBfr_data(stream)+hdr_size, ZnkBfr_size(stream)-hdr_size,
			enable_std_input );

#elif defined(Znk_TARGET_UNIX)
	setupEnv_forGET( hdr_vars, query_str, mod, is_xhr_dmz );
	/***
	 * QUERY_STRINGが存在する場合
	 * PHPではおそらくQUERY_STRING全体がそこにあるかどうかをチェックしているようで
	 * それ全体をここで送らなければ処理待ちが発生し、制御が戻らなくなってしまう.
	 */
	return runCGIProcess_forUNIX( cmd, curdir_new, sock, 
			(uint8_t*)ZnkStr_cstr( query_str ), ZnkStr_leng( query_str )+1,
			false );
#endif
}

int
MoaiCGI_runPost( const char* cmd, const char* curdir_new, ZnkSocket sock, RanoModule mod,
		const size_t hdr_size, ZnkStrAry hdr1st, ZnkVarpAry hdr_vars,
		size_t content_length, ZnkBfr stream, ZnkStr query_str, bool is_xhr_dmz )
{

#if defined(Znk_TARGET_WINDOWS)
	static const bool enable_std_input = true;
	setupEnv_forPOST( hdr_vars, content_length, query_str, mod, is_xhr_dmz );
	return runCGIProcess_forWin32( cmd, curdir_new, sock,
			ZnkBfr_data(stream)+hdr_size, ZnkBfr_size(stream)-hdr_size,
			enable_std_input );

#elif defined(Znk_TARGET_UNIX)
	setupEnv_forPOST( hdr_vars, content_length, query_str, mod, is_xhr_dmz );
	return runCGIProcess_forUNIX( cmd, curdir_new, sock, 
			ZnkBfr_data(stream)+hdr_size, ZnkBfr_size(stream)-hdr_size,
			true );
#endif
}


