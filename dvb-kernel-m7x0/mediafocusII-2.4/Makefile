ifeq ($(obj),)
obj = .
endif

MDIR		:= misc

saa7146-objs    := saa7146_core.o saa7146_i2c.o 
saa7146-vv-objs	:= saa7146_fops.o saa7146_video.o saa7146_hlp.o saa7146_vbi.o saa7146_vv_ksyms.o

export-objs	:= saa7146_vv_ksyms.o saa7146_core.o video-buf.o v4l2-common.o v4l1-compat.o

obj-saa7146	:= video-buf.o v4l2-common.o v4l1-compat.o saa7146.o saa7146_vv.o 
obj-i2c 	:= drp3510II.o ee24lc16.o msp3400satII.o sp5055II.o stv0056af.o saa7113h.o 

mf2-objs 	:= mfII-sat.o mfII-core.o
obj-mf2 	:= mfII.o 

obj-m		:= $(obj-mf2) $(obj-i2c) $(obj-saa7146) 

emacs-etags	:= TAGS

#################################################
# compile modules

ifneq ($(KERNELRELEASE),)
# recursive call from kernel build system
dummy		:= $(shell echo $(KERNELRELEASE) > $(obj)/.version)

multi-m		:= $(filter $(list-multi), $(obj-m))
int-m		:= $(sort $(foreach m, $(multi-m), $($(basename $(m))-objs)))
EXTRA_CFLAGS	:= -MD -I ../linux/include -I .

include $(TOPDIR)/Rules.make


else
# take version info from last module build if available
KERNELRELEASE	:= $(shell uname -r)
endif

KDIR		:= /usr/src/linux-$(KERNELRELEASE)
PWD		:= $(shell pwd)
DEST		:= /lib/modules/$(KERNELRELEASE)/$(MDIR)

build::
	$(MAKE) -C $(KDIR) CC="gcc -I$(PWD)/include -I$(PWD)/" SUBDIRS=$(PWD) modules

install:: build
	if [ -r /lib/modules/$(KERNELRELEASE)/misc/videodev.o ]; then echo -e "\n\nWarning: There is a stale /lib/modules/$(KERNELRELEASE)/misc/videodev.o -- probably from a previous installation of this driver. Make sure you have Video 4 Linux support in your kernel (either as module or statically linked) and delete this file. This has changed from previous versions. I'm aborting now.. \n\n"; false; fi
	strip --strip-debug $(obj-m)
	-su -c "mkdir -p $(DEST); cp -v $(obj-m) $(DEST); depmod -a"

clean::
	-rm -f *~ .version
	-rm -f *.o *.d .*.o.flags *.o.flags *.o.cmd
	-rm -f TAGS

distclean::
	find . -type l -exec rm {} \;
	-rm -f *~ .version
	-rm -f *.o *.d .*.o.flags *.o.flags *.o.cmd
	-rm -f TAGS

mfII.o: $(mf2-objs)
	$(LD) -r -o $@ $(mf2-objs)

saa7146.o: $(saa7146-objs)
	$(LD) -r -o $@ $(saa7146-objs)

saa7146_vv.o: $(saa7146-vv-objs)
	$(LD) -r -o $@ $(saa7146-vv-objs)

tags:
	etags *.[ch]

-include $(wildcard *.d) dummy

fdump:
	$(CC) -o $@ fdump.c


