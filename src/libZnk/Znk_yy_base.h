#ifndef INCLUDE_GUARD__Znk_yy_base_h__
#define INCLUDE_GUARD__Znk_yy_base_h__

#include <Znk_base.h>
#include <stdio.h>

Znk_EXTERN_C_BEGIN

/* flexが吐くコードにおいての旧式の関数スタイルを使わないようにする. */
#ifndef   YY_USE_PROTOS
#  define YY_USE_PROTOS
#endif

#if defined(_MSC_VER) || defined(__DMC__) || defined(__BORLANDC__)
#  ifndef   YY_NO_UNISTD_H
#    define YY_NO_UNISTD_H
#  endif
#endif

int yyparse( void );
int yylex( void );

/***
 * ZnkYYInfo_update_line_idxはLex側の字句「\n」検出時に呼び出しても
 * よいと思われるかもしれないが、これはよろしくない.
 * その理由は、yyerror関数の呼び出しがLex側の字句「\n」検出時の前後双方で
 * 発生する可能性があるからである.
 * この位置にZnkYYInfo_update_line_idxを置くことによって、
 * このタイミングをすべてのケースにおいて確実に「\n」検出時より後とすることができる.
 */
void   ZnkYYInfo_reset_line_idx( void );
void   ZnkYYInfo_update_line_idx( void );
size_t ZnkYYInfo_the_line_idx( void );
void   ZnkYYInfo_print_error( const char* msg, const char* near_text );

#define ZnkYYInfo_set_yyin( fp ) do { \
	extern FILE* yyin; \
	yyin = fp; \
} while( 0 )

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
