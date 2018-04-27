#include <Est_parser.h>
#include <Znk_mem_find.h>
#include <Znk_str_ex.h>
#include <Znk_str_fio.h>
#include <Znk_missing_libc.h>
#include <Znk_htp_util.h>
#include <string.h>
#include <ctype.h>


Znk_INLINE bool
isASCII( uint32_t c ) { return (bool)( c < 0x80 ); }
Znk_INLINE bool
SJIS_isHankakuKatakana( uint8_t b ){ return ( b >= 0xa1 && b <= 0xdf ); }
Znk_INLINE bool
SJIS_isSecondByte( const char* base, const char* p )
{
	int lbc = 0;
	while( p > base ){
		--p;
		if( isASCII(*p) ){
			/* second or ascii */
			break;
		} else if( SJIS_isHankakuKatakana(*p) ){
			/* second or hankaku_katakana */
			break;
		} else {
			/* second or first */
		}
		++lbc;
	}
	return (bool)(lbc & 1);
}

/**
 * @brief
 *   クォート文字で囲まれている範囲のうち、開始クォート文字より直後から終了クォート文字までの部分の範囲を取得する.
 *   つまりppとして与える位置は開始クォート文字の直後を示していなければならない.
 *   また終了クォート文字はinclude_end_quoteがtrueの場合のみ取得される.
 *   escapeシーケンス \ を解釈し、その直後にある特殊文字はそのままの文字として取得する.
 *
 * @param pp 
 *   開始クォート文字の直後を示す文字列ポインタのポインタ.
 *   
 * @param qch 
 *   終了クォート文字.
 *
 * @param ans
 *   値の取得用文字列.
 *   NULLを指定することもでき、その場合この関数は単にクォート文字で囲まれている範囲を読み飛ばす処理になる.
 *
 * @param include_end_quote
 *   trueの場合、終了クォート文字もansの最後尾に追加する.
 *
 * @return:
 *   指定したqchでちゃんと終端していた場合はtrueを返す.
 *   qchが現れる前にNULL終端していた場合はfalseを返す.
 */
static bool
scanQuote( const char** pp, char qch, ZnkStr ans, bool include_end_quote )
{
	bool result = false;
	const char* p = *pp;
	bool pre_esc = false;
	while( *p ){
		if( *p == qch ){
			if( !pre_esc ){
				result = true;
				if( include_end_quote && ans ){ ZnkStr_add_c( ans, *p ); }
				++p;
				break;
			}
			pre_esc = false;
		} else if( *p == '\\' ){
			if( pre_esc ){
				pre_esc = false;
			} else {
				if( !SJIS_isSecondByte( *pp, p ) ){
					pre_esc = true;
				}
			}
		} else {
			/* 通常の文字 */
			if( pre_esc ){
				/* 例えば\nなどのケース. この場合ここでpre_escは解除しなければならない.
				 * ただしここでは\nなどのケースを実際の改行コードに変更したりはせずそのままの形を維持させる. */
				pre_esc = false;
			}
		}
		if( ans ){ ZnkStr_add_c( ans, *p ); }
		++p;
	}
	*pp = p;
	return result;
}

/**
 * @brief
 *   タグが終了するかホワイトスペースが登場するまでの範囲を読み込み取得する.
 *
 * @param pp 
 *   取得開始文字を示す文字列ポインタのポインタ.
 *   
 * @param ans
 *   値の取得用文字列.
 *
 * @return:
 *   タグ終端またはホワイトスペースが登場した場合はtrueを返す.
 *   それらが現れる前にNULL終端していた場合はfalseを返す.
 */
static bool
scanToDelimiter( const char** pp, ZnkStr ans )
{
	const char* p = *pp;

	/***
	 * XHTML式empty-tag終了は厳密には " />" と 半角スペースを最初に入れる必要がある.
	 * 従って、このscanToDelimiter処理上では、このようなtag終了が登場する可能性を考慮する必要はない.
	 *
	 * では、半角スペースから始まらず単に"/>" とある場合も拡張としてempty-tag終了とみなすかどうかであるが、
	 * 実際には <A HREF=http://info.5ch.net/>という例が存在し、このとき、HREFの値としては http://info.5ch.net/ 
	 * となるべきである. つまり "/>" をtag終端とみなすのはまずい.
	 */
	while( *p ){
		switch( *p ){
		case ' ':
		case '\t':
			*pp = p;
			return true;
		case '>':
			/* tag終了. */
			*pp = p;
			return true;
		default:
			/* 値を構成する文字の一つとみなす */
			ZnkStr_add_c( ans, *p );
			break;
		}
		++p;
	}
	/* tag終了しないうちにNULL終端した. */
	*pp = p;
	return false;
}


