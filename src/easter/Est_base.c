#include <Est_base.h>
#include <Est_config.h>

#include <Rano_log.h>
#include <Rano_cgi_util.h>
#include <Rano_htp_boy.h>
#include <Rano_htp_modifier.h>
#include <Rano_file_info.h>

#include <Znk_s_base.h>
#include <Znk_htp_util.h>
#include <Znk_htp_hdrs.h>
#include <Znk_myf.h>
#include <Znk_cookie.h>
#include <Znk_varp_ary.h>
#include <Znk_stdc.h>
#include <Znk_dir.h>
#include <Znk_str_fio.h>
#include <Znk_str_ex.h>
#include <Znk_mem_find.h>
#include <Znk_date.h>
#include <Znk_dir_recursive.h>
#include <Znk_rand.h>


const char*
EstBase_getHostnameAndRequrp_fromEstVal( char* hostname, size_t hostname_size, ZnkStr req_urp, const char* src_url, bool* is_https )
{
	const char* p;
	const char* q;
	const char* src = src_url;

	ZnkHtpURL_unescape_toStr( req_urp, src, Znk_strlen(src) );

	if( is_https && ZnkS_isBegin( ZnkStr_cstr(req_urp), "https://" ) ){
		*is_https = true;
	}

	p = RanoCGIUtil_skipProtocolPrefix( ZnkStr_cstr(req_urp) );
	q = Znk_strchr( p, '/' );
	if( q ){
		ZnkS_copy( hostname, hostname_size, p, q-p );
		ZnkStr_set( req_urp, q );
	} else {
		ZnkS_copy( hostname, hostname_size, p, Znk_NPOS );
		ZnkStr_set( req_urp, "/" );
	}
	return src;
}

static void
initHtpHdr( ZnkHtpHdrs htp_hdrs, const char* hostname, const char* ua, ZnkVarpAry cookie, bool is_https, const char* explicit_referer )
{
	ZnkVarp varp;
	ZnkSRef sref = { 0 };
	const int rand_factor = ZnkRand_getRandI( 0, 100 );
	char referer_bur[ 512 ] = "";

	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Host",   Znk_strlen_literal("Host"),
			hostname, Znk_strlen(hostname), true );

	/* dummy(順番を維持するため) */
	ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"User-Agent", Znk_strlen_literal("User-Agent"),
			ua, Znk_strlen(ua), true );

	ZnkSRef_set_literal( &sref, "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" );
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Accept", Znk_strlen_literal("Accept"),
			sref.cstr_, sref.leng_, true );

	ZnkSRef_set_literal( &sref, "ja,en-US;q=0.7,en;q=0.3" );
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Accept-Language", Znk_strlen_literal("Accept-Language"),
			sref.cstr_, sref.leng_, true );

	ZnkSRef_set_literal( &sref, "gzip, deflate" );
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Accept-Encoding", Znk_strlen_literal("Accept-Encoding"),
			sref.cstr_, sref.leng_, true );

	if( !ZnkS_empty( explicit_referer ) ){
		Znk_snprintf( referer_bur, sizeof(referer_bur), "%s", explicit_referer );
		sref.cstr_ = referer_bur;
		sref.leng_ = Znk_strlen( referer_bur );
	} else { 
		if( is_https ){
			if( rand_factor < 30 ){
				ZnkSRef_set_literal( &sref, "https://www.google.com" );
			} else if( rand_factor < 50 ){
				ZnkSRef_set_literal( &sref, "https://www.2chan.net" );
			} else {
				Znk_snprintf( referer_bur, sizeof(referer_bur), "https://%s", hostname );
				sref.cstr_ = referer_bur;
				sref.leng_ = Znk_strlen( referer_bur );
			}
		} else {
			if( rand_factor < 30 ){
				ZnkSRef_set_literal( &sref, "http://www.google.com" );
			} else if( rand_factor < 50 ){
				ZnkSRef_set_literal( &sref, "http://www.2chan.net" );
			} else {
				Znk_snprintf( referer_bur, sizeof(referer_bur), "http://%s", hostname );
				sref.cstr_ = referer_bur;
				sref.leng_ = Znk_strlen( referer_bur );
			}
		}
	}
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Referer", Znk_strlen_literal("Referer"),
			sref.cstr_, sref.leng_, true );


	if( cookie ){
		ZnkStr     cok_stmt = ZnkStr_new( "" );
		ZnkCookie_extend_toCookieStatement( cookie, cok_stmt );
		varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
				"Cookie",   Znk_strlen_literal("Cookie"),
				ZnkStr_cstr(cok_stmt), ZnkStr_leng(cok_stmt), true );
		ZnkStr_delete( cok_stmt );
	}

	ZnkSRef_set_literal( &sref, "keep-alive" );
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Connection", Znk_strlen_literal("Connection"),
			sref.cstr_, sref.leng_, true );

	Znk_UNUSED( varp );
}

