#ifndef INCLUDE_GUARD__Rano_htp_boy_h__
#define INCLUDE_GUARD__Rano_htp_boy_h__

#include <Znk_base.h>
#include <Znk_htp_hdrs.h>
#include <Znk_htp_rar.h>
#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

void
RanoHtpBoy_setTmpDirCommon( const char* tmpdir_common );

bool
RanoHtpBoy_initiateHttpsModule( const char* lib_basename, const char* cert_pem );

bool
RanoHtpBoy_processEx( const char* cnct_hostname, uint16_t cnct_port,
		const char* result_filename, const char* cookie_filename,
		const char* send_hdr_filename, const char* body_img_filename,
		const char* recv_hdr_filename, bool is_https,
		ZnkHtpOnRecvFuncArg* prog_fnca, ZnkStr ermsg );
bool
RanoHtpBoy_process( const char* cnct_hostname, uint16_t cnct_port,
		const char* result_filename, const char* cookie_filename,
		const char* send_hdr_filename, const char* body_img_filename,
		const char* recv_hdr_filename );

void
RanoHtpBoy_getResultFile( const char* hostname, const char* unesc_req_urp, ZnkStr result_filename,
		const char* cachebox, const char* target );

void
RanoHtpBoy_readRecvHdrVar( const char* var_name, char* val_buf, size_t val_buf_size, const char* target, const char* tmpdir );
void
RanoHtpBoy_readRecvHdrStatus( ZnkStr ans, size_t tkn_idx, const char* target, const char* tmpdir );

bool
RanoHtpBoy_cipher_get( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename, bool is_https, ZnkStr ermsg );
bool
RanoHtpBoy_cipher_get_with404( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename, bool is_without_404, int* ans_status_code, bool is_https,
		ZnkHtpOnRecvFuncArg* prog_fnca, ZnkStr ermsg );
bool
RanoHtpBoy_cipher_head( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir,
		ZnkStr result_filename, bool is_https, ZnkStr ermsg );
bool
RanoHtpBoy_cipher_post( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs, ZnkVarpAry post_vars,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename, bool is_https, ZnkStr ermsg );

bool
RanoHtpBoy_do_get( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename );
bool
RanoHtpBoy_do_get_with404( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename, bool is_without_404, int* ans_status_code,
		ZnkHtpOnRecvFuncArg* prog_fnca );
bool
RanoHtpBoy_do_head( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs,
		const char* parent_proxy,
		const char* tmpdir,
		ZnkStr result_filename );
bool
RanoHtpBoy_do_post( const char* hostname, const char* unesc_req_urp, const char* target,
		struct ZnkHtpHdrs_tag* htp_hdrs, ZnkVarpAry post_vars,
		const char* parent_proxy,
		const char* tmpdir, const char* cachebox,
		ZnkStr result_filename );

void
RanoHtpBoy_seekExeDir( ZnkStr exedir, size_t depth, char dsp, const char* target_filename );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
