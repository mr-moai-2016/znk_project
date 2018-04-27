#ifndef INCLUDE_GUARD__CB_ua_conf_h__
#define INCLUDE_GUARD__CB_ua_conf_h__

#include <Znk_str.h>
#include <CB_snippet.h>

Znk_EXTERN_C_BEGIN

bool
CBUAConf_getUA( ZnkStr ua_str, const char* browser, int* major_ver, CBUAOS* uaos, CBUAMachine* uamach, CBUABrowser* uabrowser );
bool
CBUAConf_getScsz( ZnkStr scsz_str );
bool
CBUAConf_getBrowser_fromCategory( ZnkStr browser, const char* category );
bool
CBUAConf_getREPath( ZnkStr ans_path, const char* RE_key );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
