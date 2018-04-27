#include "Moai_http.h"
#include "Moai_io_base.h"
#include "Moai_info.h"

#include <Rano_log.h>

#include <Znk_socket.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <Znk_zlib.h>
#include <Znk_def_util.h>
#include <Znk_htp_hdrs.h>
#include <Znk_dir.h>
#include <Znk_cookie.h>

#include <string.h>
#include <assert.h>

bool
MoaiHttp_scanHeaderVar( const char* var_name, const uint8_t* buf,
		size_t result_size, char* val_buf, size_t val_buf_size )
{
	/***
	 * 続いてVar: ヘッダーラインを検出.
	 */
	char var_ptn[ 4096 ];
	char* begin = (char*)buf;
	char* end;
	size_t remain_size = result_size;
	size_t var_ptn_leng;
	char* val_begin;
	Znk_snprintf( var_ptn, sizeof(var_ptn), "%s: ", var_name );
	var_ptn_leng = Znk_strlen( var_ptn );
	while( remain_size ){
		end = memchr( begin, '\r', remain_size );
		if( end == NULL ){
			/* Var line not found or broken */
			return false;
		}
		end++;
		if( end - begin >= (ptrdiff_t)remain_size ){
			/* Var line not found or broken */
			return false;
		}
		if( *end != '\n' ){
			/* broken line */
			return false;
		}
		end++;
		if( ZnkS_isCaseBegin( begin, var_ptn ) ){
			val_begin = begin + var_ptn_leng;
			ZnkS_copy( val_buf, val_buf_size, val_begin, end-2 - val_begin );
			/* found */
			return true;
		}
		remain_size -= end - begin;
		begin = end;
	}
	return false;
}


typedef struct {
	const uint8_t* src_;
	size_t         src_size_;
	ZnkStr         ans_;
} GZipInfo;
static unsigned int
supplyDstStr( uint8_t* dst_buf, size_t dst_size, void* arg )
{
	GZipInfo* info = (GZipInfo*)arg;
	ZnkStr_append( info->ans_, (char*)dst_buf, dst_size );
	return dst_size;
}
static unsigned int
demandSrc( uint8_t* src_buf, size_t src_size, void* arg )
{
	GZipInfo* info = (GZipInfo*)arg;
	const size_t cpy_size = Znk_MIN( info->src_size_, src_size );
	memmove( src_buf, info->src_, cpy_size );
	info->src_      += cpy_size;
	info->src_size_ -= cpy_size;
	return (unsigned int)cpy_size;
}
static bool
inflateGZipBody_toStr( ZnkStr ans, const uint8_t* body_data, const size_t body_size, ZnkStr emsg )
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
						dst_buf, sizeof(dst_buf), supplyDstStr, &gzip_info,
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


static ZnkFile
openResultFile( RanoTextType txt_type )
{
	ZnkFile fp = NULL;
	/***
	 * 現状ではまだ固定名なので問題ないが、ここをもしhost上にあるファイル名で保存する場合は
	 * カレントディレクトリへ直に保存するのは危険であるのでtmp ディレクトリを作成して
	 * その配下へ保存する形にした方がよい.
	 */
	ZnkDir_mkdirPath( "./tmp", Znk_NPOS, '/', NULL );
	switch( txt_type ){
	case RanoText_HTML:
		fp = Znk_fopen( "./tmp/result.html", "wb" );
		break;
	case RanoText_JS:
		fp = Znk_fopen( "./tmp/result.js", "wb" );
		break;
	case RanoText_CSS:
		fp = Znk_fopen( "./tmp/result.css", "wb" );
		break;
	case RanoText_Binary:
		fp = Znk_fopen( "./tmp/result.bin", "wb" );
		break;
	default:
		fp = Znk_fopen( "./tmp/result.dat", "wb" );
		break;
	}
	return fp;
}

typedef enum {
	 ChunkRecv_OK=0
	,ChunkRecv_UnexpectedZero
	,ChunkRecv_BrokenData
	,ChunkRecv_Error
}ChunkRecv;


