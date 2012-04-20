/*
 * 
 * This source-code is licensed under GPL v2.
 * 
 * Copyright (c) 2009-2012  atinar <atinar1@hotmail.com>
 * 
 * 
*/

$.uniqueId = (function(prefix){
	var uid=0;
	return function(){
		uid++;
		return prefix + uid;
	}
})('wi');


var webif={
	conf:{
		ajaxDisabled: false,
		popupsDisabled: false
	},
	state:{},
	messages:{}, 
	images:{},
	urls:{},
	pages:{},
	actions:{},
	callbacks:{},
	timers:{},
	ajaxCommonData:{
		ajax:true
	},
	formHandlers:new Array(),
	prepareForms:function(ctx){
		var l=this.formHandlers.length;
		if (l > 0){
			$('form',ctx).each(function(){
				var $form=$(this);
				if (!$form.hasClass('prepared')){
					for (var i=0; i<l; i++){
						if (webif.formHandlers[i].handlesForm($form)){
							webif.formHandlers[i].prepareForm($form);
							break;
						}
					}
				}
			});
		}
	},
	submitForm:function($form, event, data){
		var l=this.formHandlers.length
		if (l > 0){
			for (var i=0; i<l; i++){
				if (webif.formHandlers[i].handlesForm($form)){
					return webif.formHandlers[i].submitForm($form, event, data);
				}
			}
		}
		return true;
	},
	getHiddenDiv:function(){
		if (!webif.hiddenDiv){
			webif.hiddenDiv=$('<div id="hiddenDiv" class="ui-helper-hidden-accessible"></div>').appendTo($('body')).uniqueId();
		}
		return webif.hiddenDiv;
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
		var t;
		var r=/(%[^%]{2})/;
		while((m=r.exec(o))!=null && m.length>1 && m[1]!=''){
			b=parseInt(m[1].substr(1),16);
			t=String.fromCharCode(b);
			o=o.replace(m[1],t);
		}
		return o;
	},
	setCssTheme:function(newCssTheme, cookie){
		var $body=$('body');
		if (webif.conf.currentCssTheme){
			$body.removeClass('theme'+webif.conf.currentCssTheme);
		}
		if (newCssTheme){
			$body.addClass('theme'+newCssTheme);
			this.conf.currentCssTheme=newCssTheme;
			if (cookie){
				$.cookie('css_theme',newCssTheme);
			}
		}
	},
	loadDeferred:function($ctx){
		var $deferred = ($ctx.hasClass('js-deferred')) ? $ctx : $('.js-deferred', $ctx);
		$deferred.filter(':visible').each(function(){
			var $deferred=$(this);
			var $infoSet=$deferred.closest('.infoSet');
			if ($infoSet){
				$deferred.removeClass('js-deferred').addClass('ajaxLoading');
				var remoteInfoUrl=$('a.remoteInfoUrl',$infoSet).attr('href');
				if (remoteInfoUrl){
					var ajaxParams={
						url: remoteInfoUrl,
						data:{},
						type: "GET",
						context: $deferred,
						dataType: 'html',
						success: function(html){
							var $newItem=$(html);
							$(this).replaceWith($newItem);
							$newItem.prepareElements();
						},
						error: function(jqXHR, textStatus, errorThrown){
							$(this).removeClass('ajaxLoading').addClass('ui-state-error')
							.append($('<p>').text(errorThrown));
						},
						complete: function(){
							$(this).removeClass('ajaxLoading');
						}
					};
					$.ajax(ajaxParams);
				}
			}
		});
	},
	afterConf: function(){
		if (!webif.conf.ajaxDisabled){
			$(document)
			.on('click', 'form button[type="submit"]', function(event){
				var $button=$(this);
				var $form=$button.closest('form');
				if ($form.length){
					$form[0].clk=this; //jquery.form injects clk's name:value
				}
			})
			.on('submit', 'form', function(event){
				var $button, data = {};
				if (!this.clk){
					$button=$('.confirm[type="submit"]');
					if ($button.length){
						this.clk=$button[0];
					}
				} 
				if (this.clk){
					$button=$(this.clk);
				}
				if ($button && $button.length) {
					if ($button.hasClass('js-no-ajax')){
						return true;
					}
					data.dialogTitle=$button.attr('title');
					if (!data.dialogTitle){
						data.dialogTitle=$button.text();
					}
				}
				return webif.submitForm($(this), event, data);
			});
		}
	},
	pageIsReady:function(){
		$('body').removeClass('js-notReady');
	}
}

