#ifndef INCLUDE_GUARD__Est_base_h__
#define INCLUDE_GUARD__Est_base_h__

#include <Znk_str.h>
#include <Rano_module.h>

Znk_EXTERN_C_BEGIN

const char*
EstBase_getHostnameAndRequrp_fromEstVal( char* hostname, size_t hostname_size, ZnkStr req_urp, const char* src_url );

bool
EstBase_download( const char* hostname, const char* unesc_req_urp, const char* target,
		const char* ua, ZnkVarpAry cookie, const char* evar_http_cookie,
		const char* parent_proxy,
		ZnkStr result_filename, ZnkStr msg, RanoModule mod, int* status_code );

const char*
EstBase_findTargetName( const ZnkMyf mtgt, const char* line );
void
EstBase_addConsoleMsg_HttpCookie( ZnkStr console_msg, const char* http_cookie );

bool
EstBase_moveOldDir( const char* src_dir, const char* dst_dir, ZnkStr ermsg, size_t days_ago, size_t sec_ago );
void
EstBase_removeOldFile( const char* topdir, ZnkStr ermsg, size_t days_ago, size_t sec_ago );

void
EstBase_unescape_forMbcFSysPath( ZnkStr fsys_path );
void
EstBase_escape_forURL( ZnkStr unknown_count_escaped_path, size_t escape_count );
void
EstBase_safeDumpBuf( ZnkStr ans, uint8_t* buf, size_t size, size_t num_per_line );


ZnkStr
EstBase_registStrToVars( ZnkVarpAry vars, const char* name, const char* val );
void
EstBase_escapeToAmpForm( ZnkStr str );
void
EstBase_unescapeToAmpForm( ZnkStr str );

size_t
EstBase_agePtrObjs( ZnkVarpAry list, ZnkVarpAry selected_list );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
