#include "Doc_source.h"
#include <Rano_dir_util.h>
#include <Rano_log.h>
#include <Rano_cgi_util.h>

#include <Znk_myf.h>
#include <Znk_s_base.h>
#include <Znk_stdc.h>
#include <Znk_mbc_jp.h>
#include <Znk_bfr_bif.h>
#include <Znk_bif.h>
#include <Znk_str_ex.h>
#include <Znk_str_fio.h>
#include <Znk_dir.h>
#include <Znk_missing_libc.h>
#include <Znk_htp_util.h>
#include <Znk_vpod.h>

#include <stdio.h>
#include <ctype.h>

static void
drawMenuBar2( ZnkStr ans, const char* query_urp, ZnkPrimpAry tbl, const char* query_category )
{
	const size_t size = ZnkPrimpAry_size( tbl );
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		ZnkPrimp  prim = ZnkPrimpAry_at( tbl, idx );
		ZnkStrAry sary = ZnkPrim_strAry( prim );
		if( ZnkStrAry_size(sary) >= 2 ){
			const char* href     = ZnkStrAry_at_cstr( sary, 0 );
			const char* title    = ZnkStrAry_at_cstr( sary, 1 );
			const char* category = ( ZnkStrAry_size(sary) >= 3 ) ? ZnkStrAry_at_cstr( sary, 2 ) : NULL;
			bool selected = query_category ?
				( category ? ZnkS_eq( category, query_category ) : false )
				: (bool)( query_urp && ZnkS_eq( href, query_urp ) );
			const char* style = selected ? "MstyNowSelectedLink" : "MstyElemLink";
			ZnkStr_addf( ans, "<a class=%s href=\"%s\">%s</a> &nbsp;\n",
					style, href, title );
		}
	}
}

static void
drawHeader( ZnkStr ans, ZnkStrAry category_path, const char* urp, ZnkMyf menu_myf, const char* doc_title )
{
	ZnkStr_addf( ans, 
			"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n"
			"<html>\n"
			"<head>\n"
			"\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
			"\t<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n"
			"\t<meta http-equiv=\"Content-Style-Type\" content=\"text/css\">\n"
			"\t<link href=\"/msty.css\" rel=\"stylesheet\" type=\"text/css\">\n"
			"</head>\n"
	);

	ZnkStr_addf( ans,
			"<body>\n"
			"<a name=TopBar></a>\n"
			"<p><b><img src=\"/imgs/here_landmark.png\"> %s</b></p>\n\n",
			doc_title ? doc_title : ""
	);

	/* root */
	{
		ZnkPrimpAry root = ZnkMyf_find_prims( menu_myf, "root" );
		drawMenuBar2( ans, urp, root,
				( category_path && ZnkStrAry_size(category_path) > 0 ) ? ZnkStrAry_at_cstr(category_path,0) : NULL );
	}
	if( category_path ){
		const size_t category_path_size = ZnkStrAry_size(category_path);
		size_t category_path_idx;
		for( category_path_idx=0; category_path_idx<category_path_size; ++category_path_idx ){
			const char* category = ZnkStrAry_at_cstr( category_path, category_path_idx );
			ZnkPrimpAry cat_tbl = ZnkMyf_find_prims( menu_myf, category );
			if( cat_tbl ){
				ZnkStr_addf( ans, "<br> <br>\n" );
				drawMenuBar2( ans, urp, cat_tbl,
						ZnkStrAry_size( category_path ) > category_path_idx + 1 ?
						ZnkStrAry_at_cstr( category_path, category_path_idx + 1 ) : NULL );
			}
		}
	}
	ZnkStr_addf( ans, "<br> <br>\n\n" );
}
	
static bool
isPtnToken( const char* begin, const char* end, const char* ptn )
{
	const size_t ptn_leng = Znk_strlen( ptn );
	if( ptn_leng == (size_t)( end - begin ) && ZnkS_eqEx( begin, ptn, ptn_leng ) ){
		return true;
	}
	return false;
}

static bool
lexSharpLine( const char* begin )
{
	const char* p = begin;
	const char* q;

	while( *p == ' ' ){ ++p; }
	q = p;
	while( *q && *q != ' ' && *q != '<' ){ ++q; }

	if( isPtnToken( p, q, "ifndef" ) ){
		return false;
	}
	if( isPtnToken( p, q, "endif" ) ){
		return false;
	}
	if( isPtnToken( p, q, "define" ) ){
		p = q;
		while( *p == ' ' ){ ++p; }
		q = p;
		while( *q && *q != ' ' ){ ++q; }
		if( ZnkS_isBegin( p, "INCLUDE_GUARD_" ) ){
			return false;
		}
	}
	return true;
}
static bool
lexHeaderLine( const char* begin )
{
	const char* p = begin;
	const char* q = begin;

	while( isalnum( (unsigned char)*q ) || *q == '_' ){ ++q; }

	if( isPtnToken( p, q, "Znk_EXTERN_C_BEGIN" ) ){
		return false;
	}
	if( isPtnToken( p, q, "Znk_EXTERN_C_END" ) ){
		return false;
	}
	return true;
}

