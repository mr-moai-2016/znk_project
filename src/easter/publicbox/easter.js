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


/*****************************************************************************/


/***
 * overwrite base4*.js
 */
function unescapeHTML(str) {//unescape comment string
	var div = document.createElement("div");
	div.innerHTML = str
		.replace( /</g,  "&lt;"  )
		.replace( />/g,  "&gt;"  )
		.replace( /\r/g, "&#13;" )
		.replace( /\n/g, "&#10;" );
	return div.textContent || div.innerText;
}
function getNumFromTD(el){
	var ary;
	var idx;

	/***
	 * 旧仕様を先にシークしなければならない.
	 *
	 * 「スレあきblk」は、それ以降のレスblkの集合すべてを含む.
	 * つまりそれ以降のレスblk内にあるinput or spanタグもすべて含むため、
	 * これらもすべてシークされる.
	 *
	 * このときもし先にspanをシークすると、EasterQTabがオープンされている場合に問題がおこる.
	 * この場合スレあきblk内のinputをスルーして、EasterQTab内のspanにマッチし、
	 * それが先に補足されてしまう.
	 */

	/* 旧仕様 : img etc */
	ary = el.getElementsByTagName("input");
	for( idx=0; idx<ary.length; ++idx ){
		var ret = ary[idx].id.match( /delcheck([0-9]+)/ );
		if( ret ){
			return parseInt(ret[1],10);
		}
	}

	/* 新仕様 : may etc */
	ary = el.getElementsByTagName("span");
	for( idx=0; idx<ary.length; ++idx ){
		if( ary[idx].className=="cno" ){
			return parseInt(ary[idx].innerHTML.replace( /No./,"" ),10);
		}
	}
	return 0;
}

function getAncestorElementWithClass(ele,className){
	while(ele && ele!=document){
		if(ele.className==className){
			return ele;
		}
		ele=ele.parentNode;
	}
	return document;
}

function searchQtSrc(str,thisnum){//search quote source
	var no,el;
	if(str.length<1){
		//console.log( "searchQtSrc: str_length<1" );
		return 0;
	}
	var threEle=getAncestorElementWithClass(document.getElementById("delcheck"+thisnum),"thre");
	if(/^(No)?\.?[0-9]+$/.test(str)){
		no=str.replace(/^(No)?\.?/,"");
		if(document.getElementById("delcheck"+no)){
			return no;
		}
	}
	fnReg=/^[0-9]+\.(jpg|png|gif|webm|mp4|webp)$/;
	if(fnReg.test(str)){
		no=str.replace(/^&gt;/,"");
		el=document.querySelectorAll(".thre a")
		for(i=0;i<el.length;i++){
			if(el[i].innerHTML==no){
				ret=getNumFromTD(el[i].parentNode);
				if(ret>0 && ret<thisnum){return ret;}
			}
		}
	}
	el=threEle.getElementsByTagName("blockquote");
	var removetag=/<("[^"]*"|'[^']*'|[^'">])*>/g;
	str=unescapeHTML(str.replace(removetag,''));
	//if(str==defCom){return 0;}
	str=str.replace(/[\\^$.*+?()[\]{}|]/g, '\\$&');//reg escape
	var i,j,ret,reg,regs=['^'+str+'$',''+str+''];
	for(j=0;j<regs.length;j++){
		reg=new RegExp(regs[j],'gm');
		//console.log( "reg=[" + reg + "]" );
		for(i=0;i<el.length;i++){
			var target=unescapeHTML(el[i].innerHTML.replace(/<br>/g,"\n").replace(removetag,''));
			//console.log( "target=[" + target + "]" );
			//if(defCom==target){continue;}
			if(target.match(reg)){
				ret=getNumFromTD(el[i].parentNode);
				//console.log( "ret=[" + ret + "]" );
				if(ret>0 && ret<thisnum){return ret;}
			}
		}
	}
	if(str.length<5){
		//console.log( "str_length<5 str=[" + str + "]" );
		return 0;
	}
	var minSimi=9999;
	var minSimiEle={};
	for(i=0;i<el.length;i++){
		var com=el[i].innerHTML;
		var lines=com.split(/<br>/i);
		for(j=0;j<lines.length;j++){
			var line=lines[j];
			line=line.replace(removetag,'');
			var simi=levenshtein(line,str);
//			console.log(simi+":"+line)
			if(simi!=0 && simi<minSimi){
				minSimi=simi;
				minSimiEle=el[i];
			}
		}
	}
	if(minSimi<10){
		ret=getNumFromTD(minSimiEle.parentNode);
//console.log("ret:"+ret);
		if(ret>0 && ret<thisnum){return ret;}
	}
	//console.log( "minSimi:" + minSimi );
	return 0;
}
function qsd(e){//send sodane for qtpop
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
	var sno=e.target.getAttribute("data-no");
	xmlhttp.open("GET", "/sd.php?"+b+"."+sno);
	xmlhttp.onreadystatechange = function() {
		if (xmlhttp.readyState == 4 ) {
			var i=parseInt(xmlhttp.responseText,10);
			if(isNaN(i)){i=0;}
//			document.getElementById("sd"+sno).innerHTML="そうだねx"+i;
			document.getElementById("sd"+sno).innerHTML="そうだねx"+i;
			e.target.innerHTML="そうだねx"+i;
		}
	};
	xmlhttp.send(null);
	return false;
}

