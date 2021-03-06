#include <CB_custom_cookie.h>
#include <Rano_conf_util.h>

typedef enum {
	 Cmd_e_Get
	,Cmd_e_SaveCookie
} CmdType;

static CmdType
getCmdType( ZnkVarpAry cb_vars )
{
	CmdType cmd_type = Cmd_e_Get;
	ZnkVarp varp = ZnkVarpAry_find_byName_literal( cb_vars, "cb_arg", false );
	if( varp ){
		if( ZnkS_eq(ZnkVar_cstr(varp),"save_cookie") ){
			cmd_type = Cmd_e_SaveCookie;
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

CBStatus
CBCustomCookie_main( ZnkVarpAry cb_vars,
		ZnkBird bird, ZnkStr RE_key, CBFgpInfo fgp_info,
		ZnkVarpAry main_vars, uint64_t* ptua64,
		CBConfigInfo* info, bool is_authenticated )
{
	const char* moai_dir = RanoConfUtil_moai_dir( NULL );
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
	case Cmd_e_SaveCookie:
		CBVirtualizer_load( main_vars, info->ua_state_filename_, "MainContext" );
		CBVirtualizer_registBird_byFutabaSendMyf( bird, moai_dir );
		CBVirtualizer_saveCookie_byCBVars( cb_vars, moai_dir );
		CBVirtualizer_registBird_byVars( bird, main_vars );
		break;
	default:
	{
		/* GET : SendMyf ptuaから値を得るべき */
		CBVirtualizer_load( main_vars, info->ua_state_filename_, "MainContext" );
		CBVirtualizer_registBird_byFutabaSendMyf( bird, moai_dir );
		cb_status = CBStatus_e_Sealed;
		{
			/* SendMyf より */
			const char* ptua_cstr = ZnkBird_at( bird, "ptua" );
			if( ptua_cstr ){
				ZnkS_getU64U( ptua64, ptua_cstr );
			}
		}
		break;
	}
	}

	CBVirtualizer_registPtuaCtx( bird, *ptua64 );

	CBFgpInfo_proc_byStateMode( fgp_info, fgp_state_mode );
	CBVirtualizer_registBirdFgp( bird, fgp_info, ZnkStr_cstr(RE_key), is_update_main );
	CBVirtualizer_registBirdCookie( bird, false, moai_dir );

	ZnkBird_regist( bird, "cb_target", target );
	{
		ZnkStr selectbar = ZnkStr_new( "" );

		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=%s&amp;cb_type=postvars\" >CustomUSERS</a> &nbsp",
				"MstyElemLink", target );
		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=%s&amp;cb_type=cookie\" >CustomCookie</a> &nbsp",
				"MstyNowSelectedLink", target );
		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=%s&amp;cb_type=feature\" >CustomFeature</a> &nbsp",
				"MstyElemLink", target );

		ZnkStr_add( selectbar, "<br>\n" );

		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=futaba&amp;cb_type=cookie\" >for futaba</a> &nbsp",
				ZnkS_eq( target, "futaba" ) ? "MstyNowSelectedLink" : "MstyElemLink" );
		ZnkStr_addf( selectbar,
				"<a class=%s href=\"/cgis/custom_boy/custom_boy.cgi?cb_target=5ch&amp;cb_type=cookie\" >for 5ch</a> &nbsp",
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
