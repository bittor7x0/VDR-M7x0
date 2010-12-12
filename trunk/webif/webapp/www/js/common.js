webif={
	state:{},
	messages:{}, //i18n
	images:{},
	urls:{},
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
	,
	showEpgEventInfo:function($event,$eventInfo,inGrid){
		var $eventChildren = $('.controls,.fromTo,.shortdesc,.desc',$event);
		var $title=$('.title',$event);
		var $shortdesc=$('.shortdesc',$event);
		var $desc=$('.desc',$event);
		var $controls=$('ul.controls',$event);
		if (inGrid) {
			$controls=$controls.clone();
		}
		if (webif.urls.eventSearch){
			var aUrl=webif.urls.eventSearch;
			aUrl=aUrl.replace(/\{title\}/gi,webif.URLEncode($title.text()));
			aUrl=aUrl.replace(/\{subtitle\}/gi,webif.URLEncode($shortdesc.text()));
			var $control=$('<li class="control"/>').appendTo($controls);
			var $anchor=$('<a class="webSearch newWindow button-i ui-state-default"><span class="ui-icon ui-icon-extlink">search Event</span></a>').appendTo($control);
			$anchor.attr('href',aUrl);
			$anchor.attr('title',webif.messages.webSearch);
		}
		$controls.appendTo($eventInfo);
		$('.fromTo',$event).clone().appendTo($eventInfo);
		if (inGrid){
			$title.clone().removeClass('titleLink').appendTo($eventInfo);
		}
		if ($shortdesc.length>0){
			if ($desc.text().toLowerCase().indexOf($shortdesc.text().toLowerCase())>-1){
				$shortdesc.remove();
			} else {
				if (inGrid) $shortdesc=$shortdesc.clone();
				$shortdesc.appendTo($eventInfo);
			}
		}
		if (inGrid) $desc=$desc.clone();
		$desc.appendTo($eventInfo);
		$eventInfo.prepareElements();
	}
	,
	URLEncode:function(c){
		var o='';
		var x=0;
		var r=/(^[a-zA-Z0-9_.]*)/;
		c=c.toString();
		while(x<c.length){
			var m=r.exec(c.substr(x));
			if(m!=null && m.length>1 && m[1]!=''){
				o+=m[1];
				x+=m[1].length;
			}else{
				if(c[x]==' ')
					o+='+';
				else{
					var d=c.charCodeAt(x);
					var h=d.toString(16);
					o+='%'+(h.length<2?'0':'')+h.toUpperCase();
				}
				x++;
			}
		}
		return o;
	},
	URLDecode:function(s){
		var o=s;
		var binVal,t;
		var r=/(%[^%]{2})/;
		while((m=r.exec(o))!=null && m.length>1 && m[1]!=''){
			b=parseInt(m[1].substr(1),16);
			t=String.fromCharCode(b);
			o=o.replace(m[1],t);
		}
		return o;
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
