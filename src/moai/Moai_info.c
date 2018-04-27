#include "Moai_info.h"
#include "Moai_server_info.h"

#include <Rano_log.h>
#include <Rano_parent_proxy.h>
#include <Rano_myf.h>

#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <Znk_htp_post.h>
#include <Znk_net_ip.h>

#include <string.h>
#include <stdlib.h>
#include <time.h>


static ZnkObjAry st_moai_info_ary;

static void
deleteMoaiInfo( void* elem )
{
	if( elem ){
		MoaiInfo* info = (MoaiInfo*)elem;
		ZnkHtpHdrs_dispose( &info->hdrs_ );
		ZnkVarpAry_destroy( info->vars_ );
		ZnkBfr_destroy( info->stream_ );
		ZnkStr_delete( info->req_urp_ );
		ZnkStr_delete( info->proxy_hostname_ );
		Znk_free( info );
	}
}

static ZnkObj
allocMoaiInfo( void* arg )
{
	MoaiInfo* info = (MoaiInfo*)Znk_alloc0( sizeof( MoaiInfo ) );
	ZnkHtpHdrs_compose( &info->hdrs_ );
	info->vars_   = ZnkVarpAry_create( true );
	info->stream_ = ZnkBfr_create_null();
	info->req_method_     = ZnkHtpReqMethod_e_Unknown;
	info->req_urp_        = ZnkStr_new( "" );
	info->proxy_hostname_ = ZnkStr_new( "" );
	return (ZnkObj)info;
}

void
MoaiInfo_initiate( void )
{
	if( st_moai_info_ary == NULL ){
		st_moai_info_ary = ZnkObjAry_create( deleteMoaiInfo );
	}
}

static void
copyPostVars( ZnkVarpAry dst_vars, const ZnkVarpAry src_vars )
{
	const size_t size = ZnkVarpAry_size( src_vars );
	size_t idx;
	ZnkVarp src_varp;
	ZnkVarp dst_varp;

	ZnkVarpAry_clear( dst_vars );
	for( idx=0; idx<size; ++idx ){
		src_varp = ZnkVarpAry_at( src_vars, idx );
		dst_varp = ZnkVarp_create( "", "", (int)src_varp->type_, ZnkPrim_type(&src_varp->prim_), ZnkPrim_get_deleter(&src_varp->prim_) ); 
		ZnkVarpAry_push_bk( dst_vars, dst_varp );
		ZnkStr_set( dst_varp->name_, ZnkStr_cstr(src_varp->name_) );
		ZnkStr_set( ZnkVar_misc(dst_varp), ZnkStr_cstr(ZnkVar_misc(src_varp)) );
		if( src_varp->type_ == ZnkHtpPostVar_e_BinaryData ){
			ZnkBfr_set_dfr( dst_varp->prim_.u_.bfr_,
					ZnkBfr_data(src_varp->prim_.u_.bfr_), ZnkBfr_size(src_varp->prim_.u_.bfr_) );
		} else {
			ZnkStr_set( dst_varp->prim_.u_.str_, ZnkStr_cstr(src_varp->prim_.u_.str_) );
		}
	}
}

static MoaiInfoID
issueNewID( void )
{
	static uint32_t st_next_seq = 0;
	time_t t;
	uint32_t t32;
	uint64_t ans = 0;

	/***
	 * 単にシーケンシャルな整数のみならず時刻に関する値も埋め込む.
	 * これにより、前回Moai起動時に発行されたIDが、次回起動時のMoaiに
	 * 万が一渡された場合でも、IDが被るのを確実に防ぐことができる.
	 *
	 * また、一方でシーケンシャルな整数を含めるのは、ミリ秒単位で
	 * 発行が集中した場合でもIDが被るのを確実に防ぐためである.
	 */
	time(&t); /* 現在時刻の取得 */
	t32 = (uint32_t)t;
	ans = t32;
	ans <<= 32;
	ans |= st_next_seq;
	++st_next_seq;

	return (MoaiInfoID)ans;
}

