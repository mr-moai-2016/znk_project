#include "Znk_str_ex.h"
#include "Znk_str_ary.h"
#include "Znk_s_base.h"
#include "Znk_mem_find.h"
#include "Znk_def_util.h"

#define DECIDE_STRLENG( str_leng, str ) Znk_setStrLen_ifNPos( &str_leng, str )


/*****************************************************************************/
/* Adapter */

Znk_DECLARE_HANDLE( StrListHandle );
typedef size_t      (*AdapterSizeFunc_T)(    StrListHandle list );
typedef void        (*AdapterReserveFunc_T)( StrListHandle list, size_t size );
typedef void        (*AdapterPushBkFunc_T)(  StrListHandle list, const char* data, size_t data_leng );
typedef const char* (*AdapterAtFunc_T)(      StrListHandle list, size_t idx );

struct AdapterImpl {
	AdapterSizeFunc_T    size_;
	AdapterReserveFunc_T reserve_;
	AdapterPushBkFunc_T  push_bk_;
	AdapterAtFunc_T      at_;
};

typedef struct AdapterImpl* Adapter;

static size_t      adp__VStr_size(    StrListHandle list )
{ return ZnkStrAry_size(      (ZnkStrAry)list ); }
static void        adp__VStr_reserve( StrListHandle list, size_t size )
{        ZnkStrAry_reserve(   (ZnkStrAry)list, size ); }
static void        adp__VStr_push_bk( StrListHandle list, const char* data, size_t data_leng )
{        ZnkStrAry_push_bk_cstr(   (ZnkStrAry)list, data, data_leng ); }
static const char* adp__VStr_at(      StrListHandle list, size_t idx )
{ return ZnkStrAry_at_cstr( (ZnkStrAry)list, idx ); }

static struct AdapterImpl st_adapter_vstr = {
	adp__VStr_size,
	adp__VStr_reserve,
	adp__VStr_push_bk,
	adp__VStr_at,
};

/* endof Adapter */
/*****************************************************************************/

static void
Adapter_addSplitC( Adapter adp, StrListHandle ans_list,
		const char* str, size_t str_leng,
		char delimit_c, const bool count_void_str, size_t expect_size )
{
	size_t token_size;
	size_t begin = 0;
	DECIDE_STRLENG( str_leng, str );

	adp->reserve_( ans_list, adp->size_(ans_list) + expect_size );

	while( true ){
		if( begin >= str_leng ){
			/***
			 * size == 0 または delimit_c の位置が調度 size-1 (strの最終文字) にある場合のみ
			 * ここに到達する.
			 * count_void_str が true ならば、空文字を加える.
			 */
			if( count_void_str ){ adp->push_bk_( ans_list, "", 0 ); }
			break;
		}

		token_size = ZnkMem_lfind_8( (uint8_t*)str+begin, str_leng-begin, delimit_c, 1 );
		if(	token_size == Znk_NPOS ){
			/***
			 * not found 
			 * 残り全部を ans_list に追加しbreakする. str_leng-begin > 0 は必ず成立.
			 */
			adp->push_bk_( ans_list, str + begin, str_leng-begin );
			break;
		}

		/***
		 * delimit_c の直前にあるトークンの範囲確定. ans_listへ追加.
		 */
		if( token_size > 0 ){
			adp->push_bk_( ans_list, str + begin, token_size );
		} else {
			/***
			 * token_size == 0 のとき count_void_str が true ならば、空文字を加える.
			 */
			if( count_void_str ){ adp->push_bk_( ans_list, "", 0 ); }
		}
		begin += (token_size + 1); /* 残り */
	}
}

void
ZnkStrEx_addSplitC( ZnkStrAry ans_list,
		const char* str, size_t str_leng,
		char delimit_c, const bool count_void_str, size_t expect_size )
{
	Adapter_addSplitC( &st_adapter_vstr,
			(StrListHandle)ans_list, str, str_leng, delimit_c, count_void_str, expect_size );
}


/***
 * str.size は 2以上、chset.size は 2以上、
 * 複雑なケースのみを前提として処理する.
 */
