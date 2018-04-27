#include "Znk_htp_hdrs.h"
#include "Znk_s_base.h"
#include "Znk_varp_ary.h"
#include "Znk_missing_libc.h"
#include <string.h>

void
ZnkHtpHdrs_registHdr1st( const ZnkStrAry hdr1st, const char* first_line, size_t first_line_leng )
{
	size_t arg_leng = 0;
	size_t arg_pos = 0;
	const char* p = first_line;
	const char* q = NULL;
	const char* end = first_line + first_line_leng;

	ZnkStrAry_resize( hdr1st, 3, "" );

	arg_pos = ZnkS_lfind_arg( p, 0, (size_t)(end-p), 0, &arg_leng, " \t\r\n", 4 );
	p += arg_pos;
	ZnkStr_assign( ZnkStrAry_at( hdr1st, 0 ), 0, p, arg_leng );
	p += arg_leng;

	arg_pos = ZnkS_lfind_arg( p, 0, (size_t)(end-p), 0, &arg_leng, " \t\r\n", 4 );
	p += arg_pos;
	ZnkStr_assign( ZnkStrAry_at( hdr1st, 1 ), 0, p, arg_leng );
	p += arg_leng;

	arg_pos = ZnkS_lfind_arg( p, 0, (size_t)(end-p), 0, &arg_leng, " \t\r\n", 4 );
	p += arg_pos;
	q = strchr( p, '\r' );
	if( q ){ end = q; }
	ZnkStr_assign( ZnkStrAry_at( hdr1st, 2 ), 0, p, (size_t)(end-p) );
}
ZnkVarp
ZnkHtpHdrs_regist( ZnkVarpAry vars,
		const char* key, size_t key_leng,
		const char* val, size_t val_leng,
		bool is_unique )
{
	ZnkVarp varp = ZnkHtpHdrs_find( vars, key, key_leng );
	if( varp == NULL ){
		varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_StrAry, NULL );
		ZnkStr_assign( varp->name_, 0, key, key_leng );
		ZnkVarpAry_push_bk( vars, varp );
	}
	if( is_unique ){
		ZnkStrAry_clear( ZnkPrim_strAry( &varp->prim_ ) );
	}
	ZnkStrAry_push_bk_cstr( varp->prim_.u_.sda_, val, val_leng );
	return varp;
}
ZnkVarp
ZnkHtpHdrs_regist_byLine( ZnkVarpAry vars, const char* hdr_line, size_t hdr_line_leng )
{
	static const bool use_eqCase = true;
	size_t key_begin; size_t key_end;
	size_t val_begin; size_t val_end;
	ZnkVarp varp = NULL;

	ZnkS_find_key_and_val( hdr_line, 0, hdr_line_leng,
			&key_begin, &key_end,
			&val_begin, &val_end,
			":", 1,
			" \t", 2 );

	if( key_begin == Znk_NPOS || key_end == Znk_NPOS || val_begin == Znk_NPOS ){
		return NULL;
	}

	varp = ZnkVarpAry_find_byName( vars,
			hdr_line+key_begin, key_end-key_begin, use_eqCase );
	if( varp == NULL ){
		/* 新規追加 */
		varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_StrAry, NULL );
		ZnkStr_assign( varp->name_, 0, hdr_line+key_begin, key_end-key_begin );
		ZnkVarpAry_push_bk( vars, varp );
	}
	ZnkStrAry_push_bk_cstr( varp->prim_.u_.sda_,
			hdr_line+val_begin, val_end-val_begin );
	return varp;
}


