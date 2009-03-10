function confMenu(){
	$('ul.sf-menu')
		.supersubs({minWidth:12,maxWidth:25,extraWidth:1})
		.superfish({pathClass:'act',pathLevels:0});
}
function confInfobox(){
	$('.infobox')
		.removeClass('infoboxCssHover')
		.hoverIntent(
			function(){
				$(this).children('.infoWrapper').fadeIn('slow');
			},
			function(){
				$(this).children('.infoWrapper').fadeOut('slow');
			}
		);
}
jQuery.fn.equalHeight = function(){
	var maxH=0;
	this.each(function(i){
		var h=$(this).height();
		if(h>maxH) maxH=h;
	});
	this.height(maxH);
	return this;
};
jQuery.fn.maxHeight = function(aHeight){
	this.each(function(){
		var jthis=$(this);
		if (jthis.height()>aHeight){
			jthis.height(aHeight);
		}
	});
	return this;
};
jQuery.fn.minHeight = function(aHeight){
	this.each(function(){
		var jthis=$(this);
		if (jthis.height()<aHeight){
			jthis.height(aHeight);
		}
	});
	return this;
};
/*
 * Only presentation oriented code.
 * See pageNumber_e in misc.h
 */
function initPage(pageNum){
	confMenu();
	switch(pageNum){
		case 0: //index
		case 1: //program
		case 2: //channels
			confInfobox();
			break;
	}
}
