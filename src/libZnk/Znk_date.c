#include "Znk_date.h"
#include "Znk_base.h"
#include "Znk_str.h"
#include "Znk_s_base.h"

#if defined(Znk_TARGET_WINDOWS)
#  include <windows.h>
#elif defined(Znk_TARGET_UNIX)
#  include <unistd.h>
#endif

#include <stdio.h>
#include <assert.h>

void ZnkDate_getCurrent( ZnkDate* date )
{
	time_t timer;
	struct tm *t_st;
	
	time(&timer); /* 現在時刻の取得 */
	
	/***
	 * ctime関数は現在時刻を次のような文字列に変換する.
	 *  Wed Mar 06 00:35:26 2002
	 *  使用例 : printf("%s\n", ctime(&timer));
	 * これはあまりよい表示方法ではないためここでは採用しない.
	 */
	
	t_st = localtime(&timer); /* 現在時刻を構造体に変換 */

	/***
	 * struct tm {
	 *   int tm_sec;   // 秒 [0-61] 最大2秒までのうるう秒を考慮
	 *   int tm_min;   // 分 [0-59]
	 *   int tm_hour;  // 時 [0-23]
	 *   int tm_mday;  // 日 [1-31]
	 *   int tm_mon;   // 月 [0-11] 0から始まることに注意
	 *   int tm_year;  // 年 [1900からの経過年数]
	 *   int tm_wday;  // 曜日 [0:日 1:月 ... 6:土]
	 *   int tm_yday;  // 年内の通し日数 [0-365] 0から始まることに注意
	 *   int tm_isdst; // 夏時間が無効であれば 0
	 * };
	 */
	ZnkDate_set_year(   date, (uint16_t)( t_st->tm_year + 1900 ) );
	ZnkDate_set_month(  date, (uint8_t) ( t_st->tm_mon  + 1 ) ); /* 月は+1 */
	ZnkDate_set_day(    date, (uint8_t) ( t_st->tm_mday ) );
	ZnkDate_set_wday(   date, (uint8_t) ( t_st->tm_wday ) ); /* 曜日(0:日 1:月 ... 6:土) */
	ZnkDate_set_hour(   date, (uint8_t) ( t_st->tm_hour ) );
	ZnkDate_set_minute( date, (uint8_t) ( t_st->tm_min ) );
	ZnkDate_set_second( date, (uint8_t) ( t_st->tm_sec ) );
}

