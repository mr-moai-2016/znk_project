#include <CB_custom_feature.h>
#include <CB_vars_base.h>
#include <CB_finger.h>

#include <Znk_htp_util.h>
#include <Znk_md5.h>
#include <Znk_dir.h>

typedef enum {
	 Cmd_e_Get
	,Cmd_e_FgpReal
	,Cmd_e_FgpStep1
	,Cmd_e_FgpStep2
	,Cmd_e_FgpUpdate
	,Cmd_e_FgpClear
	,Cmd_e_FgpSavePng
} CmdType;

static CmdType
getCmdType( ZnkVarpAry cb_vars )
{
	CmdType cmd_type = Cmd_e_Get;
	ZnkVarp varp = ZnkVarpAry_find_byName_literal( cb_vars, "cb_arg", false );
	if( varp ){
		if( ZnkS_eq(ZnkVar_cstr(varp),"fgp_real") ){
			cmd_type = Cmd_e_FgpReal;
		} else if( ZnkS_eq(ZnkVar_cstr(varp),"fgp_step1") ){
			cmd_type = Cmd_e_FgpStep1;
		} else if( ZnkS_eq(ZnkVar_cstr(varp),"fgp_step2") ){
			cmd_type = Cmd_e_FgpStep2;
		} else if( ZnkS_eq(ZnkVar_cstr(varp),"fgp_update") ){
			cmd_type = Cmd_e_FgpUpdate;
		} else if( ZnkS_eq(ZnkVar_cstr(varp),"fgp_clear") ){
			cmd_type = Cmd_e_FgpClear;
		} else if( ZnkS_eq(ZnkVar_cstr(varp),"fgp_save_png") ){
			cmd_type = Cmd_e_FgpSavePng;
		}
	} else {
		cmd_type = Cmd_e_Get;
	}
	return cmd_type;
}

static void
convertBirdToInputHiddens( ZnkVarpAry input_hiddens, ZnkBird bird, const char* target )
{
	if( ZnkS_eq( target, "futaba" ) ){
		static const char* tbl[][2] = {
			{ "js",   "on" },
			{ "pthb", NULL },
			{ "pthc", NULL },
			{ "pthd", NULL },
			{ "ptua", NULL },
			{ "scsz", NULL },
			{ "pwd",  NULL },
			{ "flrv", NULL },
			{ "flvv", NULL },
		};
		const size_t size = Znk_NARY( tbl );
		size_t idx;

		for( idx=0; idx<size; ++idx ){
			const char* name = tbl[ idx ][ 0 ];
			const char* val  = ZnkBird_at( bird, name );
			if( val == NULL ){
				val = tbl[ idx ][ 1 ];
			}
			if( val ){
				ZnkVarp new_varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_Str, NULL );
				ZnkStr_assign( new_varp->name_, 0, name, Znk_NPOS );
				ZnkVar_set_val_Str( new_varp, val, Znk_NPOS );
				ZnkVarpAry_push_bk( input_hiddens, new_varp );
			}
		}
	}
}

static void
makePostVarsUI( ZnkStr ans, ZnkVarpAry input_hiddens )
{
	const size_t size = ZnkVarpAry_size( input_hiddens );
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		const char* style_class = ( idx % 2 ) ? "MstyItemOdd" : "MstyItemEvn";
		ZnkVarp varp = ZnkVarpAry_at( input_hiddens, idx );
		const char* key = ZnkVar_name_cstr( varp );
		const char* val = ZnkVar_cstr( varp );
		ZnkStr_addf( ans, "<tr class=%s><td class=MstyNoneWordBreak><b>%s</b></td><td>%s</td></tr>\n",
				style_class, key, val );
	}
}

static bool
getPtua64_fromVars( ZnkVarpAry vars, uint64_t* ptua64 )
{
	ZnkVarp varp = ZnkVarpAry_find_byName_literal( vars, "ptua", false );
	if( varp ){
		if( ZnkS_getU64U( ptua64, ZnkVar_cstr(varp) ) ){
			return true;
		}
	}
	return false;
}

static void
registScsz_byFgpInfo( ZnkBird bird, CBFgpInfo fgp_info )
{
	char scsz_buf[ 256 ] = "";
	Znk_snprintf( scsz_buf, sizeof(scsz_buf), "%sx%s", ZnkStr_cstr(fgp_info->Fgp_ScreenResolution_), ZnkStr_cstr(fgp_info->Fgp_colorDepth_) );
	ZnkBird_regist( bird, "scsz", scsz_buf );
}