bool
EstBase_download( const char* hostname, const char* unesc_req_urp, const char* target,
		const char* ua, ZnkVarpAry cookie, const char* evar_http_cookie,
		const char* parent_proxy,
		ZnkStr result_filename, ZnkStr msg, RanoModule mod, int* status_code, bool is_https )
{
	bool        result   = false;
	const char* tmpdir   = EstConfig_getTmpDirPID( true );
	const char* cachebox = "./cachebox/";
	struct ZnkHtpHdrs_tag htp_hdrs = { 0 };
	bool   is_without_404 = true;
	const char* explicit_referer = EstConfig_getExplicitReferer();

	ZnkHtpHdrs_compose( &htp_hdrs );
	initHtpHdr( &htp_hdrs, hostname, ua, cookie, is_https, explicit_referer );

	/***
	 * Header and Cookie filtering
	 * PostVar filteringにおけるon_post関数の呼び出しで、headerやcookie_varsの値を
	 * 参照/加工するような処理にも対応するため、この filteringをここで呼び出す.
	 */
	if( mod ){
		bool is_all_replace = true;
		RanoModule_filtHtpHeader(  mod, htp_hdrs.vars_ );
		RanoModule_filtCookieVars( mod, htp_hdrs.vars_, is_all_replace, NULL );
	}

	if( mod ){
		ZnkMyf ftr_send = RanoModule_ftrSend( mod );
		const ZnkVarpAry ftr_vars = ZnkMyf_find_vars( ftr_send, "header_vars" );
		if( ftr_vars ){
			const ZnkVarp ua_var = ZnkVarpAry_find_byName( ftr_vars, "User-Agent", Znk_strlen_literal("User-Agent"), false );
			if( RanoHtpModifier_modifySendHdrs( htp_hdrs.vars_, ZnkVar_cstr(ua_var), msg ) ){
				if( msg ){
					ZnkStr_add( msg, "  RanoHtpModifier_modifySendHdrs : true\n" );
				}
			} else {
				if( msg ){
					ZnkStr_add( msg, "  RanoHtpModifier_modifySendHdrs : false\n" );
				}
			}
		}
	}

	result = RanoHtpBoy_cipher_get_with404( hostname, unesc_req_urp, target,
			&htp_hdrs,
			parent_proxy,
			tmpdir, cachebox, result_filename,
			is_without_404, status_code, is_https,
			NULL, msg );

	if( !result && msg ){
		ZnkStr_addf( msg,
				"  RanoHtpBoy_cipher_get_with404 : result=[%d] hostname=[%s] req_urp=[%s]<br>"
				"                    : target=[%s] parent_proxy=[%s] tmpdir=[%s]<br>"
				"                    : result_filename=[%s].<br>",
				result, hostname, unesc_req_urp, target, parent_proxy, tmpdir, ZnkStr_cstr(result_filename) );
	}

	ZnkHtpHdrs_dispose( &htp_hdrs );
	return result;
}

