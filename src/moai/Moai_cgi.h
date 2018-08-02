#ifndef INCLUDE_GUARD__Moai_cgi_h__
#define INCLUDE_GUARD__Moai_cgi_h__

#include <Znk_socket.h>
#include <Znk_str_ary.h>
#include <Znk_varp_ary.h>
#include <Rano_module.h>

Znk_EXTERN_C_BEGIN

int
MoaiCGI_runGet( const char* cmd, const char* curdir_new, ZnkSocket sock, RanoModule mod,
		const size_t hdr_size, ZnkStrAry hdr1st, ZnkVarpAry hdr_vars,
		size_t content_length, ZnkBfr stream, ZnkStr query_str, bool is_xhr_dmz );

int
MoaiCGI_runPost( const char* cmd, const char* curdir_new, ZnkSocket sock, RanoModule mod,
		const size_t hdr_size, ZnkStrAry hdr1st, ZnkVarpAry hdr_vars,
		size_t content_length, ZnkBfr stream, ZnkStr query_str, bool is_xhr_dmz );

void
MoaiCGIManager_makeHeader( ZnkStr html, const char* title, bool inline_script );

bool
MoaiCGIManager_load( void );
bool
MoaiCGIManager_save( void );

const ZnkVarpAry
MoaiCGIManager_getURPAliasList( void );

ZnkVarp
MoaiCGIManager_parseURPTail( const char* urp,
		ZnkStr dir, ZnkStr filename,
		ZnkStr path_info, ZnkStr query_str, bool is_cgi_exec_deny );

bool
MoaiCGIManager_isMatchedReqUrp( const char* req_urp, const char* urp_ptn_list_name );
bool
MoaiCGIManager_isValidReqUrp_forRun( const char* req_urp );
bool
MoaiCGIManager_isValidReqUrp_forPathBegin( const char* req_urp, const char* path_list_name );

/**
 * @note
 * Ç±ÇÃä÷êîÇ≈fsys_path ÇÕ "./"Ç≈énÇ‹ÇËÅAÇªÇÍà»ç~Ç™ req_urp Ç∆ìØÇ∂ï∂éöóÒÇ…Ç»Ç¡ÇƒÇ¢ÇÈÇ±Ç∆Ç™ëOíÒ.
 * ÇªÇ§Ç≈ÇÕÇ»Ç¢èÍçáÇÕâΩÇ‡ÇµÇ»Ç¢.
 */
const bool
MoaiCGIManager_mapFSysDir( ZnkStr fsys_path, const char* profile_dir );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
