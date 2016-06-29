#include "Moai_post.h"
#include "Moai_io_base.h"
#include "Moai_post_vars.h"
#include "Moai_connection.h"
#include "Moai_log.h"
#include "Moai_parent_proxy.h"
#include "Moai_enc_util.h"
#include <Znk_str.h>
#include <Znk_str_ex.h>
#include <Znk_bfr.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <Znk_htp_hdrs.h>
#include <string.h>


static bool st_post_confirm = true;


static void
addHdrFirstLine( ZnkStr str, const ZnkStrAry hdr1st )
{
	ZnkStr_addf( str, "  RequestLine[%s][%s][%s]\n",
			ZnkStrAry_at_cstr(hdr1st,0),
			ZnkStrAry_at_cstr(hdr1st,1),
			ZnkStrAry_at_cstr(hdr1st,2) );
}
static void
addHdrVarsStr( ZnkStr str, const ZnkVarpAry vars )
{
	const size_t size = ZnkVarpAry_size( vars );
	size_t idx;
	ZnkVarp var;
	const char* key;
	const char* val;
	size_t val_size;
	size_t val_idx;
	for( idx=0; idx<size; ++idx ){
		var = ZnkVarpAry_at( vars, idx );
		key = ZnkHtpHdrs_key_cstr( var );
		val_size = ZnkHtpHdrs_val_size( var );
		for( val_idx=0; val_idx<val_size; ++val_idx ){
			val = ZnkHtpHdrs_val_cstr( var, val_idx );
			ZnkStr_addf( str, "  %s: %s\n", key, val );
		}
	}
}

static void
addPostVarsStr( ZnkStr str, const ZnkVarpAry hpvs )
{
	const size_t size = ZnkVarpAry_size( hpvs );
	size_t idx;
	ZnkVarp varp;
	ZnkStr_addf( str, "  MoaiPostVars ID=[%p]\n\n", hpvs );
	for( idx=0; idx<size; ++idx ){
		varp = ZnkVarpAry_at( hpvs, idx );
		if( varp->type_ == MoaiPostVar_e_BinaryData ){
			ZnkStr_addf( str, "  name=[%s] filename=[%s] data byte size=[%u]\n",
					ZnkStr_cstr(varp->name_), ZnkStr_cstr(varp->filename_),
					ZnkBfr_size(varp->prim_.u_.bfr_) );
		} else {
			ZnkStr_addf( str, "  name=[%s] val=[%s]\n",
					ZnkStr_cstr(varp->name_), ZnkStr_cstr(varp->prim_.u_.str_) );
		}
	}
}

Znk_INLINE void
appendBfr_byCStr( ZnkBfr bfr, const char* cstr )
{
	const size_t leng = strlen( cstr );
	ZnkBfr_append_dfr( bfr, (uint8_t*)cstr, leng );
}
static void
extendPostVarsToStream_forMPFD( const ZnkVarpAry hpvs, ZnkBfr bfr, const char* boundary )
{
	const size_t size = ZnkVarpAry_size( hpvs );
	size_t idx;
	ZnkVarp varp;
	for( idx=0; idx<size; ++idx ){
		varp = ZnkVarpAry_at( hpvs, idx );
		appendBfr_byCStr( bfr, boundary );
		appendBfr_byCStr( bfr, "\r\n" );
		appendBfr_byCStr( bfr, "Content-Disposition: form-data; name=\"" );
		appendBfr_byCStr( bfr, ZnkStr_cstr( varp->name_ ) );
		appendBfr_byCStr( bfr, "\"" );
		if( varp->type_ == MoaiPostVar_e_BinaryData ){
			appendBfr_byCStr( bfr, "; filename=\"" );
			appendBfr_byCStr( bfr, ZnkStr_cstr( varp->filename_ ) );
			appendBfr_byCStr( bfr, "\"" );
			appendBfr_byCStr( bfr, "\r\n" );
			appendBfr_byCStr( bfr, "Content-Type: " );
			{
				const char* ext = ZnkS_get_extension( ZnkStr_cstr(varp->filename_), '.' );
				const char* cntype = "application/octet-stream";
				if( ZnkS_eqCase( ext, "jpg" ) || ZnkS_eqCase( ext, "jpg" ) ){
					cntype = "image/jpeg";
				} else if( ZnkS_eqCase( ext, "png" ) ){
					cntype = "image/png";
				} else if( ZnkS_eqCase( ext, "gif" ) ){
					cntype = "image/gif";
				} else if( ZnkS_eqCase( ext, "txt" ) ){
					cntype = "text/plain";
				} else if( ZnkS_eqCase( ext, "htm" ) || ZnkS_eqCase( ext, "htm" ) ){
					cntype = "text/html";
				} else {
				}
				appendBfr_byCStr( bfr, cntype );
			}
		}
		appendBfr_byCStr( bfr, "\r\n\r\n" );
		if( varp->type_ == MoaiPostVar_e_BinaryData ){
			ZnkBfr_append_dfr( bfr, ZnkBfr_data( varp->prim_.u_.bfr_ ), ZnkBfr_size( varp->prim_.u_.bfr_ ) ); 
		} else {
			ZnkBfr_append_dfr( bfr, (uint8_t*)ZnkStr_cstr( varp->prim_.u_.str_ ), ZnkStr_leng( varp->prim_.u_.str_ ) ); 
		}
		appendBfr_byCStr( bfr, "\r\n" );
	}
	appendBfr_byCStr( bfr, boundary );
	appendBfr_byCStr( bfr, "--\r\n" );
}
static void
extendPostVarsToStream_forURLE( const ZnkVarpAry hpvs, ZnkBfr bfr )
{
	const size_t size = ZnkVarpAry_size( hpvs );
	size_t idx;
	ZnkVarp varp;
	ZnkStr stmt = ZnkStr_new("");
	for( idx=0; idx<size; ++idx ){
		varp = ZnkVarpAry_at( hpvs, idx );

		ZnkStr_clear( stmt );
		MoaiEncURL_escape( stmt, (uint8_t*)ZnkVar_name_cstr(varp), ZnkStr_leng(varp->name_) );
		ZnkStr_add_c( stmt, '=' );
		MoaiEncURL_escape( stmt, (uint8_t*)ZnkVar_cstr(varp), ZnkVar_str_leng(varp) );
		if( idx != size-1 ){
			ZnkStr_add_c( stmt, '&' );
		}

		ZnkBfr_append_dfr( bfr, (uint8_t*)ZnkStr_cstr(stmt), ZnkStr_leng(stmt) );
	}
	ZnkStr_delete( stmt );
}


