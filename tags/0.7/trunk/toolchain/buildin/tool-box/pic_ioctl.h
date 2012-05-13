/*
 * Copyright (C) 2007 Andreas Koch - the open7x0.org group
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * The author can be reached at andreas@open7x0.org
 *
 * The project's page is at http://www.open7x0.org
 *
 */
#ifndef __PIC_IOCTL_H
#define __PIC_IOCTL_H 1
#define PIC_MAX_IOCTL_ARGS 10
#include <stdint.h>

struct pic_ioctl_arg {
	uint32_t arg_count;
	uint32_t args[PIC_MAX_IOCTL_ARGS];
};

enum pic_start_mode {
	pstm_powerfail = 1,
	pstm_alarm = 2,
	pstm_user = 4,
	pstm_reboot = 8
};

#define PIC_IOCTL_GET_TIME 0x4
#define PIC_IOCTL_SET_TIME 0x1

#define PIC_IOCTL_GET_ALARM 0x5
#define PIC_IOCTL_SET_ALARM 0x2
#define PIC_IOCTL_CLEAR_ALARM 0x3

#define PIC_IOCTL_SHUTDOWN 0x6
#define PIC_IOCTL_REBOOT 0x7
#define PIC_IOCTL_GET_START 0x10

#define PIC_IOCTL_ENABLE_MLED 0x8
#define PIC_IOCTL_ENABLE_OLED 0x9
#define PIC_IOCTL_DISABLE_MLED 0xa
#define PIC_IOCTL_DISABLE_OLED 0xb

#define PIC_IOCTL_SET_DIM_LEDS 0xc
#define PIC_IOCTL_GET_DIM_LEDS 0xd

#define PIC_IOCTL_LINK_LEDS 0x11
#define PIC_IOCTL_UNLINK_LEDS 0x12

#define PIC_IOCTL_BLINK_LED 0x13

#define PIC_IOCTL_GET_DATA 0x187
#define PIC_IOCTL_SET_DATA 0x188

#define PIC_IOCTL_GET_VOLTAGE 0xe
#define PIC_IOCTL_SET_WD 0xf
#define PIC_IOCTL_GET_CHIP_NR 0x100

#define TOUCH_FILE "/etc/fw-version"
#endif
