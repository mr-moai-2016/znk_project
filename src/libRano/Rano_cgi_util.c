#include <Rano_cgi_util.h>
#include <Rano_type.h>
#include <Rano_log.h>
#include <Rano_post.h>

#include <Znk_str.h>
#include <Znk_str_ex.h>
#include <Znk_str_fio.h>
#include <Znk_s_base.h>
#include <Znk_stdc.h>
#include <Znk_htp_util.h>
#include <Znk_htp_post.h>
#include <Znk_missing_libc.h>
#include <Znk_envvar.h>
#include <Znk_dir.h>

#include <stdio.h>
#include <string.h>


void /* XSS-safe */
RanoCGIUtil_addHdrVarsStr( ZnkStr str, const ZnkVarpAry vars )
{
	const size_t size = ZnkVarpAry_size( vars );
	size_t idx;
	ZnkVarp var;
	const char* key;
	size_t val_size;
	size_t val_idx;
	ZnkStr tmp = ZnkStr_new( "" );
	for( idx=0; idx<size; ++idx ){
		var = ZnkVarpAry_at( vars, idx );
		key = ZnkHtpHdrs_key_cstr( var );
		val_size = ZnkHtpHdrs_val_size( var );
		for( val_idx=0; val_idx<val_size; ++val_idx ){
			ZnkStr_addf( tmp, "  %s: %s\n", key, ZnkHtpHdrs_val_cstr( var, val_idx ) );
		}
	}
	ZnkHtpURL_negateHtmlTagEffection( tmp ); /* for XSS */
	ZnkStr_add( str, ZnkStr_cstr(tmp) );

	ZnkStr_delete( tmp );
}
void /* XSS-safe */
RanoCGIUtil_addPostVarsStr( ZnkStr str, const ZnkVarpAry hpvs )
{
	const size_t size = ZnkVarpAry_size( hpvs );
	size_t idx;
	ZnkVarp varp;
	ZnkStr tmp = ZnkStr_new( "" );

	ZnkStr_addf( str, "  PostVars ID=[%p]\n\n", hpvs );
	for( idx=0; idx<size; ++idx ){
		varp = ZnkVarpAry_at( hpvs, idx );
		if( varp->type_ == ZnkHtpPostVar_e_BinaryData ){
			ZnkStr_addf( tmp, "  name=[%s] filename=[%s] data byte size=[%zu]\n",
					ZnkStr_cstr(varp->name_), ZnkStr_cstr(varp->misc_),
					ZnkBfr_size(varp->prim_.u_.bfr_) );
		} else {
			ZnkStr_addf( tmp, "  name=[%s] val=[%s]\n",
					ZnkStr_cstr(varp->name_), ZnkStr_cstr(varp->prim_.u_.str_) );
		}
	}

	ZnkHtpURL_negateHtmlTagEffection( tmp ); /* for XSS */
	ZnkStr_add( str, ZnkStr_cstr(tmp) );
	ZnkStr_delete( tmp );
}

void
RanoCGIUtil_getStdInStr( ZnkBfr stdin_bfr, size_t content_length )
{
	int flag = 0;
	int chr  = 0;
	size_t count = 0;

	while( flag == 0 ){
		/* 渡されたデータを標準入力より取得する */
		chr = Znk_fgetc( Znk_stdin() );

		/* データ終了のチェック */
		if( chr == EOF || count == content_length ){
			flag = 1;
		} else {
			++count;
			ZnkBfr_push_bk( stdin_bfr, (uint8_t)chr );
		}
	}
}

