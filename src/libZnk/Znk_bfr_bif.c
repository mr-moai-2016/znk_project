#include <Znk_bfr_bif.h>
#include <Znk_bfr.h>

static void znk_bfr_append( void* bfr, const uint8_t* src, size_t size ){
	ZnkBfr_append_dfr( (ZnkBfr)bfr, src, size );
}
static void znk_bfr_push_bk( void* bfr, uint8_t u8 ){
	ZnkBfr_push_bk( (ZnkBfr)bfr, u8 );
}
static void znk_bfr_reserve( void* bfr, size_t size ){
	ZnkBfr_reserve( (ZnkBfr)bfr, size );
}
static void znk_bfr_resize( void* bfr, size_t size ){
	ZnkBfr_resize( (ZnkBfr)bfr, size );
}
static size_t znk_bfr_size( const void* bfr ){
	return ZnkBfr_size( (ZnkBfr)bfr );
}
static uint8_t* znk_bfr_data( void* bfr ){
	return ZnkBfr_data( (ZnkBfr)bfr );
}
static void znk_bfr_swap( void* bfr1, void* bfr2 ){
	ZnkBfr_swap( (ZnkBfr)bfr1, (ZnkBfr)bfr2 );
}
bool ZnkBfrBif_create( ZnkBif bif )
{
	static struct ZnkBifModule_tag st_module = {
		znk_bfr_append,
		znk_bfr_push_bk,
		znk_bfr_reserve,
		znk_bfr_resize,
		znk_bfr_size,
		znk_bfr_data,
		znk_bfr_swap,
	};
	bif->module_ = &st_module;
	bif->bfr_ = ZnkBfr_create_null();
	return (bool)( bif->bfr_ != NULL );
}
void ZnkBfrBif_destroy( ZnkBif bif )
{
	if( bif ){
		ZnkBfr_destroy( bif->bfr_ );
	}
}

#if 0
ZnkBifModule C1Buf_theBifModule()
{
	static st_module = {
		c1buf_append,
		c1buf_push_bk,
		c1buf_reserve,
		c1buf_resize,
		c1buf_size,
		c1buf_data,
		c1buf_swap,
	};
	return &st_module;
}
bool C1Buf_createBif( ZnkBif bif, C1Buf c1buf )
{
	bif->module_ = C1Buf_theBifModule();
	bif->bfr_ = C1Buf_create( c1buf, NULL, 0 );
	return (bool)( bif->bfr_ != NULL );
}
void C1Buf_destroyBif( ZnkBif bif, bool is_heap )
{
	if( bif ){
		C1Buf_destroy( bif->bfr_, is_heap );
	}
}
test()
{
	struct ZnkBif_tag bif_instance = { 0 };
	ZnkBif bif = &bif_instance;
	ZnkBfr_createBif( bif );

	ZnkBif_push_bk( bif, 'A' );

	ZnkBfr_destroyBif( bif );
}
#endif

