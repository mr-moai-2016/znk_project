#ifndef INCLUDE_GUARD__Moai_myf_h__
#define INCLUDE_GUARD__Moai_myf_h__

#include <Znk_myf.h>

Znk_EXTERN_C_BEGIN

ZnkMyf
MoaiMyf_theConfig( void );
ZnkMyf
MoaiMyf_theTarget( void );
ZnkMyf
MoaiMyf_theAnalysis( void );

bool
MoaiMyf_loadConfig( void );
bool
MoaiMyf_loadTarget( void );
bool
MoaiMyf_loadAnalysis( void );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