const char*
EstBase_findTargetName( const ZnkMyf mtgt, const char* line )
{
	const size_t  size = ZnkMyf_numOfSection( mtgt );
	size_t        idx;
	ZnkMyfSection sec;
	ZnkStrAry     lines;
	for( idx=0; idx<size; ++idx ){
		sec   = ZnkMyf_atSection( mtgt, idx );
		lines = ZnkMyfSection_lines( sec ); 
		if( ZnkStrAry_find_isMatch( lines, 0, line, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
			/* found */
			return ZnkMyfSection_name( sec );
		}
	}
	/* not found */
	return NULL;
}

void
EstBase_addConsoleMsg_HttpCookie( ZnkStr console_msg, const char* http_cookie )
{
	if( http_cookie ){
		ZnkVarpAry cookie   = ZnkVarpAry_create( true );
		size_t size = 0;
	
		ZnkCookie_regist_byCookieStatement( cookie, http_cookie, Znk_strlen(http_cookie) );
		size = ZnkVarpAry_size( cookie );
	
		if( size == 1 ){
			ZnkStr_addf( console_msg, "http_cookie=[%s]\n", http_cookie );
		} else if( size > 1 ){
			size_t idx;
			ZnkVarp varp;
			ZnkStr_add( console_msg, "http_cookie={\n" );
			for( idx=0; idx<size; ++idx ){
				varp = ZnkVarpAry_at( cookie, idx );
				ZnkStr_addf( console_msg, "  %s=%s;\n", ZnkVar_name_cstr(varp), ZnkVar_cstr(varp) );
			}
			ZnkStr_add( console_msg, "}" );
		}
	
		ZnkVarpAry_destroy( cookie );
	}
}


typedef struct {
	const char* title_;
	char        dst_dir_[ 256 ];
	ZnkStr      ermsg_;
	size_t      days_ago_;
	size_t      sec_ago_;
} MoveInfo;

static bool
isOldFile( const char* file_path, size_t days_ago, size_t sec_ago )
{
	ZnkDate current = { 0 };
	ZnkDate date = { 0 };
	ZnkDate threshold = { 0 };
	long diff_sec = 0;
	bool is_old = false;

	ZnkDate_getCurrent( &current );
	RanoFileInfo_getLastUpdatedTime( file_path, &date );

	if( days_ago == 0 ){
		ZnkDate_getNDaysAgo( &threshold, &current, 1 );
	
		if( ZnkDate_compareDay( &date, &threshold ) >= 0 ){
			diff_sec = ZnkDate_diffSecond( &current, &date, 1 );
			if( diff_sec >= (long)sec_ago ){
				is_old = true;
			}
		} else {
			is_old = true;
		}
	} else {
		ZnkDate_getNDaysAgo( &threshold, &current, days_ago );
		is_old = (bool)( ZnkDate_compareDay( &date, &threshold ) < 0 );
	}

	return is_old;
}
static bool report_onEnterDir( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	const char* title    = info->title_;
	ZnkStr      ermsg    = info->ermsg_;

	if( ermsg ){
		ZnkStr_addf( ermsg, "%s : onEnterDir : [%s]\n", title, top_dir );
	}
	return true;
}
static bool move_processFile( ZnkDirRecursive recur, const char* file_path, void* arg, size_t local_err_num )
{
	bool result = false;
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	const char* title    = info->title_;
	const char* dst_dir  = info->dst_dir_;
	ZnkStr      ermsg    = info->ermsg_;
	size_t      days_ago = info->days_ago_;
	size_t      sec_ago  = info->sec_ago_;

	if( isOldFile( file_path, days_ago, sec_ago ) ){
		char dst_file_path[ 256 ] = "";
		const char* q = NULL;
		Znk_snprintf( dst_file_path, sizeof(dst_file_path), "%s/%s", dst_dir, file_path );
		q = Znk_strrchr( dst_file_path, '/' );
		if( q ){
			char dst_dir_path[ 256 ] = "";
			ZnkS_copy( dst_dir_path, sizeof(dst_dir_path), dst_file_path, q-dst_file_path );
			ZnkDir_mkdirPath( dst_dir_path, Znk_NPOS, '/', NULL );
			if( ZnkDir_copyFile_byForce( file_path, dst_file_path, ermsg ) ){
				if( ZnkDir_deleteFile_byForce( file_path ) ){
					if( ermsg ){
						ZnkStr_addf( ermsg, "%s : moveFile : [%s] to [%s] OK.\n", title, file_path, dst_dir_path );
					}
					result = true;
				} else {
					if( ermsg ){
						ZnkStr_addf( ermsg, "%s : moveFile : [%s] to [%s] failure.\n", title, file_path, dst_dir_path );
					}
					result = false;
				}
			}
		}
	}

	return result;
}
static bool remove_processFile( ZnkDirRecursive recur, const char* file_path, void* arg, size_t local_err_num )
{
	bool result = false;
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	const char* title    = info->title_;
	ZnkStr      ermsg    = info->ermsg_;
	size_t      days_ago = info->days_ago_;
	size_t      sec_ago  = info->sec_ago_;

	if( isOldFile( file_path, days_ago, sec_ago ) ){
		if( ZnkDir_deleteFile_byForce( file_path ) ){
			if( ermsg ){
				ZnkStr_addf( ermsg, "%s : deleteFile : [%s] OK.\n", title, file_path );
			}
			result = true;
		} else {
			if( ermsg ){
				ZnkStr_addf( ermsg, "%s : deleteFile : [%s] failure.\n", title, file_path );
			}
			result = false;
		}
	}

	return result;
}
static bool rmdir_onExitDir( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	bool result = false;
	MoveInfo* info = Znk_force_ptr_cast( MoveInfo*, arg );
	ZnkStr      ermsg = info->ermsg_;
	const char* title = info->title_;

	if( local_err_num == 0 ){
		if( ZnkDir_rmdir( top_dir ) ){
			if( ermsg ){
				ZnkStr_addf( ermsg, "%s : rmdir : [%s] OK.\n", title, top_dir );
			}
			result = true;
		} else {
			if( ermsg ){
				ZnkStr_addf( ermsg, "%s : rmdir : [%s] failure.\n", title, top_dir );
			}
			result = false;
		}
	} else {
		if( ermsg ){
			ZnkStr_addf( ermsg, "%s : rmdir_onExitDir : local_err_num = [%zu].\n", title, local_err_num );
		}
		result = false;
	}
	return result;
}

bool
EstBase_moveOldDir( const char* src_dir, const char* dst_dir, ZnkStr ermsg, size_t days_ago, size_t sec_ago )
{
	bool is_err_ignore = true;
	MoveInfo info = { 0 };
	ZnkDirRecursive recur = NULL;

	info.ermsg_ = ermsg;
	info.title_ = "EstBase_moveOldDir";
	ZnkS_copy( info.dst_dir_, sizeof(info.dst_dir_), dst_dir, Znk_NPOS );
	info.days_ago_ = days_ago;
	info.sec_ago_  = sec_ago;

	if( ZnkS_empty( dst_dir ) ){
		return false;
	}
	if( ZnkS_empty( src_dir ) ){
		return false;
	}
	
	recur = ZnkDirRecursive_create( is_err_ignore,
			report_onEnterDir, &info,
			move_processFile,  &info,
			rmdir_onExitDir,   &info );
	ZnkDirRecursive_traverse( recur, src_dir, NULL );
	ZnkDirRecursive_destroy( recur );

	return true;
}
void
EstBase_removeOldFile( const char* topdir, ZnkStr ermsg, size_t days_ago, size_t sec_ago )
{
	bool is_err_ignore = true;
	MoveInfo info = { 0 };
	ZnkDirRecursive recur = NULL;

	info.ermsg_ = ermsg;
	info.title_ = "EstBase_removeOldFile";
	info.days_ago_ = days_ago;
	info.sec_ago_  = sec_ago;
	
	recur = ZnkDirRecursive_create( is_err_ignore,
			report_onEnterDir,  &info,
			remove_processFile, &info,
			rmdir_onExitDir,    &info );
	ZnkDirRecursive_traverse( recur, topdir, NULL );
	ZnkDirRecursive_destroy( recur );
}

static void
escapeEvilCharOnFSys( ZnkStr ans, const uint8_t* data, size_t data_size )
{
	const uint8_t* begin = data;
	const uint8_t* end   = data + data_size;
	const uint8_t* p = begin;
	while( p != end ){
		switch( *p ){
		case '*':
		case '?':
		case ':':
		case ';':
			ZnkStr_addf2( ans, "%%%02X", *p );
			break;
#if 0
		case '~':
			ZnkStr_addf2( ans, "%%%02X", *p );
			break;
#endif
		default:
			ZnkStr_add_c( ans, *p );
			break;
		}
		++p;
	}
}
void
EstBase_unescape_forMbcFSysPath( ZnkStr fsys_path )
{
	ZnkStr tmp = ZnkStr_new( "" );
	ZnkHtpURL_unescape_toStr( tmp, ZnkStr_cstr(fsys_path), ZnkStr_leng(fsys_path) );
	ZnkStr_clear( fsys_path );
	ZnkHtpURL_unescape_toStr( fsys_path, ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );
	ZnkStr_clear( tmp );
	escapeEvilCharOnFSys( tmp, (uint8_t*)ZnkStr_cstr(fsys_path), ZnkStr_leng(fsys_path) );
	ZnkStr_swap( fsys_path, tmp );
	ZnkStr_delete( tmp );
}
void
EstBase_escape_forURL( ZnkStr unknown_count_escaped_path, size_t escape_count )
{
	ZnkStr unknown = unknown_count_escaped_path;
	ZnkStr tmp = ZnkStr_new( "" );
	ZnkHtpURL_unescape_toStr( tmp, ZnkStr_cstr(unknown), ZnkStr_leng(unknown) );
	ZnkStr_clear( unknown );
	ZnkHtpURL_unescape_toStr( unknown, ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );
	ZnkStr_clear( tmp );
	if( escape_count ){
		ZnkHtpURL_escape_withoutSlash( tmp, (uint8_t*)ZnkStr_cstr(unknown), ZnkStr_leng(unknown) );
		ZnkStr_swap( unknown, tmp );
	}
	ZnkStr_delete( tmp );
}
void
EstBase_safeDumpBuf( ZnkStr ans, uint8_t* buf, size_t size, size_t num_per_line )
{
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		if( buf[idx] >= 0x20 && buf[idx] < 0x7f ){
			ZnkStr_addf( ans, "%c", buf[idx] );
		} else {
			ZnkStr_addf( ans, "%02x", buf[idx] );
		}
		if( num_per_line && (idx+1) % num_per_line == 0 ){
			ZnkStr_addf( ans, "\n" );
		}
	}
}

