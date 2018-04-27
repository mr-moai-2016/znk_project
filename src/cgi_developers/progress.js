var count = 0;
var st_dat_path = "";
function set_dat_path( dat_path ){
	st_dat_path = dat_path;
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
			var i = parseInt( xmlhttp.responseText, 10 );
			if( isNaN(i) ){ i=0; }
			document.getElementById( "count" ).innerHTML = "[" + i + "] %";
		}
	};
	xmlhttp.send( null );
	++count;
	return false;
}
var id = setInterval( function(){
	refer_state();
	if(count > 50){
		clearInterval(id);
		}}, 1000 );
