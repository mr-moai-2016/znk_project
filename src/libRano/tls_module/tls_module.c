#include <Znk_dlhlp.h>
#include <Znk_str.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <Znk_htp_hdrs.h>
#include <Znk_stdc.h>
#include <Znk_str_fio.h>
#include <Znk_sys_errno.h>
#include <Znk_cookie.h>
#include <Znk_htp_sbio.h>
#include <tls_module/tls.h>

/*****************************************************************************/
/* DLink Module */

typedef int         (*FuncType_tls_init)(void);
typedef struct tls_config* (*FuncType_tls_config_new)(void);
typedef int         (*FuncType_tls_config_set_ca_file)(struct tls_config *_config, const char *_ca_file);
typedef struct tls* (*FuncType_tls_client)(void);
typedef int         (*FuncType_tls_configure)(struct tls *_ctx, struct tls_config *_config);
typedef int         (*FuncType_tls_connect)(struct tls *_ctx, const char *_host, const char *_port);
typedef const char* (*FuncType_tls_error)(struct tls *_ctx);
typedef ssize_t     (*FuncType_tls_write)(struct tls *_ctx, const void *_buf, size_t _buflen);
typedef ssize_t     (*FuncType_tls_read)(struct tls *_ctx, void *_buf, size_t _buflen);
typedef int         (*FuncType_tls_close)(struct tls *_ctx);
typedef void        (*FuncType_tls_free)(struct tls *_ctx);
typedef void        (*FuncType_tls_config_free)(struct tls_config *_config);

typedef struct {
	ZnkDLinkHandler handler_;
	FuncType_tls_init        tls_init_;
	FuncType_tls_config_new  tls_config_new_;
	FuncType_tls_config_set_ca_file tls_config_set_ca_file_;
	FuncType_tls_client      tls_client_;
	FuncType_tls_configure   tls_configure_;
	FuncType_tls_connect     tls_connect_;
	FuncType_tls_error       tls_error_;
	FuncType_tls_write       tls_write_;
	FuncType_tls_read        tls_read_;
	FuncType_tls_close       tls_close_;
	FuncType_tls_free        tls_free_;
	FuncType_tls_config_free tls_config_free_;
} ModuleTls;

static ModuleTls* theModuleTls( bool auto_load, const char* dlib_name ){
	Znk_DLHLP_THE_MODULE_BEGIN( ModuleTls )
	Znk_DLHLP_GET_FUNC( tls_init );
	Znk_DLHLP_GET_FUNC( tls_config_new );
	Znk_DLHLP_GET_FUNC( tls_config_set_ca_file );
	Znk_DLHLP_GET_FUNC( tls_client );
	Znk_DLHLP_GET_FUNC( tls_configure );
	Znk_DLHLP_GET_FUNC( tls_connect );
	Znk_DLHLP_GET_FUNC( tls_error );
	Znk_DLHLP_GET_FUNC( tls_write );
	Znk_DLHLP_GET_FUNC( tls_read );
	Znk_DLHLP_GET_FUNC( tls_close );
	Znk_DLHLP_GET_FUNC( tls_free );
	Znk_DLHLP_GET_FUNC( tls_config_free );
	Znk_DLHLP_THE_MODULE_END
}

static Znk_DLHLP_DECLARE_UNLOAD_MODULE( unloadModuleTls, ModuleTls )

/* endof DLink Module */
/*****************************************************************************/

static char st_lib_basename[ 256 ] = "";
static char st_cert_pem[ 256 ] = "";

static ModuleTls* theModuleTlsSt( void )
{
#if 0
	static ModuleTls st_module = { NULL, };
	st_module.handler_ = NULL;
	st_module.inflateInit2__ = inflateInit2_;
	st_module.inflate_       = inflate;
	st_module.inflateEnd_    = inflateEnd;
	return &st_module;
#endif
	return NULL;
}
static ModuleTls* theModuleTlsBoth( const char* lib_basename )
{
	static const bool is_dynamic_load = true;
	ModuleTls* module = NULL;
	if( is_dynamic_load && !ZnkS_empty(lib_basename) ){
		char lib_filename[ 256 ] = "";
#if defined(__CYGWIN__)
		Znk_snprintf( lib_filename, sizeof(lib_filename), "%s.dll", lib_basename );
#elif defined(Znk_TARGET_WINDOWS)
		Znk_snprintf( lib_filename, sizeof(lib_filename), "%s.dll", lib_basename );
#else
		Znk_snprintf( lib_filename, sizeof(lib_filename), "%s.so", lib_basename );
#endif
		module = theModuleTls( true, lib_filename );
	} else {
		module = theModuleTlsSt();
	}
	return module;
}

