function Easter_reload( url_src, bbs_id_name )
{
	var scrly = document.documentElement.scrollTop || document.body.scrollTop;
	var resn = document.getElementsByName( bbs_id_name ).item(0).value;
	document.cookie="scrl="+resn+"."+scrly+"; max-age=60;";
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
			document.getElementById("sd"+sno).innerHTML="‚»‚¤‚¾‚Ëx"+i;
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
	for( i=0; i<size; ++i ){
		if( stmt_ary[ i ].indexOf( "=" ) > -1 ){
			name_ary[ i ]  = stmt_ary[ i ].substring( 0, stmt_ary[ i ].indexOf("=") ).trim();
			//val_ary[ i ] = stmt_ary[ i ].substring( stmt_ary[ i ].indexOf("=")+1, stmt_ary[ i ].length ).trim();
		}
	}
	return name_ary;
}

function clearAllCookie()
{
	var name_ary = getDocCookieNameArray();
	var size     = name_ary.length;
	for( i=0; i<size; ++i ){
		document.cookie = name_ary[ i ] + "=; max-age=0;";
	}
}

function getCookie(key,tmp1,tmp2,xx1,xx2,xx3)
{
	tmp1=" "+document.cookie+";";
	xx1=xx2=0;
	len=tmp1.length;
	while(xx1<len){
		xx2=tmp1.indexOf(";",xx1);
		tmp2=tmp1.substring(xx1+1,xx2);
		xx3=tmp2.indexOf("=");
		if(tmp2.substring(0,xx3)==key){
			return(unescape(tmp2.substring(xx3+1,xx2-xx1-1)));
		}
		xx1=xx2+1;
	}
	return("");
}

function Easter_scroll( bbs_id_name ){
	var scrly=getCookie("scrl").split(".");
	if(scrly[1]!=null &&  scrly[1]>0 && document.getElementsByName( bbs_id_name ).item(0).value == scrly[0]){
		window.scroll(0,scrly[1]);
	}
	//document.cookie="scrl=; max-age=0;";
	clearAllCookie();
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
	document.getElementById("ddbut").innerHTML=dispdel?"‰B‚·":"Œ©‚é";
	ddrefl();
	reszk();
}

function reszt(){
	var ofm=document.getElementById("ftbl");
	var oufm=document.getElementById("ufm");
	if(oufm==null||ofm==null){return;}
	ofm.style.position="static";
	oufm.style.lineHeight="0px";
	oufm.innerHTML="";
	ofm.style.marginLeft="auto";
	ofm.style.visibility="visible";
}

function reszu(){
	var ofm=document.getElementById("ftbl");
	var oufm=document.getElementById("ufm");
	if(oufm==null||ofm==null){return;}
	oufm.style.lineHeight=ofm.offsetHeight+"px";
	oufm.innerHTML="&nbsp;";
	ofm.style.position="absolute";
	ofm.style.left="50%";
	ofm.style.marginLeft="-"+(ofm.offsetWidth/2)+"px";
	ofm.style.top=(document.body.offsetTop+oufm.offsetTop)+"px";
	ofm.style.visibility="visible";
}

function reszk(){
	if(document.getElementById("ftbl")==null){
		tmpobj=document.getElementById("reszb");
		if(tmpobj!=null){
			tmpobj.innerHTML="";
		}
		tmpobj=document.getElementById("contres");
		if(tmpobj!=null){
			tmpobj.innerHTML="";
		}
		return;
	}
	var resztmp=getCookie("reszc");
	if(resztmp!=""&&resztmp!=null){
		reszflg=resztmp;
		if(reszflg==1){reszu();}else{reszt();}
	}
}

var easter_autolink_target_list = document.getElementsByClassName("MstyAutoLinkTarget");
for( var i=0; i<easter_autolink_target_list.length; ++i ){
	easter_autolink_target_list[ i ].innerHTML = "http://" + location.host + easter_autolink_target_list[ i ].innerHTML;
}