static bool
isBrokenChunkSizeStr( const char* cstr, size_t cstr_leng, bool permit_whitespace )
{
	/***
	 * chunk_size部の後ろにwhilespaceが含まれているようなサイトが存在する.
	 * つまり
	 *
	 *   chunk_size部 whilespace 改行
	 *
	 * のような構造となっている状況である.
	 * そのようなwhilespaceも許容する.
	 */
	bool whitespace_incoming = false;

	size_t idx;
	if( cstr_leng == 0 || cstr_leng > sizeof(size_t)*2 ){
		/* cstrが空、またはsize_t型変数の16進数最大桁を超えている場合は異常とみなす */
		return true;
	}
	for( idx=0; idx<cstr_leng; ++idx ){
		switch( cstr[ idx ] ){
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
			if( whitespace_incoming ){
				/* 途中whitespaceが来た後に16進数が来ている場合 */
				/* broken. */
				return true;
			}
			continue;
		case ' ':
			if( permit_whitespace ){
				whitespace_incoming = true;
			} else {
				/* whitespace_incoming を認めない. broken. */
				return true;
			}
			continue;
		default:
			/* broken. */
			return true;
		}
	}
	return false;
}

/***
 * いきなりparseChunkDataを読んでもロジックとしては問題ないのだが、
 * detectChunkedBodyBeginの方がrecv量を最小に抑えている分高速であるので、
 * まずこれを試した方がよい.
 */
static ChunkRecv
detectChunkedBodyBegin( ZnkBfr stream, size_t chunk_begin, ZnkSocket O_sock, MoaiFdSet mfds,
		size_t* total_recv_size )
{
	/* checked が指定されていないケース(Implicit checked)の場合は、やや判定を厳しくする */
	static const bool permit_whitespace = false;
	static const size_t check_size = sizeof(size_t)*2+2;

	size_t      dbfr_size;
	const char* chunk_size_cstr_begin;
	const char* chunk_size_cstr_end;
	size_t      chunk_size_cstr_leng = 0;
	size_t      result_size = 0;

	dbfr_size = ZnkBfr_size( stream );
	chunk_size_cstr_begin = (char*)ZnkBfr_data( stream ) + chunk_begin;
	chunk_size_cstr_leng = dbfr_size - chunk_begin;
	if( chunk_size_cstr_leng < check_size ){
		/* checkに必要なサイズに達していないため、そこまでrecvする */
		if( !MoaiIO_recvBySize( stream, O_sock, mfds, check_size-chunk_size_cstr_leng, &result_size ) ){
			/* recv error. */
			return false;
		}
		if( total_recv_size ){ *total_recv_size += result_size; }
	}

	dbfr_size = ZnkBfr_size( stream );
	chunk_size_cstr_begin = (char*)ZnkBfr_data( stream ) + chunk_begin; /* for realloc */
	chunk_size_cstr_end = Znk_memmem( chunk_size_cstr_begin, dbfr_size-chunk_begin, "\r\n", 2 );
	if( chunk_size_cstr_end == NULL ){
		/* check_size内で \r\nが検出されないため、checkedではないとみなす */
		return false;
	}
	chunk_size_cstr_leng = (size_t)( chunk_size_cstr_end - chunk_size_cstr_begin );
	if( isBrokenChunkSizeStr( chunk_size_cstr_begin, chunk_size_cstr_leng, permit_whitespace ) ){
		/* \r\nまでの範囲で、16進数以外の文字が含まれている. checkedではないとみなす */
		return false;
	}
	/***
	 * check_size内かつ\r\nまでの範囲で、すべて16進数文字であった場合.
	 * chunkedである可能性が濃厚である.
	 */
	RanoLog_printf( "  Implicit chunked is detected. chunk_size_cstr=[%s]\n", chunk_size_cstr_begin );
	return true;
}


