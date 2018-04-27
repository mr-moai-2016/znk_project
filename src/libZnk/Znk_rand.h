#ifndef INCLUDE_GUARD__Znk_rand_h__
#define INCLUDE_GUARD__Znk_rand_h__

/* Generating rand by Mersenne twister method.  */

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

/**
 * @brief
 *  内部でseedを設定しシャッフル処理を行う.
 *  seed_deltaに 0 以外を指定した場合は内部で生成された時間によるseed値に
 *  さらにバイアス値を可算することができる.
 */
void ZnkRand_shuffle( unsigned long seed_delta );

/**
 * @brief
 *  [ 0, num ) までのランダムunsigned longを得る.
 */
unsigned long ZnkRand_getRandUL( unsigned long num );

/**
 * @brief
 *  [ min_real, max_real ) までのランダムdouble値を得る.
 *  ただし min_real > max_real の場合 ( max_real, min_real ] の範囲となる.
 */
double ZnkRand_getRandR( double min_real, double max_real );

/**
 * @brief
 *  [ min_ival, max_ival ) までのランダムint値を得る.
 *  ただし min_ival > max_ival の場合 ( max_ival, min_ival ] の範囲となる.
 *
 * @note
 *  ここでは実装として単にZnkRand_getRandRを呼び出している.
 *  これを int専用の実装にしたとしても、たいして高速化できない.
 *  内部で減算処理が発生するため、例えば INT_MIN, INT_MAX などを指定した場合でも問題なく処理するためには
 *  結局もっと高い精度(int64_tやdouble)へ変換せねばならないためである.
 */
Znk_INLINE int ZnkRand_getRandI( int min_ival, int max_ival ){
	return (int)ZnkRand_getRandR( min_ival, max_ival );
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
