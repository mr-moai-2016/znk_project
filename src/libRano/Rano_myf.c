#include "Rano_myf.h"
#include "Rano_log.h"

static ZnkMyf st_config   = NULL;
static ZnkMyf st_hosts    = NULL;
static ZnkMyf st_target   = NULL;
static ZnkMyf st_analysis = NULL;

ZnkMyf
RanoMyf_theConfig( void )
{
	if( st_config == NULL ){
		st_config = ZnkMyf_create();
	}
	return st_config;
}
ZnkMyf
RanoMyf_theHosts( void )
{
	if( st_hosts == NULL ){
		st_hosts = ZnkMyf_create();
	}
	return st_hosts;
}
ZnkMyf
RanoMyf_theTarget( void )
{
	if( st_target == NULL ){
		st_target = ZnkMyf_create();
	}
	return st_target;
}
ZnkMyf
RanoMyf_theAnalysis( void )
{
	if( st_analysis == NULL ){
		st_analysis = ZnkMyf_create();
	}
	return st_analysis;
}

bool
RanoMyf_loadConfig( void )
{
	bool result = false;
	ZnkMyf config = RanoMyf_theConfig();

	result = ZnkMyf_load( config, "config.myf" );
	if( result ){
		RanoLog_printf2( "Rano : config load OK.\n" );
	} else {
		RanoLog_printf2( "Rano : config load NG.\n" );
	}
	return result;
}

bool
RanoMyf_loadHosts( void )
{
	bool result = false;
	ZnkMyf hosts = RanoMyf_theHosts();

	result = ZnkMyf_load( hosts, "hosts.myf" );
	if( result ){
		RanoLog_printf2( "Rano : hosts load OK.\n" );
	} else {
		RanoLog_printf2( "Rano : hosts load NG.\n" );
	}
	return result;
}

bool
RanoMyf_loadTarget( void )
{
	bool result = false;
	ZnkMyf target = RanoMyf_theTarget();

	result = ZnkMyf_load( target, "target.myf" );
	if( result ){
		RanoLog_printf2( "Rano : target load OK.\n" );
	} else {
		RanoLog_printf2( "Rano : target load NG.\n" );
	}
	return result;
}

bool
RanoMyf_loadAnalysis( void )
{
	bool result = false;
	ZnkMyf analysis = RanoMyf_theAnalysis();

	result = ZnkMyf_load( analysis, "analysis.myf" );
	if( result ){
		RanoLog_printf2( "Rano : analysis load OK.\n" );
	} else {
		RanoLog_printf2( "Rano : analysis load SKIP.\n" );
	}
	return result;
}

