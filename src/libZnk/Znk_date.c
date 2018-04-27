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
	
	time(&timer); /* Œ»İ‚Ìæ“¾ */
	
	/***
	 * ctimeŠÖ”‚ÍŒ»İ‚ğŸ‚Ì‚æ‚¤‚È•¶š—ñ‚É•ÏŠ·‚·‚é.
	 *  Wed Mar 06 00:35:26 2002
	 *  g—p—á : printf("%s\n", ctime(&timer));
	 * ‚±‚ê‚Í‚ ‚Ü‚è‚æ‚¢•\¦•û–@‚Å‚Í‚È‚¢‚½‚ß‚±‚±‚Å‚ÍÌ—p‚µ‚È‚¢.
	 */
	
	t_st = localtime(&timer); /* Œ»İ‚ğ\‘¢‘Ì‚É•ÏŠ· */

	/***
	 * struct tm {
	 *   int tm_sec;   // •b [0-61] Å‘å2•b‚Ü‚Å‚Ì‚¤‚é‚¤•b‚ğl—¶
	 *   int tm_min;   // •ª [0-59]
	 *   int tm_hour;  //  [0-23]
	 *   int tm_mday;  // “ú [1-31]
	 *   int tm_mon;   // Œ [0-11] 0‚©‚çn‚Ü‚é‚±‚Æ‚É’ˆÓ
	 *   int tm_year;  // ”N [1900‚©‚ç‚ÌŒo‰ß”N”]
	 *   int tm_wday;  // —j“ú [0:“ú 1:Œ ... 6:“y]
	 *   int tm_yday;  // ”N“à‚Ì’Ê‚µ“ú” [0-365] 0‚©‚çn‚Ü‚é‚±‚Æ‚É’ˆÓ
	 *   int tm_isdst; // ‰ÄŠÔ‚ª–³Œø‚Å‚ ‚ê‚Î 0
	 * };
	 */
	ZnkDate_set_year(   date, (uint16_t)( t_st->tm_year + 1900 ) );
	ZnkDate_set_month(  date, (uint8_t) ( t_st->tm_mon  + 1 ) ); /* Œ‚Í+1 */
	ZnkDate_set_day(    date, (uint8_t) ( t_st->tm_mday ) );
	ZnkDate_set_wday(   date, (uint8_t) ( t_st->tm_wday ) ); /* —j“ú(0:“ú 1:Œ ... 6:“y) */
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
	static const char* tz = "GMT"; /* ‚Æ‚è‚ ‚¦‚¸ŒÅ’è */

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
	 * year, month, day ‚ÆƒŒƒxƒ‹‚ª‘å‚«‚¢‡‚ÉãˆÊbit‚©‚çŠ„‚èU‚ç‚ê‚Ä‚¢‚é.
	 * ‚æ‚Á‚Ä‚±‚ê‚ğuint32_tŒ^‚Ì®”‚Æ‚µ‚Äƒˆ‚É”äŠr‚µ‚Ä‚àA‚»‚ÌŒ‹‰Ê‚Í
	 * year, month, day ‚Å‚Ì”äŠrŒ‹‰Ê‚Æˆê’v‚·‚é‚Í‚¸‚Å‚ ‚é.
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
	/* compare == 0 ‚Ì‚Æ‚«‚Í “ú•t(d1)‚Í“¯‚¶ */
	const int compare = ZnkDate_compareDay( date1, date2 );
	/***
	 * compare == 1 ‚Ì‚Æ‚«
	 *   ‚Ü‚¸date1‚©‚çdate2‚ğˆø‚­ŒvZ‚ğ‚µA‚»‚ê‚É  compare*delta_day*24*3600 ‚ğ‰Á‚¦‚é.
	 * compare == -1‚Ì‚Æ‚«
	 *   ‚Ü‚¸date2‚©‚çdate1‚ğˆø‚­ŒvZ‚ğ‚µA‚»‚ê‚É -compare*delta_day*24*3600 ‚ğ‰Á‚¦‚é.
	 *   ‚³‚ç‚É‚»‚ÌŒ‹‰Ê‚Ì®‚Ì•„†‚ğ‹t“]‚³‚¹‚ê‚ÎAŒ‹‹Ç compare == 1 ‚Ì®‚Æˆê’v‚·‚é.
	 * compare == 0 ‚Ì‚Æ‚«
	 *   ’P‚Édate1‚©‚çdate2‚ğˆø‚­ŒvZ‚ğ‚·‚ê‚Î‚æ‚¢.
	 *   ‚±‚Ì‚Æ‚«–¾‚ç‚©‚É compare == 1 ‚Ì®‚Æˆê’v‚·‚é.
	 *
	 * Œ‹‹Ç‚Ç‚Ì‚æ‚¤‚Èê‡‚Å‚àˆÈ‰º‚Ì®(compare==1‚Ìê‡‚ÌŒ`®)‚É“ˆê‚³‚ê‚é‚±‚Æ‚ª‚í‚©‚é.
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
	 * ‚½‚¾‚µmonth=1, 2 ‚Ìê‡‚Í13, 14 ‚É“Ç‚İ‘Ö‚¦‚ÄŒvZ‚µ‚Ä‚ ‚é.
	 */
	static const unsigned int st_tbl13[] = {
		0,    /* dummy */
		0, 3, /* as month=13, month=14 */
		2, 5, 0, 3,
		5, 1, 4, 6, 2, 4,
	};
	/***
	 * 1, 2Œ‚Í‘O”N“x‚Ì13Œ, 14Œ‚ª—^‚¦‚ç‚ê‚½‚à‚Ì‚Æ‚İ‚È‚·.
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
			/* ˆê‚Â‘O‚ÌŒ‚Ìî•ñ‚ğæ“¾ */
			if( month > 1 ){
				--month;
			} else {
				--year;
				month = 12;
			}
			end_day = ZnkDate_getEndDayOfManth( year, month );

			/* remain_day ‚ªˆê‚Â‘O‚ÌŒ‚Ì“ú”ˆÈã‚È‚ç‚³‚ç‚ÉŒ‚ğ‘O‚Ö‚Æ’H‚é */
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
	/* —j“ú‚ÌXV */
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
			/* ˆê‚ÂŒã‚ÌŒ‚Ìî•ñ‚ğæ“¾ */
			if( month < 12 ){
				++month;
			} else {
				++year;
				month = 1;
			}
			end_day = ZnkDate_getEndDayOfManth( year, month );

			/* remain_day ‚ªˆê‚ÂŒã‚ÌŒ‚Ì“ú”‚æ‚è‘å‚«‚¢‚È‚ç‚³‚ç‚ÉŒ‚ğŒã‚Ö‚Æ’H‚é */
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
	/* —j“ú‚ÌXV */
	ZnkDate_updateWDay( ans );
}

