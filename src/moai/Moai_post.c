#include "Moai_post.h"
#include "Moai_io_base.h"
#include "Moai_post_vars.h"
#include "Moai_connection.h"
#include <Znk_str.h>
#include <Znk_bfr.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <Znk_htp_hdrs.h>
#include <string.h>

typedef enum {
	 PostMode_e_Confirm=0
	,PostMode_e_SendAfterConfirm
	,PostMode_e_SendDirect
} PostMode;

static ZnkObjDAry st_post_vars_dary;

static void
deleteHtpInfo( void* elem )
{
	if( elem ){
		MoaiInfo* info = (MoaiInfo*)elem;
		ZnkHtpHdrs_dispose( &info->hdrs_ );
		ZnkVarpDAry_destroy( info->vars_ );
		ZnkBfr_destroy( info->stream_ );
		Znk_free( info );
	}
}
static MoaiInfo*
makeHtpInfo( void )
{
	MoaiInfo* info = (MoaiInfo*)Znk_malloc( sizeof( MoaiInfo ) );
	ZnkHtpHdrs_compose( &info->hdrs_ );
	info->vars_   = ZnkVarpDAry_create( true );
	info->stream_ = ZnkBfr_create_null();
	info->hdr_size_ = 0;
	return info;
}

void
MoaiPost_initiate( void )
{
	if( st_post_vars_dary == NULL ){
		st_post_vars_dary = ZnkObjDAry_create( deleteHtpInfo );
	}
}

static MoaiInfo*
findHtpInfo( MoaiInfo* query_info )
{
	MoaiInfo* info;
	const size_t size = ZnkObjDAry_size( st_post_vars_dary );
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		info = (MoaiInfo*) ZnkObjDAry_at( st_post_vars_dary, idx );
		if( info == query_info ){
			/* found */
			return info;
		}
	}
	/* not found */
	return NULL;
}

static void
addHdrFirstLine( ZnkStr str, const ZnkStrDAry hdr1st )
{
	ZnkStr_addf( str, "  RequestLine[%s][%s][%s]\n",
			ZnkStrDAry_at_cstr(hdr1st,0),
			ZnkStrDAry_at_cstr(hdr1st,1),
			ZnkStrDAry_at_cstr(hdr1st,2) );
}
static void
addHdrVarsStr( ZnkStr str, const ZnkVarpDAry vars )
{
	const size_t size = ZnkVarpDAry_size( vars );
	size_t idx;
	ZnkVarp var;
	const char* key;
	const char* val;
	size_t val_size;
	size_t val_idx;
	for( idx=0; idx<size; ++idx ){
		var = ZnkVarpDAry_at( vars, idx );
		key = ZnkHtpHdrs_key_cstr( var );
		val_size = ZnkHtpHdrs_val_size( var );
		for( val_idx=0; val_idx<val_size; ++val_idx ){
			val = ZnkHtpHdrs_val_cstr( var, val_idx );
			ZnkStr_addf( str, "  %s: %s\n", key, val );
		}
	}
}

