#include "Rano_htp_modifier.h"
#include <Znk_str_ary.h>
#include <Znk_s_base.h>
#include <Znk_rand.h>
#include <Znk_htp_hdrs.h>
#include <string.h>
#include <ctype.h>


static bool
replaceContentTypeBoundary( ZnkVarpAry vars, const char* ct_boundary )
{
	ZnkVarp varp = ZnkHtpHdrs_find_literal( vars, "Content-Type" );
	if( varp ){
		/* found */
		ZnkStr str = ZnkHtpHdrs_val( varp, 0 );
		if( ZnkStr_isBegin( str, "multipart/form-data" ) ){
			const char* begin = ZnkStr_cstr(str);
			const char* p = begin + Znk_strlen_literal( "multipart/form-data" );
			p = strstr( p, "boundary=" );
			if( p ){
				const char* q;
				/* boundary found */
				p += Znk_strlen_literal( "boundary=" );
				q = strchr( p, ';' );
				if( q ){
					ZnkStr_replace( str, p-begin, q-p, ct_boundary, Znk_NPOS );
				} else {
					ZnkStr_replace( str, p-begin, Znk_NPOS, ct_boundary, Znk_NPOS );
				}
				return true;
			}
		}
	}
	/* not found */
	return false;
}
static bool
replaceHdrVarVal( ZnkVarpAry vars, const char* var_name, const char* val, bool intern )
{
	ZnkVarp varp = ZnkHtpHdrs_find( vars, var_name, Znk_NPOS );
	if( varp ){
		/* found */
		ZnkStr str = ZnkHtpHdrs_val( varp, 0 );
		ZnkStr_set( str, val );
		return true;
	} else if( intern ){
		/* not found, but we regist new var. */
		ZnkHtpHdrs_regist( vars, var_name, Znk_NPOS, val, Znk_NPOS, false );
		return true;
	}
	/* not found */
	return false;
}


typedef enum {
	 HtpHdrUA_e_Unknown=0
	,HtpHdrUA_e_IE7
	,HtpHdrUA_e_IE8
	,HtpHdrUA_e_IE
	,HtpHdrUA_e_Firefox
	,HtpHdrUA_e_Firefox2
	,HtpHdrUA_e_Firefox3
	,HtpHdrUA_e_AppleWebKit
	,HtpHdrUA_e_Presto
}HtpHdrUA;

typedef enum {
	 HtpHdrOS_e_Unknown=0
	,HtpHdrOS_e_Windows
	,HtpHdrOS_e_Linux
	,HtpHdrOS_e_MacOSX
}HtpHdrOS;

typedef bool (*FuncT_isBoundary)( const char* ct_boundary );
typedef void (*FuncT_makeBoundary)( ZnkStr ct_boundary );

typedef struct {
	FuncT_isBoundary   isBoundary_;
	FuncT_makeBoundary makeBoundary_;
} BoundaryModifier;



static bool
isBoundary_IE( const char* ct_boundary )
{
	static const size_t ubar_leng = 27;
	const size_t leng = strlen( ct_boundary );
	if( leng >= ubar_leng+12 ){
		if( ct_boundary[ ubar_leng ] == '7' ){
			if( ct_boundary[ ubar_leng+1 ] == 'd' || ct_boundary[ ubar_leng+1 ] == 'e' ){
				size_t idx;
				for( idx=ubar_leng+2; idx<leng; ++idx ){
					switch( ct_boundary[ idx ] ){
					case '0': case '1': case '2': case '3': case '4':
					case '5': case '6': case '7': case '8': case '9':
					case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
						break;
					default:
						return false;
					}
				}
				return true;
			}
		}
	}
	return false;
}
static void
makeBoundary_IE( ZnkStr ct_boundary )
{
	const int c = ZnkRand_getRandI( 'd', 'e'+1 );
	const int i = ZnkRand_getRandI( 0x10, 0x10000 );
	const unsigned long ul = ZnkRand_getRandUL( 0xffffffff );
	ZnkStr_setf2( ct_boundary, "---------------------------7%c%x%08x", c, i, ul );
}

