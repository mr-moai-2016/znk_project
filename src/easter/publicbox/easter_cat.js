
function fragmentFromString(strHTML) {//create fragment from string
	var frag = document.createDocumentFragment(),tmp = document.createElement('body'), child;
	tmp.innerHTML = strHTML;
	while (child = tmp.firstChild) {
		frag.appendChild(child);
	}
	return frag;
}
function getBoardURL( src )
{
	var idx = src.indexOf( Easter_hostname );
	var url = src.substr( idx );
	idx = url.indexOf( '/futaba.php' );
	return url.substr( url, idx );
}
function updateres(ret,targetId,st,step){//append updated article
	var data = ret["res"],updated=false;
	var nx = document.getElementById("catnext");
	if(nx){
		nx.parentNode.removeChild(nx);
	}
	var th = document.getElementById(targetId);
	var board_url = getBoardURL( Easter_hyperpost_url );
	for (var ke=st;ke<data.length;ke++){
		var d1 = document.createElement('div');
		d1.className="cs cs"+ret["size"];
		var d2 = document.createElement('div');
		d2.className="cu cs"+ret["size"];
		var a1 = document.createElement('a');
		a1.href = '/easter?est_view=' + board_url + '/' + "res/"+data[ke].no+".htm";
		a1.target = "_blank";
		var t1 = fragmentFromString(data[ke].com);
		var t2 = fragmentFromString(data[ke].cr);
		var b1=document.createElement('br');
		var b2=document.createElement('br');
		if(data[ke].w>0 && data[ke].h>0){
			var i1 = document.createElement('img');
			i1.width=data[ke].w;
			i1.height=data[ke].h;
			i1.alt="";
			i1.src= "//" + Easter_hostname + data[ke].src;
			a1.appendChild(i1);//a<-img
			d2.appendChild(a1);//a
			d2.appendChild(b1);//br
			d2.appendChild(t1);//com
			d2.appendChild(b2);//br
			d2.appendChild(t2);//count
		}else{
			a1.appendChild(t1);//a<-com
			d2.appendChild(a1);//br
			d2.appendChild(b1);//br
			d2.appendChild(t2);//count
		}
		d1.appendChild(d2);
		th.appendChild(d1);
		if(ke>=st+step-1){
			var d3 = document.createElement('div');
			d3.id="catnext";
			d3.appendChild(document.createTextNode("["));
			var s1 = document.createElement('span');
			s1.innerHTML  = "‘±‚«‚ð•\Ž¦‚·‚é";
			d3.appendChild(s1);
			d3.appendChild(document.createTextNode("]"));
			d3.onclick = new Function("updateres(ret,\""+targetId+"\","+(st+step)+","+step+");return(false);");
			th.appendChild(d3);
			break;
		}
	}
}
function setStyle( ele, styles ) {
	Object.keys( styles ).forEach( function ( key ) {
		ele.style[key] = styles[key];
	});
}

function calcStrHeight( str, sz, fmly ) {
	var ele = document.createElement('span');
	ele.id = 'calc';
	ele.className ='s';
	ele.innerHTML = str;
	var styles = {'white-space': 'nowrap','visibility': 'hidden'};
	if( sz != undefined ) {
		styles['font-size'] = sz;
	}
	if( fmly != undefined ) {
		styles['font-family'] = fmly;
	}
	setStyle( ele, styles );
	document.body.appendChild( ele );
	var ele_h = ele.offsetHeight;
	document.body.removeChild( ele );
	return ele_h;
}

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

function Init(){//initialize when page loaded
	var strheight=calcStrHeight("M",'small',false)*2;
	var sizearr=[[56,85],[79,110],[104,135],[129,160],[154,185],[179,210],[254,285]];
	var rules=[];
	for (var i = 0; i < 7; i++){
		rules[i]=['.cs'+i,['height',(sizearr[i][1]-28+strheight)+'px']];
	}
	addStylesheetRules(rules);
	updateres(ret,"cattable",0,100);
	catresize();
}
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

function catresize(){
	var ct=document.getElementById('cattable');
	if(!ct){return;}
	var ra=document.getElementById('rightadc');
	if(!ra){return;}

	if(typeof ct.clientWidth!=='undefined' && (window.innerWidth || document.documentElement.clientWidth || 0)>640){
		ra.style.position='fixed';
		ra.style.top=ct.offsetTop+'px';
		ct.classList.remove("ch");
	}else{
		ra.style.position='static';
		ra.style.top='';
		ct.classList.add("ch");
	}
}
var queue = null,wait = 10;
window.addEventListener( 'resize', function() {
	clearTimeout( queue );
	queue = setTimeout(function() {
		catresize();
	}, wait );
}, false );


