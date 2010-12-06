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
 * $Id: pic_tool.c 208 2007-10-03 16:01:59Z andreas $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/mtd/mtd.h>
#include "pic_ioctl.h"
#include "tools.h"

#define PICDEVNAME "/dev/pic"
#define MTDCONFBLOCKDEV "/dev/mtdblock3"
#define MTDCONFCHARDEV "/dev/mtd3"
#define OUTER_LED 1
#define MIDDLE_LED 2
#define LINK_LED 3
#define SECTORSIZE (64 *1024)
#define BOOTCOUNTSECTOR 3
#define FLASH_CONTENT_BACKUP1 "/rw-flash/etc/flash_content"
#define FLASH_CONTENT_BACKUP2 "/rw-flash/flash_content"
#define FLASH_CONTENT_SIZE 512
#define MTDCONFSIZE (SECTORSIZE * 4)
struct flash_conf {
	int fd_char;
	int fd_block;
	mtd_info_t info;
	uint8_t *buf;
};

static int do_pic_ioctl(int cmd, struct pic_ioctl_arg *arg, int retrys)
{
	int fd;
	int r;
	int trys;

	fd = open(PICDEVNAME, O_RDWR);
	if (fd < 0) {
		ERROR_LOG("Cannot open pic device");
		return -1;
	}

	trys = 0;
	do {
		r = ioctl(fd, cmd, arg);
		if (!r)
			break;

		ERROR_LOG("Pic command %x failed %d try %d", cmd, r, trys);
		usleep(10000);
		trys ++;
	} while (!retrys || trys < retrys);

	close(fd);
	return r;
}

static int open_mtd_char(struct flash_conf *conf)
{
	conf->fd_char = open(MTDCONFCHARDEV,O_RDWR);
	if (conf->fd_char < 0) {
		ERROR_LOG("Cannot open flash device '" MTDCONFCHARDEV "' ");
		return -1;
	}

	if (ioctl(conf->fd_char, MEMGETINFO, &conf->info)) {
		ERROR_LOG("Cannot get flash info for '" MTDCONFCHARDEV "' ");
		close(conf->fd_char);
		conf->fd_char = -1;
		return -1;
	}
	return 0;
}

static int open_mtd_block(struct flash_conf *conf)
{

	conf->fd_block = open(MTDCONFBLOCKDEV,O_RDONLY);
	if (conf->fd_block < 0) {
		ERROR_LOG("Cannot open flash device '" MTDCONFBLOCKDEV "' ");
		return -1;
	}

	conf->info.size = MTDCONFSIZE;
	conf->buf = mmap(NULL, conf->info.size, PROT_READ, MAP_SHARED,
		conf->fd_block, 0);
	if (conf->buf == MAP_FAILED) {
		ERROR_LOG("Cannot mmap block flash device '" MTDCONFBLOCKDEV "' ");
		close(conf->fd_block);
		conf->fd_block = - 1;
		conf->buf = NULL;
		return -1;
	}
	return 0;
}

static void close_mtd(struct flash_conf *conf)
{
	if (conf->fd_char >= 0) {
		close(conf->fd_char);
	}

	if (conf->buf) {
		munmap(conf->buf, conf->info.size);
	}

	if (conf->fd_block >= 0) {
		close(conf->fd_block);
	}
}

static int erase_mtd_sector(struct flash_conf *conf, int sector)
{
	erase_info_t erase;
	if (conf->info.erasesize != SECTORSIZE) {
		ERROR_LOG_NE("Erase size %d of flash mismatch", conf->info.erasesize);
		return -1;
	}

	erase.start = sector * SECTORSIZE;
	erase.length = SECTORSIZE;
	if (erase.start + erase.length > conf->info.size) {
		ERROR_LOG_NE("Erase sector %d not in flash", sector);
		return -1;
	}

	if (ioctl(conf->fd_char, MEMERASE, &erase)) {
		ERROR_LOG("Cannot erase sector %d",sector);
		return -1;
	}
	return 0;
}

