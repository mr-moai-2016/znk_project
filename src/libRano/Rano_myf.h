#ifndef INCLUDE_GUARD__Rano_myf_h__
#define INCLUDE_GUARD__Rano_myf_h__

#include <Znk_myf.h>

Znk_EXTERN_C_BEGIN

ZnkMyf
RanoMyf_theConfig( void );
ZnkMyf
RanoMyf_theHosts( void );
ZnkMyf
RanoMyf_theTarget( void );
ZnkMyf
RanoMyf_theAnalysis( void );

bool
RanoMyf_loadConfig( void );
bool
RanoMyf_loadHosts( void );
bool
RanoMyf_loadTarget( void );
bool
RanoMyf_loadAnalysis( void );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
