webif.currentPageInit=function(){
	if (webif.state.currentAction==5){ //PA_EDIT
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
		$('#confirm').click(function(event){
			if( $('input[name=search]').blank()==true ){
				alert(webif.messages.searches.nothingToSearchFor);
				event.preventDefault();
			}
			if( $('#compareSet input:checkbox:checked').length == 0 ){
				alert(webif.messages.searches.nothingToCompareWith);
				event.preventDefault();
			}
		});
	}
}

