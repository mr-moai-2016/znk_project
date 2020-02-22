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
#include <Znk_str_ex.h>
#include <Znk_str_fio.h>
#include <Znk_str_path.h>
#include <Znk_cookie.h>
#include <Znk_missing_libc.h>
#include <Znk_dir.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tls_module/tls_module.h>

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
const char*
RanoHtpBoy_getTmpDirCommon( void )
{
	return st_tmpdir_common;
}

/***
 * User-Agent, Referer, Cookie�Ȃǂ�HTTP�w�b�_�𑕏�����.
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
	ZnkFile fp = Znk_force_ptr_cast( ZnkFile, arg );
	const size_t result_size = Znk_fwrite( buf, buf_size, fp );
	return (int)result_size;
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
RanoHtpBoy_initiateHttpsModule( const char* lib_basename, const char* cert_pem )
{
	return TlsModule_initiate( lib_basename, cert_pem );
}

static void
addHdrStream( ZnkStr ans, const char* hdr_stream, const char* line_prefix )
{
	ZnkStrAry lines = ZnkStrAry_create( true );
	size_t    size = 0;
	size_t    idx  = 0;
	ZnkStrEx_addSplitCSet( lines, hdr_stream, Znk_NPOS,
			"\r\n", 2,
			8 );
	size = ZnkStrAry_size( lines );
	for( idx=0; idx<size; ++idx ){
		ZnkStr_addf( ans, "%s%s\n", line_prefix, ZnkStrAry_at_cstr( lines, idx ) );
	}
	ZnkStrAry_destroy( lines );
}

bool
RanoHtpBoy_processEx( const char* cnct_hostname, uint16_t cnct_port,
		const char* result_filename, const char* cookie_filename,
		const char* send_hdr_filename, const char* body_img_filename,
		const char* recv_hdr_filename, bool is_https,
		ZnkHtpOnRecvFuncArg* prog_fnca, ZnkStr ermsg )
{
	bool result = false;
	ZnkBfr     send_body = ZnkBfr_create_null();
	ZnkVarpAry cookie    = ZnkVarpAry_create(true);
	ZnkBfr     wk_bfr    = ZnkBfr_create_null();
	ZnkFile    fp        = NULL;

	struct ZnkHtpHdrs_tag send_hdrs = { 0 };
	struct ZnkHtpHdrs_tag recv_hdrs = { 0 };
	ZnkHtpOnRecvFuncArg recv_fnca = { 0 };
	ZnkHtpOnRecvFuncArg prog_fnca_internal = { 0 };
	bool   is_proxy = false;

	ZnkCookie_load( cookie, cookie_filename );

	fp = Znk_fopen( result_filename, "wb" );
	if( fp == NULL ){
		if( ermsg ){
			ZnkStr_addf( ermsg, "RanoHtpBoy : Cannot fopen(wb) result_filename [%s]\n", result_filename );
		}
		goto FUNC_END;
	}

	if( prog_fnca ){
		prog_fnca_internal = *prog_fnca;
	}

	recv_fnca.func_ = writeFP;
	recv_fnca.arg_  = fp;

	ZnkHtpHdrs_compose( &send_hdrs );
	ZnkHtpHdrs_compose( &recv_hdrs );

	if( !loadHdrs( &send_hdrs, send_hdr_filename ) ){
		if( ermsg ){
			ZnkStr_addf( ermsg, "RanoHtpBoy : Cannot open hdr file [%s]\n", send_hdr_filename );
		}
		goto FUNC_END;
	}
	loadBody( send_body, body_img_filename );

	if( is_https ){
		if( ermsg ){
			ZnkStr_addf( ermsg, "RanoHtpBoy : HTTPS.\n" );
		}
		result = TlsModule_getHtpsProcess( cnct_hostname, cnct_port,
					&send_hdrs, send_body,
					&recv_hdrs, recv_fnca, prog_fnca_internal,
					cookie,
					is_proxy, wk_bfr, ermsg );
		if( ermsg ){
			ZnkStr_addf( ermsg, "\n" );
		}
	} else {
		size_t try_connect_num = 3;
		if( ermsg ){
			ZnkStr_addf( ermsg, "RanoHtpBoy : HTTP.\n" );
		}
		result = ZnkHtpRAR_sendAndRecv( cnct_hostname, cnct_port,
				&send_hdrs, send_body,
				&recv_hdrs, recv_fnca, prog_fnca_internal,
				cookie,
				try_connect_num, is_proxy, wk_bfr, ermsg );
	}

	if( !result ){
		if( ermsg ){
			ZnkStr_addf( ermsg, "RanoHtpBoy : Cannot connect [%s:%u]\n", cnct_hostname, cnct_port );
		}
		goto FUNC_END;
	}

	{
		ZnkBfr_clear( wk_bfr );
		ZnkHtpHdrs_extendToStream( send_hdrs.hdr1st_, send_hdrs.vars_, wk_bfr, false );
		ZnkBfr_push_bk( wk_bfr, '\0' );
		if( ermsg ){
			ZnkStr_addf( ermsg, "  @@L send_hdrs\n" );
			addHdrStream( ermsg, (char*)ZnkBfr_data(wk_bfr), "  " );
			//ZnkStr_addf( ermsg, "%s", (char*)ZnkBfr_data(wk_bfr) );
			ZnkStr_addf( ermsg, "  @@.\n" );
		}

		ZnkBfr_clear( wk_bfr );
		ZnkHtpHdrs_extendToStream( recv_hdrs.hdr1st_, recv_hdrs.vars_, wk_bfr, false );
		ZnkStr_terminate_null( wk_bfr, true );
		if( ermsg ){
			ZnkStr_addf( ermsg, "  @@L recv_hdrs\n" );
			addHdrStream( ermsg, (char*)ZnkBfr_data(wk_bfr), "  " );
			//ZnkStr_addf( ermsg, "%s", (char*)ZnkBfr_data(wk_bfr) );
			ZnkStr_addf( ermsg, "  @@.\n" );
		}

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
	return result;
}

bool
RanoHtpBoy_process( const char* cnct_hostname, uint16_t cnct_port,
		const char* result_filename, const char* cookie_filename,
		const char* send_hdr_filename, const char* body_img_filename,
		const char* recv_hdr_filename )
{
	static const bool is_https = false;
	return RanoHtpBoy_processEx( cnct_hostname, cnct_port,
			result_filename, cookie_filename,
			send_hdr_filename, body_img_filename,
			recv_hdr_filename, is_https,
			NULL, NULL );
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
	 * hostname�ɂ͕s���ȕ����񂪊܂܂�Ă���\��������A�����sanitize����K�v������.
	 * ���Ƃ���port�w�肪����Ă���悤�ȏꍇ�A":" �Ȃǂ��܂܂�Ă���\��������.
	 * ":" ��Windows��̃t�@�C���V�X�e���ł͕s���ȕ����ł��邽�߁A�t�@�C���̐����Ɏ��s����.
	 */
	{
		ZnkHtpURL_sanitizeReqUrpDir( hostname_esc, true );
	}

	ZnkHtpURL_getReqUrpDir( req_urp_dir, req_urp_tail, unesc_req_urp );
	{
		ZnkStr esc_filename = ZnkStr_new( "" ); 
		/***
		 * unesc_req_urp�ɂ͕s���ȕ����񂪊܂܂�Ă���\��������A�����sanitize����K�v������.
		 * ���Ƃ��΃N�G���[�X�g�����O�L��̏ꍇ"?" �A���������N�̏ꍇ"http://"�Ȃǂ��܂܂�Ă���\��������.
		 * �܂�..���܂܂�Ă���P�[�X���h�䂷��K�v������.
		 * req_urp_dir����̏ꍇ�����蓾��. (http://hostname/file.htm�Ȃǂ̏ꍇ)
		 */
		if( !ZnkStr_empty(req_urp_dir) ){
			ZnkHtpURL_sanitizeReqUrpDir( req_urp_dir, true );
		}

		Znk_snprintf( result_dir, sizeof(result_dir), "%s%s/%s%s", tmpdir, target, ZnkStr_cstr(hostname_esc), ZnkStr_cstr(req_urp_dir) );
		ZnkDir_mkdirPath( result_dir, Znk_NPOS, '/', NULL );

		/***
		 * tail�ȍ~�͒ʏ��URL Escape�����Ă���.
		 * ����̓t�@�C�����_�E�����[�h���ꂽ�Ƃ����̂܂܃t�@�C�����ƂȂ�.
		 * / �� \ �܂�Escape����邽�߁A���� .. ���܂܂�Ă����Ƃ��Ă��A
		 * ���ꂪ�e�f�B���N�g���Ƃ��ċ@�\���邱�Ƃ͂Ȃ����߂���͕��u�ł悢.
		 * ������ tail�� .. �Ɗ��S�ɓ������ꍇ�́A��͂薼�O�� index.html �ɋ������Ă���.
		 * ( http://127.0.0.1/my_dir/..�ȂǂƂ�����URL���^����ꂽ�ꍇ)
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
				/* ���X���݂��Ȃ��L���b�V���t�@�C���Ȃ̂���������Ă����K�v������.
				 * �����Ȃ���΃L���b�V���@�\�����̋�̃t�@�C�����Q�Ƃ��Ă��܂�. */
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
				/* TODO : �f�B���N�e�B�u����������P�[�X�ɑΉ��ł��Ă��Ȃ��̂ł͂Ȃ��� ? */
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
writeSendHdrDat( struct ZnkHtpHdrs_tag* htp_hdrs, const char* tmpdir, const char* target, ZnkStr ermsg )
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
	if( ermsg ){
		ZnkStr_addf( ermsg, "RanoHtpBoy : writeSendHdrDat : Cannot open send_hdr file [%s]\n", http_hdr_file );
	}
	return false;
}

