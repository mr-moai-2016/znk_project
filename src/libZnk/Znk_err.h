#ifndef INCLUDE_GUARD__Znk_err_h__
#define INCLUDE_GUARD__Znk_err_h__

#include <Znk_base.h>
#include <Znk_str.h>
#include <Znk_s_atom.h>

Znk_EXTERN_C_BEGIN

/***
 * ZnkErrはErrorメッセージを格納し保持する.
 * Error メッセージを格納するためにZnkStrを与えても同様のことはできるが
 * これら二つには主にパフォーマンスの点において違いがある.
 * 以下、これらの相違点を示す.
 *
 * ZnkErr:
 *   ZnkErrはErrorが実際に発生した場合にのみErrorメッセージ用の領域を新規に生成して格納し保持する.
 *   発生しなければその処理は行われない.
 *
 *   ユーザ側がErrorメッセージを必要とするか否かに関わらず、ユーザは単にZnkErr_D( err ); などと宣言すればよい.
 *   この宣言は単なるauto変数の宣言であり、メモリの動的確保などは一切行われないためこの宣言によるデメリットはないとみなせる.
 *   これを引数として配備する関数側も、これが必ず指定されるものとみなしてよく特別な配慮を行う必要はない.
 *
 *   エラーメッセージを１回だけ構築すれば事足りるような用途に向いている.
 *   リポートメッセージなど常に大量のメッセージの追加が発生するような用途には不向きである.
 *   なぜならば各メッセージの追加のたびに領域の生成処理が発生してしまうためである.
 *   その生成処理は当然ながら単なるZnkStr_addと比べて時間がかかる.
 *
 * ZnkStr:
 *   ユーザ側がErrorメッセージを必要とする場合は、
 *   実際にエラーが発生するか否かに関わらず、ZnkStrの生成と解放処理をユーザは行わなければならない.
 *   ユーザ側がErrorメッセージを必要としない場合は、これを引数として配備する関数側はNULLを指定された場合に
 *   これを無視するという仕様にするとユーザ側の負担を減らすことができる.
 *   しかしそうすると当然、関数側でZnkStr_addを呼ぶたびにNULLチェックを行わなければならないという負担は発生する.
 *
 *   一旦確保してしまえば、後は単なるZnkStr_addの呼び出しの連続でよく、これは軽量である.
 *   よってリポートメッセージなど常に大量のメッセージを取得するような用途に最も高速で対応できる.
 *   勿論エラーメッセージを格納するのと兼用してもよい.
 */

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
