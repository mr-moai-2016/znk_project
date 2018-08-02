#include "Bdm_base.h"

#include <Rano_log.h>

#include <Znk_str_path.h>
#include <Znk_dir.h>
#include <Znk_stdc.h>
#include <Znk_missing_libc.h>

ZnkStr
BdmBase_getMoaiDir( void )
{
	const char dsp = '/';
	size_t depth = 5;
	ZnkStr moai_dir = ZnkStr_new( "../" );
	if( !ZnkStrPath_searchParentDir( moai_dir, depth, "target.myf", ZnkDirType_e_File, dsp ) ){
		ZnkStr_delete( moai_dir );
		return NULL;
	}
	return moai_dir;
}
bool
BdmBase_loadAuthenticKey( char* authentic_key_buf, size_t authentic_key_buf_size, const char* moai_dir )
{
	char path[ 256 ] = "";
	ZnkFile fp = NULL;
	Znk_snprintf( path, sizeof(path), "%sauthentic_key.dat", moai_dir );
	fp = Znk_fopen( path, "rb" );
	if( fp ){
		Znk_fgets( authentic_key_buf, authentic_key_buf_size, fp );
		Znk_fclose( fp );
		return true;
	}
	return false;
}