Znk_INLINE void
Adapter_splitCSet_core( Adapter adp, StrListHandle ans_list,
		const char* str, size_t str_leng, const char* chset, size_t chset_leng,
		size_t expect_size )
{
	size_t idx;
	size_t begin = 0;

	adp->reserve_( ans_list, adp->size_(ans_list) + expect_size );

	while( true ){
		/***
		 * chset を読み飛ばす処理.
		 */
		begin = ZnkS_lfind_one_not_of( str, begin, str_leng, chset, chset_leng );
		if( begin >= str_leng || begin == Znk_NPOS ){ return; } /* strの終端に達した場合 */

		/***
		 * 次にchsetが現れる場所(拾うべきトークンの最終文字位置+1)を求める.
		 * str[begin] 自体はchsetでないことは、直前のZnkS_lfind_one_not_ofにより
		 * 保証されている.
		 */
		idx = ZnkS_lfind_one_of( str, begin, str_leng, chset, chset_leng );
		if(	idx >= str_leng || idx == Znk_NPOS ){
			/*
			 * 結局最後までWS文字は見つからなかった場合.
			 * begin より残り全部が一つのトークンとなる.
			 **/
			if( begin < str_leng ){ adp->push_bk_( ans_list, str + begin, str_leng - begin ); }
			break;
		}

		/***
		 * トークンの範囲確定. ans_listへ追加.
		 *
		 * str[begin] 自体はchsetでないことが保証されており
		 * かつ idxは ZnkS_lfind_one_of の結果であるため、必ず idx > begin が成り立つはず.
		 * つまり token_size = idx - begin > 0
		 */
		adp->push_bk_( ans_list, str + begin, idx - begin );

		begin = idx + 1; /* 残り */
	}
}

static void
Adapter_addSplitCSet( Adapter adp, StrListHandle ans_list,
		const char* str,   size_t str_leng,
		const char* chset, size_t chset_leng,
		size_t expect_size )
{
	DECIDE_STRLENG( str_leng, str );
	/***
	 * str が空の場合
	 */
	if( str_leng == 0 ){ return; }

	DECIDE_STRLENG( chset_leng, chset );
	/***
	 * chset が空の場合
	 */
	if( chset_leng == 0 ){
		/***
		 * ここでは指定された範囲を一つの塊として単にプッシュするものとする.
		 * このとき、str を全部 1 文字ずつバラバラに分解するという解釈もできるが、
		 * string 自体、1 文字ずつバラバラにしたものの集合であるので
		 * このような目的で処理を行うこと自体に意味がない.
		 */
		adp->push_bk_( ans_list, str, str_leng );
		return;
	}

	/***
	 * str が1文字の場合
	 */
	if( str_leng == 1 ){
		/* 唯一の文字 str[0] が chset でなければそれを単にプッシュすればよい.
		 * chset ならばなにもしない. */
		if( ZnkMem_lfind_8( (uint8_t*)chset, chset_leng, (uint8_t)str[0], 1 ) == Znk_NPOS ){
			adp->push_bk_( ans_list, str, 1 );
		}
		return;
	}

	if( chset_leng == 1 ){
	 	/* このときは要するに splitC処理である. */
		Adapter_addSplitC( adp, ans_list,
				str, str_leng, chset[ 0 ], false, expect_size );
		return;
	} else {
		/***
		 * chset のうち実際に使用されるものを(str内を)検索して選びだし、
		 * 新しく chset_modify を得る.
		 */
		char   chset_modify[ 256 ]; /* 1byteコードの集合であるからNULL文字を除けば最大でも255通り */
		size_t chset_modify_count = 0;
		size_t idx;

		Znk_CLAMP_MAX( chset_leng, 256 ); /* 256以上であった場合は明らかに指定が変であるので打ち切る */
		for(idx=0; idx<chset_leng; ++idx){
			if( ZnkMem_lfind_8( (uint8_t*)str, str_leng, (uint8_t)chset[idx], 1 ) != Znk_NPOS ){
				/***
				 * ここで重複のチェックはなされない.
				 */
				chset_modify[ chset_modify_count ] = chset[ idx ];
				++chset_modify_count;
			}
		}

		chset_leng = chset_modify_count;
		chset_modify[ chset_modify_count ] = '\0';
	
		if( chset_leng == 0 ){
			/***
			 * chset_modify が空の場合. 明らかに str は分断されない.
			 */
			adp->push_bk_( ans_list, str, str_leng );
		} else if( chset_leng == 1 ){
			/***
			 * chset_modify が1文字の場合. このときは要するに splitC処理である.
			 */
			Adapter_addSplitC( adp, ans_list, str, str_leng, chset_modify[ 0 ], false, expect_size );
		} else {
			/***
			 * 最も複雑なケース. str_leng >= 2 かつ chset_modify leng >= 2 の場合.
			 */
			Adapter_splitCSet_core( adp, ans_list, str, str_leng, chset_modify, chset_leng, expect_size );
		}
	}
}