MoaiInfoID
MoaiInfo_regist( const MoaiInfo* info )
{
	size_t ans_idx  = Znk_NPOS;
	bool   is_newly = false;
	MoaiInfo* new_info = NULL;

	MoaiInfo_initiate();
	new_info = (MoaiInfo*)ZnkObjAry_intern( st_moai_info_ary, &ans_idx,
			NULL, NULL,
			allocMoaiInfo, NULL,
			&is_newly );

	new_info->id_ = issueNewID();

	/* deep-copy all members except id */
	if( info ){
		/* deep-copy hdrs */
		ZnkStrAry_copy( new_info->hdrs_.hdr1st_, info->hdrs_.hdr1st_ );
		ZnkHtpHdrs_copyVars( new_info->hdrs_.vars_, info->hdrs_.vars_ );
	
		/* deep-copy POST vars */
		copyPostVars( new_info->vars_, info->vars_ );
	
		/* deep-copy stream */
		ZnkBfr_set_dfr( new_info->stream_,
				ZnkBfr_data(info->stream_), ZnkBfr_size(info->stream_) );
	
		/* deep-copy others */
		new_info->hdr_size_   = info->hdr_size_;
		ZnkStr_set( new_info->req_urp_,        ZnkStr_cstr(info->req_urp_) );
		ZnkStr_set( new_info->proxy_hostname_, ZnkStr_cstr(info->proxy_hostname_) );
		new_info->proxy_port_ = info->proxy_port_;
	}
	return new_info->id_;
}
void
MoaiInfo_clear( MoaiInfo* info )
{
	ZnkHtpHdrs_clear( &info->hdrs_ );
	ZnkVarpAry_clear( info->vars_ );
	ZnkBfr_clear( info->stream_ );
	ZnkStr_clear( info->req_urp_ );
	ZnkStr_clear( info->proxy_hostname_ );
	info->hdr_size_   = 0;
	info->req_method_ = ZnkHtpReqMethod_e_Unknown;
	info->response_state_ = 0;
}

MoaiInfo*
MoaiInfo_find( MoaiInfoID query_id )
{
	const size_t size = ZnkObjAry_size( st_moai_info_ary );
	size_t idx;
	MoaiInfo* info;
	for( idx=0; idx<size; ++idx ){
		info = (MoaiInfo*)ZnkObjAry_at( st_moai_info_ary, idx );
		if( info->id_ == query_id ){
			/* found */
			return info;
		}
	}
	/* not found */
	return NULL;
}

static bool queryMoaiInfoID( ZnkObj obj, void* arg )
{
	MoaiInfoID query_id = Znk_force_ptr_cast( MoaiInfoID, arg );
	MoaiInfo*  info     = (MoaiInfo*)obj;
	return (bool)( info->id_ == query_id );
}
void
MoaiInfo_erase( MoaiInfoID query_id )
{
	ZnkObjAry_erase_withQuery( st_moai_info_ary,
			queryMoaiInfoID, Znk_force_ptr_cast( void*, query_id ) );
}

void
MoaiInfoID_getStr( MoaiInfoID id, MoaiInfoIDStr* id_str )
{
	Znk_snprintf( id_str->buf_, sizeof(id_str->buf_), "%08I64x", (uint64_t)id );
}
MoaiInfoID
MoaiInfoID_scanIDStr( MoaiInfoIDStr* id_str )
{
	uint64_t ans = 0;
	if( !ZnkS_getU64X( &ans, id_str->buf_ ) ){
		assert(0);
	}
	return (MoaiInfoID)ans;
}


