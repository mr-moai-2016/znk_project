#ifndef INCLUDE_GUARD__Rano_cgi_util_h__
#define INCLUDE_GUARD__Rano_cgi_util_h__

#include <Rano_type.h>
#include <Rano_module.h>
#include <Znk_bfr.h>
#include <Znk_str.h>
#include <Znk_varp_ary.h>
#include <Znk_htp_hdrs.h>
#include <Znk_bird.h>

Znk_EXTERN_C_BEGIN

typedef struct RanoCGIEVar_tag {
	char* server_name_;
	char* server_port_;
	char* content_type_;
	char* content_length_;
	char* remote_addr_;
	char* remote_host_;
	char* remote_port_;
	char* request_method_;
	char* query_string_;
	char* http_cookie_;
	char* http_user_agent_;
	char* http_accept_;
} RanoCGIEVar;

typedef struct {
	bool (*func_)( ZnkVarpAry post_vars, void* arg, const char* content_type, bool* is_updated );
	void*  arg_;
} RanoCGIUtilFnca_procPostVars;

typedef int (*RanoCGIUtilFuncT_procStr)( ZnkStr str, void* arg );

void
RanoCGIUtil_addHdrVarsStr( ZnkStr str, const ZnkVarpAry vars );
void
RanoCGIUtil_addPostVarsStr( ZnkStr str, const ZnkVarpAry hpvs );

void
RanoCGIUtil_getStdInStr( ZnkBfr stdin_bfr, size_t content_length );

RanoCGIEVar*
RanoCGIEVar_create( void );

void
RanoCGIEVar_destroy( RanoCGIEVar* evar );

/**
 * @brief
 * URLにおける Query String が指定されている場合はそれを取得する.
 * 指定されていない場合、この関数は空文字を返す.
 */
Znk_INLINE const char*
RanoCGIUtil_getQueryString( RanoCGIEVar* evar )
{
	/* Query String が指定されていない場合、evar->query_string_ は NULLとなっている. */
	return evar->query_string_ ? evar->query_string_ : "";
}

void
RanoCGIUtil_splitQueryString( ZnkVarpAry post_vars, const char* query_string, const size_t query_string_leng, bool is_unescape_val );


bool
RanoCGIUtil_getPostVars_fromInBfr( ZnkVarpAry post_vars, RanoModule mod, ZnkHtpHdrs htp_hdrs,
		ZnkBfr in_bfr, size_t content_length,
		const char* content_type, const char* src_boundary, const char* dst_boundary, 
		ZnkStr pst_str, const char* varname_of_urlenc_body, bool is_unescape_val,
		RanoCGIUtilFnca_procPostVars* fnca_proc_post_vars );

/**
 * @brief
 *   POST時における multipart/form-data および application/x-www-form-urlencoded 形式を解析することにより
 *   post_varsを取得する.
 *
 * @param evar:
 *   このCGIにおける環境変数が格納されたRanoCGIEVar型を指定する.
 *   この指定は必須である.
 *
 * @param post_vars:
 *   結果を格納するPost変数用の配列.
 *   この指定は必須である.
 *
 * @param mod:
 *   フィルタ用のモジュール.
 *   RanoModuleによるフィルタリングを行いたい場合に指定する.
 *   それが不要ならNULLを指定する.
 *
 * @param htp_hdrs:
 *   フィルタ対象となるHTTPヘッダ.
 *   RanoModuleによるフィルタリングを行いたい場合に指定する.
 *   それが不要ならNULLを指定する.
 *
 * @param pst_str:
 *   Post変数配列の内容をレポートする文字列を取得したい場合に指定する.
 *   それが不要ならNULLを指定する.
 *
 * @param varname_of_urlenc_body:
 *   これは application/x-www-form-urlencoded 形式の場合のみ意味を持つ.
 *   これを指定するとそのような変数名を持つPost変数の一つとして追加することができ、
 *   その値の内容はHTTPヘッダ以降に続く application/x-www-form-urlencoded 形式の文字列そのものとなる.
 *   それが不要ならNULLを指定する.
 */
bool
RanoCGIUtil_getPostedFormData( RanoCGIEVar* evar, ZnkVarpAry post_vars, RanoModule mod, ZnkHtpHdrs htp_hdrs,
		ZnkStr pst_str, const char* varname_of_urlenc_body, bool is_unescape_val );

void
RanoCGIMsg_initiate( bool with_html_start, const char* base_href );

void
RanoCGIMsg_finalize( void );

void
RanoCGIMsg_begin( void );

void
RanoCGIMsg_end( void );

bool
RanoCGIUtil_displayBinary( const char* filename, const char* content_type );

bool
RanoCGIUtil_displayText( const char* filename, const char* content_type, RanoCGIUtilFuncT_procStr proc_str, void* proc_str_arg );

void
RanoCGIUtil_replaceNLtoHtmlBR( ZnkStr str );

bool
RanoCGIUtil_printTemplateHTML( RanoCGIEVar* evar, ZnkBird bird, const char* template_html_file );

const char*
RanoCGIUtil_skipProtocolPrefix( const char* src );

RanoTextType
RanoCGIUtil_getTextType( const char* content_type );

size_t
RanoCGIUtil_initLog( const char* log_filebasename, const char* count_filename,
		size_t pitch_of_count, size_t pitch_of_logfiles, bool keep_open );
size_t
RanoCGIUtil_initMultiDirLog( const char* logdir_basename, const char* count_filename,
		size_t pitch_of_count, size_t pitch_of_logfiles, bool keep_open );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
