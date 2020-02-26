/***
 * Moai WebServer Engine (introduced by Ver2.0 2016.12)
 *
 * @brief
 *  Moai is yet another portable web-server engine.
 *
 * @auther
 *  Mr.Moai
 *
 * @licence
 *  Copyright (c) Zen-nippon Network Kenkyujo(ZNK)
 *  Licensed under the NYSL( see http://www.kmonos.net/nysl/index.en.html for detail ).
 *
 * @disclaimer
 *  This software is provided 'as-is', without any express or implied warranty.
 *  In no event will the authors be held liable for any damages arising
 *  from the use of this software.
 */
#include "Moai_context.h"
#include "Moai_io_base.h"
#include "Moai_connection.h"
#include "Moai_info.h"
#include "Moai_fdset.h"
#include "Moai_post.h"
#include "Moai_server_info.h"
#include "Moai_cgi.h"

#include <Rano_module_ary.h>
#include <Rano_log.h>
#include <Rano_parent_proxy.h>
#include <Rano_myf.h>
#include <Rano_file_info.h>
#include <Rano_cgi_util.h>

#include <Znk_str.h>
#include <Znk_str_ptn.h>
#include <Znk_str_ex.h>
#include <Znk_str_path.h>
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <Znk_bird.h>
#include <Znk_dir.h>
#include <Znk_envvar.h>
#include <Znk_htp_util.h>
#include <Znk_net_ip.h>
#include <Znk_thread.h>
#include <Znk_process.h>

#include <string.h>

#define SJIS_HYO "\x95\x5c" /* �\ */
#define SJIS_NOU "\x94\x5c" /* �\ */

static size_t     st_input_ui_idx = 0;

static void
printInputUI_Text( ZnkStr html,
		const char* varname, const char* current_val, const char* new_val, bool is_enable,
		const char* destination )
{
	const char* class_name = ( st_input_ui_idx % 2 ) ? "MstyItemOdd" : "MstyItemEvn";
	const char* readonly_str = is_enable ? "" : "readonly disabled=true";
	ZnkStr_addf( html,
			"<tr class=\"%s\">"
			"<td><b>%s</b></td>"
			"<td><input class=MstyInputField type=text id=%s name=%s value=\"%s\" size=30 %s></td>"
			"<td><font size=-1>%s</font></td>"
			"</tr>\n",
			class_name,
			varname,
			varname, varname, new_val, readonly_str, destination );
	++st_input_ui_idx;
}
static void
printInputUI_CheckBox( ZnkStr html,
		const char* varname, bool current_val, bool new_val, bool is_enable,
		const char* destination )
{
	const char* class_name = ( st_input_ui_idx % 2 ) ? "MstyItemOdd" : "MstyItemEvn";
	const char* readonly_str = is_enable ? "" : "readonly disabled=true";

	ZnkStr_addf( html,
			"<tr class=\"%s\">"
			"<td><b>%s</b></td>"
			"<td><input type=checkbox name=%s value=\"on\" %s %s></td>"
			"<td><font size=-1>%s</font></td>"
			"</tr>\n",
			class_name,
			varname, varname, new_val ? "checked" : "", readonly_str, destination );
	++st_input_ui_idx;
}
static void
printInputUI_Int( ZnkStr html,
		const char* varname, int current_val, int new_val, bool is_enable,
		const char* destination )
{
	const char* class_name = ( st_input_ui_idx % 2 ) ? "MstyItemOdd" : "MstyItemEvn";
	const char* readonly_str = is_enable ? "" : "readonly disabled=true";
	ZnkStr_addf( html,
			"<tr class=\"%s\">"
			"<td><b>%s</b></td>"
			"<td><input class=MstyInputField type=text name=%s value=\"%d\" %s></td>"
			"<td><font size=-1>%s</font>"
			"</td></tr>\n",
			class_name,
			varname, varname, new_val, readonly_str, destination );
	++st_input_ui_idx;
}


static void
printInputUI_SelectBox( ZnkStr html,
		const char* varname, const char* current_val, const char* new_val, bool is_enable,
		ZnkStrAry str_list,
		const char* destination )
{
	const char* class_name = ( st_input_ui_idx % 2 ) ? "MstyItemOdd" : "MstyItemEvn";
	const char* readonly_str = is_enable ? "" : "disabled=true";
	const size_t size = ZnkStrAry_size( str_list );
	size_t idx;
	bool is_selected = false;
	const char* val = "";

	ZnkStr_addf( html,
			"<tr class=\"%s\">"
			"<td><b>%s</b></td>"
			"<td><select class=MstyInputField name=%s %s>\n",
			class_name,
			varname, varname, readonly_str );

	for( idx=0; idx<size; ++idx ){
		val = ZnkStrAry_at_cstr( str_list, idx );
		is_selected = ZnkS_eq( val, current_val );
		ZnkStr_addf( html, "<option%s>%s</option>", is_selected ? " selected" : "", val );
	}
	ZnkStr_addf( html,
			"</td>"
			"<td><font size=-1>%s</font></td>"
			"</tr>\n", destination );
	++st_input_ui_idx;
}

#if 0
static void
makeStyleForXhrDMZ( ZnkStr html )
{
	ZnkStr_add(  html, "<style type=\"text/css\">\n" );
	ZnkStr_add(  html, ".MstyElemLink {\n" );
	ZnkStr_add(  html, "	color: #000;\n" );
	ZnkStr_add(  html, "	border: 1px solid #3D803D;\n" );
	ZnkStr_add(  html, "	padding: 4px 8px;\n" );
	ZnkStr_add(  html, "	display: inline-block;\n" );
	ZnkStr_add(  html, "	background: #E0E0B0 none repeat scroll 0% 0%;\n" );
	ZnkStr_add(  html, "	text-align: center;\n" );
	ZnkStr_add(  html, "	border-radius: 10px;\n" );
	ZnkStr_add(  html, "	margin-bottom: 5px;\n" );
	ZnkStr_add(  html, "	text-decoration: none;\n" );
	ZnkStr_add(  html, "    word-break: normal;\n" );
	ZnkStr_add(  html, "}\n" );
	ZnkStr_add(  html, ".MstyIndent {\n" );
	ZnkStr_add(  html, "	margin-left: 30px;\n" );
	ZnkStr_add(  html, "}\n" );
	ZnkStr_add(  html, "</style>\n" );
}
#endif

static void
printPPBScript( ZnkStr html )
{
	ZnkStr_add( html, "<script type='text/javascript'>\n" );
	ZnkStr_add( html, "function setProfileDir( dir_path ){\n" );
	ZnkStr_add( html, "\tdocument.getElementById( 'profile_dir' ).value = dir_path;\n" );
	ZnkStr_add( html, "}\n" );
	ZnkStr_add( html, "function setFiltersDir( dir_path ){\n" );
	ZnkStr_add( html, "\tdocument.getElementById( 'filters_dir' ).value = dir_path;\n" );
	ZnkStr_add( html, "}\n" );
	ZnkStr_add( html, "function reportPPB( msg ){\n" );
	ZnkStr_add( html, "\tdocument.getElementById( 'report_area' ).innerHTML = msg;\n" );
	ZnkStr_add( html, "}\n" );
	ZnkStr_add( html, "</script>\n" );
}
static void
printPresumeProperValBtn( ZnkStr html, const char* profile_dir, const char* filters_dir )
{
	bool        userdata_dir_ok = false;
	const char* userdata_dir    = "";
	ZnkStr      profile_dir_mod = ZnkStr_new( profile_dir );
	ZnkStr      filters_dir_mod = ZnkStr_new( filters_dir );
	bool        ok_profile_dir = false;
	bool        ok_filters_dir = false;
	ZnkStr      msg = ZnkStr_new( "" );

	/***
	 * �K�؂� userdata_dir ��T��.
	 */
	do {
		userdata_dir = "/sdcard/Download";
		if( ZnkDir_getType( userdata_dir ) == ZnkDirType_e_Directory ){ userdata_dir_ok = true; break; }

		userdata_dir = "/storage/emulated/0/Download";
		if( ZnkDir_getType( userdata_dir ) == ZnkDirType_e_Directory ){ userdata_dir_ok = true; break; }

		userdata_dir = "/storage/emulated/legacy/Download";
		if( ZnkDir_getType( userdata_dir ) == ZnkDirType_e_Directory ){ userdata_dir_ok = true; break; }

	} while( false );

	/***
	 * profile_dir �������ȏꍇ�A��֒l�𓱏o
	 */
	if( ZnkDir_getType( profile_dir ) != ZnkDirType_e_Directory ){ 
		if( userdata_dir_ok ){
			ZnkStr_setf( profile_dir_mod, "%s/moai_profile", userdata_dir );
			ZnkDir_mkdirPath( ZnkStr_cstr(profile_dir_mod), Znk_NPOS, '/', NULL );
		}
	}
	/***
	 * filters_dir �������ȏꍇ�A��֒l�𓱏o
	 */
	if( ZnkDir_getType( filters_dir ) != ZnkDirType_e_Directory ){ 
		if( userdata_dir_ok ){
			ZnkStr_setf( filters_dir_mod, "%s/moai_profile/filters", userdata_dir );
			ZnkDir_mkdirPath( ZnkStr_cstr(filters_dir_mod), Znk_NPOS, '/', NULL );
		}
	}

	/***
	 * �������ݎ���
	 */
	if( ZnkDir_getType( ZnkStr_cstr(profile_dir_mod) ) == ZnkDirType_e_Directory ){ 
		int state = MoaiIO_testFileWriting( ZnkStr_cstr(profile_dir_mod) );
		if( state == 2 ){
			/* ���i */
			ok_profile_dir = true;
		}
	}
	if( ZnkDir_getType( ZnkStr_cstr(filters_dir_mod) ) == ZnkDirType_e_Directory ){ 
		int state = MoaiIO_testFileWriting( ZnkStr_cstr(filters_dir_mod) );
		if( state == 2 ){
			/* ���i */
			ok_filters_dir = true;
		}
	}

	/***
	 * ����/���s�Ɋւ�炸�K�v.
	 */
	ZnkStrPath_replaceDSP( profile_dir_mod, '/' );
	ZnkStrPath_replaceDSP( filters_dir_mod, '/' );

	if( ok_profile_dir ){
		ZnkStr_add( msg, "<font color=red>profile����. �u�ݒ�̊m��v�Ŋm��ł��܂�.</font><br>" );
	} else {
		ZnkStr_addf( msg, "<font color=red>profile���s(%s).</font><br>", ZnkStr_cstr(profile_dir_mod) );
	}
	if( ok_filters_dir ){
		ZnkStr_add( msg, "<font color=red>filters����. �u�ݒ�̊m��v�Ŋm��ł��܂�.</font><br>" );
	} else {
		ZnkStr_addf( msg, "<font color=red>filters���s(%s).</font><br>", ZnkStr_cstr(filters_dir_mod) );
	}

	if( !ok_profile_dir ){
		/* ���ɖ߂� */
		ZnkStr_setf( profile_dir_mod, profile_dir );
	}
	if( !ok_filters_dir ){
		/* ���ɖ߂� */
		ZnkStr_setf( filters_dir_mod, filters_dir );
	}

	ZnkStr_addf(  html, "<a class=MstyElemLink "
			"onclick=\"setProfileDir( '%s' ); setFiltersDir( '%s' ); reportPPB( '%s' ); \">�X�L����</a>",
			ZnkStr_cstr(profile_dir_mod), ZnkStr_cstr(filters_dir_mod),
			ZnkStr_cstr(msg) );

	ZnkStr_delete( msg );
	ZnkStr_delete( profile_dir_mod );
	ZnkStr_delete( filters_dir_mod );
}

