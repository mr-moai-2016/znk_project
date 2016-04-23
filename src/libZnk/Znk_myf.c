#include <Znk_myf.h>
#include <Znk_str.h>
#include <Znk_str_fio.h>
#include <Znk_str_dary.h>
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <Znk_varp_dary.h>
#include <string.h>

struct ZnkMyfSectionImpl {
	ZnkStr            name_;
	ZnkMyfSectionType type_;
	union Data_tag {
		ZnkStrDAry  lines_;
		ZnkVarpDAry vars_;
	} u_;
};

ZnkStrDAry
ZnkMyfSection_lines( const ZnkMyfSection sec )
{
	assert( sec->type_ == ZnkMyfSection_e_Lines );
	return sec->u_.lines_;
}
ZnkVarpDAry
ZnkMyfSection_vars( const ZnkMyfSection sec )
{
	assert( sec->type_ == ZnkMyfSection_e_Vars );
	return sec->u_.vars_;
}
const char*
ZnkMyfSection_name( const ZnkMyfSection sec )
{
	return ZnkStr_cstr( sec->name_ );
}

static void
makeUnionData( union Data_tag* u, ZnkMyfSectionType type )
{
	if( type == ZnkMyfSection_e_Lines ){
		u->lines_ = ZnkStrDAry_create( true );
	} else if( type == ZnkMyfSection_e_Vars ){
		u->vars_ = ZnkVarpDAry_create( true );
	}
}
static void
deleteUnionData( union Data_tag* u, ZnkMyfSectionType type )
{
	if( type == ZnkMyfSection_e_Lines ){
		ZnkStrDAry_destroy( u->lines_ );
	} else if( type == ZnkMyfSection_e_Vars ){
		ZnkVarpDAry_destroy( u->vars_ );
	}
}

static ZnkMyfSection
makeMyfSection( const char* name, ZnkMyfSectionType type )
{
	ZnkMyfSection sec = (ZnkMyfSection)Znk_alloc0( sizeof( struct ZnkMyfSectionImpl ) );
	sec->name_ = ZnkStr_new( "" );
	sec->type_ = type;
	if( sec->type_ == ZnkMyfSection_e_Lines ){
		sec->u_.lines_ = ZnkStrDAry_create( true );
	} else if( sec->type_ == ZnkMyfSection_e_Vars ){
		sec->u_.vars_ = ZnkVarpDAry_create( true );
	}
	return sec;
}
static void
deleteMyfSection( void* elem )
{
	ZnkMyfSection sec = (ZnkMyfSection)elem;
	if( sec ){
		ZnkStr_delete( sec->name_ );
		if( sec->type_ == ZnkMyfSection_e_Lines ){
			ZnkStrDAry_destroy( sec->u_.lines_ );
		} else if( sec->type_ == ZnkMyfSection_e_Vars ){
			ZnkVarpDAry_destroy( sec->u_.vars_ );
		}
		sec->type_ = ZnkMyfSection_e_None;
		Znk_free( sec );
	}
}

struct ZnkMyfImpl {
	char       quote_begin_[ 32 ];
	char       quote_end_[ 32 ];
	ZnkObjDAry sec_ary_;
};

ZnkMyf
ZnkMyf_create( void )
{
	ZnkMyf myf = Znk_alloc0( sizeof( struct ZnkMyfImpl ) );
	myf->quote_begin_[ 0 ] = '\0';
	myf->quote_end_[ 0 ]   = '\0';
	myf->sec_ary_ = ZnkObjDAry_create( deleteMyfSection );
	return myf;
}

void
ZnkMyf_destroy( ZnkMyf myf )
{
	if( myf ){
		ZnkObjDAry_destroy( myf->sec_ary_ );
		Znk_free( myf );
	}
}

const char*
ZnkMyf_quote_begin( const ZnkMyf myf )
{
	return myf->quote_begin_;
}
const char*
ZnkMyf_quote_end( const ZnkMyf myf )
{
	return myf->quote_end_;
}

ZnkMyfSection
ZnkMyf_registSection( ZnkMyf myf, const char* name, ZnkMyfSectionType type )
{
	size_t size = ZnkObjDAry_size( myf->sec_ary_ );
	size_t idx;
	ZnkMyfSection sec = NULL;
	for( idx=0; idx<size; ++idx ){
		sec = (ZnkMyfSection)ZnkObjDAry_at( myf->sec_ary_, idx );
		if( ZnkStr_eq( sec->name_, name ) ){
			/* already exist */
			switch( type ){
			case ZnkMyfSection_e_Lines:
			case ZnkMyfSection_e_Vars:
				if( sec->type_ != type ){
					/* remaking */
					deleteUnionData( &sec->u_, sec->type_ );
					sec->type_ = type;
					makeUnionData( &sec->u_, sec->type_ );
				}
				break;
			case ZnkMyfSection_e_None:
				/* deleting */
				deleteUnionData( &sec->u_, sec->type_ );
				sec->type_ = type;
				return NULL;
			default:
				assert( 0 );
				break;
			}
			return sec;
		}
	}
	switch( type ){
	case ZnkMyfSection_e_Lines:
	case ZnkMyfSection_e_Vars:
		sec = makeMyfSection( name, type );
		break;
	default:
		sec = NULL;
		break;
	}
	return sec;
}

