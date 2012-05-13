#ifndef _LED_H
#define _LED_H

#include "pic_ioctl.h"

#define OUTER_LED 1
#define MIDDLE_LED 2
#define LINK_LED 3

int do_pic_ioctl(int cmd, struct pic_ioctl_arg *arg, int retrys);
/* Led command */
int pic_set_led(int retrys, int led, int on);
int pic_blink_led(int retrys);
int pic_set_dim_led(int retrys, int low_val, int high_val,int step_dur10ms, int flags);
int pic_get_dim_led(int retrys, int *low_val, int *high_val, int *step_dur10ms, int *flags);

#endif //_LED_H

