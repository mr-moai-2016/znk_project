/* 0-byte avoidance */

function EQTabBlk_compose( src_no )
{
	var blk = new Object();

	var elem_fg = document.createDocumentFragment();
	var ctrl_fg = document.createDocumentFragment();

	var parent_node = document.getElementById("delcheck"+src_no).parentNode;
	var jmp = document.createElement('span');
	jmp.innerHTML="<a href='javascript:void(0);' onclick='EQTab.jumpToSrcNo(" + src_no + ");'>Jump</a>";
	jmp.className="MstyElemLink";
	ctrl_fg.appendChild(jmp);

	/* 現時点ではスマホ上で問題がある.
	var cpsel_btn = document.createElement('span');
	cpsel_btn.innerHTML="<a href='javascript:void(0);' onclick='EQTab.copySelectedToForm();'>Quote</a>";
	cpsel_btn.className="MstyElemLink";
	ctrl_fg.appendChild(cpsel_btn);
	*/

	ctrl_fg.appendChild( fragmentFromString(" &nbsp; ") );

	var move_btn = document.createElement('span');
	move_btn.innerHTML="<a href='javascript:void(0);' onclick='EQTab.switchFrame();'>Move</a>";
	move_btn.className="MstyElemLink";
	ctrl_fg.appendChild(move_btn);

	var hide_btn = document.createElement('span');
	hide_btn.innerHTML="<a href='javascript:void(0);' onclick='EQTab.hideFrame();'>Hide</a>";
	hide_btn.className="MstyElemLink";
	ctrl_fg.appendChild(hide_btn);

	var cls_btn = document.createElement('span');
	var delete_btn = "<button class='delete' onclick='EQTab.unregistAll();'></button>";
	cls_btn.innerHTML="<a href='javascript:void(0);' onclick='EQTab.unregistAll();'>" + delete_btn + " Close</a>";
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
function EQTabBlk_dispose( blk )
{
	var elem = blk.elem_;
	blk.ctrl_ = null;
	blk.elem_ = null;
}

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
	if( el ){
		var ary;
		var idx;
		/***
		 * 旧仕様を先にシークしなければならない.
		 *
		 * 「スレあきblk」は、それ以降のレスblkの集合すべてを含む.
		 * つまりそれ以降のレスblk内にあるinput or spanタグもすべて含むため、
		 * これらもすべてシークされる.
		 *
		 * このときもし先にspanをシークすると、EQTabがオープンされている場合に問題がおこる.
		 * この場合スレあきblk内のinputをスルーして、EQTab内のspanにマッチし、
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
	}
	return 0;
}

function getAncestorElementWithClass(ele,className)
{
	while(ele && ele!=document){
		if(ele.className==className){
			return ele;
		}
		ele=ele.parentNode;
	}
	return document;
}

var EQTabSearch = (function(){ /* module-pattern */
	'use strict';

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
				var min = v0[s1_idx + 1] + 1;
				var b = v1[s1_idx] + 1;
				var c = v0[s1_idx] + cost;
				if (b < min) {
					min = b;
				}
				if (c < min) {
					min = c;
				}
				v1[s1_idx + 1] = min;
			}
			var v_tmp = v0;
			v0 = v1;
			v1 = v_tmp;
		}
		return v0[s1_len];
	}

	function searchQtSrc(str,thisnum)
	{
		//search quote source
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
		var fnReg=/^[0-9]+\.(jpg|png|gif|webm|mp4|webp)$/;
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

	return {
		searchQtSrc: searchQtSrc
	};

})(); /* enodof EQTabSearch */


