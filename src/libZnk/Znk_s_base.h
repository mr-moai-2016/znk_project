#ifndef INCLUDE_GUARD__Znk_s_base_h__
#define INCLUDE_GUARD__Znk_s_base_h__

#include <Znk_stdc.h>
#include <Znk_s_posix.h>
#include <stdarg.h>

Znk_EXTERN_C_BEGIN

typedef struct {
	const char* cstr_;
	uintptr_t   leng_;
} ZnkSRef;

#define ZnkSRef_set_literal( sref, literal_str ) \
	(sref)->cstr_ = literal_str, (sref)->leng_ = Znk_strlen_literal( literal_str )


Znk_INLINE void
Znk_setStrLen_ifNPos( size_t* leng, const char* cstr ){
	if( *leng == Znk_NPOS ){ *leng = Znk_strlen(cstr); }
}



Znk_INLINE bool
ZnkS_empty( const char* str )
{
	return (bool)( str == NULL || str[0] == '\0' );
}
/**
 * @brief
 *   文字列を連結する.
 *   bufとcstrが指す領域は重なっていてもよい.
 *   buf内に'\0'が含まれていないような異常な文字列を与えてもよいが
 *   その場合なにもせずにNULLを返す.
 *   また連結後の文字列の長さがbuf_sizeを超える場合、バッファ終端で強制的に打ち切られ、
 *   最後に'\0'が付加される.
 *   この関数では strcatやstrncatのbuffer overflowの危険性がほぼ完全に排除されている.
 *   そのかわり速度は若干劣る.
 * @return
 *   bufにおける'\0'終端している位置を示すポインタ返す.
 *   (要するに連結後の文字列の終端位置である. この点がstrcatやstrncatの仕様と異なる点に注意)
 *   万が一buf内で'\0'が見つからない場合は何もせずNULLを返す.
 */
char*
ZnkS_concatEx( char* buf, size_t buf_size, const char* cstr, size_t cstr_leng );
char*
ZnkS_concat( char* buf, size_t buf_size, const char* cstr );

void
ZnkS_copy( char* buf, size_t buf_size, const char* cstr, size_t cstr_leng );

#define ZnkS_copy_literal( buf, buf_size, literal_str ) \
	ZnkS_copy( (buf), (buf_size), literal_str, Znk_strlen_literal(literal_str) )

