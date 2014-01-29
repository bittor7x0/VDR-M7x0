ifeq ($(KERNELRELEASE),)

KERNELRELEASE := $(shell uname -r)
KERNELSRCDIR := /lib/modules/$(KERNELRELEASE)/build
BUILD_DIR := $(shell pwd)
export DVB_SRC_DIR := $(shell cd ..; pwd)
 
include $(KERNELSRCDIR)/.config

# if no DVB drivers are selected in the kernel config,
# we check if an av7110 driver firmware is available
# if so, we compile the firmware statically into the driver
# otherwise the driver will need firmware hotplug support to work
ifndef CONFIG_DVB
CONFIG_DVB_AV7110_FIRMWARE_FILE := $(wildcard dvb-ttpci*.fw)
ifneq ($(CONFIG_DVB_AV7110_FIRMWARE_FILE),)
CONFIG_DVB_AV7110_FIRMWARE = y
CONFIG_DVB_AV7110_OSD = y
DVB_FIRMWARE += av7110_firm.h
endif
else
ifdef CONFIG_DVB_AV7110_FIRMWARE
DVB_FIRMWARE += av7110_firm.h
endif
endif

# if no DVB driver is selected, video-buf.o won't be compiled
# make sure that we compile it locally
ifndef CONFIG_VIDEO_BUF
VIDEO_BUF = video-buf.c
endif

all: makelinks $(DVB_FIRMWARE) $(VIDEO_BUF)
	rm -rf bt848.h
	ln -s $(KERNELSRCDIR)/drivers/media/video/bt848.h bt848.h
	$(MAKE) -C $(KERNELSRCDIR) SUBDIRS=$(BUILD_DIR) AV7110_FIRMWARE=$(CONFIG_DVB_AV7110_FIRMWARE_FILE) AV7110_OSD=$(CONFIG_DVB_AV7110_OSD)

video-buf.c:
	rm -rf video-buf.c
	ln -s $(KERNELSRCDIR)/drivers/media/video/video-buf.c video-buf.c

clean:
	rm -f *.o *.ko .*.o.cmd .*.ko.cmd *.mod.c .*.o.d fdump av7110_firm.h video-buf.c bt848.h
	find . -type l | xargs -r rm

install:
	@echo "Use insmod.sh to load/unload the driver modules."

makelinks:
	[ -L saa7146_video.c ] || ./getlinks

# prepare firmware if selected above
fdump:
	$(CC) -o $@ fdump.c

av7110_firm.h: fdump
	./fdump $(CONFIG_DVB_AV7110_FIRMWARE_FILE) dvb_ttpci_fw $@

else

ifneq ($(AV7110_FIRMWARE),)
FW := -DCONFIG_DVB_AV7110_FIRMWARE -DCONFIG_DVB_AV7110_FIRMWARE_FILE=$(AV7110_FIRMWARE)
endif

ifneq ($(AV7110_OSD),)
OSD := -DCONFIG_DVB_AV7110_OSD
endif

export CC := $(CROSS_COMPILE)gcc -I$(DVB_SRC_DIR)/linux/include $(FW) $(OSD)

#EXTRA_CFLAGS = -Idrivers/media/video

saa7146-objs    := saa7146_i2c.o saa7146_core.o 
saa7146_vv-objs := saa7146_vv_ksyms.o saa7146_fops.o saa7146_video.o saa7146_hlp.o saa7146_vbi.o
obj-m += saa7146.o
obj-m += saa7146_vv.o

dvb-core-objs = dvbdev.o dmxdev.o dvb_demux.o dvb_filter.o \
	        dvb_ca_en50221.o dvb_functions.o dvb_frontend.o \
		dvb_i2c.o dvb_net.o dvb_ksyms.o dvb_ringbuffer.o
obj-m += dvb-core.o

ifndef CONFIG_VIDEO_BUF
obj-m += video-buf.o
endif
obj-m += dst.o
obj-m += stv0299.o
obj-m += alps_tdlb7.o
obj-m += alps_tdmb7.o
obj-m += at76c651.o
obj-m += cx24110.o
obj-m += grundig_29504-491.o
obj-m += grundig_29504-401.o
obj-m += mt312.o
obj-m += mt352.o
obj-m += ves1820.o
obj-m += ves1x93.o
obj-m += tda1004x.o
obj-m += sp887x.o
obj-m += nxt6000.o

obj-m += skystar2.o
obj-m += bt878.o dvb-bt8xx.o
obj-m += dvb-ttusb-budget.o
obj-m += ttusb_dec.o

obj-m += ttpci-eeprom.o
obj-m += budget-core.o
obj-m += budget.o
obj-m += budget-av.o
obj-m += budget-ci.o
obj-m += budget-patch.o

dvb-ttpci-objs := av7110_hw.o av7110_v4l.o av7110_av.o av7110_ca.o av7110.o av7110_ipack.o av7110_ir.o
obj-m += dvb-ttpci.o

endif