static bool
isBoundary_Firefox( const char* ct_boundary )
{
	static const size_t ubar_leng = 27;
	const size_t leng = strlen( ct_boundary );
	if( leng >= ubar_leng+11 ){
		size_t idx;
		for( idx=ubar_leng; idx<leng; ++idx ){
			switch( ct_boundary[ idx ] ){
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				break;
			default:
				return false;
			}
		}
		return true;
	}
	return false;
}
static void
makeBoundary_Firefox( ZnkStr ct_boundary )
{
	const int i = ZnkRand_getRandI( 1000, 10000000 ); /* 3-7桁 */
	const unsigned long ul = ZnkRand_getRandUL( 100000000 ); /* 8桁 */
	ZnkStr_setf2( ct_boundary, "---------------------------%u%08u", i, ul );
}

static bool
isBoundary_FirefoxMac( const char* ct_boundary )
{
	static const size_t ubar_leng = 27;
	const size_t leng = strlen( ct_boundary );
	if( leng >= ubar_leng+27 ){
		size_t idx;
		for( idx=ubar_leng; idx<leng; ++idx ){
			switch( ct_boundary[ idx ] ){
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				break;
			default:
				return false;
			}
		}
		return true;
	}
	return false;
}
static void
makeBoundary_FirefoxMac( ZnkStr ct_boundary )
{
	const int i = ZnkRand_getRandI( 10, 100000 ); /* 2-5桁 */
	const unsigned long ul1 = ZnkRand_getRandUL( 100000000 ); /* 8桁 */
	const unsigned long ul2 = ZnkRand_getRandUL( 100000000 ); /* 8桁 */
	const unsigned long ul3 = ZnkRand_getRandUL( 100000000 ); /* 8桁 */
	ZnkStr_setf2( ct_boundary, "---------------------------1%u%08u%08u%08u", i, ul1, ul2, ul3 );
}

