#ifndef INCLUDE_GUARD__Znk_def_util_h__
#define INCLUDE_GUARD__Znk_def_util_h__

#include <Znk_base.h>

#define Znk_CLAMP_MIN( val, min ) ( ((val)<(min)) ? (val)=(min) : (val) )
#define Znk_CLAMP_MAX( val, max ) ( ((val)>(max)) ? (val)=(max) : (val) )
#define Znk_CLAMP_MINMAX( val, min, max ) \
	( ((val)>(max)) ? (val)=(max) : Znk_CLAMP_MIN((val),(min)) )
#define Znk_MIN(x,y) ( (x)<(y) ? (x) : (y) )
#define Znk_MAX(x,y) ( (x)>(y) ? (x) : (y) )

#endif /* INCLUDE_GUARD */
