/* - mediafocusII-specific extensions to the Video4Linux interface - */

#ifndef __LINUX_VIDEODEV_PRIV_H
#define __LINUX_VIDEODEV_PRIV_H

/* - We pass additional control flags in the argument's high-word of a	- */
/* - VIDIOC<G|S>FREQ ioctl call. The low-word is by far sufficient to	- */
/* - pass the actual frequency value (in units of 1/16 MHz).		- */
/* bit 16: frequency/transponder is in high Ku-band */
#define HIBAND	(1<<16)
/* bit 17: transponder has vertical polarity */
#define VPOL	(2<<16)
#define HPOL	0
/* bit 18: transponder has mono audio carrier @ 5.8 MHz */
#define MONO580	(4<<16)
/* bit 19: transponder has mono audio carrier @ 6.6 MHz */
#define MONO660	(8<<16)
/* bit 20: transponder has mono audio carrier @ 6.65 MHz */
#define MONO665 (16<<16)
/* bit 24: transponder/satellite is at a 2nd orbital position */
#define POS2ND	(1<<24)
/* bit 25: transponder/satellite is at a 3rd orbital position */
#define POS3RD	(2<<24)
/* bit 26: transponder/satellite is at a 4th orbital position */
#define POS4TH	(4<<24)

/* - struct video_audio_sat extends the features of the original struct	- */
/* - video_audio from the V4L specification. We use it to pass special 	- */
/* - commands and data of mfII driver's audio section.			- */
struct video_audio_sat {
	int	audio;
	__u32	flags;
/* flags/commands additional to those already defined for "struct video_audio" */
#define	VIDEO_AUDIO_UNMUTE	256	/* unmute audio (to last volume level) */
#define VIDEO_AUDIO_MODE	512	/* set mode */
#define VIDEO_AUDIO_CARRIER	1024	/* set carrier */
#define VIDEO_AUDIO_MP2ON	2048	/* turn MP2 recording on */
#define VIDEO_AUDIO_MP2OFF	4096	/* turn MP2 recording off */
	__u16	volume;
	__u16	bass;
	__u16	treble;
	__u16	mode;
/* modes additional to those already defined for "struct video_audio" */
#define VIDEO_SOUND_ADR		256
#define VIDEO_SOUND_ADR_L	512
#define VIDEO_SOUND_ADR_R	1024
	int	l_carrier;		/* left stereo channel carrier frequency */
	int	r_carrier;		/* right stereo channel carrier frequency */
	ulong	mp2bufsize;		/* required number of bytes in MP2 recording buffer (get only) */
};

struct eeprom_s {
	__u16	addr;
	__u16	count;
	__u8	*data;
};

/* ioctls additional to those already defined */
#define	VIDIOCGAUDIO_SAT	_IOR('v', BASE_VIDIOCPRIVATE+0, struct video_audio_sat)
#define VIDIOCSAUDIO_SAT	_IOW('v', BASE_VIDIOCPRIVATE+1, struct video_audio_sat)
/* BTTV_FIELDNR original definition took over from the bttv package. It */
/* *MUST* have offset PRIVATE+2. */
#define BTTV_FIELDNR		_IOR('v', BASE_VIDIOCPRIVATE+2, unsigned int)
#define VIDIOCGADRDATA		_IOR('v', BASE_VIDIOCPRIVATE+3, __u8 *)
#define VIDIOCGEEPROM		_IOR('v', BASE_VIDIOCPRIVATE+4, struct eeprom_s)
#define VIDIOCSEEPROM		_IOW('v', BASE_VIDIOCPRIVATE+5, struct eeprom_s)
/* BTTV_VBISIZE original definition took over from the bttv package. It */
/* *MUST* have offset PRIVATE+8. */
#define BTTV_VBISIZE		_IOR('v', BASE_VIDIOCPRIVATE+8, int)

#endif