static int
printConfig( ZnkSocket sock, ZnkStrAry result_msgs, uint32_t peer_ipaddr )
{
	int         ret                   = 0;
	char        proxy[ 1024 ]         = "";
	const char* parent_proxy_hostname = NULL;
	ZnkStr      html                  = ZnkStr_new( "" );
	ZnkStrAry   str_list              = ZnkStrAry_create( true );
	ZnkStr      uxs_tmp               = ZnkStr_new( "" );
	const bool  is_enable_log_file    = MoaiServerInfo_is_enable_log_file();
	const bool  is_enable_log_verbose = MoaiServerInfo_is_enable_log_verbose();
	const char* filters_dir           = MoaiServerInfo_filters_dir();
	const char* profile_dir           = MoaiServerInfo_profile_dir();
	const char* moai_authentic_key    = MoaiServerInfo_authenticKey();

	MoaiCGIManager_makeHeader( html, "Moai Configuration", false );

	ZnkStr_add( html, "<body>\n" );

	printPPBScript( html );

	ZnkStr_add( html, "<b><img src=\"moai.png\"> Moai�G���W���ݒ�</b><br>\n" );
	ZnkStr_add( html, "<a class=MstyNowSelectedLink href=/config              >Moai��{�ݒ�</a> &nbsp;\n" );
	ZnkStr_add( html, "<a class=MstyElemLink        href=/config?mode=sys     >Moai�Z�L�����e�B�ݒ�</a> &nbsp;\n" );
	ZnkStr_add( html, "<a class=MstyElemLink        href=/config?mode=upgrade >Moai�A�b�v�O���[�h</a> &nbsp;\n" );
	ZnkStr_add( html, "<br> <br>\n" );

	/* ==== Instantly Updatable variables ==== */
	ZnkStr_add( html, "<b>Moai��{�ݒ�</b><br>\n" );
	st_input_ui_idx = 0;
	ZnkStr_add( html, "<form action=/config method=POST enctype=multipart/form-data>\n" );
	parent_proxy_hostname = RanoParentProxy_getHostname();
	if( ZnkS_empty(parent_proxy_hostname) ){
		ZnkS_copy( proxy, sizeof(proxy), "NONE", Znk_NPOS );
	} else {
		Znk_snprintf( proxy, sizeof(proxy), "%s:%u",
				parent_proxy_hostname, RanoParentProxy_getPort() );
	}

	ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
	ZnkStr_add( html, "<thead>\n" );
	ZnkStr_add( html, "<tr><th>�ϐ���</th><th>���݂̒l</th><th>����</th></tr>\n" );
	ZnkStr_add( html, "</thead>\n" );
	ZnkStr_add( html, "<tbody>\n" );

	{
		/* Parent Proxy */
		ZnkStrAry parent_proxys;
		RanoParentProxy_loadCandidate( "parent_proxy.txt" );
		parent_proxys = RanoParentProxy_getParentProxys();
		printInputUI_SelectBox( html,
				"parent_proxy", proxy, proxy, true,
				parent_proxys,
				"�O���v���L�V���g�p����ꍇ�I�����܂�.<br>(NONE�̏ꍇ�g�p���܂���)." );
	}

	printInputUI_Text( html, "filters_dir", filters_dir, filters_dir, true,
			"filters�t�H���_�̃p�X���w�肵�܂�.<br>(��΃p�X�܂���moai�f�B���N�g������̑��΃p�X�ł�)." );
	printInputUI_Text( html, "profile_dir", profile_dir, profile_dir, true,
			"profile�t�H���_�̃p�X���w�肵�܂�.<br>(��΃p�X�܂���moai�f�B���N�g������̑��΃p�X�ł�)." );

	printInputUI_CheckBox( html,
			"post_confirm", MoaiPost_isPostConfirm(), MoaiPost_isPostConfirm(), true,
			"Moai�����[�J���v���L�V�Ƃ��Ďg�p�����ꍇ�A<br>Post���ɕϐ��̓��e��" SJIS_HYO "�������m�F��ʂ��o���܂�." );

	printInputUI_CheckBox( html,
			"enable_log_file", is_enable_log_file, is_enable_log_file, true,
			"Moai�̃��O�t�@�C�������o�����[�h��On/Off�ł�" );

	printInputUI_CheckBox( html,
			"enable_log_verbose", is_enable_log_verbose, is_enable_log_verbose, true,
			"Moai�̃��O�t�@�C��������ɏڍׂɏ����o�����ۂ��ł�" );

#if 0
	/* ���͂⃆�[�U���ݒ肷��܂ł��Ȃ��펞off(��u���b�L���O)�ł悩�낤 */
	printInputUI_CheckBox( html,
			"blocking_mode", MoaiConnection_isBlockingMode(), MoaiConnection_isBlockingMode(), true,
			"Connection Blocking Mode(on/off)." );
#endif

	ZnkStr_add(  html, "<tr class=MstyItemOdd><td><br>\n" );
	printPresumeProperValBtn( html, profile_dir, filters_dir );
	ZnkStr_add(  html, "</td>" );
	ZnkStr_add(  html, "<td>\n" );
	ZnkStr_addf(  html, "<spin id=report_area></spin>\n" );
	ZnkStr_add(  html, "</td>\n" );
	ZnkStr_add(  html, "<td><font size=-1>filters�����profile�t�H���_�Ƃ���<br>�������݉�" SJIS_NOU "�ȏꏊ���X�L�������܂�.</font></td></tr>\n" );

	ZnkStr_add(  html, "<tr><td colspan=3><br>\n" );
	ZnkStr_add(  html, "<input type=hidden name=Moai_Update       value=\"update\">\n" );
	ZnkStr_addf( html, "<input type=hidden name=Moai_AuthenticKey value=\"%s\">\n", moai_authentic_key );
	ZnkStr_add(  html, "<input class=MstyWideButton type=submit value=\"�ݒ�̊m��\">\n" );
	ZnkStr_add(  html, "</td></tr>\n" );

	ZnkStr_add( html, "</tbody>\n" );
	ZnkStr_add( html, "</table>\n" );

	ZnkStr_add(  html, "</form>\n" );

	ZnkStr_add(  html, "&nbsp;\n" );

	if( result_msgs && ZnkStrAry_size(result_msgs) ){
		size_t idx;
		const size_t size = ZnkStrAry_size( result_msgs );
		ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
		ZnkStr_add( html, "<tr><td colspan=2>\n" );
		ZnkStr_add( html, "<font size=-1 color=red>\n" );
		for( idx=0; idx<size; ++idx ){
			ZnkStr_set( uxs_tmp, ZnkStrAry_at_cstr( result_msgs, idx ) );
			ZnkHtpURL_negateHtmlTagEffection( uxs_tmp );
			ZnkStr_addf( html, "%s<br>\n", ZnkStr_cstr( uxs_tmp ) ); /* XSS-safe */
		}
		ZnkStr_add( html, "</font>\n" );
		ZnkStr_add( html, "</td></tr>\n" );
		ZnkStr_add( html, "</table>\n" );
	}

	/* ==== Restart ==== */
	if( MoaiServerInfo_you_need_to_restart_moai() ){
		ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
		ZnkStr_add( html, "<tr><td colspan=2>\n" );
		ZnkStr_add( html, "<font size=-1 color=red>�@���̐ݒ�̕ύX�𔽉f������ɂ�Moai���ċN��������K�v������܂�.</font>\n" );
		ZnkStr_add( html, "</td></tr>\n" );
		ZnkStr_add( html, "</table>\n" );
	}
	ZnkStr_add(  html, "<form action=\"/config\" method=\"POST\" enctype=\"multipart/form-data\">\n" );
	ZnkStr_add(  html, "<input type=hidden name=Moai_RestartServer value=\"reboot\">\n" );
	ZnkStr_addf( html, "<input type=hidden name=Moai_AuthenticKey  value=\"%s\">\n", moai_authentic_key );
	ZnkStr_add(  html, "<input class=MstyWideButton type=submit value=\"Moai���ċN��\">\n" );
	ZnkStr_add(  html, "</form>\n" );

	ZnkStr_add( html, "<br>\n" );

	ZnkStr_add( html, "<a class=MstyElemLink href=\"/\">Moai�g�b�v�y�[�W</a>\n" );
	ZnkStr_add( html, "</body></html>\n" );

	ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr(html) );
	ZnkStr_delete( html );
	ZnkStr_delete( uxs_tmp );
	ZnkStrAry_destroy( str_list );
	return ret;
}

static int
printSysConfig( ZnkSocket sock, ZnkStrAry result_msgs, uint32_t peer_ipaddr )
{
	int         ret                = 0;
	bool        sys_config_enable  = true;
	ZnkStr      html               = ZnkStr_new( "" );
	ZnkStrAry   str_list           = ZnkStrAry_create( true );
	const char* server_name        = MoaiServerInfo_server_name();
	uint16_t    moai_port          = MoaiServerInfo_port();
	uint16_t    xhr_dmz_port       = MoaiServerInfo_XhrDMZPort();
	const char* acceptable_host    = MoaiServerInfo_acceptable_host();
	const char* moai_authentic_key = MoaiServerInfo_authenticKey();

	MoaiCGIManager_makeHeader( html, "Moai System Configuration", false );
	ZnkStr_add( html, "<body>\n" );

	ZnkStr_add( html, "<b><img src=\"moai.png\"> Moai�G���W���ݒ�</b><br>\n" );
	ZnkStr_add( html, "<a class=MstyElemLink        href=/config              >Moai��{�ݒ�</a> &nbsp;\n" );
	ZnkStr_add( html, "<a class=MstyNowSelectedLink href=/config?mode=sys     >Moai�Z�L�����e�B�ݒ�</a> &nbsp;\n" );
	ZnkStr_add( html, "<a class=MstyElemLink        href=/config?mode=upgrade >Moai�A�b�v�O���[�h</a> &nbsp;\n" );
	ZnkStr_add( html, "<br> <br>\n" );

	/* ==== Security Configuration( �O���}�V������̍X�V�͕s�Ƃ��� ) ==== */
	if( peer_ipaddr == 0x0100007f || peer_ipaddr == MoaiServerInfo_private_ip() ){
		/* Loopback�ڑ� */
		sys_config_enable = true;
	} else {
		/* ���̃}�V������̐ڑ� */
		sys_config_enable = false;
	}
	ZnkStr_add( html, "<b>Moai�Z�L�����e�B�ݒ�</b><br>\n" );
	ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
	ZnkStr_add( html, "<tr><td colspan=2>\n" );
	ZnkStr_add( html, "<font size=-1 color=red>�@��: ���̃p�l���̓G�L�X�p�[�g�����̂��̂Œʏ�͗��p����K�v�͂���܂���B<br>" );
	ZnkStr_add( html, "�@�܂������ł̐ݒ�́A�Z�L�����e�B�[��̗��R��localhost����̂݉�" SJIS_NOU "�ƂȂ�܂�.</font>\n" );
	ZnkStr_add( html, "</td></tr>\n" );
	ZnkStr_add( html, "</table>\n" );

	st_input_ui_idx = 0;
	ZnkStr_add( html, "<form action=/config?mode=sys method=POST enctype=multipart/form-data>\n" );

	ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
	ZnkStr_add( html, "<thead>\n" );
	ZnkStr_add( html, "<tr><th>�ϐ���</th><th>���݂̒l</th><th>����</th></tr>\n" );
	ZnkStr_add( html, "</thead>\n" );
	ZnkStr_add( html, "<tbody>\n" );

	{
		ZnkStrAry_clear( str_list );
		ZnkStrAry_push_bk_cstr( str_list, "LOOPBACK", Znk_NPOS );
		ZnkStrAry_push_bk_cstr( str_list, "ANY", Znk_NPOS );
		printInputUI_SelectBox( html,
				"acceptable_host", acceptable_host, acceptable_host, sys_config_enable,
				str_list,
				"LOOPBACK�̏ꍇ�A���[�J���}�V���ȊO����̐ڑ������S�ɖ�O�������܂�.<br>"
				"ANY�̏ꍇ�A���[�J���}�V���ȊO����̐ڑ����Ƃ肠�����󂯕t���A<br>"
				"���������̍X�Ȃ�ׂ����󗝂̔����hosts.myf�̓��e�Ɉς˂܂�." );
	}

	printInputUI_Text( html, "server_name", server_name, server_name, sys_config_enable,
			"�T�[�o���܂���LAN���猩���ꍇ��IP�A�h���X�ł�.<br>XhrDMZ��hostname�Ƃ��Ă��g���܂�." );

	printInputUI_Int( html, "moai_port", moai_port, moai_port, sys_config_enable,
			"Moai�{�̂��g�p����TCP port�ł�." );

	printInputUI_Int( html, "xhr_dmz_port", xhr_dmz_port, xhr_dmz_port, sys_config_enable,
			"XhrDMZ���g�p����TCP port�ł�." );

	if( sys_config_enable ){
		ZnkStr_add(  html, "<tr colspan=4><td><br>\n" );
		ZnkStr_add(  html, "<input type=hidden name=Moai_UpdateSys    value=\"update_sys\">\n" );
		ZnkStr_addf( html, "<input type=hidden name=Moai_AuthenticKey value=\"%s\">\n", moai_authentic_key );
		ZnkStr_add(  html, "<input class=MstyWideButton type=submit value=\"�ݒ�̊m��\">\n" );
		ZnkStr_add(  html, "</td></tr>\n" );
	}

	ZnkStr_add( html, "</tbody>\n" );
	ZnkStr_add( html, "</table>\n" );

	ZnkStr_add( html, "</form>\n" );

	ZnkStr_add(  html, "&nbsp;\n" );

	/* ==== Restart ==== */
	if( MoaiServerInfo_you_need_to_restart_moai() ){
		ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
		ZnkStr_add( html, "<tr><td colspan=2>\n" );
		ZnkStr_add( html, "<font size=-1 color=red>�@���̐ݒ�̕ύX�𔽉f������ɂ�Moai���ċN��������K�v������܂�.</font>\n" );
		ZnkStr_add( html, "</td></tr>\n" );
		ZnkStr_add( html, "</table>\n" );
	}
	ZnkStr_add(  html, "<form action=/config?mode=sys method=POST enctype=multipart/form-data>\n" );
	ZnkStr_add(  html, "<input type=hidden name=Moai_RestartServer value=reboot>\n" );
	ZnkStr_addf( html, "<input type=hidden name=Moai_AuthenticKey  value=\"%s\">\n", moai_authentic_key );
	ZnkStr_add(  html, "<input class=MstyWideButton type=submit value=\"Moai���ċN��\">\n" );
	ZnkStr_add(  html, "</form>\n" );

	ZnkStr_add( html, "<br>\n" );

	ZnkStr_add( html, "<a class=MstyElemLink href=\"/\">Moai�g�b�v�y�[�W</a>\n" );
	ZnkStr_add(  html, "</body></html>\n" );

	ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr(html) );
	ZnkStr_delete( html );
	ZnkStrAry_destroy( str_list );
	return ret;
}

