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
		/* Xhrは変換元をUTF8と仮定して、ページで設定された文字コードへと自動的に変換を行う.
		 * その変換元を明示的に指定するのが overrideMimeType であるが、これはIE9以前ではサポートされていない.
		 * よってこれに頼らず、state.dat 自体をUTF8で出力すべきである. */
		//xmlhttp.overrideMimeType( 'text/plain; charset=Shift_JIS' );
	}
	xmlhttp.open( "GET", st_dat_path + '?date=' + new Date().getTime() );
	xmlhttp.onreadystatechange = function() {
		//if( xmlhttp.readyState == 4 ){
		if( xmlhttp.readyState == 4 && xmlhttp.status == 200 ){
			var ary = xmlhttp.responseText.split( ' ' );
			if( ary.length >= 2 ){
				count = parseInt( ary[ 0 ], 10 );
				ary.shift();
				var msg = ary.join( ' ' );
				if( isNaN(count) ){ count=100; }
				document.getElementById( "progress_state_msg" ).innerHTML = msg;
			} else {
				count = parseInt( xmlhttp.responseText, 10 );
				if( isNaN(count) ){ count=100; }
				document.getElementById( "progress_state_msg" ).innerHTML = "[" + count + "] %";
			}
		} else {
			document.getElementById( "progress_state_msg" ).innerHTML = "[" + count + "]";
			++count;
		}
	};
	xmlhttp.send( null );
	//++count;
	return false;
}
var id = setInterval( function(){
	refer_state();
	if(count >= 100){
		clearInterval(id);
		}}, 1000 );