/**
 * @brief
 *   varname=varvalという形式のattribute指定を解釈する.
 *   varvalがクォート文字で囲まれている場合はクォート文字も含めて取得される.
 *
 * @return:
 *   tag終了した場合、pp は tag終了文字 '>' を指し示した状態でtrueを返す.
 *   tag終了せずに NUL終端した場合 false を返す.
 */
static bool
scanAttribute( const char** pp, ZnkStr varname, ZnkVarpAry attr_ary )
{
	bool exist_assignment = false;
	const char* p = *pp;
	ZnkVarp attr = NULL;
	ZnkStr varval = NULL;
	char qch = 0;

	/* skip whitespace */
	while( *p && *p == ' ' ){ ++p; }

	if( *p == '>' ){
		/* tag終了 */
		*pp = p;
		return true;
	}

	/* lexical-scan varname */
	ZnkStr_clear( varname );
	while( *p && *p != '=' && *p != ' ' && *p != '>' ){
		ZnkStr_add_c( varname, *p );
		++p;
	}

	/* skip whitespace or assignment */
	while( *p && *p == ' ' ){ ++p; }
	if( *p == '=' ){
		exist_assignment = true;
		++p;
	}
	while( *p && *p == ' ' ){ ++p; }

	if( *p == '\0' ){
		/* tag終了もなしにいきなり文字列が終了している. */
		*pp = p;
		return false;
	}

	if( !exist_assignment ){
		/* varnameという形で値なしにステートメントが終了している.
		 * いわゆる論理属性 :
		 * scriptタグのasync, optionタグのselected, inputタグのchecked
		 * などでこれはあり得る. */
		/* ZnkVarpではInt型としてこれを保持するものとし、指定されている場合その値は1とする */
		*pp = p;
		attr = ZnkVarp_create( "", "", 0, ZnkPrim_e_Int, NULL );
		ZnkStr_swap( attr->name_, varname );
		attr->prim_.u_.i32_ = 1;
		ZnkVarpAry_push_bk( attr_ary, attr );
		return true;
	} else {
		attr = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
		ZnkStr_swap( attr->name_, varname );
		ZnkVarpAry_push_bk( attr_ary, attr );
	}

	/* 属性値あり属性 : その値を解析. */
	/* クォート文字はfilenameに格納するものとする */
	varval = ZnkVar_str( attr );
	switch( *p ){
	case '\'':
	case '"':
		qch = *p;
		EstHtmlAttr_set_quote_char( attr, *p );
		++p;
		if( !scanQuote( &p, qch, varval, true ) ){
			/* unexpected null-terminated */
			return false;
		}
		ZnkStr_cut_back_c( varval ); /* 終了クォートのみカット */
		break;
	case '>':
		/* varname=という形で = が存在するにもかかわらず右辺値の指定がなくtag終了している. */
		/* この場合、一応空文字とみなす */

		/* スペースの場合、クォーティングされていない値の形式であることを示す */
		EstHtmlAttr_set_quote_char( attr, ' ' );
		break;
	default:
		/* lexical-scan varval-end */

		/* スペースの場合、クォーティングされていない値の形式であることを示す */
		EstHtmlAttr_set_quote_char( attr, ' ' );

		if( !scanToDelimiter( &p, varval ) ){
			/* unexpected null-terminated */
			return false;
		}
		break;
	}
	*pp = p;
	return true;
}

static bool
scanComment( const char** pp, ZnkStr tagname, ZnkStr tagend, const char* begin, const char* end )
{
	const char* p = *pp;
	const size_t end_leng = Znk_strlen(end);
	/* comment out of HTML */
	ZnkStr_set( tagname, begin );
	p += Znk_strlen(begin);
	ZnkStr_clear( tagend );
	while( *p ){
		if( ZnkS_isBegin( p, end ) ){
			/* endof comment */
			ZnkStr_add( tagend, end );
			p += end_leng;
			*pp = p;
			return true;
		}
		ZnkStr_add_c( tagend, *p );
		++p;
	}
	*pp = p;
	return false;
}

