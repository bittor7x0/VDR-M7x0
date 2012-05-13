function TimerEditFormHandler(options){
	this.init(options);
}
TimerEditFormHandler.prototype=new FormHandler();
TimerEditFormHandler.prototype.handlesForm=function($form){
	return ($form.hasClass('timerEdit'));
}
TimerEditFormHandler.prototype.submitForm=function($form, event, data){
	var $button=$($form.get(0).clk);
	if ($button.hasClass('delete') && !confirm(webif.messages.timerDeleteConfirm)){
		return false;
	}
	return FormHandler.prototype.submitForm.call(this,$form, event, data);
}

webif.timerEditFormHandler=new TimerEditFormHandler();
webif.formHandlers.push(webif.timerEditFormHandler);
$(document).on('click','a.timerCreate,a.timerEdit',webif.timerEditFormHandler.clickHandler);
		
webif.timersPageInit=function(){
	$('body').prepareElements();
	webif.pageIsReady();
};