void
ZnkStrEx_addSplitCSet( ZnkStrAry ans_list,
		const char* str,   size_t str_leng,
		const char* chset, size_t chset_leng,
		size_t expect_size )
{
	Adapter_addSplitCSet( &st_adapter_vstr,
			(StrListHandle)ans_list, str, str_leng, chset, chset_leng, expect_size );
}


static void
Adapter_addSplitStr( Adapter adp, StrListHandle ans_vstr, const char* str, size_t str_leng,
		const char* delimiter, size_t delimiter_leng,
		const bool count_void_str, size_t expect_size )
{
	if( delimiter_leng == 1 ){
		Adapter_addSplitC( adp, ans_vstr, str, str_leng,
				delimiter[0], count_void_str, expect_size );
		return;
	} else {
		size_t token_size;
		size_t begin = 0;

		DECIDE_STRLENG( str_leng, str );
		DECIDE_STRLENG( delimiter_leng, delimiter );

		adp->reserve_( ans_vstr, expect_size );
	
		while( true ){

			if( begin >= str_leng ){
				if( count_void_str ){ adp->push_bk_( ans_vstr, "", 0 ); } /* 空を追加 */
				break;
			}
	
			/***
			 * strstrは使えない(ここではNULL終端しているとは限らない).
			 * begin < str_leng が保証されているため、str_leng-begin は 1 以上となる.
			 * また、ここで取得されるのは str+begin からの相対位置であるため、
			 * すなわちトークンの長さそのものである( Znk_NPOS である場合を除く ).
			 */
			token_size = ZnkMem_lfind_data_BF( (uint8_t*)str+begin, str_leng-begin,
					(uint8_t*)delimiter, delimiter_leng, 1 );
			if(	token_size == Znk_NPOS ){
				/***
				 * begin より残り全部を一つのトークンとして追加する.
				 */
				adp->push_bk_( ans_vstr, str+begin, str_leng-begin );
				break;
			}
	
			/***
			 * [ begin, begin+token_size )の部分を一つのトークンとして追加.
			 *
			 * beginがいきなりdelimiterの開始文字を指し示している可能性はある.
			 * 例えば、delimiter="ABC" として、str="ABD..." や str="...ABCABC..." となっている場合である.
			 * 従って、token_size が 0 になる可能性もある.
			 */
			if( token_size == 0 ){
				if( count_void_str ){ adp->push_bk_( ans_vstr, "", 0 ); } /* 空を追加 */
			} else {
				adp->push_bk_( ans_vstr, str+begin, token_size );
			}
	
			/***
			 * 残り
			 *
			 * この時点で、begin+token_sizeはdelimiterの開始文字の位置である.
			 * よって、次の新しいbeginは、begin+token_size+delimiter_lengとなる.
			 *
			 * 新しいbeginがいきなりdelimiterの開始文字を指し示す可能性はある.
			 * 例えば、delimiter="ABC", str="...ABCABC..." となっている場合である.
			 *
			 * 尚、delimiterが調度strの最終部分にある場合(str="...ABC"となっている場合)
			 * この更新によって、begin は str_leng(>0) に等しくなる.
			 * 逆に begin が str_leng(>0) に等しくなるのはこの場合をおいて他にない.
			 *
			 * 下記の更新後、次のループの周回始めにおいて、begin >= str_leng という比較をして
			 * これが真となるならば、それは要するに begin == str_leng と断定してよく、
			 * delimiterが調度strの最終部分にある場合と判断できる.
			 *
			 * count_void_str が trueの場合は、この後ろにサイズ0のトークンがあると考える必要がある.
			 */
			begin += ( token_size + delimiter_leng );
		}
	}
}

