#ifndef INCLUDE_GUARD__Znk_date_h__
#define INCLUDE_GUARD__Znk_date_h__

#include <Znk_str.h>
#include <time.h>

Znk_EXTERN_C_BEGIN

typedef struct {
	/***
	 * d1_
	 *   上位16bit : year
	 *   次の8bit  : month(1から12まで)
	 *   次の8bit  : day
	 */
	uint32_t d1_;
	/***
	 * d2_
	 *   上位8bit : wday 曜日(0:日 1:月 ... 6:土)
	 *   次の8bit : hour
	 *   次の8bit : min
	 *   次の8bit : sec
	 */
	uint32_t d2_;
}ZnkDate;

typedef enum {
	 ZnkDateStr_e_Std=0
	,ZnkDateStr_e_HttpLastModified
	,ZnkDateStr_e_JavascriptLastModified
} ZnkDateStrType;

Znk_INLINE uint16_t ZnkDate_year(   const ZnkDate* date ){ return (uint16_t)( date->d1_ >> 16 ); }
Znk_INLINE uint8_t  ZnkDate_month(  const ZnkDate* date ){ return (uint8_t)( (date->d1_ & 0x0000ff00) >> 8 ); }
Znk_INLINE uint8_t  ZnkDate_day(    const ZnkDate* date ){ return (uint8_t)(  date->d1_ & 0x000000ff ); }
Znk_INLINE uint8_t  ZnkDate_wday(   const ZnkDate* date ){ return (uint8_t)(  date->d2_ >> 24 ); }
Znk_INLINE uint8_t  ZnkDate_hour(   const ZnkDate* date ){ return (uint8_t)( (date->d2_ & 0x00ff0000) >> 16 ); }
Znk_INLINE uint8_t  ZnkDate_minute( const ZnkDate* date ){ return (uint8_t)( (date->d2_ & 0x0000ff00) >> 8 ); }
Znk_INLINE uint8_t  ZnkDate_second( const ZnkDate* date ){ return (uint8_t)(  date->d2_ & 0x000000ff ); }

Znk_INLINE void
ZnkDate_set_year(   ZnkDate* date, uint16_t year   ){ date->d1_ &= 0x0000ffff; date->d1_ |= (((uint32_t)year) <<16); }
Znk_INLINE void
ZnkDate_set_month(  ZnkDate* date, uint8_t  month  ){ date->d1_ &= 0xffff00ff; date->d1_ |= (((uint32_t)month)<< 8); }
Znk_INLINE void
ZnkDate_set_day(    ZnkDate* date, uint8_t  day    ){ date->d1_ &= 0xffffff00; date->d1_ |= (uint32_t)day; }
Znk_INLINE void
ZnkDate_set_wday(   ZnkDate* date, uint8_t  wday   ){ date->d2_ &= 0x00ffffff; date->d2_ |= (((uint32_t)wday)  <<24); }
Znk_INLINE void
ZnkDate_set_hour(   ZnkDate* date, uint8_t  hour   ){ date->d2_ &= 0xff00ffff; date->d2_ |= (((uint32_t)hour)  <<16); }
Znk_INLINE void
ZnkDate_set_minute( ZnkDate* date, uint8_t  minute ){ date->d2_ &= 0xffff00ff; date->d2_ |= (((uint32_t)minute)<< 8); }
Znk_INLINE void
ZnkDate_set_second( ZnkDate* date, uint8_t  second ){ date->d2_ &= 0xffffff00; date->d2_ |= (uint32_t)second; }

Znk_INLINE bool ZnkDate_eq( const ZnkDate* date1, const ZnkDate* date2 ){
	return (bool)( date1->d1_ == date2->d1_ && date1->d2_ == date2->d2_ );
}

/**
 * @brief
 * 今現在の日付と時間を得る.
 */
void ZnkDate_getCurrent( ZnkDate* date );

/**
 * @brief
 * 指定したZnkDateを文字列に変換する.
 *
 * @param pos:
 *   ZnkStr_snprintfにおけるposと同じであり、結果が上書きされる位置を示す.
 *   例えば0の場合、strの開始位置から結果が上書きされる. Znk_NPOSを指定する
 *   こともでき、その場合はstrの最後尾に結果が連結される.
 *
 * @param type:
 *   出力文字列の形式のタイプを指定する.
 */
void ZnkDate_getStr( ZnkStr str, size_t pos, const ZnkDate* date, ZnkDateStrType type );
/**
 * @brief
 * 指定した文字列をスキャンしZnkDateを得る.
 *
 * @param type:
 *   指定文字列の形式のタイプを指定する.
 */
bool ZnkDate_scanStr( ZnkDate* date, const char* str, ZnkDateStrType type );

/**
 * @brief
 * 指定した２つのZnkDateの日付部(d1部)を比較する.
 * このときd2部は完全に無視される.
 * date1の方が日付が未来の場合は 1を返す.
 * date2の方が日付が未来の場合は-1を返す.
 * 双方の日付が全く同じならば 0 を返す.
 */
