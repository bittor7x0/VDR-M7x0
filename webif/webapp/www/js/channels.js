webif.channelsPageInit=function(){
	$('body').prepareElements();
	/*$('tr.channelInfo').hide();*/
	this.confInfobox();
	$('tr.channel .titleLink').live('click',function(e){
		e.preventDefault();
		var $channel=$(this).closest('tr.channel');
		var $channelInfo=$channel.next('tr.channelInfo');
		$channelInfo.toggle();
		return false;
	});
};

