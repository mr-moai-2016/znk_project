#ifndef INCLUDE_GUARD__Moai_type_h__
#define INCLUDE_GUARD__Moai_type_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 MoaiRASResult_e_OK=0
	,MoaiRASResult_e_Ignored
	,MoaiRASResult_e_CriticalError
	,MoaiRASResult_e_RestartServer
	,MoaiRASResult_e_AutoUpdate
	,MoaiRASResult_e_RestartProcess
} MoaiRASResult;

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
