$.fn.prepareElements=function(){
	$('a.command').click(function(e){
		e.preventDefault();
		var $anchor=$(e.target);
		if ($anchor.hasClass('confirm') && confirm(webif.messages.commandConfirm)!=true){
			return false;
		}
		var $resultContainer=$('#resultContainer');
		var $iframeResult=$('iframe#result',$resultContainer);
		if ($iframeResult.length==0){
			$iframeResult=$('<iframe id=\"result\"></iframe>').appendTo($resultContainer);
			$iframeResult.height($resultContainer.height()-2);
		}
		$iframeResult.attr('src',$anchor.attr('href'));
	});
	return this.defaultPrepareElements();
};
webif.commandsPageInit=function(){
	$('body').prepareElements();
}
