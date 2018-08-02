#ifndef INCLUDE_GUARD__Est_config_h__
#define INCLUDE_GUARD__Est_config_h__

#include <Rano_cgi_util.h>
#include <Rano_module.h>
#include <Znk_varp_ary.h>
#include <Znk_str_ary.h>
#include <Znk_myf.h>

Znk_EXTERN_C_BEGIN

const char*
EstConfig_getTmpDirPID( bool with_end_dsp );
const char*
EstConfig_getTmpDirCommon( bool with_end_dsp );
size_t
EstConfig_getSessionCount( void );
size_t
EstConfig_getShowFileNum( void );
size_t
EstConfig_getThumbSize( void );
size_t
EstConfig_getLineFileNum( void );
size_t
EstConfig_getLineTagNum( void );
size_t
EstConfig_getPreviewMaxWidth( void );
size_t
EstConfig_getPreviewMaxHeight( void );
size_t
EstConfig_getCacheDaysAgo( void );
size_t
EstConfig_getDustboxDaysAgo( void );
const char*
EstConfig_getExplicitReferer( void );

ZnkStr
EstConfig_getMoaiDir( void );
bool
EstConfig_loadAuthenticKey( char* authentic_key_buf, size_t authentic_key_buf_size, const char* moai_dir );

bool
EstConfig_initiate( RanoCGIEVar* evar, const char* moai_dir, size_t count );

void
EstConfig_finalize( void );

const char*
EstConfig_findTargetName( const char* line );
RanoModule
EstConfig_findModule( const char* query_target );
const char*
EstConfig_getTargetAndModule( RanoModule* modp, const char* hostname );

bool
EstConfig_isDirectImgLink( void );

bool
EstConfig_isAutoLink( void );

ZnkVarpAry
EstConfig_filterOnclickJSCall( void );

const char*
EstConfig_XhrDMZ( void );
const char*
EstConfig_XhrAuthHost( void );
bool
EstConfig_isXhrAuthHost( const RanoCGIEVar* evar );

const char*
EstConfig_authenticKey( void );
const char*
EstConfig_moai_dir( void );
const char*
EstConfig_filter_dir( void );
const char*
EstConfig_parent_proxy( void );

const char*
EstConfig_profile_dir( void );
const char*
EstConfig_favorite_dir( void );
const char*
EstConfig_stockbox_dir( void );
const char*
EstConfig_userbox_dir( void );
const char*
EstConfig_topics_dir( void );

ZnkStrAry
EstConfig_AllowJSHosts( void );

ZnkMyf
EstConfig_tags_myf( void );
ZnkMyf
EstConfig_target_myf( void );
ZnkMyf
EstConfig_hosts_myf( void );

void
EstConfig_saveTagsMyf( void );
void
EstConfig_saveEasterMyf( void );

void
EstConfig_readRecvHdrSetCookie( RanoModule mod, const char* target );
bool
EstConfig_isInputHiddensPostVarNames( const char* target, const char* query_varname );

void
EstConfigManager_main( RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr msg, const char* authentic_key );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