void
ZnkStrEx_addSplitStr( ZnkStrAry ans_vstr, const char* str, size_t str_leng,
		const char* delimiter, size_t delimiter_leng,
		const bool count_void_str, size_t expect_size )
{
	Adapter_addSplitStr( &st_adapter_vstr, (StrListHandle)ans_vstr,
			str, str_leng,
			delimiter, delimiter_leng,
			count_void_str, expect_size );
}

static void
Adapter_addJoin( Adapter adp,
		ZnkStr ans, const StrListHandle str_list,
		size_t begin, size_t end,
		const char* connector, size_t connector_leng, size_t expect_elem_leng )
{
	const size_t num = adp->size_( str_list );
	end = Znk_MIN( num, end );
	if( end > begin ){
		size_t idx;

		DECIDE_STRLENG( connector_leng, connector );

		/* おおまかな予想予約サイズ */
		ZnkBfr_reserve( ans, ZnkBfr_size(ans) + ( expect_elem_leng + connector_leng ) * (end-begin) );
	
		/***
		 * ZnkStr_add, あるいは ZnkStr_appendで十分高速である.
		 * これらを使わずに工夫を凝らしたとしてもほとんど差はない.
		 */
		for( idx=begin; idx<end-1; ++idx ){
			ZnkStr_add( ans, adp->at_( str_list, idx ) );
			ZnkStr_append( ans, connector, connector_leng );
		}
		ZnkStr_add( ans, adp->at_( str_list, end-1 ) );
	}
}

void
ZnkStrEx_addJoin( ZnkStr ans, const ZnkStrAry str_list,
		size_t begin, size_t end,
		const char* connector, size_t connector_leng, size_t expect_elem_leng )
{
	Adapter_addJoin( &st_adapter_vstr, ans, (StrListHandle)str_list,
			begin, end,
			connector, connector_leng, expect_elem_leng );
}

/***
 * 結論としては、いかなる場合であれ、順にシークしていく方法が最速である.
 *
 * begin_quot と end_quot の両文字列が全く同じケースの場合、
 * 以下のような特殊なアルゴリズムで処理するのはどうかと考えるかもしれない.
 * すなわち、例えば str=[ " abc"  "de" "" "fgh" "" ] の場合にまず
 * ZnkStrEx_addSplitStr(C) などを呼び、一時コンテナである splited_strs に
 * splited_strs={[ ],[ abc],[  ],[de],[ ],[],[ ],[fgh],[ ],[],[ ]} という結果を得る.
 * この後、splited_strs の奇数番目のみをとって{[ abc],[de],[],[fgh],[]}を結果として
 * 格納するというものである.
 *
 * しかしながらこのような方法はどのような場合であれ低速なのでここでは却下とする.
 * まずsplited_strsをコンテナとして確保解放する処理にオーバーヘッドがかかる.
 * ( 順当なシークであれば 使用するのはfind処理だけであり、このような構築/破棄の処理は
 * 必要ない ). さらにそれにプッシュする処理を(本来必要とする処理の二倍の回数)行った後、
 * さらにansに奇数番目のみをプッシュしなければならない. これは順当にシークしていく
 * 処理と比べて明らかに無駄である.
 *
 * begin_quot と end_quot が異なる場合のケースについては、そもそも上記の方法は使用できない.
 * 便宜上、begin_quot = "<" 、end_quot = ">" とする.
 *
 * 例1. str=[ < abc>  <de> <> <fgh> <> ] の場合、
 *   仮に begin_quot によるSplit分割を行った場合、
 *   splited_strs={[ ],[ abc>  ],[de> ],[> ],[fgh> ],[> ]}
 *   となる.
 *   これらの２番目以降をとり、それぞれを end_quot まで抽出しつつ ans_list へ push していけば
 *   ans_list = {[ abc],[de],[],[fgh],[]}となる.
 *   この場合はうまくいく.
 *
 * 例2. str=[< abc<>] の場合、
 *   この場合、上記のアルゴリズムではうまくいかない.
 *   ans_list = {[ abc<]} となってほしいところだが、
 *   上記のアルゴリズムでは ans_list = {[ abc], []} となってしまう.
 */