RanoCGIEVar*
RanoCGIEVar_create( void )
{
	RanoCGIEVar* evar = Znk_malloc( sizeof( struct RanoCGIEVar_tag ) );
	evar->server_name_    = ZnkEnvVar_get( "SERVER_NAME" );
	evar->server_port_    = ZnkEnvVar_get( "SERVER_PORT" );
	evar->content_type_   = ZnkEnvVar_get( "CONTENT_TYPE" );
	evar->content_length_ = ZnkEnvVar_get( "CONTENT_LENGTH" );
	evar->remote_addr_    = ZnkEnvVar_get( "REMOTE_ADDR" );
	evar->remote_host_    = ZnkEnvVar_get( "REMOTE_HOST" );
	evar->remote_port_    = ZnkEnvVar_get( "REMOTE_PORT" );
	evar->request_method_ = ZnkEnvVar_get( "REQUEST_METHOD" );
	evar->query_string_   = ZnkEnvVar_get( "QUERY_STRING" );
	evar->http_cookie_    = ZnkEnvVar_get( "HTTP_COOKIE" );
	evar->http_user_agent_= ZnkEnvVar_get( "HTTP_USER_AGENT" );
	evar->http_accept_    = ZnkEnvVar_get( "HTTP_ACCEPT" );
	return evar;
}
void
RanoCGIEVar_destroy( RanoCGIEVar* evar )
{
	if( evar ){
		ZnkEnvVar_free( evar->server_name_ );
		ZnkEnvVar_free( evar->server_port_ );
		ZnkEnvVar_free( evar->content_type_ );
		ZnkEnvVar_free( evar->content_length_ );
		ZnkEnvVar_free( evar->remote_addr_ );
		ZnkEnvVar_free( evar->remote_host_ );
		ZnkEnvVar_free( evar->remote_port_ );
		ZnkEnvVar_free( evar->request_method_ );
		ZnkEnvVar_free( evar->query_string_ );
		ZnkEnvVar_free( evar->http_cookie_ );
		ZnkEnvVar_free( evar->http_user_agent_ );
		ZnkEnvVar_free( evar->http_accept_ );
		Znk_free( evar );
	}
}

void
RanoCGIUtil_splitQueryString( ZnkVarpAry post_vars, const char* query_string, const size_t query_string_leng, bool is_unescape_val )
{
	ZnkStrAry stmts = ZnkStrAry_create( true );
	size_t idx;
	size_t stmt_list_size;
	ZnkStr stmt;
	ZnkStr key = ZnkStr_new("");
	ZnkStr val = ZnkStr_new("");
	ZnkStr key_unesc = ZnkStr_new("");
	ZnkVarp varp;

	/***
	 * &に関してsplitする.
	 */
	ZnkStrEx_addSplitC( stmts,
			query_string, query_string_leng,
			'&', false, 8 );
	/***
	 * URL unescapeしたものをpost_varsに登録.
	 */
	stmt_list_size = ZnkStrAry_size( stmts );
	for( idx=0; idx<stmt_list_size; ++idx ){
		stmt = ZnkStrAry_at( stmts, idx );
		ZnkStr_clear( key );
		ZnkStr_clear( val );
		ZnkStrEx_getKeyAndVal( ZnkStr_cstr(stmt), 0, ZnkStr_leng(stmt),
				"=", "",
				key, val );
		ZnkStr_clear( key_unesc );
		ZnkHtpURL_unescape_toStr( key_unesc, ZnkStr_cstr(key), ZnkStr_leng(key) );

		/* 多重登録防止 */
		if( ZnkVarpAry_findIdx_byName( post_vars, ZnkStr_cstr(key_unesc), ZnkStr_leng(key_unesc), false ) == Znk_NPOS ){
			varp = ZnkVarp_create( ZnkStr_cstr(key_unesc), "", ZnkHtpPostVar_e_None, ZnkPrim_e_Str, NULL );
			if( is_unescape_val ){
				ZnkHtpURL_unescape_toStr( ZnkVar_str(varp), ZnkStr_cstr(val), ZnkStr_leng(val) );
			} else {
				ZnkStr_set( ZnkVar_str(varp), ZnkStr_cstr(val) );
			}
			/* この varp の寿命は post_varsの管理下に置かれる */
			ZnkVarpAry_push_bk( post_vars, varp );
		}
	}
	ZnkStr_delete( key );
	ZnkStr_delete( val );
	ZnkStr_delete( key_unesc );
	ZnkStrAry_destroy( stmts );
}

