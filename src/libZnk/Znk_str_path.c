#include <Znk_str_path.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <Znk_str_ary.h>
#include <Znk_str_ex.h>
#include <ctype.h>

Znk_INLINE bool
isASCII( uint32_t c ) { return (bool)( c < 0x80 ); }
Znk_INLINE bool
SJIS_isHankakuKatakana( uint8_t b ){ return ( b >= 0xa1 && b <= 0xdf ); }
Znk_INLINE bool
SJIS_isSecondByte( const char* base, const char* p )
{
	int lbc = 0;
	while( p > base ){
		--p;
		if( isASCII(*p) ){
			/* second or ascii */
			break;
		} else if( SJIS_isHankakuKatakana(*p) ){
			/* second or hankaku_katakana */
			break;
		} else {
			/* second or first */
		}
		++lbc;
	}
	return (bool)(lbc & 1);
}

void
ZnkStrPath_replaceDSP( ZnkStr path, char dsp )
{
	char* p = Znk_force_ptr_cast( char*, ZnkStr_cstr( path ) );
	const char* base = p;

	/* Skip UNC-Path root. */
	if( *p == '\\' ){
		if( *(p+1) == '\\' ){
			p += 2;
			base = p;
		}
	}
	while( *p ){
		if( *p == '/' || *p == '\\' ){
			if( !SJIS_isSecondByte( base, p ) ){
				*p = dsp;
				base = p+1;
			}
		}
		++p;
	}
}

size_t
ZnkStrPath_getTailPos( const char* path )
{
	const char* p = path;
	const char* base = p;
	size_t ans = 0;

	/* Skip UNC-Path root. */
	if( *p == '\\' ){
		if( *(p+1) == '\\' ){
			p += 2;
			base = p;
			ans = 2;
		}
	}
	while( *p ){
		if( *p == '/' || *p == '\\' ){
			if( !SJIS_isSecondByte( base, p ) ){
				ans = p - path + 1;
				base = p+1;
			}
		}
		++p;
	}
	return ans;
}

void
ZnkStrPath_addPathEnvVar( ZnkStr path_envvar, char psp, const char* new_path )
{
	size_t idx;
	ZnkStrAry list = ZnkStrAry_create( true );

	ZnkStrEx_addSplitC( list, ZnkStr_cstr(path_envvar), ZnkStr_leng(path_envvar),
			psp, false, 4 );
	idx = ZnkStrAry_find( list, 0, new_path, Znk_NPOS );
	if( idx == Znk_NPOS ){
		char connector[ 2 ] = "";
		connector[ 0 ] = psp;
		ZnkStrAry_push_bk_cstr( list, new_path, Znk_NPOS );
		ZnkStr_clear(path_envvar);
		ZnkStrEx_addJoin( path_envvar, list, 0, Znk_NPOS, connector, 1, 64 );
	}

	ZnkStrAry_destroy( list );
}

bool
ZnkStrPath_convertCygDrive_toDosDrive( ZnkStr path )
{
	if( ZnkStr_isBegin( path, "/cygdrive/" ) ){
		const char c1 = ZnkStr_at( path, 10 );
		if( isalpha( c1 ) ){
			const char c2 = ZnkStr_at( path, 11 );
			if( c2 == '/' || c2 == '\0' ){
				char dos_drive[ 4 ];
				dos_drive[ 0 ] = c1;
				dos_drive[ 1 ] = ':';
				dos_drive[ 2 ] = '\0';
				ZnkStr_replace( path, 0, 11, dos_drive, 2 );
				return true;
			}
		}
	}
	return false;
}

bool
ZnkStrPath_searchParentDir( ZnkStr ans_dir, size_t depth, const char* landmark, ZnkDirType landmark_type, char dsp )
{
	bool found = false;
	char path_target[ 256 ]  = "";
	while( depth ){
		Znk_snprintf( path_target, sizeof(path_target), "%s%s", ZnkStr_cstr(ans_dir), landmark );
		if( ZnkDir_getType( path_target ) == landmark_type ){
			found = true;
			break;
		}
		ZnkStr_add( ans_dir, "../" );
		--depth;
	}
	ZnkStrPath_replaceDSP( ans_dir, dsp );
	return found;
}

