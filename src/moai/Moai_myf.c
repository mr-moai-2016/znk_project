#include "Moai_myf.h"
#include "Moai_log.h"

static ZnkMyf st_config   = NULL;
static ZnkMyf st_target   = NULL;
static ZnkMyf st_analysis = NULL;

ZnkMyf
MoaiMyf_theConfig( void )
{
	if( st_config == NULL ){
		st_config = ZnkMyf_create();
	}
	return st_config;
}
ZnkMyf
MoaiMyf_theTarget( void )
{
	if( st_target == NULL ){
		st_target = ZnkMyf_create();
	}
	return st_target;
}
ZnkMyf
MoaiMyf_theAnalysis( void )
{
	if( st_analysis == NULL ){
		st_analysis = ZnkMyf_create();
	}
	return st_analysis;
}

bool
MoaiMyf_loadConfig( void )
{
	bool result = false;
	ZnkMyf config = MoaiMyf_theConfig();

	result = ZnkMyf_load( config, "config.myf" );
	if( result ){
		MoaiLog_printf( "Moai : config load OK.\n" );
	} else {
		MoaiLog_printf( "Moai : config load NG.\n" );
	}
	return result;
}

bool
MoaiMyf_loadTarget( void )
{
	bool result = false;
	ZnkMyf target = MoaiMyf_theTarget();

	result = ZnkMyf_load( target, "target.myf" );
	if( result ){
		MoaiLog_printf( "Moai : target load OK.\n" );
	} else {
		MoaiLog_printf( "Moai : target load NG.\n" );
	}
	return result;
}

bool
MoaiMyf_loadAnalysis( void )
{
	bool result = false;
	ZnkMyf analysis = MoaiMyf_theAnalysis();

	result = ZnkMyf_load( analysis, "analysis.myf" );
	if( result ){
		MoaiLog_printf( "Moai : analysis load OK.\n" );
	} else {
		MoaiLog_printf( "Moai : analysis load NG.\n" );
	}
	return result;
}
