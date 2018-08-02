#include <Est_link.h>
#include <Est_parser.h>
#include <Est_config.h>
#include <Znk_mem_find.h>
#include <Znk_str_ex.h>
#include <Znk_missing_libc.h>
#include <Znk_htp_util.h>
#include <Znk_dir.h>
#include <Znk_s_base.h>
#include <Rano_cgi_util.h>
#include <Rano_htp_boy.h>
#include <Rano_log.h>
#include <string.h>
#include <ctype.h>


const char*
EstLink_findTargetName( const ZnkMyf mtgt, const char* line )
{
	const size_t  size = ZnkMyf_numOfSection( mtgt );
	size_t        idx;
	ZnkMyfSection sec;
	ZnkStrAry     lines;
	for( idx=0; idx<size; ++idx ){
		sec   = ZnkMyf_atSection( mtgt, idx );
		lines = ZnkMyfSection_lines( sec ); 
		if( ZnkStrAry_find_isMatch( lines, 0, line, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
			/* found */
			return ZnkMyfSection_name( sec );
		}
	}
	/* not found */
	return NULL;
}

int
EstLink_filterImg( ZnkStr str, struct EstLinkInfo* link_info )
{
	int result = 0;
	char buf[ 1024 ] = "";
	const char* p      = ZnkStr_cstr( str );
	const char* q      = NULL;
	const char* target = NULL;
	bool use_cache      = false;
	bool protocol_exist = false;
	ZnkStr esc_str         = ZnkStr_new( "" );
	ZnkStr src_prefix      = ZnkStr_new( "" );
	ZnkStr result_filename = ZnkStr_new( "" );
	const char* xhr_dmz = EstConfig_XhrDMZ();

	q = RanoCGIUtil_skipProtocolPrefix( p );
	if( q > p ){
		/***
		 * このケースではsrcに必ずホスト名を含む.
		 */
		const char* r;
		char hostname[ 1024 ] = "";
		protocol_exist = true;
		r = strchr( q, '/' );
		if( r ){
			ZnkS_copy( hostname, sizeof(hostname), q, r-q );
			ZnkStr_assign( src_prefix, 0, p, r-p );
		} else {
			ZnkS_copy( hostname, sizeof(hostname), q, Znk_NPOS );
			ZnkStr_assign( src_prefix, 0, p, Znk_NPOS );
		}
		target = EstLink_findTargetName( link_info->mtgt_, hostname );
		if( target == NULL ){
			/* targetに存在しないlinkの場合は何も加工しない */
			goto FUNC_END;
		} else {
			if( r ){
				/* str には req_urp が残る. */
				ZnkStr_erase( str, 0, r-p );
			} else {
				ZnkStr_clear( str );
			}
		}
	} else {
		/***
		 * strが http:// などのプロトコル指定で始まっていない場合.
		 * / で始まるリンク:
		 *   str には req_urpだけが指定されているケースと考えられる.
		 *   hostnameをsrc_prefixとする
		 * / で始まらないリンク:
		 *   str は プロトコル指定のないフルパス、または相対パスだけが指定されているケースと考えられる.
		 *   (しかしそのどちらであるかを一般的に判断するのは難しい)
		 *   base_href_をsrc_prefixとする.
		 */
		target = EstLink_findTargetName( link_info->mtgt_, ZnkStr_cstr(link_info->hostname_) );
		if( *p == '/' ){
			ZnkStr_set( src_prefix, ZnkStr_cstr(link_info->hostname_) );
			//target = EstLink_findTargetName( link_info->mtgt_, ZnkStr_cstr(link_info->hostname_) );
		} else {
			/***
			 * hostnameで開始していないパターンのみbase_hrefを使う.
			 * そうでない場合はsrc_prefixは空にしておく.
			 */
			if( ZnkStr_isBegin( str, ZnkStr_cstr(link_info->hostname_) ) ){
				ZnkStr_clear( src_prefix );
			} else {
				ZnkStr_set( src_prefix, ZnkStr_cstr(link_info->base_href_) );
			}
		}
	}
	/***
	 * 以後、target の範囲内にあるものであり、srcはreq_urpを意味する.
	 */
	ZnkStrEx_replace_BF( str, 0, "&amp;", Znk_strlen_literal("&amp;"), "&", 1,
			Znk_NPOS, Znk_NPOS );

	if( target ){
		const char* cachebox = "cachebox/";
		const char* unesc_req_urp = ZnkStr_cstr(str);
		RanoHtpBoy_getResultFile( ZnkStr_cstr(link_info->hostname_), unesc_req_urp, result_filename, cachebox, target );
		if( ZnkDir_getType( ZnkStr_cstr(result_filename) ) == ZnkDirType_e_File ){ 
			/* キャッシュファイルが存在 */
			use_cache = true;
		}
	}

	if( use_cache ){
		Znk_snprintf( buf, sizeof(buf),
				"http://%s/cgis/easter/%s",
				xhr_dmz,
				ZnkStr_cstr(result_filename) );
		ZnkStr_set( str, buf );
	} else {
		if( link_info->img_link_direct_ ){
			/***
			 * imgタグによりフルパスで指定する場合は必ずプロトコル指定が必要.
			 */
			Znk_snprintf( buf, sizeof(buf),
					"%s%s",
					protocol_exist ? "" : "//",
					ZnkStr_cstr(src_prefix) );

		} else {
			Znk_snprintf( buf, sizeof(buf),
					"/easter?"
					"est_%s=%s",
					EstRequest_getCStr( link_info->est_req_ ),
					ZnkStr_cstr(src_prefix) );
		
		}
		
		/* escapeしない */
		//ZnkHtpURL_escape( esc_str, (uint8_t*)ZnkStr_cstr(str), ZnkStr_leng(str) );
		//ZnkStr_swap( str, esc_str );

		ZnkStr_insert( str, 0, buf, Znk_NPOS );
	}

	result = 1;
FUNC_END:
	ZnkStr_delete( esc_str );
	ZnkStr_delete( src_prefix );
	ZnkStr_delete( result_filename );
	return result;
}

int
EstLink_filterScript( ZnkStr str, struct EstLinkInfo* link_info )
{
	int result = 0;
	char buf[ 1024 ] = "";
	const char* p      = ZnkStr_cstr( str );
	const char* q      = NULL;
	const char* target = NULL;
	ZnkStr src_prefix      = ZnkStr_new( "" );
	char alter_path[ 256 ] = "";
	bool use_alternative   = false;
	bool protocol_exist = false;
	const char* xhr_auth_host = EstConfig_XhrAuthHost();
	char hostname[ 1024 ] = "";

	q = RanoCGIUtil_skipProtocolPrefix( p );
	if( q > p ){
		/***
		 * このケースではsrcに必ずホスト名を含む.
		 */
		const char* r;
		protocol_exist = true;
		r = strchr( q, '/' );
		if( r ){
			ZnkS_copy( hostname, sizeof(hostname), q, r-q );
			ZnkStr_assign( src_prefix, 0, p, r-p );
		} else {
			ZnkS_copy( hostname, sizeof(hostname), q, Znk_NPOS );
			ZnkStr_assign( src_prefix, 0, p, Znk_NPOS );
		}
		target = EstLink_findTargetName( link_info->mtgt_, hostname );
		if( target == NULL ){
			/* targetに存在しないlinkの場合は例外を除き問答無用で消去する */
			/* allow_js_hosts はその例外リストである. */
			ZnkStrAry allow_js_hosts = EstConfig_AllowJSHosts();
			ZnkStrAry_find( allow_js_hosts, 0, hostname, Znk_NPOS );
			if( ZnkStrAry_find_isMatch( allow_js_hosts, 0, hostname, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
				goto FUNC_END; /* 例外 */
			}
			ZnkStr_clear( str );
			goto FUNC_END;
		} else {
			if( r ){
				/* str には req_urp が残る. */
				ZnkStr_erase( str, 0, r-p );
			} else {
				ZnkStr_clear( str );
			}
		}
	} else {
		/***
		 * strが http:// などのプロトコル指定で始まっていない場合.
		 * / で始まるリンク:
		 *   str には req_urpだけが指定されているケースと考えられる.
		 *   hostnameをsrc_prefixとする
		 * / で始まらないリンク:
		 *   str は プロトコル指定のないフルパス、または相対パスだけが指定されているケースと考えられる.
		 *   (しかしそのどちらであるかを一般的に判断するのは難しい)
		 *   base_href_をsrc_prefixとする.
		 */
		target = EstLink_findTargetName( link_info->mtgt_, ZnkStr_cstr(link_info->hostname_) );
		ZnkS_copy( hostname, sizeof(hostname), ZnkStr_cstr(link_info->hostname_), Znk_NPOS );
		if( *p == '/' ){
			ZnkStr_set( src_prefix, ZnkStr_cstr(link_info->hostname_) );
		} else {
			/***
			 * hostnameで開始していないパターンのみbase_hrefを使う.
			 * そうでない場合はsrc_prefixは空にしておく.
			 */
			if( ZnkStr_isBegin( str, ZnkStr_cstr(link_info->hostname_) ) ){
				ZnkStr_clear( src_prefix );
			} else {
				ZnkStr_set( src_prefix, ZnkStr_cstr(link_info->base_href_) );
			}
		}
	}
	/***
	 * 以後、target の範囲内にあるものであり、srcはreq_urpを意味する.
	 */
	ZnkStrEx_replace_BF( str, 0, "&amp;", Znk_strlen_literal("&amp;"), "&", 1,
			Znk_NPOS, Znk_NPOS );

	/***
	 * 代替ファイルという概念を導入.
	 * それが存在している場合は代替スクリプトを実行.
	 */
	if( target ){
		const char* unesc_req_urp = ZnkStr_cstr(str);
		const char* r = Znk_strchr( unesc_req_urp, '?' );
		Znk_snprintf( alter_path, sizeof( alter_path ), "publicbox/alternative/%s/%s", target, hostname );
		if( r ){
			ZnkS_concatEx( alter_path, sizeof( alter_path ), unesc_req_urp, r-unesc_req_urp );
		} else {
			ZnkS_concat( alter_path, sizeof( alter_path ), unesc_req_urp );
		}
		if( ZnkDir_getType( alter_path ) == ZnkDirType_e_File ){ 
			/* 代替ファイルが存在 */
			use_alternative = true;
		}
	}

	if( use_alternative ){
		/***
		 * これはDMZ側でもよい気がするが、とりあえず...
		 */
		Znk_snprintf( buf, sizeof(buf),
				"http://%s/cgis/easter/%s",
				xhr_auth_host,
				alter_path );
		ZnkStr_set( str, buf );
	} else {
		/***
		 * サイトからダイレクトに取得するようにフルパスで指定する.
		 * この場合は必ずプロトコル指定が必要.
		 * Same-Origin Policy の観点から見てもこちらの方が望ましい.
		 */
		Znk_snprintf( buf, sizeof(buf),
				"%s%s",
				protocol_exist ? "" : "//",
				ZnkStr_cstr(src_prefix) );

		ZnkStr_insert( str, 0, buf, Znk_NPOS );
	}

FUNC_END:
	ZnkStr_delete( src_prefix );
	return result;
}


int
EstLink_filterLink( ZnkStr str, struct EstLinkInfo* link_info, EstLinkXhr link_xhr )
{
	int result = 0;
	char buf[ 1024 ] = "";
	const char* p = ZnkStr_cstr( str );
	const char* q = NULL;
	ZnkStr esc_str    = ZnkStr_new( "" );
	ZnkStr src_prefix = ZnkStr_new( "" );
	const char* target = NULL;
	const char* xhr_auth_host = EstConfig_XhrAuthHost();
	const char* xhr_dmz       = EstConfig_XhrDMZ();

	q = RanoCGIUtil_skipProtocolPrefix( p );
	if( ZnkS_isBegin( p, "javascript:" ) ){
		/* Javascript関数の呼び出しであるとみなし、何もしない */
		goto FUNC_END;
	}
	if( ZnkS_isBegin( p, "mailto:" ) ){
		/* Mailtoの呼び出しであるとみなし、何もしない */
		goto FUNC_END;
	}
	if( ZnkS_isBegin( p, "#" ) ){
		/* # での同ページ参照の場合、何もしない */
		goto FUNC_END;
	}

	if( q > p ){
		/***
		 * このケースではstrに必ずホスト名を含む.
		 */
		const char* r;
		char hostname[ 1024 ] = "";

		if( ZnkS_isBegin( p, "http://" ) ){
			ZnkStr_set( src_prefix, "http://" );
		} else if( ZnkS_isBegin( p, "https://" ) ){
			ZnkStr_set( src_prefix, "https://" );
		} else {
			/* // の場合. is_https_parent_の設定を受け継ぐ */
			if( link_info->is_https_parent_ ){
				ZnkStr_set( src_prefix, "https://" );
			} else {
				ZnkStr_set( src_prefix, "http://" );
			}
		}

		r = strchr( q, '/' );
		if( r ){
			ZnkS_copy( hostname, sizeof(hostname), q, r-q );
			ZnkStr_append( src_prefix, q, r-q );
		} else {
			ZnkS_copy( hostname, sizeof(hostname), q, Znk_NPOS );
			ZnkStr_append( src_prefix, q, Znk_NPOS );
		}
		target = EstLink_findTargetName( link_info->mtgt_, hostname );
		if( target == NULL ){
			/* targetに存在しないlinkの場合は何も加工しない */
			goto FUNC_END;
		} else {
			if( r ){
				/* str には req_urp が残る. */
				ZnkStr_erase( str, 0, r-p );
			} else {
				ZnkStr_clear( str );
			}
		}
	} else {
		/***
		 * strが http:// などのプロトコル指定で始まっていない場合.
		 * / で始まるリンク:
		 *   str には req_urpだけが指定されているケースと考えられる.
		 *   hostnameをsrc_prefixとする
		 * / で始まらないリンク:
		 *   str は プロトコル指定のないフルパス、または相対パスだけが指定されているケースと考えられる.
		 *   (しかしそのどちらであるかを一般的に判断するのは難しい)
		 *   base_href_をsrc_prefixとする.
		 */
		if( *p == '/' ){
			/* is_https_parent_の設定を受け継ぐ */
			if( link_info->is_https_parent_ ){
				ZnkStr_set( src_prefix, "https://" );
			}
			ZnkStr_add( src_prefix, ZnkStr_cstr(link_info->hostname_) );
		} else {
			/***
			 * hostnameで開始していないパターンのみbase_hrefを使う.
			 * そうでない場合はsrc_prefixは空にしておく.
			 */
			if( ZnkStr_isBegin( str, ZnkStr_cstr(link_info->hostname_) ) ){
				ZnkStr_clear( src_prefix );
			} else {
				/* is_https_parent_の設定を受け継ぐ */
				if( link_info->is_https_parent_ ){
					if(  ZnkStr_isBegin( link_info->base_href_, "http://" ) 
					  || ZnkStr_isBegin( link_info->base_href_, "https://" ) )
					{
						/* base_href_に明示的にプロトコル指定されている場合はそれに従う */
						ZnkStr_set( src_prefix, ZnkStr_cstr(link_info->base_href_) );
					} else if( ZnkStr_isBegin( link_info->base_href_, "//" ) ){
						/* base_href_に//指定されている場合はそれを除去してhttps://へ */
						ZnkStr_setf( src_prefix, "https://%s", ZnkStr_cstr(link_info->base_href_)+Znk_strlen_literal("//") );
					} else {
						/* base_href_にプロトコル指定されていない場合. */
						if( ZnkStr_first( link_info->base_href_ ) == '/' ){
							ZnkStr_setf( src_prefix, "https://%s%s",
									ZnkStr_cstr(link_info->hostname_), ZnkStr_cstr(link_info->base_href_) );
						} else {
							/* 特別措置 */
							if( ZnkStr_isBegin( link_info->base_href_, ZnkStr_cstr(link_info->hostname_) ) ){
								/* base_href_がhostname_から始まっている場合 */
								ZnkStr_setf( src_prefix, "https://%s", ZnkStr_cstr(link_info->base_href_) );
							} else {
								/* base_href_がhostname_から始まっていない場合 */
								ZnkStr_setf( src_prefix, "https://%s/%s",
										ZnkStr_cstr(link_info->hostname_), ZnkStr_cstr(link_info->base_href_) );
							}
						}
					}
				} else {
					ZnkStr_set( src_prefix, ZnkStr_cstr(link_info->base_href_) );
				}
			}
		}
	}
	/***
	 * 以後、target の範囲内にあるものであり、strはreq_urpを意味する.
	 */
	ZnkStrEx_replace_BF( str, 0, "&amp;", Znk_strlen_literal("&amp;"), "&", 1,
			Znk_NPOS, Znk_NPOS );

	{
		char easter_prefix[ 1024 ] = "/easter?";

		switch( link_xhr ){
		case EstLinkXhr_e_ImplicitDMZ:
			ZnkS_copy( easter_prefix, sizeof(easter_prefix), "/easter?", Znk_NPOS );
			break;
		case EstLinkXhr_e_ImplicitAuth:
			ZnkS_copy( easter_prefix, sizeof(easter_prefix), "/easter?", Znk_NPOS );
			break;
		case EstLinkXhr_e_ExplicitDMZ:
			Znk_snprintf( easter_prefix, sizeof(easter_prefix), "http://%s/easter?", xhr_dmz );
			break;
		case EstLinkXhr_e_ExplicitAuth:
			Znk_snprintf( easter_prefix, sizeof(easter_prefix), "http://%s/easter?", xhr_auth_host );
			break;
		default:
			break;
		}

		Znk_snprintf( buf, sizeof(buf),
				"%s"
				"est_%s=%s",
				easter_prefix,
				EstRequest_getCStr( link_info->est_req_ ), ZnkStr_cstr(src_prefix) );
	}

	ZnkHtpURL_escape_withoutSlash( esc_str, (uint8_t*)ZnkStr_cstr(str), ZnkStr_leng(str) );
	ZnkStr_swap( str, esc_str );

	ZnkStr_insert( str, 0, buf, Znk_NPOS );

FUNC_END:
	ZnkStr_delete( esc_str );
	ZnkStr_delete( src_prefix );
	return result;
}

bool
EstLink_filterOnclickJSCall( ZnkVarpAry varp_ary, ZnkVarpAry onclick_jscall )
{
	ZnkVarp      class   = ZnkVarpAry_find_byName_literal( varp_ary, "class",   true );
	ZnkVarp      href    = ZnkVarpAry_find_byName_literal( varp_ary, "href",    true );
	ZnkVarp      onclick = ZnkVarpAry_find_byName_literal( varp_ary, "onclick", true );
	const size_t size  = ZnkVarpAry_size( onclick_jscall );
	size_t       idx;
	ZnkVarp      varp    = NULL;
	ZnkStrAry    filter  = NULL;
	const char*  query_class_name = NULL;
	const char*  cmd = NULL;

	if( class == NULL || href == NULL || onclick == NULL 
	  || !ZnkS_isBegin( EstHtmlAttr_val(href), "javascript:" ) )
	{
		return false;
	}

	for( idx=0; idx<size; ++idx ){
		varp = ZnkVarpAry_at( onclick_jscall, idx );
		query_class_name = ZnkVar_name_cstr( varp );
		if( !ZnkS_eq( EstHtmlAttr_val(class), query_class_name ) ){
			continue;
		}
		filter = ZnkVar_str_ary( varp );
		if( ZnkStrAry_size(filter) == 0 ){
			continue;
		}
		cmd = ZnkStrAry_at_cstr( filter, 0 );
		if( ZnkS_eq( cmd, "replace" ) && ZnkStrAry_size(filter) >= 3 ){
			const char* old_ptn = ZnkStrAry_at_cstr( filter, 1 );
			const char* new_ptn = ZnkStrAry_at_cstr( filter, 2 );
			ZnkStr str = EstHtmlAttr_str( onclick );
			ZnkStrEx_replace_BF( str, 0, old_ptn, Znk_strlen(old_ptn), new_ptn, Znk_strlen(new_ptn),
					Znk_NPOS, Znk_NPOS ); 
			return true;
		}
	}

	return false;
}


