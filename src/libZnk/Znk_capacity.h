#ifndef INCLUDE_GUARD__Znk_capacity_h__
#define INCLUDE_GUARD__Znk_capacity_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 ZnkCapacity_Exp2  =  0
	,ZnkCapacity_Pad8  =  3
	,ZnkCapacity_Pad16
	,ZnkCapacity_Pad32
	,ZnkCapacity_Pad64
	,ZnkCapacity_Pad128
	,ZnkCapacity_Pad256
	,ZnkCapacity_Pad512
	,ZnkCapacity_Pad1024
	,ZnkCapacity_Pad2048
	,ZnkCapacity_Pad4096
} ZnkCapacityType;

size_t
ZnkCapacity_calc( size_t size, ZnkCapacityType type );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
