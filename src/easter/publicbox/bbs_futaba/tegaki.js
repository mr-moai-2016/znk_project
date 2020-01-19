/* 0-byte avoidance */

function TegakiBase_buildHtmlFrgm( width, height )
{
	var fg = fragmentFromString( ' \
		<div id="oest1"><canvas id="oejs" width="'+width+'" height="'+height+'"></canvas></div>\
		<div id="oest2">\
			<input type="input" name="c4" value="" size="12" id="oecolor"/>\
			<div id="oepb">\
				<div class="oepbb"><div class="oepba"></div></div>\
			</div>\
			<div id="oest5">\
				<input type="radio" name="switch" id="sw0" value="0" class="css-checkbox" checked/><label for="sw0" id="sw0label" class="oelabel"></label>\
				<input type="radio" name="switch" id="sw1" value="1" class="css-checkbox"        /><label for="sw1" id="sw1label" class="oelabel"></label>\
			</div>\
			<div id="oest3">\
				<div id="oest4">\
					<svg width="14" height="32">\
						<circle cx="7" cy="7" r="6.5" fill="#000"></circle>\
					</svg>\
					<svg width="13" height="32">\
						<circle cx="9" cy="26" r="1" fill="#000"></circle>\
					</svg>\
				</div>\
				<div id="oest6">\
					<input type="hidden" name="slider" id="slider1o" value="45">\
					<div id="slider1" class="js-slider">\
						<div class="bar"></div>\
						<div class="knob"></div>\
					</div>\
				</div>\
			</div>\
			<input type="button" class="zbtn" id="oejsErase">\
		</div>\
		<!--パレット-->\
		<div class="oeplframe">\
			<div class="oepl" id="oepalet">\
				<div class="oeplt">\
					<div class="oeplc"></div>\
					<input class="oepli">\
				</div>\
			</div>\
		</div>\
		<!--全消しダイアログ-->\
		<div class="oezkbk">\
			<div class="oezk">\
				<div>全消し</div>\
				<div>全部消しますか？</div>\
				<div><input type="button" value="Yes"><input type="button" value="No"></div>\
			</div>\
		</div>\
	' );
	return fg;
}

var TegakiUndo = (function(){ /* module-pattern */
	'use strict';

	/* Undo */
	var m_undo_idx_max = 10;
	var m_undo_idx     = 0;
	var m_undo=[];
	for(var i=0;i<=10;i++){m_undo.unshift("");}

	return {
		registUndo: function( ctx )
		{
			m_undo_idx++;
			if( m_undo_idx > m_undo_idx_max ){
				m_undo_idx = 0;
			}
	
			m_undo[ m_undo_idx ] = ctx.getImageData( 0, 0,
					ctx.canvas.width,
					ctx.canvas.height );
		},
		doUndo: function( ctx )
		{
			m_undo_idx--;
			if( m_undo_idx < 0 ){
				m_undo_idx = m_undo_idx_max;
			}
			if( m_undo[ m_undo_idx ] == "" ){
				m_undo_idx++;
				if( m_undo_idx > m_undo_idx_max ){
					m_undo_idx=0;
				}
			} else {
				ctx.putImageData( m_undo[ m_undo_idx ], 0, 0 );
			}
		},
		recoverImg: function( ctx )
		{
			ctx.putImageData( m_undo[ m_undo_idx ], 0, 0 );
		}
	}

})(); /* enodof TegakiUndo */


