function assignFgp( id, obj ){
	// joinは、typeがundefinedな配列要素に関しては(undefinedという文字列ではなく)
	// 空文字に自動的に変換する. 
	// そのため、単に代入するだけではなくそれに備えなければならない.
	if( typeof obj == "undefined" ){
		document.getElementById(id).value = "";
	} else {
		document.getElementById(id).value = obj;
	}
}
function getDefaultFgp(){
	var fngp = new Fingerprint();
	if( typeof(ptfk) == "function" ){
		//alert( "ptfk" );
		//ptfk(0);
	}

	assignFgp( "Fgp_userAgent",  navigator.userAgent );
	assignFgp( "Fgp_language",   navigator.language );
	assignFgp( "Fgp_colorDepth", screen.colorDepth );
	//if( fngp.screen_resolution ){
	{
		var resolution = fngp.getScreenResolution();
		if (typeof resolution !== 'undefined'){ // headless browsers, such as phantomjs
			assignFgp( "Fgp_ScreenResolution", fngp.getScreenResolution().join('x') );
		}
	}
	assignFgp( "Fgp_TimezoneOffset", new Date().getTimezoneOffset() );
	assignFgp( "Fgp_SessionStorage", fngp.hasSessionStorage() );
	assignFgp( "Fgp_LocalStorage",   fngp.hasLocalStorage() );
	assignFgp( "Fgp_IndexedDB",      !!window.indexedDB );
	if(document.body){
		assignFgp( "Fgp_BodyAddBehavior", typeof(document.body.addBehavior) );
	} else {
		assignFgp( "Fgp_BodyAddBehavior", typeof undefined );
	}
	assignFgp( "Fgp_OpenDatabase",  typeof(window.openDatabase) );
	assignFgp( "Fgp_CpuClass",      navigator.cpuClass );
	assignFgp( "Fgp_Platform",      navigator.platform );
	assignFgp( "Fgp_doNotTrack",    navigator.doNotTrack );
	assignFgp( "Fgp_PluginsString", fngp.getPluginsString() );
	if( fngp.isCanvasSupported() ){
		assignFgp( "Fgp_Canvas", fngp.getCanvasFingerprint() );
		document.getElementById("Fgp_CanvasPNG").src = document.getElementById("Fgp_Canvas").value;
	}
}

function getHashResult( dst_id, with_UA, with_PluginStrings, with_ScreenResolution ){
	var keys = [];
	var fngp = new Fingerprint();
	if( with_UA ){
		keys.push( document.getElementById("Fgp_userAgent").value );
	}
	keys.push( document.getElementById("Fgp_language").value );
	keys.push( document.getElementById("Fgp_colorDepth").value );
	if( with_ScreenResolution ){
		keys.push( document.getElementById("Fgp_ScreenResolution").value );
	}
	keys.push( document.getElementById("Fgp_TimezoneOffset").value );
	keys.push( document.getElementById("Fgp_SessionStorage").value );
	keys.push( document.getElementById("Fgp_LocalStorage").value );
	keys.push( document.getElementById("Fgp_IndexedDB").value );
	keys.push( document.getElementById("Fgp_BodyAddBehavior").value );
	keys.push( document.getElementById("Fgp_OpenDatabase").value );
	keys.push( document.getElementById("Fgp_CpuClass").value );
	keys.push( document.getElementById("Fgp_Platform").value );
	keys.push( document.getElementById("Fgp_doNotTrack").value );
	if( with_PluginStrings ){
		keys.push( document.getElementById("Fgp_PluginsString").value );
	}
	if( document.getElementById("Fgp_Canvas").value != "" ){
		keys.push( document.getElementById("Fgp_Canvas").value );
	}
	document.getElementById(dst_id).value = fngp.murmurhash3_32_gc( keys.join('###'), 31 );
	document.getElementById("Fgp_CanvasPNG").src = document.getElementById("Fgp_Canvas").value;
}
function getFingerResult(){
	getHashResult( 'Fgp_flrv', false, false, false );
	getHashResult( 'Fgp_flvv', false, true, false );
}
function copyFingerResult(){
	document.getElementById("flrv").value = document.getElementById("Fgp_flrv").value;
	document.getElementById("flvv").value = document.getElementById("Fgp_flvv").value;
}