static void
Adapter_addQuotedTokens( Adapter adp, StrListHandle ans_vstr,
		const char* str, size_t str_leng,
		const char* begin_quot, size_t begin_quot_leng,
		const char* end_quot,   size_t end_quot_leng,
		size_t expect_size )
{
	size_t tmp_size;
	size_t pos = 0;

	adp->reserve_( ans_vstr, adp->size_(ans_vstr) + expect_size );

	DECIDE_STRLENG( str_leng, str );
	DECIDE_STRLENG( begin_quot_leng, begin_quot );
	DECIDE_STRLENG( end_quot_leng,   end_quot );

	while( pos < str_leng ){
		/***
		 * この時点では pos は クォート外を指しているはずであるから、
		 * 次の begin_quot を探す.
		 * begin_quotの最初の文字を指している可能性もあるが、この場合もクォート外とみなす.
		 *
		 * begin_quot_leng が 1 の場合は、ZnkMem_lfind_data_BF よりも ZnkMem_lfind_8 の方が
		 * わずかに無駄が少ない. しかしZnkMem_lfind_data_BFでも leng が 1 の場合はただちに
		 * ZnkMem_lfind_8 を呼び出すため、実際上この差はほとんどない. 仮にこの呼び出しを
		 * leng に応じて関数tableに分けたとしても、その抽象化のためのラッパー関数の呼び出し
		 * のオーバーヘッドが一回つくため、ZnkMem_lfind_data_BFを直接呼び出すのと同等か
		 * あるいは(leng>=2の場合)それよりも無駄な呼び出しが一回つくことになる.
		 *
		 * この場合、得られる tmp_size は str+posからの相対位置であることに注意する.
		 */
		tmp_size = ZnkMem_lfind_data_BF( (uint8_t*)str+pos, str_leng-pos,
				(uint8_t*)begin_quot, begin_quot_leng, 1 );
		if( tmp_size == Znk_NPOS ){
			/***
			 * begin_quot がもはや存在しない場合、終了する.
			 */
			return;
		}
		/***
		 * クォート内へ
		 * この後も依然として pos <= str_lengは保たれるはずである.
		 */
		pos += ( tmp_size + begin_quot_leng );

		/***
		 * この時点では pos は クォート内を指しているはずであるから、
		 * 次の end_quot を探す.
		 * end_quotの最初の文字を指している可能性もあるが、この場合もクォート内とみなす.
		 *
		 * end_quot が存在しない場合、strが不正な文法となっていると考えられるが、
		 * 一応最後まで拾って終了する.
		 *
		 * この場合、得られる tmp_size は str+posからの相対位置であるが、
		 * これは同時に、クォートで囲まれた中身のトークンサイズそのものであることに注意する.
		 */
		tmp_size = ZnkMem_lfind_data_BF( (uint8_t*)str+pos, str_leng-pos,
				(uint8_t*)end_quot, end_quot_leng, 1 );
		if( tmp_size == Znk_NPOS ){
			/***
			 * end_quot がみつからない状態でシークが最後まで到達.
			 * とりあえず最後までの部分を push して終了.
			 */
			adp->push_bk_( ans_vstr, str+pos, str_leng-pos );
			return; /* ここでただちに終了する */
		} else {
			/***
			 * 通常のトークンである場合.
			 */
			adp->push_bk_( ans_vstr, str+pos, tmp_size );
		}

		/***
		 * クォート外へ
		 * この後も依然として pos <= str_lengは保たれるはずである.
		 * 特に pos == str_lengならば次のループの始めで終了する形になる.
		 */
		pos += ( tmp_size + end_quot_leng );
	}
}
void
ZnkStrEx_addQuotedTokens( ZnkStrAry ans_vstr,
		const char* str, size_t str_leng,
		const char* begin_quot, size_t begin_quot_leng,
		const char* end_quot,   size_t end_quot_leng,
		size_t expect_size )
{
	Adapter_addQuotedTokens( &st_adapter_vstr, (StrListHandle) ans_vstr,
			str, str_leng,
			begin_quot, begin_quot_leng,
			end_quot,   end_quot_leng,
			expect_size );
}