static int
printUpgrade( ZnkSocket sock, ZnkStrAry result_msgs, uint32_t peer_ipaddr, bool is_query, bool inline_script )
{
	int         ret                = 0;
	ZnkStr      html               = ZnkStr_new( "" );
	ZnkStrAry   str_list           = ZnkStrAry_create( true );
	const char* moai_authentic_key = MoaiServerInfo_authenticKey();
	const bool  is_apk             = MoaiServerInfo_is_apk();

	MoaiCGIManager_makeHeader( html, "Moai Upgrade", inline_script );

	ZnkStr_add( html, "<body>\n" );

	if( inline_script ){
		/**
		 * Moai process���ꎞ�I�����ɑ��t�@�C����V�K�ɓǂݍ������Ƃ���u���E�U������.
		 * (�Ⴆ��linux�ł�firefox�Ȃ�)
		 * �������l�b�g���[�N�I�ɉ����͂Ȃ����߃��[�h�҂���ԂƂȂ�A�T�[�o����socket�����ɑ��݂��Ȃ����߁A
		 * ���̃��[�h�҂����I���Ȃ��󋵂ƂȂ�.
		 * �����������邽�߁A���[�g�ƂȂ�g�b�v�y�[�W�ŁA�ʏ�Ȃ瑼�t�@�C���ł܂��Ȃ��d�������ׂĂ��������Ă����K�v������.
		 */

		/* css */
		ZnkStr_add( html, "<style type=\"text/css\" ><!--\n" ); 
		ZnkStr_add( html, ".MstyManagerTable {\n" );
		ZnkStr_add( html, "	padding: 1px !important;\n" );
		ZnkStr_add( html, "	border: 2px solid rgb(240,235,180) !important;\n" );
		ZnkStr_add( html, "	#-moz-border-radius: 20px !important;\n" );
		ZnkStr_add( html, "	border-radius: 20px !important;\n" );
		ZnkStr_add( html, "	background-color: rgba(255,255,250,0.6) !important;\n" );
		ZnkStr_add( html, "	word-break: normal;\n" );
		ZnkStr_add( html, "}\n" );
		ZnkStr_add( html, "@media only screen and (max-device-width : 480px){\n" );
		ZnkStr_add( html, "	/* for smart-phone */\n" );
		ZnkStr_add( html, "	.MstyManagerTable {\n" );
		ZnkStr_add( html, "		padding: 1px !important;\n" );
		ZnkStr_add( html, "		border: 2px solid rgb(240,235,180) !important;\n" );
		ZnkStr_add( html, "		#-moz-border-radius: 20px !important;\n" );
		ZnkStr_add( html, "		border-radius: 20px !important;\n" );
		ZnkStr_add( html, "		background-color: rgba(255,255,250,0.6) !important;\n" );
		ZnkStr_add( html, "\n" );
		ZnkStr_add( html, "	    width:100%;\n" );
		ZnkStr_add( html, "		word-break: break-all;\n" );
		ZnkStr_add( html, "	}\n" );
		ZnkStr_add( html, "}\n" );
		ZnkStr_add( html, ".MstyElemLink {\n" );
		ZnkStr_add( html, "	color: #000;\n" );
		ZnkStr_add( html, "#	font-size: 16px;\n" );
		ZnkStr_add( html, "	border: 1px solid #3D803D;\n" );
		ZnkStr_add( html, "	padding: 4px 8px;\n" );
		ZnkStr_add( html, "	display: inline-block;\n" );
		ZnkStr_add( html, "	background: #E0E0B0 none repeat scroll 0% 0%;\n" );
		ZnkStr_add( html, "	text-align: center;\n" );
		ZnkStr_add( html, "	border-radius: 10px;\n" );
		ZnkStr_add( html, "	margin-bottom: 5px;\n" );
		ZnkStr_add( html, "	text-decoration: none;\n" );
		ZnkStr_add( html, "    word-break: normal;\n" );
		ZnkStr_add( html, "}\n" );
		ZnkStr_add( html, ".MstyElemLink:hover {\n" );
		ZnkStr_add( html, "	color: #000;\n" );
		ZnkStr_add( html, "#	font-size: 16px;\n" );
		ZnkStr_add( html, "	border: 1px solid #3D803D;\n" );
		ZnkStr_add( html, "	padding: 4px 8px;\n" );
		ZnkStr_add( html, "	display: inline-block;\n" );
		ZnkStr_add( html, "	background: #B0B0E0 none repeat scroll 0% 0%;\n" );
		ZnkStr_add( html, "	text-align: center;\n" );
		ZnkStr_add( html, "	border-radius: 10px;\n" );
		ZnkStr_add( html, "	margin-bottom: 5px;\n" );
		ZnkStr_add( html, "	text-decoration: none;\n" );
		ZnkStr_add( html, "    word-break: normal;\n" );
		ZnkStr_add( html, "}\n" );
		ZnkStr_add( html, ".MstyNowSelectedLink {\n" );
		ZnkStr_add( html, "	color: #000;\n" );
		ZnkStr_add( html, "	border: 1px solid #3D8080;\n" );
		ZnkStr_add( html, "	padding: 3px 8px;\n" );
		ZnkStr_add( html, "	display: inline-block;\n" );
		ZnkStr_add( html, "	background: #A0A0FF none repeat scroll 0% 0%;\n" );
		ZnkStr_add( html, "	text-align: center;\n" );
		ZnkStr_add( html, "	border-radius: 10px;\n" );
		ZnkStr_add( html, "	margin-bottom: 5px;\n" );
		ZnkStr_add( html, "	text-decoration: none;\n" );
		ZnkStr_add( html, "}\n" );
		ZnkStr_add( html, ".MstyWideButton {\n" );
		ZnkStr_add( html, "	padding: 0.4em 1em;\n" );
		ZnkStr_add( html, "}\n" );
		ZnkStr_add( html, "</style>\n" );

		/* javascript */
		ZnkStr_add( html, "<script type=\"text/javascript\" >\n" ); 
		ZnkStr_add( html, "var count = 0;\n" );
		ZnkStr_add( html, "var st_dat_path = '';\n" );
		ZnkStr_add( html, "function set_dat_path( dat_path ){\n" );
		ZnkStr_add( html, "	st_dat_path = dat_path;\n" );
		ZnkStr_add( html, "}\n" );
		ZnkStr_add( html, "function refer_state(){\n" );
		ZnkStr_add( html, "	var xmlhttp = false;\n" );
		ZnkStr_add( html, "	if( typeof ActiveXObject != 'undefined' ){\n" );
		ZnkStr_add( html, "		try {\n" );
		ZnkStr_add( html, "			xmlhttp = new ActiveXObject( 'Microsoft.XMLHTTP' );\n" );
		ZnkStr_add( html, "		} catch (e) {\n" );
		ZnkStr_add( html, "			xmlhttp = false;\n" );
		ZnkStr_add( html, "		}\n" );
		ZnkStr_add( html, "	}\n" );
		ZnkStr_add( html, "	if( !xmlhttp && typeof XMLHttpRequest != 'undefined' ){\n" );
		ZnkStr_add( html, "		xmlhttp = new XMLHttpRequest();\n" );
		ZnkStr_add( html, "	}\n" );
		ZnkStr_add( html, "	xmlhttp.open( 'GET', st_dat_path + '?date=' + new Date().getTime() );\n" );
		ZnkStr_add( html, "	xmlhttp.onreadystatechange = function() {\n" );
		ZnkStr_add( html, "		if( xmlhttp.readyState == 4 && xmlhttp.status == 200 ){\n" );
		ZnkStr_add( html, "			var ary = xmlhttp.responseText.split( ' ' );\n" );
		ZnkStr_add( html, "			if( ary.length >= 2 ){\n" );
		ZnkStr_add( html, "				count = parseInt( ary[ 0 ], 10 );\n" );
		ZnkStr_add( html, "				ary.shift();\n" );
		ZnkStr_add( html, "				var msg = ary.join( ' ' );\n" );
		ZnkStr_add( html, "				if( isNaN(count) ){ count=100; }\n" );
		ZnkStr_add( html, "				document.getElementById( 'progress_state_msg' ).innerHTML = msg;\n" );
		ZnkStr_add( html, "			} else {\n" );
		ZnkStr_add( html, "				count = parseInt( xmlhttp.responseText, 10 );\n" );
		ZnkStr_add( html, "				if( isNaN(count) ){ count=100; }\n" );
		ZnkStr_add( html, "				document.getElementById( 'progress_state_msg' ).innerHTML = '[' + count + ']';\n" );
		ZnkStr_add( html, "			}\n" );
		ZnkStr_add( html, "		} else {\n" );
		ZnkStr_add( html, "			document.getElementById( 'progress_state_msg' ).innerHTML = '[' + count + ']';\n" );
		ZnkStr_add( html, "			++count;\n" );
		ZnkStr_add( html, "		}\n" );
		ZnkStr_add( html, "	};\n" );
		ZnkStr_add( html, "	xmlhttp.send( null );\n" );
		ZnkStr_add( html, "	return false;\n" );
		ZnkStr_add( html, "}\n" );
		ZnkStr_add( html, "var id = setInterval( function(){\n" );
		ZnkStr_add( html, "	refer_state();\n" );
		ZnkStr_add( html, "	if(count >= 100){\n" );
		ZnkStr_add( html, "		clearInterval(id);\n" );
		ZnkStr_add( html, "		}}, 1000 );\n" );
		ZnkStr_add( html, "set_dat_path(\"/state.dat\");\n" );
		ZnkStr_add( html, "</script>\n" );
	} else {
		ZnkStr_add( html, "<script type=\"text/javascript\" src=\"/progress.js\" ></script>\n" );
		ZnkStr_add( html, "<script type=\"text/javascript\"> set_dat_path(\"/state.dat\"); </script>\n" );
	}

	if( inline_script ){
		ZnkStr_add( html, "<b>Moai Upgrade</b><br>\n" );
	} else {
		ZnkStr_add( html, "<b><img src=\"moai.png\"> Moai Upgrade</b><br>\n" );
	}
	ZnkStr_add( html, "<a class=MstyElemLink        href=/config              >Moai��{�ݒ�</a> &nbsp;\n" );
	ZnkStr_add( html, "<a class=MstyElemLink        href=/config?mode=sys     >Moai�Z�L�����e�B�ݒ�</a> &nbsp;\n" );
	ZnkStr_add( html, "<a class=MstyNowSelectedLink href=/config?mode=upgrade >Moai�A�b�v�O���[�h</a> &nbsp;\n" );
	ZnkStr_add( html, "<br> <br>\n" );

	ZnkStr_add( html, "<b>Birdman Console</b><br>\n" );
	ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
	ZnkStr_add( html, "<tr><td colspan=2>\n" );

	if( is_apk ){
		ZnkStr_add( html, "<font size=-1 color=red>"
				"�@Android(apk�g�p)�łł͌���Moai�A�b�v�O���[�h���T�|�[�g���Ă��܂���.<br>\n"
				"�@���萔�ł����ŐV�łւ̃A�b�v�O���[�h��apk�t�@�C�����s���ĉ�����.</font>\n" );
		ZnkStr_addf( html, "<br><br>\n" );
		//ZnkStr_addf( html, "<a class=MstyWrapLink href='https://mr-moai-2016.github.io/download/moai-v2.2-android.apk'>�ŐV�ł�apk�t�@�C��</a>\n\n" );
		//ZnkStr_addf( html, "<br><br>\n" );
		ZnkStr_addf( html, "<a class=MstyWrapLink href='https://mr-moai-2016.github.io/moai2.0/install_android.html#ForApk'>�C���X�g�[�����@</a>\n\n" );
		ZnkStr_addf( html, "<br><br>\n" );
	} else {
		ZnkStr_add( html, "<font size=-1 color=red>�@���̉�ʂł�Moai�V�X�e���S�̂��A�b�v�O���[�h���܂�.</font>\n" );
	}
	ZnkStr_add( html, "</td></tr>\n" );
	ZnkStr_add( html, "</table>\n" );

	if( is_apk ){
		{
			ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
			ZnkStr_add( html, "<tr><td colspan=2>\n" );
			ZnkStr_add( html, "<div id=progress_state_msg></div>\n" );
			ZnkStr_add( html, "</td></tr>\n" );
			ZnkStr_add( html, "</table>\n" );
			ZnkStr_add( html, "<br>\n" );
		}
	} else {
		/* ==== Restart ==== */
		if( MoaiServerInfo_you_need_to_restart_moai() ){
			ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
			ZnkStr_add( html, "<tr><td colspan=2>\n" );
			ZnkStr_add( html, "<font size=-1 color=red>�@���̐ݒ�̕ύX�𔽉f������ɂ�Moai���ċN��������K�v������܂�.</font>\n" );
			ZnkStr_add( html, "</td></tr>\n" );
			ZnkStr_add( html, "</table>\n" );
		}
		{
			ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
			ZnkStr_add( html, "<tr><td colspan=2>\n" );
			ZnkStr_add( html, "<div id=progress_state_msg></div>\n" );
			ZnkStr_add( html, "</td></tr>\n" );
			ZnkStr_add( html, "</table>\n" );
			ZnkStr_add( html, "<br>\n" );
		}
	
		if( is_query ){
			ZnkStr_add(  html, "<form action=\"/config?mode=upgrade\" method=\"POST\" enctype=\"multipart/form-data\">\n" );
			ZnkStr_addf( html, "<input type=hidden name=Moai_AuthenticKey  value=\"%s\">\n", moai_authentic_key );
			ZnkStr_addf( html, "<input type=hidden name=Moai_UpgradeCmd    value=\"query\">\n" );
			ZnkStr_add(  html, "<input class=MstyWideButton type=submit value=\"�ŐV�o�[�W�����̊m�F\">\n" );
			ZnkStr_add(  html, "</form>\n" );
		}
	}

	ZnkStr_add( html, "<br>\n" );
	ZnkStr_add( html, "<a class=MstyElemLink href=\"/\">Moai�g�b�v�y�[�W</a>\n" );
	ZnkStr_add(  html, "</body></html>\n" );

	ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr(html) );
	ZnkStr_delete( html );
	ZnkStrAry_destroy( str_list );
	return ret;
}


