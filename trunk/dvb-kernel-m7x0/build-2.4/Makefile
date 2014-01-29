ifeq ($(obj),)
obj = .
endif

MDIR		:= kernel/drivers/dvb

saa7146-objs    := saa7146_core.o saa7146_i2c.o 
saa7146-vv-objs	:= saa7146_fops.o saa7146_video.o saa7146_hlp.o saa7146_vbi.o saa7146_vv_ksyms.o

dvb-ttpci-budget-objs := budget-core.o budget.o
dvb-ttpci-budget-ci-objs := budget-core.o budget-ci.o
dvb-ttpci-budget-av-objs := budget-core.o budget-av.o
dvb-ttpci-budget-patch-objs := budget-core.o budget-patch.o

dvb-ttpci-objs	  :=  av7110_hw.o av7110_v4l.o av7110_av.o av7110_ca.o av7110.o av7110_ipack.o
ifneq ($(CONFIG_INPUT_EVDEV),)
dvb-ttpci-objs    +=  av7110_ir.o
endif

dvb-core-objs   := dvb_functions.o dvb_compat.o dmxdev.o dvb_demux.o dvb_net.o dvb_i2c.o \
		dvb_filter.o dvb_frontend.o dvbdev.o \
		dvb_ksyms.o dvb_ringbuffer.o dvb_ca_en50221.o

export-objs	:= ttpci-eeprom.o dvb_functions.o budget-core.o dvb_compat.o saa7146_vv_ksyms.o saa7146_core.o \
		dvb_ksyms.o video-buf.o v4l2-common.o v4l1-compat.o bt878.o dvb_ringbuffer.o # videodev.o

obj-saa7146	:= video-buf.o v4l2-common.o v4l1-compat.o saa7146.o saa7146_vv.o 
obj-dvb 	:= dvb-core.o \
			cinergyT2.o \
 			stv0299.o alps_tdlb7.o alps_tdmb7.o \
			tda1004x.o at76c651.o cx24110.o grundig_29504-401.o \
			grundig_29504-491.o ves1820.o ves1x93.o nxt6000.o sp887x.o \
			dst.o dvb_dummy_fe.o  \
			dvb-ttpci-budget.o dvb-ttpci-budget-ci.o \
			dvb-ttpci-budget-av.o \
			dvb-bt8xx.o bt878.o \
			skystar2.o \
			dvb-ttusb-budget.o \
			ttusb_dec.o mt312.o mt352.o ttpci-eeprom.o

# create a variable that holds the name of a firmware file for the av7110
AV7110_FIRMWARE_PRESENT := $(wildcard dvb-ttpci*.fw)

# if a firmware file was found in the current directory, build the av7710
# drivers that depend on it
ifneq ($(AV7110_FIRMWARE_PRESENT),)
obj-dvb += dvb-ttpci-budget-patch.o dvb-ttpci.o
# if it does not exist, make a dependecy to a fake target which simply
# prints out a message 
else
obj-dvb += dvb-ttpci-info.txt 
endif

obj-m		:= $(obj-saa7146) $(obj-dvb) # videodev.o

emacs-etags	:= TAGS

#################################################
# compile modules

ifneq ($(KERNELRELEASE),)
# recursive call from kernel build system
dummy		:= $(shell echo $(KERNELRELEASE) > $(obj)/.version)

multi-m		:= $(filter $(list-multi), $(obj-m))
int-m		:= $(sort $(foreach m, $(multi-m), $($(basename $(m))-objs)))
EXTRA_CFLAGS	:= -MD -I ../linux/include -I . -DCONFIG_DVB_AV7110_OSD

include $(TOPDIR)/Rules.make


else
# take version info from last module build if available
KERNELRELEASE	:= $(shell cat $(obj)/.version 2>/dev/null || echo '2.4.21-xfs')
endif

KDIR		:= $(DESTDIR)/lib/modules/$(KERNELRELEASE)/build
PWD		:= $(shell pwd)
DEST		:= $(DESTDIR)/lib/modules/$(KERNELRELEASE)/$(MDIR)

include .config

build:
	[ -L saa7146_video.c ] || ./getlinks
	$(MAKE) -C $(KDIR) CC="$(CC) -I$(PWD)/include" SUBDIRS=$(PWD) modules