/*****************************************************************************/

MoaiPostMode
MoaiPost_decidePostMode( ZnkMyf config, const char* dst_hostname )
{
	MoaiPostMode post_mode = MoaiPostMode_e_SendDirect;
	ZnkStrAry   post_confirm_hosts = ZnkMyf_find_lines( config, "post_confirm_hosts" );

	if( ZnkStrAry_find_isMatch( post_confirm_hosts, 0, dst_hostname, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
		post_mode = MoaiPostMode_e_Confirm;
	}
	if( !st_post_confirm ){
		post_mode = MoaiPostMode_e_SendDirect;
	}
	return post_mode;
}
bool
MoaiPost_isPostConfirm( void )
{
	return st_post_confirm;
}
void
MoaiPost_setPostConfirm( bool post_confirm )
{
	st_post_confirm = post_confirm;
}

void
MoaiPost_parsePostAndCookieVars( ZnkSocket sock, MoaiFdSet mfds,
		ZnkStr str,
		const size_t hdr_size, ZnkStrAry hdr1st, const ZnkVarpAry hdr_vars,
		size_t content_length, ZnkBfr stream,
		ZnkVarpAry post_vars,
		const MoaiModule mod )
{
	const char* content_type = NULL;
	/***
	 * 実際に使用するboundaryはContent-Type内に記載されている文字列の前に
	 * さらに -- を付加したものを使用しなければならない.
	 */
	ZnkStr boundary = ZnkStr_new( "--" );
	ZnkStr pst_str = ZnkStr_new( "" );

	content_type = ZnkHtpHdrs_scanContentType( hdr_vars, boundary );
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
		size_t remain_size = 0;
		bool result;
		/***
		 * この場合、bodyの取得も試みる.
		 */
#if 1
		{
			ZnkVarp varp = ZnkHtpHdrs_find_literal( hdr_vars, "Content-Length" );
			if( varp ){
				const char* val = ZnkHtpHdrs_val_cstr( varp, 0 );
				ZnkF_printf_e( "  content_length=[%s]\n", val );
			} else {
				ZnkF_printf_e( "  content_length=[%u]\n", content_length );
			}
		}
#endif
		if( ZnkBfr_size( stream ) < hdr_size + content_length ){
			remain_size = hdr_size + content_length - ZnkBfr_size( stream );
		} else {
			remain_size = 0;
		}
		if( remain_size ){
			size_t result_size = 0;
			MoaiIO_recvBySize( stream, sock, mfds, remain_size, &result_size );
			ZnkF_printf_e( "result_size=[%u]\n", result_size );
		}

		result = MoaiPostVars_regist_byHttpBody( post_vars,
				ZnkStr_cstr(boundary), ZnkStr_leng(boundary),
				ZnkBfr_data(stream)+hdr_size,
				ZnkBfr_size(stream)-hdr_size );
		if( result ){
			bool is_filter_done = false;
			if( mod ){
				/***
				 * ここで外部Callbackを呼び、現在のpost_varsの値を加味して
				 * fltrの微調整なども可能とする.
				 */
				MoaiModule_invokeOnPost( mod, post_vars );
				if( MoaiModule_filtPostVars( mod, post_vars ) > 0 ){
					/***
					 * PostVarsの修正が発生.
					 * 一度streamに展開し、Content-Lengthを更新した後、再構築しなおす.
					 */
					ZnkBfr stream_mdy = ZnkBfr_create_null();

					is_filter_done = true;
					extendPostVarsToStream_forMPFD( post_vars, stream_mdy, ZnkStr_cstr(boundary) );
					{
						char val_str[ 256 ];
						ZnkVarp varp = ZnkHtpHdrs_find_literal( hdr_vars, "Content-Length" );
						ZnkStr val;
						Znk_snprintf( val_str, sizeof(val_str), "%u", ZnkBfr_size(stream_mdy) );
						val = ZnkHtpHdrs_val( varp, 0 );
						ZnkStr_set( val, val_str );
					}
					ZnkBfr_destroy( stream_mdy );
				}
			}
			if( is_filter_done ){
				ZnkStr_add( pst_str, "<p><font color=red>  POST Variables are Modified by your send filter.</font></p>" );
			}
			addPostVarsStr( pst_str, post_vars );
		} else {
			ZnkStr_add( pst_str, "  Broken multipart/form-data.\n" );
		}

	} else if( ZnkS_isBegin_literal( content_type, "application/x-www-form-urlencoded" )
//	        || ZnkS_isBegin_literal( content_type, "application/ocsp-request" )
	        || ZnkS_isBegin_literal( content_type, "text/xml" ) ){

		const uint8_t* body      = ZnkBfr_data(stream) + hdr_size;
		const size_t   body_leng = ZnkBfr_size(stream) - hdr_size;
		/***
		 * この場合にもbodyが存在する.
		 */
		ZnkStr_add( pst_str, "  This is application/x-www-form-urlencoded or text/xml\n" );
		ZnkStr_add( pst_str, "  " );
		ZnkStr_append( pst_str, (char*)body, body_leng );
		ZnkStr_add( pst_str, "\n" );

		{
			ZnkStrAry stmts = ZnkStrAry_create( true );
			size_t idx;
			size_t stmt_list_size;
			ZnkStr stmt;
			ZnkStr key = ZnkStr_new("");
			ZnkStr val = ZnkStr_new("");
			ZnkVarp varp;
			bool is_filter_done = false;

			/***
			 * &に関してsplitする.
			 */
			ZnkStrEx_addSplitC( stmts,
					(const char*)body, body_leng,
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
				varp = ZnkVarp_create( "", "", MoaiPostVar_e_None, ZnkPrim_e_Str );
				MoaiEncURL_unescape_toStr( varp->name_,         ZnkStr_cstr(key), ZnkStr_leng(key) );
				MoaiEncURL_unescape_toStr( varp->prim_.u_.str_, ZnkStr_cstr(val), ZnkStr_leng(val) );
				MoaiLog_printf( "URL unescape varp name=[%s] val=[%s]\n", ZnkVar_name_cstr(varp), ZnkVar_cstr(varp) );
				/* この varp の寿命は post_varsの管理下に置かれる */
				ZnkVarpAry_push_bk( post_vars, varp );
			}
			ZnkStr_delete( key );
			ZnkStr_delete( val );
			ZnkStrAry_destroy( stmts );

			if( mod ){
				/***
				 * ここで外部Callbackを呼び、現在のpost_varsの値を加味して
				 * fltrの微調整なども可能とする.
				 */
				MoaiModule_invokeOnPost( mod, post_vars );
				if( MoaiModule_filtPostVars( mod, post_vars ) > 0 ){
					/***
					 * PostVarsの修正が発生.
					 * 一度streamに展開し、Content-Lengthを更新した後、再構築しなおす.
					 */
					ZnkBfr stream_mdy = ZnkBfr_create_null();

					is_filter_done = true;
					extendPostVarsToStream_forURLE( post_vars, stream_mdy );
					{
						char val_str[ 256 ];
						ZnkVarp varp = ZnkHtpHdrs_find_literal( hdr_vars, "Content-Length" );
						ZnkStr val;
						Znk_snprintf( val_str, sizeof(val_str), "%u", ZnkBfr_size(stream_mdy) );
						val = ZnkHtpHdrs_val( varp, 0 );
						ZnkStr_set( val, val_str );
					}

					ZnkStr_add( pst_str, "\n" );
					ZnkStr_add( pst_str, "<font color=red>  POST Variables are Modified by your send filter.</font>\n\n" );
					addPostVarsStr( pst_str, post_vars );
					/***
					 * Moai_HtpPostTextBodyという変数に修正後の全bodyを格納して登録する.
					 */
					MoaiPostVars_regist( post_vars, "Moai_HtpPostTextBody", "",
							MoaiPostVar_e_None, ZnkBfr_data(stream_mdy), ZnkBfr_size(stream_mdy) );
					{
						ZnkVarp varp = ZnkVarpAry_find_byName( post_vars, "Moai_HtpPostTextBody", Znk_NPOS, false );
						ZnkStr_add( pst_str, "\n" );
						ZnkStr_add( pst_str, "  The following is finaly sending post vars stream.\n" );
						ZnkStr_addf( pst_str, "  %s\n", ZnkVar_cstr( varp ) );
					}

					ZnkBfr_destroy( stream_mdy );
				}
			}

			if( !is_filter_done ){
				addPostVarsStr( pst_str, post_vars );
				/***
				 * Moai_HtpPostTextBodyという変数に全bodyを格納して登録する.
				 */
				MoaiPostVars_regist( post_vars, "Moai_HtpPostTextBody", "",
						MoaiPostVar_e_None, body, body_leng );
			}
		}

	}
	//ZnkF_printf_e( "  boundary=[%s]\n", ZnkStr_cstr(boundary) );

	/***
	 * Cookie filtering
	 * PostVar filteringにおけるon_post関数の呼び出しでcookie_varsの値を
	 * 加工するような処理にも対応するため、このCookie filteringをここで呼び出す.
	 */
	if( mod ){
		MoaiModule_filtCookieVars( mod, hdr_vars );
	}

	/***
	 * confirm用
	 * メッセージ内容を正確なものとするため、filtering処理などが全て終った後に
	 * 全文を構築しなければならない.
	 */
	{
		ZnkStr_add( str, "<p><b>Moai : HTTP Header Confirmation.</b></p>" );
		addHdrFirstLine( str, hdr1st );
		ZnkStr_addf( str, "\n" );
		addHdrVarsStr( str, hdr_vars );
		ZnkStr_addf( str, "\n" );

		ZnkStr_add( str, "<p><b>Moai : POST Variable Confirmation.</b></p>" );
		ZnkStr_add( str, "  This is application/x-www-form-urlencoded or text/xml\n" );
		ZnkStr_add( str, ZnkStr_cstr(pst_str) );
	}

	ZnkStr_delete( boundary );
	ZnkStr_delete( pst_str );
}

