(function(window, document, undefined) {

	/* IE8 */
	var elems = document.getElementsByTagName( 'span' );
	for( var i=0; i<elems.length; ++i ){
		if( elems[ i ].className == "MstyAutoLinkTarget" ){
			elems[ i ].innerHTML = "http://" + location.host + elems[ i ].innerHTML;
		}
	}
	/* IE8 ˆÈŠO
	var easter_autolink_target_list = document.getElementsByClassName("MstyAutoLinkTarget");
	for( var i=0; i<easter_autolink_target_list.length; ++i ){
		easter_autolink_target_list[ i ].innerHTML = "http://" + location.host + easter_autolink_target_list[ i ].innerHTML;
	}
	*/

}(window, document));
