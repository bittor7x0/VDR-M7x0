function getVlc(){
	return $("#vlc")[0];
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
		vlc.video.toggleFullscreen();
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
