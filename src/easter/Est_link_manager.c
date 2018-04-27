#include <Est_link_manager.h>
#include <Est_config.h>
#include <Est_ui.h>
#include <Est_unid.h>
#include <Est_hint_manager.h>
#include <Est_base.h>
#include <Est_linf_list.h>
#include <Est_box_ui.h>

#include <Znk_htp_util.h>
#include <Znk_str_ex.h>
#include <Znk_missing_libc.h>
#include <Znk_nset.h>
#include <Znk_dir.h>

#define IS_OK( val ) (bool)( (val) != NULL )


/* Easter-URLが与えられた場合は標準URLに直す. */
static void
modifyEasterURL_toStdURL( ZnkStr unesc_url, ZnkStr msg )
{
	const char* p = ZnkStrEx_strstr( unesc_url, "/easter?" );
	/***
	 * パターン p が存在した場合は Easter-URLが与えられたとみなす.
	 */
	if( p ){
		const char* q = NULL;
		ZnkSRef ptn = { 0 };
		ZnkStr tmp = ZnkStr_new( "" );

		ZnkStr_addf( msg, "default:config : this is easter url. it is modified to standard url automatically.\n" );

		/***
		 * Easter-URLは元々がURL-Encodingされているため、
		 * ここでまずさらにこれをunescapeしておく必要がある.
		 * (逆に言えばEaster-URLでない場合はこの処理を行うべきではない.
		 * 元々%エンコードされているURLであればそれを不用意にデコードすべきではない)
		 */
		ZnkHtpURL_unescape_toStr( tmp, ZnkStr_cstr(unesc_url), ZnkStr_leng(unesc_url) );
		ZnkStr_swap( tmp, unesc_url );

		/***
		 * pが無効値になるため再取得.
		 */
		p = ZnkStrEx_strstr( unesc_url, "/easter?" );

		/***
		 * 以降に以下のいずれかのptnが存在した場合、その直後のポインタqを取得.
		 */
		do {
			ZnkSRef_set_literal( &ptn, "est_view=" );
			q = Znk_strstr( p, ptn.cstr_ );
			if( q ){ q += ptn.leng_; break; }

			ZnkSRef_set_literal( &ptn, "est_get=" );
			q = Znk_strstr( p, ptn.cstr_ );
			if( q ){ q += ptn.leng_; break; }

		} while( false );

		if( q ){
			ZnkStr_cut_front( unesc_url, q - ZnkStr_cstr(unesc_url) );
		}

		ZnkStr_delete( tmp );
	}
}

static bool 
registNewLink( const char* est_link_new_url, const char* est_link_new_comment,
		ZnkVarpAry linf_list, ZnkStr msg, ZnkStr new_linkid, const char* elem_prefix )
{
	const size_t size = EstLInfList_size(linf_list);
	size_t idx;
	bool found = false;
	ZnkStr unesc_url     = ZnkStr_new( "" );
	ZnkStr unesc_comment = ZnkStr_new( "" );
	const char* url     = NULL;

	ZnkHtpURL_unescape_toStr( unesc_url,     est_link_new_url,     Znk_strlen(est_link_new_url) );
	if( est_link_new_comment ){
		ZnkHtpURL_unescape_toStr( unesc_comment, est_link_new_comment, Znk_strlen(est_link_new_comment) );
	}

	modifyEasterURL_toStdURL( unesc_url, msg );

	for( idx=0; idx<size; ++idx ){
		EstLInf linf = EstLInfList_at( linf_list, idx );
		url     = EstLInf_url( linf );
		/***
		 * commentとunesc_commentの比較はやめた方がよいだろう.
		 * 違うURLでも同じコメントになるというのは偶然の事故としてあり得るからである.
		 */
		if( ZnkS_eqCase( url, ZnkStr_cstr(unesc_url) )){
			found = true;
			break;
		}
	}
	if( found ){
		EstLInf linf = EstLInfList_at( linf_list, idx );
		EstLInf_set_url(     linf, ZnkStr_cstr(unesc_url) );
		EstLInf_set_comment( linf, ZnkStr_cstr(unesc_comment) );

		ZnkStr_addf( msg, "Est_link_manager : registNewLink : [%s] is updated.", ZnkStr_cstr(unesc_url) );
	} else {
		char buf[ 256 ] = "";
		char new_id[ 256 ] = "";
		EstUNID_issue( buf, sizeof(buf) );
		Znk_snprintf( new_id, sizeof(new_id), "link_%s", buf );

		EstLInfList_add( linf_list, new_id, ZnkStr_cstr(unesc_url), ZnkStr_cstr(unesc_comment) );

		ZnkStr_addf( msg, "Est_link_manager : registNewLink : [%s] is saved newly.", ZnkStr_cstr(unesc_url) );
		ZnkStr_set( new_linkid, new_id );
	}

	ZnkStr_delete( unesc_url );
	ZnkStr_delete( unesc_comment );

	return true;
}