static int read_mtd_conf(struct flash_conf *conf, int *conf_sector,
	int *conf_size, int *offset_fc)
{
	uint16_t filecount;
	int offset;
	int offset_start;
	int offset_filestart;
	int filecount_s;

	offset = 0;
	*conf_sector = 0;
	*conf_size = 0;
	*offset_fc = -1;
	filecount = (conf->buf[0] << 8) | conf->buf[1];
	if (filecount >= 0x8000) {
		offset = SECTORSIZE;
		*conf_sector = 1;
		filecount = (conf->buf[SECTORSIZE] << 8) | conf->buf[SECTORSIZE + 1];
		if (filecount >= 0x8000) {
			ERROR_LOG_NE("No configuration files found in flash");
			return -1;
		}
	}

	offset_start = offset;
	offset +=2;
	filecount_s = filecount;
	for (;filecount && offset - offset_start < SECTORSIZE; filecount--) {
		int file_size;
		int file_name_len;
		uint8_t *p;
		offset_filestart = offset;

		file_size = (conf->buf[offset] << 8) | conf->buf[offset + 1];
		offset += 2;

		file_name_len = strnlen((const char *)(conf->buf + offset),
			SECTORSIZE - (offset - offset_start) - 1);
		p = memrchr(conf->buf + offset, '/',file_name_len);
		if (!p)
			p = conf->buf + offset - 1;
		p++;
		if (!strncasecmp(p, "flash_content", file_name_len + 1 -
				(p - conf->buf - offset))) {
			*offset_fc = offset_filestart - offset_start;
		}
		offset += file_name_len + 1 + file_size;
	}

	*conf_size = offset - offset_start;
	if (*conf_size > SECTORSIZE || filecount) {
		ERROR_LOG_NE("Invalid configuration files found in flash");
		*conf_size = 0;
		return -1;
	}
	INFO_LOG("Found configuration at sector %d with %d files and %d bytes",
		*conf_sector, filecount_s, *conf_size);
	return 0;
}

static int write_mtd_conf(struct flash_conf *conf, int cur_sector, int size,
	uint8_t buf[SECTORSIZE])
{
	int write_sector;
	const uint32_t search = 0xffffffffUL;
	int write_offset;
	uint32_t *write_p;
	int check_bytes;
	int written;
	uint8_t new_buf[SECTORSIZE];
	int read_bytes;

	write_sector = (cur_sector + 1) % 2;
	write_offset = write_sector * SECTORSIZE;
	write_p = (uint32_t *)(conf->buf + write_offset);
	for (check_bytes = (size + 3) & ~3; check_bytes > 0; check_bytes -= 4) {
		if (*write_p != search)
			break;
		write_p++;
	}

	if (check_bytes > 0) {
		WARN_LOG("Need to erase config write sector %d", write_sector);
		if (erase_mtd_sector(conf, write_sector))
			return -1;
	}

	memset(buf + size, 0, SECTORSIZE - size);

	if (lseek(conf->fd_char, write_offset, SEEK_SET) != write_offset) {
		ERROR_LOG("Cannot seek to config write sector offset 0x%x",write_offset);
		return -1;
	}

	written = 0;
	while (written < SECTORSIZE) {
		int r;
		r = write(conf->fd_char, buf + written, SECTORSIZE - written);
		if (r < 0) {
			ERROR_LOG("Cannot write to config flash area");
			return -1;
		}
		written += r;
	}

	/* Changes going to the mtd char dev are _not_ immediately visible
	 * to the corresponding block dev.
	 * The block dev is a simple emulation on top of the char dev,
	 * so it might not be valid, as these are two different devs.
	 * Reading from char dev has to be
	  * always valid. */
	if (lseek(conf->fd_char, write_offset, SEEK_SET) != write_offset) {
		ERROR_LOG("Cannot seek to config write sector offset 0x%x for reread",write_offset);
		return -1;
	}

	read_bytes = 0;
	while (read_bytes < size) {
		int r;
		r = read(conf->fd_char, new_buf + read_bytes, size - read_bytes);
		if (r < 0) {
			ERROR_LOG("Cannot read from config flash area");
			return -1;
		}
		read_bytes += r;
	}

	if (memcmp(buf, new_buf, size)) {
		ERROR_LOG_NE("Verify of config flash area failed. "
			"Erase it for not leaving incorrect data.");

		erase_mtd_sector(conf, write_sector);
		return -1;
	}

	return erase_mtd_sector(conf, cur_sector);
}

/* Time commands */

static int pic_get_time(int retrys, int *pic_time)
{
	struct pic_ioctl_arg cmd_arg;
	memset(&cmd_arg, 0, sizeof(cmd_arg));

	if (do_pic_ioctl(PIC_IOCTL_GET_TIME,&cmd_arg, retrys) ||
			cmd_arg.arg_count != 1)
		return -1;
	*pic_time = cmd_arg.args[0];
	return 0;
}

