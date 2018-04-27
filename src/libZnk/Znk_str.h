#ifndef INCLUDE_GUARD__Znk_str_h__
#define INCLUDE_GUARD__Znk_str_h__

#include <Znk_bfr.h>
#include <stdarg.h>

Znk_EXTERN_C_BEGIN

/***
 * ZnkStrは、ZnkBfrの別名でありこれらは全く同一の型である.
 * しかし、ZnkStrでは内部バッファの状態に特有のポリシーを持つ.
 */
typedef ZnkBfr ZnkStr;


/**
 * @brief
 *   zkstr のlengを new_lengで指定した長さにする.
 *   ZnkBfr_resizeとの違いは、new_lengは実際のbuffer sizeよりも1小さい値であるという
 *   ことと、NULL終端が考慮されることである.
 *   new_lengにはZnk_NPOSを指定することはできない.
 */
void ZnkStr_releng( ZnkStr zkstr, size_t new_leng );

/**
 * @brief
 * 内部バッファをsizeの範囲内で確実にNULL終端させる.
 */
void
ZnkStr_terminate_null( ZnkBfr zkbfr, bool plus_one );

/**
 * @brief
 * 内部バッファをsizeの範囲内で確実にNULL終端させ、さらに必要ならばNULL文字の位置に合わせて
 * sizeを縮小させる(正規化する).
 */
void
ZnkStr_normalize( ZnkBfr zkbfr, bool plus_one );

/**
 * NULL終端する文字列を作成する.
 *
 * @param init_data
 *   初期化のための文字列を指定する.
 *   また、以下のような特別な場合も許容する.
 *
 *   1. NULL終端していないバイトシーケンスを与えてもよい.
 *      この場合lengに有限かつ適切な値を指定する必要がある.
 *      (Znk_NPOSを指定してはならない).
 *
 *   2. NULLを指定してもよい.
 *      その場合は文字列での初期化を行わない.
 *      またこのときに限り、lengは特別な意味になる(lengを参照).
 *
 * @param leng
 *   基本的には文字列の長さ(バイト)を与える.
 *   内部でメモリ確保されるsizeは、この値と等しくなるとは限らない.
 *   (大抵の場合、NULL文字分サイズが追加されるため、これよりも大きいサイズとなる)
 *   しかし、ZnkStr_lengで取得される値は(特にZnkStrに変更がない限り)この値となる.
 *   要はZnkStrが保持する内部文字列に対してstrlenを施したときの戻り値に相当する値である.
 *
 *   また、以下のような特別な場合も許容する.
 *
 *   1. init_dataがNULL終端しているCの文字列である場合に限り、lengにZnk_NPOSを指定してもよい.
 *     その場合、strlen( init_data )が指定されたのと同じとみなされる.
 *
 *   2. init_dataにNULLを指定した場合、ここに指定された値に 1 を足した値で メモリがreserveされる.
 *     ただし、メモリのreserve量にかかわらず、この状態でのZnkStr_lengの値は常に 0 である.
 *     (これはZnkStr_clearを施した状態と等価である. 厳密に言えば、このときの内部ZnkBfrにおける
 *     いわゆるサイズは常に 1 に設定されている)
 */
ZnkStr
ZnkStr_create_ex( const char* init_data, size_t leng, ZnkBfrType type );

Znk_INLINE ZnkStr
ZnkStr_create( const char* init_data, size_t leng )
{
	return ZnkStr_create_ex( init_data, leng, ZnkBfr_Pad128 );
}

Znk_INLINE void
ZnkStr_destroy( ZnkStr zkstr ){
	ZnkBfr_destroy( zkstr );
}

/***
 * これはZnkStr_create/ZnkStr_destroyと全く同じものだが、
 * 典型的なパターンを簡潔に形式化したものである.
 */