static size_t
removeLink( ZnkVarpAry post_vars, ZnkVarpAry list, ZnkStr msg, const char* elem_prefix )
{
	/***
	 * ＠正順削除バグ:
	 * 例えば elem_ary = { elem0, elem1, elem2, elem3 }とする.
	 * また1番目の要素と2番目の要素をまとめて削除したいとする.
	 * このとき、もし elem_ary を正順(つまり0から要素数-1まで昇順)に配列indexによるイテレートで探索し
	 * その順番で削除してしまうと、最初の1番目の要素は正しく削除されるが2番目の要素以降取得にズレが発生して
	 * 誤った要素が削除されてしまう(この例の場合、elem3が削除されてしまう).
	 * この種のバグをここでは「正順削除バグ」と呼ぶ.
	 * これを防ぐには正順ではなく逆順にイテレートする.
	 *
	 * ＠二重POST削除:
	 * 例えば elem_ary = { elem0, elem1, elem2, elem3 }とする.
	 * また1番目の要素を削除したいとする.
	 * この結果、elem_ary = { elem0, elem2, elem3 }となる.
	 * CGIでは一度送信した後の画面での状態を例えばブラウザの戻るボタンなどでもう一度復元し得る.
	 * そして再び「1番目の要素を削除する」という要求を送信することができてしまう.
	 * するとユーザの意に反して今度はelem2が削除されてしまい、elem_ary = { elem0, elem3 }となる.
	 * この種の現象をここでは「二重POST削除」と呼ぶ.
	 * これを防ぐにはelem_aryを単なる要素の配列ではなくID付きの変数の配列にしなければならない.
	 * またこのIDは近い時間では被らないような発行の仕方にすべきである.
	 */
	const size_t   post_vars_size = ZnkVarpAry_size( post_vars );
	const ZnkVarp* post_vars_ptr  = ZnkVarpAry_ary_ptr( post_vars );
	ZnkVarp        post_varp = NULL;
	size_t         erase_count = 0;
	const char*    id = NULL;
	size_t         erase_idx = Znk_NPOS;
	size_t idx;

	for( idx=0; idx<post_vars_size; ++idx ){
		post_varp = post_vars_ptr[ idx ];
		if( ZnkS_isBegin( ZnkVar_name_cstr( post_varp ), elem_prefix )){
			id = ZnkVar_name_cstr( post_varp );
			erase_idx = EstLInfList_findIdx_byName( list, id );
			if( erase_idx != Znk_NPOS ){
				/* IDによる取得の直後のため、この場合は正順でも問題ない */
				ZnkStr_addf( msg, "default:config remove_link : id=[%s]\n", id );
				EstLInfList_erase_byIdx( list, erase_idx );
				++erase_count;
			}
		}
	}
	return erase_count;
}

