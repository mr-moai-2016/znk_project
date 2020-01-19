/* 0-byte avoidance */

function PascuaGUI_createPagination( num, current_idx, on_click_func )
{
	var ans = "";
	if( num ){
		var one_side  = 2;
		var both_size = one_side * 2 + 1;
		var idx;
		var prev_idx  = current_idx >  0     ? current_idx - 1 : 0;
		var next_idx  = current_idx <  num-1 ? current_idx + 1 : num-1;
		var begin_idx = Math.floor( current_idx / both_size ) * both_size;
		var end_idx   = begin_idx + both_size <= num ? begin_idx + both_size : num;

		ans += "<div>";
		ans += "<a class='MstyElemLink' onclick='" + on_click_func + "(" + prev_idx + ")'>Prev</a>";
		ans += "<a class='MstyElemLink' onclick='" + on_click_func + "(" + next_idx + ")'>Next</a>";
		for( idx=begin_idx; idx<end_idx; ++idx ){
			if( idx == current_idx ){
				ans += "<span class='pagination-link is-current'>" + idx + "</span>";
			} else {
				ans += "<a class='pagination-link' onclick='" + on_click_func + "(" + idx + ")' accesskey=" + idx + ">" + idx + "</a>";
			}
		}
		ans += "</div>";
	}
	return ans;
}

var PascuaGUI = (function(){ /* module-pattern */
	'use strict';

	/* public function */
	/***
	 * has feature SVG.
	 */
	function hasSVG()
	{
		return document.implementation.hasFeature("http://www.w3.org/TR/SVG11/feature#BasicStructure", "1.1");
	}
	/***
	 * has Canvas and ImageData.
	 */
	function hasCanvasImageData()
	{
		var canvas = document.createElement( "canvas" );
		if( canvas && canvas.getContext ){
			var ctx = canvas.getContext( "2d" );
			if( ctx ){
				if( "getImageData" in ctx ){
					return true;
				}
			}
		}
		return false;
	}
	return {
		hasSVG:             hasSVG,
		hasCanvasImageData: hasCanvasImageData,
		__endof_public__: function(){}
	};

})(); /* enodof PascuaGUI */