static bool
updatePostVars( ZnkVarpAry post_vars,
		RanoCGIUtilFnca_procPostVars* fnca_proc_post_vars, RanoModule mod, ZnkHtpHdrs htp_hdrs,
		const char* content_type, ZnkStr pst_str, bool* is_updated )
{
	if( fnca_proc_post_vars ){
		bool result;
		bool updated_by_callback = false;
		result = fnca_proc_post_vars->func_( post_vars, fnca_proc_post_vars->arg_, content_type, &updated_by_callback );
		if( !result ){
			ZnkStr_add( pst_str, "  fnca_proc_post_vars is failure.\n" );
			return false;
		}
		*is_updated |= updated_by_callback;
	}
	if( mod ){
		bool   updated_by_plugin = false;
		size_t count = 0;
		/***
		 * ここで外部Callbackを呼び、現在のpost_varsの値を加味してfltrの微調整なども可能とする.
		 */
		RanoModule_invokeOnPost( mod, htp_hdrs->vars_, post_vars, &updated_by_plugin );
		count = RanoModule_filtPostVars( mod, post_vars );
		*is_updated |= ( updated_by_plugin || count > 0 );
	}
	return true;
}

bool
RanoCGIUtil_getPostVars_fromInBfr( ZnkVarpAry post_vars, RanoModule mod, ZnkHtpHdrs htp_hdrs,
		ZnkBfr in_bfr, size_t content_length,
		const char* content_type, const char* src_boundary, const char* dst_boundary, 
		ZnkStr pst_str, /* XSS-safe */ const char* varname_of_urlenc_body, bool is_unescape_val,
		RanoCGIUtilFnca_procPostVars* fnca_proc_post_vars )
{
	bool result = false;
	bool is_filter_done = false;
	bool is_updated = false;

	if( ZnkS_isBegin( content_type, "multipart/form-data;" ) ){
		/* ここで渡すboundaryはsrc_boundaryでなければならない */
		result = ZnkHtpPostVars_regist_byMPFDBody( post_vars,
				src_boundary, Znk_strlen(src_boundary),
				ZnkBfr_data(in_bfr), ZnkBfr_size(in_bfr), NULL );
		if( result ){
			result = updatePostVars( post_vars,
					fnca_proc_post_vars, mod, htp_hdrs,
					content_type, pst_str, &is_updated );
			if( !result ){
				goto FUNC_END;
			}
			if( is_updated ){
				/***
				 * PostVarsの修正が発生.
				 * 一度streamに展開し、Content-Lengthを更新した後、再構築しなおす.
				 */
				ZnkBfr stream_mdy = ZnkBfr_create_null();
				is_filter_done = true;
				/* ここで渡すboundaryはdst_boundaryでなければならない */
				RanoPost_extendPostVarsToStream_forMPFD( post_vars, stream_mdy, dst_boundary );
				ZnkHtpHdrs_registContentLength( htp_hdrs->vars_, ZnkBfr_size(stream_mdy) );
				ZnkBfr_destroy( stream_mdy );
			}
			if( is_filter_done && pst_str ){
				ZnkStr_add( pst_str, "<p><font color=red>  POST Variables are Modified by your send filter.</font></p>" );
			}
			if( pst_str ){
				RanoCGIUtil_addPostVarsStr( pst_str, post_vars ); /* XSS-safe */
			}
		} else if( pst_str ){
			ZnkStr_add( pst_str, "  Broken multipart/form-data.\n" );
		}

	} else {
		RanoCGIUtil_splitQueryString( post_vars, (char*)ZnkBfr_data(in_bfr), ZnkBfr_size(in_bfr), is_unescape_val );
		result = true; /* とりあえず必ず成功するとみなす */
		if( result ){
			result = updatePostVars( post_vars,
					fnca_proc_post_vars, mod, htp_hdrs,
					content_type, pst_str, &is_updated );
			if( !result ){
				goto FUNC_END;
			}
			if( is_updated ){
				/***
				 * PostVarsの修正が発生.
				 * 一度streamに展開し、Content-Lengthを更新した後、再構築しなおす.
				 */
				ZnkBfr stream_mdy = ZnkBfr_create_null();
				is_filter_done = true;
				RanoPost_extendPostVarsToStream_forURLE( post_vars, stream_mdy );
				ZnkHtpHdrs_registContentLength( htp_hdrs->vars_, ZnkBfr_size(stream_mdy) );

				/***
				 * varname_of_urlenc_body という変数にstream_mdyを格納して登録する.
				 * この変数を用いる場合は後に実際にPOST送信する際、RanoPost_extendPostVarsToStream_forURLE では展開しないことを想定している.
				 * (しかしながら素直に展開した方が良いのでは?)
				 */
				if( varname_of_urlenc_body ){
					ZnkHtpPostVars_regist( post_vars, varname_of_urlenc_body, "", ZnkHtpPostVar_e_None,
							ZnkBfr_data(stream_mdy), ZnkBfr_size(stream_mdy) );
				}

				ZnkBfr_destroy( stream_mdy );
			}

			if( !is_filter_done && varname_of_urlenc_body ){
				/***
				 * varname_of_urlenc_body という変数にin_bfrを格納して登録する.
				 */
				ZnkHtpPostVars_regist( post_vars, varname_of_urlenc_body, "", ZnkHtpPostVar_e_None,
						ZnkBfr_data(in_bfr), ZnkBfr_size(in_bfr) );
			}

			if( is_filter_done && pst_str ){
				ZnkStr_add( pst_str, "<p><font color=red>  POST Variables are Modified by your send filter.</font></p>" );
			}
			if( pst_str ){
				RanoCGIUtil_addPostVarsStr( pst_str, post_vars ); /* XSS-safe */
			}
		} else if( pst_str ){
			ZnkStr_add( pst_str, "  Broken application/x-www-form-urlencoded." );
		}

	}
FUNC_END:

	return result;
}