static const char* st_month_table[] = {
	"", /* dummy */
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};
static const char* st_wday_table[] = {
	"Sun", "Man", "Tue", "Wed", "Thu", "Fri", "Sat",
};
void
ZnkDate_getStr( ZnkStr str, size_t pos, const ZnkDate* date, ZnkDateStrType type )
{
	static const char* tz = "GMT"; /* とりあえず固定 */

	switch( type ){
	case ZnkDateStr_e_HttpLastModified:
		ZnkStr_snprintf( str, pos, Znk_NPOS, 
				"%s, %02d %s %d %02d:%02d:%02d %s",
				st_wday_table[ (int)ZnkDate_wday( date ) ],
				(int)ZnkDate_day( date ),
				st_month_table[ (int)ZnkDate_month( date ) ],
				(int)ZnkDate_year( date ),
				(int)ZnkDate_hour( date ),
				(int)ZnkDate_minute( date ),
				(int)ZnkDate_second( date ),
				tz );
		break;
	case ZnkDateStr_e_JavascriptLastModified:
		ZnkStr_snprintf( str, pos, Znk_NPOS, 
				"%02d/%02d/%d %02d:%02d:%02d",
				(int)ZnkDate_month( date ),
				(int)ZnkDate_day( date ),
				(int)ZnkDate_year( date ),
				(int)ZnkDate_hour( date ),
				(int)ZnkDate_minute( date ),
				(int)ZnkDate_second( date ) );
		break;
	default:
		ZnkStr_snprintf( str, pos, Znk_NPOS, 
				"%d/%02d/%02d %02d:%02d:%02d",
				(int)ZnkDate_year( date ),
				(int)ZnkDate_month( date ),
				(int)ZnkDate_day( date ),
				(int)ZnkDate_hour( date ),
				(int)ZnkDate_minute( date ),
				(int)ZnkDate_second( date ) );
		break;
	}

}
static int 
findTableStr( const char** table, size_t table_size, const char* query_str )
{
	size_t i;
	for( i=0; i<table_size; ++i ){
		if( ZnkS_eq( table[ i ], query_str ) ){
			break;
		}
	}
	return (int)i;
}
bool
ZnkDate_scanStr( ZnkDate* date, const char* str, ZnkDateStrType type )
{
	char wday_str[ 16 ]      = "";
	char month_str[ 16 ] = "";
	char tz[ 16 ]        = "GMT";
	int year = 0, month  = 0, day = 0;
	int hour = 0, minute = 0, second = 0;
	int wday = 0;

	switch( type ){
	case ZnkDateStr_e_HttpLastModified:
		sscanf( str, "%s, %02d %s %d %02d:%02d:%02d %s",
				wday_str, &day, month_str, &year,
				&hour, &minute, &second,
				tz );
		month = findTableStr( st_month_table, Znk_NARY(st_month_table), month_str );
		if( month >= Znk_NARY(st_month_table) ){
			month = 0;
		}
		wday = findTableStr( st_wday_table, Znk_NARY(st_wday_table), wday_str );
		if( wday >= Znk_NARY(st_wday_table) ){
			wday = 0;
		}
		ZnkDate_set_wday(  date, (uint8_t)wday );
		break;
	case ZnkDateStr_e_JavascriptLastModified:
		sscanf( str, "%02d/%02d/%d %02d:%02d:%02d",
				&month, &day, &year,
				&hour, &minute, &second );
		break;
	default:
		sscanf( str, "%d/%02d/%02d %02d:%02d:%02d",
				&year, &month, &day,
				&hour, &minute, &second );
		break;
	}

	ZnkDate_set_year(  date, (uint16_t)year );
	ZnkDate_set_month( date,  (uint8_t)month );
	ZnkDate_set_day(   date,  (uint8_t)day );
	ZnkDate_set_hour(   date, (uint8_t)hour );
	ZnkDate_set_minute( date, (uint8_t)minute );
	ZnkDate_set_second( date, (uint8_t)second );
	return true;

}

int ZnkDate_compareDay( const ZnkDate* date1, const ZnkDate* date2 )
{
	/***
	 * year, month, day とレベルが大きい順に上位bitから割り振られている.
	 * よってこれをuint32_t型の整数として純粋に比較しても、その結果は
	 * year, month, day での比較結果と一致するはずである.
	 */
	int64_t diff = (int64_t)date1->d1_ - (uint64_t)date2->d1_;
	return ( diff == 0 ) ? 0 : ( diff > 0 ) ? 1 : -1; 
}
long
ZnkDate_diffSecond( const ZnkDate* date1, const ZnkDate* date2, uint32_t delta_day )
{
	const int hour1   = (int)ZnkDate_hour( date1 );
	const int hour2   = (int)ZnkDate_hour( date2 );
	const int minute1 = (int)ZnkDate_minute( date1 );
	const int minute2 = (int)ZnkDate_minute( date2 );
	const int second1 = (int)ZnkDate_second( date1 );
	const int second2 = (int)ZnkDate_second( date2 );
	/* compare == 0 のときは 日付(d1)は同じ */
	const int compare = ZnkDate_compareDay( date1, date2 );
	/***
	 * compare == 1 のとき
	 *   まずdate1からdate2を引く計算をし、それに  compare*delta_day*24*3600 を加える.
	 * compare == -1のとき
	 *   まずdate2からdate1を引く計算をし、それに -compare*delta_day*24*3600 を加える.
	 *   さらにその結果の式の符号を逆転させれば、結局 compare == 1 の式と一致する.
	 * compare == 0 のとき
	 *   単にdate1からdate2を引く計算をすればよい.
	 *   このとき明らかに compare == 1 の式と一致する.
	 *
	 * 結局どのような場合でも以下の式(compare==1の場合の形式)に統一されることがわかる.
	 */
	return ( hour1 - hour2 ) * 3600 + ( minute1 - minute2 ) * 60 + second1 - second2
		+ compare * delta_day * 24 * 3600;
}

