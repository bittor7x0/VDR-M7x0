webif.epgGridPageInit=function(){
	this.prepareHiddenDiv();
	$('body').prepareElements();
	$('#datepicker').datepicker({
		showOn: 'button', 
		buttonImage: webif.images.calendar, 
		buttonImageOnly: true,
		showButtonPanel: false,
		dateFormat: 'yymmdd'
	});
	this.timerEditFormHandler=new TimerEditFormHandler({
		$targetContainer:this.hiddenDiv,
		prepareForm:function($form){
			TimerEditFormHandler.prototype.prepareForm.call(this,$form);
			$('<input name="epg_grid_start" type="hidden"/>').val(webif.gridStart()).appendTo($form);
		},
	});
	this.searchEditFormHandler=new SearchEditFormHandler({
		$targetContainer:this.hiddenDiv,
	});
	$('#datepicker').live('change',webif.updateGrid);
	$('#timepicker').live('change',webif.updateGrid);
	$('a.timerCreate,a.timerEdit').live('click',this.timerEditFormHandler.clickHandler);
	$('a.searchCreate').live('click',this.searchEditFormHandler.clickHandler);
	$('a.change').live('click',function (e) {
		e.preventDefault();
		webif.loadGrid($(this).attr('href'));
		return false;
	});
	$('.channel .event .title,.channel .event .more',this).live('click',function (e) {
		e.preventDefault();
		var $event = $(this).closest('.event');
		var $channelEvents=$event.closest('.channelEvents');
		var $channelContent=$channelEvents.closest('.channelContent');
		var $channelEventInfoDiv=$channelContent.find('.eventInfoDiv');
		if ($event.hasClass('open')){
			$event.removeClass('open');
			if ($channelEventInfoDiv.length>0) $channelEventInfoDiv.remove();
		} else {
			$channelEvents.find('.open').removeClass('open');
			var $eventChildren = $('.eventContent>*',$event);
			if ($eventChildren.length>0 && $channelEventInfoDiv.length==0){
				$channelEventInfoDiv=$('<div class="eventInfoDiv"></div>').append($('<div class="eventInfo"></div>'));
				$channelEvents.after($channelEventInfoDiv);
			}
			if ($channelEventInfoDiv.length>0){
				var $eventInfo=$('.eventInfo',$channelEventInfoDiv).empty().append($eventChildren.clone());
				var desc=$eventInfo.find('.desc');
				if (desc.length>0){
					$eventInfo.find('.shortdesc').remove();
				}
				$event.addClass('open');
				$channelEventInfoDiv.prepareElements();
			}
		}
		return false;
	});
};
webif.gridStart=function(){
	return $('#datepicker').val()+$('#timepicker').val().replace(':','');
};
webif.updateGrid=function(){
	var href= window.location.pathname+'?a='+webif.actions.getEpgGrid+'&start='+webif.gridStart();
	webif.loadGrid(href)
}
webif.loadGrid=function(href){
	this.openLoadingDialog();
	$.get(
		href,
		function(html) {
			webif.closeLoadingDialog();
			var $html=$(html);
			$html.filter('[id]').each(function(){
				var $node=$(this);
				var id=$node.attr('id');
				if (id=='datepickerValue'){	
					$('#datepicker').val($node.text());
				} else {
					$('#'+id).empty().append($node.contents()).prepareElements();
				}
			});
		},
		'html'
	);
};
