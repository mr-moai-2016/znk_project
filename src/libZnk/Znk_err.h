#ifndef INCLUDE_GUARD__Znk_err_h__
#define INCLUDE_GUARD__Znk_err_h__

#include <Znk_base.h>
#include <Znk_str.h>
#include <Znk_s_atom.h>

Znk_EXTERN_C_BEGIN

/**
 * @brief
 *   ZnkErr型の定義.
 *   ユーザは 内部メンバ pr__ にアクセスしてはならない.
 */
typedef struct { ZnkSAtomPr pr__; } ZnkErr;
#define ZnkErr_D( name ) ZnkErr name = {{ 0,0 }}

/***
 * 構造体にZnkErrメンバを持たせた場合に 0 クリアしたい場合に用いることを想定したもの.
 * この初期化は現状必須ではない.
 */
Znk_INLINE void ZnkErr_set_null( ZnkErr* err ){
	ZnkErr_D( tmp ); *err = tmp;
}

/**
 * @brief
 *   ZnkErr の内部AtomDBへ新しいエラーメッセージを登録し、
 *   そのIDとなるZnkErr(実体はZnkSAtomPr型である)を返す.
 *   この関数を呼び出すのにユーザはなんら準備はいらない.
 *   (ZnkErr_initiate関数を呼び出す必要性は特にない)
 *   internf 関数においては、printf形式のフォーマット指定を行うことができ、
 *   その結果生成される文字列をキーとして内部AtomDBへ登録されるというものである.
 *
 * @param ans_err:
 *   internされた結果返されるZnkErr値を格納する先を示すポインタを指定する.
 *   errをユーザが取得する必要がない場合ここにNULLを指定することもできる.
 *   ただし、その場合でも内部でintern処理(つまり文字列とerrの登録)自体は行われる.
 *
 * @exam
 *   登録した文字列は、返却されたZnkErrをZnkErr_cstr関数に引き渡すことで使用することができる.
 *   以下に、ZnkErrの典型的な使用例を示す.
 *
 *     bool some_func( ZnkErr* ans_err )
 *     {
 *       // some process...
 *       if( ... ){
 *         // Error A.
 *         ZnkErr_intern( ans_err, "Error A occur." );
 *         return false;
 *       }
 *       if( ... ){
 *         // Error B. (use printf-like interface)
 *         ZnkErr_internf( ans_err, "Error B occur on idx=[%d].", idx );
 *         return false;
 *       }
 *       return true;
 *     }
 *
 *     void test()
 *     {
 *       ZnkErr_D( err ); // この0初期化は必須ではないが、行いたい場合はこのようにする.
 *       bool result = some_func( &err );
 *       if( !result ){
 *         // Error Messageの内容を表示
 *         fprintf( stderr, "ErrorMsg is [%s]\n", ZnkErr_cstr(err) );
 *       }
 *     }
 *
 * @note
 *   この関数はスレッドセーフである.
 */
void ZnkErr_intern(  ZnkErr* ans_err, const char* err_msg );
void ZnkErr_internf( ZnkErr* ans_err, const char* fmt, ... );

/**
 * @brief
 *   ZnkErr が示す文字列を返す.
 *   これが示す文字列とは、ZnkErr_intern(ZnkErr_internf)呼び出し時に登録した文字列である.
 *
 * @return
 *   ZnkErr が示す文字列のポインタであり、C文字列である.
 *   これが示す文字列ポインタは、ZnkErr_finalizeが呼ばれない限り無効になることはない.
 *
 * @note
 *   この関数はスレッドセーフである.
 */
const char* ZnkErr_cstr( const ZnkErr err );

void
ZnkErr_initiate( void );
void
ZnkErr_finalize( void );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
