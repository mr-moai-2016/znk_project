/* 0-byte avoidance */

function getHistory(catmode){
	var cathists;
	if(catmode=="9"){
		cathists=getCookie("cathists");
	}else{
		cathists=getCookie("catviews");
	}
	var cathist,cathide;
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

function addhistory(e,no,hide){//add history , delete history
	var catmode=getParam("sort");
	var histhide=getHistory(catmode);
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
	var mes = "í«â¡ÇµÇ‹ÇµÇΩ";
	if (hide>0 && cathide.indexOf(hide) == -1){
		cathide.push(hide);
		setc=true;
		mes = "çÌèúÇµÇ‹ÇµÇΩ";
	}
	if(setc){
		var cname;
		if(catmode=="9"){
			cname="cathists";
		}else{
			cname="catviews";
		}
		document.cookie=cname+"="+(cathist.join('-')+'/'+cathide.join('-'))+"; max-age=8640000; path=/"+b+";";
	}

	var stt = document.createElement("div");
	stt.className = "pddtipc";
	stt.style.position = "absolute";
	stt.style.left = e.target.offsetLeft +20+ "px";
	stt.style.top = e.target.offsetTop + "px";
	stt.innerHTML = mes;
	e.target.offsetParent.appendChild(stt);
	var pdm=document.getElementById("pdm");
	setTimeout(function(){
		if(stt){stt.parentNode.removeChild(stt);}
		if(pdm){pdm.parentNode.removeChild(pdm);}
		hidetd();
	},1000);
}

function getCookie(key,tmp1,tmp2,xx1,xx2,xx3){//get cookie
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

function pdmopen(e){//open pull-down menu
	var pdm=document.getElementById("pdm");
	if(pdm){
		pdm.parentNode.removeChild(pdm);
	}
	var t = e.target;
	var no = t.getAttribute("data-no");
	var p = t.parentNode;

	var m = document.createElement("div");
		m.className = "pdmmc";
		m.id = "pdm";
	var xy=getScrollPosition(t);
	if(window.screen.width<=799){
		m.style.top = (xy.y)+40+"px";
		m.style.left = (xy.x-60)+"px";
	}else{
		m.style.top = (xy.y)+20+"px";
		m.style.left = (xy.x-10)+"px";
	}
	m.style.position = "absolute";

	var d2=document.createElement("div");
		d2.innerHTML="çÌèúàÀóä(del)";
		d2.addEventListener("click",function(){delsend(arguments[0],no,"110");},false);
		d2.className="pdmsc pdmcd";

	var d4=document.createElement("div");
	if(getParam("sort")=="9"){
		d4.innerHTML="óöóÇ©ÇÁçÌèú";
	}else{
		d4.innerHTML="å©óÇ©ÇÁçÌèú";
	}
		d4.addEventListener("click",function(){addhistory(arguments[0],0,no);},false);
		d4.className="pdmsc pdmcd";

	var d3=document.createElement("div");
		d3.innerHTML="å©óÇ…í«â¡";
		d3.addEventListener("click",function(){addhistory(arguments[0],no,0);},false);
		d3.className="pdmsc";

	m.appendChild(d2);
	if(getParam("sort")=="9"||getParam("sort")=="7"){
		m.appendChild(d4);
	}else{
		m.appendChild(d3);
	}
	m.setAttribute("data-no", no);
	m.setAttribute("data-res", p.getAttribute("data-res"));
	document.body.appendChild(m);
//	console.log("open pdm");
}

function getParam(name, url) {
    if (!url) url = window.location.href;
    name = name.replace(/[\[\]]/g, "\\$&");
    var regex = new RegExp("[?&]" + name + "(=([^&#]*)|&|#|$)"),
        results = regex.exec(url);
    if (!results) return null;
    if (!results[2]) return '';
    return decodeURIComponent(results[2].replace(/\+/g, " "));
}

function getScrollPosition(elm){//getBoundingClientRect + scroll
	var xy=getPosition(elm);
	var scroll=getScroll();
	return {x: scroll.x+xy.x, y: scroll.y+xy.y};
}

function getPosition(elm) {//getBoundingClientRectë„ë÷
	if(typeof elm.getBoundingClientRect == "function"){
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
function getScroll(){//scrollë„ë÷
	var supportPageOffset = window.pageXOffset !== undefined;
	var isCSS1Compat = ((document.compatMode || "") === "CSS1Compat");
	var x = supportPageOffset ? window.pageXOffset : isCSS1Compat ? document.documentElement.scrollLeft : document.body.scrollLeft;
	var y = supportPageOffset ? window.pageYOffset : isCSS1Compat ? document.documentElement.scrollTop : document.body.scrollTop;
	return {x: x, y: y};
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
	//xmlhttp.open("POST", "/del.php");
	xmlhttp.open("POST", "/easter?est_post=" + Easter_hostname + "/del.php");
	xmlhttp.onreadystatechange = function() {
		if (xmlhttp.readyState == 4 ) {
			var res=xmlhttp.responseText;
			var pdm=document.getElementById("pdm");
			var pdd=document.getElementById("pdd");
			if(res=="ok"){
				var stt = document.createElement("div");
				stt.className = "pddtipc";
				stt.style.position = "absolute";
				stt.style.left = e.target.offsetLeft +20+ "px";
				stt.style.top = e.target.offsetTop + "px";
				stt.innerHTML = "ìoò^ÇµÇ‹ÇµÇΩ";
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
	xmlhttp.setRequestHeader( "Content-Type", "application/x-www-form-urlencoded" );
	xmlhttp.send( EncodeHTMLForm( data ) );
	return false;
}

function EncodeHTMLForm( data ){//Convert data to HTML form format
	var params = [];
	for( var name in data ){
		var value = data[ name ];
		var param = encodeURIComponent( name ) + "=" + encodeURIComponent( value );
		params.push( param );
	}
	return params.join( "&" ).replace( /%20/g, "+" );
}

function thumbonclick(e){
	var t;
	if("touches" in e && e.touches.length>0){
		e=e.touches[0];
	}
	if ((t = e.target) == document) {
		return;
	}
	var c=t;
	var removepdm=false;
	var pdmno=-1;
	var pdm=document.getElementById("pdm");
	pdmchk:
	if(pdm){
		pdmno=pdm.getAttribute("data-no");
		while(c) {
			if(c == document){break;}
			if(c.id=="pdm"){break pdmchk;}
			c = c.parentNode;
		}
		pdm.parentNode.removeChild(pdm);
		removepdm=true;
//		console.log("close pdm");
	}

	if(t.className == 'pdmc'){
		if(!removepdm||t.getAttribute("data-no")!=pdmno){
			if (pdmopen(e)) {
				e.preventDefault();
			}
		}
	}
}

function catresize(){
	var ct=document.getElementById("cattable");
	if(!ct){return;}
	var ra=document.getElementById("rightadc");
	if(!ra){return;}

	if(typeof ct.clientWidth!=="undefined" && ct.clientWidth<(window.innerWidth || document.documentElement.clientWidth || 0)-350){
		ra.style.position="fixed";
		ra.style.top=ct.offsetTop+"px";
	}else{
		ra.style.position="static";
		ra.style.top="";
	}
}

var queue = null;

function hidetd(){
	var catmode=getParam("sort");
	if(catmode!="9" && catmode!="7"){
		return;
	}
	var histhide=getHistory(catmode);
	var cathide=histhide.cathide;
	var ct=document.getElementById("cattable");
	var ca=ct.getElementsByTagName("a");
	for(i=0;i<ca.length;i++){
		var ci=ca[i];
		var cn=ci.href.match(/res\/([0-9]+)\.htm/)[1];
		if(cn==null){continue;}
		if (cn>0 && cathide.indexOf(cn) != -1){
			ci.parentNode.style.display="none";
		}
	}
}

function reflectform(){
	var vah,vahf,vhel=document.getElementById('vh');
	if(vhel){
		try {
			vah = window.localStorage.viewaddhist;
			vahf = false;
		} catch (e) {
			vahf = true;
		}
		if(vahf||typeof vah==="undefined"){vah="true";}
		vhel.checked=((vah=="true")?true:false);
	}
	var fsz,fszf,fsel=document.getElementById('fs');
	if(fsel){
		try {
			fsz = window.localStorage.fontsize;
			fszf = false;
		} catch (e) {
			fszf = true;
		}
		if(fszf||typeof fsz==="undefined"){fsz="100";}
		document.getElementById('fs').value=fsz;
	}
}

//Add a style sheet
function addStylesheetRules (rules) {
	var styleEl = document.createElement('style');
	document.head.appendChild(styleEl);
	var styleSheet = styleEl.sheet;
	for (var i = 0; i < rules.length; i++) {
		var j = 1, 
		rule = rules[i], 
		selector = rule[0], 
		propStr = '';
		if (Array.isArray(rule[1][0])) {
			rule = rule[1];
			j = 0;
		}
		for (var pl = rule.length; j < pl; j++) {
			var prop = rule[j];
			propStr += prop[0] + ': ' + prop[1] + (prop[2] ? ' !important' : '') + ';\n';
		}
		styleSheet.insertRule(selector + '{' + propStr + '}', styleSheet.cssRules.length);
	}
}

function initStyle(){
	var fsz,fszf;
	try {
		fsz = window.localStorage.fontsize;
		fszf = false;
	} catch (e) {
		fszf = true;
	}
	if(fszf||typeof fsz==="undefined"){fsz="100";}
	var rules=[];
	rules[0]=['body',['font-size',fsz+'%']];
	addStylesheetRules(rules);
}

function Init(){//initialize when page loaded
	reflectform();
	var el=document.getElementsByClassName('spinnerBtn');
	if(el.length>0){
		for(var i=0;i<el.length;i++){
			el[i].addEventListener('touchstart',longTap,false);
			el[i].addEventListener('mousedown',longTap,false);
			el[i].addEventListener('click',longTap,false);
		}
		document.addEventListener('touchend',stopLongTap,false);
		document.addEventListener('mouseup',stopLongTap,false);
		document.addEventListener('scroll',stopLongTap,false);
		var sfel=document.getElementById('settingform');
		if(sfel){
			sfel.addEventListener('submit',submitform,false);
		}
		var cfel=document.getElementById('clearform');
		if(cfel){
			cfel.addEventListener('submit',submitclearform,false);
		}
	}
	if(getParam("mode")!="catset"){
		initStyle();
	}
	window.addEventListener("resize", function() {
		clearTimeout( queue );
		queue = setTimeout(function() {
			catresize();
		}, 10 );
	}, false );
	catresize();
	var ct=document.getElementById("cattable");
	if(ct){
		var ca=ct.getElementsByTagName("a");
		for(i=0;i<ca.length;i++){
			var ci=ca[i];
			var cn=ci.href.match(/res\/([0-9]+)\.htm/)[1];
			if(cn==null){continue;}
			var cd=document.createElement("div");
			cd.className="pdmc";
			cd.setAttribute("data-no", cn);
			ci.parentNode.appendChild(cd);
		}
		if(document.addEventListener){
			document.addEventListener((navigator.userAgent.match(/iP(hone|od|ad)/i)) ? "touchstart" : "click", thumbonclick, false);
		}
		hidetd();
	}
}

function submitform(e){
	var vhel=document.getElementById('vh');
	if(vhel){
		try{
			window.localStorage.viewaddhist=vhel.checked;
		}catch(e){}
	}
	var fsel=document.getElementById('fs');
	if(fsel){
		try{
			window.localStorage.fontsize=fsel.value;
		}catch(e){}
	}
	return true;
}

function submitclearform(e){
	window.localStorage.removeItem("viewaddhist");
	window.localStorage.removeItem("fontsize");
	reflectform();
}

function longTap(e){
	e.stopPropagation();
	if(spinnerArr['interval']){return false;}
	var target = e.target.getAttribute('data-target');
	spinnerArr['target'] = target;
	spinnerArr['timestamp'] = e.timeStamp;
	spinnerArr['cal'] = Number(e.target.getAttribute('data-cal'));
	if(e.type == 'click'){
		setSpinnerNum();
		spinnerArr = [];
		return false;
	}
	setTimeout(function(){
		if(!spinnerArr['interval'] && spinnerArr['timestamp'] == e.timeStamp){
		    spinnerArr['interval'] = setInterval(setSpinnerNum, 40);
		}
	}, 300);
}

function stopLongTap(e){
	e.stopPropagation();
	if(spinnerArr['interval']){
		clearInterval(spinnerArr['interval']);
		spinnerArr = [];
	}
}

function setSpinnerNum(){
	var t=spinnerArr['target'],target;
	if(t.charAt(0)=="."){
		t=t.slice(1);
		target=document.getElementsByClassName(t)[0];
	}else{
		if(t.charAt(0)=="#"){
			t=t.slice(1);
		}
		target=getElementById(t);
	}
	var num = Number(target.value);
	num = num + spinnerArr['cal'];
	if(num > Number(target.getAttribute('data-max'))){
		target.value=Number(target.getAttribute('data-max'));
	}else if(Number(target.getAttribute('data-min')) > num){
		target.value=Number(target.getAttribute('data-min'));
	}else{
		target.value=num;
	}
}

var spinnerArr = [];

if(document.addEventListener){//add EventListener for initialize
	document.addEventListener('DOMContentLoaded',Init,false);
}else if(document.attachEvent){
	var CheckReadyState = function(){
		if(document.readyState=='complete'){
			document.detachEvent('onreadystatechange',CheckReadyState);
			Init();
		}
	};
	document.attachEvent('onreadystatechange',CheckReadyState);
	(function(){
		try{
			document.documentElement.doScroll( 'left' );
		}catch( e ){
			setTimeout( arguments.callee, 10 );
			return;
		}
		document.detachEvent('onreadystatechange', CheckReadyState);
		Init();
	})();
}