Znk_INLINE ZnkStr
ZnkStr_new( const char* init_data ){
	return ZnkStr_create( init_data, Znk_NPOS );
}
ZnkStr
ZnkStr_newf( const char* fmt, ... );
Znk_INLINE void
ZnkStr_delete( ZnkStr zkbfr ){
	ZnkBfr_destroy( zkbfr );
}

Znk_INLINE size_t
ZnkStr_leng( const ZnkStr zkstr )
{
	/* 内部バッファは正規化されているものとして扱う */
	const size_t size = ZnkBfr_size( zkstr );
	/* size == 0 の場合は特別に許容する. */
	return ( size == 0 ) ? 0 : size-1;
}
Znk_INLINE bool ZnkStr_empty( const ZnkStr zkstr ){ return (bool)( ZnkStr_leng( zkstr ) == 0 ); }

/**
 * @brief
 * NULL終端文字列の内部ポインタを得る.
 */
Znk_INLINE const char*
ZnkStr_cstr( const ZnkStr zkstr )
{
	/* 内部バッファは正規化されているものとして扱う */
	return (const char*)ZnkBfr_data( zkstr );
}

/**
 * @brief
 * 与えたZnkBfrを空文字を意味する正規化文字列にする.
 * (つまり、NULL文字が一つだけ格納されたsize=1のバッファとする)
 */
Znk_INLINE void
ZnkStr_clear( ZnkBfr zkbfr ){ ZnkStr_releng( zkbfr, 0 ); }

/**
 * @brief
 * src, src_lengで指定した文字列でzkstr_dstのdst_pos以降の内容を上書きする.
 * zkstr_dst 内のdst_pos以降にある旧文字列の内容はそのサイズ如何に関わらず全破棄され、
 * 完全に新しい文字列に置き換わる(dst_posより前の文字列はなんら影響はない).
 *
 * この関数の呼び出し以前に取得していたzkstr_dstの生データポインタは、内部ralloc呼び出しにより
 * 無効となる可能性があることに注意する.
 *
 * @param zkstr_dst
 *   基本的には正規化文字列を指定する.
 *   ただしsizeが0のZnkBfrの指定も認める.
 *
 * @param dst_pos
 *   zkstr_dst での置き換えを始める位置.
 *   Znk_NPOSを指定した場合、文字列の最終位置(内部バッファサイズ-1)が指定されたものとみなされる.
 *   この最終位置は、zkstr_dst内ではNULL文字が格納されているはずの位置であり、この場合連結処理となる.
 *
 * @param src
 *   新しく置き換える文字列の先頭を指すポインタ.
 *
 * @param src_leng
 *   新しく置き換える文字列の長さ.
 *   srcがNULL終端している場合のみここにZnk_NPOSを指定してもよく、
 *   その場合、srcに対し、strlenを実行した結果が指定されたものとみなされる.
 */
void
ZnkStr_assign( ZnkStr zkstr_dst, size_t dst_pos, const char* src, size_t src_leng );

/**
 * @brief
 *  いわゆるC文字列を上書き代入する.
 *
 * @note
 *  やはりC文字列を与えて代入する処理が一番頻度が高い.
 *  よってここではこの一番短いsetという名前を与える.
 */
Znk_INLINE void
ZnkStr_set( ZnkStr zkstr, const char* cstr )
{
	ZnkStr_assign( zkstr, 0, cstr, Znk_NPOS );
}

/***
 * @brief
 * シュガー関数.
 */
Znk_INLINE void
ZnkStr_copy( ZnkStr dst, const ZnkStr src )
{
	ZnkStr_assign( dst, 0, ZnkStr_cstr(src), ZnkStr_leng(src) );
}


/**
 * @brief
 * 文字列を後ろへ連結する.
 * この関数の呼び出し以前に取得していたzkstrの生データポインタは、内部ralloc呼び出しにより
 * 無効となる可能性があることに注意する.
 * 各引数については、ZnkStr_assignに準じる.
 */
