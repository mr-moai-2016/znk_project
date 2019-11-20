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

function Easter_del( d )
{
	/* self.location.href="/del.php?b="+b+"&d="+d; */
	/* & : %26 */
	self.location.href="/easter?est_get=" + Easter_hostname + "/del.php?b="+b+"%26d="+d;
}

function Easter_sd( sno )
{
	var xmlhttp = false;
	if(typeof ActiveXObject != "undefined"){
		try {
			xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
		} catch (e) {
			xmlhttp = false;
		}
	}
	if(!xmlhttp && typeof XMLHttpRequest != "undefined") {
		xmlhttp = new XMLHttpRequest();
	}
	//xmlhttp.open("GET", "/sd.php?"+b+"."+sno);
	xmlhttp.open("GET", "/easter?est_get=" + Easter_hostname + "/sd.php?"+b+"."+sno);
	xmlhttp.onreadystatechange = function() {
		if (xmlhttp.readyState == 4 ) {
			var i=parseInt(xmlhttp.responseText,10);
			if(isNaN(i)){i=0;}
			document.getElementById("sd"+sno).innerHTML="そうだねx"+i;
		}
	};
	xmlhttp.send(null);
	return false;
}

function getDocCookieNameArray()
{
	var cookie = document.cookie;
	if( cookie == "" ){
		return [];
	}
	var stmt_ary = cookie.split(";");
	var size     = stmt_ary.length;
	var name_ary = new Array( size-1 );

	/* for unsupported trim() */
	if (!String.prototype.trim) {
		String.prototype.trim = function () {
			return this.replace(/^[\s\uFEFF\xA0]+|[\s\uFEFF\xA0]+$/g, '');
		};
	}
	for( i=0; i<size; ++i ){
		if( stmt_ary[ i ].indexOf( "=" ) > -1 ){
			var tmp = stmt_ary[ i ].substring( 0, stmt_ary[ i ].indexOf("=") );
			name_ary[ i ]  = tmp.trim();
		}
	}
	return name_ary;
}

function findAryElem( ary, query_elem )
{
	var size = ary.length;
	for( i=0; i<size; ++i ){
		if( ary[ i ] == query_elem ){
			return true;
		}
	}
	return false;
}

function Easter_clearCookie( query_name_ary, is_alive )
{
	var name_ary = getDocCookieNameArray();
	var size     = name_ary.length;
	for( i=0; i<size; ++i ){
		if( query_name_ary != null ){
			if( is_alive ){
				/* query_name_ary indicate alive list */
				if( findAryElem( query_name_ary, name_ary[ i ] ) ){
					continue;
				}
			} else {
				/* query_name_ary indicate death list */
				if( !findAryElem( query_name_ary, name_ary[ i ] ) ){
					continue;
				}
			}
		}
		document.cookie = name_ary[ i ] + "=; max-age=0;";
	}
}

function getCookie(key,tmp1,tmp2,xx1,xx2,xx3)
{
	ans = "";
	tmp1=" "+document.cookie+";";
	//alert( "getCookie key=[ " + key + "] tmp1=[" + tmp1 + "]" );
	xx1=xx2=0;
	len=tmp1.length;
	while(xx1<len){
		xx2=tmp1.indexOf(";",xx1);
		tmp2=tmp1.substring(xx1+1,xx2);
		xx3=tmp2.indexOf("=");
		if(tmp2.substring(0,xx3)==key){
			ans = unescape(tmp2.substring(xx3+1,xx2-xx1-1));
			break;
		}
		xx1=xx2+1;
	}
	tmp_ck= document.cookie;
	//alert( "getCookie ans=[" + ans + "]" );
	return ans;
}

function Easter_scroll( bbs_id_name ){
	var scrly=getCookie("scrl").split(".");
	if(scrly[1]!=null &&  scrly[1]>0 && document.getElementsByName( bbs_id_name ).item(0).value == scrly[0]){
		window.scroll(0,scrly[1]);
	}
	//document.cookie="scrl=; max-age=0;";
	{
		var except_name_ary = [ 'reszc' ];
		Easter_clearCookie( except_name_ary, true );
	}
}