/**
 * @brief
 *  いわゆるvsnprintfとsnprintf
 *  引数の与え方はおおよそ標準と同じだが、C99の新機能については
 *  サポートされていない場合もある. 戻り値についてはC99と異なる.
 *
 * @return
 *  最近のVC(というかPlatformSDKというべきか)ではvsnprintfがサポートされているが
 *  古いPlatformSDKを使っている場合、VC では _vsnprintf を使う必要がある. 
 *  この _vsnprintf の仕様がC99とは異なるため、ここでの戻り値は、多少工夫が必要である.
 *  以下のように取り決める. (下記でstr_lenとは、fmt と ap による合成結果の文字列の長さを示す.)
 *
 *   buf_size == 0 の場合
 *     0 以上の予測文字列長(下記noteを参照)が返る.
 *     buf に一切触れずなにもしない.
 *
 *   内部エラーが発生した場合
 *     -1 を返す.
 *     この関数を使う側はこれ以上の処理は中断するべきである.
 *
 *   str_len < buf_size の場合
 *     str_len が返る.
 *     buf[ str_len ] の位置にNULL文字が代入される.
 *
 *   str_len >= buf_size の場合
 *     buf_size以上の予測文字列長(下記noteを参照)が返る.
 *     buf[ buf_size-1 ] の位置にNULL文字が強制代入される.
 *     少なくとも1文字以上の文字列欠損が発生している.
 * 
 * @note:
 *  ※1)
 *  通常、C99の仕様では buf_size に敢えて 0 を指定することにより、文字列展開後の正確なサイズを
 *  戻り値として予め知ることができる.
 *  しかし、VCの _vsnprintf はこの機能がないため、やむを得ず予測文字列長という形にする.
 *  これは次にリトライするときにbuf_sizeに指定する値として加味して欲しい予測文字列長であり、
 *  多くの場合は次にこのサイズ + 1を指定することでうまくいくと思われるが、本当に十分であるという補償はないため、
 *  次のリトライでもユーザはその結果の戻り値をきちんと確認する必要がある.
 *
 *  (実装詳細なので本来この場に書くべきではないが)参考のため、現在サポートされている各コンパイラ
 *  環境毎にどのような値を返すかを以下に列挙しておく.
 *
 *  VCの場合:
 *    buf_size = 0 を指定した場合:
 *      まずstrchr( fmt, '%' ) により、fmt内にざっくりと%が含まれるか否かをスキャンする.
 *      そして % が存在する場合は strlen( fmt ) + 512 を返す.
 *      % が存在しない場合は strlen( fmt ) を返す.
 *    buf_size > 0 を指定した場合:
 *      展開後のOverが発生しなかった場合は、予測文字列長ではなく実際に展開され書き込まれた文字列長を返す.
 *      (多くの場合は、プログラマのbuf_size予想は正しいとして最初の一回目は指定されたbuf_sizeをそのまま試す).
 *      展開後のOverが発生した場合は、予測文字列長として MAX( strlen( fmt ) + 512, buf_size*2 ) を返す.
 *      (プログラマのbuf_size予想が外れる場合は、大抵の場合は大量の文字列が与えられた場合なので
 *      サイズは急速にgrowさせるべきと考えられる)
 *
 *  GCCなどC99対応しているvsnprintfが呼ばれる環境:
 *    buf_size = 0 を指定した場合:
 *      C99の仕様通りである.
 *      正確に展開後のサイズを予測文字列長として返す.
 *    buf_size > 0 を指定した場合:
 *      C99の仕様通りである.
 *      展開後のOverが発生しなかった場合は、実際に展開され書き込まれた文字列長を返す.
 *      展開後のOverが発生した場合は、正確に展開後のサイズを予測文字列長として返す.
 */
int // Znk_vsnprintf_C99 への移行が終わるまでしばらくの間、この後ろに__がついた妙な名前にしておく.
ZnkS_vsnprintf_sys__( char* buf, size_t buf_size, const char* fmt, va_list ap );
int
ZnkS_snprintf_sys__( char* buf, size_t buf_size, const char* fmt, ... );


/**
 * 文字列を10/16/8進数整数, double,float として解釈して結果を得る.
 * sscanfのラッパーである.
 * 成功した場合はtrue, さもなければfalseを返す.
 */
bool ZnkS_getIntD( int* ans, const char* str );
bool ZnkS_getIntX( int* ans, const char* str );
bool ZnkS_getIntO( int* ans, const char* str );

bool ZnkS_getI16D( int16_t*  ans, const char* str );
bool ZnkS_getI32D( int32_t*  ans, const char* str );
bool ZnkS_getI64D( int64_t*  ans, const char* str );

bool ZnkS_getU16U( uint16_t* ans, const char* str );
bool ZnkS_getU32U( uint32_t* ans, const char* str );
bool ZnkS_getU64U( uint64_t* ans, const char* str );
bool ZnkS_getU16X( uint16_t* ans, const char* str );
bool ZnkS_getU32X( uint32_t* ans, const char* str );
bool ZnkS_getU64X( uint64_t* ans, const char* str );

bool ZnkS_getSzU( size_t* ans, const char* str );
bool ZnkS_getSzX( size_t* ans, const char* str );

bool ZnkS_getReal( double* ans, const char* str );
bool ZnkS_getRealF( float* ans, const char* str );
/**
 * 文字列が、T, 1, true の場合に限りtrueを返す。
 * それ以外はいかなる場合もfalseを返す.
 */
bool ZnkS_getBool( const char* str );

bool ZnkS_getPtrX( void** ans, const char* str );


/**
 * @brief
 *   この関数型は、strncmpやstrncasecmpにあるような３つの引数に加え、
 *   最後に user param を指定可能なように拡張したものである.
 */
