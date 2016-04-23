#ifndef INCLUDE_GUARD__Znk_htp_hdrs_h__
#define INCLUDE_GUARD__Znk_htp_hdrs_h__

#include <Znk_str.h>
#include <Znk_s_base.h>
#include <Znk_varp_dary.h>
#include <assert.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 ZnkHtpReqMethod_e_Unknown=0
	,ZnkHtpReqMethod_e_GET
	,ZnkHtpReqMethod_e_POST
	,ZnkHtpReqMethod_e_HEAD
	,ZnkHtpReqMethod_e_PUT
	,ZnkHtpReqMethod_e_DELETE
	,ZnkHtpReqMethod_e_CONNECT
	,ZnkHtpReqMethod_e_OPTIONS
	,ZnkHtpReqMethod_e_TRACE
} ZnkHtpReqMethod;

typedef enum {
	 ZnkHtpPostVar_e_None=0 /* text/plainと同等として扱う */
	,ZnkHtpPostVar_e_BinaryData
} ZnkHtpPostVarType;

Znk_INLINE const char*
ZnkHtpReqMethod_getCStr( ZnkHtpReqMethod req_method )
{
	const char* str = Znk_TO_STR( ZnkHtpReqMethod_e_Unknown );
	switch( req_method ){
	case Znk_BIND_STR( : str=, ZnkHtpReqMethod_e_GET );     break;
	case Znk_BIND_STR( : str=, ZnkHtpReqMethod_e_POST );    break;
	case Znk_BIND_STR( : str=, ZnkHtpReqMethod_e_HEAD );    break;
	case Znk_BIND_STR( : str=, ZnkHtpReqMethod_e_PUT );     break;
	case Znk_BIND_STR( : str=, ZnkHtpReqMethod_e_DELETE );  break;
	case Znk_BIND_STR( : str=, ZnkHtpReqMethod_e_CONNECT ); break;
	case Znk_BIND_STR( : str=, ZnkHtpReqMethod_e_OPTIONS ); break;
	case Znk_BIND_STR( : str=, ZnkHtpReqMethod_e_TRACE );   break;
	default: break;
	}
	return str + Znk_strlen_literal( "ZnkHtpReqMethod_e_" );
}


typedef struct ZnkHtpHdrs_tag {
	ZnkStrDAry  hdr1st_;
	ZnkVarpDAry vars_;
} *ZnkHtpHdrs;

Znk_INLINE ZnkStr
ZnkHtpHdrs_key( const ZnkVarp varp )
{
	return varp->name_;
}
Znk_INLINE ZnkStr
ZnkHtpHdrs_val( const ZnkVarp varp, size_t idx )
{
	const size_t size = ZnkStrDAry_size( varp->prim_.u_.sda_ );
	Znk_UNUSED( size );
	assert( idx < size );
	return ZnkStrDAry_at( varp->prim_.u_.sda_, idx );
}
Znk_INLINE const char*
ZnkHtpHdrs_key_cstr( const ZnkVarp varp )
{
	return ZnkStr_cstr( varp->name_ );
}
Znk_INLINE const char*
ZnkHtpHdrs_val_cstr( const ZnkVarp varp, size_t idx )
{
	const size_t size = ZnkStrDAry_size( varp->prim_.u_.sda_ );
	ZnkStr str;
	Znk_UNUSED( size );
	assert( idx < size );
	str = ZnkStrDAry_at( varp->prim_.u_.sda_, idx );
	return ZnkStr_cstr( str );
}
Znk_INLINE size_t
ZnkHtpHdrs_val_size( const ZnkVarp varp )
{
	return ZnkStrDAry_size( varp->prim_.u_.sda_ );
}

Znk_INLINE void
ZnkHtpHdrs_compose( ZnkHtpHdrs info ){
	info->hdr1st_ = ZnkStrDAry_create( true );
	info->vars_   = ZnkVarpDAry_create( true );
}
Znk_INLINE void
ZnkHtpHdrs_dispose( ZnkHtpHdrs info ){
	if( info ){
		ZnkStrDAry_destroy( info->hdr1st_ );
		ZnkVarpDAry_destroy( info->vars_ );
	}
}
Znk_INLINE void
ZnkHtpHdrs_clear( ZnkHtpHdrs info ){
	ZnkStrDAry_clear( info->hdr1st_ );
	ZnkVarpDAry_clear( info->vars_ );
}

void
ZnkHtpHdrs_registHdr1st( const ZnkStrDAry hdr1st, const char* first_line, size_t first_line_leng );

Znk_INLINE ZnkVarp
ZnkHtpHdrs_find( const ZnkVarpDAry vars, const char* query_name, size_t query_name_leng )
{
	/***
	 * 例えば、Content-Type や Content-typeなどとなっているサイトもある.
	 * これに対応するため、ここではCase系文字列比較関数を用いなければならない.
	 */
	return ZnkVarpDAry_find_byName( vars, query_name, query_name_leng, true );
}
#define ZnkHtpHdrs_find_literal( vars, query_name_literal ) \
	ZnkHtpHdrs_find( vars, query_name_literal, Znk_strlen( query_name_literal ) )

ZnkVarp
ZnkHtpHdrs_regist( ZnkVarpDAry vars,
		const char* key, size_t key_leng,
		const char* val, size_t val_leng );
ZnkVarp
ZnkHtpHdrs_regist_byLine( ZnkVarpDAry vars, const char* hdr_line, size_t hdr_line_leng );

Znk_INLINE ZnkVarp
ZnkHtpHdrs_registHost( ZnkVarpDAry vars, const char* val, size_t val_leng )
{
	ZnkSRef sref = { 0 };
	ZnkSRef_set_literal( &sref, "Host" );
	return ZnkHtpHdrs_regist( vars, sref.cstr_, sref.leng_, val, val_leng );
}
Znk_INLINE ZnkVarp
ZnkHtpHdrs_registUserAgent( ZnkVarpDAry vars, const char* val, size_t val_leng )
{
	ZnkSRef sref = { 0 };
	ZnkSRef_set_literal( &sref, "User-Agent" );
	return ZnkHtpHdrs_regist( vars, sref.cstr_, sref.leng_, val, val_leng );
}
Znk_INLINE ZnkVarp
ZnkHtpHdrs_registReferer( ZnkVarpDAry vars, const char* val, size_t val_leng )
{
	ZnkSRef sref = { 0 };
	ZnkSRef_set_literal( &sref, "Referer" );
	return ZnkHtpHdrs_regist( vars, sref.cstr_, sref.leng_, val, val_leng );
}
ZnkVarp
ZnkHtpHdrs_registCookie( ZnkVarpDAry vars, const ZnkVarpDAry cookie );

bool
ZnkHtpHdrs_erase( const ZnkVarpDAry vars, const char* query_name );

void
ZnkHtpHdrs_copyVars( ZnkVarpDAry dst_vars, const ZnkVarpDAry src_vars );

const char*
ZnkHtpHdrs_scanContentType( const ZnkVarpDAry vars, ZnkStr boundary_ans );

void
ZnkHtpHdrs_extendToStream( const ZnkStrDAry hdr1st, const ZnkVarpDAry vars,
		ZnkBfr bfr, bool is_terminate_hdrs_rn );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
