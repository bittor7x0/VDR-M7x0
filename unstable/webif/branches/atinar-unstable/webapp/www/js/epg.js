webif.epgPageInit=function(){
	this.prepareHiddenDiv();
	$('body').prepareElements();
	this.confInfobox();
	this.timerEditFormHandler=new TimerEditFormHandler({
		$targetContainer:this.hiddenDiv,
		prepareForm:function($form){
			TimerEditFormHandler.prototype.prepareForm.call(this,$form);
			$('<input name="epg_channel_num" type="hidden"/>').val($('#channelNum').val()).appendTo($form);
		},
	});
	this.searchEditFormHandler=new SearchEditFormHandler({
		$targetContainer:this.hiddenDiv,
	});
	$('.level4 .event .titleLink').live('click',function(){
		var $event=$(this).closest('.event');
		$event.toggleClass('fullEvent');
		if ($event.hasClass('fullEvent')){
			var $eventInfo=$('.eventInfo',$event);
			if ($eventInfo.length==0){
				var $eventContent=$('.eventContent',$event);
				var $eventInfo=$('<div class="eventInfo"></div>').appendTo($event);
				$('ul.controls',$eventContent).appendTo($eventInfo);
				$('.fromTo',$eventContent).clone().appendTo($eventInfo);
				var $shortDesc=$('.shortdesc',$eventContent);
				var $desc=$('.desc',$eventContent);
				if ($shortDesc.length && (!$desc.length || $shortDesc.text().length<30)){
					$shortDesc.appendTo($eventInfo);
				} else {
					$shortDesc.remove;
				}
				$desc.appendTo($eventInfo);
			}
		}
	});
	$('a.timerCreate,a.timerEdit').live('click',this.timerEditFormHandler.clickHandler);
	$('a.searchCreate').live('click',this.searchEditFormHandler.clickHandler);
};