bool
ZnkHtpHdrs_erase( const ZnkVarpAry vars, const char* query_key )
{
	const size_t size = ZnkVarpAry_size( vars );
	size_t idx;
	ZnkVarp varp;
	for( idx=0; idx<size; ++idx ){
		varp = ZnkVarpAry_at( vars, idx );
		/***
		 * 例えば、Content-Type や Content-typeなどとなっているサイトもある.
		 * これに対応するため、ここではCase系文字列比較関数を用いなければならない.
		 */
		if( ZnkS_eqCase( ZnkStr_cstr(varp->name_), query_key ) ){
			/* found */
			ZnkVarpAry_erase_byIdx( vars, idx );
			return true;
		}
	}
	return false;
}
void
ZnkHtpHdrs_copyVars( ZnkVarpAry dst_vars, const ZnkVarpAry src_vars )
{
	const size_t size = ZnkVarpAry_size( src_vars );
	size_t idx;
	ZnkVarp src_var;
	ZnkVarp dst_var;
	ZnkVarpAry_clear( dst_vars );
	for( idx=0; idx<size; ++idx ){
		src_var = ZnkVarpAry_at( src_vars, idx );
		dst_var = ZnkVarp_create( 
				ZnkStr_cstr( src_var->name_ ), ZnkStr_cstr( src_var->misc_ ),
				(int)src_var->type_, ZnkPrim_e_StrAry, NULL );
		ZnkStrAry_copy( dst_var->prim_.u_.sda_, src_var->prim_.u_.sda_ );
		ZnkVarpAry_push_bk( dst_vars, dst_var );
	}
}

void
ZnkHtpHdrs_scanContentTypeVal( const char* content_type_val, ZnkStr boundary_ans )
{
	if( boundary_ans && ZnkS_isBegin( content_type_val, "multipart/form-data" ) ){
		const char* p = content_type_val + Znk_strlen_literal( "multipart/form-data" );
		p = strstr( p, "boundary=" );
		if( p ){
			const char* q;
			/* boundary found */
			p += Znk_strlen_literal( "boundary=" );
			q = strchr( p, ';' );
			if( q ){
				ZnkStr_append( boundary_ans, p, q-p );
			} else {
				ZnkStr_add( boundary_ans, p );
			}
		}
	}
}

Znk_INLINE void
appendBfr_byCStr( ZnkBfr bfr, const char* cstr )
{
	const size_t leng = strlen( cstr );
	ZnkBfr_append_dfr( bfr, (uint8_t*)cstr, leng );
}

#define appendBfr_byCStr_literal(  bfr, cstr_literal ) \
	ZnkBfr_append_dfr( bfr, (uint8_t*)cstr_literal, Znk_strlen_literal(cstr_literal) )

void
ZnkHtpHdrs_extendToStream( const ZnkStrAry hdr1st, const ZnkVarpAry vars,
		ZnkBfr bfr, bool is_terminate_hdrs_rn )
{
	/* First Line(Request or Status Line) */
	if( hdr1st ){
		appendBfr_byCStr( bfr, ZnkStrAry_at_cstr( hdr1st, 0 ) );
		appendBfr_byCStr_literal( bfr, " " );
		appendBfr_byCStr( bfr, ZnkStrAry_at_cstr( hdr1st, 1 ) );
		appendBfr_byCStr_literal( bfr, " " );
		appendBfr_byCStr( bfr, ZnkStrAry_at_cstr( hdr1st, 2 ) );
		appendBfr_byCStr_literal( bfr, "\r\n" );
	}

	/* Header Variables */
	if( vars ){
		const size_t size = ZnkVarpAry_size( vars );
		size_t idx;
		const char* key;
		const char* val;
		size_t val_size;
		size_t val_idx;
		ZnkVarp var;
		for( idx=0; idx<size; ++idx ){
			var = ZnkVarpAry_at( vars, idx );
			key = ZnkHtpHdrs_key_cstr( var );
			val_size = ZnkHtpHdrs_val_size( var );
			for( val_idx=0; val_idx<val_size; ++val_idx ){
				val = ZnkHtpHdrs_val_cstr( var, val_idx );
				appendBfr_byCStr( bfr, key );
				appendBfr_byCStr_literal( bfr, ": " );
				appendBfr_byCStr( bfr, val );
				appendBfr_byCStr_literal( bfr, "\r\n" );
			}
		}
	}
	if( is_terminate_hdrs_rn ){
		appendBfr_byCStr_literal( bfr, "\r\n" );
	}
}