bool
RanoCGIUtil_getPostedFormData( RanoCGIEVar* evar, ZnkVarpAry post_vars, RanoModule mod, ZnkHtpHdrs htp_hdrs,
		ZnkStr pst_str, const char* varname_of_urlenc_body, bool is_unescape_val )
{
	bool        result = false;
	size_t      content_length = 0;
	const char* content_type   = evar->content_type_;
	ZnkBfr      in_bfr         = ZnkBfr_create_null();
	ZnkStr      HT_boundary    = ZnkStr_new( "--" ); /* -- が必要. */

	ZnkS_getSzU( &content_length, evar->content_length_ );
	RanoCGIUtil_getStdInStr( in_bfr, content_length );

	if( ZnkS_isBegin( content_type, "multipart/form-data;" ) ){
		const char* p = content_type + Znk_strlen_literal( "multipart/form-data;" );
		p = Znk_strstr( p, "boundary=" );
		if( p ){
			char* q;
			p += Znk_strlen_literal( "boundary=" );
			q = strchr( p, ';' );
			if( q ){
				ZnkStr_append( HT_boundary, p, q-p );
			} else {
				ZnkStr_add( HT_boundary, p );
			}
		}
	}

	result = RanoCGIUtil_getPostVars_fromInBfr( post_vars, mod, htp_hdrs,
			in_bfr, content_length,
			content_type, ZnkStr_cstr(HT_boundary), ZnkStr_cstr(HT_boundary),
			pst_str, varname_of_urlenc_body, is_unescape_val, NULL );

	ZnkBfr_destroy( in_bfr );
	return result;
}


