#ifndef INCLUDE_GUARD__finger_h__
#define INCLUDE_GUARD__finger_h__

#include <CB_ua_info.h>
#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

#if 0
void
CBFinger_loadREData( const char* RE_key, ZnkStr str );
void
CBFinger_loadREPng( const char* RE_key, ZnkStr str );
#endif

void
CBFinger_Fgp_language( ZnkStr str, const CBUAInfo info, const char* RE );
void
CBFinger_Fgp_colorDepth( ZnkStr str, const CBUAInfo info, const char* RE );
void
CBFinger_Fgp_ScreenResolution( ZnkStr str, const CBUAInfo info, const char* RE );
void
CBFinger_Fgp_TimezoneOffset( ZnkStr str, const CBUAInfo info, const char* RE );
void
CBFinger_Fgp_SessionStorage( ZnkStr str, const CBUAInfo info, const char* RE );
void
CBFinger_Fgp_LocalStorage( ZnkStr str, const CBUAInfo info, const char* RE );
void
CBFinger_Fgp_IndexedDB( ZnkStr str, const CBUAInfo info, const char* RE );
void
CBFinger_Fgp_BodyAddBehavior( ZnkStr str, const CBUAInfo info, const char* RE );
void
CBFinger_Fgp_OpenDatabase( ZnkStr str, const CBUAInfo info, const char* RE );
void
CBFinger_Fgp_CpuClass( ZnkStr str, const CBUAInfo info, const char* RE );
void
CBFinger_Fgp_Platform( ZnkStr str, const CBUAInfo info, const char* RE );
void
CBFinger_Fgp_doNotTrack( ZnkStr str, const CBUAInfo info, const char* RE );
void
CBFinger_Fgp_PluginsString( ZnkStr str, const CBUAInfo info, const char* RE );
void
CBFinger_Fgp_Canvas( ZnkStr str, const char* RE );
bool
CBFinger_saveREPng( const char* file_path, ZnkStr str );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