typedef int (*ZnkS_FuncT_StrCompare)(
		const char* /* s1 */, const char* /* s2 */, size_t /* leng */,
		void* /* user param */ );
typedef struct { ZnkS_FuncT_StrCompare func_; void* arg_; } ZnkS_FuncArg_StrCompare;

/**
 * @brief
 *   この関数型は、chがある集合に含まれるか否かを判定するためのものである.
 */
typedef bool (*ZnkS_FuncT_IsKeyChar)( uint32_t ch, void* /* user param */ );
typedef struct { ZnkS_FuncT_IsKeyChar  func_; void* arg_; } ZnkS_FuncArg_IsKeyChar;

/**
 * @brief
 *   この関数型は、ptnで指定した規則にqueryがマッチするか否かを返す.
 */
typedef bool (*ZnkS_FuncT_IsMatch)( const char* ptn, size_t ptn_leng, const char* query, size_t query_leng );

/**
 * @brief
 *   第2引数に strncmp型の関数へのポインタを指定し、それをダイレクトに
 *   呼び出すような ZnkS_FuncArg_StrCompare 型を作成する.
 *   結果は第1引数へ格納される.
 *   第2引数の値は、strcompare_funcargのarg_メンバに格納されるため、
 *   この関数呼び出し後にこのメンバを上書きすることはできない.
 *
 * @note
 *   この関数を使う代わりにもちろん以下のようなZnkS_FuncT_StrCompare型関数を
 *   独自に定義し、それを直接ZnkS_FuncArg_StrCompareのfunc_へ格納するといった
 *   方法をとってもよい(というよりこの方法がむしろ正当な方法である).
 *
 *   static int
 *   StrCompare_strncmp( const char* s1, const char* s2, size_t n, void* arg )
 *   { return strncmp( s1, s2, n ); }
 *
 *   test(){
 *     ZnkS_FuncArg_StrCompare strcompare_funcarg = { StrCompare_strncmp, NULL };
 *     // ZnkS_FuncArg_StrCompare を要求する関数...
 *   }
 *
 *   この関数は、標準関数であるstrncmpをただ呼び出すだけであり、かつargを全く
 *   使わないような場合に、上記の手間を軽減するために用意されたものである.
 *   この場合は、以下のように書ける.
 *
 *   test(){
 *     ZnkS_FuncArg_StrCompare strcompare_funcarg;
 *     ZnkS_makeStrCompare_fromStdFunc( &strcompare_funcarg, strncmp );
 *     // この後、strcompare_funcarg.arg_ は変更不可.
 *     // ZnkS_FuncArg_StrCompare を要求する関数...
 *   }
 */
void
ZnkS_makeStrCompare_fromStdFunc(
		ZnkS_FuncArg_StrCompare* strcompare_funcarg,
		int (*strncmp_func)( const char*, const char*, size_t ) );

/**
 * 文字列比較関数の一般化
 */
bool
ZnkS_compareBegin(
		const char* str, size_t str_leng,
		const char* ptn, size_t ptn_leng, const ZnkS_FuncArg_StrCompare* strcompare_funcarg );
bool
ZnkS_compareEnd(
		const char* str, size_t str_leng,
		const char* ptn, size_t ptn_leng, const ZnkS_FuncArg_StrCompare* strcompare_funcarg );
bool
ZnkS_compareContain(
		const char* str, size_t str_leng, size_t pos,
		const char* ptn, size_t ptn_leng, const ZnkS_FuncArg_StrCompare* strcompare_funcarg );

/***
 * strcmp/strncmpを使った文字列比較
 */
Znk_INLINE bool
ZnkS_eq( const char* s1, const char* s2 ){
	return (bool)( Znk_strcmp( s1, s2 ) == 0 );
}
Znk_INLINE bool
ZnkS_eqEx( const char* s1, const char* s2, size_t leng ){
	return (bool)( Znk_strncmp( s1, s2, leng ) == 0 );
}
bool
ZnkS_isBegin( const char* str, const char* ptn );
bool
ZnkS_isEnd( const char* str, const char* ptn );
bool
ZnkS_isBeginEx( const char* str, const size_t str_leng, const char* ptn, size_t ptn_leng );
bool
ZnkS_isEndEx( const char* str, const size_t str_leng, const char* ptn, size_t ptn_leng );