static void
convertSJIStoUTF8( ZnkStr ans )
{
	struct ZnkBif_tag bif_instance = { 0 };
	struct ZnkBif_tag tmp_instance = { 0 };
	ZnkErr_D( err );
	ZnkBif bif = &bif_instance;
	ZnkBif tmp = &tmp_instance;
	ZnkBfrBif_create( bif );
	ZnkBfrBif_create( tmp );
	ZnkBif_append( bif, (uint8_t*)ZnkStr_cstr(ans), ZnkStr_leng(ans) );
	ZnkMbc_convert_self( bif, ZnkMbcType_SJIS, ZnkMbcType_UTF8, tmp, &err );
	ZnkStr_assign( ans, 0, (char*)ZnkBif_data(bif), ZnkBif_size(bif) );
	ZnkBfrBif_destroy( bif );
	ZnkBfrBif_destroy( tmp );
}


typedef enum {
	 DcgTyp_e_Ignore=0
	,DcgTyp_e_Integer
	,DcgTyp_e_Real
	,DcgTyp_e_Preprocessor
	,DcgTyp_e_String
	,DcgTyp_e_Char
	,DcgTyp_e_Operator
	,DcgTyp_e_ParenthesisBegin
	,DcgTyp_e_ParenthesisEnd
	,DcgTyp_e_BreathBegin
	,DcgTyp_e_BreathEnd
	,DcgTyp_e_BracketBegin
	,DcgTyp_e_BracketEnd
	,DcgTyp_e_CommentBegin
	,DcgTyp_e_CommentEnd
	,DcgTyp_e_CommentBody
	,DcgTyp_e_CommentLine
	,DcgTyp_e_CommentLineBody
	,DcgTyp_e_Keyword
	,DcgTyp_e_Typename
	,DcgTyp_e_FuncDecl
	,DcgTyp_e_Identifier
	,DcgTyp_e_NL
	,DcgTyp_e_Tab
} DcgTyp;

static bool
registToken( ZnkBfr typs, ZnkStrAry tkns, DcgTyp typ, const char* begin, size_t size )
{
	ZnkVUInt32_push_bk( typs, (uint32_t)typ );
	ZnkStrAry_push_bk_cstr( tkns, begin, size );
	return true;
}
static DcgTyp
getCurrentTyp( ZnkBfr typs )
{
	const size_t size = ZnkVUInt32_size( typs );
	if( size ){
		uint32_t typ = ZnkVUInt32_at( typs, size-1 );
		return (DcgTyp)typ;
	}
	return DcgTyp_e_Ignore;
}
static ZnkStr
getCurrentTkn( ZnkStrAry tkns )
{
	const size_t size = ZnkStrAry_size( tkns );
	if( size ){
		return ZnkStrAry_at( tkns, size-1 );
	}
	return NULL;
}
static DcgTyp
atTyp( ZnkBfr typs, size_t idx )
{
	const size_t size = ZnkVUInt32_size( typs );
	if( size ){
		uint32_t typ = ZnkVUInt32_at( typs, idx );
		return (DcgTyp)typ;
	}
	return DcgTyp_e_Ignore;
}

