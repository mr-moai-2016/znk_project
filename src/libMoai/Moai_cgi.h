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
 * この関数でfsys_path は "./doc_root" あるいは "./" で始まり、
 * それ以降が req_urp と同じ文字列になっていることが前提.
 * そうではない場合は何もしない.
 *
 * @note
 * 特に req_urp が /doc_root と明示的に指定されている場合を explicit_doc_root と呼び、
 * その場合は fsys_path は ./doc_root で始まるような文字列でセットされ、かつ explicit_doc_rootにtrueを指定しなければならない.
 */
const bool
MoaiCGIManager_mapFSysDir2( ZnkStr fsys_path, const char* profile_dir, bool explicit_doc_root );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
