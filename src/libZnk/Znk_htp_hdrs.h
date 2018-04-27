#ifndef INCLUDE_GUARD__Znk_htp_hdrs_h__
#define INCLUDE_GUARD__Znk_htp_hdrs_h__

#include <Znk_str.h>
#include <Znk_s_base.h>
#include <Znk_varp_ary.h>
#include <Znk_missing_libc.h>
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
Znk_INLINE ZnkHtpReqMethod
ZnkHtpReqMethod_getType_fromCStr( const char* req_method_cstr )
{
	switch( req_method_cstr[0] ){
	case 'G':
		if( ZnkS_eq( req_method_cstr, "GET" ) ){
			return ZnkHtpReqMethod_e_GET;
		}
		break;
	case 'P':
		if( ZnkS_eq( req_method_cstr, "POST" ) ){
			return ZnkHtpReqMethod_e_POST;
		} else if( ZnkS_eq( req_method_cstr, "PUT" ) ){
			return ZnkHtpReqMethod_e_PUT;
		}
		break;
	case 'H':
		if( ZnkS_eq( req_method_cstr, "HEAD" ) ){
			return ZnkHtpReqMethod_e_HEAD;
		}
		break;
	case 'D':
		if( ZnkS_eq( req_method_cstr, "DELETE" ) ){
			return ZnkHtpReqMethod_e_DELETE;
		}
		break;
	case 'C':
		if( ZnkS_eq( req_method_cstr, "CONNECT" ) ){
			return ZnkHtpReqMethod_e_CONNECT;
		}
		break;
	case 'O':
		if( ZnkS_eq( req_method_cstr, "OPTIONS" ) ){
			return ZnkHtpReqMethod_e_OPTIONS;
		}
		break;
	case 'T':
		if( ZnkS_eq( req_method_cstr, "TRACE" ) ){
			return ZnkHtpReqMethod_e_TRACE;
		}
		break;
	default:
		break;
	}
	return ZnkHtpReqMethod_e_Unknown;
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
ZnkHtpHdrs_set_cstr( ZnkVarp varp, size_t idx, const char* val )
{
	ZnkStr_set( ZnkHtpHdrs_val( varp, idx ), val );
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
	ZnkHtpHdrs_find( vars, query_name_literal, Znk_strlen_literal( query_name_literal ) )

/**
 * @param is_unique:
 *  SetCookie: などのように複数指定可能なDirectiveも存在する.
 *  従って、内部変数の値はZnkStrAry、つまり文字列の配列としてある.
 *  こうすることによって一つのkey(例えばSetCookie)に対して
 *  複数の値を保持できるようにする.
 *  しかし一方で、User-Agentのように明らかに単独で存在すべきDirectiveも存在する.
 *  is_uniqueフラグをtrueにするとそのようなDirectiveを確実にUniqueに登録できる.
 *  (つまり既に存在している文字列配列値は一旦全消去され、要素数1の文字列配列として再構成される)
 */
ZnkVarp
ZnkHtpHdrs_regist( ZnkVarpAry vars,
		const char* key, size_t key_leng,
		const char* val, size_t val_leng,
		bool is_unique );
ZnkVarp
ZnkHtpHdrs_regist_byLine( ZnkVarpAry vars, const char* hdr_line, size_t hdr_line_leng );

Znk_INLINE ZnkVarp
ZnkHtpHdrs_registHost( ZnkVarpAry vars, const char* val, size_t val_leng )
{
	ZnkSRef sref = { 0 };
	ZnkSRef_set_literal( &sref, "Host" );
	return ZnkHtpHdrs_regist( vars, sref.cstr_, sref.leng_, val, val_leng, true );
}
Znk_INLINE ZnkVarp
ZnkHtpHdrs_registUserAgent( ZnkVarpAry vars, const char* val, size_t val_leng )
{
	ZnkSRef sref = { 0 };
	ZnkSRef_set_literal( &sref, "User-Agent" );
	return ZnkHtpHdrs_regist( vars, sref.cstr_, sref.leng_, val, val_leng, true );
}
Znk_INLINE ZnkVarp
ZnkHtpHdrs_registReferer( ZnkVarpAry vars, const char* val, size_t val_leng )
{
	ZnkSRef sref = { 0 };
	ZnkSRef_set_literal( &sref, "Referer" );
	return ZnkHtpHdrs_regist( vars, sref.cstr_, sref.leng_, val, val_leng, true );
}
Znk_INLINE ZnkVarp
ZnkHtpHdrs_registContentLength( ZnkVarpAry vars, size_t content_length )
{
	char val_str[ 256 ] = "";
	ZnkSRef sref = { 0 };
	ZnkSRef_set_literal( &sref, "Content-Length" );
	Znk_snprintf( val_str, sizeof(val_str), "%zu", content_length );
	return ZnkHtpHdrs_regist( vars, sref.cstr_, sref.leng_, val_str, Znk_strlen(val_str), true );
}

bool
ZnkHtpHdrs_erase( const ZnkVarpAry vars, const char* query_name );

void
ZnkHtpHdrs_copyVars( ZnkVarpAry dst_vars, const ZnkVarpAry src_vars );

void
ZnkHtpHdrs_scanContentTypeVal( const char* content_type_val, ZnkStr boundary_ans );
Znk_INLINE const char*
ZnkHtpHdrs_scanContentType( const ZnkVarpAry vars, ZnkStr boundary_ans )
{
	ZnkVarp varp = ZnkHtpHdrs_find_literal( vars, "Content-Type" );
	if( varp ){
		const char* content_type_val = ZnkHtpHdrs_val_cstr( varp, 0 );
		ZnkHtpHdrs_scanContentTypeVal( content_type_val, boundary_ans );
		return content_type_val;
	}
	return NULL;
}

void
ZnkHtpHdrs_extendToStream( const ZnkStrAry hdr1st, const ZnkVarpAry vars,
		ZnkBfr bfr, bool is_terminate_hdrs_rn );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
