#include <Rano_htp_boy.h>
#include <Rano_log.h>
#include <Rano_post.h>

#include <Znk_zlib.h>
#include <Znk_stdc.h>
#include <Znk_net_base.h>
#include <Znk_htp_rar.h>
#include <Znk_htp_hdrs.h>
#include <Znk_htp_util.h>
#include <Znk_s_base.h>
#include <Znk_str.h>
#include <Znk_str_fio.h>
#include <Znk_str_path.h>
#include <Znk_cookie.h>
#include <Znk_missing_libc.h>
#include <Znk_dir.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char st_tmpdir_common[ 256 ] = "tmp/";

void
RanoHtpBoy_setTmpDirCommon( const char* tmpdir_common )
{
	if( !ZnkS_empty( tmpdir_common ) ){
		size_t leng = Znk_strlen( tmpdir_common );
		if( tmpdir_common[ leng ] == '/' ){
			Znk_snprintf( st_tmpdir_common, sizeof(st_tmpdir_common), "%s", tmpdir_common );
		} else {
			Znk_snprintf( st_tmpdir_common, sizeof(st_tmpdir_common), "%s/", tmpdir_common );
		}
	}
}

/***
 * User-Agent, Referer, CookieなどでHTTPヘッダを装飾する.
 */
static bool
loadHdrs( ZnkHtpHdrs hdrs, const char* send_hdr_filename )
{
	ZnkFile fp = Znk_fopen( send_hdr_filename, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		ZnkStrFIO_fgets( line, 0, 4096, fp );
		ZnkStr_chompNL( line );
		ZnkHtpHdrs_registHdr1st( hdrs->hdr1st_, ZnkStr_cstr(line), ZnkStr_leng(line) );
		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				/* eof */
				break;
			}
			ZnkStr_chompNL( line );
			ZnkHtpHdrs_regist_byLine( hdrs->vars_, ZnkStr_cstr(line), ZnkStr_leng(line) );
		}
		Znk_fclose( fp );
		ZnkStr_delete( line );
		return true;
	}
	return false;
}
static bool
loadBody( ZnkBfr ans, const char* body_img_filename )
{
	if( body_img_filename ){
		uint8_t buf[ 1024 ];
		ZnkFile fp = Znk_fopen( body_img_filename, "rb" );
		if( fp ){
			size_t readed_size = 0;
			while( !Znk_feof( fp ) ){
				readed_size = Znk_fread( buf, 1024, fp );
				ZnkBfr_append_dfr( ans, buf, readed_size );
			}
			Znk_fclose( fp );
			return true;
		}
	}
	return false;
}

static int
writeFP( void* arg, const uint8_t* buf, size_t buf_size )
{
	ZnkFile fp = (ZnkFile)arg;
	const size_t result_size = Znk_fwrite( buf, buf_size, fp );
	return result_size;
}

static bool
saveText( const char* filename, ZnkStr text )
{
	ZnkFile fp = Znk_fopen( filename, "wb" );
	if( fp ){
		Znk_fwrite( (uint8_t*)ZnkStr_cstr(text), ZnkStr_leng(text), fp );
		Znk_fclose( fp );
		return true;
	}
	return false;
}