static bool
sortCategory( ZnkVarpAry post_vars, ZnkVarpAry list, ZnkStr msg, const char* elem_prefix )
{
	const size_t list_size = ZnkVarpAry_size( list );
	EstLInf      linf = NULL;
	size_t       idx;
	const char*  target = NULL;

	ZnkVarpAry   stocks = ZnkVarpAry_create( true );
	ZnkVarp      refs = NULL;
	size_t       refs_idx = 0;
	ZnkNSet32    nset = ZnkNSet32_create();
	ZnkObjAry    oda = NULL;
	//void*        oda_ptr = NULL;
	size_t       oda_size = 0;
	size_t       nset_size = 0;
	//size_t       offset = 0;
	const char*  url = NULL;
	char hostname[ 256 ] = "";

	{
		ZnkMyf target_myf = EstConfig_target_myf();
		const size_t  size = ZnkMyf_numOfSection( target_myf );
		ZnkMyfSection sec;
		const char*   target = NULL;
		for( idx=0; idx<size; ++idx ){
			sec    = ZnkMyf_atSection( target_myf, idx );
			target = ZnkMyfSection_name( sec );
			refs   = ZnkVarp_create( target, "", 0, ZnkPrim_e_ObjAry, NULL );
			ZnkVarpAry_push_bk( stocks, refs );
		}
		refs = ZnkVarp_create( "others", "", 0, ZnkPrim_e_ObjAry, NULL );
		ZnkVarpAry_push_bk( stocks, refs );
	}

	for( idx=0; idx<list_size; ++idx ){
		linf = EstLInfList_at( list, idx );
		url = EstLInf_url( linf );
		{
			const char* q = RanoCGIUtil_skipProtocolPrefix( url );
			const char* r = Znk_strchr( q, '/' );
			ZnkS_copy( hostname, sizeof(hostname), q, r ? r-q : Znk_NPOS );
		}
		target = EstConfig_findTargetName( hostname );
		if( target == NULL ){
			target = "others";
		}
		refs_idx = ZnkVarpAry_findIdx_byName( stocks, target, Znk_NPOS, false );
		if( refs_idx != Znk_NPOS ){
			ZnkNSet32_push( nset, refs_idx );
			refs = ZnkVarpAry_at( stocks, refs_idx );
		}
		oda = ZnkPrim_objAry( ZnkVar_prim( refs ) );
		if( oda ){
			ZnkObjAry_push_bk( oda, (ZnkObj)linf );
		}
	}

	nset_size = ZnkNSet32_size( nset );
	for( idx=0; idx<nset_size; ++idx ){
		refs_idx = ZnkNSet32_at( nset, idx );
		refs     = ZnkVarpAry_at( stocks, refs_idx );
		oda      = ZnkPrim_objAry( ZnkVar_prim( refs ) );
		oda_size = ZnkObjAry_size( oda );
		{
			size_t oda_idx;
			const char* id;
			size_t target_idx = 0;
			for( oda_idx=0; oda_idx<oda_size; ++oda_idx ){
				linf = (EstLInf)ZnkObjAry_at( oda, oda_idx );
				id   = EstLInf_id( linf );
				target_idx = EstLInfList_find( list, id );
				if( target_idx != Znk_NPOS ){
					EstLInfList_shiftToLast( list, target_idx );
				}
			}
		}
		//Znk_memmove( list_ptr + offset, oda_ptr, oda_size*sizeof(ZnkVarp) );
		//offset += oda_size;
	}

	ZnkNSet32_destroy( nset );

	return true;
}


static size_t
getEditLinkIdx( ZnkVarpAry post_vars, ZnkVarpAry list, ZnkStr msg, const char* elem_prefix )
{
	const size_t   post_vars_size = ZnkVarpAry_size( post_vars );
	const ZnkVarp* post_vars_ptr  = ZnkVarpAry_ary_ptr( post_vars );
	ZnkVarp        post_varp = NULL;
	const char*    id = NULL;
	size_t         first_selected_idx = Znk_NPOS;
	size_t         idx;
	for( idx=0; idx<post_vars_size; ++idx ){
		post_varp = post_vars_ptr[ idx ];
		if( ZnkS_isBegin( ZnkVar_name_cstr( post_varp ), elem_prefix )){
			id = ZnkVar_name_cstr( post_varp );
			first_selected_idx = ZnkVarpAry_findIdx_byName( list, id, Znk_NPOS, false );
			if( first_selected_idx != Znk_NPOS ){
				ZnkStr_addf( msg, "default:config open_editlink : id=[%s]\n", id );
				return first_selected_idx;
			}
		}
	}
	return Znk_NPOS;
}