var dispdel = 0;

function ddrefl(){
  var c=0;
  var ddtags=document.getElementsByTagName("table");
  for(var i in ddtags){
    if(ddtags[i].className=="deleted"){
      ddtags[i].style.display=dispdel?(/*@cc_on!@*/true?"table":"block"):"none";c++;
    }
  }
}

function onddbut(){
	dispdel=1-dispdel;
	document.getElementById("ddbut").innerHTML=dispdel?"隠す":"見る";
	ddrefl();
	reszk();
}

function Easter_moveTopOfTbl( tbl, dst ){//display form on top
	if( dst == null || tbl == null ){ return; }
	dst.style.lineHeight = tbl.offsetHeight+"px";
	dst.innerHTML        = "&nbsp;";
	tbl.style.position   = "absolute";
	tbl.style.left       = "50%";
	tbl.style.marginLeft = "-"+(tbl.offsetWidth/2)+"px";
	tbl.style.top        = (document.body.offsetTop+dst.offsetTop)+"px";
	tbl.style.visibility = "visible";
}

function Easter_setCookieOnJS( key, val, req_urp ){
	var ck_draft = key + "=" + val + "; max-age=864000; path=http://" + location.host + req_urp + ";";
	//alert( "ck_draft=[" +ck_draft+ "]" );
	document.cookie = ck_draft;
}

function Easter_switchFormPos(){
	// update reszc(on cookie) and reszflg(on js)
	var reszc=getCookie("reszc");

	if( reszc != "" && reszc != null ){
		reszflg = reszc;
		reszflg = 1-reszflg;
	} else {
		/* fail-safe */
		reszflg = 1-reszflg;
	}

	Easter_setCookieOnJS( "reszc", reszflg, "" );

	// move top of tbl to landmark : "tfm" or "ufm" 
	var tbl = document.getElementById("ftbl");
	var dst = document.getElementById( (reszflg == 1) ? "ufm" : "tfm" );
	Easter_moveTopOfTbl( tbl, dst );
	window.scroll(0,document.getElementById("ftbl").offsetTop);
}

function Easter_noneFunc(){
}

function Easter_onLoad(){
	var reszc=getCookie("reszc");

	if( reszc != "" && reszc != null ){
		reszflg = reszc;
	} else {
		/* fail-safe */
		//alert( "Easter_onLoad reszc is not set." );
		reszflg = 0;
	}

	Easter_setCookieOnJS( "reszc", reszflg, "" );

	// move top of tbl to landmark : "tfm" or "ufm" 
	var tbl = document.getElementById("ftbl");
	var dst = document.getElementById( (reszflg == 1) ? "ufm" : "tfm" );
	Easter_moveTopOfTbl( tbl, dst );
	//window.scroll(0,document.getElementById("ftbl").offsetTop);

}

window.onresize = Easter_onLoad;
window.onload   = Easter_onLoad;

/***
 * overwrite base4*.js
 */