var EQTab = (function(){ /* module-pattern */
	'use strict';

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

	var m_blk_ary=[];//array for quote popup
	var m_current_idx = -1;
	var m_frame = null;
	var m_frame_position = "bottom";
	var m_selected_str;

	function rewriteFrame( blk )
	{
		m_frame.innerHTML = "";
		if( m_frame_position == "bottom" ){
			m_frame.innerHTML += blk.elem_.innerHTML;
			m_frame.innerHTML += "<br>";
			m_frame.innerHTML += PascuaGUI_createPagination( m_blk_ary.length, m_current_idx, 'EQTab.activateOne' );
			m_frame.innerHTML += blk.ctrl_.innerHTML;
		} else {
			m_frame.innerHTML += PascuaGUI_createPagination( m_blk_ary.length, m_current_idx, 'EQTab.activateOne' );
			m_frame.innerHTML += blk.ctrl_.innerHTML;
			m_frame.innerHTML += "<br>";
			m_frame.innerHTML += blk.elem_.innerHTML;
		}
	}
	function activateSrcNo( src_no )
	{
		var idx;
		var blk;
		for( idx=0; idx<m_blk_ary.length; ++idx ){
			blk = m_blk_ary[ idx ];
			if( blk.src_no_ == src_no ){
				m_current_idx = idx;
				rewriteFrame( blk );
				break;
			}
		}
	}
	function moveFrame( position )
	{
		m_frame_position = position;
		m_frame.style.maxWidth = "90%";
		m_frame.style.zIndex   = "100";
	
		if( position == "bottom" ){
			m_frame.style.top    = "auto";
			m_frame.style.bottom = "20px"; /* 下方には他になにかある可能性が高いので若干離す */
			//m_frame.style.left   = "10px";
			//m_frame.style.right  = "10px";
			m_frame.style.position = "fixed";
		} else if( position == "top" ){
			m_frame.style.top    = "10px";
			m_frame.style.bottom = "auto";
			//m_frame.style.left   = "10px";
			//m_frame.style.right  = "10px";
			m_frame.style.position = "fixed";
		}
		EQTab.activateOne( m_current_idx );
	}

	function open( tgt, this_src_no )
	{
		var reg = /^&gt;/;
		var src_no      = 0;
	
		if( reg.test(tgt.innerHTML) ){
			var searchstr = tgt.innerHTML.replace(reg,"");
			src_no        = EQTabSearch.searchQtSrc( searchstr, this_src_no );
			if( src_no == 0 || document.getElementsByClassName('thre').length>1 ){
				return;
			}
		} else {
			src_no = this_src_no;
		}
	
		if( m_frame == null ){
			m_frame = document.createElement('div');
			//m_frame.className = "qtd";
			//m_frame.className = "notification";
			m_frame.className = "MstyCtrlPopup";
		
			moveFrame( "bottom" );
	
			var thre = getAncestorElementWithClass(tgt,"thre");
			thre.appendChild( m_frame );
		}
	
		EQTab.registSrcNo( src_no );
		activateSrcNo( src_no );
		m_frame.style.display  = "block";
	}
	function close(e)
	{
		EQTab.unregistAll();
	}
	function unregistIdx( idx )
	{
		var blk = m_blk_ary[ idx ];
		if( blk != null ){
			EQTabBlk_dispose( blk );
			m_blk_ary.splice( idx, 1 );
			return true;
		}
		return false;
	}

	return {
		copySelectedToForm: function(p,e,noflag)
		{
			copyStrToForm( m_selected_str );
		},
		jumpToSrcNo: function( src_no )
		{
			var parent_node = document.getElementById("delcheck"+src_no).parentNode;
			if(!parent_node.offsetHeight){
				dispdel=1;
				document.getElementById("ddbut").innerHTML=dispdel?"隠す":"見る";
				ddrefl();
				reszk();
			}
			window.scroll(0,getScrollPosition(parent_node).y);
		},
		switchFrame: function()
		{
			if( m_frame_position == "bottom" ){
				moveFrame( "top" );
			} else {
				moveFrame( "bottom" );
			}
		},
		activateOne: function( query_idx )
		{
			var idx;
			var blk;
			for( idx=0; idx<m_blk_ary.length; ++idx ){
				blk = m_blk_ary[ idx ];
				if( idx == query_idx ){
					m_current_idx = idx;
					rewriteFrame( blk );
					break;
				}
			}
		},
		hideFrame: function()
		{
			m_frame.innerHTML = "<a class=MstyElemLink href='javascript:void(0);' onclick='EQTab.activateOne(" + m_current_idx + ");'>Show</a>";
		},
		registSrcNo: function( src_no )
		{
			var blk = null;
			var idx;
			for( idx=0; idx<m_blk_ary.length; ++idx ){
				blk = m_blk_ary[ idx ];
				if( blk != null ){
					if( blk.src_no_ === src_no ){
						/* already exist */
						return blk;
					}
				}
			}
			/* not found : register newly */
			blk = EQTabBlk_compose( src_no );
			m_blk_ary.push( blk );
			return blk;
		},
		unregistAll: function()
		{
			var idx;
			for( idx=0; idx<m_blk_ary.length; ++idx ){
				var blk = m_blk_ary[ idx ];
				if( blk != null ){
					EQTabBlk_dispose( blk );
				}
			}
			m_blk_ary = [];
		
			/* frameの非表示 */
			if( m_frame != null ){
				m_frame.style.display = "none";
			}
			return true;
		},

		onMouseUp: function(e)
		{
			if( window.getSelection ){
				m_selected_str = window.getSelection().toString();
				if( m_selected_str !== '' && m_selected_str !== '\n' ){
					console.log(m_selected_str);
				}
			}
		},
		popup: function(e)
		{
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
				timeout=setTimeout(function(){open(tgt,this_src_no);},latency);
				tgt.addEventListener('mouseleave', function(){clearTimeout(timeout)}, false);
			}
		}

	};

})(); /* enodof EQTab */

