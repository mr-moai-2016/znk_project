#include <Znk_zlib.h>
#include <Znk_dlhlp.h>
#include <Znk_stdc.h>
#include <assert.h>
#include "zlib/zlib.h"

static char*
getZLibStatusStr( int status )
{
	switch( status ){
	case Znk_BIND_STR( :return, Z_OK );
	case Znk_BIND_STR( :return, Z_STREAM_END );
	case Znk_BIND_STR( :return, Z_NEED_DICT );
	case Znk_BIND_STR( :return, Z_ERRNO );
	case Znk_BIND_STR( :return, Z_STREAM_ERROR );
	case Znk_BIND_STR( :return, Z_DATA_ERROR );
	case Znk_BIND_STR( :return, Z_MEM_ERROR );
	case Znk_BIND_STR( :return, Z_BUF_ERROR );
	case Znk_BIND_STR( :return, Z_VERSION_ERROR );
	default:
		break;
	}
	return "UnknownZLibStatus";
}

/*****************************************************************************/
/* DLink Module */

typedef int ZEXPORT (*FuncType_inflateInit2_) OF((z_streamp strm, int  windowBits,
                              const char *version, int stream_size));
typedef int ZEXPORT (*FuncType_inflate) OF((z_streamp strm, int flush));
typedef int ZEXPORT (*FuncType_inflateEnd) OF((z_streamp strm));

typedef struct {
	ZnkDLinkHandler handler_;
	FuncType_inflateInit2_ inflateInit2__;
	FuncType_inflate       inflate_;
	FuncType_inflateEnd    inflateEnd_;
} ModuleZLib;

static ModuleZLib* theModuleZLib( bool auto_load, const char* dlib_name ){
	Znk_DLHLP_THE_MODULE_BEGIN( ModuleZLib )
	Znk_DLHLP_GET_FUNC( inflateInit2_ );
	Znk_DLHLP_GET_FUNC( inflate );
	Znk_DLHLP_GET_FUNC( inflateEnd );
	Znk_DLHLP_THE_MODULE_END
}

static Znk_DLHLP_DECLARE_UNLOAD_MODULE( unloadModuleZLib, ModuleZLib )

/* endof DLink Module */
/*****************************************************************************/

static ModuleZLib* theModuleZLibSt( void )
{
	static ModuleZLib st_module = { NULL, };
	st_module.handler_ = NULL;
	st_module.inflateInit2__ = inflateInit2_;
	st_module.inflate_       = inflate;
	st_module.inflateEnd_    = inflateEnd;
	return &st_module;
}
static ModuleZLib* theModuleZLibBoth( void )
{
	static const bool is_dynamic_load = false;
	ModuleZLib* module = NULL;
	if( is_dynamic_load ){
#if defined(Znk_TARGET_WINDOWS)
		module = theModuleZLib( true, "zlib-1.2.3.dll" );
#else
		module = theModuleZLib( true, "libz.so.1.2.3" );
#endif
	} else {
		module = theModuleZLibSt();
	}
	return module;
}

bool
ZnkZlib_initiate( void )
{
	return (bool)( theModuleZLibBoth() != NULL );
}
void
ZnkZlib_finalize( void )
{
	unloadModuleZLib();
}