void
EstLinkManager_main( RanoCGIEVar* evar, ZnkVarpAry post_vars, ZnkStr msg, const char* authentic_key )
{
	ZnkBird bird = ZnkBird_create( "#[", "]#" );
	const char* template_html_file = "templates/link_manager.html";
	struct ZnkHtpHdrs_tag htp_hdrs = { 0 };
	RanoModule mod = NULL;
	ZnkVarpAry  linf_list  = EstLInfList_create();
	ZnkStr     pst_str    = ZnkStr_new( "" );
	ZnkStr     new_linkid = ZnkStr_new( "" );
	bool   is_authenticated = false;
	bool   is_unescape_val = false;
	const bool is_xhr_authhost = EstConfig_isXhrAuthHost( evar );
	ZnkVarp varp;
	const char* profile_dir = EstConfig_profile_dir();
	char link_myf_path[ 256 ] = "";

	Znk_snprintf( link_myf_path, sizeof(link_myf_path), "%s/link_list.myf", profile_dir );
	if( ZnkDir_getType( link_myf_path ) != ZnkDirType_e_File ){ 
		ZnkDir_copyFile_byForce( "default/link_list.myf", link_myf_path, NULL );
	}
	EstLInfList_load( linf_list, link_myf_path );

	RanoCGIUtil_getPostedFormData( evar, post_vars, mod, &htp_hdrs, pst_str, NULL, is_unescape_val );

	if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "Moai_AuthenticKey", false ) )
	  && ZnkS_eq( authentic_key, ZnkVar_cstr(varp) ) ){
		is_authenticated = true;
	} else {
	}
	ZnkStr_addf( msg, "is_authenticated=[%d]\n", is_authenticated );
	ZnkStr_addf( msg, "is_xhr_authhost=[%d]\n",  is_xhr_authhost );
	ZnkStr_addf( msg, "xhr_authhost=[%s]\n",  EstConfig_XhrAuthHost() );
	ZnkStr_addf( msg, "evar->server_name_=[%s]\n", evar->server_name_ );
	ZnkStr_addf( msg, "evar->server_port_=[%s]\n", evar->server_port_ );

	ZnkStr_addf( msg, "query_string=[%s]\n", evar->query_string_ );

	ZnkBird_regist( bird, "EstLM_url_val", "" );
	ZnkBird_regist( bird, "EstLM_comment_val", "" );
	ZnkBird_regist( bird, "EstLM_edit_ui", "" );

	if( is_authenticated && IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "command", false ) )){
		template_html_file = "templates/link_manager.html";

		if( ZnkS_eq( ZnkVar_cstr(varp), "save_link" ) ){
			const char* EstLM_url     = NULL;
			const char* EstLM_comment = NULL;
			if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "EstLM_url", false ) )){
				EstLM_url = ZnkVar_cstr(varp);
			}
			if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "EstLM_comment", false ) )){
				EstLM_comment = ZnkVar_cstr(varp);
			}

			if( EstLM_url ){
				if( registNewLink( EstLM_url, EstLM_comment, linf_list, msg, new_linkid, "link" ) ){
					EstLInfList_save( linf_list, link_myf_path );
				}
			}
		} else if( ZnkS_eq( ZnkVar_cstr(varp), "remove_link" ) ){
			ZnkVarp confirm;
			ZnkStr  result_view = ZnkStr_new( "" );
			bool is_confirm = false;
			if( IS_OK( confirm = ZnkVarpAry_find_byName_literal( post_vars, "confirm", false ) )){
				if( ZnkS_eq( ZnkVar_cstr(confirm), "on" ) ){
					is_confirm = true;
				}
			}
			if( is_confirm ){
				template_html_file = "templates/remove_confirm.html";

				{
					const size_t   post_vars_size = ZnkVarpAry_size( post_vars );
					const ZnkVarp* post_vars_ptr  = ZnkVarpAry_ary_ptr( post_vars );
					ZnkVarp        post_varp = NULL;
					size_t  idx;
					EstLInf linf;
					const char* id;
					size_t target_idx;
					for( idx=0; idx<post_vars_size; ++idx ){
						post_varp = post_vars_ptr[ idx ];
						id = ZnkVar_name_cstr( post_varp );
						if( ZnkS_isBegin( id, "link_" )){
							target_idx = EstLInfList_find( linf_list, id );
							if( target_idx != Znk_NPOS ){
								const char* comment;
								const char* val;
								linf = EstLInfList_at( linf_list, target_idx );
								comment = EstLInf_comment( linf );
								val     = EstLInf_url( linf );
								ZnkStr_addf( result_view,
										"<input type=checkbox id=%s name=%s value=\"%s\" checked>\n",
										id, id, val );
								ZnkStr_addf( result_view,
										"<div class=MstyAutoLinkOther>%s</div><br>\n",
										comment );
							}
						}
					}
				}
				ZnkStr_add( result_view, "<br>\n" );
				ZnkStr_add( result_view, "<font size=-1>\n" );
				ZnkStr_add( result_view, "上記のものを削除しようとしています.<br>\n" );
				ZnkStr_add( result_view, "よろしいですか？<br>\n" );
				ZnkStr_add( result_view, "</font>\n" );
				ZnkStr_add( result_view, "<br>\n" );

				ZnkStr_addf( result_view, "<a class=MstyElemLink href=\"javascript:EjsAssort_submitCommandAndArg( document.fm_main, 'link', 'remove_link', '' )\">" );
				ZnkStr_addf( result_view, "はい、実際に削除します.</a><br>\n" );

				ZnkStr_addf( result_view, "<a class=MstyElemLink href=\"/easter\">" );
				ZnkStr_addf( result_view, "いいえ、何もせず元に戻ります.</a><br>\n" );
			} else {
				size_t erase_count = removeLink( post_vars, linf_list, msg, "link" );
				if( erase_count ){
					EstLInfList_save( linf_list, link_myf_path );
				}
				template_html_file = "templates/command_complete.html";
				ZnkStr_addf( result_view, "%zu 件削除しました.\n", erase_count );
				ZnkStr_addf( result_view, "<a class=MstyElemLink href=\"/easter\">" );
				ZnkStr_addf( result_view, "戻る</a><br>\n" );
			}

			ZnkBird_regist( bird, "manager", "link" );
			ZnkBird_regist( bird, "result_view",   ZnkStr_cstr(result_view) );
			ZnkStr_delete( result_view );

		} else if( ZnkS_eq( ZnkVar_cstr(varp), "age_link" ) ){
			size_t age_count = EstBase_agePtrObjs( linf_list, post_vars );
			if( age_count ){
				EstLInfList_save( linf_list, link_myf_path );
			}

		} else if( ZnkS_eq( ZnkVar_cstr(varp), "sort_category" ) ){
			bool is_updated = sortCategory( post_vars, linf_list, msg, "link" );
			if( is_updated ){
				EstLInfList_save( linf_list, link_myf_path );
			}

		} else if( ZnkS_eq( ZnkVar_cstr(varp), "open_registlink" ) ){
			ZnkStr EstLM_edit_ui = ZnkStr_new( "" );
			ZnkStr_addf( EstLM_edit_ui,
					"Linkの新規登録<br>\n"
					"<input class=MstyInputField type=text name=EstLM_url     placeholder=\"URL\"  value=\"\" size=50><br>\n"
					"<input class=MstyInputField type=text name=EstLM_comment placeholder=\"説明\" value=\"\" size=50><br>\n" );
			ZnkStr_addf( EstLM_edit_ui,
					"<button class=MstyWideButton type='submit' name='command' value='save_link'>上記で登録</button><br>\n" );
			ZnkBird_regist( bird, "EstLM_edit_ui", ZnkStr_cstr( EstLM_edit_ui ) );
			ZnkStr_delete( EstLM_edit_ui );

		} else if( ZnkS_eq( ZnkVar_cstr(varp), "open_editlink" ) ){
			size_t edit_idx = getEditLinkIdx( post_vars, linf_list, msg, "link" );
			if( edit_idx == Znk_NPOS ){
				ZnkBird_regist( bird, "EstLM_edit_ui", "リンクが選択されていません." );
			} else {
				EstLInf linf = EstLInfList_at( linf_list, edit_idx );
				if( linf ){
					const char* id      = EstLInf_id( linf );
					const char* url     = EstLInf_url( linf );
					const char* comment = EstLInf_comment( linf );
					ZnkStr EstLM_edit_ui = ZnkStr_new( "" );
					ZnkStr_addf( EstLM_edit_ui,
							"ID=[%s]<br>\n"
							"<input type=hidden name=EstLM_update_id value=\"%s\">\n"
							"<input class=MstyInputField type=text name=EstLM_url     placeholder=\"URL\"  value=\"%s\" size=50><br>\n"
							"<input class=MstyInputField type=text name=EstLM_comment placeholder=\"説明\" value=\"%s\" size=50><br>\n",
							id, id, url, comment );
					ZnkStr_addf( EstLM_edit_ui,
							"<button class=MstyWideButton type='submit' name='command' value='update_link'>更新</button><br>\n" );
					ZnkBird_regist( bird, "EstLM_edit_ui", ZnkStr_cstr( EstLM_edit_ui ) );
					ZnkStr_delete( EstLM_edit_ui );
				}

			}

		} else if( ZnkS_eq( ZnkVar_cstr(varp), "update_link" ) ){
			if( IS_OK( varp = ZnkVarpAry_find_byName_literal( post_vars, "EstLM_update_id", false ) )){
				const char* EstLM_url     = NULL;
				const char* EstLM_comment = NULL;
				const char* id = ZnkVar_cstr( varp );
				ZnkStr unesc_url     = ZnkStr_new( "" );
				ZnkStr unesc_comment = ZnkStr_new( "" );
				size_t idx;

				if( IS_OK( varp = ZnkVarpAry_findObj_byName_literal( post_vars, "EstLM_url", false ) )){
					EstLM_url = ZnkVar_cstr(varp);
					ZnkHtpURL_unescape_toStr( unesc_url, EstLM_url, Znk_strlen(EstLM_url) );
					modifyEasterURL_toStdURL( unesc_url, msg );
				}
				if( IS_OK( varp = ZnkVarpAry_findObj_byName_literal( post_vars, "EstLM_comment", false ) )){
					EstLM_comment = ZnkVar_cstr(varp);
					ZnkHtpURL_unescape_toStr( unesc_comment, EstLM_comment, Znk_strlen(EstLM_comment) );
				}

				idx = EstLInfList_findIdx_byName( linf_list, id );
				if( idx != Znk_NPOS ){
					EstLInf linf = EstLInfList_at( linf_list, idx );
					EstLInf_set_url(     linf, ZnkStr_cstr(unesc_url) );
					EstLInf_set_comment( linf, ZnkStr_cstr(unesc_comment) );
					EstLInfList_save( linf_list, link_myf_path );
				}
				ZnkStr_delete( unesc_url );
				ZnkStr_delete( unesc_comment );
			}

		} else {
			ZnkStr_addf( msg, "default:config unknown command [%s].", ZnkVar_cstr(varp) );
		}

	} else {
		ZnkStr_add( msg, "default:others.\n" );
	}

	{
		ZnkStr links = ZnkStr_new( "" );
		ZnkStr url_prefix = ZnkStr_new( "" );
		const char* xhr_dmz = EstConfig_XhrDMZ();
		ZnkStr_setf( url_prefix, "http://%s/easter?est_view=", xhr_dmz );

		EstBoxUI_make_forLinks( links, linf_list,
				0, Znk_NPOS, authentic_key );

		ZnkBird_regist( bird, "links", ZnkStr_cstr(links) );
		ZnkStr_delete( url_prefix );
		ZnkStr_delete( links );
	}

	ZnkBird_regist( bird, "Moai_AuthenticKey", authentic_key );
	{
		ZnkStr hint_table = EstHint_getHintTable( "link_manager" );
		if( hint_table ){
			ZnkBird_regist( bird, "hint_table", ZnkStr_cstr(hint_table) );
		} else {
			ZnkBird_regist( bird, "hint_table", "" );
		}
	}
	ZnkHtpURL_negateHtmlTagEffection( msg ); /* for XSS */
	RanoCGIUtil_replaceNLtoHtmlBR( msg );
	ZnkBird_regist( bird, "msg",   ZnkStr_cstr(msg) );
	RanoCGIUtil_printTemplateHTML( evar, bird, template_html_file );
	ZnkBird_destroy( bird );
	ZnkStr_delete( pst_str );
	ZnkStr_delete( new_linkid );
}

