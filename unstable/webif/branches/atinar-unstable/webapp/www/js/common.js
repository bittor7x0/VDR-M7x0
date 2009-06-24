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
jQuery.fn.loadAndRefresh=function(url,callback){
	var loadCallback=function(responseText,textStatus,xhr){
		var jthis=$(this);
		if (textStatus=='success'){
			if (callback){
				callback(jthis);
			}
			var refreshHeader=xhr.getResponseHeader('Refresh').split(';',1);
			if (refreshHeader){
				var refreshTime=refreshHeader[0]*1000;
				if (refreshTime<60000) refreshTime=60000;
				if ($.webif.state.refreshTimeout){
					clearTimeout($.webif.state.refreshTimeout);
					delete $.webif.state.refreshTimeout;
				}
				$.webif.state.refreshTimeout=setTimeout(
					function(){
						jthis.loadAndRefresh(url,callback);
					},
					refreshTime
				);
			}
		}
	}
	this.load(url,loadCallback)
	return this;
}
jQuery.fn.replaceIframe=function(callback){
	this.each(function(){
		var jthis=$(this);
		var iframe=jthis.children('iframe')[0];
		if (iframe) {
			var url=$(iframe).remove().attr('src');
			jthis.loadAndRefresh(url,callback);
		}
	});
	return this;
}
$.webif={state:{},messages:{}};

function initPageWatchIt(){
	$.webif.state.playlistId=new Array(); //almacena ids de items en playlist
	$.webif.state.refreshTimeout=null; //temporizador para actualizar info
	var doTabs=function(jthis){
		var tabs=jthis.find('#tabs');
		tabs.tabs();
	};
	$('#channelNum').change(function(){ 
		try {
			var channelNum=$(this).val();
			var vlc=getVlc();
			if (vlc && vlc.playlist) {
				var itemId=$.webif.state.playlistId[channelNum];
				if (!itemId){
					var url=$.webif.state.url_base+':3000/'+channelNum;
					itemId=vlc.playlist.add(url);
					$.webif.state.playlistId[channelNum]=itemId;
				}
				vlc.playlist.playItem(itemId);
			}
			$('#watchItInfo').loadAndRefresh($.webif.state.url_base+'/now-next.kl1?chan='+channelNum,doTabs);
		} catch (err){
			alert(err.description);
		}
	})
	.change();
	$('#PlayOrPause').click(togglePause);
	$('#Fullscreen').click(toggleFullscreen);
	$('#Mute').click(toggleMute);
	$('#VolumeDown').click(doVolumeDown);
	$('#VolumeUp').click(doVolumeUp);
	$(document).unload(function(){
		//Desactivar timer de refresco de info
		if ($.webif.state.refreshTimeout) {
			clearTimeout($.webif.state.refreshTimeout); 
		}
		delete $.webif.state.refreshTimeout;
	});
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

function initPageSearches(){
	if ($.webif.state.currentAction==5){ //PA_EDIT
		//cada uno de estos campos oculta/muestra el fieldset etiquetado "<campoId>Cfg"
		$('#startFilter,#durationFilter,#wdayFilter,#useAsSearchTimer,#repeatsAvoid').each(function(){
			if (!this.checked) 
				$('#' + this.id + 'Cfg').hide();
			$(this).click(function(){
				var cfg=$('#' + this.id + 'Cfg');
				if (cfg) {
					//FIXME toggle() no funciona en IE 8
					if (!this.checked) cfg.hide(); else cfg.show();
				}
			});
		});
		//campos a ocultar para cada valor de channelFilter:
		$.channelFilterHideMap = {'0':$('#channelFilterCfg'),'1':$('#channelGroupLabel'),'2':$('#channelMinLabel,#channelMaxLabel'),'3':$([])};
		$.channelFilterHideMap[$('#channelFilter').val()].hide();
		$('#channelFilter').change(function(){
			$.each($.channelFilterHideMap,function(){$(this).show()});
			$.channelFilterHideMap[$(this).val()].hide();
		});
		//campos a ocultar para cada valor de searchAction:
		$.searchActionHideMap = {'0':$('#announceCfg,#switchCfg'),'1':$('#recordCfg,#switchCfg'),'2':$('#recordCfg,#announceCfg')};
		$.searchActionHideMap[$('#searchAction').val()].hide();
		$('#searchAction').change(function(){
			$.each($.searchActionHideMap,function(){$(this).show()});
			$.searchActionHideMap[$(this).val()].hide();
		});
	}
}

function initPage(){
	confMenu();
	$('a.newWindow').click(function(e){e.preventDefault();window.open($(this).attr('href'));});
	$('.ui-state-default').hover(
		function() { $(this).addClass('ui-state-hover'); },
		function() { $(this).removeClass('ui-state-hover'); }
	);
	$('button.delete').click(function(event){
		if (confirm($.webif.messages.confirmDelete)!=true){
			event.preventDefault();
		}
	});
	switch($.webif.state.currentPage){
		// See pageNumber_e in misc.h
		case 0: //index
		case 1: //program
		case 2: //channels
			confInfobox(); 	break;
		case 4: //searches
			initPageSearches(); break;
		case 8: //watchit
			initPageWatchIt(); 	break;
		case 9:
			initPageBrowse(); break;
	}
}
