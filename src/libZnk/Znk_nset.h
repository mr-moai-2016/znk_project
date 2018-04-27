#ifndef INCLUDE_GUARD__Znk_nset_h__
#define INCLUDE_GUARD__Znk_nset_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

/***
 * é©ëRêîÇÃèWçá(ÇΩÇæÇµ0Ç‡ä‹ÇﬂÇÈÇ±Ç∆Ç™Ç≈Ç´ÇÈ)
 */
Znk_DECLARE_HANDLE( ZnkNSet32 );

ZnkNSet32
ZnkNSet32_create( void );
void
ZnkNSet32_destroy( ZnkNSet32 nset );
uint32_t
ZnkNSet32_at( const ZnkNSet32 nset, size_t idx );
size_t
ZnkNSet32_size( const ZnkNSet32 nset );

size_t
ZnkNSet32_find( ZnkNSet32 nset, uint32_t query_val );

Znk_INLINE bool
ZnkNSet32_exist( ZnkNSet32 nset, uint32_t query_val ){
	return (bool)( ZnkNSet32_find( nset, query_val ) != Znk_NPOS );
}

void
ZnkNSet32_push( ZnkNSet32 nset, uint32_t val );
void
ZnkNSet32_erase( ZnkNSet32 nset, uint32_t val );
void
ZnkNSet32_copy( ZnkNSet32 dst, const ZnkNSet32 src );
bool
ZnkNSet32_eq( const ZnkNSet32 nset1, const ZnkNSet32 nset2 );

void
ZnkNSet32_print_e( const ZnkNSet32 nset );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */



