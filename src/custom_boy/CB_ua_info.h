#ifndef INCLUDE_GUARD__CB_ua_info_h__
#define INCLUDE_GUARD__CB_ua_info_h__

#include <CB_ua_browser.h>
#include <CB_ua_os.h>
#include <CB_ua_machine.h>
#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

typedef struct CBUAInfo_tag {
	CBUABrowser browser_;
	size_t      major_ver_;
	size_t      minor_ver_;
	CBUAOS      os_;
	CBUAMachine machine_;
	size_t      sc_width_;
	size_t      sc_height_;
	size_t      sc_depth_;
} *CBUAInfo;

bool
CBUAInfo_load( CBUAInfo ua_info, const char* filename );
bool
CBUAInfo_load2( CBUAInfo ua_info, const char* filename, const char* sec_name );
bool
CBUAInfo_save( const CBUAInfo ua_info, const char* filename );
bool
CBUAInfo_save2( const CBUAInfo ua_info, const char* filename, const char* sec_name );
bool
CBUAInfo_init_byCBVars( CBUAInfo ua_info, ZnkVarpAry cb_vars );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
