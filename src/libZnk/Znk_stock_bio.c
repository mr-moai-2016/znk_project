#include "Znk_stock_bio.h"
#include "Znk_stdc.h"

struct ZnkStockBIOImpl {
	uint8_t*         stock_buf_;
	size_t           stock_buf_size_;
	uint8_t*         stock_cur_;
	size_t           stock_remain_size_; 
	size_t           readed_size_; /* 最後のread関数によって実際に取得されたデータサイズ */
	ZnkStockRecvFunc recver_func_;
	void*            recver_arg_;
	int              recver_result_; /* 最後のrecver_func_によってreturnされた値 */
};


ZnkStockBIO
ZnkStockBIO_create( size_t stock_buf_size, ZnkStockRecvFunc recver_func, void* recver_arg )
{
	ZnkStockBIO sbio = (ZnkStockBIO)Znk_malloc( sizeof( struct ZnkStockBIOImpl ) );
	sbio->stock_buf_ = (uint8_t*)Znk_malloc( stock_buf_size );
	sbio->stock_cur_ = NULL;
	sbio->stock_buf_size_    = stock_buf_size;
	sbio->stock_remain_size_ = 0;
	sbio->recver_func_ = recver_func;
	sbio->recver_arg_  = recver_arg;
	sbio->recver_result_ = 0;
	return sbio;
}
void
ZnkStockBIO_destroy( ZnkStockBIO sbio )
{
	if( sbio ){
		Znk_DELETE_PTR( sbio->stock_buf_, Znk_free, NULL );
		Znk_free( sbio );
	}
}
void
ZnkStockBIO_clear( ZnkStockBIO sbio, ZnkStockRecvFunc recver_func, void* recver_arg )
{
	sbio->stock_cur_ = NULL;
	sbio->stock_remain_size_ = 0;
	sbio->readed_size_ = 0;
	sbio->recver_func_ = recver_func;
	sbio->recver_arg_  = recver_arg;
	sbio->recver_result_ = 0;
}

size_t
ZnkStockBIO_getReadedSize( const ZnkStockBIO sbio )
{
	return sbio->readed_size_;
}
int
ZnkStockBIO_getRecverResult( const ZnkStockBIO sbio )
{
	return sbio->recver_result_;
}

static bool
gainData( ZnkStockBIO sbio, uint8_t* uc )
{
	if( sbio->stock_remain_size_ == 0 ){
		sbio->recver_result_ = (*sbio->recver_func_)( sbio->recver_arg_, sbio->stock_buf_, sbio->stock_buf_size_ );
		if( sbio->recver_result_ < 0 ){
			/* error of recv */
			return false;
		}
		sbio->stock_remain_size_ = (size_t)sbio->recver_result_;
		if( sbio->stock_remain_size_ == 0 ){
			/* end of recv */
			return false;
		}
		sbio->stock_cur_ = sbio->stock_buf_;
	}
	*uc = *sbio->stock_cur_++;
	sbio->stock_remain_size_--;
	return true;
}

bool
ZnkStockBIO_readLineCRLF( ZnkStockBIO sbio, char* dst_buf, size_t dst_buf_size )
{
	bool    result = false;
	uint8_t uc;
	size_t  idx = 0;
	while( idx < dst_buf_size ){
		if( !gainData( sbio, &uc ) ){
			/* recver end or error */
			result = false;
			goto FUNC_END;
		}
		if( uc == '\n' ){
			/* LF found */
			result = true;
			goto FUNC_END;
		}
		if( uc == '\r' ){
			/* skip */
			continue;
		} else {
			dst_buf[ idx ] = uc;
			++idx;
		}
	}
FUNC_END:
	/***
	 * 取得したデータのサイズがdst_buf_size未満の場合のみnull終端し
	 * そうでない場合はnull終端しない.
	 * これはfgetsの仕様に倣ったものである.
	 */
	if( idx < dst_buf_size ){
		dst_buf[ idx ] = '\0';
	}
	sbio->readed_size_ = idx;
	return result;
}
size_t
ZnkStockBIO_read( ZnkStockBIO sbio, uint8_t* dst_buf, size_t dst_buf_size )
{
	uint8_t uc;
	size_t  idx = 0;
	while( idx < dst_buf_size ){
		if( !gainData( sbio, &uc ) ){
			/* recver end or error */
			goto FUNC_END;
		}
		dst_buf[ idx ] = uc;
		++idx;
	}
FUNC_END:
	sbio->readed_size_ = idx;
	return idx;
}