size_t
ZnkStrEx_getFront_byPattern( const char* str, size_t str_leng,
		size_t begin, const char* ptn, size_t ptn_leng,
		ZnkStr ans )
{
	DECIDE_STRLENG( str_leng, str );
	DECIDE_STRLENG( ptn_leng, ptn );

	/***
	 * これが成立しない場合、Front は常に空であるとみなす.
	 * (すなわち begin を返す)
	 */
	if( begin < str_leng && ptn_leng ){
		size_t end = ZnkMem_lfind_data_BF( (uint8_t*)str+begin, str_leng-begin,
			(uint8_t*)ptn, ptn_leng, 1 );
		if( end == Znk_NPOS ){
			/***
			 * not found.
			 * str[begin] 以降すべてを Front とみなす.
			 */
			if( ans ){ ZnkStr_append( ans, str+begin, str_leng-begin ); }
			return Znk_NPOS;
		} else {
			end += begin; /* 相対位置から絶対位置へ直す */
			if( ans ){ ZnkStr_append( ans, str+begin, end-begin ); }
			return end;
		}
	}
	return begin;
}

size_t
ZnkStrEx_getBack_byPattern( const char* str, size_t str_leng,
		size_t begin, const char* ptn, size_t ptn_leng,
		ZnkStr ans )
{
	DECIDE_STRLENG( str_leng, str );
	DECIDE_STRLENG( ptn_leng, ptn );

	/***
	 * この場合、特に [ begin, str_leng ) の範囲を Backとみなす.
	 */
	if( ptn_leng == 0 ){
		if( ans ){ ZnkStr_append( ans, str+begin, str_leng-begin ); }
		return begin;
	}

	/***
	 * これが成立しない場合、Back は常に空であるとみなす.
	 * (すなわち Znk_NPOS を返す)
	 */
	if( begin < str_leng ){
		size_t end = ZnkMem_lfind_data_BF( (uint8_t*)str+begin, str_leng-begin,
			(uint8_t*)ptn, ptn_leng, 1 );
		if( end == Znk_NPOS ){
			/***
			 * not found.
			 * Back は空であるとみなす.
			 */
			return Znk_NPOS;
		} else {
			end += begin; /* 相対位置から絶対位置へ直す */
			end += ptn_leng; /* end は ptn の開始位置であったため、この加算結果は str_leng以下である */
			if( ans ){ ZnkStr_append( ans, str+end, str_leng-end ); }
			return end;
		}
	}
	return Znk_NPOS;
}


size_t
ZnkStrEx_getKeyAndValEx( const char* str, size_t begin, size_t end,
		size_t* ans_key_begin, size_t* ans_key_end,
		size_t* ans_val_begin, size_t* ans_val_end,
		const char* delimiter,  size_t delimiter_leng,
		const char* skip_chset, size_t skip_chset_leng,
		ZnkStr ans_key, ZnkStr ans_val )
{
	size_t key_begin, key_end, val_begin, val_end;
	const size_t delimiter_pos = ZnkS_find_key_and_val( str, begin, end,
			&key_begin, &key_end,
			&val_begin, &val_end,
			delimiter,  delimiter_leng,
			skip_chset, skip_chset_leng );

	if( ans_key_begin ){ *ans_key_begin = key_begin; }
	if( ans_key_end   ){ *ans_key_end   = key_end; }
	if( ans_val_begin ){ *ans_val_begin = val_begin; }
	if( ans_val_end   ){ *ans_val_end   = val_end; }

	if( ans_key && key_begin != Znk_NPOS ){
		const size_t token_size = ( key_end == Znk_NPOS ) ?  end - key_begin : key_end - key_begin;
		ZnkStr_append( ans_key, str + key_begin, token_size );
	}
	if( ans_val && val_begin != Znk_NPOS ){
		const size_t token_size = ( val_end == Znk_NPOS ) ?  end - val_begin : val_end - val_begin;
		ZnkStr_append( ans_val, str + val_begin, token_size );
	}
	return delimiter_pos;
}


