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
		$event.toggleClass('full');
		if ($event.hasClass('full')){
			var $eventInfo=$('.eventInfo',$event);
			if ($eventInfo.length==0){
				var $eventChildren = $('.controls,.fromTo,.shortdesc,.desc',$event);
				var $eventInfo=$('<div class="eventInfo"></div>').appendTo($event);
				$('.controls',$event).appendTo($eventInfo);
				$('.fromTo',$event).clone().appendTo($eventInfo);
				var $shortdesc=$('.shortdesc',$event);
				var $desc=$('.desc',$event);
				if ($shortdesc.length>0){
					if ($desc.text().toLowerCase().indexOf($shortdesc.text().toLowerCase())>-1){
						$shortdesc.remove();
					} else {
						$shortdesc.appendTo($eventInfo);
					}
				}
				$desc.appendTo($eventInfo);
			}
		}
	});
	$('a.timerCreate,a.timerEdit').live('click',this.timerEditFormHandler.clickHandler);
	$('a.searchCreate').live('click',this.searchEditFormHandler.clickHandler);
};