/********************* miscellaneous *********************/
function getScroll(){//scroll代替
	var supportPageOffset = window.pageXOffset !== undefined;
	var isCSS1Compat = ((document.compatMode || "") === "CSS1Compat");
	var x = supportPageOffset ? window.pageXOffset : isCSS1Compat ? document.documentElement.scrollLeft : document.body.scrollLeft;
	var y = supportPageOffset ? window.pageYOffset : isCSS1Compat ? document.documentElement.scrollTop : document.body.scrollTop;
	return {x: x, y: y};
}

function getScrollPosition(elm){//getBoundingClientRect + scroll
	var xy=getPosition(elm);
	var scroll=getScroll();
	return {x: scroll.x+xy.x, y: scroll.y+xy.y};
}

function getPosition(elm) {//getBoundingClientRect代替
	if(typeof elm.getBoundingClientRect == 'function'){
		return {x: elm.getBoundingClientRect().left,
						y: elm.getBoundingClientRect().top};
	}else{
		var xPos = 0, yPos = 0;
		while(elm) {
			xPos += (elm.offsetLeft + elm.clientLeft);
			yPos += (elm.offsetTop  + elm.clientTop);
			elm = elm.offsetParent;
		}
		var isCSS1Compat = ((document.compatMode || "") === "CSS1Compat");
		xPos -= (isCSS1Compat ? document.documentElement.scrollLeft : document.body.scrollLeft);
		yPos -= (isCSS1Compat ? document.documentElement.scrollTop : document.body.scrollTop);
		return { x: xPos, y: yPos };
	}
}

function fragmentFromString(strHTML) {//html文字列から断片を生成する
	var frag = document.createDocumentFragment(),tmp = document.createElement('body'), child;
	tmp.innerHTML = strHTML;
	while (child = tmp.firstChild) {
		frag.appendChild(child);
	}
	return frag;
}

