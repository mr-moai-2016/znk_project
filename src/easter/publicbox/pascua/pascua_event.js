
var PascuaEventType = (function(){ /* module-pattern */
	'use strict';

	/* public */
	return {
		pointerdown : function(){
			return ( /* multiple-line statement */
				window.PointerEvent          ? 'pointerdown' :
				( 'ontouchstart' in window ) ? 'touchstart'  :
				                               'mousedown'
			);
		},
		pointerup : function(){
			return ( /* multiple-line statement */
				( window.PointerEvent )      ? 'pointerup'   :
				( 'ontouchstart' in window ) ? 'touchend'    :
				                               'mouseup'
			);
		},
		pointermove : function(){
			return ( /* multiple-line statement */
				( window.PointerEvent )      ? 'pointermove' :
				( 'ontouchstart' in window ) ? 'touchmove'   :
				                               'mousemove'
			);
		}
	};

})(); /* endof PascuaEventType */


var PascuaEventListener = (function(){ /* module-pattern */
	'use strict';

	/* public */
	return {
		gainPassiveFalseOptions: function()
		{
			var is_support_passive = false;
			var ev_listen_opts;
			try {
				var opt = Object.defineProperty({}, 'passive', {
					get: function() {
						is_support_passive = true;
					}
				});
				var listener = function(){};
				window.addEventListener(    "checkpassive", listener, opt );
				window.removeEventListener( "checkpassive", listener, opt );
			} catch ( err ) {}
		
			/* multiple type */
			if( is_support_passive ){
				ev_listen_opts = { passive: false }; /* object */
			}else{
				ev_listen_opts = false; /* boolean */
			}
			return ev_listen_opts;
		}
	}
})(); /* endof PascuaEventListener */
