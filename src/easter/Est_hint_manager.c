#include <Est_hint_manager.h>
#include <Znk_varp_ary.h>
#include <Znk_myf.h>

static ZnkMyf     st_hint_myf   = NULL;
static ZnkVarpAry st_hint_table = NULL;

static void
generateJsMsgFunc( ZnkStr ans, const char* id, ZnkStrAry lines )
{
	const size_t size = lines ? ZnkStrAry_size( lines ) : 0;
	size_t idx;
	const char* line = NULL;
	ZnkStr_addf( ans, "st_func_tbls.%s = function(){\n", id );
	ZnkStr_add(  ans, "\tvar msg = \"\";\n" );

	for( idx=0; idx<size; ++idx ){
		line = ZnkStrAry_at_cstr( lines, idx );
		ZnkStr_add( ans, "\tmsg += \"" );
		if( line[ 0 ] == '\t' ){
			ZnkStr_add( ans, "&nbsp;&nbsp;&nbsp;&nbsp;" );
		}
		ZnkStr_add( ans, line );
		if(  ZnkS_isEnd( line, "<ol>" ) 
		  || ZnkS_isEnd( line, "</ol>" ) )
		{
			ZnkStr_add( ans, "\";\n" );
		} else {
			ZnkStr_add( ans, "<br>\";\n" );
		}
	}

	ZnkStr_add( ans, "\treturn msg;\n" );
	ZnkStr_add( ans, "}\n" );
}

bool
EstHint_initiate( void )
{
	bool result = false;
	if( st_hint_myf == NULL ){
		st_hint_myf = ZnkMyf_create();
	}
	result = ZnkMyf_load( st_hint_myf, "templates/hint.myf" );
	if( result ){
		if( st_hint_table == NULL ){
			st_hint_table = ZnkVarpAry_create( true );
		}
		{
			ZnkVarpAry   category_list = ZnkMyf_find_vars( st_hint_myf, "category_list" );
			const size_t cat_num       = ZnkVarpAry_size( category_list );
			size_t       cat_idx;
			size_t       hint_idx;
			size_t       hint_num;

			for( cat_idx=0; cat_idx<cat_num; ++cat_idx ){
				ZnkVarp      cat      = ZnkVarpAry_at( category_list, cat_idx );
				const char*  cat_name = ZnkVar_name_cstr( cat );
				ZnkVarp      cat_varp = ZnkVarpAry_findObj_byName( st_hint_table, cat_name, Znk_NPOS, false );
				ZnkStr       cat_str;
				ZnkStrAry    hint_sda = ZnkVar_str_ary( cat );

				hint_num = ZnkStrAry_size( hint_sda );
				if( cat_varp == NULL ){
					cat_varp = ZnkVarp_create( cat_name, "", 0, ZnkPrim_e_Str, NULL );
					ZnkVarpAry_push_bk( st_hint_table, cat_varp );
				}
				cat_str = ZnkVar_str( cat_varp );
				ZnkStr_clear( cat_str );

				for( hint_idx=0; hint_idx<hint_num; ++hint_idx ){
					const char* hint_name  = ZnkStrAry_at_cstr( hint_sda, hint_idx );
					ZnkStrAry   hint_lines = ZnkMyf_find_lines( st_hint_myf, hint_name );
					generateJsMsgFunc( cat_str, hint_name, hint_lines );
				}
			}
		}
	}
	return result;
}

ZnkStr
EstHint_getHintTable( const char* cat_name )
{
	ZnkVarp varp = NULL;
	if( st_hint_table == NULL ){
		EstHint_initiate();
		if( st_hint_table == NULL ){
			return NULL;
		}
	}
	varp = ZnkVarpAry_findObj_byName( st_hint_table, cat_name, Znk_NPOS, false );
	if( varp ){
		return ZnkVar_str( varp );
	}
	return NULL;
}

