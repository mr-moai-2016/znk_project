/* 0-byte avoidance */

function Easter_toURP_forPOST( urp )
{
	var mdf_urp = urp.replace( /\?/g, "%3F" );
	var mdf_urp = urp.replace( /=/g,  "%3D" );
	var mdf_urp = urp.replace( /&/g,  "%26" );
	return "/easter?est_post=" + Easter_hostname + mdf_urp;
}
function Easter_toURP_forGET( urp )
{
	var mdf_urp = urp.replace( /\?/g, "%3F" );
	var mdf_urp = urp.replace( /=/g,  "%3D" );
	var mdf_urp = urp.replace( /&/g,  "%26" );
	return "/easter?est_get=" + Easter_hostname + mdf_urp;
}

function Easter_reload( url_src, bbs_id_name )
{
	var scrly = document.documentElement.scrollTop || document.body.scrollTop;
	var bbss = document.getElementsByName( bbs_id_name );
	if( bbss && bbss.length > 0 ){
		var resn = bbss.item(0).value;
		document.cookie="scrl="+resn+"."+scrly+"; max-age=60;";
	}
	location.href="/easter?est_reload="+url_src;
	return false;
}

function Easter_scroll( bbs_id_name )
{
	var scrly=getCookie("scrl").split(".");
	if(scrly[1]!=null &&  scrly[1]>0 && document.getElementsByName( bbs_id_name ).item(0).value == scrly[0]){
		window.scroll(0,scrly[1]);
	}
	//document.cookie="scrl=; max-age=0;";
	{
		var except_name_ary = [ 'reszc' ];
		PascuaCookie.clearCookie( except_name_ary, true );
	}
}

/***
 * 不要なCallback関数を何もしない関数に置き換えるために用いる.
 */
function Easter_noneFunc()
{
}

(function(window, document, undefined) {

	/* IE8 */
	var elems = document.getElementsByTagName( 'span' );
	for( var i=0; i<elems.length; ++i ){
		if( elems[ i ].className == "MstyAutoLinkTarget" ){
			elems[ i ].innerHTML = "http://" + location.host + elems[ i ].innerHTML;
		}
	}
	/* IE8 以外
	var easter_autolink_target_list = document.getElementsByClassName("MstyAutoLinkTarget");
	for( var i=0; i<easter_autolink_target_list.length; ++i ){
		easter_autolink_target_list[ i ].innerHTML = "http://" + location.host + easter_autolink_target_list[ i ].innerHTML;
	}
	*/

}(window, document));
