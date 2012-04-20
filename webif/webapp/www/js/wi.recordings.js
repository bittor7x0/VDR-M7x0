function RecEditFormHandler(options){
	this.init(options);
}
RecEditFormHandler.prototype=new FormHandler();
RecEditFormHandler.prototype.handlesForm=function($form){
	return ($form.hasClass('recEdit') || $form.hasClass('itemControls'));
}
RecEditFormHandler.prototype.submitForm=function($form, event, data){
	var $button=$($form.get(0).clk);
	if ($button.hasClass('delete') && !confirm(webif.messages.recDeleteConfirm)){
		return false;
	}
	if ($form.hasClass('itemControls')){
		var $itemDiv=$form.closest('.itemDiv');
		if ($itemDiv){
			$itemDiv.uniqueId();
			$.extend(webif.ajaxCommonData, {
				itemDivId:$itemDiv.attr('id')
			});
		}
	}
	return FormHandler.prototype.submitForm.call(this, $form, event, data);
}
webif.deleteItemDiv=function($itemDiv){
	if ($itemDiv){
		if ($itemDiv.is('td')){
			$itemDiv.closest('tr').remove();
		} else {
			var $parentItemDiv=$itemDiv.parent().closest('.itemDiv');
			$itemDiv.remove();
			if ($parentItemDiv.length==1){
				if ($('.itemDiv',$parentItemDiv).length==0){
					this.deleteItemDiv($parentItemDiv);
				}
			}
		}
	}
}

webif.recEditFormHandler=new RecEditFormHandler();
webif.formHandlers.push(webif.recEditFormHandler);
	
webif.recordingsPageInit=function(){
	$('body').prepareElements();
	webif.pageIsReady();
}