void
RanoCGIMsg_initiate( bool with_html_start, const char* base_href )
{
	//Znk_printf( "Content-type: text/plain; charset=iso-8859-1\n" );
	Znk_printf( "Content-type: text/html; charset=Shift_JIS\r\n" );
	/* ブラウザにキャッシュさせない。 */
	Znk_printf( "Pragma: no-cache\r\n" );
	Znk_printf( "Cache-Control: no-cache\r\n" );
	Znk_printf( "\r\n" );

	if( with_html_start ){
		Znk_printf( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
		Znk_printf( "<html>\n" );
		Znk_printf( "<head>\n" );
		if( base_href ){
			Znk_printf( "<base href=\"%s\">\n", base_href );
		}
		Znk_printf( "<META http-equiv=\"Content-type\" content=\"text/html; charset=Shift_JIS\">\n" );
		Znk_printf( "<META http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n" );
		Znk_printf( "<META http-equiv=\"Content-Style-Type\" content=\"text/css\">\n" );
		Znk_printf( "</head>\n" );
		Znk_printf( "<body>\n" );
	}
}

void
RanoCGIMsg_finalize( void )
{
	Znk_printf( "</body></html>\n" );
	Znk_fflush( Znk_stdout() );
}

void
RanoCGIMsg_begin( void )
{
	Znk_printf( "<table bgcolor=\"lightgray\"><tr><td><font size=-1>" );
	Znk_printf( "<pre>\n" );
}

void
RanoCGIMsg_end( void )
{
	Znk_printf( "</pre>" );
	Znk_printf( "</font></td></tr></table>\n" );
	Znk_fflush( Znk_stdout() );
}

bool
RanoCGIUtil_displayBinary( const char* filename, const char* content_type )
{
	ZnkFile fp = Znk_fopen( filename, "rb" );
	if( fp ){
		uint8_t buf[ 4096 ];
		size_t read_size = 0;

		Znk_printf( "Content-type: %s\r\n", content_type );
		Znk_printf( "\r\n" );
		while( true ){
			read_size = Znk_fread( buf, sizeof(buf), fp );
			if( read_size == 0 ){
				break;
			}
			Znk_fwrite( buf, read_size, Znk_stdout() );
		}
		Znk_fclose( fp );
		Znk_fflush( Znk_stdout() );
	}

	return (bool)( fp != NULL );
}
bool
RanoCGIUtil_displayText( const char* filename, const char* content_type, RanoCGIUtilFuncT_procStr proc_str, void* proc_str_arg )
{
	ZnkFile fp = Znk_fopen( filename, "rb" );
	if( fp ){
		ZnkStr text = ZnkStr_new( "" );
		ZnkStr line = ZnkStr_new( "" );

		Znk_printf( "Content-type: %s\r\n", content_type );
		Znk_printf( "\r\n" );
		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				break;
			}
			ZnkStr_add( text, ZnkStr_cstr(line) );
		}
		Znk_fclose( fp );
		ZnkStr_delete( line );

		if( proc_str ){
			proc_str( text, proc_str_arg );
		}

		Znk_fwrite( (uint8_t*)ZnkStr_cstr(text), ZnkStr_leng(text), Znk_stdout() );
		Znk_fflush( Znk_stdout() );

		ZnkStr_delete( text );
	}

	return (bool)( fp != NULL );
}

void
RanoCGIUtil_replaceNLtoHtmlBR( ZnkStr str )
{
	ZnkSRef old_ptn = { 0 };
	ZnkSRef new_ptn = { 0 };
	ZnkSRef_set_literal( &old_ptn, "\n" );
	ZnkSRef_set_literal( &new_ptn, "<br>\n" );
	ZnkStrEx_replace_BF( str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
}

bool
RanoCGIUtil_printTemplateHTML( RanoCGIEVar* evar, ZnkBird bird, const char* template_html_file )
{
	ZnkFile fp = Znk_fopen( template_html_file, "rb" );
	if( fp ){
		ZnkStr  line = ZnkStr_new( "" );
		ZnkStr  text = ZnkStr_new( "" );

		RanoCGIMsg_initiate( false, NULL );
		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				/* eof */
				break;
			}
			ZnkStr_add( text, ZnkStr_cstr(line) );
		}

		if( bird ){
			ZnkBird_extend_self( bird, text, ZnkStr_leng(text) );
		}
		Znk_fputs( ZnkStr_cstr(text), Znk_stdout() );

		ZnkStr_delete( line );
		ZnkStr_delete( text );
		Znk_fclose( fp );
		return true;
	}
	return false;
}


const char*
RanoCGIUtil_skipProtocolPrefix( const char* src )
{
	if( Znk_strncmp( src, "http://", Znk_strlen_literal("http://") ) == 0 ){
		src += Znk_strlen_literal("http://");
	} else if( Znk_strncmp( src, "https://", Znk_strlen_literal("https://") ) == 0 ){
		src += Znk_strlen_literal("https://");
	} else if( Znk_strncmp( src, "file://", Znk_strlen_literal("file://") ) == 0 ){
		src += Znk_strlen_literal("file://");
	} else if( Znk_strncmp( src, "//", Znk_strlen_literal("//") ) == 0 ){
		/* //で始まるURL形式. 今見ているページプロトコルを引き継ぐ */
		src += Znk_strlen_literal("//");
	}
	return src;
}

