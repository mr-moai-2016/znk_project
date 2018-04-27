#ifndef INCLUDE_GUARD__CB_fgp_info_h__
#define INCLUDE_GUARD__CB_fgp_info_h__

#include <Znk_varp_ary.h>
#include <CB_ua_info.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 CBFgpStateMode_e_Uninitialized
	,CBFgpStateMode_e_SaveFile
	,CBFgpStateMode_e_StockContext
	,CBFgpStateMode_e_CBGetting
}CBFgpStateMode;

typedef struct CBFgpInfo_tag* CBFgpInfo;
struct CBFgpInfo_tag {
	ZnkStr Fgp_userAgent_;
	ZnkStr Fgp_language_;
	ZnkStr Fgp_colorDepth_;
	ZnkStr Fgp_ScreenResolution_;
	ZnkStr Fgp_TimezoneOffset_;
	ZnkStr Fgp_SessionStorage_;
	ZnkStr Fgp_LocalStorage_;
	ZnkStr Fgp_IndexedDB_;
	ZnkStr Fgp_BodyAddBehavior_;
	ZnkStr Fgp_OpenDatabase_;
	ZnkStr Fgp_CpuClass_;
	ZnkStr Fgp_Platform_;
	ZnkStr Fgp_doNotTrack_;
	ZnkStr Fgp_PluginsString_;
};

CBFgpInfo
CBFgpInfo_create( void );

void
CBFgpInfo_destroy( CBFgpInfo fgp_info );

void
CBFgpInfo_init_byCBVars( CBFgpInfo fgp_info, ZnkVarpAry cb_vars );

bool
CBFgpInfo_load( CBFgpInfo fgp_info, const char* filename, const char* sec_name );

bool
CBFgpInfo_save( const CBFgpInfo fgp_info, const char* filename, const char* sec_name );

void
CBFgpInfo_proc_byStateMode( CBFgpInfo fgp_info, CBFgpStateMode state_mode );

void
CBFgpInfo_generate_byCBUAInfo( CBFgpInfo fgp_info, ZnkStr RE_key, const CBUAInfo ua_info, const double RE_random_factor );

uint32_t
CBFgpInfo_calcHash( const CBFgpInfo fgp_info,
		bool with_UA, bool with_PluginStrings, bool with_ScreenResolution, const char* RE_key );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
