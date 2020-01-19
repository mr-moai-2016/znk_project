/* 0-byte avoidance */

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
	xmlhttp.open("GET", Easter_toURP_forGET( "/sd.php?"+b+"."+sno ) );
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

function replaceRes(threadNo,no) {//get article by json
	var xmlhttp2 = createXMLHttpRequest();
	if(!xmlhttp2){return false;}
	xmlhttp2.onreadystatechange = function () {
		if (xmlhttp2.readyState == 4) {
			if (xmlhttp2.status == 200) {
				var restxt=xmlhttp2.responseText;
				if(restxt.slice(0,1)!="{"){restxt="{}";}
				var ret=JSON.parse(restxt);
				var data = ret["res"];
				for (var no in data){
					var t=makeArticle(ret,no);
					var el=document.getElementById("delcheck"+no);
					while(el && el!=document){
						if(el.tagName.toLowerCase()=="table"){
							break;
						}
						el=el.parentNode;
					}
					if( el && el.parentNode ){
						el.parentNode.replaceChild(t,el);
					}
				}
			}
		}
	};
	xmlhttp2.open("GET", Easter_toURP_forGET( "/"+b+"/futaba.php?mode=json&res="+threadNo+"&start="+no+"&end="+no+"&"+Math.random() ) );
	xmlhttp2.send(null);
	return true;
}

function usrdelsend(e){//user-delete send by pull-down menu
	e.preventDefault();
	var t=e.target;
	var no=t.parentNode.parentNode.getAttribute("data-no");
	console.log( "usrdelsend" + no );
	var data = {"responsemode": "ajax"};
	for(var i=0;i<t.length;i++){
		var value="";
		var ti=t[i];
		switch(ti.type){
			case "checkbox":
				if(ti.checked){
					value="on";
				}
				break;
			default:
				value=ti.value;
		}
		if(ti.name!=""){
			data[ti.name]=value;
		}
	}
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
	xmlhttp.open("POST", Easter_toURP_forPOST( "/"+b+"/futaba.php" ) );
	xmlhttp.onreadystatechange = function() {
		if (xmlhttp.readyState == 4 ) {
			var res=xmlhttp.responseText;
			if(res=="ok"){
				var threno=getAncestorElement(document.getElementById("delcheck"+no),"thre").getAttribute("data-res");
				console.log(threno+" "+no);
				replaceRes(threno,no);
				var pdm=document.getElementById("pdm");
				if(pdm){
					pdm.parentNode.removeChild(pdm);
				}
				console.log(res);
			}else{
				alert(res);
			}
			return false;
		}
	};
	xmlhttp.setRequestHeader( 'Content-Type', 'application/x-www-form-urlencoded' );
	xmlhttp.send( EncodeHTMLForm( data ) );
	return false;
}

function Easter_moveTopOfTbl( tbl, dst )
{
	//display form on top
	if( dst == null || tbl == null ){ return; }
	dst.style.lineHeight = tbl.offsetHeight+"px";
	dst.innerHTML        = "&nbsp;";
	tbl.style.position   = "absolute";
	tbl.style.left       = "50%";
	tbl.style.marginLeft = "-"+(tbl.offsetWidth/2)+"px";
	tbl.style.top        = (document.body.offsetTop+dst.offsetTop)+"px";
	tbl.style.visibility = "visible";
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

	PascuaCookie.setCookieOnJS( "reszc", reszflg, "" );

	// move top of tbl to landmark : "tfm" or "ufm" 
	var tbl = document.getElementById("ftbl");
	var dst = document.getElementById( (reszflg == 1) ? "ufm" : "tfm" );
	Easter_moveTopOfTbl( tbl, dst );
	window.scroll(0,document.getElementById("ftbl").offsetTop);
}


