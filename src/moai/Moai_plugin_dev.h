#ifndef INCLUDE_GUARD__Moai_plugin_dev_h__
#define INCLUDE_GUARD__Moai_plugin_dev_h__

#include <Znk_myf.h>

Znk_EXTERN_C_BEGIN

/**
 * @brief
 *  target固有のsendフィルタの初期化処理を行う.
 *  Web Configurationの「Virtual USERS Initiation」ボタンを押すとこの関数が呼び出される.
 * 
 * @param ftr_send:
 *  初期化対象となるフィルタへの参照である.
 *  pluginはこの関数内でこの値を参照および変更してかまわない.
 * 
 * @param parent_proxy:
 *  Moaiが現在使用中の外部プロキシがhostname:portの形式で設定されている.
 *  (外部プロキシを使用していない場合はこの値が空値であるかまたはNONEであるかまたは:0が指定されている)
 *  pluginはこの関数内でこの値の参照は可能だが変更することはできない.
 * 
 * @param result_msg:
 *  ここにはこの関数の処理をおこなった結果のメッセージを格納しなければならない.
 *  これは処理が成功した場合はそれをリポートメッセージであり、エラーが発生した場合はそれを示す
 *  エラーメッセージとなる.
 *  「Virtual USERS Initiation」ボタンを押すとすぐ下にこのメッセージが表示される形になる.
 */
bool initiate( ZnkMyf ftr_send, const char* parent_proxy, ZnkStr result_msg );

/**
 * @brief
 *  target固有のPOST直前時の処理を行う.
 *  POST直前に毎回呼び出される.
 * 
 * @param ftr_send:
 *  処理対象となるフィルタへの参照である.
 *  pluginはこの関数内でこの値を参照および変更してかまわない.
 *
 * @param ftr_send:
 *  処理対象となるPOST変数の参照である.
 *  pluginはこの関数内でこの値を参照および変更してかまわない.
 */
bool on_post( ZnkMyf ftr_send, ZnkVarpAry post_vars );

/**
 * @brief
 * target固有のレスポンスヘッダを受け取った場合における処理を行う.
 * レスポンスを受け取った場合に毎回呼び出される.
 *
 * @param ftr_send:
 *  処理対象となるフィルタへの参照である.
 *  pluginはこの関数内でこの値を参照および変更してかまわない.
 *
 * @param hdr_vars:
 *  処理対象となるレスポンスヘッダへの参照である.
 *  pluginはこの関数内でこの値を参照および変更してかまわない.
 *
 * @param text:
 *  処理対象となるレスポンステキストデータへの参照である.
 *  pluginはこの関数内でこの値を参照および変更してかまわない.
 *
 * @param req_urp:
 *  処理対象となるURIのpath部分(URIにおけるオーソリティより後ろの部分)が渡される.
 *  pluginはこの関数内でこの値の参照は可能だが変更することはできない.
 */
bool on_response( ZnkMyf ftr_send,
		ZnkVarpAry hdr_vars, ZnkStr text, const char* req_urp );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
