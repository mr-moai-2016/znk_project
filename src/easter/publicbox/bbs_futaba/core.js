/* 0-byte avoidance */

/********************* miscellaneous *********************/
function getScroll(){//scrollë„ë÷
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

function getPosition(elm) {//getBoundingClientRectë„ë÷
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

function fragmentFromString(strHTML) {//htmlï∂éöóÒÇ©ÇÁífï–Çê∂ê¨Ç∑ÇÈ
	var frag = document.createDocumentFragment(),tmp = document.createElement('body'), child;
	tmp.innerHTML = strHTML;
	while (child = tmp.firstChild) {
		frag.appendChild(child);
	}
	return frag;
}

