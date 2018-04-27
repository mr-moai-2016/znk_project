#ifndef INCLUDE_GUARD__Rano_module_h__
#define INCLUDE_GUARD__Rano_module_h__

#include <Znk_varp_ary.h>
#include <Znk_myf.h>
#include <Rano_txt_filter.h>
#include <Rano_type.h>

Znk_EXTERN_C_BEGIN

typedef struct RanoModuleImpl* RanoModule;

typedef bool (*RanoInitiateFunc)( ZnkMyf ftr_send, const char* parent_proxy, ZnkStr result_msg );
typedef bool (*RanoOnPostFunc)( ZnkMyf ftr_send, ZnkVarpAry hdr_vars, ZnkVarpAry post_vars );
typedef bool (*RanoOnResponse)( ZnkMyf ftr_send, ZnkVarpAry hdr_vars, ZnkStr text, const char* req_urp );

RanoModule
RanoModule_create( void );

void
RanoModule_destroy( RanoModule mod );

bool
RanoModule_load( RanoModule mod, const char* target_name,
		const char* filters_dir, const char* plugins_dir );

bool
RanoModule_saveFilter( const RanoModule mod );

const char*
RanoModule_target_name( const RanoModule mod );

ZnkMyf
RanoModule_ftrSend( const RanoModule mod );

RanoTxtFilterAry
RanoModule_ftrHtml( const RanoModule mod );
RanoTxtFilterAry
RanoModule_ftrJS( const RanoModule mod );
RanoTxtFilterAry
RanoModule_ftrCSS( const RanoModule mod );
ZnkStrAry
RanoModule_ftrCSSAdditional( const RanoModule mod );

bool
RanoModule_invokeInitiate( const RanoModule mod, const char* parent_proxy, ZnkStr result_msg );
bool
RanoModule_invokeOnPost( const RanoModule mod, ZnkVarpAry hdr_vars, ZnkVarpAry post_vars, bool* updated );
bool
RanoModule_invokeOnResponse( const RanoModule mod, ZnkVarpAry hdr_vars, ZnkStr text, const char* req_urp );

/**
 * @brief
 * ヘッダ変数の配列 hdr_vars を フィルタ ftr_send でフィルタリングする.
 */
size_t
RanoModule_filtHtpHeader( const RanoModule mod, ZnkVarpAry hdr_vars );

/**
 * @brief
 * POST変数の配列 post_vars を フィルタ ftr_send でフィルタリングする.
 */
size_t
RanoModule_filtPostVars( const RanoModule mod, ZnkVarpAry post_vars );

/**
 * @brief
 * ヘッダ変数の配列 hdr_vars 内にCookieフィールドが存在する場合は、
 * それを フィルタ ftr_send でフィルタリングする.
 *
 * @param is_all_replace:
 *   ブラウザのCookieを完全隠蔽するモード.
 *   これがtrueの場合、cookie_vars/cookie_forceにない未知の変数がブラウザ側に残っている場合でも
 *   それを通さない.
 *   これがfalseの場合は、それをそのまま通す.
 *
 * @param extra_vars:
 *   cookie_vars, cookie_force のいずれにも含まれないCookie変数がブラウザ側に存在した場合
 *   その変数群を取得することができる.
 *   この情報が不要な場合はNULLを指定することもできる.
 */
size_t
RanoModule_filtCookieVars( const RanoModule mod, ZnkVarpAry hdr_vars, bool is_all_replace, ZnkVarpAry extra_vars );

/**
 * @brief
 * サイトから発行されるSet-CookieによりCookieの値変更が指示された場合は
 * それを優先してfilterの値も変える.
 */
void
RanoFltr_updateCookieFilter_bySetCookie( const ZnkVarpAry hdr_vars, ZnkMyf ftr_send );

/**
 * @brief
 * 指定した hostname がこのRanoModuleの処理対象であるか否かを返す.
 */
bool
RanoModule_isTargetHost( const RanoModule mod, const ZnkMyf mtgt, const char* hostname );


const char*
RanoTarget_findTargetName( const ZnkMyf mtgt, const char* hostname );

void
RanoModule_filtTxt( const RanoModule mod, ZnkStr text, RanoTextType txt_type );

/**
 * @brief
 * filterファイルの更新時刻が異なる場合のみ再読み込み.
 * (現時点ではpluginは対象外)
 */
bool
RanoModule_reloadFilters_byDate( RanoModule mod, const char* target_name, const char* filters_dir );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