#define ZnkS_isBegin_literal( str, ptn_literal ) \
	ZnkS_isBeginEx( (str), Znk_NPOS, ptn_literal, Znk_strlen_literal(ptn_literal) )
#define ZnkS_isEnd_literal( str, ptn_literal ) \
	ZnkS_isEndEx( (str), Znk_NPOS, ptn_literal, Znk_strlen_literal(ptn_literal) )

/***
 * strcasecmp/strncasecmpを使った文字列比較
 */
Znk_INLINE bool
ZnkS_eqCase( const char* s1, const char* s2 )
{
	return (bool)( ZnkS_strcasecmp( s1, s2 ) == 0 );
}
Znk_INLINE bool
ZnkS_eqCaseEx( const char* s1, const char* s2, size_t leng )
{
	return (bool)( ZnkS_strncasecmp( s1, s2, leng ) == 0 );
}
bool
ZnkS_isCaseBegin( const char* str, const char* ptn );
bool
ZnkS_isCaseEnd( const char* str, const char* ptn );
bool
ZnkS_isCaseBeginEx( const char* str, size_t str_leng, const char* ptn, size_t ptn_leng );
bool
ZnkS_isCaseEndEx( const char* str, size_t str_leng, const char* ptn, size_t ptn_leng );

#define ZnkS_isCaseBegin_literal( str, ptn_literal ) \
	ZnkS_isCaseBeginEx( (str), Znk_NPOS, ptn_literal, Znk_strlen_literal(ptn_literal) )
#define ZnkS_isCaseEnd_literal( str, ptn_literal ) \
	ZnkS_isCaseEndEx( (str), Znk_NPOS, ptn_literal, Znk_strlen_literal(ptn_literal) )

/**
 * @brief
 *   str の開始にあるキーワードが ptn とマッチするかどうかを調べる.
 *
 *   ここで「マッチする」とは、strがptnで指定される文字列から始まっており、
 *   かつ、その直後はキーワード文字集合以外の文字になっている場合を示す.
 *   つまり、逆に言えばstrがptnで指定される文字列から始まっていたとしても、
 *   strのその直後の位置においてさらにキーワード文字集合に含まれる文字が
 *   連続して続いている場合は、「マッチする」とはみなさない.
 *   これは以下の例を見た方がわかりやすいだろう. この例ではキーワード文字集合は
 *   すべてアルファベット小文字であるとする.
 *
 *   例1. str="function", ptn="func"
 *     この場合は、funcで始まってはいるが、すぐ後ろに't'というキーワード文字集合
 *     に含まれる文字が続くのでマッチしているとはみなさない.
 *
 *   例2. str="func:tion", ptn="func"
 *     この場合は、funcで始まっており、かつ、すぐ後ろに':'というキーワード文字集合
 *     に含まれない文字が続くのでマッチしている.
 *
 *   キーワード文字集合の定義は、keycharsで与えることにより行う.
 *
 * @return
 *   str の開始が ptn とマッチすれば true を返す. さもなければ false を返す.
 */
/**
 * @brief
 *   str の開始にあるキーワードが ptn とマッチするかどうかを調べる.
 *   キーワードの定義は、述語関数を与えることにより行う.
 *
 * @param predicate_func
 *   その文字がキーワード文字であれば true を、
 *   さもなければ false を返すような述語関数へのポインタを指定する.
 *   これをNULLにした場合は、ZnkS_isBeginと挙動が全く同じになるものとする.
 *
 * @return
 *   str の開始が ptn とマッチすれば true を返す. さもなければ false を返す.
 */
bool
ZnkS_isMatchBeginEx(
		const char* str, size_t str_leng,
		const char* ptn, size_t ptn_leng,
		const ZnkS_FuncArg_IsKeyChar*  iskeychar_funcarg,
		const ZnkS_FuncArg_StrCompare* strcompare_funcarg );
