function sameVal($input1,$input2){
	if ($input1.attr('type')=='checkbox'){
		return ($input1.val()==$input2.val())==$input1.attr('checked');
	} else {
		return $input1.val()==$input2.val();
	}
}
webif.settingsPageInit=function(){
	$('body').prepareElements();
	$('a.revert').live('click',function (e) {
		e.preventDefault();
		var $param=$(this).closest('.param');
		var $paramValue=$('.paramValue',$param);
		var $oldParamValue=$('.oldParamValue',$param);
		if ($paramValue.length==1 && $oldParamValue.length==1){
			if ($paramValue.attr('type')=='checkbox'){
					$paramValue.each(function(){
						var $paramValuei=$(this);
						if ($paramValuei.val()==$oldParamValue.val()){
							$paramValuei.attr('checked','checked');
						} else {
							$paramValuei.removeAttr('checked');
						}
					});
			} else {
				$paramValue.val($oldParamValue.val());
			}
			$param.removeClass('changed');
		}
		return false;
	});
	$('.paramValue').live('change',function () {
		var $param=$(this).closest('.param');
		var $paramValue=$('.paramValue',$param); //puede haber varios aparte de this
		var $oldParamValue=$('.oldParamValue',$param);
		if (sameVal($paramValue,$oldParamValue)){
			$param.removeClass('changed');
		} else {
			$param.addClass('changed');
		}
	});
}