static int pic_set_time(int retrys, int new_time)
{
	struct pic_ioctl_arg cmd_arg;
	int diff;
	int trys;
	int pic_time;
	int r;

	trys = 0;
	do {
		memset(&cmd_arg, 0, sizeof(cmd_arg));
		cmd_arg.arg_count = 1;
		cmd_arg.args[0] = new_time;
		if (do_pic_ioctl(PIC_IOCTL_SET_TIME, &cmd_arg, retrys))
			return -1;

		r = pic_get_time(retrys, &pic_time);
		if (r < 0)
			return r;

		diff = pic_time - new_time;
		if (diff > -60 && diff < 60)
			return 0;

		ERROR_LOG_NE("Time set differs %d seconds", diff);
		usleep(10000);
		trys++;
	} while (!retrys || trys < retrys);

	return -1;
}

static int pic_time_to_sys(int retrys)
{
	struct timeval tv;
	int r;
	int pic_time;

	r = pic_get_time(retrys, &pic_time);
	if (r < 0)
		return r;

	tv.tv_sec = pic_time;
	tv.tv_usec = 0;
	if (settimeofday(&tv,NULL)) {
		ERROR_LOG("Cannot set system time");
		return -1;
	}
	return 0;
}

static int sys_time_to_pic(int retrys)
{
	int now;
	now = time(NULL);
	if (now < 0) {
		ERROR_LOG("Cannot get system time");
		return -1;
	}
	return pic_set_time(retrys,now);
}

/* Alarm (wakeup) commands */

static int pic_get_alarm(int retrys, int *alarm)
{
	struct pic_ioctl_arg cmd_arg;
	memset(&cmd_arg, 0, sizeof(cmd_arg));

	if (do_pic_ioctl(PIC_IOCTL_GET_ALARM, &cmd_arg, retrys) ||
			cmd_arg.arg_count != 1)
		return -1;

	*alarm = cmd_arg.args[0];
	return 0;
}

static int pic_clear_alarm(int retrys)
{
	struct pic_ioctl_arg cmd_arg;
	memset(&cmd_arg, 0, sizeof(cmd_arg));
	return do_pic_ioctl(PIC_IOCTL_CLEAR_ALARM,&cmd_arg, retrys);
}

static int pic_set_alarm(int retrys, int alarm_time)
{
	struct pic_ioctl_arg cmd_arg;
	int alarm_get;
	int trys;

	trys = 0;
	do {
		if (pic_clear_alarm(retrys))
			return -1;

		memset(&cmd_arg, 0, sizeof(cmd_arg));
		cmd_arg.arg_count = 1;
		cmd_arg.args[0] =  alarm_time;

		if (do_pic_ioctl(PIC_IOCTL_SET_ALARM,&cmd_arg, retrys))
			return -1;

		if (pic_get_alarm(retrys, &alarm_get))
			return -1;

		if (alarm_time == alarm_get)
			return 0;

		ERROR_LOG_NE("Alarm not set correct! (%d != %d)",alarm_time, alarm_get);
		trys++;
	} while(!retrys || trys < retrys);

	return -1;
}

/* Led command */
static int pic_set_led(int retrys, int led, int on)
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

static int pic_blink_led(int retrys)
{
	struct pic_ioctl_arg cmd_arg;
	memset(&cmd_arg, 0, sizeof(cmd_arg));
	return do_pic_ioctl(PIC_IOCTL_BLINK_LED,&cmd_arg, retrys);
}

static int pic_set_dim_led(int retrys, int low_val, int high_val,
	 int step_dur10ms, int flags)
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

static int pic_get_dim_led(int retrys, int *low_val, int *high_val,
	 int *step_dur10ms, int *flags)
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

/* Boot/Shutdown commands */

static int pic_get_start(int retrys, enum pic_start_mode *start_mode)
{
	struct pic_ioctl_arg cmd_arg;
	memset(&cmd_arg, 0, sizeof(cmd_arg));

	if (do_pic_ioctl(PIC_IOCTL_GET_START, &cmd_arg, retrys) ||
			cmd_arg.arg_count != 2)
		return -1;

	*start_mode = (enum pic_start_mode) cmd_arg.args[1];
	return 0;
}

