#include <pat_diff.h>
#include <pat_make.h>
#include <Znk_myf.h>
#include <Znk_s_base.h>
#include <Znk_stdc.h>

int main( int argc, char** argv )
{
	ZnkStr ermsg = ZnkStr_new( "" );
	ZnkMyf config = ZnkMyf_create();
	const char* cmd = NULL;

	ZnkMyf_load( config, "patgen.myf" );

	if( argc < 2 ){
		Znk_printf_e( "Usage : patgen [diff|make|marge] options...\n" );
		Znk_printf_e( "  patgen diff dir1 dir2\n" );
		Znk_printf_e( "  patgen make pat_diff_result common_dir platform_dir\n" );
		Znk_printf_e( "  patgen marge old_pmk new_pmk result_pmk\n" );
		goto FUNC_END;
	}
	cmd = argv[ 1 ];

	if( ZnkS_eq( cmd, "diff" ) ){
		const char* dir1  = NULL;
		const char* dir2  = NULL;
		if( argc < 4 ){
			Znk_printf_e( "Usage : patgen diff dir1 dir2\n" );
			goto FUNC_END;
		}
		dir1 = argv[ 2 ];
		dir2 = argv[ 3 ];
		Pat_diff( dir1, dir2,
				"pat_diff", config, ermsg,
				NULL, NULL );
	} else if( ZnkS_eq( cmd, "make" ) ){
		const char* pmk_file = NULL;
		const char* common_dir   = NULL;
		const char* platform_dir = NULL;
		const char* platform_id  = NULL;
		if( argc < 6 ){
			Znk_printf_e( "Usage : patgen make pat_diff_result common_dir platform_dir platform_id\n" );
			goto FUNC_END;
		}
		pmk_file     = argv[ 2 ];
		common_dir   = argv[ 3 ];
		platform_dir = argv[ 4 ];
		platform_id  = argv[ 5 ];
		Pat_make( pmk_file, common_dir, platform_dir, platform_id,
				config, ermsg );

	} else if( ZnkS_eq( cmd, "marge" ) ){
		const char* old_file = NULL;
		const char* new_file = NULL;
		const char* save_file = NULL;
		if( argc < 5 ){
			Znk_printf_e( "Usage : patgen marge old_pmk new_pmk result_pmk\n" );
			goto FUNC_END;
		}
		old_file  = argv[ 2 ];
		new_file  = argv[ 3 ];
		save_file = argv[ 4 ];
		Pat_marge( old_file, new_file, save_file );
	}
	Znk_printf_e( "%s", ZnkStr_cstr(ermsg) );

FUNC_END:
	ZnkMyf_destroy( config );
	ZnkStr_delete( ermsg );

	return 0;
}