static void
getCnctHostnameAndPort( ZnkStr cnct_hostname, uint16_t* cnct_port, const char* parent_cnct, bool is_https )
{
	const char* port_p = NULL;

	/***
	 * parent_cnct �ɂ͒ʏ�v���g�R���X�L�[���͕t������Ă��Ȃ��͂��ŁA
	 * �ȉ��͔O�̂��߂̏����ł���.
	 * ����Ă����ł�is_https�ł��邩�ۂ�����ʂɔ���ł��Ȃ�.
	 */
	//RanoLog_printf( "RanoHtpBoy : Report : parent_cnct=[%s]\n", parent_cnct );
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
		if( is_https ){
			*cnct_port = 443;
		} else {
			*cnct_port = 80;
		}
	}
}

/***
 * post_vars��NULL���w�肷�邱�Ƃɂ��Afilename�̒��g����f�[�^�ŃN���A���邱�Ƃ��ł���.
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
			 * Content-Type �����݂��Ȃ�.
			 * ���̂悤��POST�͂ǂ������ׂ���?
			 * �����̏ꍇ�A�{���G���[�Ƃ��ׂ��Ǝv���邪�A
			 * �����ł͂Ƃ肠���� application/x-www-form-urlencoded �Ƃ��Ď�舵��.
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

	/* stream_fnl == NULL �̏ꍇ��filename�̒��g����f�[�^�ŃN���A���鏈���ƂȂ� */
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
 * GET���\�b�h�ł̓{�f�B����t�����ׂ��ł͂Ȃ�.
 *
 * ���Ƃ��΁A�����GET�̒��O��multipart/form-data��POST���Ă����ꍇ���l����.
 * ���̂Ƃ� send_body.dat �̒��g�͑O��̃{�f�B�f�[�^���c���Ă���.
 * ��������̂܂܂ɂ��č����GET���N�G�X�g�ɂ����āA�{�f�B�Ƃ��ĕt�����đ����ďꍇ�A
 * �{�����݂��Ȃ��͂��̗]���ȑ��M�{�f�B�����݂��邱�ƂɂȂ�A���̌�̃f�[�^����M�ŃG���[�𔭐�������ꍇ������.
 * ����Ă����ł͊m���Ƀ{�f�B���t����h�~����悤�Abody_img_filename��NULL���w�肵�Ȃ���΂Ȃ�Ȃ�.
 *
 * �ł�URL��ł̃N�G���[�X�g�����O(?�ȍ~�̕���)���t�����ꂽ�ꍇ�͂ǂ��Ȃ�̂�?
 * ���ꂪGET�ő�����Ȃ�΁AGET�w�b�_�̈�s�ڂ�req_urp�ɂ��ꂪ���̂܂܊܂܂��`�ƂȂ�.
 * �܂�GET���N�G�X�g�̏ꍇ�́A���Ƃ����M���ׂ��N�G���[�X�g�����O�����݂���ꍇ�ł��{�f�B�����t������邱�Ƃ͂Ȃ�.
 * Web�T�[�o�ɂ���Ă�GET�ŏ����̂悤�ȃ{�f�B�f�[�^��t���������̂����߂ł�����̂����݂��邩������Ȃ���
 * ��������҂��ׂ��ł͂Ȃ�.
 *
 * ���A���̂悤�ȃN�G���[�X�g�����O��POST�ő�����ꍇ(form����Ȃǂ�N�G���[�X�g�����O�����܂�ɒ����ꍇ�Ȃ�)�A
 * �{�f�B�f�[�^�Ƃ��ăN�G���[�X�g�����O����s���t������đ��M�����`�ƂȂ�.
 */
bool
RanoHtpBoy_cipher_get( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename, bool is_https, ZnkStr ermsg )
{
	bool     result = false;
	uint16_t cnct_port = 80;
	ZnkStr   cnct_hostname = ZnkStr_new( "" );
	char     req_uri[ 1024 ] = "";
	char     parent_cnct[ 4096 ] = "";

	if( ZnkS_empty( parent_proxy ) || ZnkS_eq( parent_proxy, "NONE" ) ){
		Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", hostname );
		Znk_snprintf( req_uri, sizeof(req_uri), "%s", unesc_req_urp );
	} else {
		/* by proxy */
		Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", parent_proxy );
		/* �����ł�http://�͕K�{�ł���(�Ȃ��Ƃ��܂������Ȃ�proxy������) */
		Znk_snprintf( req_uri, sizeof(req_uri), "http://%s%s", hostname, unesc_req_urp );
	}

	{
		char buf[ 4096 ];
		Znk_snprintf( buf, sizeof(buf), "GET %s HTTP/1.1\r\n", req_uri );
		ZnkHtpHdrs_registHdr1st( htp_hdrs->hdr1st_, buf, Znk_strlen(buf) );
	}

	RanoHtpBoy_getResultFile( hostname, unesc_req_urp, result_filename, cachebox, target );
	writeSendHdrDat( htp_hdrs, tmpdir, target, ermsg );

	getCnctHostnameAndPort( cnct_hostname, &cnct_port, parent_cnct, is_https );

	{
		char cookie_filename[ 256 ] = "";
		char send_hdr_filename[ 256 ] = "";
		char recv_hdr_filename[ 256 ] = "";
		char* body_img_filename = NULL;

		Znk_snprintf( cookie_filename,   sizeof(cookie_filename),   "%s%s/cookie.txt",    st_tmpdir_common, target );
		Znk_snprintf( send_hdr_filename, sizeof(send_hdr_filename), "%s%s/send_hdr.dat",  tmpdir, target );
		Znk_snprintf( recv_hdr_filename, sizeof(recv_hdr_filename), "%s%s/recv_hdr.dat",  tmpdir, target );

		result = RanoHtpBoy_processEx( ZnkStr_cstr(cnct_hostname), cnct_port,
					ZnkStr_cstr(result_filename), cookie_filename,
					send_hdr_filename, body_img_filename, recv_hdr_filename, is_https,
					NULL, ermsg );
	}

	ZnkStr_delete( cnct_hostname );
	return result;
}
bool
RanoHtpBoy_cipher_get_with404( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename, bool is_without_404, int* ans_status_code, bool is_https,
		ZnkHtpOnRecvFuncArg* prog_fnca, ZnkStr ermsg )
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
		/* �����ł�http://�͕K�{�ł���(�Ȃ��Ƃ��܂������Ȃ�proxy������) */
		Znk_snprintf( req_uri, sizeof(req_uri), "http://%s%s", hostname, unesc_req_urp );
	}

	{
		char buf[ 4096 ];
		Znk_snprintf( buf, sizeof(buf), "GET %s HTTP/1.1\r\n", req_uri );
		ZnkHtpHdrs_registHdr1st( htp_hdrs->hdr1st_, buf, Znk_strlen(buf) );
	}

	RanoHtpBoy_getResultFile( hostname, unesc_req_urp, result_filename, cachebox, target );
	writeSendHdrDat( htp_hdrs, tmpdir, target, ermsg );

	getCnctHostnameAndPort( cnct_hostname, &cnct_port, parent_cnct, is_https );

	{
		char cookie_filename[ 256 ] = "";
		char send_hdr_filename[ 256 ] = "";
		char recv_hdr_filename[ 256 ] = "";
		char* body_img_filename = NULL;

		Znk_snprintf( cookie_filename,   sizeof(cookie_filename),   "%s%s/cookie.txt",    st_tmpdir_common, target );
		Znk_snprintf( send_hdr_filename, sizeof(send_hdr_filename), "%s%s/send_hdr.dat",  tmpdir, target );
		Znk_snprintf( recv_hdr_filename, sizeof(recv_hdr_filename), "%s%s/recv_hdr.dat",  tmpdir, target );

		result = RanoHtpBoy_processEx( ZnkStr_cstr(cnct_hostname), cnct_port,
					ZnkStr_cstr(result_filename_ref), cookie_filename,
					send_hdr_filename, body_img_filename, recv_hdr_filename, is_https,
					prog_fnca, ermsg );

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
					/* 404�łȂ������ꍇ�Ɍ���Aresult_filename�֏㏑���ړ����� */
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
RanoHtpBoy_cipher_head( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir,
		ZnkStr result_filename, bool is_https, ZnkStr ermsg )
{
	bool     result = false;
	uint16_t cnct_port = 80;
	ZnkStr   cnct_hostname = ZnkStr_new( "" );
	char     req_uri[ 1024 ] = "";
	char     parent_cnct[ 4096 ] = "";

	if( ZnkS_empty( parent_proxy ) || ZnkS_eq( parent_proxy, "NONE" ) ){
		Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", hostname );
		Znk_snprintf( req_uri, sizeof(req_uri), "%s", unesc_req_urp );
	} else {
		/* by proxy */
		Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", parent_proxy );
		/* �����ł�http://�͕K�{�ł���(�Ȃ��Ƃ��܂������Ȃ�proxy������) */
		Znk_snprintf( req_uri, sizeof(req_uri), "http://%s%s", hostname, unesc_req_urp );
	}

	{
		char buf[ 4096 ];
		Znk_snprintf( buf, sizeof(buf), "HEAD %s HTTP/1.1\r\n", req_uri );
		ZnkHtpHdrs_registHdr1st( htp_hdrs->hdr1st_, buf, Znk_strlen(buf) );
	}

	ZnkStr_setf( result_filename, "%s%s/head_result.dat", tmpdir, target );
	writeSendHdrDat( htp_hdrs, tmpdir, target, ermsg );

	getCnctHostnameAndPort( cnct_hostname, &cnct_port, parent_cnct, is_https );

	{
		char cookie_filename[ 256 ] = "";
		char send_hdr_filename[ 256 ] = "";
		char recv_hdr_filename[ 256 ] = "";
		char* body_img_filename = NULL;

		Znk_snprintf( cookie_filename,   sizeof(cookie_filename),   "%s%s/cookie.txt",    st_tmpdir_common, target );
		Znk_snprintf( send_hdr_filename, sizeof(send_hdr_filename), "%s%s/send_hdr.dat",  tmpdir, target );
		Znk_snprintf( recv_hdr_filename, sizeof(recv_hdr_filename), "%s%s/recv_hdr.dat",  tmpdir, target );

		result = RanoHtpBoy_processEx( ZnkStr_cstr(cnct_hostname), cnct_port,
					ZnkStr_cstr(result_filename), cookie_filename,
					send_hdr_filename, body_img_filename, recv_hdr_filename, is_https,
					NULL, ermsg );
	}

	ZnkStr_delete( cnct_hostname );
	return result;
}

#if 0
Znk_INLINE void
appendBfr_byCStr( ZnkBfr bfr, const char* cstr )
{
	const size_t leng = strlen( cstr );
	ZnkBfr_append_dfr( bfr, (uint8_t*)cstr, leng );
}
#endif

bool
RanoHtpBoy_cipher_post( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs, ZnkVarpAry post_vars,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename, bool is_https, ZnkStr ermsg )
{
	bool     result = false;
	uint16_t cnct_port = 80;
	ZnkStr   cnct_hostname = ZnkStr_new( "" );
	char     req_uri[ 1024 ] = "";
	char     parent_cnct[ 4096 ] = "";

	if( ZnkS_empty( parent_proxy ) || ZnkS_eq( parent_proxy, "NONE" ) ){
		Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", hostname );
		Znk_snprintf( req_uri, sizeof(req_uri), "%s", unesc_req_urp );
	} else {
		/* by proxy */
		Znk_snprintf( parent_cnct, sizeof(parent_cnct), "%s", parent_proxy );
		/* �����ł�http://�͕K�{�ł���(�Ȃ��Ƃ��܂������Ȃ�proxy������) */
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
	writeSendHdrDat( htp_hdrs, tmpdir, target, ermsg );

	getCnctHostnameAndPort( cnct_hostname, &cnct_port, parent_cnct, is_https );

	{
		char cookie_filename[ 256 ] = "";
		char send_hdr_filename[ 256 ] = "";
		char body_img_filename[ 256 ] = "";
		char recv_hdr_filename[ 256 ] = "";

		Znk_snprintf( cookie_filename,   sizeof(cookie_filename),   "%s%s/cookie.txt",    st_tmpdir_common, target );
		Znk_snprintf( send_hdr_filename, sizeof(send_hdr_filename), "%s%s/send_hdr.dat",  tmpdir, target );
		Znk_snprintf( body_img_filename, sizeof(body_img_filename), "%s%s/send_body.dat", tmpdir, target );
		Znk_snprintf( recv_hdr_filename, sizeof(recv_hdr_filename), "%s%s/recv_hdr.dat",  tmpdir, target );

		result = RanoHtpBoy_processEx( ZnkStr_cstr(cnct_hostname), cnct_port,
					ZnkStr_cstr(result_filename), cookie_filename,
					send_hdr_filename, body_img_filename, recv_hdr_filename, is_https,
					NULL, ermsg );
	}

	ZnkStr_delete( cnct_hostname );

	return result;
}

bool
RanoHtpBoy_do_get( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename )
{
	static const bool is_https = false;
	return RanoHtpBoy_cipher_get( hostname, unesc_req_urp, target,
			htp_hdrs,
			parent_proxy,
			tmpdir, cachebox,
			result_filename, is_https, NULL );
}
bool
RanoHtpBoy_do_get_with404( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename, bool is_without_404, int* ans_status_code,
		ZnkHtpOnRecvFuncArg* prog_fnca )
{
	static const bool is_https = false;
	return RanoHtpBoy_cipher_get_with404( hostname, unesc_req_urp, target,
			htp_hdrs,
			parent_proxy,
			tmpdir, cachebox,
			result_filename, is_without_404, ans_status_code, is_https,
			prog_fnca, NULL );
}
bool
RanoHtpBoy_do_head( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir,
		ZnkStr result_filename )
{
	static const bool is_https = false;
	return RanoHtpBoy_cipher_head( hostname, unesc_req_urp, target,
			htp_hdrs,
			parent_proxy,
			tmpdir,
			result_filename, is_https, NULL );
}
bool
RanoHtpBoy_do_post( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs, ZnkVarpAry post_vars,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename )
{
	static const bool is_https = false;
	return RanoHtpBoy_cipher_post( hostname, unesc_req_urp, target,
			htp_hdrs, post_vars,
			parent_proxy,
			tmpdir, cachebox,
			result_filename, is_https, NULL );
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