static int pic_set_shutdown(int retrys, int halt_sec, int wait)
{
	struct pic_ioctl_arg cmd_arg;

	do {
		memset(&cmd_arg, 0, sizeof(cmd_arg));
		cmd_arg.arg_count = 1;
		cmd_arg.args[0] = halt_sec;
		if (do_pic_ioctl(PIC_IOCTL_SHUTDOWN, &cmd_arg, retrys))
			return -1;

		if (!wait)
			return 0;

		sleep(halt_sec + 2);
	} while (1);
	return 1;
}

static int pic_set_reboot(int retrys, int halt_sec, int reboot_sec, int wait)
{
	struct pic_ioctl_arg cmd_arg;

	do {
		memset(&cmd_arg, 0, sizeof(cmd_arg));
		cmd_arg.arg_count = 2;
		cmd_arg.args[0] = halt_sec;
		cmd_arg.args[1] = reboot_sec;
		if (do_pic_ioctl(PIC_IOCTL_REBOOT, &cmd_arg, retrys))
			return -1;

		if (!wait)
			return 0;

		sleep(halt_sec + 2);
	} while (1);
	return 1;
}

static int reset_boot_counter(void)
{
	struct flash_conf conf;
	uint8_t buf[16];
	int i;

	memset(&conf, 0, sizeof(conf));
	if (open_mtd_char(&conf)) {
		return -1;
	}

	if (lseek(conf.fd_char, BOOTCOUNTSECTOR * SECTORSIZE, SEEK_SET) !=
			BOOTCOUNTSECTOR * SECTORSIZE) {
		ERROR_LOG("Cannot seek to boot counter");
		close_mtd(&conf);
		return -1;
	}

	if (read(conf.fd_char, buf, 16) != 16) {
		ERROR_LOG("Cannot read boot counter");
		close_mtd(&conf);
		return -1;
	}

	for (i = 0; i < 16 && !buf[i]; i++)
		;

	if (i < 16 && buf[i] != 0xff) {
		ERROR_LOG_NE("Obscure boot counter");
		close_mtd(&conf);
		return -1;
	}

	if (!i) {
		INFO_LOG("Boot counter is 0 no need to reset");
		close_mtd(&conf);
		return 0;
	}

	INFO_LOG("Boot count is %d", i>>1);

	i = erase_mtd_sector(&conf, BOOTCOUNTSECTOR);
	close_mtd(&conf);
	return i;
}

/* MAC commands */

static int mac_valid(uint8_t *buf)
{
	return memcmp(buf,"\0\0\0\0\0\0",6) &&
		memcmp(buf,"\xff\xff\xff\xff\xff\xff",6) &&
		memcmp(buf,"\x00\x01\x02\x03\x04\x05",6);
}

