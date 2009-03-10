function getVlc(){
	return $("#vlc")[0];
}

function updateVolume(deltaVol){
	var vlc = getVlc();
	if (vlc && vlc.audio){
		vlc.audio.volume += deltaVol;
		$("#VolumeText").text(vlc.audio.volume+"%");
	}
}

function doPlayOrPause(){
	var vlc = getVlc();
	if (vlc && vlc.playlist){
		vlc.playlist.togglePause();
		if( vlc.playlist.isPlaying ){
			$("#PlayOrPause > img").attr("src","/img/pause16.png");
		} else {
			$("#PlayOrPause > img").attr("src","/img/play16.png");
		}
	}
}

function doFullScreen(){
	var vlc=getVlc();
	if (vlc && vlc.video) {
		vlc.video.toggleFullscreen();
	}
}

function doToggleMute(){
	var vlc = getVlc();
	if (vlc && vlc.audio){
		vlc.audio.toggleMute();
	}
}
