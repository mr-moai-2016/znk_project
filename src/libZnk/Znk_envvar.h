#ifndef INCLUDE_GUARD__Znk_envvar_h__
#define INCLUDE_GUARD__Znk_envvar_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

/**
 * @brief
 * ZnkEnvVar_freeが呼ばれるまでの間、
 * 戻り値は文字列として自由に使用できる.
 */
char*
ZnkEnvVar_get( const char* varname );

/**
 * @brief
 * valとしてNULLを与えた場合、
 * varnameという名前の環境変数を消去する.
 */
void
ZnkEnvVar_set( const char* varname, const char* val );

/**
 * @brief
 * ZnkEnvVar_getで得た値は必ずこの関数で解放しなければならない.
 */
void
ZnkEnvVar_free( char* val );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