static bool
isCKeyword( const char* p, size_t leng )
{
	static ZnkSRef st_table[] = {
		{ "if",       },
		{ "for",      },
		{ "while",    },
		{ "do",       },
		{ "switch",   },
		{ "case",     },
		{ "default",  },
		{ "break",    },
		{ "continue", },
		{ "return",   },
		{ "goto",     },
		{ "typedef",  },
		{ "struct",   },
		{ "union",   },
		{ "enum",     },
		{ "class",    },
	};

	if( st_table[ 0 ].leng_ == 0 ){
		size_t idx;
		for( idx=0; idx<Znk_NARY(st_table); ++idx ){
			st_table[ idx ].leng_ = Znk_strlen( st_table[ idx ].cstr_ );
		}
	}
	{
		size_t idx;
		for( idx=0; idx<Znk_NARY(st_table); ++idx ){
			if( leng == st_table[ idx ].leng_ ){
				if( ZnkS_eqEx( p, st_table[ idx ].cstr_, leng ) ){
					/* found */
					return true;
				}
			}
		}
	}

	return false;
}
static bool
isCTypename( const char* p, size_t leng )
{
	static ZnkSRef st_table[] = {
		{ "char",     },
		{ "short",    },
		{ "int",      },
		{ "long",     },
		{ "float",    },
		{ "double",   },
		{ "size_t",   },
		{ "bool",     },
		{ "signed",   },
		{ "unsigned", },
		{ "const",    },
		{ "volatile", },
		{ "register", },
		{ "void",     },
		{ "static",   },
		{ "Znk_INLINE", },
		{ "uint8_t",  },
		{ "uint16_t", },
		{ "uint32_t", },
		{ "uint64_t", },
		{ "uintptr_t", },
		{ "ZnkStr", },
	};

	if( st_table[ 0 ].leng_ == 0 ){
		size_t idx;
		for( idx=0; idx<Znk_NARY(st_table); ++idx ){
			st_table[ idx ].leng_ = Znk_strlen( st_table[ idx ].cstr_ );
		}
	}
	{
		size_t idx;
		for( idx=0; idx<Znk_NARY(st_table); ++idx ){
			if( leng == st_table[ idx ].leng_ ){
				if( ZnkS_eqEx( p, st_table[ idx ].cstr_, leng ) ){
					/* found */
					return true;
				}
			}
		}
	}

	return false;
}
static bool
isCConstant( const char* p, size_t leng )
{
	static ZnkSRef st_table[] = {
		{ "NULL",     },
		{ "true",     },
		{ "false",    },
		{ "Znk_NPOS", },
	};

	if( st_table[ 0 ].leng_ == 0 ){
		size_t idx;
		for( idx=0; idx<Znk_NARY(st_table); ++idx ){
			st_table[ idx ].leng_ = Znk_strlen( st_table[ idx ].cstr_ );
		}
	}
	{
		size_t idx;
		for( idx=0; idx<Znk_NARY(st_table); ++idx ){
			if( leng == st_table[ idx ].leng_ ){
				if( ZnkS_eqEx( p, st_table[ idx ].cstr_, leng ) ){
					/* found */
					return true;
				}
			}
		}
	}

	return false;
}

static bool
isHex( uint8_t ch )
{
	switch( ch ){
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		return true;
	default:
		break;
	}
	return false;
}