bool
ZnkS_isMatchBegin( const char* str, const char* ptn, const char* keychars );
bool
ZnkS_isMatchSWC( const char* ptn, size_t ptn_leng,
		const char* query, size_t query_leng );


/**
 * @brief
 *   4文字以下/8文字以下の長さの文字列sをそのままデシリアライズした32bit/64bit整数値
 *   として返す. ここではEndian等は一切考えない. したがって、取得される整数の算術的意味は、
 *   LEとBEの環境によって異なる結果になる. ここではこれを高速化のためのIDと考える.
 *
 * @exam
 *   // 以下はZnkS_get_id32の場合の例であるが、ZnkS_get_id64の場合は4文字のところを8文字に読み替えて欲しい.
 *   // type_str4は4文字以内の文字列が与えられると仮定し、万一それより長い場合は、4文字で打ち切る.
 *   bool checkType( const char* type_str4 )
 *   {
 *     static uint32_t TEXT = 0; 
 *     static uint32_t PICT = 0; 
 *     static bool     initialized = false;
 *
 *     if( !initialized ){
 *       // この関数が初めて実行される場合は、整数パターンを初期化.
 *       TEXT = ZnkS_get_id32( "TEXT", Znk_NPOS ); 
 *       PICT = ZnkS_get_id32( "PICT", Znk_NPOS ); 
 *       initialized = true;
 *     }
 *
 *     // 以下、整数での比較が可能(文字列比較より高速).
 *     // この例ではパターンがTEXTとPICTの二つしかないので知れているが、
 *     // パターンの数が大量かつ、この比較をリアルタイムに出来るだけ高速に行わなければならない場合等に効果が期待できる.
 *     // この種の4文字IDのテクニックはAdobe Photoshop File のフォーマットなどにも見られる.
 *     {type
 *       const uint32_t type_id32 = ZnkS_get_id32( type_str4, Znk_NPOS ); 
 *       if( type_id32 == TEXT ){
 *         ...
 *       } else if( type_id32 == PICT ){
 *         ...
 *       }
 *     }
 *   }
 *    
 */
uint32_t ZnkS_get_id32( const char* s, size_t leng );
uint64_t ZnkS_get_id64( const char* s, size_t leng );

/**
 * @brief
 *   C文字列str(典型的にはファイルパスなど)とセパレータとなる文字dot_chを指定し、
 *   strにおいてdot_chが最後に現れる位置の次の位置を示すポインタを返す.
 *   strにおいてdot_chが含まれていない場合は拡張子は空であると考え、文字列リテラル
 *   "" を返す(NULLではないことに注意). また、dot_chとしてNULL文字'\0'(=0)を指定
 *   した場合、特例として同じく 文字列リテラル "" を返す.
 *
 * @note
 *   いかなる場合もチェックすることなしに戻り値をそのままstrcmp系の関数へ引き渡す
 *   ことも可能であり、strrchr関数と比べた場合の利点として挙げられる.
 */
const char* ZnkS_get_extension( const char* str, char dot_ch );

/**
 * @brief
 *   C文字列str内の最初に出現する改行コードを除去し
 *   そこで強制的にNUL終端させる.
 */
Znk_INLINE void
ZnkS_chompNL( char* str ){
	while( *str ){
		switch( *str ){
		case '\n': case '\r':
			*str = '\0';
			return;
		default: break;
		}
		++str;
	}
}