bool
TlsModule_initiate( const char* lib_basename, const char* cert_pem )
{
	ZnkS_copy( st_lib_basename, sizeof(st_lib_basename), lib_basename, Znk_NPOS );
	ZnkS_copy( st_cert_pem,     sizeof(st_cert_pem),     cert_pem,     Znk_NPOS );
	return (bool)( theModuleTlsBoth( st_lib_basename ) != NULL );
}
void
TlsModule_finalize( void )
{
	unloadModuleTls();
}

struct RecvInfo {
	ModuleTls*  module_;
	struct tls* ctx_;
	ZnkStr      ermsg_;
	ZnkHtpOnRecvFuncArg prog_fnca_;
};

static int
recvTls( void* arg, uint8_t* buf, size_t buf_size )
{
	struct RecvInfo* info = Znk_force_ptr_cast( struct RecvInfo*, arg );
	ssize_t len = 0;
	while( true ){
		len = info->module_->tls_read_( info->ctx_, buf, buf_size );
		if( len == TLS_WANT_POLLIN || len == TLS_WANT_POLLOUT ){
			continue;
		}
		if( len < 0 ){
			if( info->ermsg_ ){
				ZnkStr_addf( info->ermsg_, "TlsModule_getHtpsProcess : Error : tls_read: %s",
						info->module_->tls_error_(info->ctx_));
			}
		}
		if( info->prog_fnca_.func_ ){
			info->prog_fnca_.func_( info->prog_fnca_.arg_, buf, buf_size );
		}
		break;
	}
	return (int)len;
}

static bool
writeBuf_byTls( ZnkBfr wk_bfr, ModuleTls* module, struct tls* ctx, ZnkStr ermsg )
{
	bool result = true;
	uint8_t* p = NULL;
	size_t remain_size = 0;
	ssize_t len = 0;
	p = ZnkBfr_data( wk_bfr );
	remain_size = ZnkBfr_size( wk_bfr );
	while( remain_size ){
		len = module->tls_write_( ctx, p, remain_size );
		if( len == TLS_WANT_POLLIN || len == TLS_WANT_POLLOUT ){
			continue;
		}
		if( len == 0 ){
			break;
		}
		if( len < 0 ){
			if( ermsg ){
				ZnkStr_addf( ermsg, "TlsModule_getHtpsProcess : Error : tls_write: %s", module->tls_error_(ctx));
			}
			result = false;
			break;
		}
		if( len <= (long)remain_size ){
			p += len;
			remain_size -= len;
		} else {
			remain_size = 0;
		}
	}
	return result;
}

static bool
sendAndRecv_byTls( ModuleTls* module, struct tls* ctx,
		ZnkHtpHdrs send_hdrs, ZnkBfr send_body,
		ZnkHtpHdrs recv_hdrs, ZnkHtpOnRecvFuncArg recv_fnca, ZnkHtpOnRecvFuncArg prog_fnca,
		ZnkVarpAry cookie, ZnkBfr wk_bfr, ZnkStr ermsg )
{
	ZnkErr_D( err );
	bool result = false;
	bool internal_wk_bfr = false;
	struct RecvInfo info = { 0 };
	ZnkStockBIO sbio = NULL;
	ZnkHtpReqMethod req_method = ZnkHtpReqMethod_getType_fromCStr(
			ZnkStrAry_at_cstr( send_hdrs->hdr1st_, 0 ) );

	info.module_    = module;
	info.ctx_       = ctx;
	info.ermsg_     = ermsg;
	info.prog_fnca_ = prog_fnca;
	sbio = ZnkStockBIO_create( 256, recvTls, (void*)&info );

	if( wk_bfr == NULL ){
		internal_wk_bfr = true;
		wk_bfr = ZnkBfr_create_null();
	}

	ZnkBfr_clear( wk_bfr );
	ZnkHtpHdrs_extendToStream( send_hdrs->hdr1st_, send_hdrs->vars_, wk_bfr, true );

	/* BodyImage‚ð’Ç‰Á */
	if( send_body ){
		ZnkBfr_append_dfr( wk_bfr, ZnkBfr_data(send_body), ZnkBfr_size(send_body) );
	}

	if( !writeBuf_byTls( wk_bfr, module, ctx, ermsg ) ){
		ZnkSysErrnoInfo* err_info = ZnkSysErrno_getInfo( ZnkSysErrno_errno() );
		ZnkErr_internf( &err,
				"ZnkSocket_send : Failure : SysErr=[%s:%s]",
				err_info->sys_errno_key_, err_info->sys_errno_msg_ );
		result = false;
		goto FUNC_END;
	}

	result = ZnkHtpSBIO_recv( sbio, req_method,
			recv_hdrs, recv_fnca,
			cookie, wk_bfr, ermsg );
	
FUNC_END:
	if( internal_wk_bfr ){
		ZnkBfr_destroy( wk_bfr );
	}
	ZnkStockBIO_destroy( sbio );
	return result;
}

