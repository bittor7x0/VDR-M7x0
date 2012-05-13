webif.settingsPageInit=function(){
	$('body').prepareElements();
	$('#page').on('click','a.revert',function() {
		var $param=$(this).closest('.param');
		var $paramValue=$('.paramValue',$param);
		var oldValue=$('.oldParamValue',$param).val();
		$paramValue.each(function(){
			var $paramValuei=$(this);
			if ($paramValuei.attr('type')=='radio'){
				if ($paramValuei.val()==oldValue){
					$paramValuei.attr('checked', true);
				}
			} else {
				$paramValuei.val(oldValue);
			}
		})
		$param.removeClass('changed');
		$paramValue.trigger('change.webif')
		return false;
	}).on('change','.paramValue',function() {
		var $param=$(this).closest('.param');
		var value=$(this).val();
		var oldValue=$('.oldParamValue',$param).val();
		if (value==oldValue){
			$param.removeClass('changed');
		} else {
			$param.addClass('changed');
		}
		return true;
	});
	webif.pageIsReady();
}
