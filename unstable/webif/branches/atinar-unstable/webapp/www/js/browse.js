webif.currentPageInit=function(){
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
