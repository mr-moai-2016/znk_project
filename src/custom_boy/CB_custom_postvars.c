#include <CB_custom_postvars.h>

typedef enum {
	 Cmd_e_Get
	,Cmd_e_MainStep1
	,Cmd_e_MainStep2
	,Cmd_e_MainAll
	,Cmd_e_MainUpdate
	,Cmd_e_MainClear
} CmdType;

static CmdType
getCmdType( ZnkVarpAry cb_vars )
{
	CmdType cmd_type = Cmd_e_Get;
	ZnkVarp varp = ZnkVarpAry_find_byName_literal( cb_vars, "cb_arg", false );
	if( varp ){
		if( ZnkS_eq(ZnkVar_cstr(varp),"main_step1") ){
			cmd_type = Cmd_e_MainStep1;
		} else if( ZnkS_eq(ZnkVar_cstr(varp),"main_step2") ){
			cmd_type = Cmd_e_MainStep2;
		} else if( ZnkS_eq(ZnkVar_cstr(varp),"main_all") ){
			cmd_type = Cmd_e_MainAll;
		} else if( ZnkS_eq(ZnkVar_cstr(varp),"main_update") ){
			cmd_type = Cmd_e_MainUpdate;
		} else if( ZnkS_eq(ZnkVar_cstr(varp),"main_clear") ){
			cmd_type = Cmd_e_MainClear;
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
	const char* hint_base_PV = CBConfig_hint_base_PV();
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		const char* style_class = ( idx % 2 ) ? "MstyItemOdd" : "MstyItemEvn";
		ZnkVarp varp = ZnkVarpAry_at( input_hiddens, idx );
		const char* key = ZnkVar_name_cstr( varp );
		const char* val = ZnkVar_cstr( varp );
		ZnkStr_addf( ans,
				"<tr class=%s><td><b>%s</b></td><td><input type=text id=%s name=%s size=28 value=\"%s\">&nbsp; "
				"<a href=\"%spv_%s\" target=_blank>Hint</a></td></tr>\n", 
				style_class, key, key, key, val,
				hint_base_PV, key );
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

CBStatus
CBCustomPostVars_main( RanoCGIEVar* evar, ZnkVarpAry cb_vars, const char* cb_src,
		ZnkBird bird, ZnkStr RE_key, CBFgpInfo fgp_info, struct CBUAInfo_tag* ua_info,
		ZnkVarpAry main_vars, uint64_t* ptua64, ZnkStr msg, ZnkStr category, ZnkStr lacking_var,
		CBConfigInfo* info, bool is_authenticated )
{
	const char* moai_dir = CBConfig_moai_dir();
	const char* target = CBConfig_theNegotiatingTarget();

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
	case Cmd_e_MainStep1:
		CBVirtualizer_doMainProc( evar, cb_vars, cb_src, bird, true, false,
				RE_key, fgp_info, ua_info, main_vars, ptua64, msg, category, lacking_var, moai_dir );
		fgp_state_mode = CBFgpStateMode_e_StockContext;
		CBVirtualizer_save( main_vars, info->ua_state_filename_, "MainContext" );
		CBVirtualizer_registBird_byVars( bird, main_vars );
		CBVirtualizer_registPtuaCtx( bird, *ptua64 );
		break;
	case Cmd_e_MainStep2:
		/* この場合CBFgpInfoはinfo->ua_state_filename_を参照して取得する */
		CBFgpInfo_load( fgp_info, info->ua_state_filename_, "FingerContext" );
		if( CBVirtualizer_doMainProc( evar, cb_vars, cb_src, bird, false, true,
					RE_key, fgp_info, ua_info, main_vars, ptua64, msg, category, lacking_var, moai_dir ) )
		{
			cb_status = CBStatus_e_Sealed;
		} else {
			cb_status = CBStatus_e_InputLacking;
		}
		CBVirtualizer_copyMainCtx_byVars( main_vars, cb_vars );
		fgp_state_mode = CBFgpStateMode_e_SaveFile;
		CBVirtualizer_save( main_vars, info->ua_state_filename_, "MainContext" );
		CBVirtualizer_registBird_byVars( bird, main_vars );

		if( getPtua64_fromVars( cb_vars, ptua64 ) ){
			CBVirtualizer_registPtuaCtx( bird, *ptua64 );
		}
		break;
	case Cmd_e_MainAll:
		CBVirtualizer_doMainProc( evar, cb_vars, cb_src, bird, true, true,
				RE_key, fgp_info, ua_info, main_vars, ptua64, msg, category, lacking_var, moai_dir );
		fgp_state_mode = CBFgpStateMode_e_SaveFile;
		CBVirtualizer_save( main_vars, info->ua_state_filename_, "MainContext" );
		CBVirtualizer_registBird_byVars( bird, main_vars );
		cb_status = CBStatus_e_Sealed;
		CBVirtualizer_registPtuaCtx( bird, *ptua64 );
		break;
	case Cmd_e_MainClear:
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

	CBFgpInfo_proc_byStateMode( fgp_info, fgp_state_mode );
	CBVirtualizer_registBirdFgp( bird, fgp_info, ZnkStr_cstr(RE_key), is_update_main );
	CBVirtualizer_registBirdCookie( bird, true, moai_dir );

	ZnkBird_regist( bird, "cb_target", target );
	{
		ZnkStr selectbar = ZnkStr_new( "" );

		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=%s&amp;cb_type=postvars\" >CustomUSERS</a> &nbsp",
				"MstyNowSelectedLink", target );
		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=%s&amp;cb_type=cookie\" >CustomCookie</a> &nbsp",
				"MstyElemLink", target );
		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=%s&amp;cb_type=feature\" >CustomFeature</a> &nbsp",
				"MstyElemLink", target );

		ZnkStr_add( selectbar, "<br>\n" );

		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=futaba&amp;cb_type=postvars\" >for futaba</a> &nbsp",
				ZnkS_eq( target, "futaba" ) ? "MstyNowSelectedLink" : "MstyElemLink" );
		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=5ch&amp;cb_type=postvars\" >for 5ch</a> &nbsp",
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
