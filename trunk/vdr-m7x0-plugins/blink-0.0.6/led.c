
#include <sys/ioctl.h>
#include <vdr/plugin.h>
#include "pic_ioctl.h"
#include "led.h"

#ifdef DEBUG
	#define debug(x...) dsyslog("blink: " x);
	#define error(x...) esyslog("blink: " x);
#else
	#define debug(x...) ;
	#define error(x...) esyslog("blink: " x);
#endif

#define PICDEVNAME "/dev/pic"

int do_pic_ioctl(int cmd, struct pic_ioctl_arg *arg, int retrys)
{
	int fd;
	int r;
	int trys;

	fd = open(PICDEVNAME, O_RDWR);
	if (fd < 0) {
		error("Cannot open pic device");
		return -1;
	}

	trys = 0;
	do {
		r = ioctl(fd, cmd, arg);
		if (!r)
			break;

		error("Pic command %x failed %d try %d", cmd, r, trys);
		cCondWait::SleepMs(1000);//1 seconds
		trys ++;
	} while (!retrys || trys < retrys);

	close(fd);
	return r;
}

/* Led command */
int pic_set_led(int retrys, int led, int on)
{
	struct pic_ioctl_arg cmd_arg;
	int cmd;

	if (led == OUTER_LED) {
		cmd = on ? PIC_IOCTL_ENABLE_OLED : PIC_IOCTL_DISABLE_OLED;
	} else if (led == MIDDLE_LED) {
		cmd = on ? PIC_IOCTL_ENABLE_MLED : PIC_IOCTL_DISABLE_MLED;
	} else {
		cmd = on ? PIC_IOCTL_LINK_LEDS : PIC_IOCTL_UNLINK_LEDS;
	}

	memset(&cmd_arg, 0, sizeof(cmd_arg));
	return do_pic_ioctl(cmd, &cmd_arg, retrys);
}

int pic_blink_led(int retrys)
{
	struct pic_ioctl_arg cmd_arg;
	memset(&cmd_arg, 0, sizeof(cmd_arg));
	return do_pic_ioctl(PIC_IOCTL_BLINK_LED,&cmd_arg, retrys);
}

int pic_set_dim_led(int retrys, int low_val, int high_val,int step_dur10ms, int flags)
{
	struct pic_ioctl_arg cmd_arg;
	memset(&cmd_arg, 0, sizeof(cmd_arg));
	cmd_arg.arg_count = 4;
	cmd_arg.args[0] = low_val;
	cmd_arg.args[1] = high_val;
	cmd_arg.args[2] = step_dur10ms;
	cmd_arg.args[3] = flags;
	return do_pic_ioctl(PIC_IOCTL_SET_DIM_LEDS,&cmd_arg, retrys);
}

int pic_get_dim_led(int retrys, int *low_val, int *high_val, int *step_dur10ms, int *flags)
{
	struct pic_ioctl_arg cmd_arg;

	memset(&cmd_arg, 0, sizeof(cmd_arg));
	if (do_pic_ioctl(PIC_IOCTL_GET_DIM_LEDS,&cmd_arg, retrys) ||
			cmd_arg.arg_count != 4)
		return -1;

	*low_val = cmd_arg.args[0];
	*high_val = cmd_arg.args[1];
	*step_dur10ms = cmd_arg.args[2];
	*flags = cmd_arg.args[3];
	return 0;
}
