#include "Mkf_include.h"

#include <Znk_str.h>
#include <Znk_bird.h>
#include <string.h>

static const char* 
parseIncludeAtNotation( const char* include_path, const char** at_end )
{
	const char* p = include_path;
	char* q;

	while( *p == ' ' ){ ++p; }
	if( *p == '@' ){
		++p;
		if( *p == '{' ){
			++p;
			q = strchr( p, '}' );
			if( q ){
				*at_end = q;
				return p;
			}
		}
	}
	return NULL;
}
void
MkfInclude_getIncludeFlag( ZnkStr include_flags, const char* include_path, ZnkStr template_include_path, const char* nl )
{
	const char* at_end   = NULL;
	const char* at_begin = NULL;

	ZnkStr_add( include_flags, "\t" );
	at_begin = parseIncludeAtNotation( include_path, &at_end );
	if( at_begin ){
		ZnkBird bird = ZnkBird_create( "$[", "]$" );
		ZnkStr lib_dir   = ZnkStr_new( "" );
	
		ZnkStr_assign( lib_dir, 0, at_begin, at_end-at_begin );
		ZnkBird_regist( bird, "lib_dir",  ZnkStr_cstr( lib_dir ) );
		ZnkBird_extend( bird, include_flags, ZnkStr_cstr(template_include_path), ZnkStr_leng( template_include_path ) * 2 );
	
		ZnkStr_delete( lib_dir );
		ZnkBird_destroy( bird );
	} else {
		ZnkStr_addf2( include_flags, "-I%s \\", include_path );
	}
	ZnkStr_add( include_flags, nl );
}