static const char* st_snp_id_tbl[] = {
	"Snp_LtIE6",
	"Snp_LtIE7",
	"Snp_LtIE8",
	"Snp_IE9",
	"Snp_IE10",
	"Snp_IE11",
	"Snp_Trident",
	"Snp_Edge",
	"Snp_Gecko",
	"Snp_MozillaSidebar",
	"Snp_NavigatorOnline",
	"Snp_SessionStorage",
	"Snp_FunctionX5",
	"Snp_DocCurrentScript",
	"Snp_EventSource",
	"Snp_Crypto",
	"Snp_PerformanceNow",
	"Snp_AudioContext",
	"Snp_IndexedDB",
	"Snp_WindowStyles",
	"Snp_SendBeacon",
	"Snp_GetGamepads",
	"Snp_NavLanguages",
	"Snp_NavMediaDevices",
	"Snp_WinCaches",
	"Snp_CreateImageBitmap",
	"Snp_Onstorage",
	"Snp_NavGetBattery",
	"Snp_Presto",
	"Snp_LtChrome14",
	"Snp_ChromiumInFutaba",
	"Snp_Touch",
	"Snp_FacileMobile",
};


static uint64_t
getPtua64_fromCustomSnippet( const ZnkVarpAry cb_vars )
{
	uint64_t ptua64 = 0;
	ZnkVarp varp = NULL;
	size_t i;
	const char* id = "";

	for( i=0; i<Znk_NARY(st_snp_id_tbl); ++i ){
		id = st_snp_id_tbl[ i ];
		varp = ZnkVarpAry_find_byName( cb_vars, id, Znk_NPOS, false );
		if( varp ){
			ptua64 |= ( UINT64_C( 1 ) << i );
		}
	}
	return ptua64;
}
static void
registU64_toVars( ZnkVarpAry vars, const char* var_name, const uint64_t u64 )
{
	char tmp[ 256 ] = "";
	Znk_snprintf( tmp, sizeof(tmp), "%I64u", u64 );
	CBVarsBase_registStr_toVars( vars, var_name, tmp, Znk_strlen(tmp) );
}
static void
registFinger( ZnkVarpAry cb_vars, ZnkVarpAry main_vars, ZnkStr RE_key, CBFgpInfo fgp_info )
{
	bool with_UA = false;
	bool with_PluginStrings = false;
	bool with_ScreenResolution = false;
	uint32_t hs = 0;
	ZnkStr tmp      = ZnkStr_new( "" );

	with_UA = false;
	with_PluginStrings = false;
	with_ScreenResolution = false;
	hs = CBFgpInfo_calcHash( fgp_info, with_UA, with_PluginStrings, with_ScreenResolution, ZnkStr_cstr(RE_key) );
	ZnkStr_setf( tmp, "%u", hs );
	CBVarsBase_registStr_toVars( cb_vars,   "flrv", ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );
	CBVarsBase_registStr_toVars( main_vars, "flrv", ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );

	with_UA = false;
	with_PluginStrings = true;
	with_ScreenResolution = false;
	hs = CBFgpInfo_calcHash( fgp_info, with_UA, with_PluginStrings, with_ScreenResolution, ZnkStr_cstr(RE_key) );
	ZnkStr_setf( tmp, "%u", hs );
	CBVarsBase_registStr_toVars( cb_vars,   "flvv", ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );
	CBVarsBase_registStr_toVars( main_vars, "flvv", ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );

	ZnkStr_delete( tmp );
}
static void
registUAStr( ZnkVarpAry cb_vars, ZnkVarpAry main_vars, ZnkStr ua_str )
{
	CBVarsBase_registStr_toVars( cb_vars,   "UserAgent", ZnkStr_cstr(ua_str), ZnkStr_leng(ua_str) );
	CBVarsBase_registStr_toVars( main_vars, "UserAgent", ZnkStr_cstr(ua_str), ZnkStr_leng(ua_str) );
}
static void
registScszStr( ZnkVarpAry cb_vars, ZnkVarpAry main_vars, ZnkStr scsz_str )
{
	CBVarsBase_registStr_toVars( cb_vars,   "scsz", ZnkStr_cstr(scsz_str), ZnkStr_leng(scsz_str) );
	CBVarsBase_registStr_toVars( main_vars, "scsz", ZnkStr_cstr(scsz_str), ZnkStr_leng(scsz_str) );
}