bool
ZnkStrEx_removeFrontCSet( ZnkStr str, const char* skip_chset, size_t skip_chset_leng )
{
	DECIDE_STRLENG( skip_chset_leng, skip_chset );

	if( skip_chset_leng != 0 && !ZnkStr_empty(str) ){
		size_t pos = ZnkS_lfind_one_not_of( ZnkStr_cstr(str), 0, ZnkStr_leng(str), skip_chset, skip_chset_leng );
		if( pos == 0 ){
			return false; /* removeの必要性なし. */
		} else if( pos == Znk_NPOS ){
			/* str のすべての文字は skip_chset からなる. */
			ZnkStr_clear( str );
			return true;
		} else {
			/* [0, pos) までを削除. */
			ZnkStr_cut_front( str, pos );
			return true;
		}
	}
	return false; /* removeの必要性なし. */
}
bool
ZnkStrEx_removeBackCSet( ZnkStr str, const char* skip_chset, size_t skip_chset_leng )
{
	DECIDE_STRLENG( skip_chset_leng, skip_chset );

	if( skip_chset_leng != 0 && !ZnkStr_empty(str) ){
		const size_t str_leng = ZnkStr_leng( str );
		size_t pos = ZnkS_rfind_one_not_of( ZnkStr_cstr(str), 0, str_leng, skip_chset, skip_chset_leng );
		if( pos == str_leng-1 ){
			return false; /* removeの必要性なし. */
		} else if( pos == Znk_NPOS ){
			/* str のすべての文字は skip_chset からなる. */
			ZnkStr_clear( str );
			return true;
		} else {
	 		/* [pos+1, ...) までを削除. */
			ZnkStr_releng( str, pos+1 );
			return true;
		}
	}
	return false; /* removeの必要性なし. */
}


bool 
ZnkStrEx_chompStr( ZnkStr str, size_t pos, const char* ptn, size_t ptn_leng )
{
	const size_t leng = ZnkStr_leng( str );
	DECIDE_STRLENG( ptn_leng, ptn );
	if( ptn_leng == 0 || ptn_leng > leng ){
		/***
		 * ptn_leng == 0 の場合はマッチするとはみなすが、結局何も効果はない.
		 * ptn_leng > leng の場合は明らかにマッチしない.
		 */
		return false;
	}
	if( ZnkStr_isContain( str, pos, ptn, ptn_leng ) ){
		if( pos == Znk_NPOS || pos + ptn_leng == leng ){
			ZnkStr_cut_back( str, ptn_leng );
		} else {
			ZnkStr_erase( str, pos, ptn_leng ); 
		}
		return true;
	}
	return false;
}


void
ZnkStrEx_addRepeatC( ZnkStr ans, char c, size_t num )
{
	/***
	 * この関数は std::string の append( num, c ) メンバ関数に対応するものである.
	 */
	const size_t old_leng = ZnkStr_leng(ans);
	uint8_t* data;
	ZnkStr_releng( ans, old_leng + num );
	data = ZnkBfr_data( ans );
	Znk_memset( data + old_leng, c, num );
}


/***
 * 与えられた uint32_t 型整数 num を bitに分解して考える.
 * 桁の小さい方から数えて最後にbitが立っている位置までの桁数までをen_bitの
 * 配列としてのサイズとする
 *
 * 例 簡単のため8bitで考える.
 *   00000100 のとき bit = 100    (en_bit はサイズ3のuint8_t配列)
 *   00101101 のとき bit = 101101 (en_bit はサイズ6のuint8_t配列)
 *   00000000 のとき bit = 空     (en_bit はサイズ0のuint8_t配列)
 */
Znk_INLINE size_t
getSizeTBits( uint8_t* en_bit, size_t val )
{
	static const size_t bit_num = sizeof(size_t) * 8;
	size_t enable_num;
	size_t idx, ridx;
	for( idx=0; idx<bit_num; ++idx ){
		ridx = bit_num - 1 - idx;
		if( val & (size_t)(0x1 << ridx) ){ break; }
	}
	enable_num = bit_num - idx;
	for( idx=0; idx<enable_num; ++idx ){
		en_bit[ idx ] = (uint8_t)( ( val & (size_t)(0x1 << idx) ) != 0 );
	}
	return enable_num;
}