static void
doConfigUpdate( const MoaiInfo* info, bool* is_proxy_updated, ZnkStrAry result_msgs )
{
	ZnkVarp var;

	var = ZnkVarpAry_find_byName_literal( info->vars_, "parent_proxy", false );
	if( var ){
		if( RanoParentProxy_set_byAuthority( ZnkVar_cstr(var) ) ){
			*is_proxy_updated = true;
		}
	}
	var = ZnkVarpAry_find_byName_literal( info->vars_, "post_confirm", false );
	MoaiPost_setPostConfirm( var ? true : false );

	var = ZnkVarpAry_find_byName_literal( info->vars_, "enable_log_file", false );
	MoaiServerInfo_set_enable_log_file( var ? true : false );

	var = ZnkVarpAry_find_byName_literal( info->vars_, "enable_log_verbose", false );
	MoaiServerInfo_set_enable_log_verbose( var ? true : false );

	var = ZnkVarpAry_find_byName_literal( info->vars_, "blocking_mode", false );
	MoaiConnection_setBlockingMode( var ? true : false );

	var = ZnkVarpAry_find_byName_literal( info->vars_, "filters_dir", false );
	if( var ){
		MoaiServerInfo_set_filters_dir( ZnkVar_cstr(var) );
	}

	{
		char* envvar_val = ZnkEnvVar_get( "MOAI_PROFILE_DIR" );
		if( envvar_val ){
			/* defined in OS */
			MoaiServerInfo_set_profile_dir( envvar_val );

			var = ZnkVarpAry_find_byName_literal( info->vars_, "profile_dir", false );
			if( var ){
				if( !ZnkS_eq( ZnkVar_cstr(var), envvar_val ) ){
					ZnkStrAry_push_bk_cstr( result_msgs,
							"���ϐ�MOAI_PROFILE_DIR���ݒ肳��Ă��邽�߁Aprofile_dir�ɂ��Ă͂��̒l���D�悳��܂���.",
							Znk_NPOS );
				}
			}
		} else {
			var = ZnkVarpAry_find_byName_literal( info->vars_, "profile_dir", false );
			if( var ){
				MoaiServerInfo_set_profile_dir( ZnkVar_cstr(var) );
			}
		}
		ZnkEnvVar_free( envvar_val );
	}

	{
		ZnkMyf config = RanoMyf_theConfig();
		ZnkVarpAry vars = ZnkMyf_find_vars( config, "config" );
		if( vars ){
			ZnkVarp var = NULL;

			var = ZnkVarpAry_find_byName_literal( vars, "parent_proxy", false );
			if( var ){
				const char*    hostname = RanoParentProxy_getHostname();
				const uint16_t port     = RanoParentProxy_getPort();
				char data[ 512 ];
				Znk_snprintf( data, sizeof(data), "%s:%u", hostname, port );
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "post_confirm", false );
			if( var ){
				const char* data = MoaiPost_isPostConfirm() ? "on" : "off";
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "enable_log_file", false );
			if( var ){
				const char* data = MoaiServerInfo_is_enable_log_file() ? "on" : "off";
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "enable_log_verbose", false );
			if( var ){
				const char* data = MoaiServerInfo_is_enable_log_verbose() ? "on" : "off";
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "blocking_mode", false );
			if( var ){
				const char* data = MoaiConnection_isBlockingMode() ? "on" : "off";
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "filters_dir", false );
			if( var ){
				const char* data = MoaiServerInfo_filters_dir();
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "profile_dir", false );
			if( var ){
				const char* data = MoaiServerInfo_profile_dir();
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			ZnkMyf_save( config, "config.myf" );
		}
	}
}
static void
doConfigUpdateSys( const MoaiInfo* info )
{
	ZnkVarp var;
	uint16_t moai_port    = MoaiServerInfo_port();
	uint16_t xhr_dmz_port = MoaiServerInfo_XhrDMZPort();

	var = ZnkVarpAry_find_byName_literal( info->vars_, "acceptable_host", false );
	if( var ){
		MoaiServerInfo_set_acceptable_host( ZnkVar_cstr(var) );
	}

	var = ZnkVarpAry_find_byName_literal( info->vars_, "server_name", false );
	if( var ){
		MoaiServerInfo_set_server_name( ZnkVar_cstr(var) );
	}

	MoaiServerInfo_autosetServerName();

	var = ZnkVarpAry_find_byName_literal( info->vars_, "moai_port", false );
	if( var ){
		ZnkS_getU16U( &moai_port, ZnkVar_cstr(var) );
		MoaiServerInfo_set_port( moai_port );
	}
	var = ZnkVarpAry_find_byName_literal( info->vars_, "xhr_dmz_port", false );
	if( var ){
		ZnkS_getU16U( &xhr_dmz_port, ZnkVar_cstr(var) );
		MoaiServerInfo_setXhrDMZPort( xhr_dmz_port );
	}

	{
		ZnkMyf config = RanoMyf_theConfig();
		ZnkVarpAry vars = ZnkMyf_find_vars( config, "config" );
		if( vars ){
			ZnkVarp var = NULL;

			var = ZnkVarpAry_find_byName_literal( vars, "acceptable_host", false );
			if( var ){
				const char* data = MoaiServerInfo_acceptable_host();
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "moai_port", false );
			if( var ){
				char data[ 256 ] = "0";
				Znk_snprintf( data, sizeof(data), "%u", moai_port );
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "xhr_dmz_port", false );
			if( var ){
				char data[ 256 ] = "0";
				Znk_snprintf( data, sizeof(data), "%u", xhr_dmz_port );
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "server_name", false );
			if( var ){
				const char* data = MoaiServerInfo_server_name();
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			ZnkMyf_save( config, "config.myf" );
		}
	}
}

static ZnkBird st_bird = NULL;

static void
replaceVersion( ZnkBfr file_img, const char* content_type )
{
	if( ZnkS_eq( content_type, "text/html" ) ){
		const size_t img_size = ZnkBfr_size(file_img);
		if( img_size ){
			const uint8_t* data = ZnkBfr_data( file_img );
			const bool is_terminate_null = (bool)( data[img_size-1] == '\0' );
			const size_t expected_size = img_size;
			ZnkStr_terminate_null( file_img, true );
			ZnkBird_extend_self( st_bird, file_img, expected_size );
			if( !is_terminate_null ){
				ZnkBfr_resize( file_img, ZnkBfr_size(file_img)-1 );
			}
		}
	}
}

static void
addPath( const char* var_name, const char* added_path, char connector )
{
	char*  envval      = ZnkEnvVar_get( var_name );
	ZnkStr path_envvar = ZnkStr_new( envval );

	ZnkStrPath_addPathEnvVar( path_envvar, connector, added_path );
	ZnkEnvVar_set( var_name, ZnkStr_cstr(path_envvar) );

	ZnkStr_delete( path_envvar );
	ZnkEnvVar_free( envval );
}

static int
procCGI( const ZnkStr req_urp, ZnkSocket sock, RanoModule mod,
		ZnkHtpReqMethod req_method, MoaiInfo* info, size_t content_length, bool is_xhr_dmz )
{
	int    ret = 0;
	ZnkStr filename    = ZnkStr_new( "" );
	ZnkStr dir         = ZnkStr_new( "" );
	ZnkStr query_str   = ZnkStr_new( "" );
	ZnkStr path_info   = ZnkStr_new( "" );
	ZnkStr curdir_save = ZnkStr_new( "" );
	char curdir_new[ 256 ] = "";
	const char* urp              = ZnkStr_cstr(req_urp);
	const char* intp_path        = NULL;
	const bool  is_cgi_exec_deny = MoaiCGIManager_isMatchedReqUrp( ZnkStr_cstr(req_urp), "cgi_exec_deny_urp" );

	ZnkVarp varp = MoaiCGIManager_parseURPTail( urp, dir, filename, path_info, query_str, is_cgi_exec_deny );

	//RanoLog_printf( "dir=[%s] fname=[%s], path_info=[%s] query_str=[%s]\n",
	//		ZnkStr_cstr(dir), ZnkStr_cstr(filename), ZnkStr_cstr(path_info), ZnkStr_cstr(query_str) );

	/* interpreter path */
	intp_path = varp ? ZnkVar_cstr(varp) : NULL;

	if( intp_path ){
		char cmd[ 256 ] = "";

		ZnkDir_getCurrentDir( curdir_save );

		/***
		 * curdir_new�ւ̃Z�b�g�́A�������s�^�͖ܘ_�Ainterpreter�^�̏ꍇ�ł��K�v.
		 * interpreter_fullpath script_file �Ƃ����`���Ŏw�肵���ꍇ�A
		 * interpreter�����炵�Ă݂�΁A�ʏ킻���script_file�̑��݂���p�X���J�����g�f�B���N�g���ł��邱�Ƃ�O��Ƃ���
		 * ������script_file��fopen����ƍl�����邩��ł���.
		 * (php�̂悤�ɃJ�����g�f�B���N�g����PATH_TRANSLATED���Q�Ƃ��Č��肷��P�[�X�͂ނ������ȗ�ł���)
		 */
		Znk_snprintf( curdir_new, sizeof(curdir_new), "%s/%s", ZnkStr_cstr(curdir_save), ZnkStr_cstr(dir) );
		if( ZnkS_empty(intp_path) ){
			/* Acceess to File resource */
			Znk_snprintf( cmd, sizeof(cmd), "./%s", ZnkStr_cstr(filename) );
		} else {
			/* Moai CGI */
			Znk_snprintf( cmd, sizeof(cmd), "%s %s", intp_path, ZnkStr_cstr(filename) );
		}

		/**
		 * Moai�f�B���N�g���ɂ���DLL���������ł���悤��PATH���邢��LD_LIBRARY_PATH��curdir_save��ǉ�����.
		 */
#if   defined(__CYGWIN__)
		addPath( "PATH", ZnkStr_cstr(curdir_save), ':' );
#elif defined(Znk_TARGET_WINDOWS)
		addPath( "PATH", ZnkStr_cstr(curdir_save), ';' );
#else
		addPath( "LD_LIBRARY_PATH", ZnkStr_cstr(curdir_save), ':' );
#endif

		/**
		 * PHP�ł͂�����Q�Ƃ��邽�߁A�ݒ肪�K�{.
		 */
		{
			ZnkStr path = ZnkStr_new( "" );
			ZnkStr_setf( path, "%s/%s/%s",
					ZnkStr_cstr(curdir_save), ZnkStr_cstr(dir), ZnkStr_cstr(filename) );
#if defined(__CYGWIN__)
			/***
			 * Cygwin�̏ꍇ��Cygwin�^�h���C�u��DOS�h���C�u�ɕύX���Ă���.
			 * Win32�p��php���w�肵�Ă���ꍇ�A���ꂪDOS�h���C�u�^�ł͂Ȃ��ƔF���ł��Ȃ����߂ł���.
			 * (�������ACygwin���ŃC���X�g�[�����ꂽphp���C���^�v���^�Ƃ��ėp����Ȃ�
			 * ���̕ϊ��͋t�ɖ�肩������Ȃ�)
			 */
			ZnkStrPath_convertCygDrive_toDosDrive( path );
#endif
			ZnkEnvVar_set( "PATH_TRANSLATED", ZnkStr_cstr(path) );
			ZnkStr_delete( path );
		}

		if( req_method == ZnkHtpReqMethod_e_POST ){
			ret = MoaiCGI_runPost( cmd, curdir_new, sock, mod,
					info->hdr_size_, info->hdrs_.hdr1st_, info->hdrs_.vars_,
					content_length, info->stream_, query_str, is_xhr_dmz );
		} else {
			ret = MoaiCGI_runGet( cmd, curdir_new, sock, mod,
					info->hdr_size_, info->hdrs_.hdr1st_, info->hdrs_.vars_,
					content_length, info->stream_, query_str, is_xhr_dmz );
		}
	} else {
		/* Simply FileGetting */

		ZnkDir_getCurrentDir( curdir_save );
		Znk_snprintf( curdir_new, sizeof(curdir_new), "%s/%s", ZnkStr_cstr(curdir_save), ZnkStr_cstr(dir) );
		ZnkDir_changeCurrentDir( curdir_new );
		{

			if( ZnkDir_getType( ZnkStr_cstr(filename) ) != ZnkDirType_e_File ){
				MoaiIO_sendTxtf( sock, "text/html",
						"<p><b><img src=\"/moai.png\"> Moai WebServer : 404 Not found [%s].</b></p>\n", urp );
			} else {
				if( !MoaiIO_sendResponseFile( sock, ZnkStr_cstr(filename), replaceVersion ) ){
					MoaiIO_sendTxtf( sock, "text/html",
							"<p><b><img src=\"/moai.png\"> Moai WebServer : 503 Service Unavaliable [%s].</b></p>\n", urp );
				}
			}

		}
		ZnkDir_changeCurrentDir( ZnkStr_cstr(curdir_save) );
	}

	ZnkStr_delete( dir );
	ZnkStr_delete( filename );
	ZnkStr_delete( query_str );
	ZnkStr_delete( path_info );
	ZnkStr_delete( curdir_save );

	/***
	 * �X�V���Ԃ��قȂ�ꍇ�̂�config�����ēǂݍ���.
	 * �ꕔ�̍��ڂ݂̂��X�V.
	 */
	{
		ZnkDate conf_date  = { 0 };
		ZnkDate proxy_date = RanoParentProxy_getDate();

		RanoFileInfo_getLastUpdatedTime( "config.myf", &conf_date );
		if( ZnkDate_compareDay( &conf_date, &proxy_date ) || ZnkDate_diffSecond( &conf_date, &proxy_date, 0 ) ){
			if( RanoMyf_loadConfig() ){
				ZnkMyf config = RanoMyf_theConfig();
				ZnkVarpAry vars = ZnkMyf_find_vars( config, "config" );
				if( vars ){
					ZnkVarp var = ZnkVarpAry_find_byName_literal( vars, "parent_proxy", false );
					RanoParentProxy_set_byAuthority( ZnkVar_cstr(var) );
				}
			}
		}
	}
	RanoLog_printf( "Moai : procCGI end.\n" );
	return ret;
}

static bool
extendURPAlias( ZnkStr req_urp, const ZnkVarpAry urp_alias_list )
{
	const size_t size  = ZnkVarpAry_size( urp_alias_list );
	ZnkVarp      alias = NULL;
	const char*  src   = NULL;
	const char*  dst   = NULL;
	size_t src_leng = 0;
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		alias = ZnkVarpAry_at( urp_alias_list, idx );
		src   = ZnkVar_name_cstr( alias );
		dst   = ZnkVar_cstr( alias );
		src_leng = Znk_strlen( src );
		if( ZnkStr_first(req_urp) == '/' && ZnkStr_isContain( req_urp, 1, src, src_leng ) ){
			char next_ch = ZnkStr_at( req_urp, 1 + src_leng );
			switch( next_ch ){
			case '/': case '?': case '#': case '\0':
				ZnkStr_replace( req_urp, 1, src_leng, dst, Znk_NPOS );
				return true;
			default:
				break;
			}
		}
	}
	return false;
}


/* ���{��t�@�C�����Ή� */
static void
escapeEvilCharOnFSys( ZnkStr ans, const uint8_t* data, size_t data_size )
{
	const uint8_t* begin = data;
	const uint8_t* end   = data + data_size;
	const uint8_t* p = begin;
	while( p != end ){
		switch( *p ){
		case '*':
		case '?':
		case ':':
		case ';':
		//case '~':
			ZnkStr_addf2( ans, "%%%02X", *p );
			break;
		default:
			ZnkStr_add_c( ans, *p );
			break;
		}
		++p;
	}
}
static void
unescape_forMbcFSysPath( ZnkStr fsys_path )
{
	ZnkStr tmp = ZnkStr_new( "" );
	/* encoding by web application => encoding by browser �Ɠ�i�K encoding ���Ȃ���Ă���ꍇ������. */
	ZnkHtpURL_unescape_toStr( tmp, ZnkStr_cstr(fsys_path), ZnkStr_leng(fsys_path) );
	ZnkStr_clear( fsys_path );
	ZnkHtpURL_unescape_toStr( fsys_path, ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );
	ZnkStr_clear( tmp );
	escapeEvilCharOnFSys( tmp, (uint8_t*)ZnkStr_cstr(fsys_path), ZnkStr_leng(fsys_path) );
	ZnkStr_swap( fsys_path, tmp );
	ZnkStr_delete( tmp );
}

static bool
procPrefixingAuthenticKey( ZnkStr req_urp, const char* moai_authentic_key )
{
	bool is_authenticated = false;
	const char* p = ZnkStr_cstr(req_urp);
	if( ZnkS_isBegin_literal( p, "/authentic/" ) ){
		const size_t key_leng = Znk_strlen(moai_authentic_key);
		const char* q = NULL;
		p += Znk_strlen_literal( "/authentic/" );
		q =  Znk_strchr( p, '/' );
		if( q && q > p ){
			if( (q == p + key_leng) && ZnkS_eqEx( p, moai_authentic_key, key_leng ) ){
				is_authenticated = true;
			}
		}
		{
			size_t cut_size = Znk_strlen_literal( "/authentic/" ) + key_leng;
			ZnkStr_cut_front( req_urp, cut_size );
		}
	}
	return is_authenticated;
}

static ZnkStr
gainOneQueryStringVal( const char* req_urp, const char* var_name )
{
	ZnkStr ans = NULL;
	const char* q = Znk_strchr( req_urp, '?' );
	if( q ){
		static const bool is_unescape_val = true;
		const char* query_str = q+1;
		ZnkVarpAry post_vars = ZnkVarpAry_create( true );
		RanoCGIUtil_splitQueryString( post_vars, query_str, Znk_NPOS, is_unescape_val );
		{
			ZnkVarp var = ZnkVarpAry_findObj_byName( post_vars, var_name, Znk_NPOS, false );
			if( var ){
				ans = ZnkStr_new( ZnkVar_cstr(var) );
			}
		}
		ZnkVarpAry_destroy( post_vars );
	}
	return ans;
}

static bool
checkProtectedCGI( const char* req_urp, bool is_authenticated )
{
	if( MoaiCGIManager_isMatchedReqUrp( req_urp, "protected_urp" ) ){
		/* ���s���̂�Moai_AuthenticKey���K�v */
		if( is_authenticated ){
			return true;
		} else {
			return false;
		}
	}
	return true;
}


static MoaiRASResult
do_get( ZnkSocket sock, ZnkStr req_urp,
		bool is_xhr_dmz, const char* xhr_authhost,
		MoaiInfo* info, const MoaiContext ctx )
{
	ZnkVarpAry       urp_alias_list = MoaiCGIManager_getURPAliasList();
	MoaiRASResult    ras_result     = MoaiRASResult_e_OK;
	int              ret            = 0;
	uint32_t         peer_ipaddr    = ctx->peer_ipaddr_;
	const RanoModule mod            = NULL;

	const char* profile_dir          = MoaiServerInfo_profile_dir();
	const char* moai_authentic_key   = MoaiServerInfo_authenticKey();
	bool        is_authenticated     = false;
	bool        explicit_doc_root    = false;

	ZnkStr msg_str      = ZnkStr_new( "" );
	ZnkStr fsys_path    = ZnkStr_new( "" );
	ZnkStr req_urp_dir  = ZnkStr_new( "" );
	ZnkStr req_urp_tail = ZnkStr_new( "" );

	Znk_UNUSED( ret );
	RanoLog_printf( "  As WebServer GET\n" );

	/***
	 * prefixing Moai_AuthenticKey
	 * /authentic/moai_authentic_key/ �Ŏn�܂�req_urp�`��������.
	 * req_urp�̂��̊J�n�����͈ȍ~�ł͍폜����邪�A�u���E�U��ł͎c��.
	 * �܂��x����ŉ{�����n�߂�ƁAMoai�F�؂����s�����Ƃ����d�g��.
	 */
	is_authenticated = procPrefixingAuthenticKey( req_urp, moai_authentic_key );

	/***
	 * Unaliasing.
	 */
	if( urp_alias_list ){
		extendURPAlias( req_urp, urp_alias_list );
	}

	/***
	 * Get req_urp_dir and req_urp_tail.
	 * (MUST)
	 */
	ZnkHtpURL_getReqUrpDir( req_urp_dir, req_urp_tail, ZnkStr_cstr(req_urp) );
	/***
	 * Unescape req_urp_dir for MBC fsys path.
	 * (MUST)
	 */
	unescape_forMbcFSysPath( req_urp_dir );
	unescape_forMbcFSysPath( req_urp_tail );
	/***
	 * Sanitize : /../ in req_urp_dir.
	 * (MUST)
	 */
	ZnkHtpURL_sanitizeReqUrpDir( req_urp_dir, false );
	/***
	 * Essential protection against XSS.
	 * (MUST)
	 */
	ZnkHtpURL_negateHtmlTagEffection( req_urp_dir );
	ZnkHtpURL_negateHtmlTagEffection( req_urp_tail ); /* test */

	/***
	 * Rebuild fsys_path by Sanitized and against XSS result.
	 */
	if( ZnkStr_isBegin( req_urp_dir, "/doc_root/" ) || ZnkStr_eq( req_urp_dir, "/doc_root" ) ){
		ZnkStr_set( fsys_path, "." );
		ZnkStr_add( fsys_path, ZnkStr_cstr(req_urp_dir) );
		explicit_doc_root = true;
	} else if( ZnkStr_isBegin( req_urp_dir, "/cgis/" ) || ZnkStr_eq( req_urp_dir, "/cgis" ) ){
		ZnkStr_set( fsys_path, "." );
		ZnkStr_add( fsys_path, ZnkStr_cstr(req_urp_dir) );
	} else {
		ZnkStr_set( fsys_path, "./doc_root" );
		ZnkStr_add( fsys_path, ZnkStr_cstr(req_urp_dir) );
	}
	ZnkStr_add_c( fsys_path, '/' );
	if( ZnkStr_empty(req_urp_tail) ){
		ZnkStr_add( fsys_path, "index.html" );
		RanoLog_printf( "  fsys_path=[%s]\n", ZnkStr_cstr(fsys_path) );
	} else {
		ZnkStr_add( fsys_path, ZnkStr_cstr(req_urp_tail) );
	}

	/***
	 * Mount special fsys_path.
	 */
	MoaiCGIManager_mapFSysDir2( fsys_path, profile_dir, explicit_doc_root );

	/***
	 * is_authenticated or not by Moai_AuthenticKey.
	 */
	{
		ZnkStr requested_key = gainOneQueryStringVal( ZnkStr_cstr(req_urp), "Moai_AuthenticKey" );
		if( requested_key && ZnkStr_eq( requested_key, moai_authentic_key ) ){
			is_authenticated = true;
		}
		ZnkStr_delete( requested_key );
	}

#if 0
	if( ZnkStr_isBegin( req_urp, "/test_req_urp/" ) ){
		ZnkStr uxs_fsys_path = ZnkStr_new( ZnkStr_cstr(fsys_path) );
		/***
		 * Essential protection against XSS.
		 * (MUST)
		 */
		ZnkHtpURL_negateHtmlTagEffection( uxs_fsys_path );
		/***
		 * Check uxs_fsys_path.
		 */
		ret = MoaiIO_sendTxtf( sock, "text/html",
				"<p><b>Moai WebServer : TestReqURP :</b></p>\n"
				"uxs_fsys_path=[%s]\n",
				ZnkStr_cstr(uxs_fsys_path) ); /* XSS-safe */

		ZnkStr_delete( uxs_fsys_path );
		goto FUNC_END;
	}
#endif

	/* fsys_path : XSS-safe */
	if( is_xhr_dmz ){
		/***
		 * for XhrDMZ URL
		 *
		 * �ȉ��̃��|�[�g�́AXhrDMZ��ŕ\�������̂ł��邩��A
		 * ���̃��|�[�g��Moai_AuthenticKey�̒l��\�����Ă͂Ȃ�Ȃ�
		 * (�]���ă��g���C�{�^���͂��̏ꍇ�񋟂ł��Ȃ�).
		 */

		if( MoaiCGIManager_isValidReqUrp_forPathBegin( ZnkStr_cstr(req_urp), "xhrdmz_cgi_urp" ) ){
			/* Moai-CGI */
			if( MoaiCGIManager_isValidReqUrp_forRun( ZnkStr_cstr(req_urp) ) ){
				/* Moai-CGI */
				if( checkProtectedCGI( ZnkStr_cstr(req_urp), is_authenticated ) ){
					ret = procCGI( req_urp, sock, mod,
							ctx->req_method_, info, ctx->body_info_.content_length_, is_xhr_dmz );
				} else {
					MoaiCGIManager_makeHeader( msg_str, "Moai XhrDMZ Report", false );
					ZnkStr_add( msg_str, "<p><b>Moai XhrDMZ get(authentic cgi) : 401 Unauthorized.</b></p>\n" );
					ZnkStr_add( msg_str, "<p>This request is missing Moai_AuthenticKey.</p>\n" );
					ZnkStr_add( msg_str, "<p>Sorry, this request is aborted.</p>\n" );
					ret = MoaiIO_sendTxtf( sock, "text/html", "%s", ZnkStr_cstr( msg_str ) ); /* XSS-safe */
				}
			} else {
				MoaiCGIManager_makeHeader( msg_str, "Moai XhrDMZ Report", false );
				ZnkStr_add( msg_str, "<p><b>Moai XhrDMZ get(xhrdmz_cgi_urp) : 401 Unauthorized.</b></p>\n" );
				ZnkStr_add( msg_str, "<p>This cgi script is invalid filename or Moai config.myf cannot be registered interpreter of this type.</p>\n" );
				ZnkStr_add( msg_str, "<p>Sorry, this request is aborted.</p>\n" );
				ret = MoaiIO_sendTxtf( sock, "text/html", "%s", ZnkStr_cstr( msg_str ) ); /* XSS-safe */
			}

		} else if( MoaiCGIManager_isMatchedReqUrp( ZnkStr_cstr(req_urp), "xhrdmz_only_urp" ) ){
			/* Moai-XhrDMZOnly
			 *
			 * Moai File Responsing.
			 * ���͈̔͂� Moai_AuthenticKey �͕s�v.
			 * XhrDMZ ����̂݃A�N�Z�X���\.
			 * WebServer ����͋t��URL�ɂ��A�N�Z�X��������Ȃ�( Moai_AuthenticKey ���w�肵�Ă��s�� ).
			 */
			if( !MoaiIO_sendResponseFile( sock, ZnkStr_cstr(fsys_path), replaceVersion ) ){
				ret = MoaiIO_sendTxtf( sock, "text/html",
						"<p><b><img src=\"/moai.png\"> Moai XhrDMZ get(xhrdmz_only) : 404 Not found [%s].</b></p>\n",
						ZnkStr_cstr(fsys_path) ); /* XSS-safe */
			}
		} else if( MoaiCGIManager_isMatchedReqUrp( ZnkStr_cstr(req_urp), "public_urp" ) ){
			/* Moai-Public
			 *
			 * Moai File Responsing.
			 * ���͈̔͂� Moai_AuthenticKey �͕s�v.
			 * �܂����ׂĂ� hostname ����̒��ڂ̃A�N�Z�X���\.
			 */
			if( !MoaiIO_sendResponseFile( sock, ZnkStr_cstr(fsys_path), replaceVersion ) ){
				ret = MoaiIO_sendTxtf( sock, "text/html",
						"<p><b><img src=\"/moai.png\"> Moai XhrDMZ get(public) : 404 Not found [%s].</b></p>\n",
						ZnkStr_cstr(fsys_path) ); /* XSS-safe */
			}
		} else {
			/* ��L�ȊO�͂��ׂăA�N�Z�X�s��. */
			MoaiCGIManager_makeHeader( msg_str, "Moai XhrDMZ Report", false );
			ZnkStr_addf( msg_str, "<p><b>Moai XhrDMZ get(others) : 403 Forbidden.</b></p>\n" );
			ZnkStr_addf( msg_str, "<p><b><u>Why?</u></b></p>\n" );
			ZnkStr_addf( msg_str, "<div class=MstyIndent>\n" );
			ZnkStr_addf( msg_str, "This access is not permitted by Moai XhrDMZ. <br>\n" );
			ZnkStr_addf( msg_str, "Please restart from <a class=MstyElemLink href=\"http://%s/\">Moai Top Page</a><br>\n", xhr_authhost );
			ZnkStr_addf( msg_str, "</div>\n" );
			ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr(msg_str) ); /* XSS-safe */
		}

	} else {
		/* for WebServer URL */

		if( ZnkStr_eq( req_urp, "/config" ) || ZnkStr_isBegin( req_urp, "/config?" ) ){
			/* Moai WebServer config
			 *
			 * ���Ύ��̂�Moai_AuthenticKey�͕s�v.
			 */
			ZnkStr mode = gainOneQueryStringVal( ZnkStr_cstr(req_urp), "mode" );
			if( mode && ZnkStr_eq( mode, "sys" ) ){
				/* Moai-SysConfig */
				ret = printSysConfig( sock, NULL, peer_ipaddr );
			} else if( mode && ZnkStr_eq( mode, "upgrade" ) ){
				/* Moai-Upgrade */
				{
					ZnkFile fp = Znk_fopen( "doc_root/state.dat", "wb" );
					if( fp ){
						Znk_fprintf( fp, "100 None." );
						Znk_fclose( fp );
					}
				}
				ret = printUpgrade( sock, NULL, peer_ipaddr, true, false );
			} else if( mode && ZnkStr_eq( mode, "init_mode" ) ){
				/* Moai-InitCommand */
				if( is_authenticated ){
					MoaiServerInfo_initiate_mod_ary();
					ret = MoaiIO_sendTxtf( sock, "text/html", "initiate_mod_ary done." );
				} else {
					ret = MoaiIO_sendTxtf( sock, "text/html", "initiate_mod_ary is not permitted." );
				}
			} else {
				/* Moai-Config */
				ret = printConfig( sock, NULL, peer_ipaddr );
			}
			ZnkStr_delete( mode );

		} else if( MoaiCGIManager_isMatchedReqUrp( ZnkStr_cstr(req_urp), "xhrdmz_only_urp" ) ){
			/* Moai-XhrDMZOnly
			 *
			 * ���͈̔͂� Moai WebServer �����URL�ɂ��A�N�Z�X��������Ȃ�.
			 * �Ⴆ�Εs�p�ӂ�Moai WebServer�ŕ\�����ׂ��ł͂Ȃ��L���b�V���Ȃǂ�����ɊY������.
			 * ( Moai_AuthenticKey ���w�肵�Ă��s�� ).
			 */
			const char* xhr_dmz = MoaiServerInfo_XhrDMZ();
			MoaiCGIManager_makeHeader( msg_str, "Moai WebServer Report", false );
			ZnkStr_addf( msg_str, "<p><b>Moai WebServer get(xhrdmz_only) : 403 Forbidden.</b></p>\n" );
			ZnkStr_addf( msg_str, "<p><b><u>Why?</u></b></p>\n" );
			ZnkStr_addf( msg_str, "<div class=MstyIndent>\n" );
			ZnkStr_addf( msg_str, "This access is permitted by Moai XhrDMZ only.<br>\n" );
			ZnkStr_addf( msg_str, "<a class=MstyElemLink href=\"http://%s%s\">Please retry this</a><br>\n",
					xhr_dmz, ZnkStr_cstr(req_urp) );
			ZnkStr_addf( msg_str, "</div>\n" );
			ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr(msg_str) ); /* XSS-safe */

		} else if( ZnkStr_isBegin( req_urp, "/cgis/" ) ){

			if( MoaiCGIManager_isValidReqUrp_forRun( ZnkStr_cstr(req_urp) ) ){
				/* Moai-CGI */
				if( checkProtectedCGI( ZnkStr_cstr(req_urp), is_authenticated ) ){
					ret = procCGI( req_urp, sock, mod,
							ctx->req_method_, info, ctx->body_info_.content_length_, is_xhr_dmz );
				} else {
					MoaiCGIManager_makeHeader( msg_str, "Moai WebServer Report", false );
					ZnkStr_addf( msg_str, "<p><b>Moai WebServer get(authentic cgi) : 401 Unauthorized.</b></p>\n" );
					ZnkStr_addf( msg_str, "<p><b><u>What this?</u></b></p>\n" );
					ZnkStr_addf( msg_str, "<div class=MstyIndent>\n" );
					ZnkStr_addf( msg_str, "We must affix Moai_AuthenticKey to our URL to execute this CGI.<br>\n" );
					ZnkStr_addf( msg_str, "<a class=MstyElemLink href=\"/authentic/%s%s\">Please retry this.</a><br>\n",
							moai_authentic_key, ZnkStr_cstr(req_urp) );
					ZnkStr_addf( msg_str, "</div>\n" );
					ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr(msg_str) ); /* XSS-safe */
				}

			} else if( MoaiCGIManager_isMatchedReqUrp( ZnkStr_cstr(req_urp), "public_urp" ) ){
				/* Moai-Public
				 *
				 * Moai File Responsing.
				 * ���͈̔͂� Moai_AuthenticKey �͕s�v.
				 * Moai WebServer �� XhrDMZ �̂����ꂩ������ڃA�N�Z�X���\.
				 */
				if( !MoaiIO_sendResponseFile( sock, ZnkStr_cstr(fsys_path), replaceVersion ) ){
					ret = MoaiIO_sendTxtf( sock, "text/html",
							"<p><b><img src=\"/moai.png\"> Moai WebServer get(public) : 404 Not found [%s].</b></p>\n",
							ZnkStr_cstr(fsys_path) ); /* XSS-safe */
				}

			} else {
				/* Moai-Authentic-Resource
				 *
				 * ���͈̔͂ł� Moai_AuthenticKey ��K�v�Ƃ���.
				 */
				if( is_authenticated ){
					if( !MoaiIO_sendResponseFile( sock, ZnkStr_cstr(fsys_path), replaceVersion ) ){
						ret = MoaiIO_sendTxtf( sock, "text/html",
								"<p><b><img src=\"/moai.png\"> Moai WebServer get(authentic resource) : 404 Not found [%s].</b></p>\n",
								ZnkStr_cstr(fsys_path) ); /* XSS-safe */
					}
				} else {
					/* Moai_AuthenticKey ��t������Retry access���K�v */
					MoaiCGIManager_makeHeader( msg_str, "Moai WebServer Report", false );
					ZnkStr_addf( msg_str, "<p><b>Moai WebServer get(authentic resource) : 401 Unauthorized.</b></p>\n" );
					ZnkStr_addf( msg_str, "<p><b><u>What this?</u></b></p>\n" );
					ZnkStr_addf( msg_str, "<div class=MstyIndent>\n" );
					ZnkStr_addf( msg_str, "We must affix Moai_AuthenticKey to our URL to get this resource. <br>\n" );
					ZnkStr_addf( msg_str, "<a class=MstyElemLink href=\"/authentic/%s%s\">Please retry this</a><br>\n",
							moai_authentic_key, ZnkStr_cstr(req_urp) );
					ZnkStr_addf( msg_str, "</div>\n" );
					ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr(msg_str) ); /* XSS-safe */
				}
			}
		} else {
			/* Moai File Responsing */

			if( MoaiCGIManager_isMatchedReqUrp( ZnkStr_cstr(req_urp), "protected_urp" ) ){
				/* Moai-Protected
				 *
				 * Moai File Responsing.
				 * ���͈̔͂ł� Moai_AuthenticKey ��K�v�Ƃ���.
				 */
				if( is_authenticated ){
					if( !MoaiIO_sendResponseFile( sock, ZnkStr_cstr(fsys_path), replaceVersion ) ){
						ret = MoaiIO_sendTxtf( sock, "text/html",
								"<p><b><img src=\"/moai.png\"> Moai WebServer get(protected) : 404 Not found [%s].</b></p>\n",
								ZnkStr_cstr(fsys_path) ); /* XSS-safe */
					}
				} else {
					MoaiCGIManager_makeHeader( msg_str, "Moai WebServer Report", false );
					ZnkStr_addf( msg_str, "<p><b>Moai WebServer get(protected) : 401 Unauthorized.</b></p>\n" );
					ZnkStr_addf( msg_str, "<p><b><u>What this?</u></b></p>\n" );
					ZnkStr_addf( msg_str, "<div class=MstyIndent>\n" );
					ZnkStr_addf( msg_str, "&nbsp;&nbsp;We must affix authentication key to our URL to get this resource. <br>\n" );
					ZnkStr_addf( msg_str, "<a class=MstyElemLink href=\"/authentic/%s%s\">Please retry this</a><br>\n",
							moai_authentic_key, ZnkStr_cstr(req_urp) );
					ZnkStr_addf( msg_str, "</div>\n" );
					ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr(msg_str) ); /* XSS-safe */
				}
			} else {
				/* Moai-Public
				 *
				 * Moai File Responsing.
				 * ���͈̔͂ł͂��ׂĂ� hostname ��� Moai_AuthenticKey �Ȃ��ŃA�N�Z�X�\�ł���.
				 */
				if( !MoaiIO_sendResponseFile( sock, ZnkStr_cstr(fsys_path), replaceVersion ) ){
					ret = MoaiIO_sendTxtf( sock, "text/html",
							"<p><b><img src=\"/moai.png\"> Moai WebServer get(public) : 404 Not found [%s].</b></p>\n",
							ZnkStr_cstr(fsys_path) ); /* XSS-safe */
				}
			}
		}

	}

	goto FUNC_END; /* for suppressing warning unused goto */
