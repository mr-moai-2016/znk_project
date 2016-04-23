#ifndef INCLUDE_GUARD__Znk_tk_base_h__
#define INCLUDE_GUARD__Znk_tk_base_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__) \
    || defined(__BORLANDC__) || defined(__DMC__) \
    || defined(MAC_OSX_TK)
#  define ZnkTk_USE_TKXLIB 1
#endif

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
