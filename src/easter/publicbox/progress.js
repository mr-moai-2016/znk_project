var st_state = "";
var st_dat_path = "";
var st_result_url = "";
function set_dat_path( dat_path ){
	st_dat_path = dat_path;
}
function set_result_url( result_url ){
	st_result_url = result_url;
}
function refer_state(){
	var xmlhttp = false;
	if( typeof ActiveXObject != "undefined" ){
		try {
			xmlhttp = new ActiveXObject( "Microsoft.XMLHTTP" );
		} catch (e) {
			xmlhttp = false;
		}
	}
	if( !xmlhttp && typeof XMLHttpRequest != "undefined" ){
		xmlhttp = new XMLHttpRequest();
	}
	xmlhttp.open( "GET", st_dat_path + '?date=' + new Date().getTime() );
	xmlhttp.onreadystatechange = function() {
		if( xmlhttp.readyState == 4 ){
			st_state = xmlhttp.responseText;
			document.getElementById( "count" ).innerHTML = st_state;
		}
	};
	xmlhttp.send( null );
	return false;
}

function replace_on_result(){
	location.replace( st_result_url );
}

var id = setInterval( function(){
	refer_state();
	if( st_state == "end" ){
		st_state = "";
		clearInterval(id);
		replace_on_result();
	} else if( st_state == "waiting" ){
		st_state = "";
		clearInterval(id);
	}
}, 500 );