Znk_INLINE void
ZnkStr_append( ZnkStr zkstr, const char* src, size_t src_leng ){
	ZnkStr_assign( zkstr, Znk_NPOS, src, src_leng );
}

void
ZnkStr_replace( ZnkStr str, size_t dst_pos, size_t dst_leng, const char* src, size_t src_leng );
void
ZnkStr_insert( ZnkStr str, size_t dst_pos, const char* src, size_t src_leng );

/**
 * @brief
 *  C文字または文字列を後ろへ連結する.
 *
 * @note
 *  これらの関数の呼び出し以前に取得していたzkstrの生データポインタは、内部ralloc呼び出しにより
 *  無効となる可能性があることに注意する.
 */
void
ZnkStr_add_c( ZnkStr zkstr, char ch );
Znk_INLINE void
ZnkStr_add( ZnkStr zkstr, const char* cstr )
{
	ZnkStr_append( zkstr, cstr, Znk_NPOS );
}
Znk_INLINE void
ZnkStr_add2( ZnkStr zkstr, const char* s1, const char* s2 )
{
	ZnkStr_add( zkstr, s1 );
	ZnkStr_add( zkstr, s2 );
}
Znk_INLINE void
ZnkStr_add3( ZnkStr zkstr, const char* s1, const char* s2, const char* s3 )
{
	ZnkStr_add( zkstr, s1 );
	ZnkStr_add( zkstr, s2 );
	ZnkStr_add( zkstr, s3 );
}
Znk_INLINE void
ZnkStr_add4( ZnkStr zkstr, const char* s1, const char* s2, const char* s3, const char* s4 )
{
	ZnkStr_add( zkstr, s1 );
	ZnkStr_add( zkstr, s2 );
	ZnkStr_add( zkstr, s3 );
	ZnkStr_add( zkstr, s4 );
}
Znk_INLINE void
ZnkStr_add5( ZnkStr zkstr, const char* s1, const char* s2, const char* s3, const char* s4, const char* s5 )
{
	ZnkStr_add( zkstr, s1 );
	ZnkStr_add( zkstr, s2 );
	ZnkStr_add( zkstr, s3 );
	ZnkStr_add( zkstr, s4 );
	ZnkStr_add( zkstr, s5 );
}
Znk_INLINE void
ZnkStr_add6( ZnkStr zkstr, const char* s1, const char* s2, const char* s3, const char* s4, const char* s5, const char* s6 )
{
	ZnkStr_add( zkstr, s1 );
	ZnkStr_add( zkstr, s2 );
	ZnkStr_add( zkstr, s3 );
	ZnkStr_add( zkstr, s4 );
	ZnkStr_add( zkstr, s5 );
	ZnkStr_add( zkstr, s6 );
}

/**
 * @brief
 * zkbfrへsnprintfを実行する.
 * 内部ではZnk_vsnprintfを呼び出している.
 * size の指定によって、展開した文字列が途中打ち切ることもできるし、
 * メモリが続く限り自動的に拡張することもできる.
 *
 * @param zkstr
 *   正規化文字列ZnkStrを指定しなければならない.
 *
 * @param pos
 *   zkbfr内の位置を指定する.
 *   この位置を開始位置として展開文字列をassignする.
 *   snprintfとは対照的に、この第2引数にはbuf_sizeを指定するのではないことに注意.
 *   pos に Znk_NPOS や文字列長(ZnkStr_lengで得られる値)以上の値を指定した場合、
 *   文字列長が指定されたものとみなされ、その場合、後ろに結果を連結することになる.
 *
 * @param size
 *   展開される文字列の限界サイズを指定する.
 *   size に Znk_NPOS 以外の値を指定した場合、snprintfとほぼ同じ意味となる.
 *   すなわち、pos から size までの範囲に展開文字列が収められ(これを超える場合は
 *   途中打ち切られ)、最終文字は強制的にNULL文字となる.
 *   size に Znk_NPOS を指定した場合、展開文字列と厳密に同じものがpos以降に
 *   assignされる. この場合、バッファのサイズは自動的に適切なものに拡張される.
 *   この場合、内部ではメモリ確保＋snprintfをトライ＆エラーで繰り返すため、
 *   展開文字列のサイズが大きい場合、最初から最適なサイズを指定した場合と比べ
 *   速度低下を招く場合がある.
 *
 * @param fmt
 *   いわゆるprintfフォーマットを指定する.
 *
 * @return
 *   Znk_vsnprintf の戻り値に準じる.
 *   ただし、size が Znk_NPOS の場合、これが -1 を返すことはない.
 */