/**
 * ZnkS_lfind_one_of:
 *  str における [ begin, end ) の範囲にある文字列において、beginの位置から出発し、
 *  順方向へスキャンしていき、chset で指定した文字のいずれかが最初に現れる位置を返す.
 *  出発点である begin の位置にいきなり見つかるケースもあり得る.
 *  end 位置に到達しても見つからない場合は Znk_NPOSを返す.
 *
 * ZnkS_lfind_one_not_of:
 *  str における [ begin, end ) の範囲にある文字列において、beginの位置から出発し、
 *  順方向へスキャンしていき、chset で指定した文字以外が最初に現れる位置を返す.
 *  出発点である begin の位置にいきなり見つかるケースもあり得る.
 *  end 位置に到達しても見つからない場合は Znk_NPOSを返す.
 *
 * ZnkS_rfind_one_of:
 *  str における [ begin, end ) の範囲にある文字列において、end-1 の位置から出発し、
 *  逆方向へスキャンしていき、chset で指定した文字のいずれかが最初に現れる位置を返す.
 *  出発点である begin の位置にいきなり見つかるケースもあり得る.
 *  end が示す位置は調べられないことに注意する.
 *  begin-1 の位置に到達しても見つからない場合は Znk_NPOSを返す.
 *
 * ZnkS_rfind_one_not_of:
 *  str における [ begin, end ) の範囲にある文字列において、end-1 の位置から出発し、
 *  逆方向へスキャンしていき、chset で指定した文字以外が最初に現れる位置を返す.
 *  出発点である begin の位置にいきなり見つかるケースもあり得る.
 *  end が示す位置は調べられないことに注意する.
 *  begin-1 の位置に到達しても見つからない場合は Znk_NPOSを返す.
 *
 * @param str:
 *  調査対象となるバイト列である.
 *  C文字列以外でもよい.
 *
 * @param begin:
 *  調査対象となるバイト列の開始位置を指定する.
 *  ここに Znk_NPOSが指定された場合は何もせずに Znk_NPOS を返す.
 *
 * @param end:
 *  調査対象となるバイト列の終了位置(最後の文字列の次の位置)を指定する.
 *  strがC文字列である(NULL終端している場合)のみ、ここにZnk_NPOS を指定することができ、
 *  その場合は strlen(str)が指定されたものとみなされる.
 *  ここに 0 が指定された場合や end <= begin であった場合は、何もせず Znk_NPOSを返す.
 *
 * @param chset
 *  検索パターンとなる文字の集合を定義するバイト列を与える.
 *  C文字列以外でもよい.
 *
 * @param chset_size
 *  chset の個数(バイトサイズ)を指定する.
 *  chset がC文字列である(NULL終端している場合)のみ、ここにZnk_NPOS を指定することができ、
 *  その場合は strlen(chset)が指定されたものとみなされる.
 *
 * @return
 *  求めるべきパターンの見つかった位置を返す.
 *  これが見つからなかった場合や begin と end の値が例外的であった場合は Znk_NPOS を返す.
 *
 * @note
 *  ZnkS_lfind_one_of     : std::stringのfind_first_ofと同じ
 *  ZnkS_lfind_one_not_of : std::stringのfind_first_not_ofと同じ
 *  ZnkS_rfind_one_of     : std::stringのfind_last_ofと同じ
 *  ZnkS_rfind_one_not_of : std::stringのfind_last_not_ofと同じ
 */
size_t
ZnkS_lfind_one_of( const char* str,
		size_t begin, size_t end, const char* chset, size_t chset_size );
size_t
ZnkS_lfind_one_not_of( const char* str,
		size_t begin, size_t end, const char* chset, size_t chset_size );
size_t
ZnkS_rfind_one_of( const char* str,
		size_t begin, size_t end, const char* chset, size_t chset_size );
size_t
ZnkS_rfind_one_not_of( const char* str,
		size_t begin, size_t end, const char* chset, size_t chset_size );

Znk_INLINE void
ZnkS_find_side_skip( const char* str, size_t begin, size_t end,
		size_t* ans_begin, size_t* ans_end,
		const char* chset, size_t chset_leng )
{
	/***
	 * begin, end の関係チェック, chset_leng が Znk_NPOSである場合のチェックについては、
	 * ZnkS_lfind_one_not_of ならびに ZnkS_rfind_one_not_of の内部で自動的に行われるため、
	 * ここでは不要である.
	 */
	*ans_begin = ZnkS_lfind_one_not_of( str, begin, end, chset, chset_leng );
	if( *ans_begin == Znk_NPOS ){ *ans_end = Znk_NPOS; return; }
	/***
	 * end から逆方向へ調べ、chset ではない文字が現れる位置を調べる.
	 * *ans_begin にある文字は chset に含まれないため、この処理では not found は起こりえない.
	 * ans_end は 目的のトークン最後の文字の次の位置を示すものであるため、+1 をして ans_end は確定する.
	 */
	*ans_end = ZnkS_rfind_one_not_of( str, *ans_begin, end, chset, chset_leng ) + 1;
}