bool
MoaiPost_sendRealy( MoaiInfoID info_id,
		bool is_realy_send, ZnkSocket O_sock, MoaiFdSet mfds )
{
	bool result = false;
	ZnkBfr stream_fnl = ZnkBfr_create_null();
	MoaiInfo* info = MoaiInfo_find( info_id );

	{
		const char* content_type = "";
		ZnkStr boundary = ZnkStr_new( "--" );
		ZnkHtpHdrs_extendToStream( info->hdrs_.hdr1st_, info->hdrs_.vars_, stream_fnl, true );
		content_type = ZnkHtpHdrs_scanContentType( info->hdrs_.vars_, boundary );
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
			extendPostVarsToStream_forMPFD( info->vars_, stream_fnl, ZnkStr_cstr(boundary) );
		} else if( ZnkS_isBegin_literal( content_type, "application/x-www-form-urlencoded" )
//	        	|| ZnkS_isBegin_literal( content_type, "application/ocsp-request" )
		        || ZnkS_isBegin_literal( content_type, "text/xml" ) ){
			ZnkVarp varp = ZnkVarpAry_find_byName( info->vars_, "Moai_HtpPostTextBody", Znk_NPOS, false );
			if( varp ){
				const char* val = ZnkStr_cstr( varp->prim_.u_.str_ );
				appendBfr_byCStr( stream_fnl, val );
			}
		}
		ZnkStr_delete( boundary );
	}
	if( is_realy_send ){
		ZnkVarp varp = ZnkHtpHdrs_find_literal( info->hdrs_.vars_, "Host" );
		if( varp ){
			//const char* hostname = ZnkHtpHdrs_val_cstr( varp, 0 );
			ZnkFdSet fdst_observe = MoaiFdSet_fdst_observe_r( mfds );
			ZnkBfr_swap( info->stream_, stream_fnl );
			ZnkSocket_send( O_sock, ZnkBfr_data(info->stream_), ZnkBfr_size(info->stream_) );
			//MoaiLog_printf( "[[[[%s]]]]\n", (char*)ZnkBfr_data(info->stream_) );
			if( !ZnkFdSet_set( fdst_observe, O_sock ) ){
				MoaiFdSet_reserveConnectSock( mfds, O_sock );
			}
		}
	}
	result = true;
	ZnkBfr_destroy( stream_fnl );
	return result;
}

