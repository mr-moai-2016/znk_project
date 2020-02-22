#ifndef INCLUDE_GUARD__Znk_vpod_aux_h__
#define INCLUDE_GUARD__Znk_vpod_aux_h__

#include <Znk_bfr.h>

Znk_EXTERN_C_BEGIN

/***
 * helper macros.
 */
#define ZnkVPod_M_CREATE( elem_type, size ) ZnkBfr_create( NULL, (size) * sizeof(elem_type), true, ZnkCapacity_Exp2 )

#define ZnkVPod_M_SIZE( bfr, elem_type ) ZnkBfr_size( bfr ) / sizeof( elem_type )

#define ZnkVPod_M_RESIZE( bfr, elem_type, size ) ZnkBfr_resize( bfr, (size) * sizeof( elem_type ) )

#define ZnkVPod_M_FILL_RESIZE_BIT( bfr, elem_type, size, val, bit ) do{ \
	const size_t old_size = ZnkVPod_M_SIZE( bfr, elem_type ); \
	if( (size) < old_size ){ \
		ZnkVPod_M_RESIZE( bfr, elem_type, (size) ); \
	} else if( (size) > old_size ){ \
		const size_t delta_size = (size) - old_size; \
		ZnkVPod_M_RESIZE( bfr, elem_type, (size) ); \
		ZnkBfr_fill_##bit( bfr, old_size * sizeof( elem_type ), (val), delta_size, Znk_isLE() ); \
	} else { /* none */ } \
} while(0)

#define ZnkVPod_M_FILL_RESIZE_BLOCK( bfr, elem_type, size, val ) do{ \
	const size_t old_size = ZnkVPod_M_SIZE( bfr, elem_type ); \
	if( (size) < old_size ){ \
		ZnkVPod_M_RESIZE( bfr, elem_type, (size) ); \
	} else if( (size) > old_size ){ \
		const size_t delta_size = (size) - old_size; \
		ZnkVPod_M_RESIZE( bfr, elem_type, (size) ); \
		ZnkBfr_fill_block( bfr, old_size * sizeof( elem_type ), (uint8_t*)&(val), sizeof( elem_type ), delta_size ); \
	} else { /* none */ } \
} while(0)

#define ZnkVPod_M_AT( bfr, elem_type, idx ) \
	(elem_type)( (const elem_type*)ZnkBfr_data(bfr) )[ idx ]

#define ZnkVPod_M_SET_BIT( bfr, idx, val, bit ) \
	ZnkBfr_set_aryval_##bit( bfr, idx, val, Znk_isLE() )

#define ZnkVPod_M_SET_BLOCK( bfr, idx, val ) \
	ZnkBfr_set_aryval_block( bfr, idx, (uint8_t*)&(val), sizeof(val) )

#define ZnkVPod_M_PUSH_BK_BIT( bfr, val, bit ) \
	ZnkBfr_push_bk_##bit( bfr, val, Znk_isLE() )

#define ZnkVPod_M_PUSH_BK_BLOCK( bfr, val ) \
	ZnkBfr_append_dfr( bfr, (uint8_t*)&(val), sizeof(val) )

#define ZnkVPod_M_POP_BK( bfr, elem_type ) \
	ZnkBfr_pop_bk_ex( bfr, NULL, sizeof(elem_type) )

#define ZnkVPod_M_REGIST_BIT( bfr, elem_type, idx, new_val, null_val, bit ) { \
	const size_t size = ZnkVPod_M_SIZE( bfr, elem_type ); \
	elem_type tmp; \
	for( idx=0; idx<size; ++idx ){ \
		tmp = ZnkVPod_M_AT( bfr, elem_type, idx ); \
		if( tmp == null_val ){ \
			ZnkVPod_M_SET_BIT( bfr, idx, new_val, bit ); \
			goto ZnkVPod_M_REGIST_END; \
		} \
	} \
	ZnkVPod_M_PUSH_BK_BIT( bfr, new_val, bit ); \
} ZnkVPod_M_REGIST_END:

#define ZnkVPod_M_REGIST_BLOCK( bfr, elem_type, idx, new_val, null_val ) { \
	const size_t size = ZnkVPod_M_SIZE( bfr, elem_type ); \
	elem_type tmp; \
	for( idx=0; idx<size; ++idx ){ \
		tmp = ZnkVPod_M_AT( bfr, elem_type, idx ); \
		if( tmp == null_val ){ \
			ZnkVPod_M_SET_BLOCK( bfr, idx, new_val ); \
			goto ZnkVPod_M_REGIST_END; \
		} \
	} \
	ZnkVPod_M_PUSH_BK_BLOCK( bfr, new_val ); \
} ZnkVPod_M_REGIST_END:

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
