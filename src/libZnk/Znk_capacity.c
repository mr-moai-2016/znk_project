#include <Znk_capacity.h>

#define GET_EXP2_CAPACITY( size, base ) \
		( (size) <= ((base)>>2) ) ? \
		    ( (size) <= ((base)>>3) ? ((base)>>3) : ((base)>>2) ) \
		:   ( (size) <= ((base)>>1) ? ((base)>>1) : (base) )

#define GET_PAD_E2( size, pad ) ( ((size) + (pad)-1 ) & ~((pad)-1) )

Znk_INLINE size_t
I_calcCapacityExp2( size_t size )
{
	if( size <= 0x00000100 ){
		size = GET_EXP2_CAPACITY( size, 0x00000100 );
	} else if( size <= 0x00001000 ){
		size = GET_EXP2_CAPACITY( size, 0x00001000 );
	} else if( size <= 0x00010000 ){
		size = GET_EXP2_CAPACITY( size, 0x00010000 );
	} else if( size <= 0x00100000 ){
		size = GET_EXP2_CAPACITY( size, 0x00100000 );
	} else if( size <= 0x01000000 ){
		size = GET_EXP2_CAPACITY( size, 0x01000000 );
	} else {
		size = GET_PAD_E2( size, 0x01000000 );
	}
	return size;
}

size_t
ZnkCapacity_calc( size_t size, ZnkCapacityType type )
{
	switch( type ){
	case ZnkCapacity_Pad8:    return GET_PAD_E2( size, 8 );
	case ZnkCapacity_Pad16:   return GET_PAD_E2( size, 16 );
	case ZnkCapacity_Pad32:   return GET_PAD_E2( size, 32 );
	case ZnkCapacity_Pad64:   return GET_PAD_E2( size, 64 );
	case ZnkCapacity_Pad128:  return GET_PAD_E2( size, 128 );
	case ZnkCapacity_Pad256:  return GET_PAD_E2( size, 256 );
	case ZnkCapacity_Pad512:  return GET_PAD_E2( size, 512 );
	case ZnkCapacity_Pad1024: return GET_PAD_E2( size, 1024 );
	case ZnkCapacity_Pad2048: return GET_PAD_E2( size, 2048 );
	case ZnkCapacity_Pad4096: return GET_PAD_E2( size, 4096 );
	case ZnkCapacity_Exp2:
	default:
		break;
	}
	return I_calcCapacityExp2( size );
}

