#ifndef __VIDEO_DECODER_PRIV_H__
#define __VIDEO_DECODER_PRIV_H__

#ifndef VIDEO_MODE_TTX
#define	VIDEO_MODE_TTX		4
#endif
#ifndef VIDEO_MODE_RAW
#define	VIDEO_MODE_RAW		5
#endif

/* - flags additional to "struct video_decoder_capability.flags" - */
#define	VIDEO_DECODER_TTX	32	/* can decode TTX signal */
#define	VIDEO_DECODER_RAW	16	/* can deliver RAW signal */
#define	VIDEO_DECODER_VBI	128	/* can deliver VBI signal */

/* - arguments additional to ioctl DECODER_SET_INPUT - */
/* These are Philips SAA7113H specific values          */
/* according to "Mode x" numbers in SAA7113H datasheet */
#define	DECODER_INPUT_MODE0	0
#define	DECODER_INPUT_MODE1	1
#define	DECODER_INPUT_MODE2	2
#define	DECODER_INPUT_MODE3	3
#define	DECODER_INPUT_MODE6	6
#define	DECODER_INPUT_MODE7	7
#define	DECODER_INPUT_MODE8	8
#define	DECODER_INPUT_MODE9	9

/* - additional ioctls - */
#define	DECODER_GET_PICTURE	_IOR('d', 8, struct video_picture)
#define	DECODER_SET_VBI   	_IOW('d', 9, int)
#define	DECODER_SET_MODE	_IOW('d',10, int)
#define	DECODER_SET_FUSE	_IOW('d',11, int)
#define	DECODER_USE_ANTIALIAS	0x01		/* SAA7113H specific values */
#define	DECODER_USE_AMPLIFIER	0x02
#define DECODER_SET_GAIN	_IOW('d',12, int)

#endif
