#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>
#include <Znk_htp_util.h>
#include <stdio.h>
#include <string.h>

static bool
saveAsResultFile( const char* ext, const uint8_t* data, size_t data_size )
{
	char    result_filename[ 256 ] = "";
	ZnkFile fp = NULL;
	size_t  write_size = 0;
	Znk_snprintf( result_filename, sizeof(result_filename), "result_file.%s", ext );
	fp = Znk_fopen( result_filename, "wb" );
	if( fp ){
		write_size =  Znk_fwrite( data, data_size, fp );
		Znk_fclose( fp );
	}
	return (bool)( write_size == data_size );
}

static void
show_result( const ZnkVarpAry post_vars )
{
	/***
	 * Output HTML for CGI
	 */
	printf( "Content-type: text/html; charset=Shift_JIS\r\n" );
	/* ブラウザにキャッシュさせない。 */
	printf( "Pragma: no-cache\r\n" );
	printf( "Cache-Control: no-cache\r\n" );
	printf( "\r\n" );

	printf( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
	printf( "<html>\n" );
	printf( "<head>\n" );
	printf( "<META http-equiv=\"Content-type\" content=\"text/html; charset=Shift_JIS\">\n" );
	printf( "<META http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n" );
	printf( "<META http-equiv=\"Content-Style-Type\" content=\"text/css\">\n" );
	printf( "</head>\n" );

	printf( "<body>\n" );

	printf( "Moai CGI Post for multipart/form-data.<br> <br>\n" );
	printf( "<form action=post2.cgi method=POST enctype=multipart/form-data>\n" );
	printf( "<b>E-mail</b><input type=text name=email size=28><br>\n" );
	printf( "<b>コメント</b><textarea name=com cols=48 rows=\"4\"></textarea><br>\n" );
	printf( "<b>添付File</b><input type=file name=upfile ><br>\n" );
	printf( "<br>\n" );
	printf( "<input type=submit value=\"返信する\"><br>\n" );
	printf( "</form>\n" );
	printf( "<ul>\n" );
	printf( "<li>添付File : GIF, JPG, PNG, WEBM. 2000KBまで</li>\n" );
	printf( "</ul>\n" );

	printf( "<pre>\n" );
	printf( "PostVars:.\n" );

	{
		const char* ext = NULL;
		bool is_image = false;
		/* ZnkStrはlibZnkが提供する文字列型です. */
		ZnkStr msg = ZnkStr_new( "" );
		size_t i; 
		
		for( i=0; i<ZnkVarpAry_size(post_vars); ++i ){
			/* Post変数を一つ取り出します.
			* この変数の名前も得ておきます. */
			ZnkVarp     var  = ZnkVarpAry_at( post_vars, i );
			const char* name = ZnkVar_name_cstr(var);
			
			if( ZnkVar_prim_type( var ) == ZnkPrim_e_Bfr ){
				/* ZnkVarpのタイプがZnkPrim_e_Bfr(任意のByte列).
				* これは添付ファイルを意味します. */
				const char*    filename  = ZnkVar_misc_cstr( var, "filename" );
				const uint8_t* data      = ZnkVar_data( var );
				const size_t   data_size = ZnkVar_data_size( var );
				
				/* ZnkS_get_extension の戻り値はファイルの拡張子となります. */
				/* この参照先は、post_varsの寿命が続く限りにおいては有効です. */
				ext = ZnkS_get_extension( filename, '.' );
				
				if( data_size > 2000000 ){ /* 2000KB以上 */
					is_image = false;
					ZnkStr_addf( msg, "Binary : %s orinal filename=[%s] : Error : This is too large.\n", name, filename );
				} else {
					/* 文字列が等しいかの比較を行います. */
					/* ただし大文字小文字の区別はしません. */
					if(  ZnkS_eqCase( ext, "jpg" )
					  || ZnkS_eqCase( ext, "png" )
					  || ZnkS_eqCase( ext, "gif" )
					  || ZnkS_eqCase( ext, "webm" )
					){
						/* 画像ファイルの拡張子であった場合はこのデータをresult_file.ext として保存します. */
						ZnkStr_addf( msg, "Image : data_size=[%zu]\n", data_size );
						if( saveAsResultFile( ext, data, data_size ) ){
							is_image = true;
							ZnkStr_addf( msg, "Image : %s orinal filename=[%s]\n", name, filename );
						} else {
							is_image = false;
							ZnkStr_addf( msg, "Image : %s orinal filename=[%s] : Error : fwrite error.\n", name, filename );
						}
					} else {
						is_image = false;
						ZnkStr_addf( msg, "Binary : %s orinal filename=[%s]\n", name, filename );
					}
				}
			} else {
				/* ZnkVarpのタイプがその他(Str).
				* これはinputタグやtextareaタグ等に由来する文字列データ. */
				const char* val = ZnkVar_cstr(var);
				ZnkStr_addf( msg, "%s = [%s] (size=[%u])\n", name, val, ZnkVar_str_leng(var) );
			}
		}
		
		/* XSS対策. msg内にある一切のHTMLタグの効果を打ち消します. */
		ZnkHtpURL_negateHtmlTagEffection( msg ); /* for XSS */
		if( is_image ){
			/* 画像ファイルであった場合はimgタグによりそれを表示します. */
			ZnkStr_addf( msg, "<br>\n" );
			ZnkStr_addf( msg, "<img src=\"./result_file.%s\">\n", ext );
			ZnkStr_addf( msg, "<br>\n" );
		}
		fputs( ZnkStr_cstr(msg), stdout );
		/* ZnkStrは使用後解放しなければなりません. */
		ZnkStr_delete( msg );
	}

	printf( "</pre>" );

	printf( "</body></html>\n" );
	fflush( stdout );
}

int main(int argc, char **argv)
{
	/***
	 * Create+Get Environment Variables for CGI
	 */
	RanoCGIEVar* evar = RanoCGIEVar_create();

	/***
	 * Get query_string;
	 */
	const char* query_string = evar->query_string_ ? evar->query_string_ : "";

	/***
	 * Create post_vars.
	 */
	ZnkVarpAry post_vars = ZnkVarpAry_create( true );

	/***
	 * for Windows : change stdin/stdout to binary-mode.
	 */
	Znk_Internal_setMode( 0, true );
	Znk_Internal_setMode( 1, true );

	/***
	 * Get post_vars from query_string.
	 */
	RanoCGIUtil_splitQueryString( post_vars, query_string, Znk_NPOS, false );

	/***
	 * CGIにおけるフォームの投稿データをpost_varsへと取得します.
	 * またこの関数においては第1引数evarの指定も必要となります.
	 */
	RanoCGIUtil_getPostedFormData( evar, post_vars, NULL, NULL, NULL, NULL, true );

	show_result( post_vars );

	ZnkVarpAry_destroy( post_vars );
	RanoCGIEVar_destroy( evar );
	return 0;
}
