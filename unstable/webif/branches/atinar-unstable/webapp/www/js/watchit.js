webif.clearUpdateProgramInfoTimer=function(){
	if (this.state.updateProgramInfoTimer){
		clearTimeout(this.state.updateProgramInfoTimer);
		delete this.state.updateProgramInfoTimer;
	}
}
webif.clearUpdatePlaylistItemTimer=function(){
	if (this.state.updatePlaylistItemTimer) {
		clearInterval(this.state.updatePlaylistItemTimer); 
		delete this.state.updatePlaylistItemTimer;
	}
}
webif.updateProgramInfo=function(){
	var url=this.state.url_base+'/now-next.kl1?chan='+this.state.channelNum;
	var updateProgramInfoCallback=function(responseText,textStatus,xhr){
		var jthis=$(this);
		if (textStatus=='success'){
			jthis.find('#tabs').tabs();
			var refreshHeader=xhr.getResponseHeader('Refresh').split(';',1);
			if (refreshHeader){
				var refreshTime=refreshHeader[0]*1000;
				if (refreshTime<60000) refreshTime=60000;
				webif.clearUpdateProgramInfoTimer();
				webif.state.updateProgramInfoTimer=setTimeout(
					function(){
						jthis.load(url,updateProgramInfoCallback);
					},
					refreshTime
				);
			}
		}
	}
	$('#watchItInfo').load(url,updateProgramInfoCallback);
}
webif.updatePlaylistItemInfo=function(){
	try {
		var vlc=getVlc();
		if (vlc && vlc.input.state==3) { //playing
			this.clearUpdatePlaylistItemTimer();
			var selectAudioTrack=$('#audioTrack');
			if (selectAudioTrack){
				selectAudioTrack.removeOption(/./);
				var numTracks=vlc.audio.count;
				var i;
				var trackDescription;
				numTracks=(numTracks)?parseInt(numTracks):3;
				for (i=1;i<=numTracks;i++){
					try {
						trackDescription=vlc.audio.description(i);
					}
					catch (descriptionErr){
						trackDescription=i;
					}
					selectAudioTrack.addOption(i,trackDescription,false);
				}
				selectAudioTrack.selectOptions(this.state.currentAudioTrack);
			}
		}
	} catch(audioTrackErr){
		if (console && console.log) console.log(audioTrackErr.description);
	}
};
webif.channelNumChange=function(channelNumSelector){ 
	try {
		this.state.channelNum=$(channelNumSelector).val();
		var vlc=getVlc();
		if (vlc && vlc.playlist) {
			try {
				this.clearUpdatePlaylistItemTimer();
				if (vlc.playlist.isPlaying==true) {
					vlc.playlist.stop();
					while (vlc.input.state!=0){
						if ( console && console.log )  console.log("Deteniendo");
					}
				}
			} catch (stopErr) {
				;
			}
			var itemId=this.state.playlistId[this.state.channelNum];
			if (!itemId){
				var url=this.state.url_base+':3000/'+this.state.channelNum;
				var channelName=$(':selected', channelNumSelector).text();
				if (!channelName) channelName=url;
				itemId=vlc.playlist.add(url,channelName);
				this.state.playlistId[this.state.channelNum]=itemId;
			}
			vlc.playlist.playItem(itemId);
			this.state.updatePlaylistItemTimer=setInterval(function(){
				webif.updatePlaylistItemInfo()
			},3500);
		}
		this.updateProgramInfo();
	} catch (err){
		alert(err.description);
	}
}
webif.audioTrackChange=function(audioTrackSelector){
	try {
		this.state.currentAudioTrack=$(audioTrackSelector).val();
		var vlc=getVlc();
		if (this.state.currentAudioTrack && vlc && vlc.audio){
			vlc.audio.track=parseInt(this.state.currentAudioTrack);
		}
	} catch (err){
		alert(err.description);
	}
}
webif.currentPageInit=function(){
	this.state.playlistId=new Array(); //almacena ids de items en playlist
	this.state.updateProgramInfoTimer=null; //temporizador para actualizar info
	this.state.updatePlaylistItemTimer=null; //temporizador para actualizar informacion del item reproducido
	this.state.currentAudioTrack=1;
	$('#channelNum')
		.change(function(){
			webif.channelNumChange(this)
		})
		.change();
	$('#audioTrack').change(function(){webif.audioTrackChange(this)});
	$('#PlayOrPause').click(togglePause);
	$('#Fullscreen').click(toggleFullscreen);
	$('#Mute').click(toggleMute);
	$('#VolumeDown').click(doVolumeDown);
	$('#VolumeUp').click(doVolumeUp);
}
webif.currentPageUnload=function(){
	//Desactivar timer de refresco de info
	this.clearUpdateProgramInfoTimer();
	this.clearUpdatePlaylistItemTimer();
}