function levenshtein(s1, s2) {//Levenshtein distance
	// http://kevin.vanzonneveld.net
	// +            original by: Carlos R. L. Rodrigues (http://www.jsfromhell.com)
	// +            bugfixed by: Onno Marsman
	// +             revised by: Andrea Giammarchi (http://webreflection.blogspot.com)
	// + reimplemented by: Brett Zamir (http://brett-zamir.me)
	// + reimplemented by: Alexander M Beedie
	// *                example 1: levenshtein('Kevin van Zonneveld', 'Kevin van Sommeveld');
	// *                returns 1: 3
	if (s1 == s2) {
		return 0;
	}
	var s1_len = s1.length;
	var s2_len = s2.length;
	if (s1_len === 0) {
		return s2_len;
	}
	if (s2_len === 0) {
		return s1_len;
	}  // BEGIN STATIC
	var split = false;
	try {
		split = !('0') [0];
	} catch (e) {
		split = true; // Earlier IE may not support access by string index
	}  // END STATIC
	if (split) {
		s1 = s1.split('');
		s2 = s2.split('');
	}
	var v0 = new Array(s1_len + 1);
	var v1 = new Array(s1_len + 1);
	var s1_idx = 0,
	s2_idx = 0,
	cost = 0;
	for (s1_idx = 0; s1_idx < s1_len + 1; s1_idx++) {
		v0[s1_idx] = s1_idx;
	}
	var char_s1 = '',
	char_s2 = '';
	for (s2_idx = 1; s2_idx <= s2_len; s2_idx++) {
		v1[0] = s2_idx;
		char_s2 = s2[s2_idx - 1];
		for (s1_idx = 0; s1_idx < s1_len; s1_idx++) {
			char_s1 = s1[s1_idx];
			cost = (char_s1 == char_s2) ? 0 : 1;
			var m_min = v0[s1_idx + 1] + 1;
			var b = v1[s1_idx] + 1;
			var c = v0[s1_idx] + cost;
			if (b < m_min) {
				m_min = b;
			}
			if (c < m_min) {
				m_min = c;
			}
			v1[s1_idx + 1] = m_min;
		}
		var v_tmp = v0;
		v0 = v1;
		v1 = v_tmp;
	}
	return v0[s1_len];
}

/********************* EasterQTab event handlers *********************/

var the_blk_ary=[];//array for quote popup
var the_current_idx = -1;
var the_frame = null;
var the_frame_position = "bottom";
var the_selected_str;

function copyStrToForm( str )
{
	var str_mdf;
	var ftx=document.getElementById("ftxa");
	if( ftx ){
		str_mdf = str.replace( /<br *\/?>(.)/gi, '\n>$1' );
		str_mdf = str_mdf.replace( /<("[^"]*"|'[^']*'|[^'">])*>/g, '' );
		str_mdf = str_mdf.replace( /\r\n/g, "\n" );
		str_mdf = str_mdf.replace( /\r/g,   "\n" );
		str_mdf = str_mdf.replace( /\n\z/g, "" );
		str_mdf = str_mdf.replace( /\n/g,   "\n>" );
		str_mdf = '>'+ str_mdf + "\n";
	}
	//ftx.value += unescapeHTML( '>'+str_mdf );
	ftx.value += unescapeHTML( str_mdf );
	var ftbl=document.getElementById("ftbl");
	if( ftbl ){
		var winh = window.innerHeight || document.documentElement.clientHeight || document.body.clientHeight;
		window.scroll(0,ftbl.offsetTop - winh/2);
		var ftxa=document.getElementById("ftxa");
		if(ftxa){
			ftxa.focus();
		}
	}
	//var pdm=e.target.parentNode;
	//pdm.parentNode.removeChild(pdm);
}

function EasterQTab_copySelectedToForm(p,e,noflag)
{
	copyStrToForm( the_selected_str );
}

