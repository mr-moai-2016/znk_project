#include "Znk_htp_hdrs.h"
#include "Znk_s_base.h"
#include "Znk_varp_dary.h"
#include <string.h>

void
ZnkHtpHdrs_registHdr1st( const ZnkStrDAry hdr1st, const char* first_line, size_t first_line_leng )
{
	size_t arg_leng = 0;
	size_t arg_pos = 0;
	const char* p = first_line;
	const char* q = NULL;
	const char* end = first_line + first_line_leng;

	ZnkStrDAry_resize( hdr1st, 3 );
	ZnkStrDAry_set( hdr1st, 0, ZnkStr_new("") );
	ZnkStrDAry_set( hdr1st, 1, ZnkStr_new("") );
	ZnkStrDAry_set( hdr1st, 2, ZnkStr_new("") );

	arg_pos = ZnkS_lfind_arg( p, 0, (size_t)(end-p), 0, &arg_leng, " \t\r\n", 4 );
	p += arg_pos;
	ZnkStr_assign( ZnkStrDAry_at( hdr1st, 0 ), 0, p, arg_leng );
	p += arg_leng;

	arg_pos = ZnkS_lfind_arg( p, 0, (size_t)(end-p), 0, &arg_leng, " \t\r\n", 4 );
	p += arg_pos;
	ZnkStr_assign( ZnkStrDAry_at( hdr1st, 1 ), 0, p, arg_leng );
	p += arg_leng;

	arg_pos = ZnkS_lfind_arg( p, 0, (size_t)(end-p), 0, &arg_leng, " \t\r\n", 4 );
	p += arg_pos;
	q = strchr( p, '\r' );
	if( q ){ end = q; }
	ZnkStr_assign( ZnkStrDAry_at( hdr1st, 2 ), 0, p, (size_t)(end-p) );
}
ZnkVarp
ZnkHtpHdrs_regist( ZnkVarpDAry vars,
		const char* key, size_t key_leng,
		const char* val, size_t val_leng )
{
	/***
	 * SetCookie: などのように複数指定可能なDirectiveも存在する.
	 * 変数の値をZnkStrDAryとすることによって一つのkey(例えばSetCookie)に対して
	 * 複数の値を保持できるようにする.
	 */
	ZnkVarp varp = ZnkHtpHdrs_find( vars, key, key_leng );
	if( varp == NULL ){
		varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_StrDAry );
		ZnkStr_assign( varp->name_, 0, key, key_leng );
		ZnkVarpDAry_push_bk( vars, varp );
	}
	ZnkStrDAry_push_bk_cstr( varp->prim_.u_.sda_, val, val_leng );
	return varp;
}
ZnkVarp
ZnkHtpHdrs_regist_byLine( ZnkVarpDAry vars, const char* hdr_line, size_t hdr_line_leng )
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

	varp = ZnkVarpDAry_find_byName( vars,
			hdr_line+key_begin, key_end-key_begin, use_eqCase );
	if( varp == NULL ){
		/* 新規追加 */
		varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_StrDAry );
		ZnkStr_assign( varp->name_, 0, hdr_line+key_begin, key_end-key_begin );
		ZnkVarpDAry_push_bk( vars, varp );
	}
	ZnkStrDAry_push_bk_cstr( varp->prim_.u_.sda_,
			hdr_line+val_begin, val_end-val_begin );
	return varp;
}
ZnkVarp
ZnkHtpHdrs_registCookie( ZnkVarpDAry vars, const ZnkVarpDAry cookie )
{
	ZnkVarp varp = NULL;
	ZnkStr str = ZnkStr_new( "" );
	ZnkSRef sref = { 0 };
	size_t size;
	size_t idx;
	const char* key = "";
	const char* val = "";

	size = ZnkVarpDAry_size( cookie );
	for( idx=0; idx<size; ++idx ){
		varp = ZnkVarpDAry_at( cookie, idx );
		key = ZnkStr_cstr( varp->name_ );
		val = ZnkVar_cstr( varp );
		ZnkStr_addf( str, "%s=%s", key, val );
		if( idx != size-1 ){
			ZnkStr_add( str, "; " );
		}
	}

	ZnkSRef_set_literal( &sref, "Cookie" );
	varp = ZnkHtpHdrs_regist( vars, sref.cstr_, sref.leng_,
			ZnkStr_cstr(str), ZnkStr_leng(str) );

	ZnkStr_delete( str );
	return varp;
}
bool
ZnkHtpHdrs_erase( const ZnkVarpDAry vars, const char* query_key )
{
	const size_t size = ZnkVarpDAry_size( vars );
	size_t idx;
	ZnkVarp varp;
	for( idx=0; idx<size; ++idx ){
		varp = ZnkVarpDAry_at( vars, idx );
		/***
		 * 例えば、Content-Type や Content-typeなどとなっているサイトもある.
		 * これに対応するため、ここではCase系文字列比較関数を用いなければならない.
		 */
		if( ZnkS_eqCase( ZnkStr_cstr(varp->name_), query_key ) ){
			/* found */
			ZnkVarpDAry_erase_byIdx( vars, idx );
			return true;
		}
	}
	return false;
}
void
ZnkHtpHdrs_copyVars( ZnkVarpDAry dst_vars, const ZnkVarpDAry src_vars )
{
	const size_t size = ZnkVarpDAry_size( src_vars );
	size_t idx;
	ZnkVarp src_var;
	ZnkVarp dst_var;
	ZnkVarpDAry_clear( dst_vars );
	for( idx=0; idx<size; ++idx ){
		src_var = ZnkVarpDAry_at( src_vars, idx );
		dst_var = ZnkVarp_create( 
				ZnkStr_cstr( src_var->name_ ), ZnkStr_cstr( src_var->filename_ ),
				(int)src_var->type_, ZnkPrim_e_StrDAry );
		ZnkStrDAry_copy( dst_var->prim_.u_.sda_, src_var->prim_.u_.sda_ );
		ZnkVarpDAry_push_bk( dst_vars, dst_var );
	}
}