size_t
ZnkMyf_numOfSection( const ZnkMyf myf )
{
	return ZnkObjDAry_size( myf->sec_ary_ );
}
ZnkMyfSection
ZnkMyf_atSection( const ZnkMyf myf, size_t sec_idx )
{
	return (ZnkMyfSection)ZnkObjDAry_at( myf->sec_ary_, sec_idx );
}

static bool
parseDefQuote( ZnkMyf myf, const char* p, size_t p_leng, size_t count, const char* defblk_name )
{
	size_t pos = 0;
	size_t arg_leng = 0;
	const char* end = p + p_leng;

	/* 1th arg */
	p += pos;
	pos = ZnkS_lfind_arg( p, 0, end-p, 0, &arg_leng, " \t", 2 );
	if( arg_leng != strlen(defblk_name) ){
		/* defblk_nameで始まる文字であるが、それそのものではない */
		goto FUNC_ERROR;
	}
	if( pos == Znk_NPOS ){ goto FUNC_ERROR; }

	/* 2th arg */
	p += pos + arg_leng;
	pos = ZnkS_lfind_arg( p, 0, end-p, 0, &arg_leng, " \t", 2 );
	if( pos == Znk_NPOS ){ goto FUNC_ERROR; }
	ZnkS_copy( myf->quote_begin_, sizeof(myf->quote_begin_), p+pos, arg_leng );

	/* 3th arg */
	p += pos + arg_leng;
	pos = ZnkS_lfind_arg( p, 0, end-p, 0, &arg_leng, " \t", 2 );
	if( pos == Znk_NPOS ){ goto FUNC_ERROR; }
	ZnkS_copy( myf->quote_end_, sizeof(myf->quote_end_), p+pos, arg_leng );

	return true;

FUNC_ERROR:
	ZnkF_printf_e( "Znk_myf load : Syntax Error : line=[%u]\n", count );
	return false;
}

static ZnkVarp
parseVarDirective( ZnkVarpDAry vars, const char* p, size_t p_leng )
{
	size_t key_begin; size_t key_end;
	size_t val_begin; size_t val_end;
	ZnkVarp varp = NULL;

	ZnkS_find_key_and_val( p, 0, p_leng,
			&key_begin, &key_end,
			&val_begin, &val_end,
			"=", 1,
			" \t", 2 );
	if( key_begin == Znk_NPOS || key_end == Znk_NPOS || val_begin == Znk_NPOS ){
		return NULL;
	}

	/***
	 * まずnameを""として値だけ登録し、すぐ後でnameを設定.
	 */
	varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_None );
	ZnkVar_set_val_Str( varp, p+val_begin, val_end-val_begin );
	ZnkVarpDAry_push_bk( vars, varp );
	ZnkStr_assign( varp->name_, 0, p+key_begin, key_end-key_begin );
	return varp;
}

