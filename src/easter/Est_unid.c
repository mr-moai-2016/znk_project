#include <Znk_date.h>
#include <Znk_missing_libc.h>
#include <Znk_s_base.h>

static char st_session_code[ 256 ] = "0";

void
EstUNID_setSessionCode_byNumber( size_t number )
{
	Znk_snprintf( st_session_code, sizeof(st_session_code), "%zu", number );
}
void
EstUNID_setSessionCode_byStr( const char* str )
{
	ZnkS_copy( st_session_code, sizeof(st_session_code), str, Znk_NPOS );
}
const char*
EstUNID_getSessionCode( void )
{
	return st_session_code;
}

char*
EstUNID_issue( char* buf, size_t buf_size )
{
	static ZnkDate st_old_date = { 0 };
	static size_t  st_count = 0;
	ZnkDate date = { 0 };
	int  comp_day = 0;
	/* date‚ªst_old_date‚Æ“¯‚¶‚©‰ß‹‚Ì’l‚Æ‚µ‚Äæ“¾‚³‚ê‚½ê‡true‚Æ‚·‚é. */
	bool is_past = false;

	ZnkDate_getCurrent( &date );
	comp_day = ZnkDate_compareDay( &date, &st_old_date );
	if( comp_day == 0 ){
		/* “¯“ú */
		if( date.d2_ <= st_old_date.d2_ ){
			is_past = true;
		}
	} else if( comp_day < 0 ){
		is_past = true;
	}

	if( is_past ){
		/* tid‚ªst_old_tid‚Æ“¯‚¶‚Æ‚µ‚Äæ“¾‚³‚ê‚½ê‡‚Ì“Á•Ê‘[’u */
		date = st_old_date;
		++st_count;
	} else {
		st_old_date = date;
		st_count = 0;
	}

	Znk_snprintf( buf, buf_size,
			"%d%02d%02d_%02d%02d%02d_%s_%zu",
				(int)ZnkDate_year( &date ),
				(int)ZnkDate_month( &date ),
				(int)ZnkDate_day( &date ),
				(int)ZnkDate_hour( &date ),
				(int)ZnkDate_minute( &date ),
				(int)ZnkDate_second( &date ),
				st_session_code,
				st_count );
	return buf;
}

