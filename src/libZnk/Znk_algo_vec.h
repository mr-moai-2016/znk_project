#ifndef INCLUDE_GUARD__Znk_algo_vec_h__
#define INCLUDE_GUARD__Znk_algo_vec_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef int  (*ZnkAlgoVecFuncT_compare)( const void* vec1, size_t vec1_idx, const void* vec2, size_t vec2_idx );
typedef void (*ZnkAlgoVecFuncT_swap)   ( void* vec1,       size_t vec1_idx, void* vec2,       size_t vec2_idx );

void
ZnkAlgoVec_quicksort( void* list, size_t pos, const size_t num,
		ZnkAlgoVecFuncT_compare func_compare,
		ZnkAlgoVecFuncT_swap    func_swap );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