function Easter_onLoad()
{
	var reszc=getCookie("reszc");

	if( reszc != "" && reszc != null ){
		reszflg = reszc;
	} else {
		/* fail-safe */
		reszflg = 0;
	}

	PascuaCookie.setCookieOnJS( "reszc", reszflg, "" );

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
	xmlhttp.open("GET", Easter_toURP_forGET("/sd.php?"+b+"."+sno) );
	xmlhttp.onreadystatechange = function() {
		if (xmlhttp.readyState == 4 ) {
			var i=parseInt(xmlhttp.responseText,10);
			if(isNaN(i)){i=0;}
//			document.getElementById("sd"+sno).innerHTML="‚»‚¤‚¾‚Ëx"+i;
			document.getElementById("sd"+sno).innerHTML="‚»‚¤‚¾‚Ëx"+i;
			e.target.innerHTML="‚»‚¤‚¾‚Ëx"+i;
		}
	};
	xmlhttp.send(null);
	return false;
}

function delsend(e,no,reason){//del-form send by pull-down menu
	var data = { "mode":"post" , "b": b , "d": no ,"reason": reason , "responsemode": "ajax"};
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
	xmlhttp.open("POST", Easter_toURP_forPOST("/del.php") );
	xmlhttp.onreadystatechange = function() {
		if (xmlhttp.readyState == 4 ) {
			var res=xmlhttp.responseText;
			var pdm=document.getElementById("pdm");
			var pdd=document.getElementById("pdd");
//			console.log(res);
			if(res=="ok"){
				var stt = document.createElement('div');
				stt.className = 'pddtip';
				stt.style.position = 'absolute';
				stt.style.left = e.target.offsetLeft +20+ "px";
				stt.style.top = e.target.offsetTop + "px";
				stt.innerHTML = "“o˜^‚µ‚Ü‚µ‚½";
				e.target.offsetParent.appendChild(stt);
				setTimeout(function(){
					if(stt){stt.parentNode.removeChild(stt);}
				},1000);
				setTimeout(function() {
					if(pdm){
						pdm.parentNode.removeChild(pdm);
					}
					if(pdd){
						pdd.parentNode.removeChild(pdd);
					}
				},1000);
			}else{
				alert(res);
			}
		}
	};
	xmlhttp.setRequestHeader( 'Content-Type', 'application/x-www-form-urlencoded' );
	xmlhttp.send( EncodeHTMLForm( data ) );
	return false;
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

function MdfrBase4_getHistory(cname){
	var cathists=getCookie(cname),cathist,cathide;
	if(cathists==""){
		cathist=[];
		cathide=[];
	}else{
		var cattmp=cathists.split('/');
		if(cattmp[0]==""){
			cathist=[];
		}else{
			cathist=cattmp[0].split('-');
		}
		if(cattmp[1]==""){
			cathide=[];
		}else{
			cathide=cattmp[1].split('-');
		}
	}
	return {"cathist":cathist,"cathide":cathide};
}

//add history
function addHistory(){
	var vah,vahf;
	try {
		vah = window.localStorage.viewaddhist;
		vahf = false;
	} catch (e) {
		vahf = true;
	}
	if(vahf||typeof vah==="undefined"){vah="true";}
	if((vah=="true")?true:false){
		var threEl=document.getElementsByClassName("thre");
		if(threEl.length!=1){return;}
		var no=threEl[0].getAttribute("data-res");
		var histhide=MdfrBase4_getHistory("catviews");
		var cathist=histhide.cathist;
		var cathide=histhide.cathide;
		var setc=false;
		if (no>0){
			for(var i=0; i<cathide.length; i++){
				if(cathide[i] == no){
					cathide.splice(i--, 1);
					setc=true;
				}
			}
			if(cathist.indexOf(no) == -1){
				cathist.push(no);
				setc=true;
			}
		}
		if(setc){
			document.cookie="catviews="+(cathist.join('-')+'/'+cathide.join('-'))+"; max-age=8640000; path=/"+b+";";
		}
	}
}

/********************* initialize  *********************/
if(document.addEventListener){//add EventListener for initialize
	if( typeof qtpopup == "function" ){
		/* futaba default */
		document.removeEventListener('mouseover', qtpopup);
	}
	document.addEventListener(
			navigator.userAgent.match(/iP(hone|od|ad)/i) ? 'touchstart' : 'click',
			EQTab.popup, false);
	if( typeof closeqtpop == "function" ){
		/* futaba default */
		document.addEventListener('blur', closeqtpop, false);
	}
	document.addEventListener('mouseup', EQTab.onMouseUp, false);
}

