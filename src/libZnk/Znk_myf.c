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
	switch( type ){
	case ZnkMyfSection_e_Lines:
	case ZnkMyfSection_e_OutOfSection:
		u->lines_ = ZnkStrDAry_create( true );
		break;
	case ZnkMyfSection_e_Vars:
		u->vars_ = ZnkVarpDAry_create( true );
		break;
	default:
		break;
	}
}
static void
deleteUnionData( union Data_tag* u, ZnkMyfSectionType type )
{
	switch( type ){
	case ZnkMyfSection_e_Lines:
	case ZnkMyfSection_e_OutOfSection:
		ZnkStrDAry_destroy( u->lines_ );
		break;
	case ZnkMyfSection_e_Vars:
		ZnkVarpDAry_destroy( u->vars_ );
		break;
	default:
		break;
	}
}

static ZnkMyfSection
makeMyfSection( const char* name, ZnkMyfSectionType type )
{
	ZnkMyfSection sec = (ZnkMyfSection)Znk_alloc0( sizeof( struct ZnkMyfSectionImpl ) );
	sec->name_ = ZnkStr_new( "" );
	sec->type_ = type;
	makeUnionData( &sec->u_, type );
	return sec;
}
static void
deleteMyfSection( void* elem )
{
	ZnkMyfSection sec = (ZnkMyfSection)elem;
	if( sec ){
		ZnkStr_delete( sec->name_ );
		deleteUnionData( &sec->u_, sec->type_ );
		sec->type_ = ZnkMyfSection_e_None;
		Znk_free( sec );
	}
}

struct ZnkMyfImpl {
	char       quote_begin_[ 32 ];
	char       quote_end_[ 32 ];
	char       nl_[ 32 ];
	/***
	 * MyfSection格納および寿命管理用
	 */
	ZnkObjDAry sec_ary_;
	/***
	 * OutOfSection格納および寿命管理用.
	 * OutOfSectionも内部的にはZnkMyfSectionとして扱う.
	 */
	ZnkObjDAry oos_ary_;
	/***
	 * ここでの順序記録はZnkMyfSectionの参照の配列として、
	 * MyfSectionおよびOutOfSectionの記載されている順序を保存する.
	 * これはload => saveの際にこれらの元の構造と順番が失われないようにするためである.
	 * この配列では参照のみを保持し、各要素の寿命は管理しない.
	 */
	ZnkObjDAry order_;
};

ZnkMyf
ZnkMyf_create( void )
{
	ZnkMyf myf = Znk_alloc0( sizeof( struct ZnkMyfImpl ) );
	myf->quote_begin_[ 0 ] = '\0';
	myf->quote_end_[ 0 ]   = '\0';
	ZnkS_copy_literal( myf->nl_, sizeof(myf->nl_), "\n" );
	myf->sec_ary_ = ZnkObjDAry_create( deleteMyfSection );
	myf->oos_ary_ = ZnkObjDAry_create( deleteMyfSection );
	/***
	 * order_ではZnkMyfSectionの参照のみを管理(寿命は管理しない).
	 */
	myf->order_   = ZnkObjDAry_create( NULL );
	return myf;
}

