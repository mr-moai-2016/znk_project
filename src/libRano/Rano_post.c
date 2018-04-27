#include "Rano_post.h"
#include <Znk_htp_post.h>
#include <Znk_str_ex.h>
#include <Znk_htp_util.h>
#include <string.h>

Znk_INLINE void
appendBfr_byCStr( ZnkBfr bfr, const char* cstr )
{
	const size_t leng = strlen( cstr );
	ZnkBfr_append_dfr( bfr, (uint8_t*)cstr, leng );
}

void
RanoPost_extendPostVarsToStream_forMPFD( const ZnkVarpAry hpvs, ZnkBfr bfr, const char* boundary )
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
		if( varp->type_ == ZnkHtpPostVar_e_BinaryData ){
			const char* filename = ZnkStr_cstr( ZnkVar_misc(varp) );
			appendBfr_byCStr( bfr, "; filename=\"" );
			appendBfr_byCStr( bfr, filename );
			appendBfr_byCStr( bfr, "\"" );
			appendBfr_byCStr( bfr, "\r\n" );
			appendBfr_byCStr( bfr, "Content-Type: " );
			{
				const char* ext = ZnkS_get_extension( filename, '.' );
				const char* cntype = "application/octet-stream";
				if( ZnkS_eqCase( ext, "jpg" ) || ZnkS_eqCase( ext, "jpeg" ) ){
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
		if( varp->type_ == ZnkHtpPostVar_e_BinaryData ){
			ZnkBfr_append_dfr( bfr, ZnkBfr_data( varp->prim_.u_.bfr_ ), ZnkBfr_size( varp->prim_.u_.bfr_ ) ); 
		} else {
			ZnkBfr_append_dfr( bfr, (uint8_t*)ZnkStr_cstr( varp->prim_.u_.str_ ), ZnkStr_leng( varp->prim_.u_.str_ ) ); 
		}
		appendBfr_byCStr( bfr, "\r\n" );
	}
	appendBfr_byCStr( bfr, boundary );
	appendBfr_byCStr( bfr, "--\r\n" );
}
void
RanoPost_extendPostVarsToStream_forURLE( const ZnkVarpAry hpvs, ZnkBfr bfr )
{
	const size_t size = ZnkVarpAry_size( hpvs );
	size_t idx;
	ZnkVarp varp;
	ZnkStr stmt = ZnkStr_new("");
	for( idx=0; idx<size; ++idx ){
		varp = ZnkVarpAry_at( hpvs, idx );

		ZnkStr_clear( stmt );
		ZnkHtpURL_escape( stmt, (uint8_t*)ZnkVar_name_cstr(varp), ZnkStr_leng(varp->name_) );
		ZnkStr_add_c( stmt, '=' );
		ZnkHtpURL_escape( stmt, (uint8_t*)ZnkVar_cstr(varp), ZnkVar_str_leng(varp) );
		if( idx != size-1 ){
			ZnkStr_add_c( stmt, '&' );
		}

		ZnkBfr_append_dfr( bfr, (uint8_t*)ZnkStr_cstr(stmt), ZnkStr_leng(stmt) );
	}
	ZnkStr_delete( stmt );
}
#if 0
void
RanoPost_registPostVars_byHttpBodyURLE( ZnkVarpAry post_vars, const uint8_t* body, size_t body_leng )
{
	ZnkStrAry stmts = ZnkStrAry_create( true );
	size_t idx;
	size_t stmt_list_size;
	ZnkStr stmt;
	ZnkStr key = ZnkStr_new("");
	ZnkStr val = ZnkStr_new("");
	ZnkVarp varp;

	/***
	 * &Ç…ä÷ÇµÇƒsplitÇ∑ÇÈ.
	 */
	ZnkStrEx_addSplitC( stmts,
			(const char*)body, body_leng,
			'&', false, 8 );
	/***
	 * URL unescapeÇµÇΩÇ‡ÇÃÇpost_varsÇ…ìoò^.
	 */
	stmt_list_size = ZnkStrAry_size( stmts );
	for( idx=0; idx<stmt_list_size; ++idx ){
		stmt = ZnkStrAry_at( stmts, idx );
		ZnkStr_clear( key );
		ZnkStr_clear( val );
		ZnkStrEx_getKeyAndVal( ZnkStr_cstr(stmt), 0, ZnkStr_leng(stmt),
				"=", "",
				key, val );
		varp = ZnkVarp_create( "", "", ZnkHtpPostVar_e_None, ZnkPrim_e_Str, NULL );
		ZnkHtpURL_unescape_toStr( varp->name_,         ZnkStr_cstr(key), ZnkStr_leng(key) );
		ZnkHtpURL_unescape_toStr( varp->prim_.u_.str_, ZnkStr_cstr(val), ZnkStr_leng(val) );
		/* Ç±ÇÃ varp ÇÃéıñΩÇÕ post_varsÇÃä«óùâ∫Ç…íuÇ©ÇÍÇÈ */
		ZnkVarpAry_push_bk( post_vars, varp );
	}
	ZnkStr_delete( key );
	ZnkStr_delete( val );
	ZnkStrAry_destroy( stmts );
}
#endif