const char*
ZnkHtpHdrs_scanContentType( const ZnkVarpDAry vars, ZnkStr boundary_ans )
{
	ZnkVarp varp = ZnkHtpHdrs_find_literal( vars, "Content-Type" );
	if( varp ){
		/* found */
		const char* val = ZnkHtpHdrs_val_cstr( varp, 0 );
		if( ZnkS_isBegin( val, "multipart/form-data" ) ){
			const char* p = val + Znk_strlen_literal( "multipart/form-data" );
			p = strstr( p, "boundary=" );
			if( p ){
				/* boundary found */
				p += Znk_strlen_literal( "boundary=" );
				ZnkStr_add( boundary_ans, p );
			}
		}
		return val;
	}
	/* not found */
	return NULL;
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
ZnkHtpHdrs_extendToStream( const ZnkStrDAry hdr1st, const ZnkVarpDAry vars,
		ZnkBfr bfr, bool is_terminate_hdrs_rn )
{
	/* First Line(Request or Status Line) */
	if( hdr1st ){
		appendBfr_byCStr( bfr, ZnkStrDAry_at_cstr( hdr1st, 0 ) );
		appendBfr_byCStr_literal( bfr, " " );
		appendBfr_byCStr( bfr, ZnkStrDAry_at_cstr( hdr1st, 1 ) );
		appendBfr_byCStr_literal( bfr, " " );
		appendBfr_byCStr( bfr, ZnkStrDAry_at_cstr( hdr1st, 2 ) );
		appendBfr_byCStr_literal( bfr, "\r\n" );
	}

	/* Header Variables */
	if( vars ){
		const size_t size = ZnkVarpDAry_size( vars );
		size_t idx;
		const char* key;
		const char* val;
		size_t val_size;
		size_t val_idx;
		ZnkVarp var;
		for( idx=0; idx<size; ++idx ){
			var = ZnkVarpDAry_at( vars, idx );
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