void
ZnkMyf_destroy( ZnkMyf myf )
{
	if( myf ){
		ZnkObjDAry_destroy( myf->sec_ary_ );
		ZnkObjDAry_destroy( myf->oos_ary_ );
		ZnkObjDAry_destroy( myf->order_ );
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
		/***
		 * 中身が空の新規Sectionを登録する.
		 */
		sec = makeMyfSection( name, type );
		ZnkObjDAry_push_bk( myf->sec_ary_, (ZnkObj)sec );
		ZnkObjDAry_push_bk( myf->order_, (ZnkObj)sec );
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
	ZnkMyfSection sec;

	/***
	 * sec_ary_およびorder_への登録.
	 */
	sec = makeMyfSection( "", ZnkMyfSection_e_Lines );
	ZnkObjDAry_push_bk( myf->sec_ary_, (ZnkObj)sec );
	ZnkObjDAry_push_bk( myf->order_, (ZnkObj)sec );

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
	ZnkMyfSection sec;

	/***
	 * sec_ary_およびorder_への登録.
	 */
	sec = makeMyfSection( "", ZnkMyfSection_e_Vars );
	ZnkObjDAry_push_bk( myf->sec_ary_, (ZnkObj)sec );
	ZnkObjDAry_push_bk( myf->order_, (ZnkObj)sec );

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

static void
registOutOfSection( ZnkMyf myf, ZnkStrDAry text )
{
	ZnkMyfSection sec;

	/***
	 * oos_ary_およびorder_への登録.
	 */
	sec = makeMyfSection( "", ZnkMyfSection_e_OutOfSection );
	ZnkObjDAry_push_bk( myf->oos_ary_, (ZnkObj)sec );
	ZnkObjDAry_push_bk( myf->order_, (ZnkObj)sec );

	/***
	 * swapにより結果的にtextの内容をlines_へ移動する形となる.
	 * (textの方はクリアされる形になる)
	 */
	ZnkStrDAry_swap( sec->u_.lines_, text );

}

bool
ZnkMyf_load( ZnkMyf myf, const char* filename )
{
	bool result = false;
	ZnkFile fp = ZnkF_fopen( filename, "rb" );
	if( fp ){
		ZnkStr line = ZnkStr_new( "" );
		size_t count = 0;
		/***
		 * OutOfSectionにおけるTextを記録する.
		 */
		ZnkStrDAry oos_text = ZnkStrDAry_create( true );
		bool is_outof_section = true;

		if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
			/* eof */
			ZnkF_printf_e( "ZnkMyf_load : Syntax Error : line:%u\n", count );
			ZnkF_printf_e( "  File is empty.\n");
			goto FUNC_END;
		}
		++count;

		/***
		 * 改行コードがどうなっているのかをまず解析.
		 */
		{
			if( ZnkStr_isEnd( line, "\r\n" ) ){
				ZnkS_copy_literal( myf->nl_, sizeof(myf->nl_), "\r\n" );
			} else if( ZnkStr_last( line ) == '\n' ){
				ZnkS_copy_literal( myf->nl_, sizeof(myf->nl_), "\n" );
			} else if( ZnkStr_last( line ) == '\r' ){
				ZnkS_copy_literal( myf->nl_, sizeof(myf->nl_), "\r" );
			} else {
				/* defaultを採用 */
			}
		}
		ZnkStr_chompNL( line );

		/***
		 * 開始行における@def_quote指定は必須とする.
		 * (その方が使用するクォート記号列がmyfファイル内でも明示化され
		 * メンテナンスする場合でも望ましいと思う)
		 */
		if( ZnkStr_isBegin( line, "@def_quote" ) ){
			const char*  p      = ZnkStr_cstr( line ) + 1;
			const size_t p_leng = ZnkStr_leng( line ) - 1;
			if( !parseDefQuote( myf, p, p_leng, count, "def_quote" ) ){
				goto FUNC_END;
			}
		} else {
			ZnkF_printf_e( "ZnkMyf_load : Syntax Error : line:%u\n", count );
			ZnkF_printf_e( "  @def_quote is not found.\n");
			goto FUNC_END;
		}

		while( true ){
			if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				/* eof */
				break;
			}
			++count;
			ZnkStr_chompNL( line );

			if( ZnkStr_isBegin( line, "@@" ) ){
				const char*  p      = ZnkStr_cstr( line ) + 3;
				const size_t p_leng = ZnkStr_leng( line ) - 3;
				char sec_type_ch = *(p-1);
				switch( sec_type_ch ){
				case 'V':
					registOutOfSection( myf, oos_text );
					if( !parseSectionVars( myf, p, p_leng, fp, &count, line ) ){
						goto FUNC_END;
					}
					is_outof_section = false;
					break;
				case 'L':
					registOutOfSection( myf, oos_text );
					if( !parseSectionLines( myf, p, p_leng, fp, &count, line ) ){
						goto FUNC_END;
					}
					is_outof_section = false;
					break;
				default:
					/* それ以外の文字が来た場合はOutOfSectionのテキストのままとして扱う */
					break;
				}
			}

			if( is_outof_section ){
				ZnkStrDAry_push_bk_cstr( oos_text, ZnkStr_cstr(line), Znk_NPOS );
			} else {
				/* OutOfSectionの再開 */
				is_outof_section = true;
			}
		}
		result = true;
FUNC_END:
		ZnkStr_delete( line );
		ZnkF_fclose( fp );
		ZnkStrDAry_destroy( oos_text );
	}
	return result;
}

bool
ZnkMyf_save( ZnkMyf myf, const char* filename )
{
	ZnkFile fp = ZnkF_fopen( filename, "wb" );
	if( fp ){
		const char* nl = myf->nl_;
		size_t order_size;
		size_t order_idx;
		size_t size;
		size_t idx;
		ZnkVarp varp = NULL;
	
		ZnkF_fprintf( fp, "@def_quote %s %s%s", myf->quote_begin_, myf->quote_end_, nl );

		order_size = ZnkObjDAry_size( myf->order_ );
		for( order_idx=0; order_idx<order_size; ++order_idx ){
			ZnkMyfSection sec = (ZnkMyfSection)ZnkObjDAry_at( myf->order_, order_idx );
			switch( sec->type_ ){
			case ZnkMyfSection_e_Lines:
				ZnkF_fprintf( fp, "@@L %s%s", ZnkStr_cstr(sec->name_),  nl );
				size = ZnkStrDAry_size( sec->u_.lines_ );
				for( idx=0; idx<size; ++idx ){
					ZnkF_fprintf( fp, "%s%s", ZnkStrDAry_at_cstr( sec->u_.lines_, idx ), nl );
				}
				ZnkF_fprintf( fp, "@@.%s", nl );
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
				ZnkF_fprintf( fp, "@@.%s", nl );
				break;
			case ZnkMyfSection_e_OutOfSection:
				size = ZnkStrDAry_size( sec->u_.lines_ );
				for( idx=0; idx<size; ++idx ){
					ZnkF_fprintf( fp, "%s%s", ZnkStrDAry_at_cstr( sec->u_.lines_, idx ), nl );
				}
				break;
			default:
				break;
			}
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

