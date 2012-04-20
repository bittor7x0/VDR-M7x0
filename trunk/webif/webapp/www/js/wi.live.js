function logErrMsg(err){
	if (err.message && console && console.log){
		console.log(err.message);
	}
}
function VideoHandler(){
}
VideoHandler.prototype={
	changeVolume: function(deltaVol){
		return false;
	},
	togglePause: function(){
		return false;
	},
	toggleFullscreen: function(){
		return false;
	},
	toggleMute: function(){
		return false;
	},
	updateAudioTrackList: function(){
		return false;
	},
	changeSource: function(url, channelNum, channelName){ 
		return false;
	},
	changeAudioTrack: function(audioTrack){
		return false;
	}
};

function VideoTagWrapper(videoElement){
	this.init(videoElement);
}
VideoTagWrapper.prototype=new VideoHandler();
$.extend(VideoTagWrapper.prototype,{
	init:function(videoElement){
		this.videoElement=videoElement;
		this.inPause=false;
		this.currentSource="";
	},
	togglePause: function(){
		if (this.inPause){
			this.videoElement.src=this.currentSource;
			this.videoElement.load();
			this.videoElement.play();
		} else {
			this.videoElement.pause();
			this.videoElement.src=""; //stop loading
		}
		this.inPause=!this.inPause;
		return true;
	},
	toggleMute: function(){
		this.videoElement.muted= !this.videoElement.muted;
		return true;
	},
	changeSource: function(url, channelNum, channelName){ 
		this.currentSource=url;
		this.inPause=true;
		return this.togglePause();
	}
});

function VlcWrapper(vlc){
	this.init(vlc);
}
VlcWrapper.prototype=new VideoHandler();
$.extend(VlcWrapper.prototype,{
	inputState:{
		IDLE_CLOSE:0,
		OPENING:1,
		BUFFERING:2,
		PLAYING:3,
		PAUSED:4,
		STOPPING:5,
		ENDED:6,
		ERROR:7
	},
	state: {
		playlistId:new Array()
	},
	init:function(vlc){
		this.vlc=vlc;
	},
	changeVolume: function(deltaVol){
		this.vlc.audio.volume += deltaVol;
		return true;
	},
	togglePause: function(){
		this.vlc.playlist.togglePause();
		return true;
	},
	toggleFullscreen: function(){
		this.vlc.video.toggleFullscreen();
		return true;
	},
	toggleMute: function(){
		this.vlc.audio.toggleMute();
		return true;
	},
	updateAudioTrackList: function(){
		try {
			if (this.vlc.input.state==this.inputState.PLAYING) {
				webif.clearUpdateAudioTrackListInterval();
				var $audioTrack=$('#audioTrack');
				$audioTrack.removeOption(/./);
				var numTracks=vlc.audio.count;
				var i;
				var trackDescription;
				numTracks=(numTracks)?parseInt(numTracks):3;
				for (i=1;i<=numTracks;i++){
					try {
						trackDescription=this.vlc.audio.description(i);
					}
					catch (descriptionErr){
						if (descriptionErr.message && console && console.log) {
							console.log(descriptionErr.message);
						}
						trackDescription=i;
					}
					$audioTrack.addOption(i,trackDescription,false);
				}
				$audioTrack.selectOptions(webif.state.currentAudioTrack);
			}
		} catch(err){
			logErrMsg(err);
			return false;
		}
		return true;
	},
	changeSource: function(url, channelNum, channelName){ 
		try {
			try {
				webif.clearUpdateAudioTrackListInterval();
				if (this.vlc.input.state==this.inputState.PLAYING) {
					this.vlc.playlist.stop();
					while (this.vlc.input.state==this.inputState.STOPPING){
						if ( console && console.log )  console.log("Deteniendo");
					}
				}
			} catch (stopErr) {
				if (stopErr.message && console && console.log) console.log(stopErr.message);
			}
			var itemId=this.state.playlistId[channelNum];
			if (itemId === undefined){
				itemId=this.vlc.playlist.add(url,channelName);
				this.state.playlistId[channelNum]=itemId;
			}
			this.vlc.playlist.playItem(itemId);
			return true;
		} catch (err){
			logErrMsg(err);
			return false;
		}
	},
	changeAudioTrack: function(audioTrack){
		try {
			this.vlc.audio.track=audioTrack;
		} catch (err){
			logErrMsg(err);
		}
	}
});

