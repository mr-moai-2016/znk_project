#ifndef RRA_PORTABLE_SNPRINTF_H
#define RRA_PORTABLE_SNPRINTF_H 1

#include <rra_portable/macros.h>
#include <stdarg.h>
#include <stddef.h>

BEGIN_DECLS

int vsnprintf( char* str, size_t count, const char* fmt, va_list args );

int snprintf( char* str, size_t count, const char* fmt,... );

END_DECLS

#endif /* !RRA_PORTABLE_SNPRINTF_H */
