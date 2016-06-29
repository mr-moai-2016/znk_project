#ifndef INCLUDE_GUARD__Znk_htp_hdrs_h__
#define INCLUDE_GUARD__Znk_htp_hdrs_h__

#include <Znk_str.h>
#include <Znk_s_base.h>
#include <Znk_varp_ary.h>
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
	ZnkStrAry  hdr1st_;
	ZnkVarpAry vars_;
} *ZnkHtpHdrs;

Znk_INLINE ZnkStr
ZnkHtpHdrs_key( const ZnkVarp varp )
{
	return varp->name_;
}
Znk_INLINE ZnkStr
ZnkHtpHdrs_val( const ZnkVarp varp, size_t idx )
{
	const size_t size = ZnkStrAry_size( varp->prim_.u_.sda_ );
	Znk_UNUSED( size );
	assert( idx < size );
	return ZnkStrAry_at( varp->prim_.u_.sda_, idx );
}
Znk_INLINE const char*
ZnkHtpHdrs_key_cstr( const ZnkVarp varp )
{
	return ZnkStr_cstr( varp->name_ );
}
Znk_INLINE const char*
ZnkHtpHdrs_val_cstr( const ZnkVarp varp, size_t idx )
{
	const size_t size = ZnkStrAry_size( varp->prim_.u_.sda_ );
	ZnkStr str;
	Znk_UNUSED( size );
	assert( idx < size );
	str = ZnkStrAry_at( varp->prim_.u_.sda_, idx );
	return ZnkStr_cstr( str );
}
Znk_INLINE size_t
ZnkHtpHdrs_val_size( const ZnkVarp varp )
{
	return ZnkStrAry_size( varp->prim_.u_.sda_ );
}

Znk_INLINE void
ZnkHtpHdrs_compose( ZnkHtpHdrs info ){
	info->hdr1st_ = ZnkStrAry_create( true );
	info->vars_   = ZnkVarpAry_create( true );
}
Znk_INLINE void
ZnkHtpHdrs_dispose( ZnkHtpHdrs info ){
	if( info ){
		ZnkStrAry_destroy( info->hdr1st_ );
		ZnkVarpAry_destroy( info->vars_ );
	}
}
Znk_INLINE void
ZnkHtpHdrs_clear( ZnkHtpHdrs info ){
	ZnkStrAry_clear( info->hdr1st_ );
	ZnkVarpAry_clear( info->vars_ );
}

void
ZnkHtpHdrs_registHdr1st( const ZnkStrAry hdr1st, const char* first_line, size_t first_line_leng );

Znk_INLINE ZnkVarp
ZnkHtpHdrs_find( const ZnkVarpAry vars, const char* query_name, size_t query_name_leng )
{
	/***
	 * 例えば、Content-Type や Content-typeなどとなっているサイトもある.
	 * これに対応するため、ここではCase系文字列比較関数を用いなければならない.
	 */
	return ZnkVarpAry_find_byName( vars, query_name, query_name_leng, true );
}
#define ZnkHtpHdrs_find_literal( vars, query_name_literal ) \
	ZnkHtpHdrs_find( vars, query_name_literal, Znk_strlen( query_name_literal ) )

ZnkVarp
ZnkHtpHdrs_regist( ZnkVarpAry vars,
		const char* key, size_t key_leng,
		const char* val, size_t val_leng );
ZnkVarp
ZnkHtpHdrs_regist_byLine( ZnkVarpAry vars, const char* hdr_line, size_t hdr_line_leng );

Znk_INLINE ZnkVarp
ZnkHtpHdrs_registHost( ZnkVarpAry vars, const char* val, size_t val_leng )
{
	ZnkSRef sref = { 0 };
	ZnkSRef_set_literal( &sref, "Host" );
	return ZnkHtpHdrs_regist( vars, sref.cstr_, sref.leng_, val, val_leng );
}
Znk_INLINE ZnkVarp
ZnkHtpHdrs_registUserAgent( ZnkVarpAry vars, const char* val, size_t val_leng )
{
	ZnkSRef sref = { 0 };
	ZnkSRef_set_literal( &sref, "User-Agent" );
	return ZnkHtpHdrs_regist( vars, sref.cstr_, sref.leng_, val, val_leng );
}
Znk_INLINE ZnkVarp
ZnkHtpHdrs_registReferer( ZnkVarpAry vars, const char* val, size_t val_leng )
{
	ZnkSRef sref = { 0 };
	ZnkSRef_set_literal( &sref, "Referer" );
	return ZnkHtpHdrs_regist( vars, sref.cstr_, sref.leng_, val, val_leng );
}

#if 0
/***
 * Deprecated.
 * 次期メジャーリリースでは削除の予定.
 * 替わりに ZnkCookie_extend_toCookieStatement を用いること.
 */
ZnkVarp
ZnkHtpHdrs_registCookie( ZnkVarpAry vars, const ZnkVarpAry cookie );
#endif

bool
ZnkHtpHdrs_erase( const ZnkVarpAry vars, const char* query_name );

void
ZnkHtpHdrs_copyVars( ZnkVarpAry dst_vars, const ZnkVarpAry src_vars );

const char*
ZnkHtpHdrs_scanContentType( const ZnkVarpAry vars, ZnkStr boundary_ans );

void
ZnkHtpHdrs_extendToStream( const ZnkStrAry hdr1st, const ZnkVarpAry vars,
		ZnkBfr bfr, bool is_terminate_hdrs_rn );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