$.extend(webif,{
	changeVolume: function(deltaVol){
		this.videoHandler.changeVolume(deltaVol);
	}
	,
	togglePause: function($playOrPause){
		if (this.videoHandler.togglePause()){
			$('.ui-icon',$playOrPause).toggleClass('ui-icon-play').toggleClass('ui-icon-pause');
		}
	}
	,
	toggleFullscreen: function($fullScreen){
		this.videoHandler.toggleFullscreen();
	}
	,
	toggleMute: function($mute){
		if (this.videoHandler.toggleMute()){
			$('.ui-icon',$mute).toggleClass('ui-icon-volume-on').toggleClass('ui-icon-volume-off');
		}
	}
	,
	clearUpdateProgramInfoTimer: function(){
		if (this.state.updateProgramInfoTimer!==undefined){
			window.clearTimeout(this.state.updateProgramInfoTimer);
			delete this.state.updateProgramInfoTimer;
		}
	}
	,
	clearUpdateAudioTrackListInterval: function(){
		if (this.state.updateAudioTrackListInterval!==undefined) {
			window.clearInterval(this.state.updateAudioTrackListInterval); 
			delete this.state.updateAudioTrackListInterval;
		}
	}
	,
	updateProgramInfo: function(channelNum){
		var url='/vdrwi.kl1?p='+webif.pages.epg+'&a='+webif.actions.getEpgNN+'&channelNum='+channelNum;
		var updateProgramInfoCallback=function(responseText,textStatus,xhr){
			var $info=$(this);
			if (textStatus=='success'){
				$info.find('#tabs').tabs();
				var refreshHeader=xhr.getResponseHeader('Refresh').split(';',1);
				if (refreshHeader){
					var refreshTime=refreshHeader[0]*1000;
					if (refreshTime<60000) refreshTime=60000;
					webif.clearUpdateProgramInfoTimer();
					webif.state.updateProgramInfoTimer=window.setTimeout(
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
	updateAudioTrackList: function(){
		this.videoHandler.updateAudioTrackList();
	}
	,
	channelNumChange: function($channelNum){ 
		var channelNum=$channelNum.val();
		var url=this.state.url_base+':3000/'+channelNum;
		var channelName=$(':selected', $channelNum).text();
		if (!channelName) channelName=url;
		if (this.videoHandler.changeSource(url, channelNum, channelName)){
			this.state.channelNum=channelNum;
			this.state.updateAudioTrackListInterval=window.setInterval(function(){
				webif.updateAudioTrackList()
			},3500);
		}
		this.updateProgramInfo(channelNum);
	}
	,
	audioTrackChange: function($audioTrack){
		try {
			var audioTrack=$audioTrack.val();
			if (this.videoHandler.changeAudioTrack(audioTrack)){
				this.state.currentAudioTrack=audioTrack;
			}
		} catch (err){
			logErrMsg(err);
		}
	}
	,
	livePageInit: function(){
		var $videoTag=$('video');
		if ($videoTag.length && $videoTag[0]){
			this.videoHandler = new VideoTagWrapper($videoTag[0]);
		}else {
			var $vlc=$('#vlc');
			if ($vlc.length>0) {
				var vlc=$vlc[0];
				if (vlc.object){
					vlc=vlc.object;
				}
				this.videoHandler= new VlcWrapper(vlc);
			} else {
				this.videoHandler = new VideoHandler();
				alert("No hay un gestor de video adecuado"); //TODO i18n
			}
		}
		$.extend(this.state,{
			updateProgramInfoTimer:null,   //temporizador para actualizar info
			updateAudioTrackListInterval:null,  //temporizador para actualizar informacion del item reproducido
			currentAudioTrack:1
		});
		$('#channelNum').change(function(){
			webif.channelNumChange($(this));
			return false;
		}).change();
		$('#audioTrack').change(function(){
			webif.audioTrackChange($(this));
			return false;
		});
		$('#PlayOrPause').click(function(){
			webif.togglePause($(this));
			return false;
		});
		$('#Fullscreen').click(function(){
			webif.toggleFullscreen($(this));
			return false;
		});
		$('#Mute').click(function(){
			webif.toggleMute($(this));
			return false;
		});
		$('#VolumeDown').click(function(){
			webif.changeVolume(-5);
			return false;
		});
		$('#VolumeUp').click(function(){
			webif.changeVolume(5);
			return false;
		});
		$(document).unload(function(){
			//Desactivar timers de refresco de info
			this.clearUpdateProgramInfoTimer();
			this.clearUpdateAudioTrackListInterval();
		});
		$('body').prepareElements();
		webif.pageIsReady();
	}
});
