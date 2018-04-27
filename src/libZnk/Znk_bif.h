#ifndef INCLUDE_GUARD__Znk_bif_h__
#define INCLUDE_GUARD__Znk_bif_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

/***
 * byte配列をコンバートする処理は大抵の場合これらだけで実現できる.
 * erase や replace 系はほとんど使われることはない.
 * (万一これらが必要となった場合は data_とresize_とmemmoveの組み合わせで強引に実現することもできる)
 *
 * 尚、push_bk_は明らかにappend_の特別な場合だが、効率のため加えてある.
 * createの抽象化はうまくいかない.
 * 強引にやろうとしても実装もインターフェースも複雑化し、かえって使いにくくなるだけなのでここでは追加しない.
 * またこれは寿命管理するコンテナというわけでもないのでdestroyの抽象化も行わないものとする.
 */
typedef void     (*ZnkBifFuncT_append )( void* bfr, const uint8_t* src, size_t src_size );
typedef void     (*ZnkBifFuncT_push_bk)( void* bfr, uint8_t u8 );
typedef void     (*ZnkBifFuncT_reserve)( void* bfr, size_t size );
typedef void     (*ZnkBifFuncT_resize )( void* bfr, size_t size );
typedef size_t   (*ZnkBifFuncT_size)( const void* bfr );
typedef uint8_t* (*ZnkBifFuncT_data)( void* bfr );
typedef void     (*ZnkBifFuncT_swap)( void* bfr1, void* bfr2 );

typedef struct ZnkBifModule_tag {
	ZnkBifFuncT_append  append_;
	ZnkBifFuncT_push_bk push_bk_;
	ZnkBifFuncT_reserve reserve_;
	ZnkBifFuncT_resize  resize_;
	ZnkBifFuncT_size    size_;
	ZnkBifFuncT_data    data_;
	ZnkBifFuncT_swap    swap_;
}* ZnkBifModule;

typedef struct ZnkBif_tag {
	ZnkBifModule module_;
	void*        bfr_;
}* ZnkBif;

Znk_INLINE void ZnkBif_append( ZnkBif bif, const uint8_t* src, size_t src_leng ){
	bif->module_->append_( bif->bfr_, src, src_leng );
}
Znk_INLINE void ZnkBif_push_bk( ZnkBif bif, uint8_t u8 ){
	bif->module_->push_bk_( bif->bfr_, u8 );
}
Znk_INLINE void ZnkBif_reserve( ZnkBif bif, size_t size ){
	bif->module_->reserve_( bif->bfr_, size );
}
Znk_INLINE void ZnkBif_resize( ZnkBif bif, size_t size ){
	bif->module_->resize_( bif->bfr_, size );
}
Znk_INLINE size_t ZnkBif_size( const ZnkBif bif ){
	return bif->module_->size_( bif->bfr_ );
}
Znk_INLINE uint8_t* ZnkBif_data( ZnkBif bif ){
	return bif->module_->data_( bif->bfr_ );
}
Znk_INLINE void ZnkBif_swap( ZnkBif bif1, ZnkBif bif2 ){
	bif1->module_->swap_( bif1->bfr_, bif2->bfr_ );
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
