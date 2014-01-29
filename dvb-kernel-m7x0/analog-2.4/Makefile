ifeq ($(obj),)
obj = .
endif

MDIR		:= misc

saa7146-objs    := saa7146_core.o saa7146_i2c.o 
saa7146-vv-objs	:= saa7146_fops.o saa7146_video.o saa7146_hlp.o saa7146_vbi.o  

export-objs	:= saa7146_core.o saa7146_fops.o saa7146_video.o saa7146_vbi.o \
		video-buf.o v4l2-common.o v4l1-compat.o

obj-saa7146	:= saa7146.o saa7146_vv.o video-buf.o v4l2-common.o v4l1-compat.o
obj-hexium	:= hexium_orion.o hexium_gemini.o 
obj-mxb 	:= mxb.o dpc7146.o  tuner.o saa7111.o tda9840.o tea6415c.o tea6420.o #saa5281.o
obj-cm7326	:= cm7326.o 

obj-m		:= $(obj-mxb) $(obj-hexium) $(obj-cm7326) $(obj-saa7146)

emacs-etags	:= TAGS

#################################################
# compile modules

ifneq ($(KERNELRELEASE),)
# recursive call from kernel build system
dummy		:= $(shell echo $(KERNELRELEASE) > $(obj)/.version)

multi-m		:= $(filter $(list-multi), $(obj-m))
int-m		:= $(sort $(foreach m, $(multi-m), $($(basename $(m))-objs)))
EXTRA_CFLAGS	:= -MD -I include/media

include $(TOPDIR)/Rules.make


else
# take version info from last module build if available
KERNELRELEASE	:= $(shell cat $(obj)/.version 2>/dev/null || uname -r)
endif

KDIR		:= /lib/modules/$(KERNELRELEASE)/build
PWD		:= $(shell pwd)
DEST		:= /lib/modules/$(KERNELRELEASE)/$(MDIR)

build::
	$(MAKE) -C $(KDIR) CC="gcc -I$(PWD)/include" SUBDIRS=$(PWD) modules

install:: build
	if [ -f /lib/modules/$(KERNELRELEASE)/misc/videodev.o ]; then echo -e "\n\nWarning: There is a stale /lib/modules/$(KERNELRELEASE)/misc/videodev.o -- probably from a previous installation of this driver. Make sure you have Video 4 Linux support in your kernel (either as module or statically linked) and delete this file. This has changed from previous versions. I'm aborting now.. \n\n"; false; fi
	strip --strip-debug $(obj-m)
	-su -c "mkdir -p $(DEST); cp -v $(obj-m) $(DEST); depmod -a"

install_hexium:: build
	if [ -f /lib/modules/$(KERNELRELEASE)/misc/videodev.o ]; then echo -e "\n\nWarning: There is a stale /lib/modules/$(KERNELRELEASE)/misc/videodev.o -- probably from a previous installation of this driver. Make sure you have Video 4 Linux support in your kernel (either as module or statically linked) and delete this file. This has changed from previous versions. I'm aborting now.. \n\n"; false; fi
	strip --strip-debug $(obj-saa7146) $(obj-hexium)
	-su -c "mkdir -p $(DEST); cp -v $(obj-saa7146) $(obj-hexium) $(DEST); depmod -a"

install_mxb:: build
	if [ -f /lib/modules/$(KERNELRELEASE)/misc/videodev.o ]; then echo -e "\n\nWarning: There is a stale /lib/modules/$(KERNELRELEASE)/misc/videodev.o -- probably from a previous installation of this driver. Make sure you have Video 4 Linux support in your kernel (either as module or statically linked) and delete this file. This has changed from previous versions. I'm aborting now.. \n\n"; false; fi
	strip --strip-debug $(obj-saa7146) $(obj-mxb)
	find /lib/modules/$(KERNELRELEASE)/kernel -iname "*tuner*" -exec mv {} /tmp/{}.old \;
	-su -c "mkdir -p $(DEST); cp -v $(obj-saa7146) $(obj-mxb) $(DEST); depmod -a"

clean::
	find . -type l -exec rm {} \;
	-rm -f *~ .version
	-rm -f *.o *.d .*.o.flags *.o.flags *.o.cmd
	-rm -f TAGS

saa7146.o: $(saa7146-objs)
	$(LD) -r -o $@ $(saa7146-objs)

saa7146_vv.o: $(saa7146-vv-objs)
	$(LD) -r -o $@ $(saa7146-vv-objs)

tags:
	etags *.[ch]

-include $(wildcard *.d) dummy

