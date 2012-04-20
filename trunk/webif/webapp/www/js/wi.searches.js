function SearchEditFormHandler(options){
	this.init(options);
}
SearchEditFormHandler.prototype=new FormHandler();
SearchEditFormHandler.prototype.handlesForm=function($form){
	return ($form.hasClass('searchEdit'));
}
SearchEditFormHandler.prototype.prepareForm=function($form){
	//cada uno de estos campos oculta/muestra el fieldset etiquetado "<campoId>Cfg"
	$('#catsFilter,#startFilter,#durationFilter,#wdayFilter,#useAsSearchTimer,#repeatsAvoid',$form).each(function(){
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
	//campos a ocultar para cada valor de searchMode:
	var $fuzzyToleranceLabel=$('#fuzzyToleranceLabel',$form);
	$.searchModeHideMap = {
		'0':$fuzzyToleranceLabel,
		'1':$fuzzyToleranceLabel,
		'2':$fuzzyToleranceLabel,
		'3':$fuzzyToleranceLabel,
		'4':$fuzzyToleranceLabel,
		'5':$([])
	};
	var $searchMode=$('#searchMode',$form);
	$.searchModeHideMap[$searchMode.val()].hide();
	$searchMode.change(function(){
		$.each($.searchModeHideMap,function(){
			$(this).show()
		});
		$.searchModeHideMap[$(this).val()].hide();
	});

	//campos a ocultar para cada valor de channelFilter:
	$.channelFilterHideMap = {
		'0':$('#channelFilterCfg',$form),
		'1':$('#channelGroupLabel',$form),
		'2':$('#channelMinLabel,#channelMaxLabel',$form),
		'3':$([])
	};
	var $channelFilter=$('#channelFilter',$form);
	$.channelFilterHideMap[$channelFilter.val()].hide();
	$channelFilter.change(function(){
		$.each($.channelFilterHideMap,function(){
			$(this).show()
		});
		$.channelFilterHideMap[$(this).val()].hide();
	});
	//campos a ocultar para cada valor de searchAction:
	$.searchActionHideMap = {
		'0':$('#announceCfg,#switchCfg'),
		'1':$('#recordCfg,#switchCfg'),
		'2':$('#recordCfg,#announceCfg')
	};
	var $searchAction=$('#searchAction',$form);
	$.searchActionHideMap[$searchAction.val()].hide();
	$searchAction.change(function(){
		$.each($.searchActionHideMap,function(){
			$(this).show()
		});
		$.searchActionHideMap[$(this).val()].hide();
	});
	$('#confirm',$form).click(function(event){
		if( $('input[name=search]',$form).blank()==true ){
			alert(webif.messages.nothingToSearchFor);
			event.preventDefault();
		}
		if( $('#compareSet input:checkbox:checked',$form).length == 0 ){
			alert(webif.messages.nothingToCompareWith);
			event.preventDefault();
		}
	});
	$('a.searchCat',$form).click(function(event){ 
		event.preventDefault();
		var $searchCat=$(event.target);
		var $directory=$('input[name=directory]');
		$directory.val($directory.val()+$searchCat.attr('title'));
	});
	return FormHandler.prototype.prepareForm.call(this,$form);
}
SearchEditFormHandler.prototype.submitForm=function($form, event, data){
	var $button=$($form.get(0).clk);
	if ($button.hasClass('delete') && !confirm(webif.messages.searchDeleteConfirm)){
		return false;
	}
	return FormHandler.prototype.submitForm.call(this,$form, event, data);
}
	
webif.searchEditFormHandler=new SearchEditFormHandler();
webif.formHandlers.push(webif.searchEditFormHandler);
$(document).on('click','a.searchCreate, a.searchEdit',webif.searchEditFormHandler.clickHandler)

webif.searchesPageInit=function(){
	$('body').prepareElements();
	webif.pageIsReady();
}