bool
RanoHtpBoy_process( const char* cnct_hostname, uint16_t cnct_port,
		const char* result_filename, const char* cookie_filename,
		const char* send_hdr_filename, const char* body_img_filename,
		const char* recv_hdr_filename )
{
	bool result = false;
	ZnkBfr     send_body = ZnkBfr_create_null();
	ZnkVarpAry cookie    = ZnkVarpAry_create(true);
	ZnkBfr     wk_bfr    = ZnkBfr_create_null();
	ZnkFile    fp        = NULL;
	ZnkStr     rmsg      = ZnkStr_new( "" );

	struct ZnkHtpHdrs_tag send_hdrs = { 0 };
	struct ZnkHtpHdrs_tag recv_hdrs = { 0 };
	ZnkHtpOnRecvFuncArg recv_fnca = { 0 };

	bool   is_proxy = false;
	size_t try_connect_num = 3;

	ZnkCookie_load( cookie, cookie_filename );

	fp = Znk_fopen( result_filename, "wb" );
	if( fp == NULL ){
		RanoLog_printf( "RanoHtpBoy : Cannot fopen(wb) result_filename [%s]\n", result_filename );
		goto FUNC_END;
	}
	recv_fnca.func_ = writeFP;
	recv_fnca.arg_  = fp;

	ZnkHtpHdrs_compose( &send_hdrs );
	ZnkHtpHdrs_compose( &recv_hdrs );

	if( !loadHdrs( &send_hdrs, send_hdr_filename ) ){
		RanoLog_printf( "RanoHtpBoy : Cannot open hdr file [%s]\n", send_hdr_filename );
		goto FUNC_END;
	}
	loadBody( send_body, body_img_filename );

	result = ZnkHtpRAR_sendAndRecv( cnct_hostname, cnct_port,
			&send_hdrs, send_body,
			&recv_hdrs, recv_fnca,
			cookie,
			try_connect_num, is_proxy, wk_bfr, rmsg );
	if( !result ){
		RanoLog_printf( "RanoHtpBoy : Cannot connect [%s:%u]\n", cnct_hostname, cnct_port );
		RanoLog_printf( "  rmsg=[[%s]]\n", ZnkStr_cstr(rmsg) );
		goto FUNC_END;
	}

	{
		ZnkBfr_clear( wk_bfr );
		ZnkHtpHdrs_extendToStream( send_hdrs.hdr1st_, send_hdrs.vars_, wk_bfr, false );
		ZnkBfr_push_bk( wk_bfr, '\0' );
		RanoLog_printf( "@@L send_hdrs\n" );
		RanoLog_printf( "%s", (char*)ZnkBfr_data(wk_bfr) );
		RanoLog_printf( "@@.\n" );
	}
	{
		ZnkBfr_clear( wk_bfr );
		ZnkHtpHdrs_extendToStream( recv_hdrs.hdr1st_, recv_hdrs.vars_, wk_bfr, false );
		ZnkStr_terminate_null( wk_bfr, true );
		RanoLog_printf( "@@L recv_hdrs\n" );
		RanoLog_printf( "%s", (char*)ZnkBfr_data(wk_bfr) );
		RanoLog_printf( "@@.\n" );
		if( recv_hdr_filename ){
			saveText( recv_hdr_filename, wk_bfr );
		}
	}

	ZnkCookie_save( cookie, cookie_filename );

FUNC_END:
	ZnkBfr_destroy( send_body );
	ZnkVarpAry_destroy( cookie );
	ZnkBfr_destroy( wk_bfr );
	Znk_fclose( fp );
	ZnkHtpHdrs_dispose( &send_hdrs );
	ZnkHtpHdrs_dispose( &recv_hdrs );
	ZnkStr_delete( rmsg );
	return result;
}

