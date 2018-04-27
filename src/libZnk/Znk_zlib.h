#ifndef INCLUDE_GUARD__Znk_zlib_h__
#define INCLUDE_GUARD__Znk_zlib_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

typedef unsigned int (*ZnkZStreamIOFunc)( uint8_t* buf, size_t buf_size, void* arg );

bool
ZnkZlib_initiate( void );

void
ZnkZlib_finalize( void );

typedef struct ZnkZStreamImpl* ZnkZStream;

ZnkZStream
ZnkZStream_create( void );
void
ZnkZStream_destroy( ZnkZStream zst );
bool
ZnkZStream_inflateInit( ZnkZStream zst, ZnkStr emsg );
bool
ZnkZStream_inflate( ZnkZStream zst,
		uint8_t* dst_buf, size_t dst_size, const uint8_t* src_buf, size_t src_size,
		size_t* expanded_dst_size, size_t* expanded_src_size,
		ZnkStr emsg );
bool
ZnkZStream_inflate2( ZnkZStream zst,
		uint8_t* dst_buf, size_t dst_size, ZnkZStreamIOFunc supplyDst, void* dst_arg,
		uint8_t* src_buf, size_t src_size, ZnkZStreamIOFunc demandSrc, void* src_arg,
		ZnkStr emsg );
bool
ZnkZStream_inflateEnd( ZnkZStream zst, ZnkStr emsg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
