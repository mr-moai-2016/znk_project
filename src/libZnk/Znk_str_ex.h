#ifndef INCLUDE_GUARD__Znk_str_ex_h__
#define INCLUDE_GUARD__Znk_str_ex_h__

#include <Znk_base.h>
#include <Znk_str.h>
#include <Znk_str_dary.h>
#include <Znk_stdc.h>

Znk_EXTERN_C_BEGIN


void
ZnkStrEx_addSplitC( ZnkStrDAry ans_list,
		const char* str, size_t str_leng,
		char delimit_c, const bool count_void_str, size_t expect_size );


void
ZnkStrEx_addSplitCSet( ZnkStrDAry ans_list,
		const char* str,   size_t str_leng,
		const char* chset, size_t chset_leng,
		size_t expect_size );

void
ZnkStrEx_addSplitStr( ZnkStrDAry ans_vstr, const char* str, size_t str_leng,
		const char* delimiter, size_t delimiter_leng,
		const bool count_void_str, size_t expect_size );

void
ZnkStrEx_addJoin( ZnkStr ans, const ZnkStrDAry str_list,
		const char* connector, size_t connector_leng, size_t expect_elem_leng );

void
ZnkStrEx_addQuotedTokens( ZnkStrDAry ans_vstr,
		const char* str, size_t str_leng,
		const char* begin_quot, size_t begin_quot_leng,
		const char* end_quot,   size_t end_quot_leng,
		size_t expect_size );

/**
 * str を与え、その beginで指定した位置以降をスキャンし、ptn で指定した
 * パターンが現れる最初の位置を end とする. このとき str における
 * [ begin, end ) の範囲の文字列をここでは Front と呼び、この関数では
 * これを取得する. ただし ptn が存在しない場合は end は Znk_NPOSとなり、
 * その場合 Front は [ begin, str_leng ) までの範囲を示すものとする.
 *
 * @param begin
 *  str におけるスキャン開始位置.
 *  Znk_NPOS や str_leng を越える値を指定した場合、Frontは空とみなされる.
 *  すなわち beginの値そのものが返り、ansには何も連結されない.
 *
 * @param ans:
 *  ここに 非NULLの ZnkStr値を指定した場合は、上記における Front の文字列が
 *  ans の後ろに連結される. ans が NULLの場合はこの処理は省略される.
 *
 * @return 
 *  end を返す.
 *  ただし、ptnが存在しなかった場合は Znk_NPOS を返す.
 *  このとき ans には上記に示すFrontの範囲が連結されるものとする.
 *  ここで、Znk_NPOS の代わりに str_lengを返す仕様にしてもよかったが、その場合
 *  呼び出し側は str の実際の長さを常に把握しておく必要性が生じる. これは場合に
 *  よっては無駄なので、Znk_NPOSを返す仕様に落ち着く形となった.
 *
 *  初期値として begin < str_leng && ptn_leng が満たされなかった場合は、
 *  ptn の値が何であれ、begin の値そのものが返る(このとき ans には
 *  何も連結されない).
 *
 * @exam
 *  str = 'ABCdefghiJKLmn'
 *  begin=3,
 *  pattern='JKL'
 *  end(戻り値) = 9
 *  ans = 'defghi'
 */
size_t
ZnkStrEx_getFront_byPattern( const char* str, size_t str_leng,
		size_t begin, const char* ptn, size_t ptn_leng,
		ZnkStr ans );

size_t
ZnkStrEx_getBack_byPattern( const char* str, size_t str_leng,
		size_t begin, const char* ptn, size_t ptn_leng,
		ZnkStr ans );


size_t
ZnkStrEx_getKeyAndValEx( const char* str, size_t begin, size_t end,
		size_t* ans_key_begin, size_t* ans_key_end,
		size_t* ans_val_begin, size_t* ans_val_end,
		const char* delimiter,  size_t delimiter_leng,
		const char* skip_chset, size_t skip_chset_leng,
		ZnkStr ans_key, ZnkStr ans_val );
Znk_INLINE size_t
ZnkStrEx_getKeyAndVal( const char* str, size_t begin, size_t end,
		const char* delimiter, const char* skip_chset,
		ZnkStr ans_key, ZnkStr ans_val )
{
	return ZnkStrEx_getKeyAndValEx( str, begin, end,
			NULL, NULL, NULL, NULL,
			delimiter,  Znk_NPOS,
			skip_chset, Znk_NPOS,
			ans_key, ans_val );
}

bool
ZnkStrEx_removeFrontCSet( ZnkStr str, const char* skip_chset, size_t skip_chset_leng );
bool
ZnkStrEx_removeBackCSet( ZnkStr str, const char* skip_chset, size_t skip_chset_leng );
/**
 * @brief
 * ZnkStrEx_removeBackCSet と ZnkStrEx_removeFrontCSet をこの順番で行う.
 */
Znk_INLINE void 
ZnkStrEx_removeSideCSet( ZnkStr str, const char* skip_chset, size_t skip_chset_leng ){
	ZnkStrEx_removeBackCSet ( str, skip_chset, skip_chset_leng );
	ZnkStrEx_removeFrontCSet( str, skip_chset, skip_chset_leng );
}

bool 
ZnkStrEx_chompStr( ZnkStr str, size_t pos, const char* ptn, size_t ptn_leng );
Znk_INLINE bool 
ZnkStrEx_chompFirstStr( ZnkStr str, const char* ptn, size_t ptn_leng ){
	return ZnkStrEx_chompStr( str, 0, ptn, ptn_leng );
}
Znk_INLINE bool 
ZnkStrEx_chompLastStr( ZnkStr str, const char* ptn, size_t ptn_leng ){
	return ZnkStrEx_chompStr( str, Znk_NPOS, ptn, ptn_leng );
}

void
ZnkStrEx_addRepeatC( ZnkStr ans, char c, size_t num );

void
ZnkStrEx_addEmbededStr( ZnkStr ans,
		char c, size_t num,
		const char* emb_str, size_t emb_str_leng,
		char positioning_mode );

size_t
ZnkStrEx_replace_BF( ZnkStr str, size_t begin,
		const char* old_ptn, size_t old_ptn_leng,
		const char* new_ptn, size_t new_ptn_leng,
		size_t seek_depth );

size_t
ZnkStrEx_replace_BMS( ZnkStr str, size_t begin,
		const char* old_ptn, size_t old_ptn_leng, const size_t* old_ptn_occ_table,
		const char* new_ptn, size_t new_ptn_leng,
		size_t seek_depth );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