void
RanoHtpBoy_getResultFile( const char* hostname, const char* unesc_req_urp, ZnkStr result_filename,
		const char* tmpdir, const char* target )
{
	char result_dir[ 256 ] = "";
	ZnkStr req_urp_dir  = ZnkStr_new( "" );
	ZnkStr req_urp_tail = ZnkStr_new( "" );
	ZnkStr hostname_esc = ZnkStr_new( hostname );

	/***
	 * hostnameには不正な文字列が含まれている可能性もあり、それをsanitizeする必要がある.
	 * たとえばport指定がされているような場合、":" などが含まれている可能性がある.
	 * ":" はWindows上のファイルシステムでは不正な文字であるため、ファイルの生成に失敗する.
	 */
	{
		ZnkHtpURL_sanitizeReqUrpDir( hostname_esc, true );
	}

	ZnkHtpURL_getReqUrpDir( req_urp_dir, req_urp_tail, unesc_req_urp );
	{
		ZnkStr esc_filename = ZnkStr_new( "" ); 
		/***
		 * unesc_req_urpには不正な文字列が含まれている可能性もあり、それをsanitizeする必要がある.
		 * たとえばクエリーストリング有りの場合"?" 、複合リンクの場合"http://"などが含まれている可能性もある.
		 * また..が含まれているケースも防御する必要がある.
		 * req_urp_dirが空の場合もあり得る. (http://hostname/file.htmなどの場合)
		 */
		if( !ZnkStr_empty(req_urp_dir) ){
			ZnkHtpURL_sanitizeReqUrpDir( req_urp_dir, true );
		}

		Znk_snprintf( result_dir, sizeof(result_dir), "%s%s/%s%s", tmpdir, target, ZnkStr_cstr(hostname_esc), ZnkStr_cstr(req_urp_dir) );
		ZnkDir_mkdirPath( result_dir, Znk_NPOS, '/', NULL );

		/***
		 * tail以降は通常のURL Escapeをしておく.
		 * これはファイルがダウンロードされたときそのままファイル名となる.
		 * / や \ までEscapeされるため、仮に .. が含まれていたとしても、
		 * それが親ディレクトリとして機能することはないためこれは放置でよい.
		 * ただし tailが .. と完全に等しい場合は、やはり名前を index.html に矯正しておく.
		 * ( http://127.0.0.1/my_dir/..などといったURLが与えられた場合)
		 */
		ZnkHtpURL_escape( esc_filename, (uint8_t*)ZnkStr_cstr(req_urp_tail), ZnkStr_leng(req_urp_tail) );
		if( ZnkStr_empty(esc_filename) || ZnkStr_eq(esc_filename, "..") ){
			ZnkStr_set( esc_filename, "index.html" );
		}
		ZnkStr_setf( result_filename, "%s/%s", result_dir, ZnkStr_cstr(esc_filename) );
		ZnkStr_delete( esc_filename );

		/***
		 * result_filename filesystem check.
		 */
#if 1
		if(  ZnkDir_getType( ZnkStr_cstr(result_filename) ) != ZnkDirType_e_File
		  && ZnkDir_getType( ZnkStr_cstr(result_filename) ) != ZnkDirType_e_Directory )
		{
			ZnkFile fp = Znk_fopen( ZnkStr_cstr(result_filename), "wb" );
			if( fp == NULL ){
				ZnkStr_setf( result_filename, "%s/result.dat", result_dir );
				fp = Znk_fopen( ZnkStr_cstr(result_filename), "wb" );
				if( fp == NULL ){
					ZnkStr_setf( result_filename, "%s%s/%s/alternative_name_result.dat", tmpdir, target, ZnkStr_cstr(hostname_esc) );
				}
			}
			if( fp ){
				/* 元々存在しないキャッシュファイルなのだから消しておく必要がある.
				 * さもなければキャッシュ機構がこの空のファイルを参照してしまう. */
				Znk_fclose( fp );
				ZnkDir_deleteFile_byForce( ZnkStr_cstr(result_filename) );
			}
		}
		//RanoLog_printf( "RanoHtpBoy_getResultFile : result_filename=[%s]\n", ZnkStr_cstr(result_filename) );
#endif

	}
	ZnkStr_delete( req_urp_tail );
	ZnkStr_delete( req_urp_dir );
	ZnkStr_delete( hostname_esc );
}