static bool
isBoundary_AppleWebKit( const char* ct_boundary )
{
	static const size_t ubar_leng = 22;
	const size_t leng = strlen( ct_boundary );
	if( leng >= ubar_leng+16 ){
		size_t idx;
		for( idx=ubar_leng; idx<leng; ++idx ){
			if( !isalnum( ct_boundary[idx] ) ){
				return false;
			}
		}
		return true;
	}
	return false;
}
static void
makeBoundary_AppleWebKit( ZnkStr ct_boundary )
{
	static const char st_basis_62[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	size_t i,j;
	unsigned long ul_mod;

	ZnkStr_set( ct_boundary, "----WebKitFormBoundary" );
	for( i=0; i<4; ++i ){
		unsigned long ul = ZnkRand_getRandUL( 14776336 ); /* 14776336 = 62^4 */
		for( j=0; j<4; ++j ){
			ul_mod = ul%62;
			ul = ul/62;
			ZnkStr_add_c( ct_boundary, st_basis_62[ ul_mod ] );
		}
	}
}

static bool
isBoundary_Presto( const char* ct_boundary )
{
	static const size_t ubar_leng = 10;
	const size_t leng = strlen( ct_boundary );
	if( leng >= ubar_leng+22 ){
		size_t idx;
		for( idx=ubar_leng; idx<leng; ++idx ){
			if( !isalnum( ct_boundary[idx] ) ){
				return false;
			}
		}
		return true;
	}
	return false;
}
static void
makeBoundary_Presto( ZnkStr ct_boundary )
{
	static const char st_basis_62[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	size_t i,j;
	unsigned long ul_mod;

	ZnkStr_set( ct_boundary, "----------" );
	for( i=0; i<5; ++i ){
		unsigned long ul = ZnkRand_getRandUL( 14776336 ); /* 14776336 = 62^4 */
		for( j=0; j<4; ++j ){
			ul_mod = ul%62;
			ul = ul/62;
			ZnkStr_add_c( ct_boundary, st_basis_62[ ul_mod ] );
		}
	}
	{
		unsigned long ul = ZnkRand_getRandUL( 14776336 ); /* 14776336 = 62^4 */
		for( j=0; j<2; ++j ){
			ul_mod = ul%62;
			ul = ul/62;
			ZnkStr_add_c( ct_boundary, st_basis_62[ ul_mod ] );
		}
	}
}


static const BoundaryModifier*
getBoundaryModifier( HtpHdrUA hdr_type, HtpHdrOS hdr_os )
{
	switch( hdr_type ){
	case HtpHdrUA_e_IE7:
	case HtpHdrUA_e_IE8:
	case HtpHdrUA_e_IE:
	{
		static const BoundaryModifier st_bm = { isBoundary_IE, makeBoundary_IE };
		return &st_bm;
	}
	case HtpHdrUA_e_Firefox2:
	case HtpHdrUA_e_Firefox3:
	case HtpHdrUA_e_Firefox:
	{
		static const BoundaryModifier st_bm     = { isBoundary_Firefox,    makeBoundary_Firefox };
		static const BoundaryModifier st_bm_mac = { isBoundary_FirefoxMac, makeBoundary_FirefoxMac };
		return hdr_os == HtpHdrOS_e_MacOSX ? &st_bm_mac : &st_bm;
	}
	case HtpHdrUA_e_AppleWebKit:
	{
		static const BoundaryModifier st_bm = { isBoundary_AppleWebKit, makeBoundary_AppleWebKit };
		return &st_bm;
	}
	case HtpHdrUA_e_Presto:
	{
		static const BoundaryModifier st_bm = { isBoundary_Presto, makeBoundary_Presto };
		return &st_bm;
	}
	default:
		break;
	}
	return NULL;
}

static HtpHdrUA
getHtpHdrType_byUA( const char* ua )
{
	if( strstr( ua, "MSIE 7.0" ) ){
		return HtpHdrUA_e_IE7;
	} else if( strstr( ua, "MSIE 8.0" ) ){
		return HtpHdrUA_e_IE8;
	} else if( strstr( ua, "MSIE " ) || strstr( ua, "Trident/" ) ){
		return HtpHdrUA_e_IE;
	} else if( strstr( ua, "Firefox/2." ) ){
		return HtpHdrUA_e_Firefox2;
	} else if( strstr( ua, "Firefox/3." ) ){
		return HtpHdrUA_e_Firefox3;
	} else if( strstr( ua, "Firefox/" ) ){
		return HtpHdrUA_e_Firefox;
	} else if( strstr( ua, "AppleWebKit/" ) ){
		/* Chrome, Opera(blink), Safari */
		return HtpHdrUA_e_AppleWebKit;
	} else if( strstr( ua, "Presto/" ) ){
		return HtpHdrUA_e_Presto;
	} else {
		/* Others */
	}
	return HtpHdrUA_e_Unknown;
}
static HtpHdrOS
getHtpHdrOS_byUA( const char* ua )
{
	if( strstr( ua, "Windows " ) || strstr( ua,"Win 9x 4.90 " ) || strstr( ua, "Win98 " ) ){
		return HtpHdrOS_e_Windows;
	} else if( strstr( ua, "Linux " ) ){
		return HtpHdrOS_e_Linux;
	} else if( strstr( ua, "Mac OS X " ) ){
		return HtpHdrOS_e_MacOSX;
	}
	return HtpHdrOS_e_Unknown;
}

static const char*
getAcceptLanguage( HtpHdrUA hdr_type )
{
	switch( hdr_type ){
	case HtpHdrUA_e_IE7:         return "ja-JP";
	case HtpHdrUA_e_IE8:         return "ja-JP";
	case HtpHdrUA_e_IE:          return "ja-JP";
	case HtpHdrUA_e_Firefox2:    return "ja,en-us;q=0.7,en;q=0.3";
	case HtpHdrUA_e_Firefox3:    return "ja,en-us;q=0.7,en;q=0.3";
	case HtpHdrUA_e_Firefox:     return "ja,en-US;q=0.7,en;q=0.3";
	case HtpHdrUA_e_AppleWebKit: return "ja,en-US;q=0.8,en;q=0.6";
	case HtpHdrUA_e_Presto:      return "ja-JP,ja;q=0.9,en;q=0.8";
	default:
		break;
	}
	return NULL;
}
static const char*
getAccept( HtpHdrUA hdr_type )
{
	switch( hdr_type ){
	case HtpHdrUA_e_IE7:
		return "application/x-ms-application, image/jpeg, application/xaml+xml, image/gif, image/pjpeg, application/x-ms-xbap, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, */*";
	case HtpHdrUA_e_IE8:
	 	return "image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*";
	case HtpHdrUA_e_IE:
		return "text/html, application/xhtml+xml, */*";
	case HtpHdrUA_e_Firefox2:
		return "text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5";
	case HtpHdrUA_e_Firefox3:
	case HtpHdrUA_e_Firefox:
		return "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8";
	case HtpHdrUA_e_AppleWebKit:
		return "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8";
	case HtpHdrUA_e_Presto:
		return "text/html, application/xml;q=0.9, application/xhtml+xml, image/png, image/webp, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1";
	default:
		break;
	}
	return NULL;
}


static const char*
getAcceptCharset( HtpHdrUA hdr_type )
{
	switch( hdr_type ){
	case HtpHdrUA_e_Firefox2:
	case HtpHdrUA_e_Firefox3:
		return "Shift_JIS,utf-8;q=0.7,*;q=0.7";
	default:
		break;
	}
	return NULL;
}
static const char*
getConnection( HtpHdrUA hdr_type )
{
	switch( hdr_type ){
	case HtpHdrUA_e_IE7:
	case HtpHdrUA_e_IE8:
	case HtpHdrUA_e_IE:
	case HtpHdrUA_e_Presto:
		return "Keep-Alive";
	case HtpHdrUA_e_Firefox2:
	case HtpHdrUA_e_Firefox3:
	case HtpHdrUA_e_Firefox:
	case HtpHdrUA_e_AppleWebKit:
		return "keep-alive";
	default:
		break;
	}
	return NULL;
}

bool
RanoHtpModifier_modifySendHdrs( ZnkVarpAry hdr_vars, const char* ua, ZnkStr msg )
{
	ZnkStr        ct_boundary  = ZnkStr_new( "" );
	const char*   content_type = ZnkHtpHdrs_scanContentType( hdr_vars, ct_boundary );
	bool result = false;
	HtpHdrUA hdr_type = HtpHdrUA_e_Unknown;
	HtpHdrOS hdr_os   = HtpHdrOS_e_Unknown;
	const BoundaryModifier* bm = NULL;
	bool change_accept_lang = false;
	bool change_accept      = false;

	if( content_type ){
		hdr_type = getHtpHdrType_byUA( ua );
		hdr_os = getHtpHdrOS_byUA( ua );

		if( ZnkStr_empty(ct_boundary) ){
			/***
			 * ct_boundaryが空.
			 * つまりGET、HEAD、またはPOSTにおける application/x-www-form-urlencoded のケース.
			 */
			const char* val = NULL;

			/* この場合、同系統UAであるか否かの判定まで拘る意味はあまりないとみなして、
			 * とにかくreplaceHdrVarValを強制的に行う */

			val = getAcceptLanguage( hdr_type );
			if( val ){ replaceHdrVarVal( hdr_vars, "Accept-Language", val, true ); }

			val = getAccept( hdr_type );
			if( val ){ replaceHdrVarVal( hdr_vars, "Accept", val, true ); }

			val = getAcceptCharset( hdr_type );
			if( val ){ replaceHdrVarVal( hdr_vars, "Accept-Charset", val, true ); }

			val = getConnection( hdr_type );
			if( val ){
				if( !replaceHdrVarVal( hdr_vars, "Connection", val, false ) ){
					replaceHdrVarVal( hdr_vars, "Proxy-Connection", val, false );
				}
			}
		} else {
			/***
			 * ct_boundaryが空でない.
			 * つまりPOSTにおける multipart/form-data のケース.
			 */
			bm = getBoundaryModifier( hdr_type, hdr_os );
			if( bm && !bm->isBoundary_( ZnkStr_cstr(ct_boundary) ) ){
				/* srcとdstが異系統UAであると考えられる */
				bm->makeBoundary_( ct_boundary );
				result = replaceContentTypeBoundary( hdr_vars, ZnkStr_cstr(ct_boundary) );

				change_accept_lang = true;
				change_accept      = true;
			}

			/* srcとdstが同系統UAであっても以下のケースにおいてはreplaceHdrVarValを強制的に行う */
			switch( hdr_type ){
			case HtpHdrUA_e_Firefox2:
			case HtpHdrUA_e_Firefox3:
			case HtpHdrUA_e_Firefox:
				change_accept_lang = true;
				change_accept      = true;
				break;
			default:
				break;
			}

			if( change_accept_lang ){
				const char* val = getAcceptLanguage( hdr_type );
				if( val ){ replaceHdrVarVal( hdr_vars, "Accept-Language", val, true ); }
			}
			if( change_accept ){
				const char* val = getAccept( hdr_type );
				if( val ){ replaceHdrVarVal( hdr_vars, "Accept", val, true ); }
			}

			{
				const char* val = getAcceptCharset( hdr_type );
				if( val ){ replaceHdrVarVal( hdr_vars, "Accept-Charset", val, true ); }
			}
			{
				const char* val = getConnection( hdr_type );
				if( val ){
					if( !replaceHdrVarVal( hdr_vars, "Connection", val, false ) ){
						replaceHdrVarVal( hdr_vars, "Proxy-Connection", val, false );
					}
				}
			}
		}
	}

	ZnkStr_delete( ct_boundary );

	/* TODO: さらに項目によるプライオリティソート */
	return result;
}
