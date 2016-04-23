#include <Znk_zlib.h>
#include <Znk_bfr.h>
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

#if 0
static bool
uncompressData( Byte* strDest, uLongf* dest_size, const Byte* strSrc, size_t src_size )
{
	ModuleZLib* module = NULL;
	z_stream zs; /* Declare io stream structure for zlib. */
	int status;
	/***
	 * zipヘッダ有    : MAX_WBITS
	 * gzipヘッダ有   : MAX_WBITS+16 
	 * zip/gzipいずれかのヘッダ有(自動判別) : MAX_WBITS+32(=47)
	 * ヘッダなしのRowデータ : -MAX_WBITS
	 */
	//int windowBits = -MAX_WBITS;
	int windowBits = MAX_WBITS+32;

	module = theModuleZLibBoth();
	if( module == NULL ){
		return false;
	}
	
	// Initialize members to use.
	Znk_memset(&zs, 0, sizeof(z_stream));
	zs.zalloc   = Z_NULL;
	zs.zfree    = Z_NULL;
	zs.opaque   = Z_NULL;
	
	zs.next_in  = Z_NULL;
	zs.avail_in = 0;
	
	status = module->inflateInit2__( &zs, windowBits,
			ZLIB_VERSION, sizeof(z_stream) );
	if( status != Z_OK ){
		ZnkF_printf_e( "uncompressData : inflateInit2 error : status=[%d]\n", status );
		return false;
	}
	
	// Configure In and Out pointers and the sizes for decompress.
	zs.next_out  = (Bytef *)strDest;
	zs.avail_out = *dest_size;
	if( windowBits == MAX_WBITS+32 ){
		zs.next_in   = (Bytef *)strSrc;
		zs.avail_in  = (uInt)src_size;
	} else {
		/* Row data ? */
		zs.next_in   = (Bytef *)(strSrc + 10);  // Skip gz headers
		zs.avail_in  = (uInt)( src_size - 10 ); // Adjust total size available.
	}
	
	// Decompress data.
	status = Z_OK;
	while( status != Z_STREAM_END ){
		if( zs.avail_in == 0 ){
			ZnkF_printf_e( "uncompressData :  Error :zs.avail_in == 0 status=[%s]\n", getZLibStatusStr( status ) );
			break;
		}
		status = module->inflate_( &zs, Z_NO_FLUSH );
		if( status == Z_STREAM_END ){
			ZnkF_printf_e( "uncompressData : Z_STREAM_END.\n" );
			break;
		}
		if( status != Z_OK ){
			ZnkF_printf_e( "uncompressData : Error : status is not OK [%s]\n", getZLibStatusStr( status ) );
			break;
		}
		ZnkF_printf_e( "uncompressData : status=[%s].\n", getZLibStatusStr(status) );
	}
	
	// Clean up.
	status = module->inflateEnd_( &zs );
	if( status != Z_OK ){
		*dest_size = 0;
		ZnkF_printf_e( "inflateEnd error : status=[%d]\n", status );
		return false;
	}
	
	// The size of expanded size.
	*dest_size = *dest_size - zs.avail_out;
	return true;
}
bool
ZnkZlib_uncompress( ZnkBfr body )
{
	uLong uncomprLen = 1*1024*1024*8;
	Byte* uncompr = (Byte*)Znk_alloc0((uInt)uncomprLen);

	if( !uncompressData( uncompr, &uncomprLen, (const Byte*)ZnkBfr_data(body), ZnkBfr_size(body) ) ){
		ZnkF_printf_e("uncompressData error:\n");
		return false;
	}

	ZnkBfr_resize( body, 0 );
	ZnkBfr_append_dfr( body, (uint8_t*)uncompr, uncomprLen );
	return true;
}
#endif

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
ZnkZStream_inflateInit( ZnkZStream zst )
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
		ZnkF_printf_e( "inflateData : inflateInit2 error : status=[%d]\n", status );
		return false;
	}
	return true;
}
bool
ZnkZStream_inflate( ZnkZStream zst,
		uint8_t* dst_buf, size_t dst_size, const uint8_t* src_buf, size_t src_size,
		size_t* expanded_dst_size, size_t* expanded_src_size )
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
		ZnkF_printf_e( "inflateData : Z_STREAM_END.\n" );
		goto FUNC_END;
	}
	if( status != Z_OK ){
		ZnkF_printf_e( "inflateData : Error : status is not OK [%s]\n", getZLibStatusStr( status ) );
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
		uint8_t* src_buf, size_t src_size, ZnkZStreamIOFunc demandSrc, void* src_arg )
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
			ZnkF_printf_e( "ZnkZStream_inflate2 : Error : status is not OK [%s]\n", getZLibStatusStr( status ) );
			ZnkF_printf_e( "  msg=[%s]\n", (zst->zs_.msg) ? zst->zs_.msg : "???");
			assert( 0 );
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
ZnkZStream_inflateEnd( ZnkZStream zst )
{
	// Clean up.
	int status = zst->module_->inflateEnd_( &zst->zs_ );
	if( status != Z_OK ){
		ZnkF_printf_e( "inflateEnd error : status=[%d]\n", status );
		return false;
	}
	return true;
}