ZnkStr
EstBase_registStrToVars( ZnkVarpAry vars, const char* name, const char* val )
{
	ZnkStr dst_ref = NULL;
	ZnkVarp var = ZnkVarpAry_findObj_byName( vars, name, Znk_NPOS, false );
	if( var == NULL ){
		var = ZnkVarp_create( name, "", 0, ZnkPrim_e_Str, NULL );
		ZnkVarpAry_push_bk( vars, var );
	}
	dst_ref = ZnkVar_str( var );
	if( val ){
		ZnkStr_set( dst_ref, val );
	}
	return dst_ref;
}

void
EstBase_escapeToAmpForm( ZnkStr str )
{

	ZnkStrEx_replace_BF( str, 0, "&", 1, "&amp;", 5, Znk_NPOS, Znk_NPOS ); /* escape & */
	ZnkHtpURL_negateHtmlTagEffection( str ); /* for XSS */
	{
		static ZnkOccTable_D( st_occ_tbl );
		if( !ZnkOccTable_isInitialized( st_occ_tbl ) ){
			ZnkMem_getLOccTable_forBMS( st_occ_tbl, (uint8_t*)"\r\n", 2 );
		}
		ZnkStrEx_replace_BMS( str, 0,
				"\r\n", 2, st_occ_tbl,
				"&br;", 4,
				Znk_NPOS, Znk_NPOS );
	}
	{
		static ZnkOccTable_D( st_occ_tbl );
		if( !ZnkOccTable_isInitialized( st_occ_tbl ) ){
			ZnkMem_getLOccTable_forBMS( st_occ_tbl, (uint8_t*)"']", 2 );
		}
		ZnkStrEx_replace_BMS( str, 0,
				"']", 2, st_occ_tbl,
				"&qt_end;", 8,
				Znk_NPOS, Znk_NPOS );
	}
}

