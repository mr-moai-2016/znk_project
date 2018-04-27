
var previewing_img_url = "";
var now_showing_idx = -1;

function changeObjBGColor( obj, color )
{
	if( obj.style && obj.style.backgroundColor ){
		obj.style.backgroundColor = '';
	} else {
		obj.style.backgroundColor = color;
	}
}
function findUrlList( query_img_url )
{
	for(var i=0;i<img_url_list.length;i++){
		if( query_img_url == img_url_list[ i ] ){
			return i;
		}
	}
	return -1;
}

/*
<script type="text/javascript">
//読み込み時の表示
window_load();

//ウィンドウサイズ変更時に更新
window.onresize = window_load;

//サイズの表示
function window_load() {
	document.winsize.sw.value = window.innerWidth;
	document.winsize.sh.value = window.innerHeight;
}
</script>
*/


function Easter_showPreview( obj, img_url, max_width, max_height )
{
	if( previewing_img_url == img_url ){
		document.getElementById("preview").innerHTML = "";
		previewing_img_url = "";
		now_showing_idx = -1;
		var ahref_previewables = document.getElementsByName("ahref_previewable");
		for(var i=0;i<ahref_previewables.length;i++){
			changeObjBGColor( ahref_previewables[i], '' );
		}
	} else {
		var img = new Image();
	
		// 640, 824 
		/***
		 * IE8対策:
		 * IE8以前ではキャッシュから画像をロードする場合にonloadイベントが発生しないという現象が
		 * たまに起こる(いつもではないようである)
		 * これを回避するには img.src = img_urlより前にonload関数を定義しておくとよいとのことである.
		 * このようにすればキャッシュからのロードであってもonloadイベントを発生させることができる.
		 */
		img.onload = function(){
			if( img.width < max_width && img.height < max_height){
				document.getElementById("preview").innerHTML = ""
						+ "Original Size:" + img.width + "x" + img.height + " This preview is original size."
						+ "<br>"
						+ "<div class=MstyComment>"
						+ "<a href=\"" + img_url + "\" target=_blank>"
						+ "<img src=\"" + img_url + "\" class=MstyImgPreview></a></div>";
			} else if( img.width >= max_width || img.height >= max_height ){
				var rate_x = max_width  / img.width;
				var rate_y = max_height / img.height;
				var mod_width  = max_width;
				var mod_height = max_height;
				if( rate_x < rate_y ){
					mod_width  = img.width  * rate_x;
					mod_height = img.height * rate_x;
				} else {
					mod_width  = img.width  * rate_y;
					mod_height = img.height * rate_y;
				}
				document.getElementById("preview").innerHTML = ""
						+ "Original Size:" + img.width + "x" + img.height + " max_width=" + max_width + " max_height=" + max_height + " This preview may be shrinked."
						+ "<br>"
						+ "<div class=MstyComment>"
						+ "<a href=\"" + img_url + "\" target=_blank>"
						+ "<img src=\"" + img_url + "\" width=" + mod_width + " height=" + mod_height + "></a></div>";
			} else {
				var authentic_key = "";
				var cache_path = "";
				var rate_nx = Math.floor( max_width * 1000 /img.width );
				var rate_rx = rate_nx % 10;
				var rate_ny = Math.floor( max_height * 1000 /img.height );
				var rate_ry = rate_ny % 10;
				var rate_ix = ( rate_nx - rate_rx ) / 10;
				var rate_iy = ( rate_ny - rate_ry ) / 10;
				var rate_str = "";
				var rate_i = 0;
				var rate_r = 0.0;
				//var class_name = "MstyImgPreviewShrinkX";
				var class_name = "MstyImgPreviewShrinkXY";
				if( rate_ix > rate_iy ){
					class_name = "MstyImgPreviewShrinkY";
					//class_name = "MstyImgPreviewShrinkXY";
					rate_i = rate_iy;
					rate_r = rate_ry;
				} else {
					//class_name = "MstyImgPreviewShrinkX";
					class_name = "MstyImgPreviewShrinkXY";
					rate_i = rate_ix;
					rate_r = rate_rx;
				}
				if( rate_i < 80 ){
					rate_str = "<font color=#ff0000>" + rate_i + "." + rate_r + "</font>";
				} else {
					rate_str = rate_i + "." + rate_r;
				}
				document.getElementById("preview").innerHTML = ""
						+ "Original Size:" + img.width + "x" + img.height + " This preview may NOT be original size."
						+ "<a href=\"" + img_url + "\" target=_blank>" + " Original Image is Here." + "</a>"
						+ "<br><div class=MstyComment>"
						+ "<a href=\"" + img_url + "\" target=_blank>"
						//+ "<img src=\"" + img_url + "\" class=" + class_name + "></a></div>";
						+ "<img src=\"" + img_url + "\" width=100%></a></div>";
			}
			previewing_img_url = img_url;
			now_showing_idx = findUrlList( previewing_img_url );
		};
		img.onerror = function(){
			document.getElementById("preview").innerHTML = "<a href=\"" + img_url + "\" target=_blank>" + "Original Image" + "</a>"
					+ "<br>Image loading error.<br>"
			previewing_img_url = img_url;
			now_showing_idx = findUrlList( previewing_img_url );
		};
		var ahref_previewables = document.getElementsByName("ahref_previewable");
		for(var i=0;i<ahref_previewables.length;i++){
			changeObjBGColor( ahref_previewables[i], '' );
		}
		changeObjBGColor( obj, '#a0a0ff' );
	
		document.getElementById("preview").innerHTML = img_url + "<br>Now loading...<br>"
		img.src = img_url;
	}
}
function Easter_showNext( obj, max_width, max_height )
{
	if( img_url_list.length ){
		var new_img_url = "";
		var show_idx = -1;
		if( previewing_img_url != "" ){
			show_idx = findUrlList( previewing_img_url );
			if( show_idx == img_url_list.length-1 ){
				show_idx = 0;
			} else {
				show_idx += 1;
			}
		} else {
			show_idx = 0;
		}
		new_img_url = img_url_list[ show_idx ];

		{
			var ahref_previewables = document.getElementsByName("ahref_previewable");
			Easter_showPreview( ahref_previewables[show_idx], new_img_url, max_width, max_height );
			document.getElementById("now_showing_path").innerHTML = new_img_url;
		}
	}

}
function Easter_showPrev( obj, max_width, max_height )
{
	if( img_url_list.length ){
		var new_img_url = "";
		var show_idx = -1;
		if( previewing_img_url != "" ){
			show_idx = findUrlList( previewing_img_url );
			if( show_idx <= 0 ){
				show_idx = img_url_list.length-1;
			} else {
				show_idx -= 1;
			}
		} else {
			show_idx = img_url_list.length-1;
		}
		new_img_url = img_url_list[ show_idx ];

		{
			var ahref_previewables = document.getElementsByName("ahref_previewable");
			Easter_showPreview( ahref_previewables[show_idx], new_img_url, max_width, max_height );
			document.getElementById("now_showing_path").innerHTML = new_img_url;
		}
	}
}
