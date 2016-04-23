#ifndef INCLUDE_GUARD__Znk_str_fio_h__
#define INCLUDE_GUARD__Znk_str_fio_h__

#include <Znk_str.h>
#include <Znk_stdc.h>

Znk_EXTERN_C_BEGIN

/**
 * @brief
 *   line の pos の位置へ fp で指定したtextファイルの内容を1行fgetsで読み込む.
 *   このとき、lineのサイズは、pos の値に実際に読み込まれたサイズが加算された
 *   値に変更される(つまり、もともとのlineのサイズがこの値よりも大きい場合は、
 *   lineのサイズは結果的に減少することもある). lineのcapacityは、必要に応じて
 *   自動的に拡張される.
 *
 *   pos に 0 を指定した場合、典型的なfgets処理にもっとも近い挙動になる.
 *   またposにZnk_NPOSやlineの現在のサイズ以上の値を指定することもでき、
 *   その場合は、line の後ろに読み込まれた内容が連結される.
 *
 *   block_size は、内部で使用する fgets が一度に読み込むバッファサイズになる.
 *   また一方でこの値で指定したサイズを一単位としてcapacityは増大していく.
 *   この値があまりに小さい場合、内部でfgetsを何度も呼び出すことになってしまうため、
 *   読み込む行の長さが大きい場合効率が落ちる. 一方であまりに大きな値を指定した場合
 *   lineのcapacityに存在する無駄な領域のサイズが大きくなり、メモリ効率が低下する.
 *   通常はblock_size=4096程度を推奨する. 尚、4未満の値を指定した場合、内部で
 *   自動的に4に補正される.
 */
bool ZnkStrFIO_fgets( ZnkStr line, size_t pos, size_t block_size, ZnkFile fp );


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
