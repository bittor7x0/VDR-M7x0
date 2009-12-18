webif={
	state:{},
	messages:{}, //i18n
	images:{},
	actions:{},  //codigos numericos de acciones
	callbacks:{},
	prepareHiddenDiv:function(){
		if (!webif.hiddenDiv){
			webif.hiddenDiv=$('<div id="hiddenDiv" class="ui-helper-hidden-accessible"></div>').appendTo($('body')).uniqueId();
		}
	}
	,
	newAppEvent:function(appEvent){
		alert("Ha pasado algo");
	}
	,
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
	,
	loadingDialogCounter:0
	,
	openLoadingDialog:function(){
		if (!this.$loading){
			this.$loading=$('<div class="ajaxLoading"/>').appendTo(this.hiddenDiv).dialog({
				modal:true,
				title:'Cargando...',
				draggable:false,
				resizable:false,
				position:['center','middle'],
				width:'150',
				height:'100',
				close: function(e,ui){
					webif.loadingDialogCounter=0;
					$(this).remove();
					delete webif.$loading;
				} 
			});
		}
		this.loadingDialogCounter++;
	}
	,
	closeLoadingDialog:function(){
		this.loadingDialogCounter--;
		if(this.loadingDialogCounter<=1){
			this.$loading.dialog('close');
		}
	}
}
$.fn.uniqueId = function(prefix){
	prefix |= '';
	var id=prefix + $.data(this.get(0));
	return this.attr('id',id);
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
$.fn.defaultPrepareElements=function(){
	$('a.newWindow',this).click(function(e){
		e.preventDefault();
		window.open($(this).attr('href'));
	});
	$('.ui-state-default').hover(
		function() { 
			$(this).addClass('ui-state-hover'); 
		},
		function() { 
			$(this).removeClass('ui-state-hover'); 
		}
	);
	return this;
};
$.fn.prepareElements=function(){
	return this.defaultPrepareElements();
};
$.fn.popMessageDialog=function(okFunction){
	var $responseDiv=$('div.response-div',this);
	var maxWidth=$(document).width()*8/10;
	$responseDiv.width('auto').css('margin','1em').css('max-width',maxWidth+'px');
	var $responseTop=$('.response-top',$responseDiv).remove();
	if (!okFunction){
		okFunction=function(){
			$(this).dialog('close');
		};
	}
	this.dialog({
		modal:true,
		title:$responseTop.html(),
		draggable:true,
		resizable:false,
		position:['center','middle'],
		height:'auto',
		width:'auto',
		maxWidth:maxWidth,
		buttons:{
			'Ok': okFunction
		},
		close: function(e,ui){
			$(this).remove();
		} 
	});
};
$(function(){
	$('ul.sf-menu')
		.supersubs({minWidth:12,maxWidth:25,extraWidth:1})
		.superfish({pathClass:'act',pathLevels:0});
	if (webif.currentPageInit){
		webif.currentPageInit();
	}
	if (webif.currentPageUnload){
		$(document).unload(webif.currentPageUnload);
	}
	$('.messageDialog').each(function(i){
		if (i==0) 
			$(this).popMessageDialog(); 
		else $(this).remove();
	});
});
