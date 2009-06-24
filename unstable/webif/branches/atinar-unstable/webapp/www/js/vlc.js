function getVlc(){
	var vlc=$("#vlc")[0];
	if (vlc && vlc.object) {
		vlc=vlc.object;
	}
	return vlc;
}
function updateVolume(deltaVol){
	var vlc = getVlc();
	if (vlc && vlc.audio){
		vlc.audio.volume += deltaVol;
		/*$("#VolumeText").text(vlc.audio.volume+"%");*/
	}
}
function doVolumeUp(event){
	event.preventDefault();
	updateVolume(5);
}
function doVolumeDown(event){
	event.preventDefault();
	updateVolume(-5);
}
function togglePause(event){
	event.preventDefault();
	var vlc = getVlc();
	if (vlc && vlc.playlist){
		vlc.playlist.togglePause();
		$('#PlayOrPause .ui-icon').toggleClass('ui-icon-play').toggleClass('ui-icon-pause');
	}
}
function toggleFullscreen(event){
	event.preventDefault();
	var vlc=getVlc();
	if (vlc && vlc.video) {
		var video=vlc.video;
		video.toggleFullscreen();
	}
}
function toggleMute(event){
	event.preventDefault();
	var vlc = getVlc();
	if (vlc && vlc.audio){
		vlc.audio.toggleMute();
		$('#Mute .ui-icon').toggleClass('ui-icon-volume-on').toggleClass('ui-icon-volume-off');
	}
}