function rewriteFrame( blk )
{
	the_frame.innerHTML = "";
	if( the_frame_position == "bottom" ){
		the_frame.innerHTML += blk.elem_.innerHTML;
		the_frame.innerHTML += "<br>";
		the_frame.innerHTML += createPopupPagination( the_blk_ary.length );
		the_frame.innerHTML += blk.ctrl_.innerHTML;
	} else {
		the_frame.innerHTML += createPopupPagination( the_blk_ary.length );
		the_frame.innerHTML += blk.ctrl_.innerHTML;
		the_frame.innerHTML += "<br>";
		the_frame.innerHTML += blk.elem_.innerHTML;
	}
}
function activateOne( query_idx )
{
	var idx;
	var blk;
	for( idx=0; idx<the_blk_ary.length; ++idx ){
		blk = the_blk_ary[ idx ];
		if( idx == query_idx ){
			the_current_idx = idx;
			rewriteFrame( blk );
			break;
		}
	}
}
function activateSrcNo( src_no )
{
	var idx;
	var blk;
	for( idx=0; idx<the_blk_ary.length; ++idx ){
		blk = the_blk_ary[ idx ];
		if( blk.src_no_ == src_no ){
			the_current_idx = idx;
			rewriteFrame( blk );
			break;
		}
	}
}
function createPopupPagination( num )
{
	var ans = "";
	if( num ){
		var one_side  = 2;
		var both_size = one_side * 2 + 1;
		var idx;
		var prev_idx  = the_current_idx >  0     ? the_current_idx - 1 : 0;
		var next_idx  = the_current_idx <  num-1 ? the_current_idx + 1 : num-1;
		/*
		var begin_idx = the_current_idx >= one_side     ? the_current_idx - one_side : 0;
		var end_idx   = begin_idx + both_size  <= num   ? begin_idx + both_size      : num;
		*/
		var begin_idx = Math.floor( the_current_idx / both_size ) * both_size;
		var end_idx   = begin_idx + both_size <= num ? begin_idx + both_size : num;


		//ans += "<div class='pagination'>";
		ans += "<div>";
		ans += "<a class='MstyElemLink' onclick='activateOne(" + prev_idx + ")'>Prev</a>";
		ans += "<a class='MstyElemLink' onclick='activateOne(" + next_idx + ")'>Next</a>";
		//ans += "<a class='pagination-previous' onclick='activateOne(" + prev_idx + ")' accesskey=z>Prev</a>";
		//ans += "<div class='pagination-list'>";
		for( idx=begin_idx; idx<end_idx; ++idx ){
			if( idx == the_current_idx ){
				ans += "<span class='pagination-link is-current'>" + idx + "</span>";
			} else {
				ans += "<a class='pagination-link' onclick='activateOne(" + idx + ")' accesskey=" + idx + ">" + idx + "</a>";
			}
		}
		//ans += "</div>";
		//ans += "<a class='pagination-next' onclick='activateOne(" + next_idx + ")' accesskey=x>Next</a>";
		ans += "</div>";
	}
	return ans;
}
function moveFrame( position )
{
	the_frame_position = position;
	the_frame.style.maxWidth = "90%";
	the_frame.style.zIndex   = "100";

	if( position == "bottom" ){
		the_frame.style.top    = "auto";
		the_frame.style.bottom = "20px"; /* 下方には他になにかある可能性が高いので若干離す */
		//the_frame.style.left   = "10px";
		//the_frame.style.right  = "10px";
		the_frame.style.position = "fixed";
	} else if( position == "top" ){
		the_frame.style.top    = "10px";
		the_frame.style.bottom = "auto";
		//the_frame.style.left   = "10px";
		//the_frame.style.right  = "10px";
		the_frame.style.position = "fixed";
	}
	activateOne( the_current_idx );
}
function Blk_compose( src_no )
{
	var blk = new Object();

	var elem_fg = document.createDocumentFragment();
	var ctrl_fg = document.createDocumentFragment();

	var parent_node = document.getElementById("delcheck"+src_no).parentNode;
	var jmp = document.createElement('span');
	jmp.innerHTML="<a href='javascript:void(0);' onclick='EasterQTab_jumpToSrcNo(" + src_no + ");'>Jump</a>";
	jmp.className="MstyElemLink";
	ctrl_fg.appendChild(jmp);

	/* 現時点ではスマホ上で問題がある.
	var cpsel_btn = document.createElement('span');
	cpsel_btn.innerHTML="<a href='javascript:void(0);' onclick='EasterQTab_copySelectedToForm();'>Quote</a>";
	cpsel_btn.className="MstyElemLink";
	ctrl_fg.appendChild(cpsel_btn);
	*/

	ctrl_fg.appendChild( fragmentFromString(" &nbsp; ") );

	var move_btn = document.createElement('span');
	move_btn.innerHTML="<a href='javascript:void(0);' onclick='EasterQTab_switchFrame();'>Move</a>";
	move_btn.className="MstyElemLink";
	ctrl_fg.appendChild(move_btn);

	var hide_btn = document.createElement('span');
	hide_btn.innerHTML="<a href='javascript:void(0);' onclick='EasterQTab_hideFrame();'>Hide</a>";
	hide_btn.className="MstyElemLink";
	ctrl_fg.appendChild(hide_btn);

	var cls_btn = document.createElement('span');
	var delete_btn = "<button class='delete' onclick='EasterQTab_unregistAll();'></button>";
	cls_btn.innerHTML="<a href='javascript:void(0);' onclick='EasterQTab_unregistAll();'>" + delete_btn + " Close</a>";
	cls_btn.className="MstyElemLink";
	ctrl_fg.appendChild(cls_btn);

	
	/* make clone of sub */
	var csb = null;
	do{ 
		var orgs = null;
		orgs = parent_node.getElementsByClassName( "csb" );
		if( orgs && orgs[0] ){ csb = orgs[0].cloneNode(true); break; }

		if( csb == null ){
			var str  = parent_node.innerHTML;
			//console.log( "str[" + str + "]" );
			var pats = str.match( /(<font color=['|"]#cc1105['|"]>.*<\/font>)/ );
			if( pats && pats[1] ){
				console.log( "pat=[" + pats[1] + "]" );
				csb = document.createElement('span');
				csb.className = "csb";
				csb.innerHTML = pats[1] + " ";
			}
		}
	} while( false );

	/* make clone of name */
	var cnm = null;
	do{ 
		var orgs = null;
		orgs = parent_node.getElementsByClassName( "cnm" );
		if( orgs && orgs[0] ){ cnm = orgs[0].cloneNode(true); break; }

		if( cnm == null ){
			var str  = parent_node.innerHTML;
			//console.log( "str[" + str + "]" );
			var pats = str.match( /(Name <font color=.*<\/font>)/ );
			if( pats && pats[1] ){
				cnm = document.createElement('span');
				cnm.className = "cnm";
				cnm.innerHTML = pats[1].replace( /Name/, "" );
			}
		}
	} while( false );

	/* make clone of date, resno, and com */
	var cnw = null;
	var cno = null;
	do{ 
		var orgs = null;

		orgs = parent_node.getElementsByClassName( "cnw" );
		if( orgs && orgs[0] ){ cnw = orgs[0].cloneNode(true); }

		orgs = parent_node.getElementsByClassName( "cno" );
		if( orgs && orgs[0] ){ cno = orgs[0].cloneNode(true); }

		if( cnw == null ){
			var str  = parent_node.innerHTML;
			var pats = str.match( /([0-9]+\/[0-1][0-9]\/[0-3][0-9].*[0-9][0-9]:[0-9][0-9]:[0-9][0-9])/ );
			if( pats && pats[1] ){
				cnw = document.createElement('span');
				cnw.className = "cnw";
				cnw.innerHTML = pats[1] + " ";
			}
			var pats = str.match( /(No.[0-9]+)/ );
			if( pats && pats[1] ){
				cno = document.createElement('span');
				cno.className = "cno";
				cno.innerHTML = pats[1];
			}
		}
	} while( false );

	var com=parent_node.getElementsByTagName("blockquote")[0].cloneNode(true);
	var marl = parseInt( com.style.marginLeft, 10 );
	if( marl>0 ){
		com.style.marginLeft=(marl/2)+"px";
	};

	/* make clone of sd */
	var sd = document.createElement('span');
	sd.className="qsd";
	sd.addEventListener("click",qsd,false);
	sd.setAttribute('data-no', src_no);
	var org_sd = parent_node.getElementsByClassName("sod")[0];
	if( org_sd ){
		sd.innerHTML = org_sd.innerHTML;
	}

	/* make clone of imgele */
	var imgele=parent_node.getElementsByTagName("img")[0];
	if(imgele){
		var w=parseInt(imgele.getAttribute("width"),10)/2;
		var h=parseInt(imgele.getAttribute("height"),10)/2;
		var linkele = imgele.parentNode;
		var linkimg = linkele.cloneNode(false);
		var cimg    = imgele.cloneNode(false);
		cimg.style.width  = w+"px";
		cimg.style.height = h+"px";
		linkimg.appendChild(cimg);

		var filename = linkele.getAttribute("href").replace(/^.*\//,"");
		var fsize    = imgele.getAttribute("alt");
		var linkfn   = linkele.cloneNode(false);
		linkfn.innerHTML = filename;
		var d2 = document.createTextNode("-("+fsize+")");
	}

	/* append child parts to elem */
	if(csb){
		elem_fg.appendChild(csb);
	}
	if(cnm){
		var label_name = document.createTextNode("Name");
		elem_fg.appendChild(label_name);
		elem_fg.appendChild(cnm);
	}
	if(cnw){
		elem_fg.appendChild(cnw);
	}
	if(cno){
		elem_fg.appendChild(cno);
	}
	elem_fg.appendChild(sd);
	if(imgele){
		elem_fg.appendChild(document.createElement('br'));
		elem_fg.appendChild(fragmentFromString(" &nbsp; &nbsp; "));
		elem_fg.appendChild(linkfn);//link+filename
		elem_fg.appendChild(d2);//-(12300 B)
		elem_fg.appendChild(document.createElement('br'));
		elem_fg.appendChild(linkimg);//link+img
	}
	elem_fg.appendChild(com);

	/* rendering */
	var elem = document.createElement('div');
	var ctrl = document.createElement('div');
	elem.appendChild( elem_fg );
	ctrl.appendChild( ctrl_fg );

	blk.src_no_ = src_no;
	blk.ctrl_   = ctrl;
	blk.elem_   = elem;

	return blk;
}
function Blk_dispose( blk )
{
	var elem = blk.elem_;
	blk.ctrl_ = null;
	blk.elem_ = null;
}

function EasterQTab_jumpToSrcNo( src_no )
{
	var parent_node = document.getElementById("delcheck"+src_no).parentNode;
	if(!parent_node.offsetHeight){
		dispdel=1;
		document.getElementById("ddbut").innerHTML=dispdel?"隠す":"見る";
		ddrefl();
		reszk();
	}
	window.scroll(0,getScrollPosition(parent_node).y);
}
function EasterQTab_switchFrame()
{
	if( the_frame_position == "bottom" ){
		moveFrame( "top" );
	} else {
		moveFrame( "bottom" );
	}
}
function EasterQTab_hideFrame()
{
	the_frame.innerHTML = "<a class=MstyElemLink href='javascript:void(0);' onclick='activateOne(" + the_current_idx + ");'>Show</a>";
}
function EasterQTab_registSrcNo( src_no )
{
	var blk = null;
	var idx;
	for( idx=0; idx<the_blk_ary.length; ++idx ){
		blk = the_blk_ary[ idx ];
		if( blk != null ){
			if( blk.src_no_ === src_no ){
				/* already exist */
				return blk;
			}
		}
	}
	/* not found : register newly */
	blk = Blk_compose( src_no );
	the_blk_ary.push( blk );
	return blk;
}
function EasterQTab_unregistIdx( idx )
{
	var blk = the_blk_ary[ idx ];
	if( blk != null ){
		Blk_dispose( blk );
		the_blk_ary.splice( idx, 1 );
		return true;
	}
	return false;
}
function EasterQTab_unregistAll()
{
	var idx;
	for( idx=0; idx<the_blk_ary.length; ++idx ){
		var blk = the_blk_ary[ idx ];
		if( blk != null ){
			Blk_dispose( blk );
		}
	}
	the_blk_ary = [];

	/* frameの非表示 */
	if( the_frame != null ){
		the_frame.style.display = "none";
	}
	return true;
}
function EasterQTab_close(e)
{
	EasterQTab_unregistAll();
}
function EasterQTab_open( tgt, this_src_no )
{
	var reg = /^&gt;/;
	var src_no      = 0;

	if( reg.test(tgt.innerHTML) ){
		var searchstr = tgt.innerHTML.replace(reg,"");
		src_no        = searchQtSrc( searchstr, this_src_no );
		if( src_no == 0 || document.getElementsByClassName('thre').length>1 ){
			return;
		}
	} else {
		src_no = this_src_no;
	}

	if( the_frame == null ){
		the_frame = document.createElement('div');
		//the_frame.className = "qtd";
		//the_frame.className = "notification";
		the_frame.className = "MstyCtrlPopup";
	
		moveFrame( "bottom" );

		var thre = getAncestorElementWithClass(tgt,"thre");
		thre.appendChild( the_frame );
	}

	EasterQTab_registSrcNo( src_no );
	activateSrcNo( src_no );
	the_frame.style.display  = "block";
}
function EasterQTab_popup(e){
	var this_src_no = 0;
	var tgt,timeout;
	var latency = 300;
	if( "touches" in e && e.touches.length>0 ){
		e = e.touches[0];
	}
	if( (tgt = e.target) == document ){
		return;
	}

	if( tgt.nodeName.toLowerCase() == 'span' && tgt.id.match( /delcheck([0-9]+)/ ) ){
		this_src_no = getNumFromTD( tgt.parentNode );
		latency = 0;
	} else if( tgt.parentNode && tgt.parentNode.nodeName.toLowerCase() == 'blockquote' ) {
		this_src_no = getNumFromTD( tgt.parentNode.parentNode );
		/***
		 * dragに干渉しにくくするため、少し遅延させた方がよい.
		 */
		latency = 300;
	} else if( tgt.nodeName.toLowerCase() == 'a' && tgt.className == "MstyElemLink" ){
		/***
		 * 旧仕様
		 */
		this_src_no = getNumFromTD( tgt.parentNode );
		console.log( "this_src_no:" + this_src_no );
		latency = 0;
	}

	if( this_src_no != 0 ){
		timeout=setTimeout(function(){EasterQTab_open(tgt,this_src_no);},latency);
		tgt.addEventListener('mouseleave', function(){clearTimeout(timeout)}, false);
	}
}
function EasterQTab_onMouseUp(e)
{
	if( window.getSelection ){
		the_selected_str = window.getSelection().toString();
		if( the_selected_str !== '' && the_selected_str !== '\n' ){
			console.log(the_selected_str);
		}
	}
}

/********************* on load *********************/

function getCookie(key,tmp1,tmp2,xx1,xx2,xx3)
{
	var ans = "";
	tmp1=" "+document.cookie+";";
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
	return ans;
}

if(document.addEventListener){//add EventListener for popup
	if( typeof qtpopup == "function" ){
		/* futaba default */
		document.removeEventListener('mouseover', qtpopup);
	}
	document.addEventListener(
			navigator.userAgent.match(/iP(hone|od|ad)/i) ? 'touchstart' : 'click',
			EasterQTab_popup, false);
	if( typeof closeqtpop == "function" ){
		/* futaba default */
		document.addEventListener('blur', closeqtpop, false);
	}
	document.addEventListener('mouseup', EasterQTab_onMouseUp, false);
}