/**
 * str の [ begin, end ) で示される範囲において、いわゆる delimiter 文字列で区切られた
 * 代入文のような形式の文字列があるとする. このとき、左辺に該当する key の部分と 右辺に
 * 該当する val の部分の範囲を、skip 文字列を考慮した形でサーチする.
 *
 * ここでは説明のため、この範囲にある文字列が例えば以下のようなものである場合を考える.
 * delimiter 文字列は "<=" とし、skip_chset = " \t" (whitespaceとタブ文字)とする.
 *
 *  "  My Key <= My Val  "
 *   01234567890123456789
 *
 * このとき、key に該当する部分は "My Key" であり、val に該当する部分は "My Val" となる.
 * これらの両側に存在する whitespace のみが除去される必要がある( "My Key" のように
 * skip文字列が途中に含まれる場合は除去の対象外である ).
 * よって、この場合取得される key の範囲は [2, 8)、val の範囲は [12, 18) となる.
 * またこの関数は delimiter の開始位置を返す. この例の場合は 9 となる.
 *
 * @note
 *  key_begin, key_end, val_begin, val_end に NULL を指定することはできない.
 *  従ってこれが不要な場合でもダミー変数のアドレスを指定する必要がある.
 */
size_t
ZnkS_find_key_and_val( const char* str, size_t begin, size_t end,
		size_t* key_begin, size_t* key_end,
		size_t* val_begin, size_t* val_end,
		const char* delimiter,  size_t delimiter_leng,
		const char* skip_chset, size_t skip_chset_leng );

/**
 * str の [ begin, end ) で示される範囲において、いわゆるスペース文字で区切られた
 * トークン文字列の羅列があるとして、それらのうち最初から数えて arg_idx 番目にある
 * トークンにおける開始位置と文字列長を求める. 戻り値はそのトークンの開始位置となり、
 * arg_leng においてそのトークンの文字列長が格納される.
 * この場合の arg_idx の指定は 0 の場合一番最初に位置するトークン、 1 の場合は
 * 2 番目に位置するトークンを示し、以下同様とする.
 * arg_idxで指定したトークンが見つからなかった場合は Znk_NPOS を返し、arg_lengには
 * 0がセットされる.
 *
 * @note
 *  arg_leng に NULL を指定することはできない.
 *  従ってこれが不要な場合でもダミー変数のアドレスを指定する必要がある.
 */
size_t
ZnkS_lfind_arg( const char* str, size_t begin, size_t end,
		size_t arg_idx, size_t* arg_leng,
		const char* skip_chset, size_t skip_chset_leng );
/**
 * str の [ begin, end ) で示される範囲において、いわゆるスペース文字で区切られた
 * トークン文字列の羅列があるとして、それらのうち最後から数えて arg_idx 番目にある
 * トークンにおける開始位置と文字列長を求める. 戻り値はそのトークンの開始位置となり、
 * arg_leng においてそのトークンの文字列長が格納される.
 * この場合の arg_idx の指定は 0 の場合一番最後に位置するトークン、 1 の場合は
 * 一番最後から 2 番目に位置するトークンを示し、以下同様とする.
 * arg_idxで指定したトークンが見つからなかった場合は Znk_NPOS を返し、arg_lengには
 * 0がセットされる.
 *
 * @note
 *  arg_leng に NULL を指定することはできない.
 *  従ってこれが不要な場合でもダミー変数のアドレスを指定する必要がある.
 */
size_t
ZnkS_rfind_arg( const char* str, size_t begin, size_t end,
		size_t arg_idx, size_t* arg_leng,
		const char* skip_chset, size_t skip_chset_leng );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
