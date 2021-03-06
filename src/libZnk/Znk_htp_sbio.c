#include <Znk_htp_sbio.h>
#include <Znk_net_base.h>
#include <Znk_cookie.h>
#include <Znk_s_base.h>
#include <Znk_str_ary.h>
#include <Znk_stdc.h>
#include <Znk_mem_find.h>
#include <Znk_sys_errno.h>
#include <Znk_stock_bio.h>
#include <Znk_zlib.h>
#include <Znk_def_util.h>

#include <stdio.h>
#include <string.h>

static void
printLastRecvError( ZnkStr ermsg, int line_idx )
{
	if( ermsg ){
		ZnkStr_addf( ermsg,
				"ZnkHtpSBIO : Error : line=%d : ZnkNetBase error : [%s]\n",
				line_idx, ZnkNetBase_getErrKey(ZnkNetBase_getLastErrCode()) );
	}
}

typedef struct HeaderInfo_tag {
	bool       is_chunked_;
	size_t     content_length_;
	bool       is_gzip_;
	ZnkStrAry set_cookie_; 
} HeaderInfo;

static HeaderInfo*
createHeaderInfo( void )
{
	HeaderInfo* hdr_info = Znk_malloc( sizeof( struct HeaderInfo_tag ) );
	hdr_info->is_chunked_     = false;
	hdr_info->content_length_ = 0;
	hdr_info->is_gzip_        = false;
	hdr_info->set_cookie_     = ZnkStrAry_create( true );
	return hdr_info;
}
static void
clearHeaderInfo( HeaderInfo* hdr_info )
{
	hdr_info->is_chunked_     = false;
	hdr_info->content_length_ = 0;
	hdr_info->is_gzip_        = false;
	ZnkStrAry_clear( hdr_info->set_cookie_ );
}
static void
destroyHeaderInfo( HeaderInfo* hdr_info )
{
	if( hdr_info ){
		ZnkStrAry_destroy( hdr_info->set_cookie_ );
		Znk_free( hdr_info );
	}
}

static bool
appendRecvedData( ZnkStockBIO sbio, ZnkBfr dst, size_t data_size, ZnkStr ermsg )
{
	uint8_t buf[ 4096 ];
	size_t readed_size;
	size_t begin = ZnkBfr_size(dst);
	while( data_size ){
		if( data_size > sizeof(buf) ){
			readed_size = ZnkStockBIO_read( sbio, buf, sizeof(buf) );
		} else {
			readed_size = ZnkStockBIO_read( sbio, buf, data_size );
		}
		if( readed_size == 0 ){
			/* end of recv */
			if( ermsg ){
				ZnkStr_addf( ermsg, "ZnkHtpSBIO : appendRecvedData end of recv(readed_size=0)\n" );
			}
			return false;
		}
#if 0
		else if( readed_size < 0 ){
			/* error of recv */
			printLastRecvError( ermsg, __LINE__ );
			return false;
		}
#endif
		data_size -= readed_size;
		ZnkBfr_append_dfr( dst, buf, readed_size );
	}
	if( ermsg ){
		ZnkStr_addf( ermsg, "ZnkHtpSBIO : appendRecvedData : size=%zu : [", data_size );
		ZnkStr_append( ermsg, (char*)ZnkBfr_data(dst)+begin, data_size );
		ZnkStr_add( ermsg, "]\n" );
	}
	return true;
}

/****
 * chunkedモードの場合、"hello_world" というbodyは以下のような形のデータとなっている.
 *
 * HTTP/1.1 200 OK
 * Content-Type: text/html
 * Transfer-Encoding: chunked
 * 
 * 6
 * hello_
 * 5
 * world
 * 0
 *
 * chunk_sizeの後に\r\nがあるのはもちろんであるが、その後chunk_size byte進んだところにも\r\nがあることに注意.
 * その後に次のchunk_sizeが来る.
 */