void
RanoHtpBoy_readRecvHdrVar( const char* var_name, char* val_buf, size_t val_buf_size, const char* target, const char* tmpdir )
{
	char filename[ 1024 ] = "";
	ZnkFile fp = NULL;

	Znk_snprintf( filename, sizeof(filename), "%s%s/recv_hdr.dat", tmpdir, target );
	fp = Znk_fopen( filename, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		ZnkVarpAry vars = ZnkVarpAry_create( true );
		ZnkVarp varp;
		char ptn[ 256 ] = "";
		Znk_snprintf( ptn, sizeof(ptn), "%s:", var_name );
		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				break;
			}
			ZnkStr_chompNL( line );
			if( ZnkS_isCaseBegin( ZnkStr_cstr(line), ptn ) ){
				/* TODO : ディレクティブが複数あるケースに対応できていないのではないか ? */
				varp = ZnkHtpHdrs_regist_byLine( vars, ZnkStr_cstr(line), ZnkStr_leng(line) );
				ZnkS_copy( val_buf, val_buf_size, ZnkHtpHdrs_val_cstr(varp, 0), Znk_NPOS );
			}
		}
		Znk_fclose( fp );
		ZnkVarpAry_destroy( vars );
		ZnkStr_delete( line );
	}
}
void
RanoHtpBoy_readRecvHdrStatus( ZnkStr ans, size_t tkn_idx, const char* target, const char* tmpdir )
{
	char filename[ 1024 ] = "";
	ZnkFile fp = NULL;

	Znk_snprintf( filename, sizeof(filename), "%s%s/recv_hdr.dat", tmpdir, target );
	fp = Znk_fopen( filename, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		if( ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			ZnkStrAry hdr1st = ZnkStrAry_create( true );
			ZnkStr_chompNL( line );
			ZnkHtpHdrs_registHdr1st( hdr1st, ZnkStr_cstr(line), ZnkStr_leng(line) );
			ZnkStr_set( ans, ZnkStrAry_at_cstr( hdr1st, tkn_idx ) );
			ZnkStrAry_destroy( hdr1st );
		}
		Znk_fclose( fp );
		ZnkStr_delete( line );
	}
}

static bool
writeSendHdrDat( struct ZnkHtpHdrs_tag* htp_hdrs, const char* tmpdir, const char* target )
{
	ZnkFile fp = NULL;
	char http_hdr_file[ 256 ] = "";
	char result_dir[ 256 ] = "";

	Znk_snprintf( result_dir, sizeof(result_dir), "%s%s", tmpdir, target );
	ZnkDir_mkdirPath( result_dir, Znk_NPOS, '/', NULL );

	Znk_snprintf( http_hdr_file, sizeof(http_hdr_file), "%s/send_hdr.dat", result_dir );
	fp = Znk_fopen( http_hdr_file, "wb" );

	if( fp ){
		ZnkBfr stream_fnl = ZnkBfr_create_null();
		ZnkHtpHdrs_extendToStream( htp_hdrs->hdr1st_, htp_hdrs->vars_, stream_fnl, true );
		Znk_fwrite( ZnkBfr_data(stream_fnl), ZnkBfr_size(stream_fnl), fp );
		Znk_fclose( fp );
		return true;
	}
	RanoLog_printf( "RanoHtpBoy : writeSendHdrDat : Cannot open send_hdr file [%s]\n", http_hdr_file );
	return false;
}

static void
getCnctHostnameAndPort( ZnkStr cnct_hostname, uint16_t* cnct_port, const char* parent_cnct )
{
	const char* port_p = NULL;

	if( ZnkS_isBegin( parent_cnct, "http://" ) ){
		parent_cnct = parent_cnct + 7;
	} else if( ZnkS_isBegin( parent_cnct, "https://" ) ){
		parent_cnct = parent_cnct + 8;
	}

	port_p = strchr( parent_cnct, ':' );
	if( port_p ){
		const size_t hostname_leng = port_p - parent_cnct;
		ZnkStr_assign( cnct_hostname, 0, parent_cnct, hostname_leng );
		sscanf( port_p+1, "%hd", cnct_port );
	} else {
		ZnkStr_assign( cnct_hostname, 0, parent_cnct, Znk_NPOS );
		*cnct_port = 80;
	}
}

