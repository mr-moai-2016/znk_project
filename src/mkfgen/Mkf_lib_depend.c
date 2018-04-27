#include "Mkf_lib_depend.h"
#include <Znk_str.h>
#include <Znk_str_ary.h>
#include <Znk_str_ex.h>
#include <string.h>

bool
MkfLibDepend_parseDependencyLib( const char* dependency_lib,
		ZnkStr dep_lib_type,
		ZnkStr lib_name,
		ZnkStr lib_dlver,
		ZnkStr lib_dir,
		bool* is_at_notation )
{
	const char* p = dependency_lib;
	char* q;

	*is_at_notation = false;
	while( *p == ' ' ){ ++p; }

	q = strchr( p, ':' );
	if( q ){
		ZnkStr_assign( dep_lib_type, 0, p, q-p );
	} else {
		/* syntax error */
		return false;
	}

	p = strchr( q, ' ' );
	if( p ){
		while( *p == ' ' ){ ++p; }
		if( *p == '@' ){
			++p;
			if( *p == '{' ){
				*is_at_notation = true;
				++p;
				q = strchr( p, '}' );
				if( q ){
					ZnkStr_assign( lib_dir, 0, p, q-p );
				}
				p = q;
			} else {
				--p;
			}
		}
		if( !*is_at_notation ){
			q = strchr( p, ' ' );
			if( q ){
				ZnkStr_assign( lib_dir, 0, p, q-p );
			} else {
				ZnkStr_set( lib_dir, p );
			}
		}
	}

	p = strchr( p, ' ' );
	if( p ){
		while( *p == ' ' ){ ++p; }
		q = strchr( p, ' ' );
		if( q ){
			ZnkStr_assign( lib_name, 0, p, q-p );
		}
		p = q;

		while( *p == ' ' ){ ++p; }
		ZnkStr_set( lib_dlver, p );
	}
	return true;
}

void
MkfLibDepend_parseSubLib( const char* sublib,
		ZnkStr submkf_dir,
		ZnkStr sublib_name )
{
	ZnkStrAry tkns = ZnkStrAry_create( true );
	ZnkStrEx_addSplitCSet( tkns,
			sublib, strlen(sublib),
			" \t", 2,
			2 );
	ZnkStr_set( submkf_dir,  ZnkStrAry_at_cstr(tkns,0) );
	ZnkStr_set( sublib_name, ZnkStrAry_at_cstr(tkns,1) );
	ZnkStrAry_destroy( tkns );
}