static bool
appendBody( ZnkStockBIO sbio, ZnkBfr body, const HeaderInfo* hdr_info, ZnkStr ermsg )
{
	/***
	 * 注:
	 * recvからの戻り値が 0 の場合、0 より大きい場合に比べてかなり長い待ち時間が発生する可能性がある.
	 * それを回避するため、なるべく終了のタイミングをrecvに頼らないようにする.
	 */
	if( hdr_info->is_chunked_ ){
		size_t chunk_size = 0;
		char chunk_size_line[ 1024 ] = "";
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkHtpSBIO : [chunked mode]\n" );
		}

		while( true ){
			/* chunk_size_lineを読み込み */
			ZnkStockBIO_readLineCRLF( sbio, chunk_size_line, sizeof(chunk_size_line) );
			/* chunk_size は 16進数で表記されている */
			if( !ZnkS_getSzX( &chunk_size, chunk_size_line ) ){
				/* error */
				if( ermsg ){
					ZnkStr_addf( ermsg, "ZnkHtpSBIO : Error : Invalid chunk_size_line=[%s]\n", chunk_size_line );
				}
				return false;
			}
			/***
			 * chunkedモードにおいては、結びのchunk_sizeは必ず0になっている.
			 * また無事これに到達したならresultは成功である.
			 */
			if( chunk_size == 0 ){
				/* success */
				break;
			}

			if( !appendRecvedData( sbio, body, chunk_size, ermsg ) ){
				/* error */
				return false;
			}

			/* chunk dataの一番最後にもCRLFがある. これを読み飛ばす */
			ZnkStockBIO_readLineCRLF( sbio, chunk_size_line, sizeof(chunk_size_line) );
		}

	} else if( hdr_info->content_length_ > 0 ){
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkHtpSBIO : [content_length(%zu) mode]\n", hdr_info->content_length_ );
		}
		if( !appendRecvedData( sbio, body, hdr_info->content_length_, ermsg ) ){
			/* error */
			return false;
		}
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkHtpSBIO : appendRecvedData result of body size=[%zu]\n", ZnkBfr_size(body) );
		}

	} else {
		uint8_t buf[ 4096 ];
		size_t readed_size;
		if( ermsg ){
			ZnkStr_add( ermsg, "ZnkHtpSBIO : [unknown mode]\n" );
		}
		/***
		 * この場合、終了のタイミングはrecvからの0戻り値に頼るほかない.
		 */
		while( true ){
			readed_size = ZnkStockBIO_read( sbio, buf, sizeof(buf) );
#if 0
			if( readed_size < 0 ){
				/* error */
				break;
			}
#endif
			if( readed_size == 0 ){
				/* successful-end */
				break;
			}
			ZnkBfr_append_dfr( body, buf, readed_size );
		}
	}
	return true;
}

/***
 * まず全Headerを得るまでSock通信によりrecvを繰り返す.
 * その際、sizeof(buf) byteずつ受信するが、これらはfirst_recved_bufという形で
 * 蓄積して取得し、Header全体を網羅した時点でrecvは中断する.
 * この関数はfirst_recved_bufを返すが、これの残りには通常bodyの開始部分が含まれて
 * いるはずなので、後に下流過程の関数でこの部分を参照する.
 * また同時にHeaderの全文字列とHeaderの終了バイト位置も取得する.
 */
static int
recvHeader( ZnkStockBIO sbio, ZnkHtpHdrs recv_hdrs, HeaderInfo* hdr_info, ZnkStr ermsg )
{
	char buf[ 8192 ] = { 0 };
	size_t readed_size;

	if( !ZnkStockBIO_readLineCRLF( sbio, buf, sizeof(buf) ) ){
		/***
		 * 通信に関するエラーが発生.
		 * これが発生することはあまりないが、発生した場合は WSAECONNRESET( ピア側での接続がリセットされました ) 
		 * が最も典型的なものである. これは通信している相手が強引に接続を断ったことを意味する.
		 */
		printLastRecvError( ermsg, __LINE__ );
		return -1;
	}

	ZnkHtpHdrs_registHdr1st( recv_hdrs->hdr1st_, buf, Znk_strlen(buf) );

	/* 最初が "HTTP/" ではじまるかどうかを確認 */
	if( !ZnkS_isBegin( buf, "HTTP/" ) ){
		/* プロトコルがHTTPとは異なる */
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkHtpSBIO : Error : recved status line : [%s]\n", (char*)buf );
			ZnkStr_add(  ermsg, "          : This is not begun by \"HTTP/\"\n" );
		}
		return -1;
	}

	/***
	 * header Key:Val部読み込み
	 */
	clearHeaderInfo( hdr_info );

	{
		/* HTTP versionを確認 */
		char* ptr = (char*)( buf + 5 );
		if( strncmp("1.0 ", ptr, 4) == 0 ){
			ptr += 4;
		} else if( strncmp("1.1 ", ptr, 4) == 0 ){
			ptr += 4;
		} else {
			if( ermsg ){
				ZnkStr_add( ermsg, "ZnkHtpSBIO : unknown HTTP version\n" );
			}
			return -1;
		}
	
		/* 次の項目(トークン)までスペースをスキップ */
		while( *ptr == ' ' ){ ++ptr; }
	
		/***
		 * HTTPの結果を解析
		 */
		if( ermsg ){
			int status_code = 0;
			sscanf( ptr, "%d", &status_code );
			switch( status_code ){
			case 200:
				break;
			case 302:
				ZnkStr_add( ermsg, "ZnkHtpSBIO : This is 302 moved.\n" );
				break;
			case 404:
				ZnkStr_add( ermsg, "ZnkHtpSBIO : This is 404 not found.\n" );
				break;
			default:
				ZnkStr_addf( ermsg, "ZnkHtpSBIO : This is status_code[%d].\n", status_code );
				break;
			}
		}
	}

	while( ZnkStockBIO_readLineCRLF( sbio, buf, sizeof(buf) ) ){
		if( ZnkS_empty( buf ) ){
			/* ヘッダ部終わりのCRLFであると思われる */
			break;
		}
		ZnkHtpHdrs_regist_byLine( recv_hdrs->vars_, buf, Znk_NPOS );
	}

	readed_size = ZnkStockBIO_getReadedSize( sbio );