static int pic_get_mac(int retrys, uint8_t mac[6])
{
	struct pic_ioctl_arg cmd_arg;
	uint8_t buf[512];
	int trys;

	trys = 0;
	do {
		memset(&cmd_arg, 0, sizeof(cmd_arg));
		memset(buf, 0, 512);
		cmd_arg.arg_count = 4;
		cmd_arg.args[3] = (uint32_t)buf;

		if (do_pic_ioctl(PIC_IOCTL_GET_DATA, &cmd_arg, retrys))
			return -1;

		memcpy(mac, buf, 6);
		if (cmd_arg.arg_count == 4 && cmd_arg.args[2] == 128 &&
				mac_valid(buf))
			return 0;

		ERROR_LOG_NE("Wrong mac %hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
			mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
		usleep(10000);
		trys++;
	} while (!retrys || trys < retrys);

	return -1;
}

static int flash_content_valid(uint8_t *buf, int size)
{
	char *systype;
	if (size != FLASH_CONTENT_SIZE ||
			memcmp(buf + 496,"END OF FILE\x0a\x00\x00\x00\x00",16))
		return 0;

	systype = getenv("SYSTEMTYPE");
	if (!systype) {
		WARN_LOG("Cannot get system type");
		return 1;
	}

	if (!strcasecmp(systype,"m740")) {
		return !memcmp(buf + 16,"M740AV",6);
	} else if (!strcasecmp(systype,"m750s")) {
		return !memcmp(buf + 16,"M750AV",6);
	}

	WARN_LOG("Unknown system type '%s'", systype);
	return 1;
}

static void gen_flash_content(uint8_t buf[FLASH_CONTENT_SIZE])
{
	char *systype;
	int fd;

	fd = open(FLASH_CONTENT_BACKUP2, O_RDONLY);
	if (fd >= 0){
		struct stat st;
		int val;
		val = !fstat(fd,&st);
		val = val && read(fd, buf, FLASH_CONTENT_SIZE) == FLASH_CONTENT_SIZE;
		close(fd);
		val = val && flash_content_valid(buf, st.st_size);
		if (val) {
			INFO_LOG("flash_content generated from '"
				 FLASH_CONTENT_BACKUP2 "'");
			return;
		}
	}
	WARN_LOG("'" FLASH_CONTENT_BACKUP2 "' not present or wrong");
	memset(buf, 0, FLASH_CONTENT_SIZE);

	memcpy(buf + 496,"END OF FILE\x0a",12);

	systype = getenv("SYSTEMTYPE");
	if (!systype) {
		WARN_LOG("Cannot get system type");
		return;
	}

	if (!strcasecmp(systype,"m740")) {
		memcpy(buf + 16,"M740AV",6);
		return;
	} else if (!strcasecmp(systype,"m750s")) {
		memcpy(buf + 16,"M750AV",6);
		return;
	}

	WARN_LOG("Unknown system type '%s'", systype);
}

static int check_mtd_flash_content(int retrys, uint8_t mac[6])
{
	struct flash_conf conf;
	int conf_ok;
	int conf_sector;
	int conf_size;
	int offset_fc;
	int fc_mtd_val;
   int fc_bkp1_val;
	int fd;
	int mac_val;
	int real_off;
	int fc_size;
	uint8_t fc_buf[FLASH_CONTENT_SIZE];
	uint8_t fc_buf_bkp1[FLASH_CONTENT_SIZE];

	memset(&conf, 0, sizeof(conf));
	memcpy(mac,"o7xorg",6);
	if (open_mtd_block(&conf)) {
		return -1;
	}

	conf_ok = !read_mtd_conf(&conf, &conf_sector, &conf_size, &offset_fc);
	fc_mtd_val = 0;
	real_off = 0;
	fc_size = 0;
	if (conf_ok && offset_fc > 0) {
		real_off = conf_sector * SECTORSIZE + offset_fc;
		fc_size = (conf.buf[real_off] << 8) | conf.buf[real_off + 1];
		real_off += 2;
		real_off += strlen(conf.buf + real_off) + 1;
		memcpy(fc_buf, conf.buf + real_off,
			fc_size < FLASH_CONTENT_SIZE ? fc_size : FLASH_CONTENT_SIZE);
		fc_mtd_val = flash_content_valid(fc_buf, fc_size);
	}

	fc_bkp1_val = 0;
	fd = open(FLASH_CONTENT_BACKUP1, O_RDONLY);
	if (fd >= 0) {
		struct stat st;
		fc_bkp1_val = !fstat(fd, &st);
		fc_bkp1_val = fc_bkp1_val && read(fd, fc_buf_bkp1, FLASH_CONTENT_SIZE)
			 == FLASH_CONTENT_SIZE;
		close(fd);
		fc_bkp1_val = fc_bkp1_val && flash_content_valid(fc_buf_bkp1, st.st_size);
	}

	if (!fc_mtd_val || (fc_bkp1_val && !mac_valid(fc_buf) &&
			mac_valid(fc_buf_bkp1))) {
		fc_mtd_val = 0;
		if (fc_bkp1_val)
			memcpy(fc_buf, fc_buf_bkp1, FLASH_CONTENT_SIZE);
		else
			gen_flash_content(fc_buf);
	}

	mac_val = !pic_get_mac(retrys, mac);
	if (mac_val && memcmp(fc_buf,mac,6)) {
		fc_mtd_val = 0;
		memcpy(fc_buf,mac,6);
	}

	if (!fc_bkp1_val || memcmp(fc_buf_bkp1, fc_buf, FLASH_CONTENT_SIZE)) {
		fd = open(FLASH_CONTENT_BACKUP1, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd >= 0) {
			write(fd, fc_buf,  FLASH_CONTENT_SIZE);
			close(fd);
		}
	}

	memcpy(mac, fc_buf, 6);
	if (!mac_valid(mac)) {
		memcpy(mac,"o7xorg", 6);
	}

	if (!fc_mtd_val) {
		uint8_t conf_buf[SECTORSIZE];
		if (open_mtd_char(&conf)) {
			close_mtd(&conf);
			return -1;
		}

		if (conf_ok) {
			memcpy(conf_buf, conf.buf + conf_sector * SECTORSIZE, conf_size);
		} else {
			offset_fc = -1;
			conf_buf[0] = 0;
			conf_buf[1] = 0;
			conf_size = 2;
			conf_sector = 1;
		}

		if (offset_fc < 0) {
			if (conf_size + 2 + 23 + FLASH_CONTENT_SIZE > SECTORSIZE) {
				ERROR_LOG_NE("Cannot generate flash_content sector size exceeded");
				close_mtd(&conf);
				return -1;
			}
			((uint16_t *)conf_buf)[0]++;
			real_off = conf_size;
			((uint16_t *)(conf_buf + real_off))[0] = FLASH_CONTENT_SIZE;
			fc_size = FLASH_CONTENT_SIZE;
			real_off += 2;
			memcpy(conf_buf + real_off, "/var/etc/flash_content",23);
			real_off += 23;
			conf_size += 2 + 23 + FLASH_CONTENT_SIZE;
		}

		if (fc_size != FLASH_CONTENT_SIZE) {
			if (conf_size + FLASH_CONTENT_SIZE - fc_size > SECTORSIZE) {
				ERROR_LOG_NE("Cannot generate flash_content sector size exceeded");
				close_mtd(&conf);
				return -1;
			}
			memmove(conf_buf + real_off + FLASH_CONTENT_SIZE,
				conf_buf + real_off + fc_size, conf_size - real_off - fc_size);
			((uint16_t *)(conf_buf + offset_fc))[0] = FLASH_CONTENT_SIZE;
			conf_size += FLASH_CONTENT_SIZE - fc_size;
		}
		memcpy(conf_buf + real_off, fc_buf, FLASH_CONTENT_SIZE);
		INFO_LOG("Write corrected flash_content to flash");
		write_mtd_conf(&conf, conf_sector, conf_size, conf_buf);
	}
	close_mtd(&conf);
	return 0;
}

static int test_write_mtd(void)
{
	struct flash_conf conf;
	int conf_sector;
	int conf_size;
	int offset_fc;
	uint8_t conf_buf[SECTORSIZE];

	memset(&conf, 0, sizeof(conf));
	if (open_mtd_block(&conf)) {
		return -1;
	}

	if (open_mtd_char(&conf)) {
		close_mtd(&conf);
		return -1;
	}

	if (read_mtd_conf(&conf, &conf_sector, &conf_size, &offset_fc)) {
		close_mtd(&conf);
		return -1;
	}

	memcpy(conf_buf, conf.buf + conf_sector * SECTORSIZE, conf_size);

	write_mtd_conf(&conf, conf_sector, conf_size, conf_buf);
	close_mtd(&conf);
	return 0;
}
static int get_mac(int retrys, uint8_t mac[6])
{
	int fd;

	fd = open(FLASH_CONTENT_BACKUP1, O_RDONLY);
	if (fd >= 0) {
		int r;
		r = read(fd, mac, 6);
		if (r == 6 && mac_valid(mac)) {
			close(fd);
			return 0;
		}
		close(fd);
	}

	INFO_LOG(FLASH_CONTENT_BACKUP1 " not present or wrong");
	return check_mtd_flash_content(retrys, mac);
}

/* Command line parsing */

static int pic_time_cmd(int argc, char **argv, int argsu, int retrys)
{
	if (argsu >= argc)
		return -1;

	if (!strcasecmp(argv[argsu],"get")) {
		int pic_time;
		if (!pic_get_time(retrys, &pic_time)) {
			printf("%d\n",pic_time);
		}
		return argsu + 1;
	}

	if (!strcasecmp(argv[argsu],"set")) {
		int new_time;
		char *p;
		argsu++;
		if (argsu >= argc)
			return -1;

		new_time = strtol(argv[argsu], &p, 0);
		if (*p || argv[argsu] == p)
			return -1;

		pic_set_time(retrys, new_time);
		return argsu + 1;
	}

	if (!strcasecmp(argv[argsu],"systopic")) {
		sys_time_to_pic(retrys);
		return argsu + 1;
	}

	if (!strcasecmp(argv[argsu],"pictosys")) {
		pic_time_to_sys(retrys);
		return argsu + 1;
	}
	return -1;
}

static int pic_alarm_cmd(int argc, char **argv, int argsu, int retrys)
{
	if (argsu >= argc)
		return -1;

	if (!strcasecmp(argv[argsu],"get")) {
		int pic_alarm;
		if (!pic_get_alarm(retrys, &pic_alarm)) {
			printf("%d\n",pic_alarm);
		}
		return argsu + 1;
	}

	if (!strcasecmp(argv[argsu],"clear")) {
		pic_clear_alarm(retrys);
		return argsu + 1;
	}

	if (!strcasecmp(argv[argsu],"set")) {
		int new_alarm;
		char *p;
		argsu++;
		if (argsu >= argc)
			return -1;

		new_alarm = strtol(argv[argsu], &p, 0);
		if (*p || argv[argsu] == p)
			return -1;

		pic_set_alarm(retrys, new_alarm);
		return argsu + 1;
	}
	return -1;
}

static int pic_led_cmd(int argc, char **argv, int argsu, int retrys)
{
	int led;
	int on;
	if (argsu >= argc)
		return -1;

	if (!strcasecmp(argv[argsu],"get-dim")) {
		int low_val;
		int high_val;
		int dur;
		int flags;
		if (!pic_get_dim_led(retrys, &low_val, &high_val, &dur, &flags)) {
			printf("%d %d %d %d\n",low_val, high_val, dur, flags);
		}
		return argsu + 1;
	}

	if (!strcasecmp(argv[argsu],"set-dim")) {
		int low_val;
		int high_val;
		int dur;
		int flags;
		char *p;

		if (argsu + 4 >= argc)
			return -1;

		argsu++;
		low_val = strtol(argv[argsu], &p, 0);
		if (*p || argv[argsu] == p)
			return -1;

		argsu++;
		high_val = strtol(argv[argsu], &p, 0);
		if (*p || argv[argsu] == p)
			return -1;

		argsu++;
		dur = strtol(argv[argsu], &p, 0);
		if (*p || argv[argsu] == p)
			return -1;

		argsu++;
		flags = strtol(argv[argsu], &p, 0);
		if (*p || argv[argsu] == p)
			return -1;

		pic_set_dim_led(retrys, low_val, high_val, dur, flags);
		return argsu + 1;
	}

	if (!strcasecmp(argv[argsu],"blink")) {
		pic_blink_led(retrys);
		return argsu + 1;
	}

	if (argsu + 1 >= argc)
		return -1;

	if (!strcasecmp(argv[argsu],"outer")) {
		led = OUTER_LED;
	} else if (!strcasecmp(argv[argsu],"middle")) {
		led = MIDDLE_LED;
	} else if (!strcasecmp(argv[argsu],"link")) {
		led = LINK_LED;
	} else {
		return -1;
	}
	argsu++;

	if (!strcasecmp(argv[argsu],"on")) {
		on = 1;
	} else if (!strcasecmp(argv[argsu],"off")) {
		on = 0;
	} else {
		return -1;
	}

	pic_set_led(retrys, led, on);
	return argsu + 1;
}

static int pic_shutdown_cmd(int argc, char **argv, int argsu, int retrys)
{
	int in_sec;
	int wait;
	char *p;
	if (argsu >= argc)
		return -1;

	wait = 0;

	in_sec = strtol(argv[argsu], &p, 0);
	if (*p || argv[argsu] == p)
		return -1;

	argsu++;
	if (argsu < argc && !strcasecmp(argv[argsu],"wait")) {
		wait = 1;
		argsu++;
	}

	pic_set_shutdown(retrys, in_sec, wait);
	return argsu;
}

static int pic_reboot_cmd(int argc, char **argv, int argsu, int retrys)
{
	int in_sec;
	int re_sec;
	int wait;
	char *p;
	if (argsu + 1 >= argc)
		return -1;

	wait = 0;

	in_sec = strtol(argv[argsu], &p, 0);
	if (*p || argv[argsu] == p)
		return -1;

	argsu++;

	re_sec = strtol(argv[argsu], &p, 0);
	if (*p || argv[argsu] == p)
		return -1;

	argsu++;
	if (argsu < argc && !strcasecmp(argv[argsu],"wait")) {
		wait = 1;
		argsu++;
	}

	pic_set_reboot(retrys, in_sec, re_sec, wait);
	return argsu;
}

static int parse_args(int argc, char **argv)
{
	int argsu;
	int retrys;
	int do_syslog;
	char *p;

	argsu = 1;
	retrys = 3;
	do_syslog = 0;
	while (argsu < argc && argv[argsu][0] == '-') {
		switch (argv[argsu][1]) {
			case 'r':
				argsu++;
				if (argsu >= argc)
					return -1;
				retrys = strtol(argv[argsu], &p, 0);
				if (*p || argv[argsu] == p)
					return -1;
				break;
			case 's':
				do_syslog = 1;
				break;
			default:
				return -1;
		}
		argsu++;
	}

	if (do_syslog) {
		init_syslog("pic_tool");
	}

	while (argsu > 0 && argsu < argc) {
		if (!strcasecmp(argv[argsu],"time")) {
			argsu = pic_time_cmd(argc, argv, argsu + 1, retrys);
		} else if (!strcasecmp(argv[argsu],"alarm")) {
			argsu = pic_alarm_cmd(argc, argv, argsu + 1, retrys);
		} else if (!strcasecmp(argv[argsu],"led")) {
			argsu = pic_led_cmd(argc, argv, argsu + 1, retrys);
		} else if (!strcasecmp(argv[argsu],"shutdown")) {
			argsu = pic_shutdown_cmd(argc, argv, argsu + 1, retrys);
		} else if (!strcasecmp(argv[argsu],"reboot")) {
			argsu = pic_reboot_cmd(argc, argv, argsu + 1, retrys);
		} else if (!strcasecmp(argv[argsu],"get-mac")) {
			uint8_t mac[6];
			get_mac(retrys, mac);
			printf("%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX\n",
				mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
			argsu++;
		} else if (!strcasecmp(argv[argsu],"get-mac-direct")) {
			uint8_t mac[6];
			if (!pic_get_mac(retrys, mac))
				printf("%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX\n",
					mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
			argsu++;
		} else if (!strcasecmp(argv[argsu],"check-flash")) {
			uint8_t mac[6];
			check_mtd_flash_content(retrys,mac);
			argsu++;
		} else if (!strcasecmp(argv[argsu],"test-write")) {
			test_write_mtd();
			argsu++;
		} else if (!strcasecmp(argv[argsu],"reset-boot")) {
			reset_boot_counter();
			argsu++;
		} else if (!strcasecmp(argv[argsu],"start-mode")) {
			enum pic_start_mode start_mode;
			if (!pic_get_start(retrys, &start_mode)) {
				switch(start_mode) {
					case pstm_powerfail:
						printf("powerfail\n");
						break;
					case pstm_alarm:
						printf("alarm\n");
						break;
					case pstm_user:
						printf("user\n");
						break;
					case pstm_reboot:
						printf("reboot\n");
						break;
					default:
						printf("unknown\n");
				}
			}
			argsu++;
		} else {
			argsu = -1;
		}
	}

	if (do_syslog) {
		close_syslog();
	}
	return argsu;
}

static void print_usage(void)
{
	fprintf(stderr,
		"pic_tool [-r <n>] [-s] [command ...]\n\n"
		"\t-r <n>\t\t<n> retries (default 3) 0:= infinite\n"
		"\t-s\t\tuse syslog\n\n"
		"commands:\n"
		"\talarm clear\t\t\t\t\tclear alarm\n"
		"\talarm get\t\t\t\t\tget alarm\n"
		"\talarm set <alarm>\t\t\t\tset alarm\n\n"
		"\tcheck-flash\t\t\t\t\tcheck and correct flash_content in flash\n\n"
		"\tget-mac\t\t\t\t\t\tget mac address\n\n"
		"\tled outer|middle|link on|off\t\t\tset led on/off\n"
		"\tled blink\t\t\t\t\tblink led\n"
		"\tled set-dim <low> <high> <step_dur> <flags>\tdim led\n"
		"\tled get-dim\t\t\t\t\tget dim led\n\n"
		"\treboot <n> <m> [wait]\t\t\t\tshutdown in <n> s reboot in <m> s\n\n"
		"\treset-boot\t\t\t\t\treset boot counter\n\n"
		"\tshutdown <n> [wait]\t\t\t\tshutdown in <n> s\n\n"
		"\tstart-mode\t\t\t\t\tget start mode\n\n"
		"\ttime get\t\t\t\t\tget time\n"
		"\ttime set <time>\t\t\t\t\tset time\n"
		"\ttime pictosys\t\t\t\t\trestore system time from pic\n"
		"\ttime systopic\t\t\t\t\tsave system time in pic\n");

}

int main(int argc, char **argv)
{
	if (argc <= 1) {
		print_usage();
		return 1;
	}

	if (parse_args(argc,argv) < 0) {
		print_usage();
		return 1;
	}
	return 0;
}