void
EstBase_unescapeToAmpForm( ZnkStr str )
{
	{
		static ZnkOccTable_D( st_occ_tbl );
		if( !ZnkOccTable_isInitialized( st_occ_tbl ) ){
			ZnkMem_getLOccTable_forBMS( st_occ_tbl, (uint8_t*)"&br;", 4 );
		}
		ZnkStrEx_replace_BMS( str, 0,
				"&br;", 4, st_occ_tbl,
				"\r\n", 2,
				Znk_NPOS, Znk_NPOS );
	}
	{
		static ZnkOccTable_D( st_occ_tbl );
		if( !ZnkOccTable_isInitialized( st_occ_tbl ) ){
			ZnkMem_getLOccTable_forBMS( st_occ_tbl, (uint8_t*)"&qt_end;", 8 );
		}
		ZnkStrEx_replace_BMS( str, 0,
				"&qt_end;", 8, st_occ_tbl,
				"']", 2,
				Znk_NPOS, Znk_NPOS );
	}
}

size_t
EstBase_agePtrObjs( ZnkVarpAry list, ZnkVarpAry selected_list )
{
	size_t       count = 0;
	const char*  id        = NULL;
	const size_t list_size = ZnkVarpAry_size( list );
	ZnkVarp      list_varp = NULL;
	ZnkObjAry    ref_list1 = ZnkObjAry_create( NULL ); /* as-reference container */
	ZnkObjAry    ref_list2 = ZnkObjAry_create( NULL ); /* as-reference container */
	ZnkVarp*     list_ptr  = ZnkVarpAry_ary_ptr( list );
	size_t       idx;

	for( idx=0; idx<list_size; ++idx ){
		list_varp = list_ptr[ idx ];
		id  = ZnkVar_name_cstr( list_varp );
		if( ZnkVarpAry_findIdx_byName( selected_list, id, Znk_NPOS, false ) != Znk_NPOS ){
			ZnkObjAry_push_bk( ref_list1, (ZnkObj)list_ptr[ idx ] );
			++count;
		} else {
			ZnkObjAry_push_bk( ref_list2, (ZnkObj)list_ptr[ idx ] );
		}
	}

	if( count ){
		ZnkObj*      ref_list1_ptr  = ZnkObjAry_ary_ptr( ref_list1 );
		const size_t ref_list1_size = ZnkObjAry_size(    ref_list1 );
		ZnkObj*      ref_list2_ptr  = ZnkObjAry_ary_ptr( ref_list2 );
		const size_t ref_list2_size = ZnkObjAry_size(    ref_list2 );
		Znk_memmove( list_ptr + 0,
				ref_list1_ptr, ref_list1_size*sizeof(ZnkObj) );
		Znk_memmove( list_ptr + ref_list1_size,
				ref_list2_ptr, ref_list2_size*sizeof(ZnkObj) );
	}

	ZnkObjAry_destroy( ref_list1 );
	ZnkObjAry_destroy( ref_list2 );
	return count;
}
