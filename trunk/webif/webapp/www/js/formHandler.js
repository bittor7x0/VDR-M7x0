/*
 * 
 * This source-code is licensed under GPL v2.
 * 
 * Copyright (c) 2009-2010  atinar <atinar1@hotmail.com>
 * 
 * 
*/

function FormHandler(options){
	this.init(options);
}
FormHandler.prototype = {
	init:function(options){
		$.extend(this,options);
		this.clickHandler=
			(function(_self){
				return function(e){
					var $anchor=$(this);
					return _self.handleClick(e,$anchor);
				}
			})(this);
		this.responseReadyListener=
			(function(_self){
				return function(responseText,textStatus,XMLHttpRequest){
					_self.responseReady(responseText,textStatus,XMLHttpRequest);
				}
			})(this);
		this.beforeSubmitFilter=
			(function(_self){
				return function(formData, $form, ops){
					var $button=$($form.get(0).clk);
					_self.$responseContainer.attr('title',$button.attr('title'));
					if (_self.$formContainer){
						_self.$formContainer.dialog('close'); //elimina .formContainer tambien
					}
					webif.openLoadingDialog();
					return true;
				};
			})(this);
		this.cancelButtonClickHandler=
			(function(_self){
				return function(e){
						e.preventDefault();
						_self.$formContainer.dialog('close');
						return false;
				};
			})(this);
		this.formContainerDialogCloseListener=
			(function(formHandler){
				var _self=formHandler;
				return function(e,ui){
					_self.$formContainer.remove();
					delete _self.$formContainer;
				}
			})(this);
	}
	,
	handleClick:function(e,$anchor){
		// Click event handler ligado a un anchor 'a' que carga, 
		// via ajax, el recurso al que apunta 'a.href' 
		// dentro de un dialogo modal.
		e.preventDefault();
		webif.openLoadingDialog();
		if (this.$responseContainer){
			this.$responseContainer.empty();
		} else {
			this.$responseContainer=$('<div/>').appendTo(this.$targetContainer).uniqueId();
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
	responseReady:function(responseText,textStatus,$form){
		// Handler llamado por $.load() tras completar la llamada ajax
		$('[id^="replace_"]',this.$responseContainer).each(function(){
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
		webif.closeLoadingDialog();
		var $form=$('form',this.$responseContainer);
		if ($form.length){
			this.$formContainer=$('<div/>').appendTo(this.$targetContainer).append(this.$responseContainer.children()).uniqueId();
			this.prepareFormContainer();
			this.prepareForm($form);
			this.$formContainer.dialog({
				modal: true,
				closeOnEscape: false,
				draggable: true,
				resizable: true,
				position: ['center','middle'],
				height: 'auto',
				width: 'auto',
				close: this.formContainerDialogCloseListener,
			})
		} else {
			$('<div></div>').appendTo(this.$targetContainer).append(this.$responseContainer.children()).popMessageDialog();
		}
		this.$responseContainer.empty().removeAttr('title');
	}
	,
	prepareFormContainer:function(){
		var $formTitle=$('.formTitle',this.$formContainer);	
		if ($formTitle.length){
			this.$formContainer.attr('title',$formTitle.text())
		}
		this.fixWidth();
		this.addCancelButton();
		this.$formContainer.prepareElements();
	}
	,
	prepareForm:function($form){
		if (!this.$responseContainer){
			this.$responseContainer=$('<div/>').appendTo(this.$targetContainer).uniqueId();
		}
		$form
			.ajaxForm({
				target: '#' + this.$responseContainer.attr('id'),
				beforeSubmit: this.beforeSubmitFilter,
				success: this.responseReadyListener,
			});
	}
	,
	fixWidth:function(){
		$('.formContent',this.$formContainer).each(function(){
				var $formContent=$(this);
				$formContent.css('max-width','100%');
			});
	}
	,
	addCancelButton:function(){
		$('.ajaxButtonsHolder',this.$formContainer).append(
			$('<button class="cancel ui-state-default button-i-t"></button>')
				.append(
					$('<div></div')
						.append('<span class="ui-icon ui-icon-close"> </span>'+webif.messages.cancel)
				)
				.click(this.cancelButtonClickHandler)
		);
	}
}