static bool
lexLineC( ZnkBfr typs, ZnkStrAry tkns, const char* p )
{
	while( *p ){
		const char* q = NULL;
		while( *p == ' ' ){ ++p; }

		switch( *p ){
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			/* Integer or Real */
			q = p;
			if( *q == '0' && ( *(q+1) == 'x' || *(q+1) == 'X' ) ){
				/* hex */
				q += 2;
				while( isHex( *q ) ){
					++q;
				}
				registToken( typs, tkns, DcgTyp_e_Integer, p, q-p );
			} else {
				/* decimal */
				++q;
				while( isdigit( (unsigned char)*q ) ){
					++q;
				}
				if( *q == '.' ){
					++q;
					while( isdigit( (unsigned char)*q ) ){
						++q;
					}
					registToken( typs, tkns, DcgTyp_e_Real, p, q-p );
				} else {
					registToken( typs, tkns, DcgTyp_e_Integer, p, q-p );
				}
			}
			break;
		case '.':
			/* Real */
			q = p;
			++q;
			while( isdigit( (unsigned char)*q ) ){
				++q;
			}
			registToken( typs, tkns, DcgTyp_e_Real, p, q-p );
			break;
		case '#':
			/* Preprocessor */
			q = p;
			++q;
			while( *q == ' ' ){ ++q; }
			while( isalnum( (unsigned char)*q ) ){
				++q;
			}
			registToken( typs, tkns, DcgTyp_e_Preprocessor, p, q-p );
			break;
		case '"':
			/* String */
			q = p;
			++q;
			while( *q && *q != '"' ){
				if( *q == '\\' ){
					if( *(q+1) == '\\' ){
						++q;
					}
				}
				++q;
			}
			++q;
			registToken( typs, tkns, DcgTyp_e_String, p, q-p );
			break;
		case '\'':
			/* Char */
			q = p;
			++q;
			while( *q && *q != '\'' ){
				if( *q == '\\' ){
					if( *(q+1) == '\\' ){
						++q;
					}
				}
				++q;
			}
			++q;
			registToken( typs, tkns, DcgTyp_e_Char, p, q-p );
			break;
		case '/':
			/* Divide or CommentBegin */
			q = p;
			if( *(q+1) == '*' ){
				bool is_end = false;
				q += 2;
				registToken( typs, tkns, DcgTyp_e_CommentBegin, p, q-p );

				p = q;
				while( *q ){
					if( *q == '*' ){
						if( *(q+1) == '/' ){
							registToken( typs, tkns, DcgTyp_e_CommentBody, p, q-p );
							registToken( typs, tkns, DcgTyp_e_CommentEnd, q, 2 );
							q += 2;
							is_end = true;
							break;
						}
					}
					++q;
				}
				if( !is_end ){
					registToken( typs, tkns, DcgTyp_e_CommentBody, p, Znk_NPOS );
					{
						ZnkStr current_tkn = getCurrentTkn(tkns);
						ZnkStr_add_c( current_tkn, '\n' );
					}
					return true; /* EOL */
				}
			} else if( *(q+1) == '/' ){
				/* CommentLine (//) */
				registToken( typs, tkns, DcgTyp_e_CommentLine, p, 2 );
				registToken( typs, tkns, DcgTyp_e_CommentLineBody, p+2, Znk_NPOS );
				return true; /* EOL */
			} else {
				/* Divide */
				q = p+1;
				registToken( typs, tkns, DcgTyp_e_Operator, p, q-p );
			}
			break;
		case '*':
			registToken( typs, tkns, DcgTyp_e_Operator, "*", 1 );
			q = p+1;
			break;
		case '%':
			registToken( typs, tkns, DcgTyp_e_Operator, "%", 1 );
			q = p+1;
			break;
		case '+':
			registToken( typs, tkns, DcgTyp_e_Operator, "+", 1 );
			q = p+1;
			break;
		case '-':
			registToken( typs, tkns, DcgTyp_e_Operator, "-", 1 );
			q = p+1;
			break;
		case ',':
			registToken( typs, tkns, DcgTyp_e_Operator, ",", 1 );
			q = p+1;
			break;
		case '=':
			registToken( typs, tkns, DcgTyp_e_Operator, "=", 1 );
			q = p+1;
			break;
		case '!':
			registToken( typs, tkns, DcgTyp_e_Operator, "!", 1 );
			q = p+1;
			break;
		case '\t':
			registToken( typs, tkns, DcgTyp_e_Tab, "\t", 1 );
			q = p+1;
			break;
		case '(':
			registToken( typs, tkns, DcgTyp_e_ParenthesisBegin, "(", 1 );
			q = p+1;
			break;
		case ')':
			registToken( typs, tkns, DcgTyp_e_ParenthesisEnd, ")", 1 );
			q = p+1;
			break;
		case '{':
			registToken( typs, tkns, DcgTyp_e_BreathBegin, "{", 1 );
			q = p+1;
			break;
		case '}':
			registToken( typs, tkns, DcgTyp_e_BreathEnd, "}", 1 );
			q = p+1;
			break;
		case '[':
			registToken( typs, tkns, DcgTyp_e_BracketBegin, "[", 1 );
			q = p+1;
			break;
		case ']':
			registToken( typs, tkns, DcgTyp_e_BracketEnd, "]", 1 );
			q = p+1;
			break;
		default:
			q = p;
			if( isalnum( (unsigned char)*q ) || *q == '_' ){
				/* Identifier */
				while( isalnum( (unsigned char)*q ) || *q == '_' ){
					++q;
				}
				if( isCKeyword( p, q-p ) ){
					registToken( typs, tkns, DcgTyp_e_Keyword, p, q-p );
				} else if( isCTypename( p, q-p ) ){
					registToken( typs, tkns, DcgTyp_e_Typename, p, q-p );
				} else if( isCConstant( p, q-p ) ){
					registToken( typs, tkns, DcgTyp_e_Integer, p, q-p );
				} else {
					bool is_func_decl = false;
					while( *q == ' ' ){ ++q; }
					if( *q == '(' ){
						DcgTyp prev_typ = getCurrentTyp( typs );
						if( prev_typ == DcgTyp_e_NL || prev_typ == DcgTyp_e_Typename ){
							is_func_decl = true;
						} else {
							if( prev_typ == DcgTyp_e_Operator ){
								ZnkStr tkn = getCurrentTkn( tkns );
								if( ZnkStr_eq( tkn, "*" ) ){
									size_t size = ZnkStrAry_size( tkns );
									if( size > 1 ){
										prev_typ = atTyp( typs, size-2 );
										if( prev_typ == DcgTyp_e_Typename ){
											is_func_decl = true;
										}
									}
								}
							}
						}
					}
					if( is_func_decl ){
						registToken( typs, tkns, DcgTyp_e_FuncDecl, p, q-p );
					} else {
						registToken( typs, tkns, DcgTyp_e_Identifier, p, q-p );
					}
				}
			} else {
				/* Ignore */
				while( *q && *q != ' ' ){
					++q;
				}
				registToken( typs, tkns, DcgTyp_e_Ignore, p, q-p );
			}
			break;
		}
		if( q == NULL ){
			/* Error */
			return false;
		}
		p = q;
	}
	return true;
}

static bool
lexNonCommentLine( ZnkBfr typs, ZnkStrAry tkns, const char* p )
{
	DcgTyp current_typ = DcgTyp_e_Ignore;
	lexLineC( typs, tkns, p );
	current_typ = getCurrentTyp( typs );
	if( current_typ != DcgTyp_e_CommentBody ){
		registToken( typs, tkns, DcgTyp_e_NL, "\n", 1 );
	}
	return true;
}

