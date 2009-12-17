VLC_INPUT_STATE={
	IDLE_CLOSE:0,
	OPENING:1,
	BUFFERING:2,
	PLAYING:3,
	PAUSED:4,
	STOPPING:5,
	ENDED:6,
	ERROR:7
};
$.extend(webif,{
	getVlc: function(){
		var vlc=$("#vlc")[0];
		if (vlc && vlc.object) {
			vlc=vlc.object;
		} else if (!vlc){
			throw "No Vlc plugin installed";
		}
		return (this.getVlc=function(){
			return vlc;
		})();
	}
	,
	updateVolume: function(deltaVol){
		this.getVlc().audio.volume += deltaVol;
	}
	,
	togglePause: function($playOrPause){
		this.getVlc().playlist.togglePause();
		$('.ui-icon',$playOrPause).toggleClass('ui-icon-play').toggleClass('ui-icon-pause');
	}
	,
	toggleFullscreen: function($fullScreen){
		this.getVlc().video.toggleFullscreen();
	}
	,
	toggleMute: function($mute){
		this.getVlc().audio.toggleMute();
		$('.ui-icon',$mute).toggleClass('ui-icon-volume-on').toggleClass('ui-icon-volume-off');
	}
	,
	clearUpdateProgramInfoTimer: function(){
		if (this.state.updateProgramInfoTimer!==undefined){
			clearTimeout(this.state.updateProgramInfoTimer);
			delete this.state.updateProgramInfoTimer;
		}
	}
	,
	clearUpdatePlaylistItemTimer: function(){
		if (this.state.updatePlaylistItemTimer!==undefined) {
			clearInterval(this.state.updatePlaylistItemTimer); 
			delete this.state.updatePlaylistItemTimer;
		}
	}
	,
	updateProgramInfo: function(){
		var url='/now-next.kl1?chan='+this.state.channelNum;
		var updateProgramInfoCallback=function(responseText,textStatus,xhr){
			var $info=$(this);
			if (textStatus=='success'){
				$info.find('#tabs').tabs();
				var refreshHeader=xhr.getResponseHeader('Refresh').split(';',1);
				if (refreshHeader){
					var refreshTime=refreshHeader[0]*1000;
					if (refreshTime<60000) refreshTime=60000;
					webif.clearUpdateProgramInfoTimer();
					webif.state.updateProgramInfoTimer=setTimeout(
						function(){
							$info.load(url,updateProgramInfoCallback);
						},
						refreshTime
					);
				}
			}
		}
		$('#info').load(url,updateProgramInfoCallback);
	}
	,
	updatePlaylistItemInfo: function(){
		try {
			var vlc=this.getVlc();
			if (vlc.input.state==VLC_INPUT_STATE.PLAYING) {
				this.clearUpdatePlaylistItemTimer();
				var $audioTrack=$('#audioTrack');
				$audioTrack.removeOption(/./);
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
					$audioTrack.addOption(i,trackDescription,false);
				}
				$audioTrack.selectOptions(this.state.currentAudioTrack);
			}
		} catch(audioTrackErr){
			if (console && console.log) console.log(audioTrackErr.description);
		}
	}
	,
	channelNumChange: function($channelNum){ 
		try {
			this.state.channelNum=$channelNum.val();
			var vlc=this.getVlc();
			try {
				this.clearUpdatePlaylistItemTimer();
				if (vlc.input.state==VLC_INPUT_STATE.PLAYING) {
					vlc.playlist.stop();
					while (vlc.input.state==VLC_INPUT_STATE.STOPPING){
						if ( console && console.log )  console.log("Deteniendo");
					}
				}
			} catch (stopErr) {
				;
			}
			var itemId=this.state.playlistId[this.state.channelNum];
			if (itemId === undefined){
				var url=this.state.url_base+':3000/'+this.state.channelNum;
				var channelName=$(':selected', $channelNum).text();
				if (!channelName) channelName=url;
				itemId=vlc.playlist.add(url,channelName);
				this.state.playlistId[this.state.channelNum]=itemId;
			}
			vlc.playlist.playItem(itemId);
			this.state.updatePlaylistItemTimer=setInterval(function(){
				webif.updatePlaylistItemInfo()
			},3500);
			this.updateProgramInfo();
		} catch (err){
			alert(err.description);
		}
	}
	,
	audioTrackChange: function($audioTrack){
		try {
			this.state.currentAudioTrack=$audioTrack.val();
			this.getVlc().audio.track=parseInt(this.state.currentAudioTrack);
		} catch (err){
			alert(err.description);
		}
	}
	,
	livePageInit: function(){
		$('body').prepareElements();
		$.extend(this.state,{
			playlistId:new Array(),        //almacena ids de items en playlist
			updateProgramInfoTimer:null,   //temporizador para actualizar info
			updatePlaylistItemTimer:null,  //temporizador para actualizar informacion del item reproducido
			currentAudioTrack:1,
		});
		$('#channelNum').change(function(){webif.channelNumChange($(this));return false;}).change();
		$('#audioTrack').change(function(){webif.audioTrackChange($(this));return false;});
		$('#PlayOrPause').click(function(){webif.togglePause($(this));return false;});
		$('#Fullscreen').click(function(){webif.toggleFullscreen($(this));return false;});
		$('#Mute').click(function(){webif.toggleMute($(this));return false;});
		$('#VolumeDown').click(function(){webif.updateVolume(-5);return false;});
		$('#VolumeUp').click(function(){webif.updateVolume(5);return false;});
		$(document).unload(function(){
			//Desactivar timers de refresco de info
			this.clearUpdateProgramInfoTimer();
			this.clearUpdatePlaylistItemTimer();
		});
	}
});