$.fn.uniqueId = function(){
	this.each(function(){
		var $this = $(this);
		var id = $this.attr('id');
		if (!id){
			id=$.uniqueId();
			$this.attr('id',id);
		}
	});
	return this;
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
		var $this=$(this);
		if ($this.height()>aHeight){
			$this.height(aHeight);
		}
	});
	return this;
};
$.fn.minHeight = function(aHeight){
	this.each(function(){
		var $this=$(this);
		if ($this.height()<aHeight){
			$this.height(aHeight);
		}
	});
	return this;
};
$.fn.blank = function(){
	return $.trim($(this).val()).length==0;
}
$.fn.prepareElements=function(){
	webif.prepareForms(this);
	$('.infoExtra', this).hide();
	return this;
};
$.fn.popMessageDialog=function(okFunction){
	var $messagesDiv=$('.messagesDiv',this);
	if ($messagesDiv.length==1){
		var maxWidth=$(document).width()*8/10;
		$messagesDiv.width('auto').css('margin','1em').css('max-width',maxWidth+'px');
		var $messagesHead=$('.messagesHead',$messagesDiv).remove();
		if (!okFunction){
			okFunction=function(){
				$(this).dialog('close');
			};
		}
		$messagesDiv.dialog({
			modal:true,
			title:$messagesHead.html(),
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
	}
};

$.ajaxSetup({
	data: {
		ajax:true
	}
});

$(document)
	.on('click','a.newWindow',function(){
		window.open($(this).attr('href'));
		return false;
	})
	.on('mouseenter','.ui-state-default',function(){ 
		$(this).addClass('ui-state-hover'); 
	})
	.on('mouseleave','.ui-state-default',function(){ 
		$(this).removeClass('ui-state-hover'); 
	})
	.on('click', '.infoToggler', function(){
		var $infoSet=$(this).closest('.infoSet');
		var $infoExtra=$('.infoExtra:first', $infoSet);
		if ($infoExtra.toggle().is(':visible')){
			webif.loadDeferred($infoExtra);
		}
		$(this).parent().toggleClass('expanded');
		return false;
	})
	.on('click','.infoInjector a',function (e) {
		//if there is an anchor, follow normal link
		e.stopPropagation();
		return true;
	})
	.on('click','.infoInjector',function (e) {
		var $infoSet, $infoContext, $infoTarget, $infoSrc, $infoExtra;
		e.preventDefault();
		$infoSet = $(this).closest('.infoSet');
		if ($infoSet.length){ 
			$infoContext=$infoSet.closest('.infoContext');
			if ($infoContext.length){
				$infoTarget=$infoContext.find('.infoTarget');
				if ($infoTarget.length){
					if ($infoSet.hasClass('infoSrc')){
						//just toggle visibility
						$infoSet.toggleClass('expanded');
						$infoTarget.toggle();
						return false;
					} else {
						//close current src
						$infoSrc=$infoContext.find('.infoSrc');
						if ($infoSrc.length){
							$infoExtra = $('.infoExtra', $infoTarget);
							if ($infoExtra.length){
								$infoSrc.removeClass('infoSrc expanded').append($infoExtra.hide());
							}
						}
						//show new src
						$infoExtra = $('.infoExtra', $infoSet);
						$infoExtra.appendTo($infoTarget).show();
						$infoSet.addClass('infoSrc expanded');
						$infoTarget.show();
						return false;
					}
				}
			}
		}
		return true;
	})
	.ready(function(){
		$('ul.sf-menu')
		.supersubs({
			minWidth:12,
			maxWidth:25,
			extraWidth:1
		})
		.superfish({
			pathClass:'act',
			pathLevels:0
		});
		if (!webif.conf.popupsDisabled){
			$('body').popMessageDialog(); 
		}
		$('#loading')
		.dialog({
			modal:true,
			draggable:false,
			resizable:false,
			position:['center','middle'],
			width:'150',
			height:'100',
			autoOpen:false
		})
		.ajaxStart(function(){
			$(this).dialog('open');
		})
		.ajaxStop(function(){
			$(this).dialog('close');
		});
	});