int ZnkStr_vsnprintf( ZnkStr zkstr, size_t pos, size_t size, const char* fmt, va_list ap );
int ZnkStr_snprintf(  ZnkStr zkstr, size_t pos, size_t size, const char* fmt, ... );
/**
 * @brief
 *  ZnkStr_vsnprintf の 第 3 引数 size において Znk_NPOS を指定したのと等価である.
 *  ゆえに sprintfという名前ではあるが、バッファオーバーフローの恐れはない.
 */
int ZnkStr_sprintf( ZnkStr zkstr, size_t pos, const char* fmt, ... );
/**
 * @brief
 *  ZnkStr_sprintf の 第 2 引数 pos において Znk_NPOS を指定したのと等価である.
 *  すなわち、指定した文字列をフォーマットしたものをzkstrの最後に連結する形になる.
 */
int ZnkStr_addf( ZnkStr zkstr, const char* fmt, ... );
/**
 * @brief
 *  ZnkStr_sprintf の 第 2 引数 pos において 0 を指定したのと等価である.
 *  すなわち、指定した文字列をフォーマットしたものをzkstrにsetする形になる.
 */
int ZnkStr_setf( ZnkStr zkstr, const char* fmt, ... );
/* 復帰作業が終わるまでしばらく残す */
#define ZnkStr_vsnprintf2 ZnkStr_vsnprintf
#define ZnkStr_snprintf2  ZnkStr_snprintf
#define ZnkStr_sprintf2   ZnkStr_sprintf
#define ZnkStr_addf2 ZnkStr_addf
#define ZnkStr_setf2 ZnkStr_setf


/**
 * strの全内容とcstrで示されるC文字列が等しいかどうかを返す.
 */
bool
ZnkStr_eq( const ZnkStr str, const char* cstr );

/**
 * @brief
 * strの[ pos, ZnkStr_leng(str) ) (すなわちposから最後まで)の範囲とdataで示されるバイト列が
 * 完全に等しいかどうかを返す.
 *
 * @note
 * ZnkStr_isContainとの違いに注意する.
 * この関数の場合、pos以降からNULL文字までの文字列長がdata_lengと一致していない場合は
 * たとえposからdata_leng番目までの文字列がdataと等しくとも、戻り値はtrueとはならない.
 */
bool
ZnkStr_eqEx( const ZnkStr str, size_t pos, const char* data, size_t data_leng );
/**
 * @brief
 * strの[ pos, pos+data_leng )の範囲とdataで示されるバイト列が等しいかどうかを返す.
 * strにおける pos+data_leng 以降の範囲は参照されないため、この部分はどのような内容
 * でも構わない.
 */
bool
ZnkStr_isContain( const ZnkStr zkstr, size_t pos, const char* data, size_t data_leng );
/**
 * strが data で示されるバイト列から開始しているか否かを返す.
 */
Znk_INLINE bool
ZnkStr_isBegin( const ZnkStr zkstr, const char* cstr ){
	return ZnkStr_isContain( zkstr, 0, cstr, Znk_NPOS );
}
/**
 * strが data で示されるバイト列で終端しているか否かを返す.
 */
Znk_INLINE bool
ZnkStr_isEnd( const ZnkStr zkstr, const char* cstr ){
	return ZnkStr_isContain( zkstr, Znk_NPOS, cstr, Znk_NPOS );
}