int  ZnkDate_compareDay( const ZnkDate* date1, const ZnkDate* date2 );
/**
 * @brief
 * 指定した２つのZnkDateの時間部(d2部)の差を秒数で計算して取得する.
 * このときdate1とdate2が示す日付(d1)は全く同じであるかまたは delta_day 日の差しかないことを前提とする.
 * これらの差が delta_day 日ではない場合、正しい結果とはならないため注意する.
 * date1とdate2のどちらを未来にしてもよいが、date1が未来の場合正の秒数を、date2が未来の場合負の秒数を返す.
 * ほとんどのケースではdelta_dayには1を指定する形になると思われる.
 * d1が全く同じ場合はdelta_dayの指定は単に無視される.
 *
 * @note
 *  上記で言う delta_day 日の日付の差とは、両者の厳密な時刻が24*delta_day時間という意味ではなく、
 * あくまで日付(d1)値のみで比較した場合の差を意味する. 例えば、2000/01/11 23:10:00 と 2000/01/10 1:10:00 
 * の２つが指定され、また delta_dayとして 1 を指定した場合、（これらは厳密な時刻でいえば46時間の差があり、
 * 24時間を超えているけれども)、d1のみに注目した場合の日付差は1日であるので、認められる入力いうことである.
 * d1の差の厳密な計算を内部で行っていない理由は、それを行うには月ごとの日数からの累計などを考慮するために
 * ループ文をまわす必要があり、その処理には時間がかかるものとなるためである. 
 * それゆえここでは単にどちらが未来かを内部で(ZnkDate_compareDayを呼び出して)判断してはいるものの、
 * その厳密な日付差についてはdelta_dayを参照することにしている.
 */
long ZnkDate_diffSecond( const ZnkDate* date1, const ZnkDate* date2, uint32_t delta_day );

/**
 * @brief 
 *  閏年判定.
 *
 * @param year:
 *   1582年以降が有効である.
 *
 * @note
 *  この判定式はグレゴリオ暦に限り有効である.
 *  グレゴリオ暦が制定されたのは1582年であるから、yearにはそれ以降の年が与えられなければならない.
 *  しかし実用上は何も問題ないであろう.
 */
Znk_INLINE bool
ZnkDate_isGregorianLeapYear( unsigned int year )
{
	/***
	 * 400 => true, 100 => false, 4 => true, else false の順に判定するのが一見するともっともわかり易いそうである.
	 * しかし現実の頻度から考えてまず4で判定すべき.
	 * 即ち基本アルゴリズムは以下となる.
	 *
	 * if( year % 4 == 0 ){
	 *   if( year % 100 == 0 ){
	 *     if( year % 400 == 0 ){
	 *       return true;
	 *     }
	 *     return false;
	 *   }
	 *   return true;
	 * }
	 * return false;
	 *
	 * 以下はこれと等価な式に変換したもの.
	 */
	return (bool)( ( year % 4 == 0 ) && ( ( year % 100 != 0 ) || ( year % 400 == 0 ) ) );
}
Znk_INLINE size_t
ZnkDate_getNumDayOfYear( unsigned int year )
{
	return ZnkDate_isGregorianLeapYear( year ) ? 366 : 365;
}

/**
 * @brief
 *  与えられた年月よりその月の最終日(つまり日数でもある)を得る.
 *
 * @param year:
 *   1582年以降が有効である.
 *
 * @param month:
 *   1 から 12 を与える.
 */
uint8_t
ZnkDate_getEndDayOfManth( unsigned int year, unsigned int month );

/**
 * @brief
 *  与えられた日付より曜日を得る.
 *
 * @param year:
 *   歴史的な経緯により1752年以降が有効である.
 *
 * @param month:
 *   1 から 12 を与える.
 *
 * @param day:
 *   指定されたyear, month に矛盾しない範囲を与えなければならない.
 *
 * @return:
 *   0=日, 1=月, ..., 6=土.
 *
 * @note:
 *   参考: Zellerの公式, C FAQ
 */
uint8_t
ZnkDate_calcWDay( unsigned int year, unsigned int month, unsigned int day );

Znk_INLINE void
ZnkDate_updateWDay( ZnkDate* date ){
	ZnkDate_set_wday( date,
			ZnkDate_calcWDay( ZnkDate_year( date ), ZnkDate_month( date ), ZnkDate_day( date ) ) );
}

/**
 * @brief
 *  与えられた日付currentよりndays日前のZnkDateを得る.
 */
void
ZnkDate_getNDaysAgo( ZnkDate* ans, ZnkDate* current, unsigned int ndays );
/**
 * @brief
 *  与えられた日付currentよりndays日後のZnkDateを得る.
 */
void
ZnkDate_getNDaysLater( ZnkDate* ans, ZnkDate* current, unsigned int ndays );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