static bool
isRelayToMe( const char* my_hostname, size_t my_hostname_leng,
		uint16_t my_port, const char* host_begin )
{
	uint16_t xhr_dmz_port = MoaiServerInfo_XhrDMZPort();
	if( ZnkS_isBeginEx( host_begin, Znk_NPOS, my_hostname, my_hostname_leng ) ){
		if( host_begin[ my_hostname_leng ] == ':' ){
			uint16_t dst_port = 80; /* default HTTP port */
			ZnkS_getU16U( &dst_port, host_begin+my_hostname_leng+1 );
			if( dst_port == my_port || dst_port == xhr_dmz_port ){
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


static bool
decideLocalProxy_or_WebServer( ZnkStr str, ZnkStr req_urp, uint16_t my_port, const char* server_name )
{
	const char* host_begin = NULL;
	bool as_local_proxy = false;

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
		as_local_proxy = true;
	} else if( ZnkStr_first( str ) == '/' ){
		/* / から始まる場合. */
		host_begin = NULL;
		/* 本件はWebServerとして受理 */
		as_local_proxy = false;
		ZnkStr_set( req_urp, ZnkStr_cstr(str) );
	} else if( ZnkStr_first( str ) == '*' ){
		/**
		 * OPTIONS methodなどの場合、この値になっているケースが有り得る.
		 * とりあえず現時点ではLocalProxyとして処理.
		 */
		host_begin = NULL;
		/* 本件はLocalProxyとして受理 */
		as_local_proxy = true;
	} else {
		/* Host名から始まる場合. */
		host_begin = ZnkStr_cstr( str );
		/* 本件はLocalProxyとして受理 */
		as_local_proxy = true;
	}

	if( host_begin ){
		ZnkSRef sref = { 0 };
		ZnkSRef_set_literal( &sref, "localhost" );
		if( isRelayToMe( sref.cstr_, sref.leng_, my_port, host_begin ) ){
			/* 本件はWebServerとしての受理に矯正 */
			as_local_proxy = false;
		}
		ZnkSRef_set_literal( &sref, "127.0.0.1" );
		if( isRelayToMe( sref.cstr_, sref.leng_, my_port, host_begin ) ){
			/* 本件はWebServerとしての受理に矯正 */
			as_local_proxy = false;
		}
		if( isRelayToMe( server_name, Znk_strlen(server_name), my_port, host_begin ) ){
			/* 本件はWebServerとしての受理に矯正 */
			as_local_proxy = false;
		}
		{
			char ipstr[ 64 ] = "";
			uint32_t private_ip = MoaiServerInfo_private_ip();
			ZnkNetIP_getIPStr_fromU32( private_ip, ipstr, sizeof(ipstr) );
			if( isRelayToMe( ipstr, Znk_strlen(ipstr), my_port, host_begin ) ){
				/* 本件はWebServerとしての受理に矯正 */
				as_local_proxy = false;
			}
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
				ZnkMyf hosts = RanoMyf_theHosts();
				size_t hostname_leng = end - host_begin;
				const bool is_applied_host = RanoParentProxy_isAppliedHost2( hosts, host_begin, hostname_leng );
				if( !is_applied_host ){
					size_t cut_size = end - ZnkStr_cstr( str );
					ZnkStr_erase( str, 0, cut_size );
				}
			}
		}
	}
	return as_local_proxy;
}

void
MoaiInfo_parseHdr( MoaiInfo* info, MoaiBodyInfo* body_info,
		bool* as_local_proxy, uint16_t my_port, bool is_request,
		const char* response_hostname,
		RanoModuleAry mod_ary, const char* server_name )
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
	bool is_exist_body = false;

	/* first line */
	line_begin = (char*)ZnkBfr_data( stream );

	/***
	 * MoaiInfo hdrsを設定.
	 */
	p = line_begin;
	while( *p != '\r' ){
		while( *p == ' ' || *p == '\t' ) ++p;
		q = p;
		if( arg_idx < 2 ){
			while( *q != ' ' && *q != '\t' ) ++q;
		} else {
			while( *q != '\r' ) ++q;
		}
		ZnkStrAry_push_bk_cstr( hdrs->hdr1st_, p, q-p );
		p = q;
		++arg_idx;
	}

	line_begin = p + 2;

	/***
	 * status line がRequest Lineの場合、
	 * 第２引数はRequest-URIとなる.
	 */
	if( is_request ){
		ZnkStr str = ZnkStrAry_at( hdrs->hdr1st_, 1 );
		if( ZnkS_eqCase( ZnkStrAry_at_cstr( hdrs->hdr1st_, 0 ), "POST" ) ){
			is_exist_body = true;
		} else {
			is_exist_body = false;
		}
		*as_local_proxy = decideLocalProxy_or_WebServer( str, info->req_urp_, my_port, server_name );
	} else {
		is_exist_body = true;
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
			RanoLog_printf( "  MoaiInfo_parseHdr : Error : key_begin=[%zu] key_end=[%zu] val_begin=[%zu]\n",
					key_begin, key_end, val_begin );
			break;
		}
		varp = ZnkHtpHdrs_regist( hdrs->vars_,
				line_begin+key_begin, key_end-key_begin,
				line_begin+val_begin, val_end-val_begin, false );
		if( varp == NULL ){
			/* memory error */
			exit( EXIT_FAILURE );
		}
		key = ZnkHtpHdrs_key_cstr( varp );
		val = ZnkHtpHdrs_val_cstr( varp, 0 );

		if( ZnkS_eqCase( key, "Referer" ) ){
			ZnkStr str = ZnkHtpHdrs_val( varp, 0 );
			//RanoLog_printf( "  Referer=[%s]\n", ZnkStr_cstr( str ) );
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
				body_info->txt_type_ = RanoText_HTML;
				//RanoLog_printf( "  MoaiInfo_parseHdr : txt_type=HTML\n" );
			} else if(
			    ZnkS_isBegin( val, "text/css" )
			){
				body_info->txt_type_ = RanoText_CSS;
				//RanoLog_printf( "  MoaiInfo_parseHdr : txt_type=CSS\n" );
			} else if(
			    ZnkS_isBegin( val, "application/javascript" )
			 || ZnkS_isBegin( val, "application/x-javascript" )
			 || ZnkS_isBegin( val, "text/javascript" )
			){
				body_info->txt_type_ = RanoText_JS;
				//RanoLog_printf( "  MoaiInfo_parseHdr : txt_type=JS\n" );
			} else {
				body_info->txt_type_ = RanoText_Binary;
				//RanoLog_printf( "  MoaiInfo_parseHdr : txt_type=Binary\n" );
			}
		}
		if( ZnkS_eqCase( key, "Content-Length" ) ){
			ZnkS_getSzU( &body_info->content_length_, val );
		}
		line_begin = line_end + 2;
	}

	/***
	 * www1.axfc.netでダウンロードする際など、is_unlimited_となっているCGIレスポンスがある.
	 * しかもそれはContent-Typeも指定されていないHTMLでもある.
	 * この場合、その内容からtxt_typeを推定しなければならない.
	 * とはいえ、今の段階ではまだヘッダしか読み込んでいないため、この推定処理は後まわしになる.
	 */
	if( is_exist_body && !body_info->is_chunked_ && body_info->content_length_ == 0 )
	{
		/***
		 * chunkedかつContent-Lengthがどちらも指定されていない場合.
		 * 残念ながら、このような行儀の悪いサーバも存在する.
		 * 特にProxyなどに見られる.
		 *
		 * 尚、body部と思わしき余剰の存在如何を関わらず、ここではis_unlimited_をtrueとする.
		 * (body部と思わしき余剰が存在する場合のみis_unlimited_をtrueにした場合、
		 * ある種のProxyを介した場合にBodyが読み込まれないことがある)
		 */
		RanoLog_printf( "  Detect unlimited body.\n" );
		body_info->is_unlimited_ = true;
#if 0
		if( line_begin - (char*)ZnkBfr_data( stream ) + 2 < (int)ZnkBfr_size( stream ) ){
			body_info->is_unlimited_ = true;
			RanoLog_printf( "  It is unlimited\n" );
		}
#endif
	}
}