#if 0
	if( readed_size < 0 ){
		printLastRecvError( ermsg, __LINE__ );
		return -1;
	}
#endif

	{
		ZnkVarp     var = NULL;
		const char* val = NULL;

		var = ZnkHtpHdrs_find_literal( recv_hdrs->vars_, "Transfer-Encoding" );
		if( var ){
			val = ZnkHtpHdrs_val_cstr( var, 0 );
			if( ZnkS_eq( val, "chunked" ) ){
				hdr_info->is_chunked_ = true;
			}
		}
		var = ZnkHtpHdrs_find_literal( recv_hdrs->vars_, "Content-Length" );
		if( var ){
			val = ZnkHtpHdrs_val_cstr( var, 0 );
			if( !ZnkS_empty( val ) ){
				ZnkS_getSzU( &hdr_info->content_length_, val );
			}
		}

		var = ZnkHtpHdrs_find_literal( recv_hdrs->vars_, "Content-Encoding" );
		if( var ){
			val = ZnkHtpHdrs_val_cstr( var, 0 );
			if( ZnkS_eq( val, "gzip" ) ){
				hdr_info->is_gzip_ = true;
			}
		}

		var = ZnkHtpHdrs_find_literal( recv_hdrs->vars_, "Set-Cookie" );
		if( var ){
			const size_t val_size = ZnkHtpHdrs_val_size( var );
			size_t val_leng = 0;
			size_t idx;
			size_t pos;
			for( idx=0; idx<val_size; ++idx ){
				val = ZnkHtpHdrs_val_cstr( var, idx );
				val_leng = Znk_strlen( val );
				pos = ZnkMem_lfind_8( (uint8_t*)val, val_leng, (uint8_t)';', 1 );
				ZnkStrAry_push_bk_cstr( hdr_info->set_cookie_, val, ( pos == Znk_NPOS ) ? val_leng : pos );
			}
		}
	}
	return 0;
}


typedef struct {
	const uint8_t* src_;
	size_t         src_size_;
	ZnkBfr         ans_;
} GZipInfo;
static unsigned int
supplyDstBfr( uint8_t* dst_buf, size_t dst_size, void* arg )
{
	GZipInfo* info = Znk_force_ptr_cast( GZipInfo*, arg );
	ZnkBfr_append_dfr( info->ans_, dst_buf, dst_size );
	return dst_size;
}
static unsigned int
demandSrc( uint8_t* src_buf, size_t src_size, void* arg )
{
	GZipInfo* info = Znk_force_ptr_cast( GZipInfo*, arg );
	const size_t cpy_size = Znk_MIN( info->src_size_, src_size );
	memmove( src_buf, info->src_, cpy_size );
	info->src_      += cpy_size;
	info->src_size_ -= cpy_size;
	return (unsigned int)cpy_size;
}
static bool
inflateGZipBody_toBfr( ZnkBfr ans, const uint8_t* body_data, const size_t body_size, ZnkStr emsg )
{
	bool result = false;
	uint8_t dst_buf[ 4096 ];
	ZnkZStream zst = ZnkZStream_create();

	if( !ZnkZStream_inflateInit( zst, emsg ) ){
		goto FUNC_END;
	} else {
		uint8_t src_buf[ 4096 ];
		GZipInfo gzip_info = { 0 };

		gzip_info.src_      = body_data;
		gzip_info.src_size_ = body_size;
		gzip_info.ans_      = ans;

		while( gzip_info.src_size_ ){
			if( !ZnkZStream_inflate2( zst,
						dst_buf, sizeof(dst_buf), supplyDstBfr, &gzip_info,
						src_buf, sizeof(src_buf), demandSrc,    &gzip_info,
						emsg ) )
			{
				goto FUNC_END;
			}
		}
	}
	if( !ZnkZStream_inflateEnd( zst, emsg ) ){
		goto FUNC_END;
	}

	result = true;
FUNC_END:

	ZnkZStream_destroy( zst );
	return result;
}