static bool
parseSectionLines( ZnkMyf myf, const char* p, size_t p_leng,
		ZnkFile fp, size_t* count, ZnkStr line )
{
	size_t pos = 0;
	size_t arg_leng = 0;
	ZnkMyfSection sec = makeMyfSection( "", ZnkMyfSection_e_Lines );

	ZnkObjDAry_push_bk( myf->sec_ary_, (ZnkObj)sec );

	/* 1th arg */
	pos = ZnkS_lfind_arg( p+pos, 0, p_leng-pos, 0, &arg_leng, " \t", 2 ) + pos;
	ZnkStr_assign( sec->name_, 0, p+pos, arg_leng );

	while( true ){
		if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			/* Unexpected eof */
			ZnkF_printf_e( "ZnkMyf_load : Unexpected EOF in SectionLines %s : line:%u\n",
					ZnkStr_cstr(sec->name_), *count );
			return false;
		}
		++*count;
		ZnkStr_chompNL( line );

		if( ZnkStr_empty( line ) ){
			continue;
		}
		if( ZnkStr_first( line ) == '#' ){
			/* treat as comment out */
			continue;
		}
		if( ZnkStr_eq( line, "@@." ) ){
			/* endof this Section */
			break;
		}
		if( ZnkStr_eq( line, "@@^" ) ){
			/* Special empty */
			ZnkStr_cut_front( line, 3 );
			break;
		}
		ZnkStrDAry_push_bk_cstr( sec->u_.lines_, ZnkStr_cstr(line), Znk_NPOS );
	}
	return true;
}
static bool
parseSectionVars( ZnkMyf myf, const char* p, size_t p_leng,
		ZnkFile fp, size_t* count, ZnkStr line )
{
	size_t pos = 0;
	size_t arg_leng = 0;
	const char*  quote_begin = myf->quote_begin_;
	const char*  quote_end   = myf->quote_end_;
	const size_t quote_begin_leng = strlen( quote_begin );
	ZnkVarp varp = NULL;
	const char* nl = "\n";
	ZnkMyfSection sec = makeMyfSection( "", ZnkMyfSection_e_Vars );

	ZnkObjDAry_push_bk( myf->sec_ary_, (ZnkObj)sec );

	/* 1th arg */
	pos = ZnkS_lfind_arg( p+pos, 0, p_leng-pos, 0, &arg_leng, " \t", 2 ) + pos;
	ZnkStr_assign( sec->name_, 0, p+pos, arg_leng );

	while( true ){
		if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			/* Unexpected eof */
			ZnkF_printf_e( "ZnkMyf_load : Unexpected EOF in SectionVars %s : line:%u\n",
					ZnkStr_cstr(sec->name_), *count );
			return false;
		}
		++*count;
		ZnkStr_chompNL( line );

		if( ZnkStr_empty( line ) ){
			continue;
		}
		if( ZnkStr_first( line ) == '#' ){
			/* treat as comment out */
			continue;
		}
		if( ZnkStr_eq( line, "@@." ) ){
			/* endof this Section */
			break;
		}
		if( ZnkStr_eq( line, "@@^" ) ){
			/* Special empty */
			ZnkStr_cut_front( line, 3 );
			break;
		}
		varp = parseVarDirective( sec->u_.vars_, ZnkStr_cstr(line), ZnkStr_leng(line) );
		if( varp == NULL ){
			ZnkF_printf_e( "ZnkMyf_load : Syntax Error in SectionVars %s : line:%u\n",
					ZnkStr_cstr(sec->name_), *count );
			return false;
		}
		{
			ZnkStr val = varp->prim_.u_.str_;
			if( ZnkStr_isBegin( val, quote_begin ) ){
				const char* begin;
				const char* end;
				ZnkStr_cut_front( val, quote_begin_leng );
				begin = ZnkStr_cstr( val );
				end   = strstr( begin, quote_end );
				if( end ){
					ZnkStr_releng( val, end-begin );
					continue;
				}
				ZnkStr_add( val, nl );
				while( true ){
					if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
						/* Unexpected eof */
						ZnkF_printf_e( "ZnkMyf_load : Unexpected EOF in SectionVars %s : line:%u\n",
								ZnkStr_cstr(sec->name_), *count );
						return false;
					}
					++*count;
					ZnkStr_chompNL( line );
					begin = ZnkStr_cstr( line );
					end   = strstr( begin, quote_end );
					if( end ){
						ZnkStr_releng( line, end-begin );
						ZnkStr_add( val, ZnkStr_cstr(line) );
						break;
					}
					ZnkStr_add( val, begin );
					ZnkStr_add( val, nl );
				}
			}
		}
	}
	return true;
}

bool
ZnkMyf_load( ZnkMyf myf, const char* filename )
{
	bool result = false;
	ZnkFile fp = ZnkF_fopen( filename, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		size_t count = 0;
		ZnkStrDAry command_ary = ZnkStrDAry_create( true );
	
		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				/* eof */
				break;
			}
			++count;
			ZnkStr_chompNL( line );

			if( ZnkStr_empty( line ) ){
				continue;
			}
			if( ZnkStr_first( line ) == '#' ){
				/* treat as comment out */
				continue;
			}

			if( ZnkStr_isBegin( line, "@def_quote" ) ){
				const char*  p      = ZnkStr_cstr( line ) + 1;
				const size_t p_leng = ZnkStr_leng( line ) - 1;
				if( !parseDefQuote( myf, p, p_leng, count, "def_quote" ) ){
					goto FUNC_END;
				}
			}

			if( ZnkStr_isBegin( line, "@@" ) ){
				const char*  p      = ZnkStr_cstr( line ) + 3;
				const size_t p_leng = ZnkStr_leng( line ) - 3;
				char sec_type_ch = *(p-1);
				switch( sec_type_ch ){
				case 'V':
					if( !parseSectionVars( myf, p, p_leng, fp, &count, line ) ){
						goto FUNC_END;
					}
					break;
				case 'L':
					if( !parseSectionLines( myf, p, p_leng, fp, &count, line ) ){
						goto FUNC_END;
					}
					break;
				case '^':
					/* OutOfSectionにある 特殊空文字 */
					/* これは単に無視することにする */
					break;
				case '.':
					/* OutOfSectionにある endof section 記号 */
					/* これは単に無視することにする */
					break;
				default:
					ZnkF_printf_e( "ZnkMyf_load : Syntax Error : line:%u\n", count );
					ZnkF_printf_e( "  Unrecognize Section begin line=[%s]\n", ZnkStr_cstr(line) );
					goto FUNC_END;
				}
			}
		}
		result = true;