/***
 * post_varsにNULLを指定することにより、filenameの中身を空データでクリアすることもできる.
 */
static bool
saveSendBody( struct ZnkHtpHdrs_tag* htp_hdrs, ZnkVarpAry post_vars, const char* filename )
{
	bool result = false;
	ZnkBfr stream_fnl = NULL;

	if( post_vars ){
		const char* content_type = "";
		ZnkStr boundary = ZnkStr_new( "--" );
		stream_fnl = ZnkBfr_create_null();
		content_type = ZnkHtpHdrs_scanContentType( htp_hdrs->vars_, boundary );
		if( content_type == NULL ){
			/***
			 * Content-Type が存在しない.
			 * このようなPOSTはどう扱うべきか?
			 * 多くの場合、本来エラーとすべきと思われるが、
			 * ここではとりあえず application/x-www-form-urlencoded として取り扱う.
			 */
			content_type = "application/x-www-form-urlencoded";
		}
		if( ZnkS_isBegin_literal( content_type, "multipart/form-data" ) ){
			RanoPost_extendPostVarsToStream_forMPFD( post_vars, stream_fnl, ZnkStr_cstr(boundary) );
		} else if( ZnkS_isBegin_literal( content_type, "application/x-www-form-urlencoded" )
		//	   	|| ZnkS_isBegin_literal( content_type, "application/ocsp-request" )
		        || ZnkS_isBegin_literal( content_type, "text/xml" ) ){
			RanoPost_extendPostVarsToStream_forURLE( post_vars, stream_fnl );
		}
		ZnkStr_delete( boundary );
	}

	/* stream_fnl == NULL の場合はfilenameの中身を空データでクリアする処理となる */
	{
		ZnkFile fp = Znk_fopen( filename, "wb" );
		if( fp ){
			if( stream_fnl ){
				Znk_fwrite( ZnkBfr_data(stream_fnl), ZnkBfr_size(stream_fnl), fp );
			}
			Znk_fclose( fp );
		}
	}

	result = true;
	ZnkBfr_destroy( stream_fnl );
	return result;
}

/***
 * Note.
 *
 * GETメソッドではボディ部を付加すべきではない.
 *
 * たとえば、今回のGETの直前にmultipart/form-dataでPOSTしていた場合を考える.
 * このとき send_body.dat の中身は前回のボディデータが残っている.
 * これをそのままにして今回のGETリクエストにおいて、ボディとして付加して送って場合、
 * 本来存在しないはずの余分な送信ボディが存在することになり、その後のデータ送受信でエラーを発生させる場合がある.
 * よってここでは確実にボディ部付加を防止するよう、body_img_filenameにNULLを指定しなければならない.
 *
 * ではURL上でのクエリーストリング(?以降の部分)が付加された場合はどうなるのか?
 * それがGETで送られるならば、GETヘッダの一行目のreq_urpにそれがそのまま含まれる形となる.
 * つまりGETリクエストの場合は、たとえ送信すべきクエリーストリングが存在する場合でもボディ部が付加されることはない.
 * WebサーバによってはGETで尚そのようなボディデータを付加したものを解釈できるものも存在するかもしれないが
 * それを期待すべきではない.
 *
 * 尚、そのようなクエリーストリングがPOSTで送られる場合(formからなどやクエリーストリングがあまりに長い場合など)、
 * ボディデータとしてクエリーストリングが一行分付加されて送信される形となる.
 */