install:
	mkdir -p $(DEST); cp -v $(filter-out %.txt,$(obj-m)) $(DEST)

clean:
	find . -type l -exec rm {} \;
	-rm -f *~ .version
	-rm -f *.o *.d .*.o.flags *.o.flags *.o.cmd
	-rm -f TAGS
	-rm -f fdump av7110_firm.h dsp_dec2000t.h dsp_dec2540t.h dsp_dec3000s.h dvb-ttpci-info.txt

saa7146.o: $(saa7146-objs)
	$(LD) -r -o $@ $(saa7146-objs)

saa7146_vv.o: $(saa7146-vv-objs)
	$(LD) -r -o $@ $(saa7146-vv-objs)

dvb-ttpci-budget-av.o: $(dvb-ttpci-budget-av-objs)
	$(LD) -r -o $@ $(dvb-ttpci-budget-av-objs)

dvb-ttpci-budget-ci.o: $(dvb-ttpci-budget-ci-objs)
	$(LD) -r -o $@ $(dvb-ttpci-budget-ci-objs)

dvb-ttpci-budget-patch.o: $(dvb-ttpci-budget-patch-objs)
	$(LD) -r -o $@ $(dvb-ttpci-budget-patch-objs)

dvb-ttpci-budget.o: $(dvb-ttpci-budget-objs)
	$(LD) -r -o $@ $(dvb-ttpci-budget-objs)

av7110_firm.h: fdump $(AV7110_FIRMWARE_PRESENT)
	./fdump $(AV7110_FIRMWARE_PRESENT) dvb_ttpci_fw $@
	
av7110.o: av7110_firm.h

dvb-ttpci-info.txt:
	( \
	echo 'Warning: no firmware file for the av7110 driver found.' \
	echo '         the driver will *not* be build.' \
	echo '         Have a look at README for details!' \
	) > $@; 

dvb-ttpci.o: $(dvb-ttpci-objs)
	$(LD) -r -o $@ $(dvb-ttpci-objs)

dvb-core.o: $(dvb-core-objs)
	$(LD) -r -o $@ $(dvb-core-objs)

tags:
	etags *.[ch]

-include $(wildcard *.d) dummy

ttusb_dec.o: dsp_dec2000t.h dsp_dec2540t.h dsp_dec3000s.h

dsp_dec2000t.h: fdump
	@if [ -r dvb-ttusb-dec-2000t.fw ]; then \
		./fdump dvb-ttusb-dec-2000t.fw dsp_dec2000t $@; \
	else \
		( \
		echo '#include <asm/types.h>'; \
		echo 'u8 dsp_dec2000t [] __initdata = {};'; \
		echo '#warning WARNING:'; \
		echo '#warning If you intend to use the DEC2000-t driver please fetch the firmware as'; \
		echo '#warning described in:'; \
		echo '#warning linux/Documentation/dvb/ttusb-dec.txt'; \
		) > $@; \
	fi

dsp_dec2540t.h: fdump
	@if [ -r dvb-ttusb-dec-2540t.fw ]; then \
		./fdump dvb-ttusb-dec-2540t.fw dsp_dec2540t $@; \
	else \
		( \
		echo '#include <asm/types.h>'; \
		echo 'u8 dsp_dec2540t [] __initdata = {};'; \
		echo '#warning WARNING:'; \
		echo '#warning If you intend to use the DEC2540-t driver please fetch the firmware as'; \
		echo '#warning described in:'; \
		echo '#warning linux/Documentation/dvb/ttusb-dec.txt'; \
		) > $@; \
	fi

dsp_dec3000s.h: fdump
	@if [ -r dvb-ttusb-dec-3000s.fw ]; then \
		./fdump dvb-ttusb-dec-3000s.fw dsp_dec3000s $@; \
	else \
		( \
		echo '#include <asm/types.h>'; \
		echo 'u8 dsp_dec3000s [] __initdata = {};'; \
		echo '#warning WARNING:'; \
		echo '#warning If you intend to use the DEC3000-s driver please fetch the firmware as'; \
		echo '#warning described in:'; \
		echo '#warning linux/Documentation/dvb/ttusb-dec.txt'; \
		) > $@; \
	fi

fdump:
	$(HOSTCC) -o $@ fdump.c