var tegakiJs = (function(){ /* module-pattern */
	'use strict';

	/* Flags */
	var m_drawing = false;
	var m_at_first = true;
	var m_req_regist_undo = false;
	var m_is_show_pal    = false;
	var m_is_show_dialog = false;

	/* Pointer */
	var m_x;
	var m_y;
	var m_timestamp=0;
	var m_last_x;
	var m_last_y;
	var m_last_timestamp;

	/* GUI */
	var m_top_widget;
	var m_canvas;
	var m_ctx;
	var m_pal_trigger;
	var m_slider;
	var m_slider_out;
	var m_slider_value;
	var m_slider_cursor;
	var m_slider_cursor_height;

	var dragging = false;
	var dragged = false;

	/* CurrentColor */
	var m_col_idx = 0;
	var m_col_stocks=[ "#800000", "#f0e0d6" ];


	var m_ua = window.navigator.userAgent.toLowerCase();
	var m_browser;
	var m_ev_listen_opts;

	function getCanvas()
	{
		return document.getElementById("oejs");
	}
	function getTopWidget( target )
	{
		return document.getElementById( target );
	}
	function getPalTrigger()
	{
		return document.getElementById( "oepb" );
	}
	function getCurrentColor( col )
	{
		return m_col_stocks[ m_col_idx ];
	}
	function setCurrentColor( col )
	{
		m_col_stocks[ m_col_idx ] = col;
		m_pal_trigger.style.backgroundColor = col;
	}

	function disableSmoothing()
	{
		if( 'imageSmoothingEnabled' in m_ctx ){
			m_ctx.imageSmoothingEnabled = false;
		} else {
			m_ctx.mozImageSmoothingEnabled    = false;
			m_ctx.msImageSmoothingEnabled     = false;
			m_ctx.webkitImageSmoothingEnabled = false;
		}
	}

	function onKeyDown(e)
	{
		if( e.keyCode == 90 && e.ctrlKey ){ /* Ctrl-Z */
			TegakiUndo.doUndo( m_ctx );
		}
	}
	function onClick_UndoButton(e)
	{
		TegakiUndo.doUndo( m_ctx );
	}
	function adjTouch(touch){//iOS4.3バグ対応
		if(touch.clientX==touch.pageX && touch.clientY==touch.pageY &&
				m_ua.indexOf('applewebkit/533') !== -1){
			var scX = (window.pageXOffset !== undefined) ? window.pageXOffset :
				(document.documentElement || document.body.parentNode || document.body).scrollLeft;
			var scY = (window.pageYOffset !== undefined) ? window.pageYOffset :
				(document.documentElement || document.body.parentNode || document.body).scrollTop;
			return {x:scX,y:scY};
		}else{
			return {x:0,y:0};
		}
	}
	function getxy(e){//マウスの座標を得る
		if( m_top_widget.style.visibility=="hidden" ){
			return;
		}
		var cx,cy;
		if( "touches" in e && e.touches.length>0 ){
			var adj = adjTouch( e.touches[0] );
			cx = e.touches[0].clientX - adj.x;
			cy = e.touches[0].clientY - adj.y;
		} else {
			cx = e.clientX;
			cy = e.clientY;
		}
		var pos = getPosition( m_canvas );
		m_x         = cx - pos.x;
		m_y         = cy - pos.y;
		m_timestamp = e.timeStamp;
	}
	function onMouseEnter(e)
	{
		getxy(e);
		m_last_x         = m_x;
		m_last_y         = m_y;
		m_last_timestamp = m_timestamp;
	}

	function create( width, height )
	{
		m_canvas = getCanvas();
		m_ctx    = m_canvas.getContext('2d');

		disableSmoothing();
		//m_ev_listen_opts = gainEventListenerOptions();
		m_ev_listen_opts = PascuaEventListener.gainPassiveFalseOptions();


		m_pal_trigger = getPalTrigger();

		m_slider               = document.getElementById('slider1');
		m_slider_out           = document.getElementById('slider1o');
		m_slider_cursor        = m_slider.getElementsByTagName('div')[1];

		if( m_at_first ){
			clearCanvas();
			m_at_first = false;
		} else {
			/* 状態回復 */
			TegakiUndo.recoverImg( m_ctx );
			//m_ctx.putImageData( m_undo[ m_undo_idx ], 0, 0 );
			m_slider_out.value = m_slider_value;

			if( m_col_idx == 0 ){
				document.getElementById( "sw0" ).checked = true;
			} else if( m_col_idx == 1 ){
				document.getElementById( "sw1" ).checked = true;
			}
		}
		m_pal_trigger.style.backgroundColor = m_col_stocks[ m_col_idx ];

		m_slider_value         = m_slider_out.value;
		m_slider_cursor_height = m_slider_cursor.clientHeight / 2;
		m_slider.addEventListener( "click", sliderClick, false );
		setSliderCursorAndValue();

		var ev_type_pointerdown = PascuaEventType.pointerdown();
		var ev_type_pointerup   = PascuaEventType.pointerup();
		var ev_type_pointermove = PascuaEventType.pointermove();

		/* canvas */
		document.addEventListener( "mouseenter",        onMouseEnter,  false );
		document.addEventListener( ev_type_pointerdown, onPointerDown, false );
		document.addEventListener( ev_type_pointerup,   onPointerUp,   false );
		document.addEventListener( ev_type_pointermove, onPointerMove, m_ev_listen_opts );

		/* pal trigger button */
		m_pal_trigger.addEventListener( ev_type_pointerdown, onClick_PalTrigger, false );

		/* slider */
		m_slider_cursor.addEventListener( ev_type_pointerdown, sliderMouseDown, false );
		document.addEventListener( ev_type_pointerup,   sliderMouseUp,   false );
		document.addEventListener( ev_type_pointermove, sliderMouseMove, m_ev_listen_opts );

		document.getElementById( "oejsErase" ).addEventListener( "click", EraseDialog, false );
		document.getElementById( "sw0label"  ).addEventListener( "click", onSwitchColor,   false );
		document.getElementById( "sw1label"  ).addEventListener( "click", onSwitchColor,   false );

		buildPalColorCellTable( document.getElementById('oepalet') );

		document.getElementsByClassName("oepli")[0].addEventListener("keypress",onSubmitColorField,false);
		document.getElementsByClassName("oezk")[0].getElementsByTagName('input')[0].addEventListener("click",EraseDialogYes,false);
		document.getElementsByClassName("oezk")[0].getElementsByTagName('input')[1].addEventListener("click",EraseDialogNo,false);
		document.getElementsByClassName("oezk")[0].addEventListener("keydown",EraseDialogKey,false);
		document.addEventListener( "keydown", onKeyDown, false );
		document.getElementById("oest4").getElementsByTagName("svg")[0].addEventListener("click",function(e){ sliderThick(e,true)},false);
		document.getElementById("oest4").getElementsByTagName("svg")[1].addEventListener("click",function(e){ sliderThick(e,false)},false);

		m_browser="";
		if (m_ua.indexOf('opera') !== -1) {
			m_browser="o";
		} else if (m_ua.indexOf('edge') !== -1) {
			m_browser="d";
		} else if (m_ua.indexOf('chrome') !== -1) {
			m_browser="c";
		} else if (m_ua.indexOf('firefox') !== -1) {
			m_browser="f";
		} else if(m_ua.match(/msie/) || m_ua.match(/trident/)) {
			if(parseFloat(m_ua.match(/(msie\s|rv:)([\d\.]+)/)[2])>=10){
				m_browser="e";
			}
		}

		if(!document.getElementById("oebtnud")){
			var undoel=document.createElement('div');
			undoel.id = "oebtnud";
			undoel.addEventListener( 'click', onClick_UndoButton, false );
			document.getElementById("oest2").appendChild(undoel);
		}
	}

	function init( target, width, height )
	{
		m_top_widget = getTopWidget( target );
		while (m_top_widget.firstChild) {
			m_top_widget.removeChild(m_top_widget.firstChild);
		}
		
		var frgm = TegakiBase_buildHtmlFrgm( width, height );
		m_top_widget.appendChild( frgm );
		create( width, height );
	}

	function clearCanvas()
	{
		m_ctx.fillStyle = "#f0e0d6";
		m_ctx.fillRect( 0, 0,
				m_canvas.width,
				m_canvas.height );

		TegakiUndo.registUndo( m_ctx );
	}
	function removeEvent()
	{
		var ev_type_pointerdown = PascuaEventType.pointerdown();
		var ev_type_pointerup   = PascuaEventType.pointerup();
		var ev_type_pointermove = PascuaEventType.pointermove();

		/* canvas */
		document.removeEventListener( "mouseenter",        onMouseEnter,  false);
		document.removeEventListener( ev_type_pointerdown, onPointerDown, false);
		document.removeEventListener( ev_type_pointerup,   onPointerUp,   false);
		document.removeEventListener( ev_type_pointermove, onPointerMove, m_ev_listen_opts);

		/* slider */
		m_slider_cursor.removeEventListener( ev_type_pointerdown, sliderMouseDown, false );
		document.removeEventListener( ev_type_pointerup,   sliderMouseUp,   false);
		document.removeEventListener( ev_type_pointermove, sliderMouseMove, m_ev_listen_opts);

		document.removeEventListener( "keydown",           onKeyDown,       false);
	}

	function toStr_Hex2(num){//16進数を返す
		return ('0' + num.toString(16)).slice(-2);
	}

	/***
	 * 与えた整数文字列を16進数文字列に変換したものを返す.
	 */
	function toHexStr( ival_str )
	{
		var ival = parseInt( ival_str );
		return ival.toString(16);
	}
	/***
	 * 与えた整数文字列を下二桁の形に矯正したものを返す.
	 */
	function clampLast2Digit( ival_str )
	{
		return ( '0' + ival_str ).slice(-2);
	}
	/***
	 * style形式のcolから16進形式の色文字列を生成する.
	 */
	function convertHexColor( col )
	{
		var ans;
		if(col.charAt(0)=="#"){
			ans = col.substr(1);
		}else{
			var list = col.replace("rgb(","")
				.replace(")","")
				.split(",");

			var idx;
			ans = "";
			for( idx=0; idx<3; ++idx ){
				ans += clampLast2Digit( toHexStr(list[idx]) );
			}
		}
		return ans;
	}
	function previewPalColor( el )
	{
		var col = el.style.getPropertyValue('background-color');
		document.getElementsByClassName("oeplc")[0].style.backgroundColor=col;

		var hex_col = convertHexColor( col );
		document.getElementsByClassName("oepli")[0].value=hex_col.toUpperCase();
	}
	function showPal( is_show_pal )
	{
		var disp = is_show_pal ? "block" : "none";
		var pal_top = document.getElementsByClassName("oeplframe")[0];
		pal_top.style.display = disp;
		m_is_show_pal = is_show_pal;
		if( m_is_show_pal ){
			previewPalColor( m_pal_trigger );
		}
	}
	function onClick_PalTrigger(e)
	{
		e.stopPropagation();
		showPal( !m_is_show_pal );
	}

	function onSwitchColor(e)
	{
		var selected_id = e.target.getAttribute("for");
		m_col_idx = document.getElementById( selected_id ).value;
		setCurrentColor( m_col_stocks[ m_col_idx ] );
	}
	function checkButtonPress( e, is_pressed )
	{
		/* Smart phone */
		if( "touches" in e ){
			is_pressed = e.touches.length>0;
			return;
		}

		/* Mouse LButton */
		switch( m_browser ){
		case 'o':
			var opv=parseFloat(m_ua.match(/(version\/)([\d\.]+)/)[2]);
			if(opv>=15){
				if(typeof e.buttons!="undefined"){
					is_pressed = (e.buttons & 1);
				}else{
					is_pressed = (e.which === 1);
				}
			}
			break;
		case 'c':
		case 'd':
			if(typeof e.buttons!="undefined"){
				is_pressed = (e.buttons & 1);
			}else{
				is_pressed = (e.which === 1);
			}
			break;
		case 'f':
			if(typeof e.buttons!="undefined"){
				is_pressed = (e.buttons & 1);
			}
			break;
		case 'e':
			is_pressed = (e.buttons & 1);
			break;
		default:
		}
	}
	function isTouchCanvas(e){//canvas内でのタッチを検出
		if("touches" in e){
			var len=e.touches.length;
			var xy=getPosition(m_canvas);
			var left=xy.x;
			var top =xy.y;
			var w=m_canvas.clientWidth;
			var h=m_canvas.clientHeight;
			for(var i=0;i<len;i++){
				var adj=adjTouch(e.touches[i]);
				var cx=e.touches[i].clientX - left - adj.x;
				var cy=e.touches[i].clientY - top  - adj.y;
				if(cx>=0 && cy>=0 && cx<=w && cy<=h){
					return true;
				}
			}
		}
		return false;
	}

	function drawLine( lastX, lastY, x, y )
	{
		if(  0<=x && x<=m_ctx.canvas.width
		  && 0<=y && y<=m_ctx.canvas.height )
		{
			m_req_regist_undo = true;
		}
		var rgba      = getCurrentColor();
		var pen_width = calcPenWidth( m_slider_out.value );
		var adj       = pen_width % 2 / 2;

		m_ctx.beginPath();
		if( lastX == x && lastY == y ){
			m_ctx.fillStyle = rgba;
			if( pen_width==1 ){
				m_ctx.fillRect( x, y, 1, 1 );
			}else{
				m_ctx.arc( x+adj, y+adj, pen_width/2, 0, 2*Math.PI, true );
				m_ctx.fill();
			}
		}else{
			m_ctx.lineWidth = pen_width;
			m_ctx.lineCap   = "round";
			m_ctx.moveTo( adj + lastX, adj + lastY );
			m_ctx.lineTo( adj + x,     adj + y     );
			m_ctx.strokeStyle = rgba;
			m_ctx.stroke();
		}
	}
	function onPointerMove(e)
	{
		//ペンが動いた時
		if( m_top_widget.style.visibility=="hidden" ){
			return;
		}
		if( m_drawing
				&& m_x > 0 && m_y > 0
				&& m_x < m_canvas.width && m_y < m_canvas.height
				&& window.getSelection() && window.getSelection().rangeCount > 0 )
		{
			window.getSelection().removeAllRanges();
		}
		if( isTouchCanvas(e) ){
			(e.preventDefault) ? e.preventDefault():e.returnValue=false;
		}
		getxy(e);
		checkButtonPress( e, m_drawing );

		/* for edge bug */
		if( m_browser == "d" ){
			var is_detect_max_touch_points =
				navigator.msMaxTouchPoints > 0 ||
				navigator.maxTouchPoints   > 0;

			if(  is_detect_max_touch_points
			  && e.pointerType == "touch"
			  && m_timestamp - m_last_timestamp > 100 )
			{
				m_drawing = false;
			}
		}

		if(m_drawing){
			drawLine( m_last_x, m_last_y, m_x, m_y );
		}
		m_last_x = m_x;
		m_last_y = m_y;
		m_last_timestamp = m_timestamp;
	}
	function calcPenWidth( v ){
		//ペンの太さを計算 線の太さ0->24 54->1
		return parseInt(24-v*23/54);
	}
	function onPointerDown(e)
	{
		//ペンが下がった時
		if(m_top_widget.style.visibility=="hidden"){
			return;
		}
		if(isTouchCanvas(e) && !m_is_show_pal && !m_is_show_dialog){
			e.preventDefault();
		}
		onMouseEnter(e);
		checkButtonPress( e, m_drawing );
		if (m_drawing){
			drawLine( m_x, m_y, m_x, m_y );
		}
		m_drawing = true;
	}
	function onPointerUp(e)
	{
		getxy(e);
		if (m_drawing && (m_last_x!=m_x || m_last_y!=m_y)){
			drawLine( m_last_x, m_last_y, m_x, m_y );
		}
		m_drawing = false;

		if( m_req_regist_undo ){
			m_req_regist_undo = false;
			TegakiUndo.registUndo( m_ctx );
		}
	}


	/***
	 * NOTE: futaba.phpにおいてbaseformにはこのような意味がある模様.
	 */
	function updateDataUri(){//canvasをdataURIに変換して格納
		var dataUri = m_canvas.toDataURL('image/png');
		dataUri=dataUri.replace(/^.*,/, '');
		var b=document.getElementById("baseform");
		if(b){b.value=dataUri;}
		return true;
	}

	function EraseDialog(e){//ダイアログを開いて消去するかどうか訊ねる
		m_is_show_dialog=true;
		document.getElementsByClassName('oezkbk')[0].style.display="block";
		document.getElementsByClassName("oezk")[0].getElementsByTagName('input')[0].focus();
	}
	function EraseDialogNo(e){//ダイアログを閉じる
		e.stopPropagation();
		document.getElementsByClassName('oezkbk')[0].style.display="none";
		m_is_show_dialog=false;
	}
	function EraseDialogYes(e){//消してダイアログを閉じる
		e.stopPropagation();
		clearCanvas();
		document.getElementsByClassName('oezkbk')[0].style.display="none";
		m_is_show_dialog=false;
	}
	function EraseDialogKey(e){//escで閉じる
		if(!m_is_show_dialog){
			return;
		}
		if(e.keyCode==27){
			EraseDialogNo(e);
		}
	}

	function setSliderCursorAndValue()
	{
		//スライダーの値を反映させる
		m_slider_cursor.style.top = ( m_slider_value - m_slider_cursor_height/2) + 'px';
		m_slider_out.value = m_slider_value;
	}
	function sliderClick(e)
	{
		if(dragging || dragged){
			return;
		}
		dragging = true;
		dragged  = true;
		sliderMouseMove(e);
		sliderMouseUp(e);
		silderTipDisp(e,true);
	}
	function sliderMouseDown(e){
		dragging = true;
		dragged  = true;
		getxy(e);
		silderTipDisp(e,false);
		return false;
	}
	function sliderMouseUp(e)
	{
		if (dragging) {
			dragging = false;
			setTimeout(function(){dragged = false;},300);
			m_slider_out.value = m_slider_value;
			var stt;
			while(stt = document.getElementsByClassName('slidertip')[0]){
				stt.parentNode.removeChild(stt);
			}
		}
	}
	function sliderMouseMove(e){//スライダーを動かす
		var top;
		if(!dragging){
			return;
		}// ドラッグ途中
		e.preventDefault();
		if(!e){
			e = window.event;
		}
		if(e.touches){
			var adj=adjTouch(e.touches[0]);
			top =e.touches[0].clientY-adj.y;
		}else{
			top = e.clientY;
		}
		var xy=getPosition(m_slider);
		// マウス座標とスライダーの位置関係で値を決める
		m_slider_value = Math.round(top - xy.y - m_slider_cursor_height);
		// スライダーからはみ出したとき
		if (m_slider_value < 0) {
			m_slider_value = 0;
		} else if (m_slider_value > parseInt(m_slider.clientHeight - m_slider_cursor_height/2)) {
			m_slider_value = parseInt(m_slider.clientHeight - m_slider_cursor_height/2);
		}
		setSliderCursorAndValue();
		var stt = document.getElementsByClassName('slidertip')[0];
		if(stt){
			var tmp = m_y - 14;
			if(tmp>45 && tmp<100){
				stt.style.top = tmp + "px";
			}
			stt.innerHTML = calcPenWidth(m_slider_value);
		}
	}
	function sliderThick(e,b){//ペンを太くor細くする
		var st=calcPenWidth(m_slider_value);
		do{
			if( b && m_slider_value>0 || !b && m_slider_value<parseInt(m_slider.clientHeight - m_slider_cursor_height/2)){
				if(b){
					m_slider_value--;
				}else{
					m_slider_value++;
				}
			}else{
				break;
			}
		}while(st==calcPenWidth(m_slider_value));
		m_slider_out.value = m_slider_value;
		setSliderCursorAndValue();
		silderTipDisp(e,true);
	}
	function silderTipDisp(e,autoremove)
	{
		//ツールチップを表示する
		var stt = document.createElement('div');
		stt.className = 'slidertip';
		stt.style.position = 'absolute';
		stt.style.left = (m_x - 37) + "px";
		stt.style.top = (m_y -14)+ "px";
		stt.innerHTML = calcPenWidth(m_slider_value);
		m_top_widget.appendChild(stt);
		if(autoremove){
			setTimeout(function(){
				if(stt){stt.parentNode.removeChild(stt);}
				}
				, 300);
		}
	}

	function palcol(x,y){//座標に応じたパレットの色を返す
		if(x==0){
			var col=["#000","#333","#666","#999","#ccc","#fff","#f00","#0f0","#00f","#ff0","#0ff","#f0f"];
			return col[y];
		}
		if(x==1){
			var col=["#111","#222","#444","#555","#777","#888","#aaa","#bbb","#ddd","#eee","#800000","#f0e0d6"];
			return col[y];
		}
		var r,g,b;
		x-=2;
		r=(parseInt(x/6)*3+parseInt(y/6)*9).toString(16);
		g=(x%6*3).toString(16);
		b=(y%6*3).toString(16);
		return "#"+r+g+b;
	}
	function onMouseEnter_PalColorCell(e)
	{
		var el = e.target;
		previewPalColor( el );
	}
	function buildPalColorCellTable( node )
	{
		//色のタグをパレットに追加
		var df=document.createDocumentFragment();
		for(var j=0;j<12;j++){
			var d0 = document.createElement('div');
			d0.className = 'oeplx';
			for(var i=0;i<20;i++){
				var d1 = document.createElement('div');
				d1.className = 'oepla';
				d1.style.backgroundColor = palcol(i,j);
				d1.addEventListener("click",selectPalColor,false);
				d1.addEventListener("mouseenter",onMouseEnter_PalColorCell,false);
				d0.appendChild(d1);
			}
			df.appendChild(d0);
		}
		node.appendChild(df);
	}
	function selectPalColor(e)
	{
		//クリックで色を決定
		var el = e.target;
		var col=window.getComputedStyle(el).backgroundColor;
		console.log( "selectPalColor" );
		setCurrentColor( col );
		showPal( false );
	}
	function onSubmitColorField(e)
	{
		//色を16進数で入力
		var val=e.target.value;
		val=val.replace(/[^0-9a-f]/ig, "");
		if(val.length<4){
			var col='#'+ (val + "000").slice(0,3);
		}else{
			var col='#'+ (val + "00").slice(0,6);
		}
		document.getElementsByClassName("oeplc")[0].style.backgroundColor = col;
		if(e.keyCode!==13){
			return;
		}
		e.preventDefault();
		setCurrentColor( col );
		showPal( false );
	}
	function oeUpdate()
	{
		if(document.getElementById('oejs')){
			updateDataUri();
		} else {
			console.log( "oejs not found." );
		}
	}

	return {
		Init:     init,
		oeUpdate: oeUpdate,
		reset:    removeEvent,
		__endof_public__: function(){}
	};
})(); /* enodof tegakiJs */