static void
addPostVarsStr( ZnkStr str, const ZnkVarpDAry hpvs )
{
	const size_t size = ZnkVarpDAry_size( hpvs );
	size_t idx;
	ZnkVarp varp;
	ZnkStr_addf( str, "  MoaiPostVars ID=[%p]\n\n", hpvs );
	for( idx=0; idx<size; ++idx ){
		varp = ZnkVarpDAry_at( hpvs, idx );
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
extendPostVarsToStream( const ZnkVarpDAry hpvs, ZnkBfr bfr, const char* boundary )
{
	const size_t size = ZnkVarpDAry_size( hpvs );
	size_t idx;
	ZnkVarp varp;
	for( idx=0; idx<size; ++idx ){
		varp = ZnkVarpDAry_at( hpvs, idx );
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

static bool
sendRealy( ZnkStr str, const MoaiInfo* target,
		bool is_realy_send, ZnkSocket sock, ZnkFdSet fdst_observe,
		bool is_print_stream )
{
	bool result = false;
	ZnkBfr stream_fnl = ZnkBfr_create_null();

	ZnkStr_addf( str, "<p><b>Moai : POST simulation[%d] to target=[%p]</b></p>", !is_realy_send, target );
	addHdrFirstLine( str, target->hdrs_.hdr1st_ );
	ZnkStr_addf( str, "\n" );
	addHdrVarsStr( str, target->hdrs_.vars_ );
	ZnkStr_addf( str, "\n" );
	addPostVarsStr( str, target->vars_ );
	{
		const char* content_type = "";
		ZnkStr boundary = ZnkStr_new( "--" );
		ZnkHtpHdrs_extendToStream( target->hdrs_.hdr1st_, target->hdrs_.vars_, stream_fnl, true );
		content_type = ZnkHtpHdrs_scanContentType( target->hdrs_.vars_, boundary );
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
			extendPostVarsToStream( target->vars_, stream_fnl, ZnkStr_cstr(boundary) );
		} else if( ZnkS_isBegin_literal( content_type, "application/x-www-form-urlencoded" )
		        || ZnkS_isBegin_literal( content_type, "text/xml" ) ){
			ZnkVarp varp = ZnkVarpDAry_find_byName( target->vars_, "Moai_HtpPostTextBody", Znk_NPOS, false );
			if( varp ){
				const char* val = ZnkStr_cstr( varp->prim_.u_.str_ );
				appendBfr_byCStr( stream_fnl, val );
			}
		}
		ZnkStr_delete( boundary );
	}
	if( is_print_stream ){
		ZnkStr_append( str,
				(char*)ZnkBfr_data(stream_fnl),
				ZnkBfr_size(stream_fnl) );
	}
	if( is_realy_send ){
		int result_size = 0;
		const char* hostname = "";
		bool connect_ok = false;
		MoaiConnection htcn = MoaiConnection_find( sock );
		ZnkF_printf_e( "is_realy_send : htcn->dst_sock_=[%d]\n", htcn->dst_sock_ );
		if( htcn->dst_sock_ == ZnkSocket_INVALID ){
			ZnkVarp varp = ZnkHtpHdrs_find_literal( target->hdrs_.vars_, "Host" );
			ZnkF_printf_e( "is_realy_send : varp=[%p]\n", varp );
			if( varp ){
				MoaiConnection htcn_r;
				hostname = ZnkHtpHdrs_val_cstr( varp, 0 );
				htcn->dst_sock_ = ZnkSocket_open();
				htcn->port_   = 80;
				ZnkS_copy( htcn->hostname_, sizeof(htcn->hostname_), hostname, Znk_NPOS );
				htcn_r = MoaiConnection_intern( htcn->dst_sock_, MoaiSockType_e_Outer );
				if( htcn_r ){
					htcn_r->dst_sock_ = sock;
					htcn_r->port_     = 80;
					ZnkS_copy( htcn_r->hostname_, sizeof(htcn_r->hostname_), hostname, Znk_NPOS );
				}
				ZnkF_printf_e( "is_realy_send : hostname=[%s] htcn->hostname_=[%s]\n", hostname, htcn->hostname_ );
				if( target->proxy_hostname_ ){
					if( !ZnkSocket_connectToServer( htcn->dst_sock_, target->proxy_hostname_, target->proxy_port_, NULL ) ){
						ZnkF_printf_e( "is_realy_send : ZnkSocket_connectToServer : Error : hostname=[%s] port=[%d]\n",
								target->proxy_hostname_, target->proxy_port_ );
						goto FUNC_END;
					} else {
						connect_ok = true;
					}
				} else {
					if( !ZnkSocket_connectToServer( htcn->dst_sock_, htcn->hostname_, htcn->port_, NULL ) ){
						ZnkF_printf_e( "is_realy_send : ZnkSocket_connectToServer : Error : hostname=[%s] port=[%d]\n",
								htcn->hostname_, htcn->port_ );
						goto FUNC_END;
					} else {
						connect_ok = true;
					}
				}
			}
		} else {
			connect_ok = true;
		}
		if( connect_ok ){
			result_size = ZnkSocket_send( htcn->dst_sock_, ZnkBfr_data(stream_fnl), ZnkBfr_size(stream_fnl) );
			ZnkF_printf_e( "is_realy_send : send to sock=[%d] result_size=[%d]\n", htcn->dst_sock_, result_size );
			ZnkFdSet_set( fdst_observe, htcn->dst_sock_ );
		}
	}
	result = true;
FUNC_END:
	ZnkBfr_destroy( stream_fnl );
	return result;
}


MoaiInfo*
MoaiPost_parsePostVars( ZnkSocket sock, ZnkFdSet fdst_observe,
		ZnkStr str, const char* dst_hostname, MoaiInfo* info, MoaiBodyInfo* body_info,
		MoaiModuleAry mod_ary, ZnkMyf mtgt )
{
	MoaiInfo* new_info = NULL;
	const char* content_type = NULL;
	const bool  is_print_stream = false;

	/***
	 * 実際に使用するboundaryはContent-Type内に記載されている文字列の前に
	 * さらに -- を付加したものを使用しなければならない.
	 */
	ZnkStr boundary = ZnkStr_new( "--" );
	new_info = makeHtpInfo();
	new_info->proxy_hostname_ = info->proxy_hostname_;
	new_info->proxy_port_     = info->proxy_port_;

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
		size_t remain_size = 0;
		/***
		 * この場合、bodyの取得も試みる.
		 */
#if 1
		{
			ZnkVarp varp = ZnkHtpHdrs_find_literal( info->hdrs_.vars_, "Content-Length" );
			if( varp ){
				const char* val = ZnkHtpHdrs_val_cstr( varp, 0 );
				ZnkF_printf_e( "  content_length=[%s]\n", val );
			}
			ZnkF_printf_e( "  body_info.content_length=[%u]\n", body_info->content_length_ );
		}
#endif
		if( ZnkBfr_size( info->stream_ ) < info->hdr_size_ + body_info->content_length_ ){
			remain_size = info->hdr_size_ + body_info->content_length_ - ZnkBfr_size( info->stream_ );
		} else {
			remain_size = 0;
		}
		if( remain_size ){
			size_t result_size = 0;
			MoaiIO_recvBySize( info->stream_, sock, fdst_observe, remain_size, &result_size );
			ZnkF_printf_e( "result_size=[%u]\n", result_size );
		}
		ZnkStr_add( str, "<p><b>Moai : POST Variable Confirmation.</b></p>" );

		{
			bool result;
			if( is_print_stream ){
				ZnkStr_append( str,
						(char*)(ZnkBfr_data(info->stream_)+info->hdr_size_),
						ZnkBfr_size(info->stream_)-info->hdr_size_ );
			}
			result = MoaiPostVars_regist_byHttpBody( new_info->vars_,
					ZnkStr_cstr(boundary), ZnkStr_leng(boundary),
					ZnkBfr_data(info->stream_)+info->hdr_size_,
					ZnkBfr_size(info->stream_)-info->hdr_size_ );
			if( result ){
				const MoaiModule mod = MoaiModuleAry_find_byHostname( mod_ary, mtgt, dst_hostname );
				if( mod ){
					/***
					 * ここで外部Callbackを呼び、fltrの微調整を可能としたい.
					 * そのようなことが可能なコマンドをinstallフォルダ内に置き、
					 * それをsystemで呼び出す指針でいく.
					 */
					MoaiModule_invokeOnPostBefore( mod );
					if( MoaiModule_filtPostVars( mod, new_info->vars_ ) > 0 ){
						/***
						 * PostVarsの修正が発生.
						 * 一度streamに展開し、Content-Lengthを更新した後、再構築しなおす.
						 */
						ZnkBfr stream_mdy = ZnkBfr_create_null();
						ZnkStr_add( str, "<p><b>Moai : POST Variable Modified.</b></p>" );
						extendPostVarsToStream( new_info->vars_, stream_mdy, ZnkStr_cstr(boundary) );
						{
							char val_str[ 256 ];
							ZnkVarp varp = ZnkHtpHdrs_find_literal( info->hdrs_.vars_, "Content-Length" );
							ZnkStr val;
							Znk_snprintf( val_str, sizeof(val_str), "%u", ZnkBfr_size(stream_mdy) );
							val = ZnkHtpHdrs_val( varp, 0 );
							ZnkStr_set( val, val_str );
						}
						if( is_print_stream ){
							ZnkStr_append( str,
									(char*)ZnkBfr_data(stream_mdy),
									ZnkBfr_size(stream_mdy) );
						}
						ZnkBfr_destroy( stream_mdy );
					}
				}
				addPostVarsStr( str, new_info->vars_ );
			} else {
				ZnkStr_add( str, "  Broken multipart/form-data.\n" );
			}
		}

	} else if( ZnkS_isBegin_literal( content_type, "application/x-www-form-urlencoded" )
	        || ZnkS_isBegin_literal( content_type, "text/xml" ) ){
		/***
		 * この場合にもbodyが存在する.
		 * とりあえず現時点では作業時間がないためデコードなどせずそのまま転送する.
		 * つまりapplication/x-www-form-urlencodedの場合はその内容をfiltering
		 * する機能を(現時点では)サポートしない.
		 */
		ZnkStr_add( str, "<p><b>Moai : POST Variable Confirmation.</b></p>" );
		ZnkStr_add( str, "  This is application/x-www-form-urlencoded or text/xml\n" );
		ZnkStr_add( str, "  " );
		ZnkStr_append( str,
				(char*)ZnkBfr_data(info->stream_)+info->hdr_size_,
				ZnkBfr_size(info->stream_)-info->hdr_size_ );
		/***
		 * 本来、urlencodedを解析して各個変数に分離した上で登録すべきだが、
		 * 現時点ではそうしない.
		 * 変わりにMoai_HtpPostTextBodyという変数に全bodyを格納して登録する.
		 */
		MoaiPostVars_regist( new_info->vars_, "Moai_HtpPostTextBody", "",
				MoaiPostVar_e_None, ZnkBfr_data(info->stream_)+info->hdr_size_,
				ZnkBfr_size(info->stream_)-info->hdr_size_ );
	}
	ZnkF_printf_e( "  boundary=[%s]\n", ZnkStr_cstr(boundary) );
	ZnkStr_delete( boundary );

	return new_info;
}

void
MoaiPost_proc( ZnkSocket sock, ZnkFdSet fdst_observe,
		const char* dst_hostname, MoaiInfo* info, MoaiBodyInfo* body_info,
		bool as_local_proxy, bool post_confirm,
		MoaiModuleAry mod_ary, ZnkMyf mtgt, ZnkMyf config, const char* req_urp )
{
	ZnkStr      str = ZnkStr_new( "" );
	MoaiInfo*   new_info = NULL;
	const bool  is_print_stream = false;
	bool        is_realy_send = false;
	MoaiInfo*   target = NULL;
	const char* content_type = NULL;
	PostMode    post_mode = PostMode_e_SendDirect;
	ZnkStrDAry  post_confirm_hosts = ZnkMyf_find_lines( config, "post_confirm_hosts" );

	if( post_confirm_hosts && ZnkStrDAry_find( post_confirm_hosts, 0, dst_hostname, Znk_NPOS ) != Znk_NPOS ){
		post_mode = PostMode_e_Confirm;
	}
	if( !post_confirm ){
		post_mode = PostMode_e_SendDirect;
	}

	ZnkStr_add( str, "<p><b>Moai : POST Header Confirmation.</b></p>" );
	ZnkStr_addf( str, "  DestinationTo[%s] as_local_proxy=[%d]\n", dst_hostname, as_local_proxy );

	addHdrFirstLine( str, info->hdrs_.hdr1st_ );
	addHdrVarsStr( str, info->hdrs_.vars_ );

	new_info = MoaiPost_parsePostVars( sock, fdst_observe,
			str, dst_hostname, info, body_info,
			mod_ary, mtgt );

	{
		ZnkVarp varp = ZnkVarpDAry_find_byName( new_info->vars_, "Moai_HtpPostVar_ID", Znk_NPOS, false );
		if( varp ){
			void* ptr = 0;
			const char* id_str = ZnkStr_cstr( varp->prim_.u_.str_ );
			post_mode = PostMode_e_SendAfterConfirm;
			ZnkS_getPtrX( &ptr, id_str );
			target = findHtpInfo( (MoaiInfo*)ptr );
		}
	}

	switch( post_mode ){
	case PostMode_e_Confirm:
	{
		const char* hostname = "";
		ZnkVarp varp = ZnkHtpHdrs_find_literal( info->hdrs_.vars_, "Host" );
		if( varp ){
			hostname = ZnkHtpHdrs_val_cstr( varp, 0 );
		}
		ZnkStr_addf( str, "<p><b>Moai : POST to %s%s.</b></p>", hostname, req_urp );
		ZnkStr_addf( str, "<form action=\"http://%s%s\" method=\"POST\" enctype=\"multipart/form-data\">",
				hostname, req_urp );
		ZnkStr_addf( str, "<input type=hidden name=Moai_HtpPostVar_ID value=\"%p\">", new_info );
		ZnkStr_addf( str, "<input type=submit value=\"Send\"> " );
		ZnkStr_addf( str, "<input type=checkbox name=Moai_HtpPostSimulation> Post Simulation Mode.\n" );
		ZnkStr_addf( str, "</form>\n" );
		ZnkStrDAry_copy( new_info->hdrs_.hdr1st_, info->hdrs_.hdr1st_ );
		ZnkHtpHdrs_copyVars( new_info->hdrs_.vars_, info->hdrs_.vars_ );
		ZnkObjDAry_push_bk( st_post_vars_dary, (ZnkObj)new_info );
		break;
	}
	case PostMode_e_SendAfterConfirm:
	{
		ZnkVarp simp = ZnkVarpDAry_find_byName( new_info->vars_, "Moai_HtpPostSimulation", Znk_NPOS, false );
		if( simp == NULL ){
			/* checkbox で指定された変数は、それがcheckされている場合のみ存在する */
			is_realy_send = true;
		}
		if( target ){
			ZnkBfr stream_fnl = ZnkBfr_create_null();
			ZnkStr_addf( str, "<p><b>Moai : POST simulation[%d] to target=[%p]</b></p>", !is_realy_send, target );
			addHdrFirstLine( str, target->hdrs_.hdr1st_ );
			ZnkStr_addf( str, "\n" );
			addHdrVarsStr( str, target->hdrs_.vars_ );
			ZnkStr_addf( str, "\n" );
			addPostVarsStr( str, target->vars_ );
			{
				ZnkStr boundary = ZnkStr_new( "--" );
				ZnkHtpHdrs_extendToStream( target->hdrs_.hdr1st_, target->hdrs_.vars_, stream_fnl, true );
				content_type = ZnkHtpHdrs_scanContentType( target->hdrs_.vars_, boundary );
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
					extendPostVarsToStream( target->vars_, stream_fnl, ZnkStr_cstr(boundary) );
				} else if( ZnkS_isBegin_literal( content_type, "application/x-www-form-urlencoded" )
				        || ZnkS_isBegin_literal( content_type, "text/xml" ) ){
					ZnkVarp varp = ZnkVarpDAry_find_byName( target->vars_, "Moai_HtpPostTextBody", Znk_NPOS, false );
					if( varp ){
						const char* val = ZnkStr_cstr( varp->prim_.u_.str_ );
						appendBfr_byCStr( stream_fnl, val );
					}
				}
				ZnkStr_delete( boundary );
			}
			if( is_print_stream ){
				ZnkStr_append( str,
						(char*)ZnkBfr_data(stream_fnl),
						ZnkBfr_size(stream_fnl) );
			}
			if( is_realy_send ){
				int result_size = 0;
				const char* hostname = "";
				bool connect_ok = false;
				MoaiConnection htcn = MoaiConnection_find( sock );
				ZnkF_printf_e( "is_realy_send : htcn->dst_sock_=[%d]\n", htcn->dst_sock_ );
				if( htcn->dst_sock_ == ZnkSocket_INVALID ){
					ZnkVarp varp = ZnkHtpHdrs_find_literal( target->hdrs_.vars_, "Host" );
					ZnkF_printf_e( "is_realy_send : varp=[%p]\n", varp );
					if( varp ){
						MoaiConnection htcn_r;
						hostname = ZnkHtpHdrs_val_cstr( varp, 0 );
						htcn->dst_sock_ = ZnkSocket_open();
						htcn->port_   = 80;
						ZnkS_copy( htcn->hostname_, sizeof(htcn->hostname_), hostname, Znk_NPOS );
						htcn_r = MoaiConnection_intern( htcn->dst_sock_, MoaiSockType_e_Outer );
						if( htcn_r ){
							htcn_r->dst_sock_ = sock;
							htcn_r->port_     = 80;
							ZnkS_copy( htcn_r->hostname_, sizeof(htcn_r->hostname_), hostname, Znk_NPOS );
						}
						ZnkF_printf_e( "is_realy_send : hostname=[%s] htcn->hostname_=[%s]\n", hostname, htcn->hostname_ );
						if( !ZnkSocket_connectToServer( htcn->dst_sock_, htcn->hostname_, htcn->port_, NULL ) ){
							ZnkF_printf_e( "is_realy_send : ZnkSocket_connectToServer : Error : hostname=[%s] port=[%d]\n",
									htcn->hostname_, htcn->port_ );
						} else {
							connect_ok = true;
						}
					}
				} else {
					connect_ok = true;
				}
				if( connect_ok ){
					result_size = ZnkSocket_send( htcn->dst_sock_, ZnkBfr_data(stream_fnl), ZnkBfr_size(stream_fnl) );
					ZnkF_printf_e( "is_realy_send : send to sock=[%d] result_size=[%d]\n", htcn->dst_sock_, result_size );
					ZnkFdSet_set( fdst_observe, htcn->dst_sock_ );
				}
			}
			ZnkBfr_destroy( stream_fnl );
		} else {
			/***
			 * 例えば、POST Confirmation画面でMoaiを途中で再起動させた場合
			 * そのポインタが無効となりfindHtpInfoに失敗するというケースは有り得る.
			 * この場合ユーザはPOST処理を最初からやり直さなければならない.
			 */
			ZnkStr_addf( str, "Moai : POST Error : [target does not found.]" );
		}
		deleteHtpInfo( new_info );
		break;
	}
	case PostMode_e_SendDirect:
		is_realy_send = true;
		ZnkStrDAry_copy( new_info->hdrs_.hdr1st_, info->hdrs_.hdr1st_ );
		ZnkHtpHdrs_copyVars( new_info->hdrs_.vars_, info->hdrs_.vars_ );
		sendRealy( str, new_info,
				is_realy_send, sock, fdst_observe,
				is_print_stream );
		deleteHtpInfo( new_info );
		break;
	default:
		assert( 0 );
		break;
	}

	if( !is_realy_send ){
		int ret = MoaiIO_sendTxtf( sock, "text/html",
				"<html><body><pre>\n%s</pre></body></html>",
				ZnkStr_cstr(str) );
		if( ret < 0 ){
			MoaiIO_closeSocket( "  PostRealy", sock, fdst_observe );
		}
	}
	ZnkStr_delete( str );
}
