#ifndef INCLUDE_GUARD__Moai_module_h__
#define INCLUDE_GUARD__Moai_module_h__

#include <Znk_varp_ary.h>
#include <Znk_myf.h>
#include <Znk_txt_filter.h>

Znk_EXTERN_C_BEGIN

typedef struct MoaiModuleImpl* MoaiModule;

typedef bool (*MoaiInitiateFunc)( ZnkMyf ftr_send, const char* parent_proxy,
		char* result_msg_buf, size_t result_msg_buf_size );
typedef bool (*MoaiOnPostFunc)( ZnkMyf ftr_send );
typedef bool (*MoaiOnResponseHdr)( ZnkMyf ftr_send, ZnkVarpAry hdr_vars );

MoaiModule
MoaiModule_create( void );

void
MoaiModule_destroy( MoaiModule mod );

bool
MoaiModule_load( MoaiModule mod, const char* target_name );

bool
MoaiModule_saveFilter( const MoaiModule mod );

const char*
MoaiModule_targe_name( const MoaiModule mod );

ZnkMyf
MoaiModule_ftrSend( const MoaiModule mod );

ZnkTxtFilterAry
MoaiModule_ftrHtml( const MoaiModule mod );
ZnkTxtFilterAry
MoaiModule_ftrJS( const MoaiModule mod );
ZnkTxtFilterAry
MoaiModule_ftrCSS( const MoaiModule mod );

bool
MoaiModule_invokeInitiate( const MoaiModule mod, const char* parent_proxy,
		char* result_msg_buf, size_t result_msg_buf_size );
bool
MoaiModule_invokeOnPostBefore( const MoaiModule mod );
bool
MoaiModule_invokeOnResponseHdr( const MoaiModule mod, ZnkVarpAry hdr_vars );

/**
 * @brief
 * ヘッダ変数の配列 hdr_vars を フィルタ ftr_send でフィルタリングする.
 */
size_t
MoaiModule_filtHtpHeader( const MoaiModule mod, ZnkVarpAry hdr_vars );

/**
 * @brief
 * POST変数の配列 post_vars を フィルタ ftr_send でフィルタリングする.
 */
size_t
MoaiModule_filtPostVars( const MoaiModule mod, ZnkVarpAry post_vars );

/**
 * @brief
 * ヘッダ変数の配列 hdr_vars 内にCookieフィールドが存在する場合は、
 * それを フィルタ ftr_send でフィルタリングする.
 */
bool
MoaiModule_filtCookieVars( const MoaiModule mod, ZnkVarpAry hdr_vars );

/**
 * @brief
 * 指定した hostname がこのMoaiModuleの処理対象であるか否かを返す.
 */
bool
MoaiModule_isTargetHost( const MoaiModule mod, const ZnkMyf mtgt, const char* hostname );


const char*
MoaiTarget_findTargetName( const ZnkMyf mtgt, const char* hostname );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