static int
writeFP( void* arg, const uint8_t* buf, size_t buf_size )
{
	ZnkFile fp = (ZnkFile)arg;
	const size_t result_size = Znk_fwrite( buf, buf_size, fp );
	return result_size;
}

bool
TlsModule_getHtpsProcess( const char* cnct_hostname, uint16_t cnct_port,
		ZnkHtpHdrs send_hdrs, ZnkBfr send_body,
		ZnkHtpHdrs recv_hdrs, ZnkHtpOnRecvFuncArg recv_fnca, ZnkHtpOnRecvFuncArg prog_fnca, 
		ZnkVarpAry cookie,
		bool is_proxy, ZnkBfr wk_bfr, ZnkStr ermsg )
{
	bool result = false;
	ModuleTls* module = theModuleTlsBoth( st_lib_basename );
	if( module == NULL ){
		if( ermsg ){
			ZnkStr_addf( ermsg, "TlsModule_getHtpsProcess : Error : module is NULL. lib_basename=[%s]", st_lib_basename );
		}
	} else {
		struct tls *ctx = NULL;
		struct tls_config* config = NULL;
		
		if( module->tls_init_() != 0 ){
			if( ermsg ){
				ZnkStr_addf( ermsg, "TlsModule_getHtpsProcess : Error : tls_init." );
			}
			goto FUNC_END;
		}
	
		config = module->tls_config_new_();
		//module->tls_config_set_ca_file_( config, "cert.pem" );
		module->tls_config_set_ca_file_( config, st_cert_pem );
	
		ctx = module->tls_client_();
		if( ctx == NULL ){
			if( ermsg ){
				ZnkStr_addf( ermsg, "TlsModule_getHtpsProcess : Error : tls_client.");
			}
			goto FUNC_END;
		}
		
		module->tls_configure_( ctx, config );

		
		{
			char port_str[ 16 ] = "0";
			Znk_snprintf( port_str, sizeof(port_str), "%u", cnct_port );
			if( module->tls_connect_( ctx, cnct_hostname, port_str ) != 0 ){
				if( ermsg ){
					ZnkStr_addf( ermsg, "TlsModule_getHtpsProcess : Error : tls_connect: %s", module->tls_error_(ctx) );
				}
				goto FUNC_END;
			}
		}

		{
			const char* hostname = cnct_hostname;
			bool        is_need_modify_req_uri = false;
			ZnkVarp     varp   = ZnkHtpHdrs_find_literal( send_hdrs->vars_, "Host" );
			ZnkStrAry   hdr1st = send_hdrs->hdr1st_;
			if( varp ){
				hostname = ZnkVar_name_cstr( varp );
				if( is_proxy ){
					ZnkStr req_uri = ZnkStrAry_at( hdr1st, 1 );
					/* connect via proxy. */
					switch( ZnkStr_first(req_uri) ){
					case '/':
						/* req_uri begin from under-path */
						is_need_modify_req_uri = true;
						break;
					case '*':
					default:
						/* req_uri begin from hostname or * */
						break;
					}
				}
			}
			if( is_need_modify_req_uri ){
				ZnkStr tmp = ZnkStr_new( "" );
				ZnkStr req_uri = ZnkStrAry_at( hdr1st, 1 );
				ZnkStr_addf( tmp, "%s%s", hostname, ZnkStr_cstr( req_uri ) );
				ZnkStr_swap( tmp, req_uri );
				ZnkStr_delete( tmp );
			}

			sendAndRecv_byTls( module, ctx,
					send_hdrs, send_body,
					recv_hdrs, recv_fnca, prog_fnca,
					cookie, wk_bfr, ermsg );
		}
		
		
		result = true;
FUNC_END:
		if( module->tls_close_(ctx) != 0 ){
			if( ermsg ){
				ZnkStr_addf( ermsg, "TlsModule_getHtpsProcess : Error : tls_close: %s", module->tls_error_(ctx));
			}
		}
		module->tls_free_(ctx);
		module->tls_config_free_( config );
	}
	return result;
}