static bool
recvHeaderAndBody( ZnkStockBIO sbio, ZnkHtpReqMethod req_method, HeaderInfo* hdr_info, ZnkHtpHdrs recv_hdrs, ZnkBfr body, ZnkStr ermsg )
{
	int result;
	bool uncomp_result = true;

	result = recvHeader( sbio, recv_hdrs, hdr_info, ermsg );
	if( result < 0 ){
		return false;
	}

	if( req_method != ZnkHtpReqMethod_e_HEAD ){
		/***
		 * bodyを取得.
		 * HTMLといえども圧縮されbinary状態で得られることもある.
		 * 従ってここではZnkStrではなくZnkBfrを使う.
		 */
		if( !appendBody( sbio, body, hdr_info, ermsg ) ){
			return false;
		}
	
		/***
		 * HTMLなどはgzip圧縮されているケースが見られる.
		 * しかしこれは別にHTML限定というわけでなく、一般にどんなバイナリであれ
		 * この形式で圧縮されていても構わない.
		 * いずれの場合でも、指定された圧縮形式の指示に従い、ここで本来のバイナリイメージへ
		 * 展開するだけである.
		 *
		 * 今後の課題としては、超大容量ファイルをダウンロードする場合に、少しずつファイルへ
		 * 落とすような形で保存する処理をサポートすることだが、とりあえずそれは後回し.
		 */
		if( hdr_info->is_gzip_ ){
			ZnkBfr dst_bfr = ZnkBfr_create_null();
			ZnkStr gz_emsg = ZnkStr_new( "" );
			const uint8_t* body_data = ZnkBfr_data( body );
			const size_t   body_size = ZnkBfr_size( body );
			if( !inflateGZipBody_toBfr( dst_bfr, body_data, body_size, gz_emsg ) ){
				if( ermsg ){
					ZnkStr_addf( ermsg, "ZnkHtpSBIO : recvHeaderAndBody : [%s]\n", ZnkStr_cstr(gz_emsg) );
				}
			} else {
				ZnkBfr_swap( dst_bfr, body );
			}
			ZnkStr_delete( gz_emsg );
			ZnkBfr_destroy( dst_bfr );

		}
	}
	return uncomp_result;
}

bool
ZnkHtpSBIO_recv( ZnkStockBIO sbio, ZnkHtpReqMethod req_method,
		ZnkHtpHdrs recv_hdrs, ZnkHtpOnRecvFuncArg recv_fnca,
		ZnkVarpAry cookie, ZnkBfr wk_bfr, ZnkStr ermsg )
{
	bool result = false;
	HeaderInfo* hdr_info = createHeaderInfo();

	ZnkBfr_clear( wk_bfr );

	if( !recvHeaderAndBody( sbio, req_method, hdr_info, recv_hdrs, wk_bfr, ermsg ) ){
		if( ermsg ){
			ZnkStr_add( ermsg, "ZnkHtpSBIO : recvHeaderAndBody : [Failure].\n" );
		}
		result = false;
	} else {
		if( cookie ){
			size_t i;
			size_t n = ZnkStrAry_size( hdr_info->set_cookie_ );
			for( i=0; i<n; ++i ){
				const char* line = ZnkStrAry_at_cstr( hdr_info->set_cookie_, i );
				ZnkCookie_regist_byAssignmentStatement( cookie, line, Znk_NPOS );
			}
		}
	
		if( recv_fnca.func_ ){
			(*recv_fnca.func_)( recv_fnca.arg_,
					ZnkBfr_data(wk_bfr), ZnkBfr_size(wk_bfr) );
		}
		result = true;
	}

	destroyHeaderInfo( hdr_info );
	return result;
}
