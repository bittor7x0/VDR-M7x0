/*
 * 
 * This source-code is licensed under GPL v2.
 * 
 * Copyright (c) 2009-2012  atinar <atinar1@hotmail.com>
 * 
 * 
*/

function FormHandler(){
}

FormHandler.prototype = {
	init:function(options){
		this.clickHandler=(function(_self){
			return function(e){
				var $anchor=$(this);
				return _self.handleClick($anchor);
			}
		})(this);
		this.responseReadyListener=(function(_self){
			return function(responseText,textStatus,XMLHttpRequest){
				var $responseContainer=$(this);
				_self.responseReady($responseContainer);
			}
		})(this);
		this.cancelButtonClickHandler=function(e){
			var $button=$(this);
			e.preventDefault();
			var $dialog = $button.closest('.js-dialog');
			if ($dialog){
				$dialog.dialog('close');
			}
			return false;
		};
		this.dialogCloseListener=function(){
			var $dialogContainer=$(this);
			$dialogContainer.dialog('destroy').remove();
		};
		$.extend(this,options);
	},
	getTargetContainer:function(){
		return webif.getHiddenDiv();
	},
	handleClick:function($anchor){
		if (this.$responseContainer){
			this.$responseContainer.empty();
		} else {
			this.$responseContainer=$('<div/>').appendTo(this.getTargetContainer()).uniqueId();
		}
		this.$responseContainer
		.attr('title',$anchor.attr('title'))
		.load(
			$anchor.attr('href'),
			this.responseReadyListener
			);
		return false;
	}
	,
	responseReady:function($responseContainer){
		$('[id^="replace_"]',$responseContainer).each(function(){
			var $nodeNew=$(this);
			var id=$nodeNew.attr('id').replace('replace_','');
			if (id){
				var $nodeOld=$('#'+id);
				if ($nodeOld){
					$nodeNew.replaceAll($nodeOld).attr('id',id).prepareElements();
				} else {
					$nodeNew.remove();
				}
			} else {
				$nodeNew.remove();
			}
		});
		var $dialogContent=$('.js-dialogContent',$responseContainer);
		if ($dialogContent.length>0){
			var $dialogContainer=$("<div/>").addClass('js-dialog').appendTo(this.getTargetContainer()).uniqueId();
			var title;
			var $dialogTitle=$('.js-dialogTitle',$responseContainer);	
			if ($dialogTitle.length){
				title=$dialogTitle.text();
				$dialogTitle.remove();
			} else {
				title=$responseContainer.attr('title');
			}
			if (title){
				$dialogContainer.attr('title',title);
			}
			$dialogContent.appendTo($dialogContainer);
			$dialogContainer.css('max-width','100%');
			var $dialogButtons=$('.js-dialogButtons',$dialogContent);
			if ($dialogButtons.length){
				$dialogButtons.append(
					$('<button class="cancel ui-state-default ui-button ui-button-text-icon-primary"></button>')
					.append('<span class="ui-button-icon-primary ui-icon ui-icon-close"></span>')
					.append('<span class="ui-button-text">'+webif.messages.cancel+'</span>')
					.click(this.cancelButtonClickHandler)
					);
			}
			$dialogContainer.prepareElements();
			$dialogContainer.dialog({
				modal: true,
				closeOnEscape: false,
				draggable: true,
				resizable: true,
				position: ['center','middle'],
				height: 'auto',
				width: 'auto',
				close: this.dialogCloseListener
			})
		} else {
			$responseContainer.popMessageDialog();
		}
		$responseContainer.empty().removeAttr('title');
	}
	,
	prepareForm:function($form){
		$form.addClass('prepared');
		return true;
	}
	,
	handlesForm:function($form){
		return false;
	}
	,
	submitForm:function($form, event, data){
		var $responseContainer=$('<div/>').appendTo(this.getTargetContainer()).uniqueId();
		if (data.dialogTitle){
			$responseContainer.attr('title',data.dialogTitle);
			delete data.dialogTitle;
		}
		data=$.extend(true,{},webif.ajaxCommonData,data);
		$form.ajaxSubmit({
			target:  $responseContainer,
			success: this.responseReadyListener,
			data: data
		});
		var $dialog = $form.closest('.js-dialog');
		if ($dialog){
			$dialog.dialog('close');
		}
		return false;
	}
}