static CBStatus
processFgpStep2( ZnkBird bird, ZnkVarpAry cb_vars, ZnkVarpAry main_vars, uint64_t* ptua64,
		CBFgpInfo fgp_info, ZnkStr RE_key, RanoCGIEVar* evar, const char* cb_src,
		ZnkStr lacking_var, ZnkStr ermsg, bool all_cookie_clear )
{
	ZnkStr caco   = NULL;
	ZnkStr ua_str = NULL;
	ZnkStr scsz_str = NULL;

	CBStatus cb_status = CBStatus_e_Sealed;
	ZnkVarp fltp = NULL;

	ZnkStr_add( ermsg, "<b>FgpStep2:</b>" );

	/* ptua64 */
	{
		*ptua64 = getPtua64_fromCustomSnippet( cb_vars );
		CBVirtualizer_registPtuaCtx( bird, *ptua64 );
		registU64_toVars( main_vars, "ptua", *ptua64 );
		registU64_toVars( cb_vars, "ptua", *ptua64 );
	}

	registFinger( cb_vars, main_vars, RE_key, fgp_info );
	CBVirtualizer_registBird_byVars( bird, cb_vars );

	fltp = ZnkVarpAry_find_byName( cb_vars, "Fgp_userAgent", Znk_NPOS, false );
	if( fltp && ZnkVar_str_leng( fltp ) ){
		ua_str = ZnkStr_new( "" );
		ZnkHtpURL_unescape_toStr( ua_str, ZnkVar_cstr(fltp), ZnkVar_str_leng(fltp) );
	} else {
		ZnkStr_set( lacking_var, "Fgp_userAgent" );
		cb_status = CBStatus_e_InputLacking;
		goto FUNC_END;
	}
	registUAStr( cb_vars, main_vars, ua_str );

	if( ZnkStr_empty(fgp_info->Fgp_ScreenResolution_) ){
		ZnkStr_set( lacking_var, "Fgp_ScreenResolution" );
		cb_status = CBStatus_e_InputLacking;
		goto FUNC_END;
	}
	if( ZnkStr_empty(fgp_info->Fgp_colorDepth_) ){
		ZnkStr_set( lacking_var, "Fgp_colorDepth" );
		cb_status = CBStatus_e_InputLacking;
		goto FUNC_END;
	}

	scsz_str = ZnkStr_new( "" );
	ZnkStr_setf( scsz_str, "%sx%s", ZnkStr_cstr(fgp_info->Fgp_ScreenResolution_), ZnkStr_cstr(fgp_info->Fgp_colorDepth_) );
	registScszStr( cb_vars, main_vars, scsz_str );

	caco = ZnkStr_new( "" );
	if( CBVirtualizer_initiateAndSave( evar, cb_vars, ermsg, cb_src, caco, ua_str, lacking_var, all_cookie_clear ) ){
		ZnkStr_add( ermsg, "<b><font color=\"#773300\">OK. VirtualUSERS Initiation done successfully.</font></b>" );
		CBVirtualizer_registBird_byVars( bird, cb_vars );
		ZnkBird_regist( bird, "pthc", ZnkStr_cstr(caco) );
		CBVarsBase_registStr_toVars( main_vars, "pthc", ZnkStr_cstr(caco), ZnkStr_leng(caco) );
		cb_status = CBStatus_e_Sealed;
	} else {
		cb_status = CBStatus_e_InputLacking;
	}

FUNC_END:
	ZnkStr_delete( scsz_str );
	ZnkStr_delete( ua_str );
	ZnkStr_delete( caco );
	return cb_status;
}