static bool
scanTag( const char** pp, ZnkStr tagname, ZnkStr varname, ZnkVarpAry attr_ary, ZnkStr tagend )
{
	/* scan tag name */
	bool result = false;
	const char* p = *pp;

	switch( *p ){
	case '!':
		if( ZnkS_isBegin( p, "!--" ) ){
			/* comment out of HTML */
			result = scanComment( &p, tagname, tagend, "!--", "-->" );
		} else if( ZnkS_isBegin( p, "!DOCTYPE" ) ){
			result = scanComment( &p, tagname, tagend, "!DOCTYPE", ">" );
		}
		goto FUNC_END;
	case '?':
		/***
		 * 典型的には<?php .... ?> <?xml ... ?> などが考えられる.
		 */
		result = scanComment( &p, tagname, tagend, "?", "?>" );
		goto FUNC_END;
	case '/':
		/* 終了tagと考えられる. */
		break;
	default:
		if( isalpha( (uint8_t)(*p) ) ){
			/* empty-tagまたは開始tagと考えられる. */
		} else {
			/* 上記とアルファベット以外の記号が来た場合はtagではないとみなし、処理を中止すべき */
			result = false;
			goto FUNC_END;
		}
		break;
	}

	/***
	 * 終了tag(</h1>など)の場合、tagnameとして/の部分も含めて取得するものとする.
	 * またempty-tagとは<br /> のような単独で完結しているタグ指定である.
	 * (大抵の環境では単に<br>でも問題ないが、XHTMLでは <br />のようにするのが正式のようである)
	 */
	ZnkStr_clear( tagname );
	ZnkStr_clear( tagend );
	if( *p == '/' ){
		ZnkStr_add_c( tagname, '/' );
		++p;
	}
	while( *p ){
		switch( *p ){
		case ' ': case '\t': case '\r': case '\n': case '>':
			goto EndOfTagname;
		default:
			break;
		}
		if( ZnkS_isBegin( p, "/>" ) ){ /* endof empty-tag */ 
			ZnkStr_add_c( tagend, '/' );
			++p;
			break;
		}
		if( isalnum( (uint8_t)(*p) ) ){
			ZnkStr_add_c( tagname, *p );
			++p;
		} else {
			/* tagnameではないと考えられる.
			 * この場合ただちに処理を中断する. */
			result = false;
			goto FUNC_END;
		}
	}
EndOfTagname:
	if( ZnkStr_empty(tagname) || ZnkStr_eq(tagname, "/") ){
		/* tagnameではないと考えられる.
		 * この場合ただちに処理を中断する. */
		result = false;
		goto FUNC_END;
	}
	while( *p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' ){ ++p; }
	if( *p == '\0' ){
		/* unexpected null-terminated */
		result = false;
		goto FUNC_END;
	}

	/* attribute指定が存在せずtag終了している場合 */
	if( *p == '>' ){
		ZnkStr_add_c( tagend, '>' );
		++p;
		result = true;
		goto FUNC_END;
	} else if( ZnkS_isBegin( p, "/>" ) ){
		ZnkStr_set( tagend, "/>" );
		p += 2;
		result = true;
		goto FUNC_END;
	}
	
	ZnkStr_clear( tagend );
	ZnkVarpAry_clear( attr_ary );
	while( *p && *p != '>' ){
		const char* q = p;
		if( *p == '<' ){
			/* tagの中にtagが存在.
			 * そもそもこれはtagではなかったと判断すべきで中断 */
			result = false;
			goto FUNC_END;
		}
		if( ZnkS_isBegin( p, "/>" ) ){ /* endof empty-tag */
			ZnkStr_add_c( tagend, '/' );
			++p;
			break;
		}
		if( !scanAttribute( &p, varname, attr_ary ) ){
			/* unexpected null-terminated */
			result = false;
			goto FUNC_END;
		}
		Znk_UNUSED( q );
		assert( q != p );
		while( *p && *p == ' ' ){ ++p; }
	}
	if( *p == '>' ){
		ZnkStr_add_c( tagend, '>' );
		++p;
	}
	result = true;
FUNC_END:

	*pp = p;
	return result;
}

static void
extendAttributeVarpAry( ZnkStr tmp, const ZnkVarpAry attr_ary )
{
	const size_t size = ZnkVarpAry_size( attr_ary );
	size_t       idx;
	char         qch = '\0';
	const char*  val = NULL;
	for( idx=0; idx<size; ++idx ){
		const ZnkVarp attr = ZnkVarpAry_at( attr_ary, idx );
		const bool  is_logical = EstHtmlAttr_isLogical( attr );
		const char* attr_name  = EstHtmlAttr_name(attr);
		ZnkStr_add( tmp, attr_name );
		if( !is_logical ){
			qch = EstHtmlAttr_quote_char( attr );
			ZnkStr_add_c( tmp, '=' );
			val = EstHtmlAttr_val(attr);
			if( qch == ' ' ){
				/* qchがスペースの場合、クォーティングを行わないことを示す */
				ZnkStr_add( tmp, val );
			} else {
				/* qchがスペース以外の場合、その文字でクォーティングを行うことを示す */
				ZnkStr_add_c( tmp, qch );
				ZnkStr_add( tmp, val );
				ZnkStr_add_c( tmp, qch );
			}
		}
		if( idx != size-1 ){
			ZnkStr_add_c( tmp, ' ' );
		}
	}
}

static const char*
dumpAround( const char* p, char* buf, size_t max_size )
{
	ZnkS_copy( buf, max_size, p, Znk_NPOS );
	return buf;
}

static void
invokePlaneTxt( ZnkStr ans, ZnkStr planetxt, const EstParserPlaneTxtProcessFuncT planetxt_event_handler, void* planetxt_event_arg )
{
	if( ZnkStr_leng( planetxt ) ){
		if( planetxt_event_handler ){
			planetxt_event_handler( planetxt, planetxt_event_arg );
		}
		ZnkStr_add( ans, ZnkStr_cstr(planetxt) );
		ZnkStr_clear( planetxt );
	}
}
bool
EstParser_invokeHtmlTagEx( ZnkStr text,
		const EstParserProcessFuncT         tag_event_handler,      void* tag_event_arg,
		const EstParserPlaneTxtProcessFuncT planetxt_event_handler, void* planetxt_event_arg,
		ZnkStr msg )
{
	const char* p = ZnkStr_cstr(text);
	const char* q = NULL;
	char qch    = 0;
	bool result = true;
	char buf[ 32 ] = "";

	size_t line_count = 0;

	/***
	 * ステート:quotable_region
	 * quotable_regionとはquoteで囲まれた領域がクォーティングとして意味を持つ領域であることを示す.
	 * 典型的には<script>...</script>で囲まれた範囲である.
	 * 逆に言えばそれ以外の領域ではscanQuoteとしての意味でその部分を取り込んではならず、
	 * プレーンテキストとして文字通りの意味で取得しなければならない.
	 * 通常、HTML開始時点ではこの性質はOffである.
	 */
	bool quotable_region = false;

	/***
	 * ステート:script_region
	 * script_regionとは<script>...</script>で囲まれた範囲である.
	 * この範囲では<>で囲まれた領域は基本的にTagとしての意味を持たない.
	 * しかしながらscript_regionを終端するためのtagだけは特別に認識しなければならない.
	 * 従って、この領域内でもとりあえずscanTag処理は必要であると考えられる.
	 * いずれの場合にしろansには元々のtagとしての形状が復元展開される.
	 * 唯一の違いは、tagとしての意味を持つ場合のみ tag_event_handler がコールされる必要があることである.
	 * ここで特に注意しなければならないのはこの領域を開始/終了するためのタグ自体はtag_event_handlerがコールされる必要があることである.
	 * 通常、HTML開始時点ではこの性質はOffである.
	 */
	bool script_region = false;

	/***
	 * ステート:ahref_region
	 * ahref_regionとは<a href=...>...</a>で囲まれた範囲である.
	 * この範囲ではさらなるリンクを含めるべきではなく、従ってオートリンク処理においては除外対象とすべきである.
	 * 通常、HTML開始時点ではこの性質はOffである.
	 */
	bool ahref_region = false;

	/***
	 * tag_event_handler がコールされなければならない状態であることを示す.
	 * 通常、HTML開始時点ではこの性質はOffである.
	 */
	bool call_tag_event_handler = false;

	/***
	 * planetxt とは quotable_region, script_region, ahref_region のどれにも該当しない範囲である.
	 * 通常、HTML開始時点ではこの性質はOnである.
	 */

	ZnkStr ans     = ZnkStr_new( "" );
	ZnkStr tagend  = ZnkStr_new( "" );
	ZnkStr tagname = ZnkStr_new( "" );
	ZnkStr varname = ZnkStr_new( "" );
	ZnkVarpAry varp_ary = ZnkVarpAry_create( true );
	ZnkStr planetxt = ZnkStr_new( "" );

	while( *p ){
		switch( *p ){
		case '\'':
		case '"':
			q = p;
			qch = *p;
			++p;
			if( quotable_region ){
				/* 直前で取得したplanetxtが溜まっているならば処理する */
				invokePlaneTxt( ans, planetxt, planetxt_event_handler, planetxt_event_arg );

				/***
				 * ここでは単にクォートで囲まれた領域をそのままansへ転写する.
				 * 一方、planetxt には転写しない.
				 */
				ZnkStr_add_c( ans, *q );
				if( !scanQuote( &p, qch, ans, true ) ){
					/* unexpected null-terminated */
					if( msg ){
						ZnkStr_addf( msg, "EstParser_invokeHtmlTag : Error : scanQuote : line=[%zu] around [%s]\n",
								line_count, dumpAround( q, buf, 8 ) );
					}
					result = false;
					goto FUNC_END;
				}

			} else if( ahref_region ){
				/* 直前で取得したplanetxtが溜まっているならば処理する */
				invokePlaneTxt( ans, planetxt, planetxt_event_handler, planetxt_event_arg );

				/***
				 * ここでは単に<a href...>...</a>で囲まれた領域をそのままansへ転写する.
				 * 一方、planetxt には転写しない.
				 */
				ZnkStr_add_c( ans, *q );

			} else {
				/***
				 * planetxt に転写.
				 * 一方、ans には転写しない.
				 */
				ZnkStr_add_c( planetxt, *q );
			}
			break;
		case '<':
			/* begin of tag scan */
			q = p;
			++p;

			ZnkStr_clear( tagname );
			ZnkVarpAry_clear( varp_ary );
			ZnkStr_clear( tagend );

			if( !scanTag( &p, tagname, varname, varp_ary, tagend ) ){
				/* tagの開始ではないと判断された場合 */
				/* pの位置を < の次の位置へ強制的にRollback */
				if( msg ){
					ZnkStr_addf( msg, "EstParser_invokeHtmlTag : Error : scanTag : line=[%zu] around [%s]\n",
							line_count, dumpAround( q, buf, 8 ) );
				}
				ZnkStr_add_c( planetxt, *q );
				p = q+1; /* Rollback */
				break;
			} else {
				/* tagの開始と判断された場合.
				 * しかし更にscript_regionであるか否かを調べ、その内部であった場合は
				 * tagの開始という判断を却下する.
				 */
				if( script_region ){
					if( ZnkS_eqCase(ZnkStr_cstr(tagname), "/script") ){
						/* scriptタグの終了 */
						/* 直前で取得したplanetxtを一旦処理する(本当はscripttxtとでもすべきか) */
						invokePlaneTxt( ans, planetxt, planetxt_event_handler, planetxt_event_arg );
						ZnkStr_add_c( ans, *q );
					} else {
						/* tag開始の判断を却下 */
						ZnkStr_add_c( planetxt, *q );
						p = q+1; /* Rollback */
						break;
					}
				} else {
					/* 直前で取得したplanetxtを一旦処理する */
					invokePlaneTxt( ans, planetxt, planetxt_event_handler, planetxt_event_arg );
					ZnkStr_add_c( ans, *q );
				}
			}

			/***
			 * 特殊タグの解析とステートの変更.
			 */
			call_tag_event_handler = true;
			if( ZnkS_eqCase(ZnkStr_cstr(tagname), "script") ){
				/***
				 * scriptタグの開始:
				 * script_region内には基本的に任意のパターンが出現し得るとみなすべきである.
				 * ここではステート遷移マシンとしてこれを処理する.
				 *
				 * script_regionの開始と終了を意味するタグ自体のtag_event_handlerは
				 * 実行されなければならない.
				 */
				if( script_region ){
					/***
					 * 既にscript_region内ならば、"<script" はtagとみなすべきではなく、
					 * 従ってcall_tag_event_handlerすべきではない.
					 */
					call_tag_event_handler = false;
				} else {
					quotable_region = true;
					script_region = true;
				}

			} else if( ZnkS_eqCase(ZnkStr_cstr(tagname), "/script") ){
				/* scriptタグの終了 */
				quotable_region = false;
				script_region = false;

			} else if( ZnkS_eqCase(ZnkStr_cstr(tagname), "a") ){
				/***
				 * aタグの開始:
				 * ahref_region内には基本的に任意のパターンが出現し得るとみなすべきである.
				 * ここではステート遷移マシンとしてこれを処理する.
				 */
				ZnkVarp href = ZnkVarpAry_find_byName_literal( varp_ary, "href", true );
				if( href ){
					ahref_region = true;
				} else {
					/* href 属性指定が存在しない場合はahref_regionとはみなさない */
				}
			} else if( ZnkS_eqCase(ZnkStr_cstr(tagname), "/a") ){
				/***
				 * aタグの終了:
				 * とりあえず現時点では、おそらく多くのブラウザでもそうしている通り、
				 * ネストされている異常なケースであっても /a の時点でahref_regionは強制終了するものとする.
				 */
				ahref_region = false;
			}

			/***
			 * call_tag_event_handler:
			 */
			if( call_tag_event_handler ){
				/* invoke event callback. */
				tag_event_handler( tagname, varp_ary, tag_event_arg, tagend );
			}

			ZnkStr_add( ans, ZnkStr_cstr(tagname) );
			if( ZnkVarpAry_size( varp_ary ) ){
				ZnkStr_add_c( ans, ' ' );
				extendAttributeVarpAry( ans, varp_ary );
			}
			ZnkStr_add( ans, ZnkStr_cstr(tagend) );
			break;
		case '\n':
			if( ahref_region ){
				ZnkStr_add_c( ans, *p );
			} else {
				ZnkStr_add_c( planetxt, *p );
			}
			++p;
			++line_count;
			break;
		default:
			if( ahref_region ){
				ZnkStr_add_c( ans, *p );
			} else {
				ZnkStr_add_c( planetxt, *p );
			}
			++p;
			break;
		}
	}
	/* 直前で取得したplanetxtが溜まっているならば処理する */
	invokePlaneTxt( ans, planetxt, planetxt_event_handler, planetxt_event_arg );

	result = true;
	ZnkStr_swap( text, ans );
FUNC_END:
	ZnkStr_delete( ans );
	ZnkStr_delete( tagend );
	ZnkStr_delete( tagname );
	ZnkStr_delete( varname );
	ZnkVarpAry_destroy( varp_ary );
	ZnkStr_delete( planetxt );
	return result;
}

static int
filterTest( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend )
{
	if( ZnkS_eqCase( ZnkStr_cstr(tagname), "a" ) ){
		ZnkVarp varp = ZnkVarpAry_find_byName_literal( varp_ary, "href", true );
		if( varp ){
			ZnkStr str = ZnkVar_str( varp );
			Znk_printf_e( "a: tagend=[%s], href=[%s]\n", ZnkStr_cstr(tagend), ZnkStr_cstr(str) );
		}
	}
	return 0;
}

void
EstParser_test( const char* r_filename, const char* w_filename )
{
	ZnkFile r_fp = NULL;
	ZnkFile w_fp = NULL;
	ZnkStr text = ZnkStr_new( "" );

	r_fp = Znk_fopen( r_filename, "rb" );
	if( r_fp ){
		ZnkStr tmp = ZnkStr_new( "" );

		while( ZnkStrFIO_fgets( tmp, 0, 4096, r_fp ) ){
			ZnkStr_add( text, ZnkStr_cstr(tmp) );
		}
		ZnkStr_delete( tmp );

		EstParser_invokeHtmlTagEx( text, filterTest, NULL, NULL, NULL, NULL );
		Znk_fclose( r_fp );
	}

	w_fp = Znk_fopen( w_filename, "wb" );
	if( w_fp ){
		Znk_fwrite( (uint8_t*)ZnkStr_cstr(text), ZnkStr_leng(text), w_fp );
		Znk_fclose( w_fp );
	}
	ZnkStr_delete( text );
}