void
ZnkStrEx_addEmbededStr( ZnkStr ans,
		char c, size_t num,
		const char* emb_str, size_t emb_str_leng,
		char positioning_mode )
{
	DECIDE_STRLENG( emb_str_leng, emb_str );

	if( emb_str_leng >= num ){
		/* add "emb_str" */
		ZnkStr_append( ans, emb_str, emb_str_leng );
	} else {
		const size_t repeat_num = num - emb_str_leng;
		switch( positioning_mode ){
		case 'L':
			/* add "emb_str   " */
			ZnkStr_append( ans, emb_str, emb_str_leng );
			ZnkStrEx_addRepeatC( ans, c, repeat_num );
			break;
		case 'R':
			/* add "   emb_str" */
			ZnkStrEx_addRepeatC( ans, c, repeat_num );
			ZnkStr_append( ans, emb_str, emb_str_leng );
			break;
		case 'C':
			/* add " emb_str  " */
			ZnkStrEx_addRepeatC( ans, c, repeat_num/2 );
			ZnkStr_append( ans, emb_str, emb_str_leng );
			ZnkStrEx_addRepeatC( ans, c, repeat_num/2 );
			if( repeat_num % 2 ){ ZnkStr_add_c( ans, c ); }
			break;
		default :
			/* none */
			break;
		}
	}
	return;
}


size_t
ZnkStrEx_replace_BF( ZnkStr str, size_t begin,
		const char* old_ptn, size_t old_ptn_leng,
		const char* new_ptn, size_t new_ptn_leng,
		size_t seek_depth, size_t delta_to_next )
{
	if( seek_depth ){
		size_t count=0;
		size_t pos_from_begin;
		size_t str_leng = ZnkStr_leng(str);

		DECIDE_STRLENG( old_ptn_leng, old_ptn );
		DECIDE_STRLENG( new_ptn_leng, new_ptn );
		if( delta_to_next == Znk_NPOS ){ delta_to_next = new_ptn_leng; }

		while( begin < str_leng ){
			pos_from_begin = ZnkMem_lfind_data_BF( (uint8_t*)ZnkStr_cstr(str)+begin, str_leng-begin,
					(uint8_t*)old_ptn, old_ptn_leng, 1 );
			if( pos_from_begin == Znk_NPOS ){ break; }
			begin += pos_from_begin;
			++count;

			ZnkBfr_replace( str, begin, old_ptn_leng, (uint8_t*)new_ptn, new_ptn_leng );
			begin += delta_to_next;

			if( count == seek_depth ){ break; }
			str_leng = ZnkStr_leng(str);
		}
		return count;
	}
	return 0;
}

size_t
ZnkStrEx_replace_BMS( ZnkStr str, size_t begin,
		const char* old_ptn, size_t old_ptn_leng, const size_t* old_ptn_occ_table,
		const char* new_ptn, size_t new_ptn_leng,
		size_t seek_depth, size_t delta_to_next )
{
	if( seek_depth ){
		size_t count=0;
		size_t pos_from_begin;
		size_t str_leng = ZnkStr_leng(str);

		DECIDE_STRLENG( old_ptn_leng, old_ptn );
		DECIDE_STRLENG( new_ptn_leng, new_ptn );
		if( delta_to_next == Znk_NPOS ){ delta_to_next = new_ptn_leng; }

		while( begin < str_leng ){
			pos_from_begin = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(str)+begin, str_leng-begin,
					(uint8_t*)old_ptn, old_ptn_leng, 1, old_ptn_occ_table );
			if( pos_from_begin == Znk_NPOS ){ break; }
			begin += pos_from_begin;
			++count;

			ZnkBfr_replace( str, begin, old_ptn_leng, (uint8_t*)new_ptn, new_ptn_leng );

			begin += delta_to_next;

			if( count == seek_depth ){ break; }
			str_leng = ZnkStr_leng(str);
		}
		return count;
	}
	return 0;
}