static ChunkRecv
parseChunkData( ZnkBfr stream, size_t chunk_begin, ZnkSocket O_sock, MoaiFdSet mfds,
		size_t* total_recv_size )
{
	/* checked が指定されているケースの場合、whitespace_incomingを認める */
	static const bool permit_whitespace = true;
	size_t      dbfr_size;
	const char* chunk_size_cstr_begin;
	const char* chunk_size_cstr_end;
	char        chunk_size_cstr_buf[ 1024 ];
	size_t      chunk_size = 0;
	size_t      chunk_size_cstr_leng = 0;
	size_t      chunk_remain = 0;
	size_t      result_size = 0;

	while( true ){
		dbfr_size = ZnkBfr_size( stream );
		chunk_size_cstr_begin = (char*)ZnkBfr_data( stream ) + chunk_begin;
		chunk_size_cstr_end   = Znk_memmem( chunk_size_cstr_begin, dbfr_size-chunk_begin, "\r\n", 2 );
		if( chunk_size_cstr_end == NULL ){
			result_size = MoaiIO_recvByPtn2( stream, O_sock, mfds, "\r\n" );
			if( result_size == Znk_NPOS ){
				/* recv error. */
				return ChunkRecv_Error;
			}
			if( total_recv_size ){ *total_recv_size += result_size; }
			result_size = 0;

			/* for realloc */
			dbfr_size = ZnkBfr_size( stream );
			chunk_size_cstr_begin = (char*)ZnkBfr_data( stream ) + chunk_begin;
			chunk_size_cstr_end   = Znk_memmem( chunk_size_cstr_begin, dbfr_size-chunk_begin, "\r\n", 2 );
			if( chunk_size_cstr_end == NULL ){
				/* pattern not found in recved data. */
				return ChunkRecv_UnexpectedZero;
			}
		}

		chunk_size_cstr_leng = (size_t)( chunk_size_cstr_end - chunk_size_cstr_begin );
		if( isBrokenChunkSizeStr( chunk_size_cstr_begin, chunk_size_cstr_leng, permit_whitespace ) ){
			return ChunkRecv_BrokenData;
		}
		ZnkS_copy( chunk_size_cstr_buf, sizeof(chunk_size_cstr_buf),
				chunk_size_cstr_begin, chunk_size_cstr_leng );

		/* chunk_size は 16進数で表記されている */
		ZnkS_getSzX( &chunk_size, chunk_size_cstr_buf );

		/***
		 * dbfrにおけるchunk_size_cstrの領域を削る.
		 */
		ZnkBfr_erase( stream, chunk_begin, chunk_size_cstr_leng+2 );

		if( ZnkBfr_size( stream ) < chunk_begin + chunk_size + 2 ){
			chunk_remain = chunk_begin + chunk_size + 2 - ZnkBfr_size( stream );
		} else {
			chunk_remain = 0;
		}
		if( chunk_remain ){
			if( !MoaiIO_recvBySize( stream, O_sock, mfds, chunk_remain, &result_size ) ){
			}
			if( total_recv_size ){ *total_recv_size += result_size; }
			result_size = 0;
		}
		/***
		 * dbfrにおけるchunkの最後にある\r\nを削る.
		 * ( chunk_size_cstrが 0 の場合でも後ろに空白行が一つ存在するため、これが必要.
		 * さらに厳密に言えば、その前にフッター行が来る可能性もあるが、今は考慮しない)
		 */
		ZnkBfr_erase( stream, chunk_begin+chunk_size, 2 );
		if( chunk_size == 0 ){
			break;
		}

		//RanoLog_printf( "  Chunk size=[%zu] ", chunk_size );

		chunk_begin += chunk_size;
	}
	return ChunkRecv_OK;
}

Znk_INLINE bool
updateContentLengthRemain( size_t* content_length_remain, size_t result_size )
{
	if( *content_length_remain >= result_size ){
		*content_length_remain -= result_size;
	} else {
		*content_length_remain = 0;
		return false;
	}
	return true;
}
static void
dumpBrokenChunkStream( ZnkBfr stream )
{
	ZnkFile fp;
	ZnkDir_mkdirPath( "./tmp", Znk_NPOS, '/', NULL );
	fp = Znk_fopen( "./tmp/dbfr_dump.dat", "wb" );
	if( fp ){
		const uint8_t* data = ZnkBfr_data( stream );
		const size_t   size = ZnkBfr_size( stream );
		RanoLog_printf( "chunk_size_cstr is broken. dump dbfr_dump.dat\n" );
		Znk_fwrite( data, size, fp );
		Znk_fclose( fp );
	}
}