static CBStatus
processFgpUpdate( ZnkBird bird, ZnkVarpAry cb_vars, ZnkVarpAry main_vars, uint64_t* ptua64,
		CBFgpInfo fgp_info, ZnkStr RE_key,
		ZnkStr lacking_var, ZnkStr ermsg )
{
	ZnkStr ua_str   = NULL;
	ZnkStr scsz_str = NULL;

	CBStatus cb_status = CBStatus_e_Editing;
	ZnkVarp fltp = NULL;

	ZnkStr_add( ermsg, "<b>FgpUpdate:</b>" );

	/* ptua64 */
	{
		*ptua64 = getPtua64_fromCustomSnippet( cb_vars );
		CBVirtualizer_registPtuaCtx( bird, *ptua64 );
		registU64_toVars( main_vars, "ptua", *ptua64 );
	}

	registFinger( cb_vars, main_vars, RE_key, fgp_info );
	CBVirtualizer_registBird_byVars( bird, cb_vars );

	fltp = ZnkVarpAry_find_byName( cb_vars, "Fgp_userAgent", Znk_NPOS, false );
	if( fltp && ZnkVar_str_leng( fltp ) ){
		ua_str = ZnkStr_new( "" );
		ZnkHtpURL_unescape_toStr( ua_str, ZnkVar_cstr(fltp), ZnkVar_str_leng(fltp) );
	} else {
		ZnkStr_set( lacking_var, "Fgp_userAgent" );
		cb_status = CBStatus_e_InputLacking;
		goto FUNC_END;
	}
	registUAStr( cb_vars, main_vars, ua_str );

	if( ZnkStr_empty(fgp_info->Fgp_ScreenResolution_) ){
		ZnkStr_set( lacking_var, "Fgp_ScreenResolution" );
		cb_status = CBStatus_e_InputLacking;
		goto FUNC_END;
	}
	if( ZnkStr_empty(fgp_info->Fgp_colorDepth_) ){
		ZnkStr_set( lacking_var, "Fgp_colorDepth" );
		cb_status = CBStatus_e_InputLacking;
		goto FUNC_END;
	}

	scsz_str = ZnkStr_new( "" );
	ZnkStr_setf( scsz_str, "%sx%s", ZnkStr_cstr(fgp_info->Fgp_ScreenResolution_), ZnkStr_cstr(fgp_info->Fgp_colorDepth_) );
	registScszStr( cb_vars, main_vars, scsz_str );

FUNC_END:
	ZnkStr_delete( scsz_str );
	ZnkStr_delete( ua_str );

	return cb_status;
}

static void
drawCustomSnp( ZnkBird bird, const char* hint_base_Snp, uint64_t ptua64 )
{

	ZnkStr str = ZnkStr_new( "" );
	size_t i;
	const char* id;
	const char* val;

	for( i=0; i<Znk_NARY(st_snp_id_tbl); ++i ){
		val = ( ptua64 & ( UINT64_C( 1 ) << i ) ) ? "checked" : "";
		id = st_snp_id_tbl[ i ];
		ZnkStr_addf( str, "<tr><td class=\"rtd\"><input type=checkbox id=%s name=%s size=\"10\" %s>&nbsp;"
				"<b><a href=\"%s%s\" target=_blank>%s</a></b></td><tr>\n",
				id, id, val, hint_base_Snp, id, id );
	}

	/* 一応これも表示させておく */
	id = "Snp_Webkit";
	val = "";
	ZnkStr_addf( str, "<tr><td class=\"rtd\"><input type=checkbox id=%s name=%s size=\"10\" %s>&nbsp;"
			"<b><a href=\"%s%s\" target=_blank>%s</a></b></td><tr>\n",
			id, id, val, hint_base_Snp, id, id );

	ZnkBird_regist( bird, "CustomSnippet_ui", ZnkStr_cstr(str) );
	ZnkStr_delete( str );
}

static void
getMD5CheckSum( ZnkMD5CheckSum* checksum, const char* file_path )
{
	ZnkMD5_CTX md5ctx = {{ 0 }};
	ZnkMD5_getDigest_byFile( &md5ctx, file_path );
	ZnkMD5CheckSum_get_byContext( checksum, &md5ctx );
}