FUNC_END:
		ZnkStr_delete( line );
		ZnkF_fclose( fp );
		ZnkStrDAry_destroy( command_ary );
	}
	return result;
}

bool
ZnkMyf_save( ZnkMyf myf, const char* filename )
{
	ZnkFile fp = ZnkF_fopen( filename, "wb" );
	if( fp ){
		const char* nl = "\n";
		size_t sec_size;
		size_t sec_idx;
		size_t size;
		size_t idx;
		ZnkVarp varp = NULL;
	
		ZnkF_fprintf( fp, "@def_quote %s %s%s", myf->quote_begin_, myf->quote_end_, nl );
		ZnkF_fputs( nl, fp );

		sec_size = ZnkObjDAry_size( myf->sec_ary_ );
		for( sec_idx=0; sec_idx<sec_size; ++sec_idx ){
			ZnkMyfSection sec = (ZnkMyfSection)ZnkObjDAry_at( myf->sec_ary_, sec_idx );
			switch( sec->type_ ){
			case ZnkMyfSection_e_Lines:
				ZnkF_fprintf( fp, "@@L %s%s", ZnkStr_cstr(sec->name_),  nl );
				size = ZnkStrDAry_size( sec->u_.lines_ );
				for( idx=0; idx<size; ++idx ){
					ZnkF_fprintf( fp, "%s%s", ZnkStrDAry_at_cstr( sec->u_.lines_, idx ), nl );
				}
				break;
			case ZnkMyfSection_e_Vars:
				ZnkF_fprintf( fp, "@@V %s%s", ZnkStr_cstr(sec->name_),  nl );
				size = ZnkVarpDAry_size( sec->u_.vars_ );
				for( idx=0; idx<size; ++idx ){
					varp = ZnkVarpDAry_at( sec->u_.vars_, idx );
					ZnkF_fprintf( fp, "%s = %s%s%s%s",
							ZnkStr_cstr( varp->name_ ),
							myf->quote_begin_, ZnkVar_cstr( varp ), myf->quote_end_,
							nl );
				}
				break;
			default:
				break;
			}
			ZnkF_fprintf( fp, "@@.%s", nl );
			ZnkF_fputs( nl, fp );
		}
	
		ZnkF_fclose( fp );
		return true;
	}
	return false;
}
ZnkVarpDAry
ZnkMyf_find_vars( const ZnkMyf myf, const char* section_name )
{
	const size_t sec_size = ZnkObjDAry_size( myf->sec_ary_ );
	size_t sec_idx;
	ZnkMyfSection sec;
	for( sec_idx=0; sec_idx<sec_size; ++sec_idx ){
		sec = (ZnkMyfSection)ZnkObjDAry_at( myf->sec_ary_, sec_idx );
		if( sec->type_ == ZnkMyfSection_e_Vars && ZnkStr_eq( sec->name_, section_name ) ){
			/* found */
			return sec->u_.vars_;
		}
	}
	return NULL;
}
ZnkStrDAry
ZnkMyf_find_lines( const ZnkMyf myf, const char* section_name )
{
	const size_t sec_size = ZnkObjDAry_size( myf->sec_ary_ );
	size_t sec_idx;
	ZnkMyfSection sec;
	for( sec_idx=0; sec_idx<sec_size; ++sec_idx ){
		sec = (ZnkMyfSection)ZnkObjDAry_at( myf->sec_ary_, sec_idx );
		if( sec->type_ == ZnkMyfSection_e_Lines && ZnkStr_eq( sec->name_, section_name ) ){
			/* found */
			return sec->u_.lines_;
		}
	}
	return NULL;
}

ZnkVarp
ZnkMyf_refVar( ZnkMyf myf, const char* section_name, const char* var_name )
{
	ZnkVarpDAry vars = ZnkMyf_find_vars( myf, section_name );
	if( vars ){
		size_t size = ZnkVarpDAry_size( vars );
		size_t idx;
		ZnkVarp varp;
		for( idx=0; idx<size; ++idx ){
			varp = ZnkVarpDAry_at( vars, idx );
			if( ZnkStr_eq( varp->name_, var_name ) ){
				return varp;
			}
		}
	}
	return NULL;
}