static bool
lexTokenCFile( ZnkBfr typs, ZnkStrAry tkns, ZnkStr line, ZnkFile fp )
{
	size_t empty_line_count = 0;
	while( ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
		DcgTyp current_typ = getCurrentTyp( typs );
		ZnkStr current_tkn = getCurrentTkn( tkns );

		if( current_typ == DcgTyp_e_CommentBody ){
			bool is_end = false;
			const char* p = ZnkStr_cstr(line);
			const char* q = p;
			empty_line_count = 0;
			while( *q ){
				if( *q == '*' && *(q+1) == '/' ){
					/* CommentEndの前まで追加 */
					if( current_tkn ){
						ZnkStr_append( current_tkn, p, q-p );
					}
					registToken( typs, tkns, DcgTyp_e_CommentEnd, q, 2 );
					q += 2;

					{
						size_t pos = q - p;
						ZnkStr_chompNL(line);
						if( !ZnkStr_empty(line) ){
							lexNonCommentLine( typs, tkns, ZnkStr_cstr(line) + pos );
						}
					}
					is_end = true;
					break;
				}
				++q;
			}
			if( !is_end ){
				/* この行はそのまま追加 */
				if( current_tkn ){
					ZnkStr_append( current_tkn, p, q-p );
				}
			}

		} else {
			ZnkStr_chompNL(line);
			if( ZnkStr_empty(line) ){
				if( empty_line_count == 0 ){
					registToken( typs, tkns, DcgTyp_e_NL, "\n", 1 );
				}
				++empty_line_count;
			} else {
				lexNonCommentLine( typs, tkns, ZnkStr_cstr(line) );
				empty_line_count = 0;
			}
		}
	}
	return true;
}

static bool
isCWordTyp( DcgTyp typ )
{
	switch( typ ){
	case DcgTyp_e_Typename:
	case DcgTyp_e_Keyword:
	case DcgTyp_e_FuncDecl:
	case DcgTyp_e_Identifier:
		return true;
	default:
		break;
	}
	return false;
}