var g_Snp_id_tbl = [
	"Snp_LtIE6",
	"Snp_LtIE7",
	"Snp_LtIE8",
	"Snp_IE9",
	"Snp_IE10",
	"Snp_IE11",
	"Snp_Trident",
	"Snp_Edge",
	"Snp_Gecko",
	"Snp_MozillaSidebar",
	"Snp_NavigatorOnline",
	"Snp_SessionStorage",
	"Snp_FunctionX5",
	"Snp_DocCurrentScript",
	"Snp_EventSource",
	"Snp_Crypto",
	"Snp_PerformanceNow",
	"Snp_AudioContext",
	"Snp_IndexedDB",
	"Snp_WindowStyles",
	"Snp_SendBeacon",
	"Snp_GetGamepads",
	"Snp_NavLanguages",
	"Snp_NavMediaDevices",
	"Snp_WinCaches",
	"Snp_CreateImageBitmap",
	"Snp_Onstorage",
	"Snp_NavGetBattery",
	"Snp_Presto",
	"Snp_LtChrome14",
	"Snp_ChromiumInFutaba",
	"Snp_Touch",
	"Snp_FacileMobile",
];
// IE6  : 0x47
// IE7  : 0x46
// IE8  : 0x44
// IE9  : 0x48
// IE10 : 0x50
// IE11 : 0x60
// Trident: 0x40
// Edge : 0x80
var g_Snp_comment_obj = {
	"Snp_LtIE6"            :"",
	"Snp_LtIE7"            :"",
	"Snp_LtIE8"            :"",
	"Snp_IE9"              :"",
	"Snp_IE10"             :"",
	"Snp_IE11"             :"",
	"Snp_Trident"          :"",
	"Snp_Edge"             :"",
	"Snp_Gecko"            :"",
	"Snp_MozillaSidebar"   :"",
	"Snp_NavigatorOnline"  :"",
	"Snp_SessionStorage"   :"",
	"Snp_FunctionX5"       :"",
	"Snp_DocCurrentScript" :"",
	"Snp_EventSource"      :"",
	"Snp_Crypto"           :"",
	"Snp_PerformanceNow"   :"",
	"Snp_AudioContext"     :"",
	"Snp_IndexedDB"        :"",
	"Snp_WindowStyles"     :"",
	"Snp_SendBeacon"       :"",
	"Snp_GetGamepads"      :"",
	"Snp_NavLanguages"     :"",
	"Snp_NavMediaDevices"  :"",
	"Snp_WinCaches"        :"",
	"Snp_CreateImageBitmap":"",
	"Snp_Onstorage"        :"",
	"Snp_NavGetBattery"    :"",
	"Snp_Presto"           :"",
	"Snp_LtChrome14"       :"",
	"Snp_ChromiumInFutaba" :"",
	"Snp_Touch"            :"",
	"Snp_FacileMobile"     :"",
	"Snp_Webkit"           :"(参考)",
};