CBStatus
CBCustomFeature_main( RanoCGIEVar* evar, ZnkVarpAry cb_vars, const char* cb_src,
		ZnkBird bird, ZnkStr RE_key, CBFgpInfo fgp_info, struct CBUAInfo_tag* ua_info,
		ZnkVarpAry main_vars, uint64_t* ptua64, ZnkStr ermsg, ZnkStr category, ZnkStr lacking_var,
		CBConfigInfo* info, bool is_authenticated, bool all_cookie_clear )
{
	const char* moai_dir = CBConfig_moai_dir();
	const char* target = CBConfig_theNegotiatingTarget();
	const char* hint_base_Snp = CBConfig_hint_base_Snp();

	bool is_update_main = false;
	CBStatus cb_status = CBStatus_e_Editing;
	CBFgpStateMode fgp_state_mode = CBFgpStateMode_e_Uninitialized;
	ZnkStr postvars_ui = ZnkStr_new( "" );
	ZnkVarpAry input_hiddens = ZnkVarpAry_create( true );

	/***
	 * Not authenticated な場合はCmd_e_Getしか許可しない.
	 */
	CmdType cmd_type  = Cmd_e_Get;
	if( is_authenticated ){
		cmd_type  = getCmdType( cb_vars );
	}

	switch( cmd_type ){
	case Cmd_e_FgpReal:
		CBVirtualizer_load( main_vars, info->ua_state_filename_, "MainContext" );
		fgp_state_mode = CBFgpStateMode_e_CBGetting;
		CBVirtualizer_registBird_byFutabaSendMyf( bird, moai_dir );
		CBVirtualizer_registBird_byVars( bird, main_vars );

		/* MainContextよりptuaを得る */
		getPtua64_fromVars( main_vars, ptua64 );

		break;
	case Cmd_e_FgpStep1:
		CBVirtualizer_doMainProc( evar, cb_vars, cb_src, bird, true, false,
				RE_key, fgp_info, ua_info, main_vars, ptua64, ermsg, category, lacking_var, moai_dir, all_cookie_clear );
		fgp_state_mode = CBFgpStateMode_e_StockContext;
		CBVirtualizer_save( main_vars, info->ua_state_filename_, "MainContext" );
		CBVirtualizer_registBird_byVars( bird, main_vars );
		ZnkStr_add( ermsg, "Cmd_e_FgpStep1" );
		break;
	case Cmd_e_FgpStep2:
		cb_status = processFgpStep2( bird, cb_vars, main_vars, ptua64,
				fgp_info, RE_key, evar, cb_src,
				lacking_var, ermsg, all_cookie_clear );
		fgp_state_mode = CBFgpStateMode_e_SaveFile;
		CBVirtualizer_registBird_byVars( bird, main_vars );
		CBVirtualizer_save( main_vars, info->ua_state_filename_, "MainContext" );
		break;
	case Cmd_e_FgpUpdate:
		cb_status = processFgpUpdate( bird, cb_vars, main_vars, ptua64,
				fgp_info, RE_key,
				lacking_var, ermsg );
		fgp_state_mode = CBFgpStateMode_e_SaveFile;
		CBVirtualizer_registBird_byVars( bird, main_vars );
		CBVirtualizer_save( main_vars, info->ua_state_filename_, "MainContext" );
		break;
	case Cmd_e_FgpClear:
		/***
		 * SendMyf から値を得てUI上ではそれを表示.
		 * MainContextもその値に上書きする.
		 * つまりStep1状態を完全にキャンセル破棄する.
		 * またこのときFingerの状態は空にクリアすべき.
		 * (CustomFingerはStep1状態を修正する用途と割り切る考え方)
		 */
		CBVirtualizer_load( main_vars, info->ua_state_filename_, "MainContext" );
		CBVirtualizer_initVars_byFutabaSendMyf( main_vars, moai_dir );

		CBVirtualizer_registBird_byFutabaSendMyf( bird, moai_dir );
		{
			/* SendMyf より */
			const char* ptua_cstr = ZnkBird_at( bird, "ptua" );
			if( ptua_cstr ){
				ZnkS_getU64U( ptua64, ptua_cstr );
			}
		}
		CBVirtualizer_save( main_vars, info->ua_state_filename_, "MainContext" );
		cb_status = CBStatus_e_Sealed;
		CBVirtualizer_registPtuaCtx( bird, *ptua64 );

		break;
	case Cmd_e_FgpSavePng:
		{
			ZnkVarp varp = ZnkVarpAry_findObj_byName_literal( cb_vars, "Fgp_Canvas", false );
			if( varp ){
				const char* src_file_path = "tmp/fgp_canvas.png";
				if( CBFinger_saveREPng( src_file_path, ZnkVar_str(varp) ) ){
					char dst_file_path[ 256 ] = "";
					ZnkMD5CheckSum checksum = {{ 0 }};
					const char* md5_of_file = NULL;
					getMD5CheckSum( &checksum, src_file_path );
					md5_of_file = ZnkMD5CheckSum_cstr( &checksum );
					Znk_snprintf( dst_file_path, sizeof(dst_file_path), "RE/%s.png", md5_of_file );
					if( ZnkDir_copyFile_byForce( src_file_path, dst_file_path, ermsg ) ){
						if( ZnkDir_deleteFile_byForce( src_file_path ) ){
							ZnkStr_addf( ermsg, "FgpSavePng : [%s] to [%s] OK.\n", src_file_path, dst_file_path );
						} else {
							ZnkStr_addf( ermsg, "FgpSavePng : Error : [%s] cannot delete.\n", src_file_path );
						}
					} else {
						ZnkStr_addf( ermsg, "FgpSavePng : Error : [%s] cannot copy to [%s].\n", src_file_path, dst_file_path );
					}
				} else {
					ZnkStr_addf( ermsg, "FgpSavePng : Error : Cannot save to [%s].\n", src_file_path );
				}
			}
		}
		cb_status = processFgpUpdate( bird, cb_vars, main_vars, ptua64,
				fgp_info, RE_key,
				lacking_var, ermsg );
		fgp_state_mode = CBFgpStateMode_e_SaveFile;
		CBVirtualizer_registBird_byVars( bird, main_vars );
		CBVirtualizer_save( main_vars, info->ua_state_filename_, "MainContext" );
		break;
	default:
	{
		ZnkVarp ua = NULL;
		/* GET : SendMyf ptuaから値を得るべき */
		CBVirtualizer_load( main_vars, info->ua_state_filename_, "MainContext" );

		ua = ZnkVarpAry_find_byName( main_vars, "UserAgent", Znk_NPOS, false );
		if( ua && !ZnkS_empty(ZnkVar_cstr(ua)) ){
			CBVirtualizer_registBird_byVars( bird, main_vars );
			{
				ZnkVarp pthc = ZnkVarpAry_find_byName( main_vars, "pthc", Znk_NPOS, false );
				if( pthc && !ZnkS_empty( ZnkVar_cstr(pthc) ) ){
					cb_status = CBStatus_e_Sealed;
				}
			}
		} else {
			CBVirtualizer_registBird_byFutabaSendMyf( bird, moai_dir );
			cb_status = CBStatus_e_Sealed;
		}

		{
			/* SendMyf より */
			const char* ptua_cstr = ZnkBird_at( bird, "ptua" );
			if( ptua_cstr ){
				ZnkS_getU64U( ptua64, ptua_cstr );
			}
		}
		CBVirtualizer_registPtuaCtx( bird, *ptua64 );
		break;
	}
	}

	drawCustomSnp( bird, hint_base_Snp, *ptua64 );
	CBFgpInfo_proc_byStateMode( fgp_info, fgp_state_mode );
	if( cb_status == CBStatus_e_Editing ){
		registScsz_byFgpInfo( bird, fgp_info );
	}
	CBVirtualizer_registBirdFgp( bird, fgp_info, ZnkStr_cstr(RE_key), is_update_main );
	CBVirtualizer_registBirdCookie( bird, true, moai_dir );

	ZnkBird_regist( bird, "cb_target", target );
	{
		ZnkStr selectbar = ZnkStr_new( "" );

		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=%s&amp;cb_type=postvars\" >CustomUSERS</a> &nbsp",
				"MstyElemLink", target );
		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=%s&amp;cb_type=cookie\" >CustomCookie</a> &nbsp",
				"MstyElemLink", target );
		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=%s&amp;cb_type=feature\" >CustomFeature</a> &nbsp",
				"MstyNowSelectedLink", target );

		ZnkStr_add( selectbar, "<br>\n" );

		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=futaba&amp;cb_type=feature\" >for futaba</a> &nbsp",
				ZnkS_eq( target, "futaba" ) ? "MstyNowSelectedLink" : "MstyElemLink" );
		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=5ch&amp;cb_type=feature\" >for 5ch</a> &nbsp",
				ZnkS_eq( target, "5ch" ) ? "MstyNowSelectedLink" : "MstyElemLink" );
		ZnkBird_regist( bird, "SelectBar_ui", ZnkStr_cstr(selectbar) );
		ZnkStr_delete( selectbar );
	}
	convertBirdToInputHiddens( input_hiddens, bird, target );
	makePostVarsUI( postvars_ui, input_hiddens );
	ZnkBird_regist( bird, "PostVars_ui", ZnkStr_cstr(postvars_ui) );

	ZnkVarpAry_destroy( input_hiddens );
	ZnkStr_delete( postvars_ui );
	return cb_status;
}