RanoTextType
RanoCGIUtil_getTextType( const char* content_type )
{
	if( ZnkS_isBegin( content_type, "text/" ) ){
		const char* offset = content_type + Znk_strlen_literal("text/");
		if( ZnkS_eq( offset, "html" ) || ZnkS_isBegin( offset, "html;" ) ){
			return RanoText_HTML;
		} else if( ZnkS_eq( offset, "javascript" ) || ZnkS_isBegin( offset, "javascript;" ) ){
			return RanoText_JS;
		} else if( ZnkS_eq( offset, "css" ) || ZnkS_isBegin( offset, "css;" ) ){
			return RanoText_CSS;
		}
		/* text/plain */
		return RanoText_Plain;
	} else if( ZnkS_isBegin( content_type, "image/" ) ){
		return RanoText_Image;
	} else if( ZnkS_isBegin( content_type, "application/" ) ){
		const char* offset = content_type + Znk_strlen_literal("application/");
		if( ZnkS_eq( offset, "javascript" ) || ZnkS_isBegin( offset, "javascript;" ) ){
			return RanoText_JS;
		}
	} else if( ZnkS_isBegin( content_type, "video/" ) ){
		return RanoText_Video;
	}
	return RanoText_Binary;
}

size_t
RanoCGIUtil_initLog( const char* log_filebasename, const char* count_filename,
		size_t pitch_of_count, size_t pitch_of_logfiles, bool keep_open )
{
	bool    additional = true;
	char    log_filepath[ 256 ] = "";
	size_t  count = 0;
	size_t  log_no = 0;
	ZnkFile fp = NULL;
	
	fp = Znk_fopen( count_filename, "rb" );
	if( fp ){
		char buf[ 256 ] = "0";
		Znk_fgets( buf, sizeof(buf), fp );
		Znk_fclose( fp );
		ZnkS_getSzU( &count, buf );
	}

	log_no = ( count / pitch_of_count ) % pitch_of_logfiles;
	if( count % pitch_of_count == 0 ){
		additional = false; /* このタイミングで古いlog fileは消す */
	}
	Znk_snprintf( log_filepath, sizeof(log_filepath), "%s_%zu.log", log_filebasename, log_no );

	//RanoLog_open( log_filename, false, true );
	RanoLog_open( log_filepath, keep_open, additional );
	RanoLog_printf( "\n==== %zu ====\n", count );

	fp = Znk_fopen( count_filename, "wb" );
	if( fp ){
		Znk_fprintf( fp, "%zu", count+1 );
		Znk_fclose( fp );
	}
	return count;
}

size_t
RanoCGIUtil_initMultiDirLog( const char* logdir_basename, const char* count_filename,
		size_t pitch_of_count, size_t pitch_of_logfiles, bool keep_open )
{
	bool    additional = false;
	char    log_dirpath[ 256 ] = "";
	char    log_filepath[ 256 ] = "";
	size_t  count = 0;
	size_t  log_no = 0;
	ZnkFile fp = NULL;
	
	/* TODO : Global lockが必要 */
	fp = Znk_fopen( count_filename, "rb" );
	if( fp ){
		char buf[ 256 ] = "0";
		Znk_fgets( buf, sizeof(buf), fp );
		Znk_fclose( fp );
		ZnkS_getSzU( &count, buf );
	}
	fp = Znk_fopen( count_filename, "wb" );
	if( fp ){
		Znk_fprintf( fp, "%zu", count+1 );
		Znk_fclose( fp );
	}
	/* TODO : Global unlockが必要 */

	log_no = ( count / pitch_of_count ) % pitch_of_logfiles;
	Znk_snprintf( log_dirpath, sizeof(log_dirpath), "%s%zu", logdir_basename, log_no );
	ZnkDir_mkdirPath( log_dirpath, Znk_NPOS, '/', NULL );
	Znk_snprintf( log_filepath, sizeof(log_filepath), "%s/%zu.log", log_dirpath, count % pitch_of_count );

	RanoLog_open( log_filepath, keep_open, additional );
	RanoLog_printf( "\n==== %zu ====\n", count );

	return count;
}