struct ZnkZStreamImpl {
	z_stream    zs_;
	ModuleZLib* module_;
};
ZnkZStream
ZnkZStream_create( void )
{
	ZnkZStream zst = Znk_malloc( sizeof( struct ZnkZStreamImpl ) );
	zst->module_ = theModuleZLibBoth();
	if( zst->module_ == NULL ){
		return NULL;
	}
	Znk_memset(&zst->zs_, 0, sizeof(z_stream));
	return zst;
}
void
ZnkZStream_destroy( ZnkZStream zst )
{
	if( zst ){
		Znk_free( zst );
	}
}
bool
ZnkZStream_inflateInit( ZnkZStream zst, ZnkStr emsg )
{
	int status;
	/***
	 * zipヘッダ有    : MAX_WBITS
	 * gzipヘッダ有   : MAX_WBITS+16 
	 * zip/gzipいずれかのヘッダ有(自動判別) : MAX_WBITS+32(=47)
	 * ヘッダなしのRowデータ : -MAX_WBITS
	 */
	//int windowBits = -MAX_WBITS;
	int windowBits = MAX_WBITS+32;
	
	// Initialize members to use.
	Znk_memset(&zst->zs_, 0, sizeof(z_stream));
	zst->zs_.zalloc   = Z_NULL;
	zst->zs_.zfree    = Z_NULL;
	zst->zs_.opaque   = Z_NULL;
	zst->zs_.next_in  = Z_NULL;
	zst->zs_.avail_in = 0;
	
	status = zst->module_->inflateInit2__( &zst->zs_, windowBits,
			ZLIB_VERSION, sizeof(z_stream) );
	if( status != Z_OK ){
		if( emsg ){
			ZnkStr_addf( emsg, "ZnkZStream_inflateInit : Error : status=[%s]", getZLibStatusStr(status) );
		}
		return false;
	}
	return true;
}
bool
ZnkZStream_inflate( ZnkZStream zst,
		uint8_t* dst_buf, size_t dst_size, const uint8_t* src_buf, size_t src_size,
		size_t* expanded_dst_size, size_t* expanded_src_size,
		ZnkStr emsg )
{
	int status;
	
	// Configure In and Out pointers and the sizes for decompress.
	zst->zs_.next_out  = (Bytef*)dst_buf;
	zst->zs_.avail_out = (uLongf)dst_size;
	zst->zs_.next_in   = (Bytef*)src_buf;
	zst->zs_.avail_in  = (uInt)src_size;
	
	// Decompress data.
	status = zst->module_->inflate_( &zst->zs_, Z_NO_FLUSH );
	if( status == Z_STREAM_END ){
		goto FUNC_END;
	}
	if( status != Z_OK ){
		if( emsg ){
			ZnkStr_addf( emsg, "ZnkZStream_inflate : Error : status=[%s]", getZLibStatusStr(status) );
		}
		*expanded_dst_size = dst_size - zst->zs_.avail_out;
		*expanded_src_size = src_size - zst->zs_.avail_in;
		return false;
	}
FUNC_END:
	*expanded_dst_size = dst_size - zst->zs_.avail_out;
	*expanded_src_size = src_size - zst->zs_.avail_in;
	return true;
}
bool
ZnkZStream_inflate2( ZnkZStream zst,
		uint8_t* dst_buf, size_t dst_size, ZnkZStreamIOFunc supplyDst, void* dst_arg,
		uint8_t* src_buf, size_t src_size, ZnkZStreamIOFunc demandSrc, void* src_arg,
		ZnkStr emsg )
{
	int status = Z_OK;
	
	zst->zs_.avail_in  = 0;
	zst->zs_.next_out  = dst_buf;
	zst->zs_.avail_out = dst_size;

	while( status != Z_STREAM_END ){
		if( zst->zs_.avail_in == 0 ){
			zst->zs_.next_in  = (Bytef*)src_buf;
			zst->zs_.avail_in = demandSrc( src_buf, src_size, src_arg );
		}
		status = zst->module_->inflate_( &zst->zs_, Z_NO_FLUSH );
		if( status == Z_STREAM_END ) break;
		if( status != Z_OK ){
			if( emsg ){
				ZnkStr_addf( emsg, "ZnkZStream_inflate2 : Error : status=[%s]", getZLibStatusStr( status ) );
				ZnkStr_addf( emsg, " zs.msg=[%s]", (zst->zs_.msg) );
			}
			return false;
		}
		if( zst->zs_.avail_out == 0 ){
			supplyDst( dst_buf, dst_size, dst_arg );
			zst->zs_.next_out  = dst_buf;
			zst->zs_.avail_out = dst_size;
		}
	}

	/* 残り */
	{
		const size_t dst_remain = dst_size - zst->zs_.avail_out;
		if( dst_remain ){
			supplyDst( dst_buf, dst_remain, dst_arg );
		}
	}
	return true;
}
bool
ZnkZStream_inflateEnd( ZnkZStream zst, ZnkStr emsg )
{
	/* Clean up */
	int status = zst->module_->inflateEnd_( &zst->zs_ );
	if( status != Z_OK ){
		if( emsg ){
			ZnkStr_addf( emsg, "ZnkZStream_inflateEnd : Error : status=[%s]", getZLibStatusStr(status) );
		}
		return false;
	}
	return true;
}