function qtpopup(e){//quote popup
	var t;
	if ((t = e.target) == document) {
		return;
	}
	if(t.parentNode && t.parentNode.nodeName.toLowerCase() == 'blockquote'){
		
	}
}
function thumbonclick(e){//click to open video tag
	var t;
	if ((t = e.target) == document) {
		return;
	}
	if(t.parentNode && t.parentNode.nodeName.toLowerCase() == 'a'){
		if (webmopen(t)) {
			e.preventDefault();
		}
	}

	var c=t;
	var pdm=document.getElementById("pdm");
	if(pdm){
		while(c) {
			if(c == document){break;}
			if(c.id=="pdm"){return;}
			c = c.offsetParent;
		}
		pdm.parentNode.removeChild(pdm);
	}

	if(t.nodeName.toLowerCase() == 'span' && t.className == 'cno'){
		if (pdmopen(t)) {
			e.preventDefault();
		}
	}
}
function qtw(no,e){
	var dcn=document.getElementById("delcheck"+no);
	if(!dcn){return;}
	var p=dcn.parentNode;
	var t=p.getElementsByTagName('blockquote')[0].innerHTML;
	var ftx=document.getElementById("ftxa");
	if(!ftx){return;}
	t=t.replace(/<br *\/?>(.)/gi,'\n>$1');
	t=t.replace(/<("[^"]*"|'[^']*'|[^'">])*>/g,'')+'\n';
	ftx.value+=unescapeHTML('>'+t);
	var ftbl=document.getElementById("ftbl");
	if(ftbl){
		var bodyHeight = window.innerHeight || document.documentElement.clientHeight || document.body.clientHeight;
		window.scroll(0,ftbl.offsetTop - bodyHeight/2);
	}
	var pdm=e.target.parentNode;
	pdm.parentNode.removeChild(pdm);
}
function pdmopen(t){//open pull-down menu
	var no = t.innerHTML.replace("No.","");
	var p = t.parentNode;
	var m = document.createElement('div');
		m.className = "pdmm";
		m.id = "pdm";
	var xy=getPosition(t);
	var supportPageOffset = window.pageXOffset !== undefined;
	var isCSS1Compat = ((document.compatMode || "") === "CSS1Compat");
	var x = supportPageOffset ? window.pageXOffset : isCSS1Compat ? document.documentElement.scrollLeft : document.body.scrollLeft;
	var y = supportPageOffset ? window.pageYOffset : isCSS1Compat ? document.documentElement.scrollTop : document.body.scrollTop;
	m.style.top = (xy.y+y)+"px";
	if(window.screen.width<=799){
		m.style.left = (xy.x+x-150)+"px";
	}else{
		m.style.left = (xy.x+x-100)+"px";
	}
	m.style.position = "absolute";
	var d1=document.createElement('div');
		d1.innerHTML="引用して書込み";
		if(document.getElementsByClassName("thre").length>1){
			d1.className="pdms pdmshide";
		}else{
			d1.addEventListener('click', function(){qtw(no,arguments[0]);}, false);
			d1.className="pdms";
		}
	var d2=document.createElement('div');
		d2.innerHTML="削除依頼(del)";
		d2.addEventListener('click', function(){Easter_del(no);}, false);
		d2.className="pdms";
	var d3=document.createElement('div');
		d3.innerHTML =
			'<form action = "/easter?est_post='+Easter_hostname+'/'+b+'/futaba.php?guid=on" method="POST">削除キー' +
			'<input type="password" name ="pwd" size="8" value="'+getCookie("pwdc")+'"><br>' + 
			'<input type="checkbox" name ="onlyimgdel"   value="on">画像だけ' +
			'<input type="submit"   value="記事削除">' +
			'<input type="hidden"   name ="'+no+'" value="delete" id="delcheck'+no+'">' +
			'<input type="hidden"   name ="mode"   value="usrdel">' +
			'</form>';
		//d3.innerHTML='<form action="/'+b+'/futaba.php?guid=on" method="POST">削除キー<input type="password" name="pwd" size="8" value="'+getCookie("pwdc")+'"><br><input type="checkbox" name="onlyimgdel" value="on">画像だけ<input type="submit" value="記事削除"><input type="hidden" name="'+no+'" value="delete" id="delcheck'+no+'"><input type="hidden" name="mode" value="usrdel"></form>';
		d3.className="pdmf";
	m.appendChild(d1);
	m.appendChild(d2);
	m.appendChild(d3);
	p.appendChild(m);
	return true;
}
if(document.addEventListener){//add EventListener for video tag
	document.addEventListener('click', thumbonclick, false);
}
if(document.addEventListener){//add EventListener for popup
	document.addEventListener('mouseenter', qtpopup, false);
}