FUNC_END:
	ZnkStr_delete( msg_str );
	ZnkStr_delete( fsys_path );
	ZnkStr_delete( req_urp_dir );
	ZnkStr_delete( req_urp_tail );

	return ras_result;
}

static MoaiRASResult
do_post( ZnkSocket sock, ZnkStr req_urp,
		bool is_xhr_dmz, const char* xhr_authhost,
		MoaiInfo* info, const MoaiContext ctx, MoaiFdSet mfds, bool* is_proxy_updated )
{
	ZnkVarpAry          urp_alias_list = MoaiCGIManager_getURPAliasList();
	MoaiRASResult       ras_result     = MoaiRASResult_e_OK;
	int                 ret            = 0;
	bool                is_authenticated = false;
	uint32_t            peer_ipaddr    = ctx->peer_ipaddr_;
	const RanoModule    mod            = NULL;
	const RanoModuleAry mod_ary        = MoaiServerInfo_mod_ary();

	ZnkStr    msg_str      = ZnkStr_new( "" );
	ZnkStr    pst_str      = ZnkStr_new( "" );
	ZnkStr    req_urp_dir  = ZnkStr_new( "" );
	ZnkStr    req_urp_tail = ZnkStr_new( "" );
	ZnkStrAry result_msgs  = ZnkStrAry_create( true );

	bool        show_forbidden = false;
	const char* moai_authentic_key = MoaiServerInfo_authenticKey();

	Znk_UNUSED( ret );
	*is_proxy_updated = false;

	/***
	 * prefixing Moai_AuthenticKey
	 * /authentic/moai_authentic_key/ �Ŏn�܂�req_urp�`��������.
	 * req_urp�̂��̊J�n�����͈ȍ~�ł͍폜����邪�A�u���E�U��ł͎c��.
	 * �܂��x����ŉ{�����n�߂�ƁAMoai�F�؂����s�����Ƃ����d�g��.
	 */
	is_authenticated = procPrefixingAuthenticKey( req_urp, moai_authentic_key );

	/***
	 * Unaliasing.
	 */
	if( urp_alias_list ){
		extendURPAlias( req_urp, urp_alias_list );
	}

	/***
	 * Get req_urp_dir and Sanitize : /../ in req_urp_dir.
	 * (MUST)
	 */
	ZnkHtpURL_getReqUrpDir( req_urp_dir, req_urp_tail, ZnkStr_cstr(req_urp) );
	ZnkHtpURL_sanitizeReqUrpDir( req_urp_dir, true );
	/***
	 * Essential protection against XSS.
	 * (MUST)
	 */
	ZnkHtpURL_negateHtmlTagEffection( req_urp_dir );
	ZnkHtpURL_negateHtmlTagEffection( req_urp_tail );

	MoaiPost_parsePostAndCookieVars( sock, mfds,
			pst_str,
			info->hdr_size_, &info->hdrs_,
			ctx->body_info_.content_length_, info->stream_,
			info->vars_,
			mod, is_authenticated );

	/***
	 * is_authenticated or not.
	 */
	if( info->vars_ ){
		ZnkVarp var = NULL;
		var = ZnkVarpAry_find_byName_literal( info->vars_, "Moai_AuthenticKey", false );
		if( var && ZnkS_eq( moai_authentic_key, ZnkVar_cstr(var) ) ){
			is_authenticated = true;
		}
	}

	ZnkHtpURL_negateHtmlTagEffection( pst_str ); /* for XSS */
	{
		ZnkSRef old_ptn = { 0 };
		ZnkSRef new_ptn = { 0 };
		ZnkSRef_set_literal( &old_ptn, "\n" );
		ZnkSRef_set_literal( &new_ptn, "<br>\n" );
		ZnkStrEx_replace_BF( pst_str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
	}

	if( is_xhr_dmz ){
		/***
		 * ��{�I��XhrDMZ��ł́APOST Request�ɂ��t�@�C���A�N�Z�X��F�߂�ׂ��ł͂Ȃ�.
		 * ���POST Request�ɂ��CGI���s�͂��蓾��.
		 * �Ⴆ��BBS��ɗp�ӂ��ꂽ���e�t�H�[���Ȃǂ͂��̕����ɂ����ď�������`�ɂȂ邾�낤.
		 */
		if( MoaiCGIManager_isValidReqUrp_forPathBegin( ZnkStr_cstr(req_urp), "xhrdmz_cgi_urp" ) ){
			/* Moai-CGI */
			if( MoaiCGIManager_isValidReqUrp_forRun( ZnkStr_cstr(req_urp) ) ){
				/* Moai-CGI */
				if( checkProtectedCGI( ZnkStr_cstr(req_urp), is_authenticated ) ){
					ret = procCGI( req_urp, sock, mod,
							ctx->req_method_, info, ctx->body_info_.content_length_, is_xhr_dmz );
				} else {
					MoaiCGIManager_makeHeader( msg_str, "Moai XhrDMZ Report", false );
					ZnkStr_add( msg_str, "<p><b>Moai XhrDMZ post(authentic cgi) : 401 Unauthorized.</b></p>\n" );
					ZnkStr_add( msg_str, "<p>This post is missing Moai_AuthenticKey.</p>\n" );
					ZnkStr_add( msg_str, "<p>Sorry, this post is aborted.</p>\n" );
					ret = MoaiIO_sendTxtf( sock, "text/html", "%s", ZnkStr_cstr( msg_str ) ); /* XSS-safe */
				}
			} else {
				MoaiCGIManager_makeHeader( msg_str, "Moai XhrDMZ Report", false );
				ZnkStr_add( msg_str, "<p><b>Moai XhrDMZ post(xhrdmz_cgi_urp) : 401 Unauthorized.</b></p>\n" );
				ZnkStr_add( msg_str, "<p>This cgi script is invalid filename or Moai config.myf cannot be registered interpreter of this type.</p>\n" );
				ZnkStr_add( msg_str, "<p>Sorry, this post is aborted.</p>\n" );
				ret = MoaiIO_sendTxtf( sock, "text/html", "%s", ZnkStr_cstr( msg_str ) ); /* XSS-safe */
			}
		} else {
			/* Moai Msg Responsing */
			ret = MoaiIO_sendTxtf( sock, "text/html", "%s", ZnkStr_cstr( msg_str ) ); /* XSS-safe */
		}
	} else {
		const char* upgrade_cmd = "";
		if( info->vars_ ){
			ZnkVarp var = NULL;
	
			var = ZnkVarpAry_find_byName_literal( info->vars_, "Moai_Update", false );
			if( var ){
				if( is_authenticated ){
					doConfigUpdate( info, is_proxy_updated, result_msgs );
					ras_result = MoaiRASResult_e_OK;
				} else {
					show_forbidden = true;
				}
			}
	
			var = ZnkVarpAry_find_byName_literal( info->vars_, "Moai_Initiate", false );
			if( var ){
				if( is_authenticated ){
					RanoModuleAry_initiateFilters( mod_ary, result_msgs );
					ras_result = MoaiRASResult_e_OK;
				} else {
					show_forbidden = true;
				}
			}
	
			var = ZnkVarpAry_find_byName_literal( info->vars_, "Moai_RestartServer", false );
			if( var ){
				if( is_authenticated ){
					ras_result = MoaiRASResult_e_RestartServer;
					MoaiServerInfo_set_you_need_to_restart_moai( false );
				} else {
					show_forbidden = true;
				}
			}
	
			var = ZnkVarpAry_find_byName_literal( info->vars_, "Moai_UpdateSys", false );
			if( var ){
				if( is_authenticated ){
					doConfigUpdateSys( info );
					ras_result = MoaiRASResult_e_OK;
					MoaiServerInfo_set_you_need_to_restart_moai( true );
				} else {
					show_forbidden = true;
				}
			}

			var = ZnkVarpAry_find_byName_literal( info->vars_, "Moai_UpgradeCmd", false );
			if( var ){
				if( is_authenticated ){
					ras_result = MoaiRASResult_e_OK;
					upgrade_cmd = ZnkVar_cstr( var );
				} else {
					show_forbidden = true;
				}
			}
	
		}

		if( show_forbidden ){
			MoaiCGIManager_makeHeader( msg_str, "Moai WebServer Report", false );
			ZnkStr_add( msg_str, "<p><b><img src=\"/moai.png\"> Moai WebServer : 403 Forbidden.</b></p>\n" );
			ZnkStr_add( msg_str, "<p><b><u>Why?</u></b></p>\n" );
			ZnkStr_add( msg_str, "<div class=MstyIndent>\n" );
			ZnkStr_add( msg_str, "Maybe, your Moai server is restarted in midstream. <br>\n" );
			ZnkStr_add( msg_str, "Or this operation doesn't follow a regular procedure.<br>\n" );
			ZnkStr_add( msg_str, "Please back to <a class=MstyElemLink href=\"/\">Moai Top Page</a><br>\n" );
			ZnkStr_add( msg_str, "</div>\n" );
			goto FUNC_END;
		}
	
		if( !MoaiServerInfo_is_enable_log_file() ){
			RanoLog_printf( "Moai : config : enable_log_file is off.\n" );
			RanoLog_printf( "Moai : Stop writing to log file and close it here.\n" );
			RanoLog_close();
		}

		if( ras_result == MoaiRASResult_e_RestartServer ){
			ZnkStr_clear( msg_str );
			ZnkStr_add( msg_str, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">" );
			ZnkStr_add( msg_str, "<html><head><META HTTP-EQUIV=\"refresh\" content=\"1;URL=/config\"></head>" );
			ZnkStr_add( msg_str, "<body>Moai Server is Restarted.<br>\n" );
			ZnkStr_add( msg_str, "<a class=MstyElemLink href=\"/config\">Moai�G���W���ݒ�ɖ߂�</a></body></html>\n" );
			ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr( msg_str ) ); /* XSS-safe */

		} else {
			/***
			 * Unaliasing.
			 */
	
			if( ZnkStr_eq( req_urp, "/config" ) || ZnkStr_isBegin( req_urp, "/config?" ) ){
				if( is_authenticated ){
					ZnkStr mode = gainOneQueryStringVal( ZnkStr_cstr(req_urp), "mode" );
					if( mode && ZnkStr_eq( mode, "sys" ) ){
						/* Moai-SysConfig */
						ret = printSysConfig( sock, result_msgs, peer_ipaddr );
					} else if( mode && ZnkStr_eq( mode, "upgrade" ) ){
						/* Moai-Upgrade */
						ZnkStr curdir_save = ZnkStr_new( "" );
						ZnkDir_getCurrentDir( curdir_save );
						ZnkDir_changeCurrentDir( "birdman" );
						if( ZnkS_eq( upgrade_cmd, "query" ) ){
							const char* birdman[] = { "birdman", "upgrade_query" };
							ret = printUpgrade( sock, NULL, peer_ipaddr, false, false );
							ZnkProcess_execChild( 2, birdman, false, ZnkProcessConsole_e_Detached );
						} else if( ZnkS_eq( upgrade_cmd, "download" ) ){
							const char* birdman[] = { "birdman", "upgrade_download" };
							ret = printUpgrade( sock, NULL, peer_ipaddr, false, false );
							ZnkProcess_execChild( 2, birdman, false, ZnkProcessConsole_e_Detached );
						} else if( ZnkS_eq( upgrade_cmd, "apply" ) ){
							ret = printUpgrade( sock, NULL, peer_ipaddr, false, true );
							ras_result = MoaiRASResult_e_RestartProcess;
						}
						ZnkDir_changeCurrentDir( ZnkStr_cstr(curdir_save) );
						ZnkStr_delete( curdir_save );

					} else if( mode && ZnkStr_eq( mode, "init_mode" ) ){
						/* Moai-InitCommand */
						MoaiServerInfo_initiate_mod_ary();
						ret = MoaiIO_sendTxtf( sock, "text/html", "initiate_mod_ary done." ); /* XSS-safe */
					} else {
						/* Moai-Config */
						ret = printConfig( sock, result_msgs, peer_ipaddr );
					}
					ZnkStr_delete( mode );
				} else {
					MoaiCGIManager_makeHeader( msg_str, "Moai WebServer Report", false );
					ZnkStr_add( msg_str, "<p><b>Moai WebServer post(config) : 401 Unauthorized.</b></p>\n" );
					ZnkStr_add( msg_str, "<p>This post is missing Moai_AuthenticKey.</p>\n" );
					ZnkStr_add( msg_str, "<p>Sorry, this post is aborted.</p>\n" );
					ret = MoaiIO_sendTxtf( sock, "text/html", "%s", ZnkStr_cstr( msg_str ) ); /* XSS-safe */
				}
			} else if( ZnkStr_isBegin( req_urp, "/cgis/" ) ){
				/* Moai-CGI */
				if( MoaiCGIManager_isValidReqUrp_forRun( ZnkStr_cstr(req_urp) ) ){
					/* Moai-CGI */
					if( checkProtectedCGI( ZnkStr_cstr(req_urp), is_authenticated ) ){
						ret = procCGI( req_urp, sock, mod,
								ctx->req_method_, info, ctx->body_info_.content_length_, is_xhr_dmz );
					} else {
						MoaiCGIManager_makeHeader( msg_str, "Moai WebServer Report", false );
						ZnkStr_add( msg_str, "<p><b>Moai WebServer post(authentic cgi) : 401 Unauthorized.</b></p>\n" );
						ZnkStr_add( msg_str, "<p>This post is missing Moai_AuthenticKey.</p>\n" );
						ZnkStr_add( msg_str, "<p>Sorry, this post is aborted.</p>\n" );
						ZnkStr_add( msg_str, "<div class=MstyComment>\n" );
						ZnkStr_add( msg_str, ZnkStr_cstr(pst_str) );
						ZnkStr_add( msg_str, "</div><br>\n" );
						ret = MoaiIO_sendTxtf( sock, "text/html", "%s", ZnkStr_cstr( msg_str ) ); /* XSS-safe */
					}
				} else {
					MoaiCGIManager_makeHeader( msg_str, "Moai WebServer Report", false );
					ZnkStr_add( msg_str, "<p><b>Moai WebServer post : 401 Unauthorized.</b></p>\n" );
					ZnkStr_add( msg_str, "<p>This cgi script is invalid filename or Moai config.myf cannot be registered interpreter of this type.</p>\n" );
					ZnkStr_add( msg_str, "<div class=MstyComment>\n" );
					ZnkStr_add( msg_str, ZnkStr_cstr(pst_str) );
					ZnkStr_add( msg_str, "</div><br>\n" );
					ret = MoaiIO_sendTxtf( sock, "text/html", "%s", ZnkStr_cstr( msg_str ) ); /* XSS-safe */
				}

			} else {
				/* Moai Msg Responsing */
				MoaiCGIManager_makeHeader( msg_str, "Moai WebServer Report", false );
				ZnkStr_add( msg_str, "<p><b><img src=\"/moai.png\"> Moai WebServer : 403 Forbidden.</b></p>\n" );
				ZnkStr_add( msg_str, "<p>The destination of this post is invalid.</p>\n" );
				ZnkStr_add( msg_str, "<div class=MstyComment>\n" );
				ZnkStr_add( msg_str, ZnkStr_cstr(pst_str) );
				ZnkStr_add( msg_str, "</div><br>\n" );
				ret = MoaiIO_sendTxtf( sock, "text/html", "%s", ZnkStr_cstr( msg_str ) ); /* XSS-safe */
			}
		}
	}

FUNC_END:
	ZnkStr_delete( pst_str );
	ZnkStr_delete( msg_str );
	ZnkStr_delete( req_urp_dir );
	ZnkStr_delete( req_urp_tail );
	ZnkStrAry_destroy( result_msgs );

	return ras_result;
}

