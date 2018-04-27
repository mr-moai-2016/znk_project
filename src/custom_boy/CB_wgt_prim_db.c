#include <CB_wgt_prim_db.h>

#include <Rano_log.h>

#include "Znk_duple_ary.h"
#include <Znk_obj_ary.h>
#include <Znk_str.h>
#include <Znk_myf.h>
#include <Znk_bird.h>
#include <Znk_missing_libc.h>

#include <stdlib.h>
#include <time.h>
#include <string.h>

static const void*
copy_val( const void* val ) {
	return val;
}
static void
delete_val( void* val ) {
	CBWgtPrimAry_destroy( (CBWgtPrimAry)val );
}
const ZnkDupleValFuncs*
getValFuncs( void )
{
	static ZnkDupleValFuncs st_val_funcs = {
		copy_val,
		delete_val,
	};
	return &st_val_funcs;
}

CBWgtPrimDB
CBWgtPrimDB_create( void )
{
	ZnkDupleAry wpdb = ZnkDupleAry_create( ZnkDupleKeyFuncs_get_strdup(), getValFuncs() );
	return (CBWgtPrimDB)wpdb;
}

void
CBWgtPrimDB_destroy( CBWgtPrimDB wpdb )
{
	ZnkDupleAry_destroy( (ZnkDupleAry)wpdb );
}

CBWgtPrimAry
CBWgtPrimDB_regist( CBWgtPrimDB wpdb, const char* key )
{
	CBWgtPrimAry ary = CBWgtPrimAry_create( true );
	ZnkDupleAry_regist( (ZnkDupleAry)wpdb, key, ary, true, NULL );
	return ary;
}

size_t
CBWgtPrimDB_size( const CBWgtPrimDB wpdb )
{
	return ZnkDupleAry_size( (ZnkDupleAry)wpdb );
}

CBWgtPrimAry
CBWgtPrimDB_atAry( CBWgtPrimDB wpdb, size_t idx )
{
	ZnkDuple dup = ZnkDupleAry_atElem( (ZnkDupleAry)wpdb, idx );
	if( dup ){
		CBWgtPrimAry ary = (CBWgtPrimAry)ZnkDuple_val( dup );
		return ary;
	}
	return NULL;
}

ZnkDuple
CBWgtPrimDB_atDuple( CBWgtPrimDB wpdb, size_t idx )
{
	return ZnkDupleAry_atElem( (ZnkDupleAry)wpdb, idx );
}

CBWgtPrimAry
CBWgtPrimDB_findAry( CBWgtPrimDB wpdb, const char* key )
{
	ZnkDuple dup = ZnkDupleAry_findElem( (ZnkDupleAry)wpdb, key );
	if( dup ){
		CBWgtPrimAry ary = (CBWgtPrimAry)ZnkDuple_val( dup );
		return ary;
	}
	return NULL;
}

static void
parseLine( ZnkStr line, ZnkStr val, double* rate, const char* quote_begin, const char* quote_end )
{
	const char* p = ZnkStr_cstr( line );
	const char* q = strstr( p, quote_begin );
	const size_t quote_begin_leng = Znk_strlen( quote_begin );
	const size_t quote_end_leng   = Znk_strlen( quote_end );
	ZnkStr_clear( val );
	if( q ){
		q += quote_begin_leng;
		p = strstr( q, quote_end );
		if( p ){
			ZnkStr_assign( val, 0, q, p-q );
			p += quote_end_leng;
			q = strchr( p, '*' );
			if( q ){
				++q;
				ZnkS_getReal( rate, q );
			}
		}
	}
}

bool
CBWgtPrimDB_load( CBWgtPrimDB wpdb, const char* conffile )
{
	bool result = false;
	ZnkMyf myf = ZnkMyf_create();
	if( ZnkMyf_load( myf, conffile ) ){
		const size_t num_of_sec = ZnkMyf_numOfSection( myf );
		size_t sec_idx;
		ZnkMyfSection sec;
		const char* sec_name;
		CBWgtPrimAry wpary;
		ZnkStrAry lines;
		size_t idx;
		ZnkStr line;
		size_t line_num;
		double rate = -1.0;
		ZnkStr val = ZnkStr_new( "" );
		for( sec_idx=0; sec_idx<num_of_sec; ++sec_idx ){
			sec = ZnkMyf_atSection( myf, sec_idx );
			sec_name = ZnkMyfSection_name( sec );
			lines = ZnkMyfSection_lines( sec );
			wpary = CBWgtPrimDB_regist( wpdb, sec_name );
			line_num = ZnkStrAry_size( lines );
			for( idx=0; idx<line_num; ++idx ){
				line = ZnkStrAry_at( lines, idx );
				rate = -1.0;
				parseLine( line, val, &rate, ZnkMyf_quote_begin(myf), ZnkMyf_quote_end(myf) );
				CBWgtPrimAry_registStr( wpary, ZnkStr_cstr(val), rate );
			}
		}
		ZnkStr_delete( val );
		result = true;
	}
	ZnkMyf_destroy( myf );
	return result;
}


static bool
processFunc( const ZnkBird info,
		ZnkStr dst,
		const char* src,
		const char* key,
		size_t begin_idx, /* srcã‚É‚¨‚¯‚ékey‚ÌŠJŽnˆÊ’u‚ðhint‚Æ‚µ‚Ä—^‚¦‚é */
		void* func_arg )
{
	/***
	 * ˆê‚Â‚Ìkey‚É‚Â‚«ˆê“x‚µ‚©select‚µ‚È‚¢‚æ‚¤‚É‚µ‚½‚¢.
	 * ‚½‚Æ‚¦‚Îversion”Ô†‚È‚Ç‚ð–ˆ‰ñƒ‰ƒ“ƒ_ƒ€‚É‚µ‚Ä‚Í‚Ü‚¸‚¢‚Ì‚Å.
	 * ˆê“xselect‚µ‚½‚à‚Ì‚ð•Ê“rZnkBird‚É“o˜^‚µ‚Ä‚¨‚­.
	 */
	if( ZnkBird_exist( info, key ) ){
		const char* val = ZnkBird_at( info, key );
		ZnkStr_add( dst, val );
	} else {
		ZnkStr key_str = ZnkStr_new( key );
		CBWgtPrimDB wpdb = Znk_force_ptr_cast( CBWgtPrimDB, func_arg );
		CBWgtPrimAry wpary = CBWgtPrimDB_findAry( wpdb, ZnkStr_cstr(key_str) );
		ZnkPrimp prim = CBWgtPrimAry_select( wpary );
		ZnkStr tmp = ZnkStr_new( "" );
		ZnkBird_extend( info, tmp, ZnkPrim_cstr(prim), 4 );
		ZnkStr_add( dst, ZnkStr_cstr(tmp) );
		ZnkBird_regist( info, ZnkStr_cstr(key_str), ZnkStr_cstr(tmp) );
		ZnkStr_delete( tmp );
		ZnkStr_delete( key_str );
	}
	return true;
}

bool
CBWgtPrimDB_exec( CBWgtPrimDB wpdb, ZnkBird bird, ZnkStr ans, const char* entry_key )
{
	ZnkBird_setFunc( bird, processFunc, wpdb );
	processFunc( bird, ans, NULL, entry_key, 0, wpdb );
	return true;
}
