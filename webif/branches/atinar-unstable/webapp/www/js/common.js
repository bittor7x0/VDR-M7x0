function confMenu(){
	$('ul.sf-menu')
		.supersubs({minWidth:12,maxWidth:25,extraWidth:1})
		.superfish({pathClass:'act',pathLevels:0});
}
function confInfobox(){
	$('.infobox')
		.removeClass('infoboxCssHover')
		.hoverIntent(
			function(){
				$(this).children('.infoWrapper').fadeIn('slow');
			},
			function(){
				$(this).children('.infoWrapper').fadeOut('slow');
			}
		);
}
jQuery.fn.equalHeight = function(){
	var maxH=0;
	this.each(function(i){
		var h=$(this).height();
		if(h>maxH) maxH=h;
	});
	this.height(maxH);
	return this;
};
jQuery.fn.maxHeight = function(aHeight){
	this.each(function(){
		var jthis=$(this);
		if (jthis.height()>aHeight){
			jthis.height(aHeight);
		}
	});
	return this;
};
jQuery.fn.minHeight = function(aHeight){
	this.each(function(){
		var jthis=$(this);
		if (jthis.height()<aHeight){
			jthis.height(aHeight);
		}
	});
	return this;
};
jQuery.fn.replaceIframe=function(callback){
	this.each(function(){
		var jthis=$(this);
		var iframe=jthis.children('iframe')[0];
		if (iframe) {
			var src=$(iframe).remove().attr('src');
			var refreshFunction=function(){
				jthis.load(src,function(responseText,textStatus,xhr){
						if (textStatus=='success'){
							if (callback){
								callback(jthis);
							}
							var refreshHeader=xhr.getResponseHeader('Refresh').split(';',1);
							if (refreshHeader){
								var refreshTime=refreshHeader[0]*1000;
								if (refreshTime<60000) refreshTime=60000;
								setTimeout(refreshFunction,refreshTime);
							}
						}
				});
			};
			refreshFunction();
		}
	});
	return this;
}
var webif={messages:{}};

function initPageWatchIt(){
	// remplazar iframe mediante ajax:
	$('#watchItInfoWrapper').replaceIframe(function(jthis){jthis.find('#tabs').tabs()});
	$('#PlayOrPause').click(togglePause);
	$('#Fullscreen').click(toggleFullscreen);
	$('#Mute').click(toggleMute);
	$('#VolumeDown').click(doVolumeDown);
	$('#VolumeUp').click(doVolumeUp);
}

function initPageBrowse(){
	$('.recFolderDetailsPending').each(function(){
		var jthis=$(this);
		var jrecFolder=jthis.parents('div.recFolder:eq(0)');
		if (jrecFolder){
			var path=jrecFolder.find('input[name=path]:eq(0)').attr('value');
			if (path){
				var hostId=jrecFolder.find('input[name=hostId]:eq(0)').attr('value');
				jthis
				.load('/recAjax.kl1',
					{	cmd: 'getInfo',
						path: path,
						hostId: hostId
					},
					function(){
						$(this).removeClass('recFolderDetailsPending');
					}
				);
			}
		}
	});
}

/*
 * Called when document.ready.
 * See pageNumber_e in misc.h
 */
function initPage(pageNum){
	confMenu();
	$('a.newWindow').click(function(e){e.preventDefault();window.open($(this).attr('href'));});
	$('.ui-state-default').hover(
		function() { $(this).addClass('ui-state-hover'); },
		function() { $(this).removeClass('ui-state-hover'); }
	);
	$('button.delete').click(function(event){
		if (!confirm(webif.messages.confirmDelete)){
			event.preventDefault();
		}
	});
	switch(pageNum){
		case 0: //index
		case 1: //program
		case 2: //channels
			confInfobox(); 	break;
		case 7: //watchit
			initPageWatchIt(); 	break;
		case 8:
			initPageBrowse(); break;
	}
}
