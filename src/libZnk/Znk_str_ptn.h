#ifndef INCLUDE_GUARD__Znk_str_ptn_h__
#define INCLUDE_GUARD__Znk_str_ptn_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

typedef int (*ZnkStrPtnProcessFuncT)( ZnkStr str, void* arg );

const uint8_t*
ZnkStrPtn_getBetween( ZnkStr ans, const uint8_t* data, const size_t data_size,
		const char* ptn_begin, const char* ptn_end );

bool
ZnkStrPtn_replaceBetween( ZnkStr ans,
		const uint8_t* data, size_t data_size,
		const char* ptn_begin, const char* ptn_end,
		const char* replace_str, bool replaced_with_ptn );

/**
 * @brief
 * quo_beg と quo_end で囲まれた範囲を検出した場合、
 * それをInQuote領域とみなし、その領域を加工するための event_handler( inquoted_str, event_arg ) を呼び出す.
 *
 * coe_beg と coe_end はコメントアウト記号とみなし、
 * このコメントアウト内にあるquo_beg, quo_end はInQuote領域の境界とはみなさず無視する.
 * coe_beg, coe_endにはNULLを指定でき、その場合はコメントアウト領域は存在しないものとする.
 * 特に coe_end のみが NULLの場合、coe_beg はエスケープシーケンスとみなし、coe_begの直後に
 * quo_endが現れた場合、それを無視するものとする.
 *
 * @param include_quote:
 *   trueのときはevent_handlerの処理対象として引き渡すstrにquo_beg, quo_endそのものまで含ませる.
 *   falseのときはそのstrにquo_beg, quo_endを含ませない(quo_beg, quo_endに囲まれた中身だけを引き渡す).
 */
void
ZnkStrPtn_invokeInQuote( ZnkStr str,
		const char* quo_beg, const char* quo_end,
		const char* coe_beg, const char* coe_end,
		const ZnkStrPtnProcessFuncT event_handler, void* event_arg,
		bool include_quote );

/**
 * @brief
 * quo_beg と quo_end で囲まれた範囲を検出した場合、
 * それをInQuote領域とみなし、その領域にもしold_ptnが含まれていた場合はnew_ptnで置き換える.
 *
 * coe_beg と coe_end は ZnkStrPtn_invokeInQuote と同じ意味である.
 *
 * @param include_quote:
 *   trueのときはold_ptnを探索する処理対象として引き渡すstrにquo_beg, quo_endそのものまで含ませる.
 *   falseのときはそのstrにquo_beg, quo_endを含ませない(quo_beg, quo_endに囲まれた中身だけを引き渡す).
 *
 * @param delta_to_next:
 *   ZnkStrEx_replace_BF におけるdelta_to_nextと全く同じ意味である.
 *   即ち検索対象範囲内において置換処理が終わった直後にカーソルをどれだけ動かすかを指定する.
 */
void
ZnkStrPtn_replaceInQuote( ZnkStr str,
		const char* quo_beg, const char* quo_end,
		const char* coe_beg, const char* coe_end,
		const char* old_ptn, const char* new_ptn, bool include_quote,
		size_t delta_to_next );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
