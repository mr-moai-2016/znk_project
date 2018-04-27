#include "proxy_finder.h"
#include "proxy_info.h"

#include <Znk_s_base.h>
#include <Znk_stdc.h>
#include <Znk_str_ex.h>
#include <Znk_str_path.h>
#include <Znk_str_fio.h>
#include <Znk_bms_find.h>
#include <Znk_net_base.h>
#include <Znk_missing_libc.h>
#include <Znk_dir.h>
#include <Znk_htp_util.h>
#include <Znk_htp_hdrs.h>

#include <Rano_htp_boy.h>

#include <string.h>
#include <stdlib.h>

bool
ProxyFinder_download( ZnkStr result_filename,
		const char* hostname, const char* url_underpath, const char* target, ZnkStr ermsg )
{
	bool result = false;

	/***
	 * libZnkにてNetwork通信を行うための初期設定.
	 */
	if( !ZnkNetBase_initiate( false ) ){
		ZnkStr_addf( ermsg, "Error : ZnkNetBase_initiate is failure.\n" );
		goto FUNC_END;
	}
	ZnkStr_addf( ermsg, "Report : ZnkNetBase_initiate is OK.\n" );

	ZnkStr_addf( ermsg, "Report : Now connecting to [%s%s]...\n", hostname, url_underpath );

	/***
	 * RanoHtpBoyにてHTTP接続.
	 * result_filenameという名前でHTML取得.
	 */
	{
		static const char*    parent_proxy = "";
		static const char*    cachebox     = "./cachebox/";
		static const char*    tmpdir       = "./tmp/";
		ZnkStr                req_urp      = ZnkStr_new( "" );
		struct ZnkHtpHdrs_tag htp_hdrs     = { 0 };

		ZnkStr_set( req_urp, url_underpath );

		/**
		 * HTTPヘッダの構築.
		 */
		ZnkHtpHdrs_compose( &htp_hdrs );

		/**
		 * HTTPヘッダの各種ディレクティブの登録.
		 */
		ZnkHtpHdrs_regist( htp_hdrs.vars_,
				"Host",   Znk_strlen_literal("Host"),
				hostname, Znk_strlen(hostname), true );

		/**
		 * HTTP GETメソッドの発行による接続および内容のダウンロード.
		 */
		result = RanoHtpBoy_do_get( hostname, ZnkStr_cstr(req_urp), target,
				&htp_hdrs,
				parent_proxy,
				tmpdir, cachebox,
				result_filename );

		/**
		 * HTTPヘッダの破壊.
		 */
		ZnkHtpHdrs_dispose( &htp_hdrs );

		ZnkStr_delete( req_urp );

		if( !result ){
			ZnkStr_addf( ermsg,
					"Error : RanoHtpBoy_do_get result=[%d] result_filename=[%s]\n",
					result, ZnkStr_cstr(result_filename) );
			goto FUNC_END;
		}
	}

	result = true;
FUNC_END:
	ZnkNetBase_finalize();
	return result;
}

bool
ProxyFinder_analysis( const char* analysing_filename, const char* parent_proxy_filename,
		const char* ip_ptn_begin,   const char* ip_ptn_end,
		const char* port_ptn_begin, const char* port_ptn_end,
		bool is_hex_port, ZnkStr ermsg )
{
	bool         result              = false;
	const bool   elem_responsibility = true;
	ProxyInfoAry list = ProxyInfoAry_create( elem_responsibility );
	ZnkStr       line = ZnkStr_new( "" );
	char         ip_buf[ 192 ]  = "";
	char         port_buf[ 16 ] = "";
	uint16_t     port = 80;
	ZnkFile      fp = NULL;
	bool         found = false;

	ZnkBmsFinder proxy_ip_finder   = ZnkBmsFinder_create( ip_ptn_begin,   ip_ptn_end );
	ZnkBmsFinder proxy_port_finder = ZnkBmsFinder_create( port_ptn_begin, port_ptn_end );

	fp = Znk_fopen( analysing_filename, "rb" );
	if( fp == NULL ){
		ZnkStr_addf( ermsg,
				"Error : Cannot open analysing_filename=[%s]\n", analysing_filename );
		goto FUNC_END;
	}
	while( ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
		found = true;

		/* Get IP */
		if( !ZnkBmsFinder_getBetweenCStr( proxy_ip_finder, ZnkStr_cstr(line), ZnkStr_leng(line), ip_buf, sizeof(ip_buf) ) ){
			continue;
		}

		/* Get Port */
		if( !ZnkBmsFinder_getBetweenCStr( proxy_port_finder, ZnkStr_cstr(line), ZnkStr_leng(line), port_buf, sizeof(port_buf) ) ){
			/* IPが取得できたにも関わらずPortが取得できない場合、次の行から順に探索 */
			found = false;
			while( ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
				if( ZnkBmsFinder_getBetweenCStr( proxy_port_finder, ZnkStr_cstr(line), ZnkStr_leng(line), port_buf, sizeof(port_buf) ) ){
					found = true;
					break;
				}
			}
		}

		if( found == false ){
			/* Unexpected */
			ZnkStr_addf( ermsg, "Error : Unexpected EOF.\n" );
			break;
		}

		if( is_hex_port ){
			/* portが16進数で記述されているサイトもあり、これに対応したもの */
			if( !ZnkS_getU16X( &port, port_buf ) ){
				ZnkStr_addf( ermsg, "Error : port number is not hexadecimal [%s].\n", port_buf );
				continue;
			}
		} else {
			if( !ZnkS_getU16U( &port, port_buf ) ){
				ZnkStr_addf( ermsg, "Error : port number is not decimal [%s].\n", port_buf );
				continue;
			}
		}
		ProxyInfoAry_regist( list, ip_buf, port, ProxyState_e_UNKN, "?" );
		ZnkStr_addf( ermsg, "Report : Get [%s:%zu].\n", ip_buf, port );
	}

	if( ProxyInfoAry_save( list, parent_proxy_filename, false ) ){
		ZnkStr_addf( ermsg, "Report : OK. We save [%s] successfully.\n", parent_proxy_filename );
	} else {
		ZnkStr_addf( ermsg, "Error : Cannot save [%s].\n", parent_proxy_filename );
	}

	result = true;
FUNC_END:
	Znk_fclose( fp );
	ZnkBmsFinder_destroy( proxy_ip_finder );
	ZnkBmsFinder_destroy( proxy_port_finder );
	if( list ){ ProxyInfoAry_destroy( list ); }
	ZnkStr_delete( line );
	return result;
}
