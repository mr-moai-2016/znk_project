#ifndef INCLUDE_GUARD__Rano_hash_h__
#define INCLUDE_GUARD__Rano_hash_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

uint32_t
RanoHash_murmurhash3_32_gc( const char* key, size_t key_length, uint32_t seed );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