var TegakiCom = (function(){ /* module-pattern */
	'use strict';
	var m_com_selected = 'tx';

	return {
		drawSelectBar: function()
		{
			var t="";
			if( PascuaGUI.hasCanvasImageData() && PascuaGUI.hasSVG() ){
				var tx_class='MstyElemLink';
				var oe_class='MstyElemLink';
		
				if( m_com_selected == 'oe' ){
					oe_class='MstyNowSelectedLink';
				} else {
					tx_class='MstyNowSelectedLink';
				}
				t+='<span id="oebtnjm" onclick="TegakiCom.switchMode(\'tx\')" class=' + tx_class + '>文字入力<\/span>';
				t+=' ';
				t+='<span id="oebtnj"  onclick="TegakiCom.switchMode(\'oe\')" class=' + oe_class + '>手書き<\/span>';
			}
			document.getElementById("oebtnd").innerHTML=t;
		},
		switchMode: function( mode )
		{
			var d=document;
			var txvs;
			var oevs;
			var t;
			var f=false,fm=false,j=false,jm=false,v="visible",h="hidden",b="block",n="none";
			m_com_selected = mode;
			if( m_com_selected == 'oe' ){
				txvs=h;oevs=v;
				jm=true;
				if('ontouchstart' in window){
					d.body.style.setProperty('-webkit-touch-callout','none');
					d.body.style.setProperty('-webkit-user-select','none');
				}
				tegakiJs.Init("oe3",344,135);
			}else{
				txvs=v;oevs=h;
				j=true;
				if('ontouchstart' in window){
					d.body.style.setProperty('-webkit-touch-callout','default');
					d.body.style.setProperty('-webkit-user-select','text;');
				}
				document.getElementById("oe3").innerHTML='';
			}
			d.getElementById("ftxa").style.visibility=txvs;
			d.getElementById("oe3" ).style.visibility=oevs;
			if(t=d.getElementById("oebtnf" )){t.style.display=f ?b:n;}
			if(t=d.getElementById("oebtnfm")){t.style.display=fm?b:n;}
			if(t=d.getElementById("oebtnud")){t.style.display=jm?b:n;}
			TegakiCom.drawSelectBar();
		}
	}
})(); /* enodof TegakiCom */

TegakiCom.drawSelectBar();