static void
makeOne( ZnkStr ans, ZnkMyf menu_myf, const char* in_file_path, const char* out_file_path )
{
	ZnkMyf myf = ZnkMyf_create();
	ZnkStr_clear( ans );
	
	{
		ZnkStrAry   category_path      = ZnkStrAry_create( true );
		const char* urp                = "test.html";
		const char* doc_title          = "Title";
		drawHeader( ans, category_path, urp, menu_myf, doc_title );
		ZnkStrAry_destroy( category_path );
	}

	//ZnkStr_addf( ans, "in_file_path=[%s] out_file_path=[%s]<br>", in_file_path, out_file_path );
#if 0
	{
		ZnkFile fp = Znk_fopen( in_file_path, "rb" );
		if( fp ){
			ZnkStr line = ZnkStr_new( "" );
			bool is_in_comment = false;
			bool is_in_dummy_comment = false;

			while( ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				const char* p = ZnkStr_cstr(line);
				bool is_comment_begin = false;
				bool is_comment_end   = false;

				while( *p == ' ' ){ ++p; }
				if( *p == '#' ){
					++p;
					if( !lexSharpLine( p ) ){
						continue; /* skip */
					}
				} else if( *p == '/' && *(p+1) == '*' ){
					if( *(p+2) == '*' && *(p+3) != '*' ){
						is_comment_begin = true;
						is_in_comment = true;
					} else {
						is_in_dummy_comment = true;
						continue; /* skip */
					}
				} else if( *p == '*' && *(p+1) == '/' ){
					is_comment_end = true;
					is_in_comment = false;
					is_in_dummy_comment = false;
				} else if( is_in_dummy_comment ){
					continue; /* skip */
				} else if( is_in_comment ){
					/* none */
				} else {
					if( !lexHeaderLine( p ) ){
						continue; /* skip */
					}
				}

				ZnkHtpURL_negateHtmlTagEffection( line ); /* for XSS */
				convertSJIStoUTF8( line );

				if( is_comment_begin ){
					ZnkStr_add( ans, "<div class=MstyComment>" );
				} else if( is_comment_end ){
					ZnkStr_add( ans, "</div><br>" );
				} else if( is_in_comment ){
					if( *p == '*' ){
						++p;
					}
					ZnkStr_addf( ans, "%s<br>", p );
				} else {
					ZnkStr_addf( ans, "%s<br>", ZnkStr_cstr(line) );
				}
			}
			ZnkStr_delete( line );
			Znk_fclose( fp );
		}
	}
#endif
	{
		ZnkFile fp = Znk_fopen( in_file_path, "rb" );
		if( fp ){
			ZnkStr    line = ZnkStr_new( "" );
			ZnkBfr    typs = ZnkBfr_create_null();
			ZnkStrAry tkns = ZnkStrAry_create( true );

			lexTokenCFile( typs, tkns, line, fp );

			{
				size_t idx;
				size_t size = ZnkStrAry_size( tkns );
				DcgTyp prev_typ = DcgTyp_e_NL;
				for( idx=0; idx<size; ++idx ){
					ZnkStr tkn = ZnkStrAry_at( tkns, idx );
					DcgTyp typ = atTyp( typs, idx );
					if( typ == DcgTyp_e_NL ){
						ZnkStr_add( ans, "<br>\n\n" );
					} else if( typ == DcgTyp_e_CommentBegin ){
						//ZnkStr_addf( ans, "<span class=MstyCode><font color=#333355>%s", ZnkStr_cstr(tkn) );
					} else if( typ == DcgTyp_e_CommentEnd ){
						//ZnkStr_addf( ans, "%s</font></span>", ZnkStr_cstr(tkn) );
					} else {
						ZnkHtpURL_negateHtmlTagEffection( tkn ); /* for XSS */
						convertSJIStoUTF8( tkn );

						switch( typ ){
						case DcgTyp_e_CommentBody:
						{
							bool is_docgen_style = false;

							/* とりあえず無効 : 単純なハイライト処理という需要の方が今のところは大きいので */
#if 0
							if( ZnkStr_at(tkn,0) == '*' ){
								if( ZnkStr_at(tkn,1) == ' ' || ZnkStr_at(tkn,1) == '\n' ){
									/* docgen style comment */
									is_docgen_style = true;
									ZnkStr_cut_front( tkn, 2 );
								}
							}
#endif
							RanoCGIUtil_replaceNLtoHtmlBR( tkn );
							if( is_docgen_style ){
								if( ZnkStr_isBegin( tkn, " * " ) ){
									ZnkStr_cut_front( tkn, 3 );
								}
								/* title */
								if( idx < size-1 ){
									size_t i;
									ZnkStr next_tkn = ZnkStrAry_at( tkns, idx+1 );
									DcgTyp next_typ = atTyp( typs, idx+1 );
									if( next_typ == DcgTyp_e_CommentEnd && idx+1 < size-1 ){
										next_tkn = ZnkStrAry_at( tkns, idx+1+1 );
										next_typ = atTyp( typs, idx+1+1 );
									}
									i = 2;
									while( next_typ != DcgTyp_e_FuncDecl && idx+i < size-1 ){
										if( next_typ == DcgTyp_e_NL ){
											next_tkn = ZnkStrAry_at( tkns, idx+i+1 );
											next_typ = atTyp( typs, idx+i+1 );
											if( next_typ == DcgTyp_e_NL ){
												/* 空行検出 */
												break;
											}
										} else {
											next_tkn = ZnkStrAry_at( tkns, idx+i+1 );
											next_typ = atTyp( typs, idx+i+1 );
										}
										++i;
									}
									if( next_typ == DcgTyp_e_FuncDecl ){
										ZnkStr_addf( ans, "<br><b>%s</b>", ZnkStr_cstr(next_tkn) );
									}
								}
								ZnkStrEx_replace_BF( tkn, 0,
										"\n *", 3,
										"\n", 1,
										Znk_NPOS, Znk_NPOS );

								/* 最後の改行をカット(しかしたいして意味はないか?) */
								ZnkStr_chompC( tkn, Znk_NPOS, ' ' );
								ZnkStr_chompNL( tkn );
								if( ZnkStr_isEnd( tkn, "<br>" ) ){
									ZnkStr_cut_back( tkn, 4 );
								}

								ZnkStr_addf( ans, "<br><div class=MstyFrame>%s</div>", ZnkStr_cstr(tkn) );
							} else {
								ZnkStrEx_replace_BF( tkn, 0,
										" ", 1,
										"&nbsp;", 6,
										Znk_NPOS, Znk_NPOS );
								ZnkStrEx_replace_BF( tkn, 0,
										"\t", 1,
										"&nbsp;&nbsp;&nbsp;&nbsp;", 6*4,
										Znk_NPOS, Znk_NPOS );
								ZnkStr_addf( ans, "<span class=MstyCode><font color=#333355>/*%s*/</font></span>", ZnkStr_cstr(tkn) );
							}
							break;
						}
						case DcgTyp_e_Tab:
							ZnkStr_add( ans, "<span class=MstyCode>&nbsp;&nbsp;&nbsp;&nbsp;</span>" );
							break;
						case DcgTyp_e_Ignore:
							RanoCGIUtil_replaceNLtoHtmlBR( tkn );
							ZnkStr_addf( ans, "<span class=MstyCode>%s</span>", ZnkStr_cstr(tkn) );
							break;
						case DcgTyp_e_Operator:
							ZnkStr_addf( ans, "<span class=MstyCode><font color=#000000>%s</font></span>", ZnkStr_cstr(tkn) );
							break;
						case DcgTyp_e_Preprocessor:
							ZnkStr_addf( ans, "<span class=MstyCode><font color=#770077>%s</font> </span>", ZnkStr_cstr(tkn) );
							break;
						case DcgTyp_e_Keyword:
							if( isCWordTyp( prev_typ ) ){ ZnkStr_add_c( ans, ' ' ); }
							ZnkStr_addf( ans, "<span class=MstyCode><font color=#553322>%s</font></span>", ZnkStr_cstr(tkn) );
							break;
						case DcgTyp_e_Identifier:
							if( isCWordTyp( prev_typ ) ){ ZnkStr_add_c( ans, ' ' ); }
							if( ZnkStr_last(tkn) == '_' ){
								ZnkStr_addf( ans, "<span class=MstyCode><font color=#777700>%s</font></span>", ZnkStr_cstr(tkn) );
							} else if( ZnkStrEx_strstr(tkn, "_e_" ) ){
								ZnkStr_addf( ans, "<span class=MstyCode><font color=#aa3311>%s</font></span>", ZnkStr_cstr(tkn) );
							} else {
								ZnkStr_addf( ans, "<span class=MstyCode>%s</span>", ZnkStr_cstr(tkn) );
							}
							break;
						case DcgTyp_e_Typename:
							if( isCWordTyp( prev_typ ) ){ ZnkStr_add_c( ans, ' ' ); }
							ZnkStr_addf( ans, "<span class=MstyCode><font color=#007700>%s</font></span>", ZnkStr_cstr(tkn) );
							break;
						case DcgTyp_e_FuncDecl:
							if( isCWordTyp( prev_typ ) ){ ZnkStr_add_c( ans, ' ' ); }
							ZnkStr_addf( ans, "<span class=MstyCode><font color=blue>%s</font></span>", ZnkStr_cstr(tkn) );
							break;
						case DcgTyp_e_String:
							ZnkStr_addf( ans, "<span class=MstyCode><font color=#ff7777>%s</font></span>", ZnkStr_cstr(tkn) );
							break;
						case DcgTyp_e_Char:
							ZnkStr_addf( ans, "<span class=MstyCode><font color=#ff7777>%s</font></span>", ZnkStr_cstr(tkn) );
							break;
						case DcgTyp_e_Integer:
						case DcgTyp_e_Real:
							if( isCWordTyp( prev_typ ) ){ ZnkStr_add_c( ans, ' ' ); }
							ZnkStr_addf( ans, "<span class=MstyCode><font color=#aa0077>%s</font></span>", ZnkStr_cstr(tkn) );
							break;
						case DcgTyp_e_ParenthesisBegin:
							ZnkStr_addf( ans, "<span class=MstyCode>%s</span>", ZnkStr_cstr(tkn) );
							ZnkStr_add_c( ans, ' ' );
							break;
						case DcgTyp_e_ParenthesisEnd:
							if( ZnkStr_last( ans ) != ' ' ){
								ZnkStr_add_c( ans, ' ' );
							}
							ZnkStr_addf( ans, "<span class=MstyCode>%s</span>", ZnkStr_cstr(tkn) );
							break;
						default:
							ZnkStr_addf( ans, "<span class=MstyCode>%s</span>", ZnkStr_cstr(tkn) );
							break;
						}
					}
					prev_typ = typ;
				}
			}

			ZnkStr_delete( line );
			ZnkBfr_destroy( typs );
			ZnkStrAry_destroy( tkns );
			Znk_fclose( fp );
		}
	}

	ZnkStr_add( ans, "</body>\n</html>" );

	{
		ZnkFile fp = Znk_fopen( out_file_path, "wb" );
		if( fp ){
			Znk_fputs( ZnkStr_cstr(ans), fp );
			Znk_fclose( fp );
		}
	}
	ZnkMyf_destroy( myf );
}

