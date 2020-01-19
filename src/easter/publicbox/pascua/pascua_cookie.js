/* 0-byte avoidance */

var PascuaCookie = (function(){ /* module-pattern */
	'use strict';

	function findAryElem( ary, query_elem )
	{
		var size = ary.length;
		for( var i=0; i<size; ++i ){
			if( ary[ i ] == query_elem ){
				return true;
			}
		}
		return false;
	}

	function getDocCookieNameArray()
	{
		var cookie = document.cookie;
		if( cookie == "" ){
			return [];
		}
		var stmt_ary = cookie.split(";");
		var size     = stmt_ary.length;
		var name_ary = new Array( size-1 );
	
		/* for unsupported trim() */
		if (!String.prototype.trim) {
			String.prototype.trim = function () {
				return this.replace(/^[\s\uFEFF\xA0]+|[\s\uFEFF\xA0]+$/g, '');
			};
		}
		for( var i=0; i<size; ++i ){
			if( stmt_ary[ i ].indexOf( "=" ) > -1 ){
				var tmp = stmt_ary[ i ].substring( 0, stmt_ary[ i ].indexOf("=") );
				name_ary[ i ]  = tmp.trim();
			}
		}
		return name_ary;
	}

	/* public */
	return {
		clearCookie : function( query_name_ary, is_alive )
		{
			var name_ary = getDocCookieNameArray();
			var size     = name_ary.length;
			for( var i=0; i<size; ++i ){
				if( query_name_ary != null ){
					if( is_alive ){
						/* query_name_ary indicate alive list */
						if( findAryElem( query_name_ary, name_ary[ i ] ) ){
							continue;
						}
					} else {
						/* query_name_ary indicate death list */
						if( !findAryElem( query_name_ary, name_ary[ i ] ) ){
							continue;
						}
					}
				}
				document.cookie = name_ary[ i ] + "=; max-age=0;";
			}
		},
		setCookieOnJS : function( key, val, req_urp )
		{
			var ck_draft = key + "=" + val + "; max-age=864000; path=http://" + location.host + req_urp + ";";
			document.cookie = ck_draft;
		}
	};

})(); /* endof PascuaCookie */
