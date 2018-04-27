function EjsAssort_submitCommand( form, manager, cmd_val )
{
	form.action ="/cgis/easter/easter.cgi?est_manager=" + manager + "&command=" + cmd_val;
	form.submit();
}
function EjsAssort_submitCommandAndArg( form, manager, cmd_val, arg )
{
	if( arg && arg != '' ){
		form.action = "/cgis/easter/easter.cgi?est_manager=" + manager + "&command=" + cmd_val + "&" + arg;
	} else {
		form.action = "/cgis/easter/easter.cgi?est_manager=" + manager + "&command=" + cmd_val;
	}
	form.submit();
}
function EjsAssort_submitCommandEx( form, manager, cmd_val, arg, anchor )
{
	if( arg && arg != '' ){
		form.action = "/cgis/easter/easter.cgi?est_manager=" + manager + "&command=" + cmd_val + "&" + arg + anchor;
	} else {
		form.action = "/cgis/easter/easter.cgi?est_manager=" + manager + "&command=" + cmd_val + anchor;
	}
	form.submit();
}
function EjsAssort_switchVisibility( id, sfx_cbtn, sfx_body, sfx_msg )
{
	var id_btn  = id + sfx_cbtn;
	var id_list = id + sfx_body;
	var id_msg  = id + sfx_msg;
	var state = "block";
	var msg = document.getElementById( id_msg );
	if( document.getElementById( id_btn ).checked ){
		state = "block";
		if( msg ){
			msg.innerHTML = "";
		}
	} else {
		state = "none";
		if( msg ){
			msg.innerHTML = "Å\ åªç›îÒï\é¶Ç≈Ç∑ Å\";
		}
	}
	document.getElementById( id_list ).style.display = state;
}
function EjsAssort_switchGroupVisibility( group_list_keys, show )
{
	for(var i=0;i<group_list_keys.length;i++){
		var group_list_key = group_list_keys[ i ] + "_list";
		if( show ){
			document.getElementById( group_list_key ).style.display = 'block';
		} else {
			document.getElementById( group_list_key ).style.display = 'none';
		}
	}
}
function EjsAssort_switch_byCheckbox( id, var_name, val_if_true, val_if_false )
{
	var ckbtn    = document.getElementById( id );
	var variable = document.getElementById( var_name );
	if( ckbtn.checked ){
		variable.value = val_if_true;
	} else {
		variable.value = val_if_false;
	}
}

function EjsAssort_checkAll( prefix, begin_idx, end_idx, val )
{
	//var js_console = document.getElementById( "js_console" );
	var name = prefix;
	//var msg = "*";
	var btn = null;
	for( idx = begin_idx; idx < end_idx; ++idx ){
		name = prefix + idx;
		btn = document.getElementById( name );
		if( btn != null ){
			btn.checked = val;
		}
	}
	//js_console.innerHTML = msg;
}

function EjsAssort_checkExist( cache_path, begin_idx, end_idx, authentic_key, previewing_img_url )
{
	var url = "/easter?est_manager=cache&command=check_exist&cache_path=" + cache_path +
		"&est_cache_begin=" + begin_idx + "&est_cache_end=" + end_idx +
		"&previewing_img_url=" + previewing_img_url +
		"&Moai_AuthenticKey=" + authentic_key + "#PageSwitcher";
	 location.href = url;
}
