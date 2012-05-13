(function(){
	var widthThreshold;
	webif.compactEvents=function(ctx){
		$('.eventDiv',ctx).each(function(){
			var $eventDiv=$(this);
			if (!widthThreshold){
				var $span=$('<span>a</span>').css('color','transparent').appendTo($eventDiv);
				widthThreshold=$span.width()*7;
				$span.remove();
			}
			if ($eventDiv.width()>widthThreshold){
				$eventDiv.removeClass('compact');
			} else {
				$eventDiv.addClass('compact');
			}
		});
	}
})();

(function(){
	var proxied=$.fn.prepareElements;
	$.fn.prepareElements=function(ctx){
		proxied.apply(this, arguments);
		webif.compactEvents(ctx);
		return this;
	}
})();

webif.epgGridPageInit=function(){
	$(window).resize(
		(function(){
			var timeoutId;
			var compactGridEvents=function(){
				webif.compactEvents($(".grid"))
			};
			return function(){
				if (timeoutId){
					window.clearTimeout(timeoutId);
				}
				timeoutId=window.setTimeout(compactGridEvents,500);
			}
		})()
		);
	$('#datepicker').datepicker({
		showOn: 'button', 
		buttonImage: webif.images.calendar, 
		buttonImageOnly: true,
		showButtonPanel: false,
		dateFormat: 'yymmdd'
	});
	$('#datepicker').on('change',webif.updateGrid);
	$('#timepicker').on('change',webif.updateGrid);
	$('body').prepareElements();
	webif.pageIsReady();
};
webif.updateGrid=function(){
	var start=$('#datepicker').val()+$('#timepicker').val().replace(':','');
	var href= window.location.pathname+'?start='+start;
	window.location=href;
};