uint8_t
ZnkDate_getEndDayOfManth( unsigned int year, unsigned int month )
{
	switch( month ){
	case 1:  return 31;
	case 2:  return ZnkDate_isGregorianLeapYear( year ) ? 29 : 28;
	case 3:  return 31;
	case 4:  return 30;
	case 5:  return 31;
	case 6:  return 30;
	case 7:  return 31;
	case 8:  return 31;
	case 9:  return 30;
	case 10: return 31;
	case 11: return 30;
	case 12: return 31;
	default: break;
	}
	assert( 0 );
	return 0;
}

uint8_t
ZnkDate_calcWDay( unsigned int year, unsigned int month, unsigned int day )
{
	/***
	 * table of [ (13*month+8)/5 ] mod 7
	 * ただしmonth=1, 2 の場合は13, 14 に読み替えて計算してある.
	 */
	static const unsigned int st_tbl13[] = {
		0,    /* dummy */
		0, 3, /* as month=13, month=14 */
		2, 5, 0, 3,
		5, 1, 4, 6, 2, 4,
	};
	/***
	 * 1, 2月は前年度の13月, 14月が与えられたものとみなす.
	 */
	year -= month < 3;
	return (uint8_t)( ( year + year/4 - year/100 + year/400 + st_tbl13[ month ] + day ) % 7 );
}

void
ZnkDate_getNDaysAgo( ZnkDate* ans, ZnkDate* current, unsigned int ndays )
{
	uint16_t year  = ZnkDate_year(  current );
	uint8_t  month = ZnkDate_month( current );
	uint8_t  day   = ZnkDate_day(   current );
	if( (unsigned int)day > ndays ){
		*ans = *current;
		ZnkDate_set_year(  ans, year );
		ZnkDate_set_month( ans, month );
		ZnkDate_set_day(   ans, (uint8_t)(day-ndays) );
	} else {
		unsigned int remain_day = ndays - (unsigned int)day;
		unsigned int end_day    = 0;
		while( true ){
			/* 一つ前の月の情報を取得 */
			if( month > 1 ){
				--month;
			} else {
				--year;
				month = 12;
			}
			end_day = ZnkDate_getEndDayOfManth( year, month );

			/* remain_day が一つ前の月の日数以上ならさらに月を前へと辿る */
			if( remain_day >= end_day ){
				remain_day -= end_day;
			} else {
				break;
			}
		}
		*ans = *current;
		ZnkDate_set_year(  ans, year );
		ZnkDate_set_month( ans, month );
		ZnkDate_set_day(   ans, (uint8_t)(end_day - remain_day) );
	}
	/* 曜日の更新 */
	ZnkDate_updateWDay( ans );
}
void
ZnkDate_getNDaysLater( ZnkDate* ans, ZnkDate* current, unsigned int ndays )
{
	uint16_t year  = ZnkDate_year(  current );
	uint8_t  month = ZnkDate_month( current );
	uint8_t  day   = ZnkDate_day(   current );
	unsigned int end_day = ZnkDate_getEndDayOfManth( year, month );

	if( end_day >= day + ndays ){
		*ans = *current;
		ZnkDate_set_year(  ans, year );
		ZnkDate_set_month( ans, month );
		ZnkDate_set_day(   ans, (uint8_t)(day+ndays) );
	} else {
		unsigned int remain_day = day + ndays - end_day;
		while( true ){
			/* 一つ後の月の情報を取得 */
			if( month < 12 ){
				++month;
			} else {
				++year;
				month = 1;
			}
			end_day = ZnkDate_getEndDayOfManth( year, month );

			/* remain_day が一つ後の月の日数より大きいならさらに月を後へと辿る */
			if( remain_day > end_day ){
				remain_day -= end_day;
			} else {
				break;
			}
		}
		*ans = *current;
		ZnkDate_set_year(  ans, year );
		ZnkDate_set_month( ans, month );
		ZnkDate_set_day(   ans, (uint8_t)remain_day );
	}
	/* 曜日の更新 */
	ZnkDate_updateWDay( ans );
}