bool
RanoHtpBoy_do_get( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename )
{
	bool     result = false;
	uint16_t cnct_port = 80;
	ZnkStr cnct_hostname = ZnkStr_new( "" );

	char req_uri[ 1024 ] = "";
	char parent_cnct[ 4096 ] = "";

	if( ZnkS_empty( parent_proxy ) || ZnkS_eq( parent_proxy, "NONE" ) ){
		Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", hostname );
		Znk_snprintf( req_uri, sizeof(req_uri), "%s", unesc_req_urp );
	} else {
		/* by proxy */
		Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", parent_proxy );
		/* ここでのhttp://は必須である(ないとうまくいかないproxyもある) */
		Znk_snprintf( req_uri, sizeof(req_uri), "http://%s%s", hostname, unesc_req_urp );
	}

	{
		char buf[ 4096 ];
		Znk_snprintf( buf, sizeof(buf), "GET %s HTTP/1.1\r\n", req_uri );
		ZnkHtpHdrs_registHdr1st( htp_hdrs->hdr1st_, buf, Znk_strlen(buf) );
	}

	RanoHtpBoy_getResultFile( hostname, unesc_req_urp, result_filename, cachebox, target );
	writeSendHdrDat( htp_hdrs, tmpdir, target );

	getCnctHostnameAndPort( cnct_hostname, &cnct_port, parent_cnct );

	{
		char cookie_filename[ 256 ] = "";
		char send_hdr_filename[ 256 ] = "";
		char recv_hdr_filename[ 256 ] = "";
		char* body_img_filename = NULL;

		Znk_snprintf( cookie_filename,   sizeof(cookie_filename),   "%s%s/cookie.txt",    st_tmpdir_common, target );
		Znk_snprintf( send_hdr_filename, sizeof(send_hdr_filename), "%s%s/send_hdr.dat",  tmpdir, target );
		Znk_snprintf( recv_hdr_filename, sizeof(recv_hdr_filename), "%s%s/recv_hdr.dat",  tmpdir, target );

		result = RanoHtpBoy_process( ZnkStr_cstr(cnct_hostname), cnct_port,
					ZnkStr_cstr(result_filename), cookie_filename,
					send_hdr_filename, body_img_filename, recv_hdr_filename );
	}

	ZnkStr_delete( cnct_hostname );
	return result;
}
bool
RanoHtpBoy_do_get_with404( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename, bool is_without_404, int* ans_status_code )
{
	bool     result = false;
	uint16_t cnct_port = 80;
	ZnkStr   cnct_hostname = ZnkStr_new( "" );
	char     req_uri[ 1024 ] = "";
	char     parent_cnct[ 4096 ] = "";
	ZnkStr   pre_resultfile_name = is_without_404 ? ZnkStr_newf( "%s%s/pre_result.dat", tmpdir, target ) : NULL;
	ZnkStr   result_filename_ref = is_without_404 ? pre_resultfile_name : result_filename;

	if( ZnkS_empty( parent_proxy ) || ZnkS_eq( parent_proxy, "NONE" ) ){
		Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", hostname );
		Znk_snprintf( req_uri, sizeof(req_uri), "%s", unesc_req_urp );
	} else {
		/* by proxy */
		Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", parent_proxy );
		/* ここでのhttp://は必須である(ないとうまくいかないproxyもある) */
		Znk_snprintf( req_uri, sizeof(req_uri), "http://%s%s", hostname, unesc_req_urp );
	}

	{
		char buf[ 4096 ];
		Znk_snprintf( buf, sizeof(buf), "GET %s HTTP/1.1\r\n", req_uri );
		ZnkHtpHdrs_registHdr1st( htp_hdrs->hdr1st_, buf, Znk_strlen(buf) );
	}

	RanoHtpBoy_getResultFile( hostname, unesc_req_urp, result_filename, cachebox, target );
	writeSendHdrDat( htp_hdrs, tmpdir, target );

	getCnctHostnameAndPort( cnct_hostname, &cnct_port, parent_cnct );

	{
		char cookie_filename[ 256 ] = "";
		char send_hdr_filename[ 256 ] = "";
		char recv_hdr_filename[ 256 ] = "";
		char* body_img_filename = NULL;

		Znk_snprintf( cookie_filename,   sizeof(cookie_filename),   "%s%s/cookie.txt",    st_tmpdir_common, target );
		Znk_snprintf( send_hdr_filename, sizeof(send_hdr_filename), "%s%s/send_hdr.dat",  tmpdir, target );
		Znk_snprintf( recv_hdr_filename, sizeof(recv_hdr_filename), "%s%s/recv_hdr.dat",  tmpdir, target );

		result = RanoHtpBoy_process( ZnkStr_cstr(cnct_hostname), cnct_port,
					ZnkStr_cstr(result_filename_ref), cookie_filename,
					send_hdr_filename, body_img_filename, recv_hdr_filename );

		if( result && ( is_without_404 || ans_status_code ) ){
			int    status_code = 0;
			ZnkStr status      = ZnkStr_new( "" );
			RanoHtpBoy_readRecvHdrStatus( status, 1, target, tmpdir );
			if( ZnkS_getIntD( &status_code, ZnkStr_cstr(status) ) ){
				/* get ans_status_code */
				if( ans_status_code ){
					*ans_status_code = status_code;
				}
				/* without_404 case */
				if( is_without_404 && status_code != 404 ){
					/* 404でなかった場合に限り、result_filenameへ上書き移動する */
					if( ZnkDir_copyFile_byForce( ZnkStr_cstr(pre_resultfile_name), ZnkStr_cstr(result_filename), NULL ) ){
						ZnkDir_deleteFile_byForce( ZnkStr_cstr(pre_resultfile_name) );
					}
				}
			}
			ZnkStr_delete( status );
		}

	}

	ZnkStr_delete( cnct_hostname );
	ZnkStr_delete( pre_resultfile_name );
	return result;
}
bool
RanoHtpBoy_do_head( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir,
		ZnkStr result_filename )
{
	bool     result = false;
	uint16_t cnct_port = 80;
	ZnkStr cnct_hostname = ZnkStr_new( "" );

	char req_uri[ 1024 ] = "";
	char parent_cnct[ 4096 ] = "";

	if( ZnkS_empty( parent_proxy ) || ZnkS_eq( parent_proxy, "NONE" ) ){
		Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", hostname );
		Znk_snprintf( req_uri, sizeof(req_uri), "%s", unesc_req_urp );
	} else {
		/* by proxy */
		Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", parent_proxy );
		/* ここでのhttp://は必須である(ないとうまくいかないproxyもある) */
		Znk_snprintf( req_uri, sizeof(req_uri), "http://%s%s", hostname, unesc_req_urp );
	}

	{
		char buf[ 4096 ];
		Znk_snprintf( buf, sizeof(buf), "HEAD %s HTTP/1.1\r\n", req_uri );
		ZnkHtpHdrs_registHdr1st( htp_hdrs->hdr1st_, buf, Znk_strlen(buf) );
	}

	ZnkStr_setf( result_filename, "%s%s/head_result.dat", tmpdir, target );
	writeSendHdrDat( htp_hdrs, tmpdir, target );

	getCnctHostnameAndPort( cnct_hostname, &cnct_port, parent_cnct );

	{
		char cookie_filename[ 256 ] = "";
		char send_hdr_filename[ 256 ] = "";
		char recv_hdr_filename[ 256 ] = "";
		char* body_img_filename = NULL;

		Znk_snprintf( cookie_filename,   sizeof(cookie_filename),   "%s%s/cookie.txt",    st_tmpdir_common, target );
		Znk_snprintf( send_hdr_filename, sizeof(send_hdr_filename), "%s%s/send_hdr.dat",  tmpdir, target );
		Znk_snprintf( recv_hdr_filename, sizeof(recv_hdr_filename), "%s%s/recv_hdr.dat",  tmpdir, target );

		result = RanoHtpBoy_process( ZnkStr_cstr(cnct_hostname), cnct_port,
					ZnkStr_cstr(result_filename), cookie_filename,
					send_hdr_filename, body_img_filename, recv_hdr_filename );
	}

	ZnkStr_delete( cnct_hostname );
	return result;
}


