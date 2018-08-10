#include <Doc_html.h>
#include <Doc_source.h>
#include <Znk_s_base.h>
#include <Znk_stdc.h>

#include <stdio.h>


int main( int argc, char** argv )
{
	const char* command = "convert";
	const char* in_dir  = "doc_in";
	const char* out_dir = "doc_out";

	if( argc >= 2 ){
		command = argv[ 1 ];
	}
	if( argc >= 3 ){
		in_dir = argv[ 2 ];
	}
	if( argc >= 4 ){
		out_dir = argv[ 3 ];
	}

	if( ZnkS_eq( command, "html_doc" ) ){
		DocHtml_make( in_dir, out_dir );

	} else if( ZnkS_eq( command, "source_doc" ) ){
		DocSource_make( in_dir, out_dir );

	}

	if( DocHtml_isPauseOnExit() ){
		Znk_getchar();
	}

	return 0;
}