struct FilterInfo {
	ZnkStr      ans_;
	ZnkMyf      menu_myf_;
	const char* src_topdir_;
	const char* dst_topdir_;
	ZnkStrAry   file_list_;
	ZnkStrAry   ignore_list_;
};

static bool
filterFile( const char* src_file_path, const char* dst_file_path, void* arg )
{
	if( ZnkS_eq( ZnkS_get_extension( dst_file_path, '.' ), "h" ) ){
		/* .h を除去 */
		char out_file_path[ 256 ] = "";
		struct FilterInfo* info = Znk_force_ptr_cast( struct FilterInfo*, arg );
		ZnkStr ans      = info->ans_;
		ZnkMyf menu_myf = info->menu_myf_;
		ZnkS_copy( out_file_path, sizeof(out_file_path), dst_file_path, Znk_strlen( dst_file_path ) - 2 );
		ZnkS_concat( out_file_path, sizeof(out_file_path), ".html" );

		{
			/***
			 * とりあえず.
			 */
			const char* p = Znk_strstr( out_file_path, "/mydoc/source_doc/" );
			if( p ){
				ZnkStrAry_push_bk_cstr( info->file_list_, p, Znk_NPOS );
			}
		}

		makeOne( ans, menu_myf, src_file_path, out_file_path );

		return true;
	}
	return false;
}

