function TimerEditFormHandler(options){
	FormHandler.call(this,options);
};
TimerEditFormHandler.prototype=new FormHandler();
$.extend(TimerEditFormHandler.prototype,{
	prepareForm:function($form){
		if (!$form || $form.length==0) return;
		$('button.delete',$form).click(function(e){
			if (confirm(webif.messages.timerDeleteConfirm)!=true){
				e.preventDefault();
				return false;
			}
		});
		FormHandler.prototype.prepareForm.call(this,$form);
	}
});

webif.timersPageInit=function(){
	this.prepareHiddenDiv();
	$('body').prepareElements();
	var $timerEditForm=$('form#timerEdit');
	if (!this.timerEditFormHandler){
		this.timerEditFormHandler=new TimerEditFormHandler({
			$targetContainer:this.hiddenDiv,
		});
		if ($timerEditForm.length){
			this.timerEditFormHandler.prepareForm($timerEditForm);
		}
	}
	$('a.timerNew,a.timerEdit').live('click',this.timerEditFormHandler.clickHandler);
};
