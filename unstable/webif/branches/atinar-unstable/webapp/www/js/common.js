webif={
	state:{},
	messages:{},
	confMenu:function(){
		$('ul.sf-menu')
			.supersubs({minWidth:12,maxWidth:25,extraWidth:1})
			.superfish({pathClass:'act',pathLevels:0});
	},
	confInfobox:function(){
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
}
$.fn.equalHeight = function(){
	var maxH=0;
	this.each(function(i){
		var h=$(this).height();
		if(h>maxH) maxH=h;
	});
	this.height(maxH);
	return this;
};
$.fn.maxHeight = function(aHeight){
	this.each(function(){
		var jthis=$(this);
		if (jthis.height()>aHeight){
			jthis.height(aHeight);
		}
	});
	return this;
};
$.fn.minHeight = function(aHeight){
	this.each(function(){
		var jthis=$(this);
		if (jthis.height()<aHeight){
			jthis.height(aHeight);
		}
	});
	return this;
};
$.fn.blank = function(){
	return $.trim($(this).val()).length==0;
} 
webif.defaultPageInit=function(){
	this.confMenu();
	$('a.newWindow').click(function(e){e.preventDefault();window.open($(this).attr('href'));});
	$('.ui-state-default').hover(
		function() { $(this).addClass('ui-state-hover'); },
		function() { $(this).removeClass('ui-state-hover'); }
	);
	$('button.delete').click(function(event){
		if (confirm(webif.messages.confirmDelete)!=true){
			event.preventDefault();
		}
	});
	if (this.currentPageInit){
		this.currentPageInit();
	}
	switch(webif.state.currentPage){
		// See pageNumber_e in misc.h
		case 0: //index
		case 1: //program
		case 2: //channels
			this.confInfobox(); 	break;
	}
	if (webif.currentPageUnload){
		$(document).unload(webif.currentPageUnload);
	}
}
