
function titleQandA( title, id )
{
	var msg = "";
	msg += "<u><b>" + title + "</b></u>";
	msg += "<a class=delete href='javascript:void(0);' onclick='closeQandA( \"" + id + "\" );'>X</a><br> <br>";
	return msg;
}
function textQandA( id ){
	var msg = st_func_tbls[ id ]();
	return msg;
}
function closeQandA( id ){
	var msg = "";
	document.getElementById( id ).innerHTML=msg;
}
function displayQandA( title, id ){
	var msg = "";
	msg += "<div class=content>";
	msg += "<div class=notification>";
	msg += titleQandA( title, id );
	msg += textQandA( id );
	msg += "</div>";
	msg += "</div>";
	document.getElementById( id ).innerHTML=msg;
}

