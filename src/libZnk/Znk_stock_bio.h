#ifndef INCLUDE_GUARD__Znk_stock_bio_h__
#define INCLUDE_GUARD__Znk_stock_bio_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

/***
 * この関数は以下を満たすものとする.
 * 1. recvしたデータが1byte以上存在する場合はそのサイズ(byte)を返す.
 * 2. データが終了した場合は0を返す.
 * 3. エラーが発生した場合は-1を返す.
 */
typedef int (*ZnkStockRecvFunc)( void* arg, uint8_t* buf, size_t buf_size );

typedef struct ZnkStockBIOImpl* ZnkStockBIO;

ZnkStockBIO
ZnkStockBIO_create( size_t stck_buf_size, ZnkStockRecvFunc recver_func, void* recver_arg );
void
ZnkStockBIO_destroy( ZnkStockBIO sbio );
void
ZnkStockBIO_clear( ZnkStockBIO sbio, ZnkStockRecvFunc recver_func, void* recver_arg );

size_t
ZnkStockBIO_getReadedSize( const ZnkStockBIO sbio );
int
ZnkStockBIO_getRecverResult( const ZnkStockBIO sbio );

bool
ZnkStockBIO_readLineCRLF( ZnkStockBIO sbio, char* dst_buf, size_t dst_buf_size );

/**
 * @return
 * 戻り値はfreadの慣例に倣う.
 * 厳密にはfreadの戻り値はbyte値ではなくblkの個数であるが、
 * ここでは実際に読み込んだバイト数を返すものとする.
 */
size_t
ZnkStockBIO_read( ZnkStockBIO sbio, uint8_t* dst_buf, size_t dst_buf_size );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
