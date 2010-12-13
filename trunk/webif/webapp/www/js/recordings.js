function RecEditFormHandler(options){
	FormHandler.call(this,options);
};
RecEditFormHandler.prototype=new FormHandler();
$.extend(RecEditFormHandler.prototype,{
	prepareForm:function($recEditForm){
		if (!$recEditForm || $recEditForm.length==0) return;
		$('button.delete',$recEditForm).click(function(e){
			if (confirm(webif.messages.recDeleteConfirm)!=true){
				e.preventDefault();
				return false;
			}
		});
		FormHandler.prototype.prepareForm.call(this,$recEditForm);
	}
});
webif.recFolderDetailsLoad=function($ctx){
	$('.recInfo.ajaxLoading',$ctx).each(function(){
		var $recInfo=$(this);
		var $recFolder=$recInfo.closest('.recFolder');
		if ($recFolder){
			var path=$recFolder.find('input[name=path]:eq(0)').attr('value');
			if (path){
				var hostId=$recFolder.find('input[name=hostId]:eq(0)').attr('value');
				var callback=function(html,textStatus){
					$recInfo.replaceWith(html);
				};
				var data={
					a: webif.actions.getRecInfo,
					path: path,
					hostId: hostId
				};
				$.ajax({
						url: "/recordings.kl1",
						data: data,
						type: "GET",
						dataType: 'html',
						success: callback
				});
			}
		}
	});
}

$.fn.prepareElements=function(){
	this.defaultPrepareElements();
	webif.recFolderDetailsLoad(this);
	webif.recEditFormHandler.prepareForm($('form.recControls',this));
};

webif.recordingsPageInit=function(){
	this.prepareHiddenDiv();
	this.recEditFormHandler=new RecEditFormHandler({
			$targetContainer:this.hiddenDiv,
		});
	$('body').prepareElements();
}