void
MoaiHttp_processResponse_forText( ZnkSocket O_sock, MoaiContext ctx, MoaiFdSet mfds,
		size_t* content_length_remain, RanoModule mod )
{
	MoaiInfo*     info      = ctx->draft_info_;
	MoaiBodyInfo* body_info = &ctx->body_info_;
	bool is_chunked = ctx->body_info_.is_chunked_;

	size_t result_size = 0;

	/***
	 * checkedが指定されていない場合でもとりあえず最初はcheckedのパターンを検査してrecvを試みる.
	 * (checkedが指定されていないにも関わらずcheckedなパターンでデータを返している行儀の悪い
	 * CGIなどが極稀に存在する(例えばwww1.axfc.net)からであり、それに対応するためである)
	 */
	/***
	 * is_unlimited_ とは !is_chunked_ && content_length_ == 0 の状態のことである.
	 * つまり !is_unlimited_では is_chunked_ || content_length_ != 0 となる.
	 */
	if( ctx->body_info_.is_unlimited_ ){
		bool is_recv_by_zero = false;

		if( !is_chunked ){
			if( detectChunkedBodyBegin( info->stream_, info->hdr_size_, O_sock, mfds, &result_size ) ){
				is_chunked = true;
			}
		}

		if( is_chunked ){
			/* parseChunkDataを実行 */
			ChunkRecv cr = parseChunkData( info->stream_, info->hdr_size_, O_sock, mfds, &result_size );
			if( cr == ChunkRecv_BrokenData ){
				is_recv_by_zero = true;
			}
		}  else {
			is_recv_by_zero = true;
		}

		if( is_recv_by_zero ){
			/* 残りはRecvZeroを受け取るまで力技でrecv. */
			RanoLog_printf( "  MoaiIO_recvByZero Mode Begin\n" );
			MoaiIO_recvByZero( info->stream_, O_sock, mfds, &result_size );
			RanoLog_printf( "  MoaiIO_recvByZero Mode End\n" );
		}

	} else if( is_chunked ){
		/* Transfer-Encodingにおいて明示的にchunkedが指定されている場合 */
		ChunkRecv cr = parseChunkData( info->stream_, info->hdr_size_, O_sock, mfds, &result_size );
		RanoLog_printf( "  parseChunkData result=[%d]\n", cr );
		updateContentLengthRemain( content_length_remain, result_size );
		if( cr == ChunkRecv_BrokenData ){
			dumpBrokenChunkStream( info->stream_ );

			/* 残りはcheckedが指定されていない場合と同様にrecv. */
			RanoLog_printf( "  MoaiIO_recvBySize(1) content_length_remain=[%zu]\n", *content_length_remain );
			while( *content_length_remain ){
				if( !MoaiIO_recvBySize( info->stream_, O_sock, mfds, *content_length_remain, &result_size ) ){
					break;
				}
				if( !updateContentLengthRemain( content_length_remain, result_size ) ){
					break;
				}
			}
		}

	} else if( *content_length_remain ){
		/* Transfer-Encodingにおいて明示的にchunkedが指定されておらず
		 * Content-Lengthの指定は存在する場合 */
		if( !is_chunked ){
			if( detectChunkedBodyBegin( info->stream_, info->hdr_size_, O_sock, mfds, &result_size ) ){
				is_chunked = true;
			}
			updateContentLengthRemain( content_length_remain, result_size );
		}
		if( is_chunked ){
			/***
			 * この場合chunkedデータとみなす.
			 */
			ChunkRecv cr = parseChunkData( info->stream_, info->hdr_size_, O_sock, mfds, &result_size );
			updateContentLengthRemain( content_length_remain, result_size );
			if( cr == ChunkRecv_BrokenData ){
				/* 残りはcheckedが指定されていない場合と同様にrecv. */
				RanoLog_printf( "  MoaiIO_recvBySize(2) content_length_remain=[%zu]\n", *content_length_remain );
				while( *content_length_remain ){
					if( !MoaiIO_recvBySize( info->stream_, O_sock, mfds, *content_length_remain, &result_size ) ){
						break;
					}
					if( !updateContentLengthRemain( content_length_remain, result_size ) ){
						break;
					}
				}
			}
		} else {
			/* chunkedではない場合.
			 * 以下によって全取得. */
			RanoLog_printf( "  MoaiIO_recvBySize(3) content_length_remain=[%zu]\n", *content_length_remain );
			while( *content_length_remain ){
				if( !MoaiIO_recvBySize( info->stream_, O_sock, mfds, *content_length_remain, &result_size ) ){
					break;
				}
				if( !updateContentLengthRemain( content_length_remain, result_size ) ){
					break;
				}
			}
		}
	}

	{
		const uint8_t* body_data = ZnkBfr_data( info->stream_ ) + info->hdr_size_;
		const size_t   body_size = ZnkBfr_size( info->stream_ ) - info->hdr_size_;

		ZnkStr_clear( ctx->text_ );
		if( body_info->is_gzip_ ){
			ZnkStr emsg = NULL;

			emsg = ctx->msgs_;
			ZnkStr_clear( ctx->msgs_ );

			if( !inflateGZipBody_toStr( ctx->text_, body_data, body_size, emsg ) ){
				RanoLog_printf( "  MoaiHttp_processResponse_forText : [%s]\n", ZnkStr_cstr(emsg) );
				ZnkStr_append( ctx->text_, (char*)body_data, body_size );
			}

		} else {
			ZnkStr_append( ctx->text_, (char*)body_data, body_size );
		}
	}

	if( mod ){
		RanoTxtFilterAry txt_ftr = NULL;
		switch( body_info->txt_type_ ){
		case RanoText_HTML:
			txt_ftr = RanoModule_ftrHtml( mod );
			break;
		case RanoText_JS:
			txt_ftr = RanoModule_ftrJS( mod );
			break;
		case RanoText_CSS:
			txt_ftr = RanoModule_ftrCSS( mod );
			break;
		default:
			break;
		}
		RanoModule_invokeOnResponse( mod, info->hdrs_.vars_, ctx->text_, ZnkStr_cstr(info->req_urp_) );
		if( txt_ftr ){
			RanoTxtFilterAry_exec( txt_ftr, ctx->text_ );

			if( body_info->txt_type_ == RanoText_CSS ){
				const ZnkStrAry css_additional = RanoModule_ftrCSSAdditional( mod );
				const size_t size = ZnkStrAry_size( css_additional );
				size_t idx;
				ZnkStr line;
				for( idx=0; idx<size; ++idx ){
					line =  ZnkStrAry_at( css_additional, idx );
					ZnkStr_add_c( ctx->text_, '\n' );
					ZnkStr_append( ctx->text_, ZnkStr_cstr(line), ZnkStr_leng(line) );
				}
			}
		}
	}

	{
		ZnkFile fp = openResultFile( body_info->txt_type_ );
		if( fp ){
			const uint8_t* html_data = (uint8_t*)ZnkStr_cstr( ctx->text_ );
			const size_t   html_size = ZnkStr_leng( ctx->text_ );
			Znk_fwrite( html_data, html_size, fp );
			Znk_fclose( fp );
		}
	}

	/***
	 * I側にはctx->text_の値で返信する.
	 * これに伴い、HeaderにおけるContent-Lengthの値を修正する.
	 */
	{
		ZnkHtpHdrs hdrs = &info->hdrs_;
		const char* key;
		bool exist_content_length = false;
		size_t size;
		size_t idx;
		ZnkVarp varp;

		ZnkHtpHdrs_erase( hdrs->vars_, "Transfer-Encoding" );

		size = ZnkVarpAry_size( hdrs->vars_ );
		for( idx=0; idx<size; ++idx ){
			varp = ZnkVarpAry_at( hdrs->vars_, idx );
			key = ZnkHtpHdrs_key_cstr( varp );

			if( ZnkS_eqCase( key, "Content-Length" ) ){
				ZnkStr str = ZnkHtpHdrs_val( varp, 0 );
				size_t new_contet_length = ZnkStr_leng( ctx->text_ );
				ZnkStr_setf( str, "%zu", new_contet_length );
				exist_content_length = true;
			}
			if( ZnkS_eqCase( key, "Content-Encoding" ) ){
				ZnkStr str = ZnkHtpHdrs_val( varp, 0 );
				/* identityは特別なEncodingはなしを示す */
				ZnkStr_set( str, "identity" );
			}
		}
		if( !exist_content_length ){
			char buf[ 1024 ];
			size_t new_contet_length = ZnkStr_leng( ctx->text_ );
			key = "Content-Length";
			Znk_snprintf( buf, sizeof(buf), "%zu", new_contet_length );
			ZnkHtpHdrs_regist( hdrs->vars_,
					key, Znk_strlen(key),
					buf, Znk_strlen(buf), true );
		}
	}
}

void
MoaiHttp_debugHdrVars( const ZnkVarpAry vars, const char* prefix_tab )
{
	const size_t size = ZnkVarpAry_size( vars );
	size_t  idx;
	size_t  val_idx;
	size_t  val_size;
	ZnkVarp varp = NULL;
	for( idx=0; idx<size; ++idx ){
		varp = ZnkVarpAry_at( vars, idx );
		val_size = ZnkHtpHdrs_val_size( varp );
		RanoLog_printf( "%s", prefix_tab );
		RanoLog_printf( "[%s]: ", ZnkHtpHdrs_key_cstr(varp) );
		for( val_idx=0; val_idx<val_size; ++val_idx ){
			RanoLog_printf( "[%s]",  ZnkHtpHdrs_val_cstr(varp,val_idx) );
		}
		RanoLog_printf( "\n" );
	}
}

