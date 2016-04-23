#include "Moai_info.h"
#include "Moai_log.h"
#include "Moai_parent_proxy.h"
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <string.h>
#include <stdlib.h>

static bool
isRelayToMe( const char* my_hostname, size_t my_hostname_leng,
		uint16_t my_port, const char* host_begin )
{
	if( ZnkS_isBeginEx( host_begin, Znk_NPOS, my_hostname, my_hostname_leng ) ){
		if( host_begin[ my_hostname_leng ] == ':' ){
			uint16_t dst_port = 80; /* default HTTP port */
			ZnkS_getU16U( &dst_port, host_begin+my_hostname_leng+1 );
			if( dst_port == my_port ){
				/***
				 * BrowserはこのプログラムをLocalProxyとして使っているにも関わらず
				 * その接続先は再びlocalなこのプログラム(WebServerとして接続)である.
				 * この場合、LocalProxyとしての中継は無駄であるので、
				 * WebServerとしての接続に矯正すべきであろう.
				 */
				return true;
			}
		}
	}
	return false;
}


void
MoaiInfo_parseHdr( MoaiInfo* info, MoaiBodyInfo* body_info,
		ZnkMyf config, bool* as_local_proxy, uint16_t my_port, bool is_request,
		const char* response_hostname,
		MoaiModuleAry mod_ary, ZnkMyf mtgt, ZnkStr req_urp )
{
	size_t key_begin; size_t key_end;
	size_t val_begin; size_t val_end;
	const char* key = "";
	const char* val = "";
	ZnkVarp varp = NULL;
	const char* line_begin;
	const char* line_end;
	const char* p;
	const char* q;
	size_t arg_idx = 0;
	ZnkHtpHdrs hdrs   = &info->hdrs_;
	ZnkBfr     stream = info->stream_;

	/* first line */
	line_begin = (char*)ZnkBfr_data( stream );

	p = line_begin;
	while( *p != '\r' ){
		while( *p == ' ' || *p == '\t' ) ++p;
		q = p;
		if( arg_idx < 2 ){
			while( *q != ' ' && *q != '\t' ) ++q;
		} else {
			while( *q != '\r' ) ++q;
		}
		ZnkStrDAry_push_bk_cstr( hdrs->hdr1st_, p, q-p );
		p = q;
		++arg_idx;
	}

	line_begin = p + 2;

	/***
	 * status line がRequest Lineの場合、
	 * 第２引数はRequest-URIとなる.
	 */
	if( is_request ){
		ZnkStr str = ZnkStrDAry_at( hdrs->hdr1st_, 1 );
		const char* host_begin = NULL;

		/***
		 * ここではホスト名から始まる場合は、UserAgentはLocalProxyとして
		 * このプログラムに接続しようとしたものとみなし、/ から始まる場合は、
		 * WebServerとしてこのプログラムに接続しようとしたものとみなす.
		 *
		 * 尚、Host名として指定されている値が localhost:Pや127.0.0.1:P
		 * (Pはこのプログラムが使っているport番号)であるか否かを調べれば、
		 * この種の判定ができそうに思えるかもしれないが、localhostを示す
		 * Host名がそれ以外の文字列である場合も有り得るため、これでは不完全である.
		 * 例えば/etc/hostsに 127.0.0.1 の別名を localhost以外にも指定していたり、
		 * 物理的に別のマシンからこのプログラムが起動しているマシンへ
		 * IPアドレスを指定して接続した場合(そのIPアドレスは異なるため当然であるが)
		 * などでこれら以外の文字列に成り得る.
		 */
		if( ZnkS_isBegin_literal( ZnkStr_cstr(str), "http://" ) ){
			/* http://から始まる場合. */
			host_begin = ZnkStr_cstr( str ) + Znk_strlen_literal( "http://" );
			/* 本件はLocalProxyとして受理 */
			*as_local_proxy = true;
		} else if( ZnkStr_first( str ) == '/' ){
			/* / から始まる場合. */
			host_begin = NULL;
			/* 本件はWebServerとして受理 */
			*as_local_proxy = false;
			ZnkStr_set( req_urp, ZnkStr_cstr(str) );
		} else if( ZnkStr_first( str ) == '*' ){
			/**
			 * OPTIONS methodなどの場合、この値になっているケースが有り得る.
			 * とりあえず現時点ではLocalProxyとして処理.
			 */
			host_begin = NULL;
			/* 本件はLocalProxyとして受理 */
			*as_local_proxy = true;
		} else {
			/* Host名から始まる場合. */
			host_begin = ZnkStr_cstr( str );
			/* 本件はLocalProxyとして受理 */
			*as_local_proxy = true;
		}

		if( host_begin ){
			ZnkSRef sref = { 0 };
			ZnkSRef_set_literal( &sref, "localhost" );
			if( isRelayToMe( sref.cstr_, sref.leng_, my_port, host_begin ) ){
				/* 本件はWebServerとしての受理に矯正 */
				*as_local_proxy = false;
			}
			ZnkSRef_set_literal( &sref, "127.0.0.1" );
			if( isRelayToMe( sref.cstr_, sref.leng_, my_port, host_begin ) ){
				/* 本件はWebServerとしての受理に矯正 */
				*as_local_proxy = false;
			}

			/***
			 * 最前にあるHost名の部分は除去しなければならない.
			 * ホスト名が含まれていてもうまく表示されるサイトもあるが、そうならないサイトがある.
			 * 例えば www.nicovideo.jpなどは、この部分にhost名があることを想定しておらず
			 * Redirect用のLocationを構成して返してくる. 具体的には以下のような流れになる.
			 *
			 * 1. GET http://www.nicovideo.jp/watch/sm0123456 HTTP/1.1 でRequestしたとする.
			 * 2. nicovideoのWebサーバは、上記第2引数を / より始まるfilepathであるとみなす.
			 *    即ちまず最初に / が現れる場所を検索し、その / の次から始まる文字列を
			 *    filepath2 とした場合、Hostディレクティブに指定された文字列 + / + filepath2
			 *    という文字列を作ってそれをリダイレクト用のLocationディレクティブに設定したものを
			 *    Responseとして返す.
			 * 3. 結果的にユーザ側のBrowserは GET www.nicovideo.jphttp/www.nicovideo.jp/watch/sm0123456 HTTP/1.1
			 *    なるRequestを次回に発行してしまう.
			 *    これは当然、www.nicovideo.jphttp というホストへの接続とみなされNotFoundになる.
			 *
			 * また逆に接続先としてさらに別のproxyを介す場合は、
			 * 当然ながら除去は行ってはならない.
			 */
			{
				/***
				 * TODO:
				 * この処理はこの関数の外へ追い出す方が望ましい.
				 */
				const char* end = strchr( host_begin, '/' );
				if( end == NULL ){
					end = host_begin + Znk_strlen( host_begin );
				}
				/***
				 * ZnkStr_eraseする前に済ませておく.
				 */
				ZnkStr_assign( req_urp, 0, end, Znk_NPOS );
				{
					size_t hostname_leng = end - host_begin;
					const bool use_parent_proxy = MoaiParentProxy_isUse( config, host_begin, hostname_leng );
					if( !use_parent_proxy ){
						size_t cut_size = end - ZnkStr_cstr( str );
						ZnkStr_erase( str, 0, cut_size );
					}
				}
			}
		}
	}

	while( line_begin - (char*)ZnkBfr_data( stream ) < (int)info->hdr_size_ ){
		line_end = Znk_memstr_literal( (uint8_t*)line_begin, info->hdr_size_ - ((uint8_t*)line_begin-ZnkBfr_data(stream)), "\r\n" );
		if( line_end == line_begin ){
			/* end of header */
			break;
		}
		ZnkS_find_key_and_val( line_begin, 0, line_end-line_begin,
				&key_begin, &key_end,
				&val_begin, &val_end,
				":", 1,
				" \t", 2 );
		if( key_begin == Znk_NPOS || key_end == Znk_NPOS || val_begin == Znk_NPOS ){
			MoaiLog_printf( "  MoaiInfo_parseHdr : Error : key_begin=[%u] key_end=[%u] val_begin=[%u]\n",
					key_begin, key_end, val_begin );
			break;
		}
		varp = ZnkHtpHdrs_regist( hdrs->vars_,
				line_begin+key_begin, key_end-key_begin,
				line_begin+val_begin, val_end-val_begin );
		if( varp == NULL ){
			/* memory error */
			exit( EXIT_FAILURE );
		}
		key = ZnkHtpHdrs_key_cstr( varp );
		val = ZnkHtpHdrs_val_cstr( varp, 0 );

		if( ZnkS_eqCase( key, "Referer" ) ){
			ZnkStr str = ZnkHtpHdrs_val( varp, 0 );
			MoaiLog_printf( "Referer=[%s]\n", ZnkStr_cstr( str ) );
			if( strstr( ZnkStr_cstr( str ), "web_start_page" ) ){
				assert( 0 );
			}
		}
		if( ZnkS_eqCase( key, "Transfer-Encoding" ) ){
			if( ZnkS_eq( val, "chunked" ) ){
				body_info->is_chunked_ = true;
			}
		}
		if( ZnkS_eqCase( key, "Content-Encoding" ) ){
			if( ZnkS_eq( val, "gzip" ) ){
				body_info->is_gzip_ = true;
			}
		}
		if( ZnkS_eqCase( key, "Content-Type" ) ){
			if( ZnkS_isBegin( val, "text/html" ) ){
				body_info->txt_type_ = MoaiText_e_HTML;
				MoaiLog_printf( "  MoaiInfo_parseHdr : txt_type=HTML\n" );
			} else if(
			    ZnkS_isBegin( val, "text/css" )
			){
				body_info->txt_type_ = MoaiText_e_CSS;
				MoaiLog_printf( "  MoaiInfo_parseHdr : txt_type=CSS\n" );
			} else if(
			    ZnkS_isBegin( val, "application/javascript" )
			 || ZnkS_isBegin( val, "application/x-javascript" )
			 || ZnkS_isBegin( val, "text/javascript" )
			){
				body_info->txt_type_ = MoaiText_e_JS;
				MoaiLog_printf( "  MoaiInfo_parseHdr : txt_type=JS\n" );
			} else {
				body_info->txt_type_ = MoaiText_e_Binary;
				MoaiLog_printf( "  MoaiInfo_parseHdr : txt_type=Binary\n" );
			}
		}
		if( ZnkS_eqCase( key, "Content-Length" ) ){
			ZnkS_getSzU( &body_info->content_length_, val );
		}
		line_begin = line_end + 2;
	}

	{
		const char* hostname = NULL;
		if( is_request ){
			varp = ZnkHtpHdrs_find_literal( hdrs->vars_, "Host" );
			if( varp ){
				hostname = ZnkHtpHdrs_val_cstr( varp, 0 );
			}
		} else {
			hostname = response_hostname;
		}
		if( hostname ){
			MoaiModule mod = MoaiModuleAry_find_byHostname( mod_ary, mtgt, hostname );
			if( mod ){
				MoaiModule_filtHtpHeader( mod, hdrs->vars_ );
			}
		}
	}

	if( !body_info->is_chunked_ && body_info->content_length_ == 0 ){
		/***
		 * chunkedかつContent-Lengthがどちらも指定されていないにも関わらず
		 * body部と思わしき余剰が存在する場合.
		 * 残念ながら、このような行儀の悪いサーバも存在する.
		 * 特にProxyなどに見られる.
		 */
		if( line_begin - (char*)ZnkBfr_data( stream ) + 2 < (int)ZnkBfr_size( stream ) ){
			body_info->is_unlimited_ = true;
		}
	}
}
