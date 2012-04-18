webif.commandsPageInit=function(){
	$('#page').on('click','a.command', function(e){
		var $anchor=$(e.target);
		if ($anchor.hasClass('confirm') && confirm(webif.messages.commandConfirm)!=true){
			return false;
		}
		$('iframe#result').attr('src',$anchor.attr('href'));
		return false;
	});
	$('body').prepareElements();
}
