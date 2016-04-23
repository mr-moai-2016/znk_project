#include <Znk_yy_base.h>
#include <stdio.h>

static size_t st_line_idx = 1;

void ZnkYYInfo_reset_line_idx( void )
{
	st_line_idx = 1;
}
void ZnkYYInfo_update_line_idx( void )
{
	++st_line_idx;
}
size_t ZnkYYInfo_the_line_idx( void )
{
	return st_line_idx;
}
void ZnkYYInfo_print_error( const char* msg, const char* near_text )
{
	fprintf( stderr, "Error: [%s] : at line %u near[%s]\n",
		msg, (unsigned int)ZnkYYInfo_the_line_idx(), near_text );
}