static bool
is_processFile( const char* file_path, void* arg )
{
	const char* in_dir = Znk_force_ptr_cast( char*, arg );
	const char* under_path = file_path + Znk_strlen(in_dir);
	RanoLog_printf( "under_path=[%s].\n", under_path );

	if( ZnkS_isBegin( under_path, "/doc_in/" ) ){ return false; }
	if( ZnkS_isBegin( under_path, "/mkf_android/" ) ){ return false; }
	if( ZnkS_isBegin( under_path, "/mkf_triggers/" ) ){ return false; }
	if( ZnkS_isBegin( under_path, "/out_dir/" ) ){ return false; }
	if( ZnkS_isBegin( under_path, "/source_doc/" ) ){ return false; }
	return true;
}

static bool
isIgnoreDir( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	struct FilterInfo* info = Znk_force_ptr_cast( struct FilterInfo*, arg );
	ZnkStrAry ignore_list = info->ignore_list_;
	size_t src_topdir_leng = Znk_strlen( info->src_topdir_ );
	const char* under_path = top_dir + src_topdir_leng;

	if( under_path[ 0 ] == '/' ){ ++under_path; }
	if( ignore_list && ZnkStrAry_find_isMatch( ignore_list, 0, under_path, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
		/* skip */
		return true;
	}
	return false;
}

static ZnkStrAry
makeIgnoreList( void )
{
	ZnkStrAry list = ZnkStrAry_create( true );
	ZnkStrAry_push_bk_cstr( list, "*/mkf_android", Znk_NPOS );
	ZnkStrAry_push_bk_cstr( list, "*/mkf_triggers", Znk_NPOS );
	ZnkStrAry_push_bk_cstr( list, "*/out_dir", Znk_NPOS );
	ZnkStrAry_push_bk_cstr( list, "*/source_doc", Znk_NPOS );
	ZnkStrAry_push_bk_cstr( list, "*/doc_in", Znk_NPOS );
	return list;
}

bool
DocSource_make( const char* in_dir, const char* out_dir )
{
	bool result = false;
	ZnkMyf menu_myf = ZnkMyf_create();
	ZnkStr ans = ZnkStr_new( "" );
	ZnkStr ermsg = ZnkStr_new( "" );
	struct FilterInfo info = { 0 };

	info.ans_      = ans;
	info.menu_myf_ = menu_myf;
	info.src_topdir_  = in_dir;
	info.dst_topdir_  = out_dir;
	info.file_list_   = ZnkStrAry_create( true );
	info.ignore_list_ = makeIgnoreList();

	if( ZnkDir_mkdirPath( out_dir, Znk_NPOS, '/', NULL ) ){
		RanoLog_printf( "out_dir=[%s] is mkdir.\n", out_dir );
	} else {
		RanoLog_printf( "out_dir=[%s] cannot mkdir.\n", out_dir );
	}

	{
		char menu_myf_path[ 256 ];
		Znk_snprintf( menu_myf_path, sizeof(menu_myf_path), "%s/docgen.myf", in_dir );
		if( !ZnkMyf_load( menu_myf, menu_myf_path ) ){
			Znk_printf_e( "docgen Error : Cannot load [%s].\n", menu_myf_path );
			Znk_getchar();
			goto FUNC_END;
		}
	}

	RanoDirUtil_filterDir( in_dir, out_dir,
			"docgen", ermsg,
			is_processFile, (void*)in_dir,
			filterFile, &info,
			isIgnoreDir, &info );

	{
		char index_path[ 256 ] = "";
		ZnkFile fp = NULL;
		Znk_snprintf( index_path, sizeof(index_path), "%s/index.html", out_dir ); 
		fp = Znk_fopen( index_path, "wb" );
		if( fp ){
			size_t idx;
			size_t size = ZnkStrAry_size( info.file_list_ );
			Znk_fprintf( fp, "<html><body>" );
			for( idx=0; idx<size; ++idx ){
				const char* href = ZnkStrAry_at_cstr( info.file_list_, idx );
				Znk_fprintf( fp, "<a class=MstyElemLink href=%s>%s</a><br>\n", href, href );
			}
			Znk_fprintf( fp, "</body></html>" );
			Znk_fclose( fp );
		}
	}

	result = true;
FUNC_END:
	ZnkStr_delete( ermsg );
	ZnkStr_delete( ans );
	ZnkMyf_destroy( menu_myf );

	ZnkStrAry_destroy( info.file_list_ );
	ZnkStrAry_destroy( info.ignore_list_ );

	return result;
}
