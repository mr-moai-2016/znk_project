<!--
if (navigator.appName.charAt(0) == 'N'){
if (navigator.userAgent.match(/Opera/)){
var app = '8';
}else if (navigator.userAgent.match(/Mozilla\/(2\.\d?)/)){
var app = RegExp.$1;
}else if (navigator.userAgent.match(/Mozilla\/(3\.\d?)/)){
var app = RegExp.$1;
}else if (navigator.userAgent.match(/Mozilla\/(4\.\d?)/)){
var app = RegExp.$1;
}else{
var app = '8';
};
}else{
var app = '8';
};
if (app.substr(0,1) < 5){
var scyho = "<table width='550' border='0' cellspacing='0' cellpadding='10' bgcolor='#FFCCCC'><tr><td class='j18'><font color='#CC0000'>★</font><b>推奨ブラウザについて</b><br />Internet Explorerは5.0以上で、Netscapeは6.0以上でご利用ください</td></tr></table><br />";
}else{
var scyho = "";
};
function LinkSelect(form, sel) {
adrs = sel.options[sel.selectedIndex].value;if (adrs != "-" ) {
window.open("http://home.alc.co.jp/db/owa/sp_item_find?" + adrs ,"","toolbar=1,location=1,directories=1,status=1,menubar=1,scrollbars=1,resizable=1,copyhistory=1");
};
};
function MM_preloadImages() { //v3.0
  var d=document; if(d.images){ if(!d.MM_p) d.MM_p=new Array();
    var i,j=d.MM_p.length,a=MM_preloadImages.arguments; for(i=0; i<a.length; i++)
    if (a[i].indexOf("#")!=0){ d.MM_p[j]=new Image; d.MM_p[j++].src=a[i];}}
}
function MM_swapImgRestore() { //v3.0
  var i,x,a=document.MM_sr; for(i=0;a&&i<a.length&&(x=a[i])&&x.oSrc;i++) x.src=x.oSrc;
}
function MM_findObj(n, d) { //v4.01
  var p,i,x;  if(!d) d=document; if((p=n.indexOf("?"))>0&&parent.frames.length) {
    d=parent.frames[n.substring(p+1)].document; n=n.substring(0,p);}
  if(!(x=d[n])&&d.all) x=d.all[n]; for (i=0;!x&&i<d.forms.length;i++) x=d.forms[i][n];
  for(i=0;!x&&d.layers&&i<d.layers.length;i++) x=MM_findObj(n,d.layers[i].document);
  if(!x && d.getElementById) x=d.getElementById(n); return x;
}
function MM_swapImage() { //v3.0
  var i,j=0,x,a=MM_swapImage.arguments; document.MM_sr=new Array; for(i=0;i<(a.length-2);i+=3)
   if ((x=MM_findObj(a[i]))!=null){document.MM_sr[j++]=x; if(!x.oSrc) x.oSrc=x.src; x.src=a[i+2];}
}
// 対象文字列全置換
function allReplace(text, sText, rText) {
	while (true) {
		dummy = text;
		text = dummy.replace(sText, rText);
		if (text == dummy) {
			break; // 置換しても変化しなければループを抜ける。
		}
	}
	return text;
}
// バリデーション
function checkWord(obj) {
	// バリデーション文字配列
	var validwd = new Array(14);
	validwd[0] = "<";
	validwd[1] = ">";
	validwd[2] = "index.html";
	validwd[3] = "favicon.ico";
	validwd[4] = "■";
	validwd[5] = "□";
	validwd[6] = "◆";
	validwd[7] = "◇";
	validwd[8] = "▲";
	validwd[9] = "△";
	validwd[10] = "▼";
	validwd[11] = "▽";
	validwd[12] = "★";
	validwd[13] = "☆";
	var ck = document.forms[obj].q.value;
	if (!ck) {
		alert("検索語が入力されていません");
		return false;
	};
	ck = allReplace(ck, "*", "");
//	ck = allReplace(ck, "＊", "");
	if (!ck) {
		alert("* 以外の検索語が入力されていません");
		return false;
	};
	var st = document.forms[obj].q.value;
	// 禁止ワードチェック
	for (var i = 0; i < validwd.length; i++) {
		if (st.toLowerCase().indexOf(validwd[i]) != -1) {
			alert(validwd[i] + " の文字は使用できません。");
			return false;
		}
	}
	st = allReplace(st, "'", "\'");
	st = allReplace(st, "　", " ");
	document.forms[obj].q.value = st;
	return true;
}
// submitボタンクリック
function goF1() {
	var a = goF1.arguments;
	var obj = "";
	var wname = "";
	if (a.length > 0) {
		obj = a[0];
	}
	if (a.length > 1) {
		wname = a[1];
	}

	if (obj != "") {
		if (!checkWord(obj)) return false;
		var ref = "";
		var ie = "UTF-8";
		if (!!window.document.f1.ie) {
			if (document.f1.ie.value != "") {
				ie = document.f1.ie.value;
			}
		}
		if (!!window.document.f1.ref) {
			if (document.f1.ref.value != "") {
				ref = "?ref=" + document.f1.ref.value;
			}
		}
		if (wname != "") {
			window.open(CONTEXT_PATH + encodeURL(document.forms[obj].q.value) + "/" + ie + "/" + ref, wname);
		}
		else {
			document.location.href = CONTEXT_PATH + encodeURL(document.forms[obj].q.value) + "/" + ie + "/" + ref;
		}
	}
}
// Enter
function goF2() {
	var a = goF2.arguments;
	var obj = "";
	var wname = "";
	if (a.length == 1) {
		obj = a[0];
		
		if (obj != "") {
			goF1(obj);
		}
		return false;
	}
	else if (a.length > 1) {
		obj = a[0];
		wname = a[1];
		if (obj != "" && wname != "") {
			goF1(obj, wname);
		}
		return false;
	}
}
// クリアボタン
function wordClear(obj) {
	document.forms[obj].q.value = "";
	document.forms[obj].q.focus();
}
// エンコード
function encodeURL(str){
	var s0, i, s, u;
	s0 = "";
	for (i = 0; i < str.length; i++){
		s = str.charAt(i);
		u = str.charCodeAt(i);
		if (s == " ") {
			s0 += "+";
//			s0 += " ";
		}
		else {
			if ( u == 0x2a || u == 0x2d || u == 0x2e || u == 0x5f || ((u >= 0x30) && (u <= 0x39)) || ((u >= 0x41) && (u <= 0x5a)) || ((u >= 0x61) && (u <= 0x7a))){
				s0 = s0 + s;
			}
			else {
				if ((u >= 0x0) && (u <= 0x7f)){
					s = "0"+u.toString(16);
					s0 += "%"+ s.substr(s.length-2);
				}
				else if (u > 0x1fffff){
					s0 += "%" + (oxf0 + ((u & 0x1c0000) >> 18)).toString(16);
					s0 += "%" + (0x80 + ((u & 0x3f000) >> 12)).toString(16);
					s0 += "%" + (0x80 + ((u & 0xfc0) >> 6)).toString(16);
					s0 += "%" + (0x80 + (u & 0x3f)).toString(16);
				}
				else if (u > 0x7ff){
					s0 += "%" + (0xe0 + ((u & 0xf000) >> 12)).toString(16);
					s0 += "%" + (0x80 + ((u & 0xfc0) >> 6)).toString(16);
					s0 += "%" + (0x80 + (u & 0x3f)).toString(16);
				}
				else {
					s0 += "%" + (0xc0 + ((u & 0x7c0) >> 6)).toString(16);
					s0 += "%" + (0x80 + (u & 0x3f)).toString(16);
				}
			}
		}
	}
	return s0;
}
// デコード
function decodeURL(str){
	var s0, i, j, s, ss, u, n, f;
	s0 = "";
	for (i = 0; i < str.length; i++){
		s = str.charAt(i);
		if (s == "+"){s0 += " ";}
		else {
			if (s != "%"){s0 += s;}
			else{
				u = 0;
				f = 1;
				while (true) {
					ss = "";
					for (j = 0; j < 2; j++ ) {
						sss = str.charAt(++i);
						if (((sss >= "0") && (sss <= "9")) || ((sss >= "a") && (sss <= "f"))  || ((sss >= "A") && (sss <= "F"))) {
							ss += sss;
						} else {--i; break;}
					}
					n = parseInt(ss, 16);
					if (n <= 0x7f){u = n; f = 1;}
					if ((n >= 0xc0) && (n <= 0xdf)){u = n & 0x1f; f = 2;}
					if ((n >= 0xe0) && (n <= 0xef)){u = n & 0x0f; f = 3;}
					if ((n >= 0xf0) && (n <= 0xf7)){u = n & 0x07; f = 4;}
					if ((n >= 0x80) && (n <= 0xbf)){u = (u << 6) + (n & 0x3f); --f;}
					if (f <= 1){break;}
					if (str.charAt(i + 1) == "%"){ i++ ;}
					else {break;}
				}
				s0 += String.fromCharCode(u);
			}
		}
	}
	return s0;
}
var CONTEXT_PATH = "http://eow.alc.co.jp/";
// -->
