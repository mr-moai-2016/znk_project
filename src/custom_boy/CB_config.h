#ifndef INCLUDE_GUARD__CB_config_h__
#define INCLUDE_GUARD__CB_config_h__

#include <Znk_str.h>
#include <Znk_myf.h>

Znk_EXTERN_C_BEGIN

typedef struct CBConfigInfo_tag {
	const char* ua_state_filename_;
	const char* send_myf_filename_;
	const char* send_save_myf_filename_;
	const char* negotiating_target_;
} CBConfigInfo;

CBConfigInfo*
CBConfig_theInfo( void );

void
CBConfig_setNegotiatingTarget( const char* target );
const char*
CBConfig_theNegotiatingTarget( void );

const char*
CBConfig_theCBSrc( void );

const char*
CBConfig_hint_base_Snp( void );
const char*
CBConfig_hint_base_PV( void );

bool
CBConfig_moai_dir_initiate( ZnkStr ermsg );

void
CBConfig_moai_dir_finalize( void );

const char*
CBConfig_moai_dir( void );

ZnkMyf
CBConfig_custom_boy_myf( void );
void
CBConfig_custom_boy_myf_destroy( void );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