/***
 * Moai WebServer Engine Ver2.0
 */
MoaiRASResult
MoaiWebServer_do( const MoaiContext ctx, ZnkSocket sock, MoaiConnection mcn, MoaiFdSet mfds )
{
	ZnkFdSet            fdst_observe_r = MoaiFdSet_fdst_observe_r( mfds );
	MoaiRASResult       ras_result     = MoaiRASResult_e_OK;
	int                 ret            = 0;

	MoaiInfo*           info           = ctx->draft_info_;
	ZnkStr              req_urp        = info->req_urp_;
	bool                is_xhr_dmz     = false;
	const char*         xhr_authhost = MoaiServerInfo_XhrAuthHost();

	/***
	 * Decide xhr_dmz or not.
	 */
	is_xhr_dmz = (bool)( MoaiConnection_port( mcn ) == MoaiServerInfo_XhrDMZPort() );

	/***
	 * Bird version replacer.
	 */
	if( st_bird == NULL ){
		st_bird = ZnkBird_create( "$[", "]$" );
	}
	{
		const char* version_str = MoaiServerInfo_version( false );
		ZnkBird_regist( st_bird, "MoaiVersion", version_str );
	}

	switch( ctx->req_method_ ){
	case ZnkHtpReqMethod_e_GET:
	{
		/***
		 * Ver2.0: GET Security-enhancement:
		 */
		ras_result = do_get( sock, req_urp,
				is_xhr_dmz, xhr_authhost,
				info, ctx );

		RanoLog_printf( "  Requested is done.\n" );
		if( ret < 0 || !mcn->I_is_keep_alive_ ){
			MoaiIO_close_ISock( "  WebServerGET", sock, mfds );
		}
		break;
	}
	case ZnkHtpReqMethod_e_POST:
	{
		bool is_proxy_updated = false;
		/***
		 * Ver2.0: POST Security-enhancement:
		 */
		ras_result = do_post( sock, req_urp,
				is_xhr_dmz, xhr_authhost,
				info, ctx, mfds, &is_proxy_updated );
		/***
		 * do_post �ɂ�����, body���̎c��͂��ׂ�Recv�����͂�.
		 */
		mcn->req_content_length_remain_ = 0;

		/***
		 * MoaiSever��Restart����ꍇ���A��U�R�l�N�g���N���[�X���������悢.
		 * �����Ȃ��ƃu���E�U���Łu�ڑ������Z�b�g����܂����v���\������邱�Ƃ�����s���D�ł���.
		 */
		if( ret < 0 || !mcn->I_is_keep_alive_ ||
				ras_result == MoaiRASResult_e_RestartServer || ras_result == MoaiRASResult_e_RestartProcess ){
			MoaiIO_close_ISock( "  WebServerPOST", sock, mfds );
		}
		/***
		 * ����Observe����Ă���Socket�����ׂċ����I��close����.
		 */
		if( is_proxy_updated ){
			MoaiIO_closeSocketAll( "  ProxyUpdate", fdst_observe_r, mfds );
		}
		break;
	}
	case ZnkHtpReqMethod_e_HEAD:
	{
#if 1
		ZnkStr ans = ZnkStr_new( "" );
		ZnkStr_add( ans, "HTTP/1.1 200 OK\r\n" );
		//ZnkStr_add( ans, "Date: Fri, 22 Sep 2017 21:18:00 GMT\r\n" );
		//ZnkStr_add( ans, "Content-Type: text/html\r\n" );
		//ZnkStr_add( ans, "Content-Length: 363857\r\n" );
		//ZnkStr_add( ans, "Connection: keep-alive\r\n" );
		//ZnkStr_add( ans, "Last-Modified: Fri, 22 Sep 2017 21:18:00 GMT\r\n" );
		////ZnkStr_add( ans, "CF-Cache-Status: HIT\r\n" );
		//ZnkStr_add( ans, "Vary: Accept-Encoding\r\n" );
		//ZnkStr_add( ans, "Expires: Fri, 23 Sep 2017 15:52:29 GMT\r\n" );
		////ZnkStr_add( ans, "Cache-Control: public, max-age=14400\r\n" );
		//ZnkStr_add( ans, "Accept-Ranges: bytes\r\n" );
		//ZnkStr_add( ans, "Server: cloudflare-nginx\r\n" );
		ZnkStr_add( ans, "\r\n" );
		/* test. */
		ZnkSocket_send_cstr( sock, ZnkStr_cstr(ans) );
#endif

		RanoLog_printf( "  Requested is done.\n" );
		if( ret < 0 || !mcn->I_is_keep_alive_ ){
			MoaiIO_close_ISock( "  WebServerGET", sock, mfds );
		}

		break;
	}
	default:
		break;
	}
	return ras_result;
}
