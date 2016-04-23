#ifndef INCLUDE_GUARD__Znk_sys_errno_h__
#define INCLUDE_GUARD__Znk_sys_errno_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef struct {
	uintptr_t   sys_errno_;
	const char* sys_errno_key_;
	const char* sys_errno_msg_;
}ZnkSysErrnoInfo;

uint32_t
ZnkSysErrno_errno( void );

ZnkSysErrnoInfo*
ZnkSysErrno_getInfo( uint32_t sys_errno );


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