Znk_INLINE void
appendBfr_byCStr( ZnkBfr bfr, const char* cstr )
{
	const size_t leng = strlen( cstr );
	ZnkBfr_append_dfr( bfr, (uint8_t*)cstr, leng );
}

bool
RanoHtpBoy_do_post( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs, ZnkVarpAry post_vars,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename )
{
	bool     result = false;
	uint16_t cnct_port = 80;
	ZnkStr cnct_hostname = ZnkStr_new( "" );

	char req_uri[ 1024 ] = "";
	char parent_cnct[ 4096 ] = "";

	if( ZnkS_empty( parent_proxy ) || ZnkS_eq( parent_proxy, "NONE" ) ){
		Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", hostname );
		Znk_snprintf( req_uri, sizeof(req_uri), "%s", unesc_req_urp );
	} else {
		/* by proxy */
		Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", parent_proxy );
		/* ここでのhttp://は必須である(ないとうまくいかないproxyもある) */
		Znk_snprintf( req_uri, sizeof(req_uri), "http://%s%s", hostname, unesc_req_urp );
	}

	{
		char buf[ 4096 ];
		Znk_snprintf( buf, sizeof(buf), "POST %s HTTP/1.1\r\n", req_uri );
		ZnkHtpHdrs_registHdr1st( htp_hdrs->hdr1st_, buf, Znk_strlen(buf) );
	}

	{
		char  body_fname[ 256 ] = "";
		char  result_dir[ 256 ] = "";

		Znk_snprintf( result_dir, sizeof(result_dir), "%s%s", tmpdir, target );
		ZnkDir_mkdirPath( result_dir, Znk_NPOS, '/', NULL );

		Znk_snprintf( body_fname, sizeof(body_fname), "%s/send_body.dat", result_dir );
		saveSendBody( htp_hdrs, post_vars, body_fname );
	}

	RanoHtpBoy_getResultFile( hostname, unesc_req_urp, result_filename, cachebox, target );
	writeSendHdrDat( htp_hdrs, tmpdir, target );

	getCnctHostnameAndPort( cnct_hostname, &cnct_port, parent_cnct );

	{
		char cookie_filename[ 256 ] = "";
		char send_hdr_filename[ 256 ] = "";
		char body_img_filename[ 256 ] = "";
		char recv_hdr_filename[ 256 ] = "";

		Znk_snprintf( cookie_filename,   sizeof(cookie_filename),   "%s%s/cookie.txt",    st_tmpdir_common, target );
		Znk_snprintf( send_hdr_filename, sizeof(send_hdr_filename), "%s%s/send_hdr.dat",  tmpdir, target );
		Znk_snprintf( body_img_filename, sizeof(body_img_filename), "%s%s/send_body.dat", tmpdir, target );
		Znk_snprintf( recv_hdr_filename, sizeof(recv_hdr_filename), "%s%s/recv_hdr.dat",  tmpdir, target );

		result = RanoHtpBoy_process( ZnkStr_cstr(cnct_hostname), cnct_port,
					ZnkStr_cstr(result_filename), cookie_filename,
					send_hdr_filename, body_img_filename, recv_hdr_filename );
	}

	ZnkStr_delete( cnct_hostname );

	return result;
}

void
RanoHtpBoy_seekExeDir( ZnkStr exedir, size_t depth, char dsp, const char* target_filename )
{
	char path_target[ 256 ]  = "";
	ZnkStr_clear( exedir );
	while( depth ){
		Znk_snprintf( path_target, sizeof(path_target), "%s%s", ZnkStr_cstr(exedir), target_filename );
		if( ZnkDir_getType( path_target ) == ZnkDirType_e_File ){
			break;
		}
		ZnkStr_add( exedir, "../" );
		--depth;
	}
	ZnkStrPath_replaceDSP( exedir, dsp );
}