/**
 * @brief
 *   zkstr の後ろの文字列を cut_size で指定した長さだけカットする.
 *   cut_size に zkstr の文字列長以上の数や Znk_NPOSが指定された場合、
 *   この関数はZnkStr_clearと同じになる.
 *
 * @param zkstr
 *   基本的には正規化文字列を指定する.
 *   ただしsizeが0のZnkBfrの指定も認める.
 *   内部でreallocは発生しない.
 */
void ZnkStr_cut_back( ZnkStr zkstr, size_t cut_size );
/**
 * @brief
 *   ZnkStr_cut_back の cut_size において 1 を指定したのと全く同じである.
 */
Znk_INLINE void
ZnkStr_cut_back_c( ZnkStr zkstr ){ ZnkStr_cut_back( zkstr, 1 ); }


/**
 * @brief
 *  zkstr の pos の位置にある文字列を cut_size で指定した長さだけ削除する.
 *  cut_size に zkstr の文字列長以上の数や Znk_NPOSが指定された場合、
 *  この関数はZnkStr_releng に pos を指定したのと同じになる.
 *  また pos に zkstr の文字列長以上の数や Znk_NPOSが指定された場合、
 *  この関数は何もしない.
 *
 * @param zkstr
 *  基本的には正規化文字列を指定する.
 *  ただしsizeが0のZnkBfrの指定も認める.
 *  内部でreallocは発生しない.
 */
void ZnkStr_erase( ZnkStr zkstr, size_t pos, size_t cut_size );
/**
 * @brief
 *  ZnkStr_erase の pos において 0 を指定したのと全く同じである.
 */
Znk_INLINE void
ZnkStr_cut_front( ZnkStr zkstr, size_t cut_size ){ ZnkStr_erase( zkstr, 0, cut_size ); }
/**
 * @brief
 *  ZnkStr_cut_front の cut_size において 1 を指定したのと全く同じである.
 */
Znk_INLINE void
ZnkStr_cut_front_c( ZnkStr zkstr ){ ZnkStr_cut_front( zkstr, 1 ); }

/**
 * @brief
 *   zkstr における pos で指定した位置にある文字が ch と等しい場合、
 *   それをカットする. pos に Znk_NPOS や zkstr のサイズ以上の値を指定した場合、
 *   最終文字のある位置が指定されたとみなされる. また zkstrの文字列長が 0 の場合は
 *   この関数は何もしない.
 */
void ZnkStr_chompC( ZnkStr zkstr, size_t pos, char ch );

/**
 * @brief
 *   line の最後尾の文字が改行コードであった場合、それらをカットする.
 *   そうでない場合は何もしない.
 */
void ZnkStr_chompNL( ZnkStr line );


/**
 * @brief
 *   文字列の idx 番目の文字を返す.
 *   ただし、文字列が空の場合は '\0' を返す.
 *
 * @note
 *   ZnkBfr_at との違いは、文字列が空である場合が考慮されていることである.
 *   また、それ以外の例えば範囲チェックなどは一切考慮されていない.
 *   従って、ZnkStr_leng(zkstr) の値より大きい idx を与えた場合の動作については
 *   未定義となる( ZnkStr_leng(zkstr) と同じ値については認める ).
 *   idxにZnk_NPOSを与えることはできない.
 */
char ZnkStr_at( const ZnkStr zkstr, size_t idx );

/**
 * @brief
 *   文字列の一番先頭の文字を返す.
 *   ただし、文字列が空の場合は '\0' を返す.
 */
Znk_INLINE char ZnkStr_first( const ZnkStr zkstr ){
	return ZnkStr_at( zkstr, 0 );
}
/**
 * @brief
 *   文字列の一番最後の文字を返す.
 *   ただし、文字列が空の場合は '\0' を返す.
 */
char ZnkStr_last( const ZnkStr zkstr );

void
ZnkStr_swap( ZnkStr str1, ZnkStr str2 );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
