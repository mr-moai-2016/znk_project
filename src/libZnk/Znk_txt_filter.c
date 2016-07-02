#include <Znk_txt_filter.h>
#include <Znk_missing_libc.h>
#include <Znk_s_base.h>
#include <Znk_obj_ary.h>
#include <Znk_str_ptn.h>
#include <string.h>

static size_t
replaceStr( ZnkStr str, size_t begin,
		const char* old_ptn, size_t old_ptn_leng,
		const char* new_ptn, size_t new_ptn_leng,
		size_t seek_depth )
{
	return ZnkStrPtn_replace( str, begin,
			(uint8_t*)old_ptn, old_ptn_leng,
			(uint8_t*)new_ptn, new_ptn_leng,
			seek_depth );
}

struct ZnkTxtFilterAryImpl {
	ZnkObjAry obj_ary_;
};

static ZnkTxtFilter*
makeZnkTxtFilter( ZnkTxtFilterType type )
{
	ZnkTxtFilter* fltr = (ZnkTxtFilter*)Znk_malloc( sizeof( struct ZnkTxtFilter_tag ) );
	fltr->type_ = type;
	fltr->old_ptn_ = ZnkStr_new( "" );
	fltr->new_ptn_ = ZnkStr_new( "" );
	return fltr;
}
static void
deleteZnkTxtFilter( void* elem )
{
	ZnkTxtFilter* fltr = (ZnkTxtFilter*)elem;
	if( fltr ){
		ZnkStr_delete( fltr->old_ptn_ );
		ZnkStr_delete( fltr->new_ptn_ );
		Znk_free( fltr );
	}
}

ZnkTxtFilterAry
ZnkTxtFilterAry_create( void )
{
	ZnkTxtFilterAry fltr_ary = (ZnkTxtFilterAry)Znk_malloc( sizeof( struct ZnkTxtFilterAryImpl ) );
	fltr_ary->obj_ary_ = ZnkObjAry_create( deleteZnkTxtFilter );
	return fltr_ary;
}

void
ZnkTxtFilterAry_destroy( ZnkTxtFilterAry fltr_ary )
{
	if( fltr_ary ){
		ZnkObjAry_destroy( fltr_ary->obj_ary_ );
		Znk_free( fltr_ary );
	}
}

ZnkTxtFilter*
ZnkTxtFilterAry_regist_byCommand( ZnkTxtFilterAry fltr_ary,
		const char* command, const char* quote_begin, const char* quote_end )
{
	/* command ‚Í type ['old_ptn'] ['new_ptn'] ‚Æ‚¢‚¤Œ`Ž®‚ð‚Æ‚é */
	const char* p = command;
	const char* q = NULL;
	const size_t quote_begin_leng = strlen( quote_begin );
	const size_t quote_end_leng   = strlen( quote_end );
	ZnkTxtFilter* fltr = NULL;

	while( *p == ' ' || *p == '\t' ) ++p; /* skip whitespace */

	p = strstr( p, "replace" );
	if( p ){
		p += 7;
		fltr = makeZnkTxtFilter( ZnkTxtFilter_e_Replace );
	
		/***
		 * ‚Æ‚è‚ ‚¦‚¸Œ»Žž“_‚Å‚Íquote‚ð•K{‚Æ‚·‚é.
		 */
		p = strstr( p, quote_begin );
		if( p == NULL ){ goto FUNC_ERROR; }
		p += quote_begin_leng;
	
		q = strstr( p, quote_end );
		if( p == NULL ){ goto FUNC_ERROR; }
		ZnkStr_assign( fltr->old_ptn_, 0, p, q-p );
		p = q + quote_end_leng;
	
		p = strstr( p, quote_begin );
		if( p == NULL ){ goto FUNC_ERROR; }
		p += quote_begin_leng;
	
		q = strstr( p, quote_end );
		if( p == NULL ){ goto FUNC_ERROR; }
		ZnkStr_assign( fltr->new_ptn_, 0, p, q-p );
		p = q + quote_end_leng;
		ZnkObjAry_push_bk( fltr_ary->obj_ary_, (ZnkObj)fltr );
		return fltr;
	}
	p = strstr( p, "replace_between" );
	if( p ){
		return fltr;
	}

FUNC_ERROR:
	deleteZnkTxtFilter( fltr );
	return NULL;
}

ZnkTxtFilter*
ZnkTxtFilterAry_at( const ZnkTxtFilterAry fltr_ary, size_t idx )
{
	ZnkTxtFilter* fltr = (ZnkTxtFilter*)ZnkObjAry_at( fltr_ary->obj_ary_, idx );
	return fltr;
}
size_t
ZnkTxtFilterAry_size( const ZnkTxtFilterAry fltr_ary )
{
	return ZnkObjAry_size( fltr_ary->obj_ary_ );
}

void
ZnkTxtFilterAry_exec( const ZnkTxtFilterAry fltr_ary, ZnkStr text )
{
	const char* old_ptn = NULL;
	const char* new_ptn = NULL;
	const size_t size = ZnkTxtFilterAry_size( fltr_ary );
	size_t idx;
	ZnkTxtFilter* fltr = NULL;
	for( idx=0; idx<size; ++idx ){
		fltr = ZnkTxtFilterAry_at( fltr_ary, idx );
		old_ptn = ZnkStr_cstr( fltr->old_ptn_ );
		new_ptn = ZnkStr_cstr( fltr->new_ptn_ );
		replaceStr( text, 0,
				old_ptn, Znk_NPOS,
				new_ptn, Znk_NPOS,
				Znk_NPOS );
	}
}

