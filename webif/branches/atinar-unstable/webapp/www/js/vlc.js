function init(){
	if( navigator.appName.indexOf("Microsoft Internet")==-1 )
	{
		updateVolume(0);
	}
	else if( document.readyState == 'complete' )
	{
		updateVolume(0);
	}
	else
	{
		/* Explorer loads plugins asynchronously */
		document.onreadystatechange=function() {
			if( document.readyState == 'complete' )
			{
				updateVolume(0);
			}
		}
	}
}

function getVLC(name)
{
	if (window.document[name])
	{
		return window.document[name];
	}
	if (navigator.appName.indexOf("Microsoft Internet")==-1)
	{
		if (document.embeds && document.embeds[name])
			return document.embeds[name];
	}
	else // if (navigator.appName.indexOf("Microsoft Internet")!=-1)
	{
		return document.getElementById(name);
	}
}

function updateVolume(deltaVol)
{
	var vlc = getVLC("vlc");
	vlc.audio.volume += deltaVol;
	$("volumeTextField").text(vlc.audio.volume+"%");
}

function doPlayOrPause()
{
	var vlc = getVLC("vlc");
	if( vlc.playlist.isPlaying ){
		$("#PlayPause > img").attr("src","/img/play16.png");
	} else {
		$("#PlayPause > img").attr("src","/img/pause16.png");
	}
	vlc.playlist.togglePause();
}