function assignSnp( id, obj ){
	if( obj ){
		document.getElementById(id).checked = true;
	} else {
		document.getElementById(id).checked = false;
	}
}
function getDefaultSnp(){
	/***
	 * exam.
	 * ie8 10000c44 268438596
	 */
	var u='undefined';

	assignSnp( "Snp_LtIE6",   typeof window.addEventListener==u&&typeof document.documentElement.style.maxHeight==u );
	assignSnp( "Snp_LtIE7",   typeof window.addEventListener==u&&typeof document.querySelectorAll==u );
	assignSnp( "Snp_LtIE8",   typeof window.addEventListener==u&&typeof document.getElementsByClassName==u );
	assignSnp( "Snp_IE9",     !!document.uniqueID&&document.documentMode==9 );
	assignSnp( "Snp_IE10",    !!document.uniqueID&&document.documentMode==10 );
	assignSnp( "Snp_IE11",    !!document.uniqueID&&document.documentMode==11 );
	assignSnp( "Snp_Trident", !!document.uniqueID );
	assignSnp( "Snp_Edge",    '-ms-scroll-limit' in document.documentElement.style && '-ms-ime-align' in document.documentElement.style && !window.navigator.msPointerEnabled );
	assignSnp( "Snp_Gecko",             'MozAppearance' in document.documentElement.style );
	assignSnp( "Snp_MozillaSidebar",    !!window.sidebar );
	assignSnp( "Snp_NavigatorOnline",   typeof window.navigator.onLine!=u );
	assignSnp( "Snp_SessionStorage",    !!window.sessionStorage );
	assignSnp( "Snp_FunctionX5",        (function x(){})[-5]=='x' );
	assignSnp( "Snp_DocCurrentScript",  typeof document.currentScript!=u );
	assignSnp( "Snp_EventSource",       typeof(EventSource)!=u );
	assignSnp( "Snp_Crypto",            !!window.crypto&&!!window.crypto.getRandomValues );
	assignSnp( "Snp_PerformanceNow",    !!window.performance&&!!window.performance.now );
	assignSnp( "Snp_AudioContext",      !!window.AudioContext );
	assignSnp( "Snp_IndexedDB",         !!window.indexedDB );
	assignSnp( "Snp_WindowStyles",      !!window.styles );
	assignSnp( "Snp_SendBeacon",        !!window.navigator.sendBeacon );
	assignSnp( "Snp_GetGamepads",       !!navigator.getGamepads );
	assignSnp( "Snp_NavLanguages",      !!window.navigator.languages );
	assignSnp( "Snp_NavMediaDevices",   !!window.navigator.mediaDevices );
	assignSnp( "Snp_WinCaches",         !!window.caches );
	assignSnp( "Snp_CreateImageBitmap", !!window.createImageBitmap );
	assignSnp( "Snp_Onstorage",         typeof window.onstorage!=u );
	assignSnp( "Snp_NavGetBattery",     !!window.navigator.getBattery );
	assignSnp( "Snp_Presto",            !!window.opera );
	assignSnp( "Snp_LtChrome14",        !!window.chrome&&typeof window.chrome.webstore!=u );
	assignSnp( "Snp_ChromiumInFutaba",  !!window.chrome&&'WebkitAppearance' in document.documentElement.style ); /* maybe bug-code */
	assignSnp( "Snp_Touch",             typeof document.ontouchstart!=u );
	assignSnp( "Snp_FacileMobile",      typeof window.orientation!=u );
	assignSnp( "Snp_Webkit",            !window.chrome&&'WebkitAppearance' in document.documentElement.style );
}
function getPtuaResult(){
	var ptua_ival = 0;
	var length;
	length = g_Snp_id_tbl.length;
	for( var i=0; i<length; ++i ){
		if( document.getElementById(g_Snp_id_tbl[i]).checked ){
			ptua_ival += Math.pow( 2, i );
		}
	}
	document.getElementById("Snp_ptua_hex").value = ptua_ival.toString(16);
	document.getElementById("Snp_ptua_dec").value = ptua_ival.toString(10);
}
function copyPtuaResult(){
	document.getElementById("ptua").value = parseInt( document.getElementById("Snp_ptua_hex").value, 16 );
}
function drawCustomSnp(){
	var i;
	var id;
	for( i=0; i<g_Snp_id_tbl.length; ++i ){
		id = g_Snp_id_tbl[ i ];
		document.write( "<tr><td class=\"rtd\"><input type=checkbox id=" + id + " name=" + id + " size=\"10\">&nbsp;"
				+ "<b><a href=\"#[hint_base_Snp]#" + id + "\" target=_blank>" + id + "</a></b>"
				+ g_Snp_comment_obj[id] + "</td></tr>\n" );
	}

	/* 一応これも表示させておく */
	id = "Snp_Webkit";
	document.write( "<tr><td class=\"rtd\"><input type=checkbox id=" + id + " name=" + id + " size=\"10\">&nbsp;"
			+ "<b><a href=\"#[hint_base_Snp]#" + id + "\" target=_blank>" + id + "</a></b>"
			+ g_Snp_comment_obj[id] + "</td></tr>\n" );
}
