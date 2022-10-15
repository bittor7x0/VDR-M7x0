# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006-2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2022 VDR-NG-EM Project
#
# More information can be found in the files COPYING and README.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# The project's page is at https://github.com/bittor7x0/VDR-M7x0
# Our English forum is at  https://vdr-m7x0.foroactivo.com.es/f19-international-forum
# Our Spanish forum is at  https://vdr-m7x0.foroactivo.com.es
#
# --- VDR-NG-EM-COPYRIGHT-NOTE-END ---

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
SIEMENS-LINUX-KERNEL_DEPS = $(BASE_BUILD_STAGEFILE) $(EGCS_INSTALLED)
ifeq ($(CONFIG_SQUASHFS_LZMA),y)
   SIEMENS-LINUX-KERNEL_DEPS += $(LZMA_BIN)
endif

ifeq ($(CONFIG_BOOTLOADER),y)
   SIEMENS-LINUX-KERNEL_DEPS += $(BOOTLOADER_INSTALLED) $(LZMA_BIN)
endif

ifeq ($(CONFIG_SIEMENS-LINUX-KERNEL),y)
ifneq ($(CONFIG_EGCS),y)
# Siemens has chosen this old compiler for compiling the kernel not me.
# IMO this is irresponsible.
   $(error dependency error: siemens-linux-kernel needs egcs enabled)
endif
endif

ifeq ($(CONFIG_JFFS2_LZO),y)
ifneq ($(and $(filter y,$(CONFIG_GENERATE_JFFS2_DIR)),$(filter y,$(CONFIG_GENERATE_JFFS2_IMAGE))),y)
   $(error dependency error: generating kernel image with JFFS2-LZO support needs generate_jffs2_dir and generate_jffs2_image enabled)
endif
endif

ifeq ($(CONFIG_GENERATE_SIEMENS-LINUX-KERNEL-IMG),y)
ifneq ($(CONFIG_SIEMENS-LINUX-KERNEL),y)
   $(error dependency error: generating kernel image needs siemens-linux-kernel enabled)
endif
endif
SIEMENS-LINUX-KERNEL-IMG = $(TOP_DIR)/$(or $(notdir \
   $(CONFIG_SIEMENS-LINUX-KERNEL-IMG)),siemens-linux-kernel-$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION).img)

SIEMENS-LINUX-KERNEL_FILE := siemens-linux-kernel.tar.xz
SIEMENS-LINUX-KERNEL_DLFILE := $(DOWNLOAD_DIR)/$(SIEMENS-LINUX-KERNEL_FILE)
SIEMENS-LINUX-KERNEL_URL := https://raw.githubusercontent.com/bittor7x0/VDR-M7x0/master/toolchain/downloads/siemens-linux-kernel.tar.xz
SIEMENS-LINUX-KERNEL_PATCHES_DIR := $(PATCHES_DIR)/siemens-linux-kernel
SIEMENS-LINUX-KERNEL_BASEDIR := slin_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)
SIEMENS-LINUX-KERNEL_DIR := $(BUILD_DIR)/$(SIEMENS-LINUX-KERNEL_BASEDIR)
SIEMENS-LINUX-KERNEL_ARCHBASEDIR := siemens-linux-kernel
SIEMENS-LINUX-KERNEL_ARCHDIR := $(BUILD_DIR)/$(SIEMENS-LINUX-KERNEL_ARCHBASEDIR)
SIEMENS-LINUX-KERNEL_CONFIG := $(CONFIGS_DIR)/siemens-linux-kernel/$(CONFIG_M7X0_TYPE)-$(CONFIG_FW_VERSION).config

SIEMENS-LINUX-KERNEL_INSTALLED = $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_installed

ifeq ($(strip $(CONFIG_SIEMENS-LINUX-KERNEL_GZIP_LEVEL)),)
  CONFIG_SIEMENS-LINUX-KERNEL_GZIP_LEVEL := 9
endif

PACKS_RULES_$(CONFIG_SIEMENS-LINUX-KERNEL) += $(SIEMENS-LINUX-KERNEL_INSTALLED)
FILE_LISTS_$(CONFIG_SIEMENS-LINUX-KERNEL) += siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION).lst
POST_RULES_$(CONFIG_GENERATE_SIEMENS-LINUX-KERNEL-IMG) += $(SIEMENS-LINUX-KERNEL-IMG)

CLEAN_RULES += clean-siemens-linux-kernel
DISTCLEAN_RULES += distclean-siemens-linux-kernel

SIEMENS-LINUX-KERNEL_MODLST := \
   lib/modules/2.4.21-xfs/kernel/drivers/usb/host/ehci-hcd.o \
   lib/modules/2.4.21-xfs/kernel/drivers/usb/host/usb-ohci.o

ifeq ($(CONFIG_FW_VERSION),pro)
	SIEMENS-LINUX-KERNEL_MODLST += \
		lib/modules/2.4.21-xfs/kernel/fs/cifs/cifs.o \
		lib/modules/2.4.21-xfs/kernel/fs/nfsd/nfsd.o

	ifeq ($(CONFIG_PPTPD),y)
	SIEMENS-LINUX-KERNEL_MODLST += \
		lib/modules/2.4.21-xfs/kernel/crypto/sha1.o \
		lib/modules/2.4.21-xfs/kernel/crypto/arc4.o \
		lib/modules/2.4.21-xfs/kernel/drivers/net/slhc.o \
		lib/modules/2.4.21-xfs/kernel/drivers/net/ppp_generic.o \
		lib/modules/2.4.21-xfs/kernel/drivers/net/ppp_async.o \
		lib/modules/2.4.21-xfs/kernel/drivers/net/ppp_mppe.o
	endif
endif

ifeq ($(CONFIG_DVB-KERNEL),y)
	SIEMENS-LINUX-KERNEL_MODLST += \
		lib/modules/2.4.21-xfs/kernel/drivers/i2c/i2c-algo-bit.o \
		lib/modules/2.4.21-xfs/kernel/drivers/i2c/i2c-dev.o \
		lib/modules/2.4.21-xfs/kernel/drivers/i2c/i2c-proc.o \
		lib/modules/2.4.21-xfs/kernel/drivers/input/evdev.o \
		lib/modules/2.4.21-xfs/kernel/drivers/input/input.o
endif

SIEMENS-LINUX-KERNEL_DIRLST := \
   lib/modules \
   lib/modules/2.4.21-xfs \
   lib/modules/2.4.21-xfs/kernel \
   lib/modules/2.4.21-xfs/kernel/drivers \
   lib/modules/2.4.21-xfs/kernel/drivers/usb \
   lib/modules/2.4.21-xfs/kernel/drivers/usb/host

ifeq ($(CONFIG_FW_VERSION),pro)
	SIEMENS-LINUX-KERNEL_DIRLST += \
		lib/modules/2.4.21-xfs/kernel/fs \
		lib/modules/2.4.21-xfs/kernel/fs/cifs \
		lib/modules/2.4.21-xfs/kernel/fs/nfsd

	ifeq ($(CONFIG_PPTPD),y)
	SIEMENS-LINUX-KERNEL_DIRLST += \
		lib/modules/2.4.21-xfs/kernel/crypto \
		lib/modules/2.4.21-xfs/kernel/drivers/net
	endif
endif

ifeq ($(CONFIG_DVB-KERNEL),y)
	SIEMENS-LINUX-KERNEL_DIRLST += \
		lib/modules/2.4.21-xfs/kernel/drivers/i2c \
		lib/modules/2.4.21-xfs/kernel/drivers/input
endif

#
# download siemens linux kernel
#

$(SIEMENS-LINUX-KERNEL_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(SIEMENS-LINUX-KERNEL_DLFILE) ] ; then \
	$(WGET) $(SIEMENS-LINUX-KERNEL_URL) -O $(SIEMENS-LINUX-KERNEL_DLFILE) ; \
	fi );
	$(TOUCH) $(SIEMENS-LINUX-KERNEL_DLFILE)

#
# unpack siemens linux kernel
#

$(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_unpacked: \
      $(SIEMENS-LINUX-KERNEL_DLFILE) \
      $(wildcard $(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/common/*.patch) \
      $(wildcard $(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/$(CONFIG_FW_VERSION)/*.patch) \
      $(wildcard $(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/$(CONFIG_M7X0_TYPE)/*.patch) \
      $(if $(filter pro,$(CONFIG_FW_VERSION)),$(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/pro/cifs-fs.tar.bz2) \
      $$(SIEMENS-LINUX-KERNEL_DEPS) $(SIEMENS-LINUX-KERNEL_CONFIG)
	-$(RM) -rf $(SIEMENS-LINUX-KERNEL_DIR)
	$(TAR) -C $(BUILD_DIR) -xJf $(SIEMENS-LINUX-KERNEL_DLFILE)
	$(MV) $(SIEMENS-LINUX-KERNEL_ARCHDIR) $(SIEMENS-LINUX-KERNEL_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_unpacked

#
# patch siemens linux kernel
#

$(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_patched: \
      $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_unpacked
# CIFS for 2.4er kernel (it's much easier to handle as smbfs and faster)
ifeq ($(CONFIG_FW_VERSION),pro)
	$(BZCAT) $(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/pro/cifs-fs.tar.bz2 | \
		 $(TAR) -C $(SIEMENS-LINUX-KERNEL_DIR) -f -
endif
# Needed patches to get kernel compiling
	$(call patch_package, $(SIEMENS-LINUX-KERNEL_DIR), \
		$(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/common)
# Needed patches for lite or pro version
	$(call patch_package, $(SIEMENS-LINUX-KERNEL_DIR), \
		$(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/$(CONFIG_FW_VERSION))
# Needed patches for other operating systems
	$(call patch_package, $(SIEMENS-LINUX-KERNEL_DIR), \
		$(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/host/$(HOST_BS))
# There are some hard coded defines in tree, which are system type depending
	$(call patch_package, $(SIEMENS-LINUX-KERNEL_DIR), \
		$(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/$(CONFIG_M7X0_TYPE))
	if [ X"$(CONFIG_PPTPD)" = X"y" ] ; then \
		$(call patch_package, $(SIEMENS-LINUX-KERNEL_DIR), \
			$(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/others/cryptoapi) ; \
		$(call patch_package, $(SIEMENS-LINUX-KERNEL_DIR), \
			$(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/others/mppe) ; \
	fi
	$(TOUCH) $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_patched

#
# configure siemens linux kernel
#

$(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_configured: \
      $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_patched
# let's do some cleanup the siemens tree is full of garbage
# seems as if siemens thinks open source developers are garbage collectors
# the source they are providing is a dump
# the only reason we using this are some very broken blobs which cannot
# be easily replaced. AFAIK there is no documentation for the mpeg stuff
# of the NEC µPD61130AS1 (mpeg-dec aka emma2). Please contact me if
# I'm wrong ... I would like to replace this broken mpeg-drivers.
	if [ ! -e $(PREFIX)/$(UCLIBC_TARGET)/bin/as.orig ] ; then \
		$(MV) $(PREFIX)/$(UCLIBC_TARGET)/bin/as $(PREFIX)/$(UCLIBC_TARGET)/bin/as.orig ; \
		$(CP) $(SCRIPTS_DIR)/as_wa.sh $(PREFIX)/$(UCLIBC_TARGET)/bin/as ; \
	fi
	$(CP) $(SIEMENS-LINUX-KERNEL_CONFIG) $(SIEMENS-LINUX-KERNEL_DIR)/.config
  # Disable CramFS support if Squash RootFS image is generated and viceversa
  ifeq ($(CONFIG_GENERATE_SQUASH_ROOTFS_IMAGE),y)
	$(SED) -i -e 's,^CONFIG_CRAMFS=y,# CONFIG_CRAMFS is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
    ifeq ($(CONFIG_SQUASHFS_LZMA),y)
		$(SED) -i -e 's,^# CONFIG_SQUASHFS_LZMA is not set,CONFIG_SQUASHFS_LZMA=y,g' \
			$(SIEMENS-LINUX-KERNEL_DIR)/.config
		$(SED) -i -e 's,^CONFIG_SQUASHFS_LZMA=y,CONFIG_SQUASHFS_LZMA=y\nCONFIG_SQUASHFS_LZMA_PB=2,g' \
			$(SIEMENS-LINUX-KERNEL_DIR)/.config
		$(SED) -i -e 's,^CONFIG_SQUASHFS_LZMA=y,CONFIG_SQUASHFS_LZMA=y\nCONFIG_SQUASHFS_LZMA_LP=2,g' \
			$(SIEMENS-LINUX-KERNEL_DIR)/.config
		$(SED) -i -e 's,^CONFIG_SQUASHFS_LZMA=y,CONFIG_SQUASHFS_LZMA=y\nCONFIG_SQUASHFS_LZMA_LC=1,g' \
			$(SIEMENS-LINUX-KERNEL_DIR)/.config
      endif
  else
  ifeq ($(CONFIG_GENERATE_CRAM_ROOTFS_IMAGE),y)
	$(SED) -i -e 's,^CONFIG_SQUASHFS=y,# CONFIG_SQUASHFS is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
  endif
  endif
  # Enable LZO and disable any JFFS2 compressors
  ifeq ($(CONFIG_JFFS2_LZO),y)
	$(SED) -i -e 's,^# CONFIG_LZO_COMPRESS is not set,CONFIG_LZO_COMPRESS=y,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^# CONFIG_LZO_DECOMPRESS is not set,CONFIG_LZO_DECOMPRESS=y,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^# CONFIG_JFFS2_LZO is not set,CONFIG_JFFS2_LZO=y,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_JFFS2_ZLIB=y,# CONFIG_JFFS2_ZLIB is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_JFFS2_RUBIN=y,# CONFIG_JFFS2_RUBIN is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
  endif
  # Enable/Disable DVB modules
  ifeq ($(CONFIG_DVB-KERNEL),y)
	$(SED) -i -e 's,^# CONFIG_INPUT is not set,CONFIG_INPUT=m,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^# CONFIG_INPUT_EVDEV is not set,CONFIG_INPUT_EVDEV=m,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^# CONFIG_I2C is not set,CONFIG_I2C=y\nCONFIG_I2C_ALGOBIT=m\nCONFIG_I2C_CHARDEV=m\nCONFIG_I2C_PROC=m,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_I2C_PROC=m,CONFIG_I2C_PROC=m\n# CONFIG_I2C_ELV is not set is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_I2C_PROC=m,CONFIG_I2C_PROC=m\n# CONFIG_I2C_VELLEMAN is not set is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_I2C_PROC=m,CONFIG_I2C_PROC=m\n# CONFIG_SCx200_I2C is not set is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_I2C_PROC=m,CONFIG_I2C_PROC=m\n# CONFIG_SCx200_ACB is not set is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_I2C_PROC=m,CONFIG_I2C_PROC=m\n# CONFIG_I2C_ALGOPCF is not set is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_I2C_PROC=m,CONFIG_I2C_PROC=m\n# CONFIG_INPUT_SERIO is not set is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_I2C_PROC=m,CONFIG_I2C_PROC=m\n# CONFIG_INPUT_IFORCE_USB is not set is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^# CONFIG_VIDEO_PROC_FS is not set,CONFIG_VIDEO_PROC_FS=y,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
  else
	$(SED) -i -e 's,^CONFIG_INPUT=m,# CONFIG_INPUT is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_INPUT_EVDEV=m,# CONFIG_INPUT_EVDEV is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_I2C=y,# CONFIG_I2C is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_I2C_ALGOBIT=m,# CONFIG_I2C_ALGOBIT is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_I2C_CHARDEV=m,# CONFIG_I2C_CHARDEV is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_I2C_PROC=m,# CONFIG_I2C_PROC is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
	$(SED) -i -e 's,^CONFIG_VIDEO_PROC_FS=y,# CONFIG_VIDEO_PROC_FS is not set,g' \
		$(SIEMENS-LINUX-KERNEL_DIR)/.config
  endif
	PATH='$(PREFIX_BIN):$(SIEMENS-LINUX-KERNEL_DIR):$(PATH)' $(MAKE) CROSS_COMPILE=$(TARGET)- ARCH=mips \
		BASH=$(BASH) CC=$(EGCS_BIN) -C $(SIEMENS-LINUX-KERNEL_DIR) oldconfig
	PATH='$(PREFIX_BIN):$(SIEMENS-LINUX-KERNEL_DIR):$(PATH)' $(MAKE) CROSS_COMPILE=$(TARGET)- ARCH=mips \
		CC=$(EGCS_BIN) -C $(SIEMENS-LINUX-KERNEL_DIR) dep
	# this file is missed (and empty) should be autogenerated
	$(TOUCH) $(SIEMENS-LINUX-KERNEL_DIR)/include/asm-mips/setup.h
	$(TOUCH) $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_configured

#
# compile siemens linux kernel
#

$(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_compiled: \
      $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_configured \
      $(TOP_DIR)/.config
# we need a unpack kernel as the bootloader cannot read elf binaries
	PATH='$(PREFIX_BIN):$(SIEMENS-LINUX-KERNEL_DIR):$(PATH)' $(MAKE) CROSS_COMPILE=$(TARGET)- ARCH=mips \
		CC=$(EGCS_BIN) -C $(SIEMENS-LINUX-KERNEL_DIR) vmlinux
	PATH='$(PREFIX_BIN):$(SIEMENS-LINUX-KERNEL_DIR):$(PATH)' $(MAKE) CROSS_COMPILE=$(TARGET)- ARCH=mips \
		CC=$(EGCS_BIN) -C $(SIEMENS-LINUX-KERNEL_DIR) modules
	$(TOUCH) $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_compiled

#
# install siemens linux kernel
#

$(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_installed: \
      $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_compiled
	$(MKDIR) -p $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)
	$(CP) -f $(SIEMENS-LINUX-KERNEL_DIR)/vmlinux $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)
	$(CAT) $(SIEMENS-LINUX-KERNEL_DIR)/System.map | $(SED) -e "s/^ffffffff//" > \
		$(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/System.map
	$(RM) $(SIEMENS-LINUX-KERNEL_DIR)/System.map
# This is what siemens is doing with the kernel. bootloader cannot handle otherwise
	$(PREFIX_BIN)/$(UCLIBC_OBJCOPY) --strip-all -O binary \
		$(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux \
		$(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux.bin
ifeq ($(CONFIG_BOOTLOADER),y)
	-$(RM) -f $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux.bin.lzma
	$(LZMA_BIN) e -lc1 -lp2 -pb2 -eos $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux.bin $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux.bin.lzma
else
	-$(RM) -f $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux.bin.gz
	$(GZIP) -$(CONFIG_SIEMENS-LINUX-KERNEL_GZIP_LEVEL) $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux.bin
endif
	PATH='$(PREFIX_BIN):$(SIEMENS-LINUX-KERNEL_DIR):$(PATH)' $(MAKE) CROSS_COMPILE=$(TARGET)- ARCH=mips \
		CC=$(EGCS_BIN) -C $(SIEMENS-LINUX-KERNEL_DIR) \
		INSTALL_MOD_PATH=$(TARGET_ROOT)/$(M7X0_KERNEL_DIR) modules_install
ifeq ($(CONFIG_PPTPD),y)
	(if [ X"`$(GREP) sha1\.o $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net`" = X"" ] ; then \
		$(SED) -i 's/\/usr\/sbin\/inetd\ \/etc\/inetd.conf\ \&/\/sbin\/insmod\ \/lib\/modules\/2.4.21-xfs\/kernel\/crypto\/sha1.o\n\ \ \ \ \/usr\/sbin\/inetd\ \/etc\/inetd.conf\ \&/g' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net ; \
	fi);
	(if [ X"`$(GREP) arc4\.o $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net`" = X"" ] ; then \
		$(SED) -i 's/\/usr\/sbin\/inetd\ \/etc\/inetd.conf\ \&/\/sbin\/insmod\ \/lib\/modules\/2.4.21-xfs\/kernel\/crypto\/arc4.o\n\ \ \ \ \/usr\/sbin\/inetd\ \/etc\/inetd.conf\ \&/g' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net ; \
	fi);
	(if [ X"`$(GREP) slhc\.o $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net`" = X"" ] ; then \
		$(SED) -i 's/\/usr\/sbin\/inetd\ \/etc\/inetd.conf\ \&/\/sbin\/insmod\ \/lib\/modules\/2.4.21-xfs\/kernel\/drivers\/net\/slhc.o\n\ \ \ \ \/usr\/sbin\/inetd\ \/etc\/inetd.conf\ \&/g' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net ; \
	fi);
	(if [ X"`$(GREP) ppp_generic\.o $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net`" = X"" ] ; then \
		$(SED) -i 's/\/usr\/sbin\/inetd\ \/etc\/inetd.conf\ \&/\/sbin\/insmod\ \/lib\/modules\/2.4.21-xfs\/kernel\/drivers\/net\/ppp_generic.o\n\ \ \ \ \/usr\/sbin\/inetd\ \/etc\/inetd.conf\ \&/g' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net ; \
	fi);
	(if [ X"`$(GREP) ppp_async\.o $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net`" = X"" ] ; then \
		$(SED) -i 's/\/usr\/sbin\/inetd\ \/etc\/inetd.conf\ \&/\/sbin\/insmod\ \/lib\/modules\/2.4.21-xfs\/kernel\/drivers\/net\/ppp_async.o\n\ \ \ \ \/usr\/sbin\/inetd\ \/etc\/inetd.conf\ \&/g' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net ; \
	fi);
	(if [ X"`$(GREP) ppp_mppe\.o $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net`" = X"" ] ; then \
		$(SED) -i 's/\/usr\/sbin\/inetd\ \/etc\/inetd.conf\ \&/\/sbin\/insmod\ \/lib\/modules\/2.4.21-xfs\/kernel\/drivers\/net\/ppp_mppe.o\n\ \ \ \ \/usr\/sbin\/inetd\ \/etc\/inetd.conf\ \&/g' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.local.net ; \
	fi);
endif
	$(TOUCH) $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_installed

#
# generate siemens linux kernel flash image
#
$(SIEMENS-LINUX-KERNEL-IMG): \
      $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_installed \
      $(if $(CONFIG_BOOTLOADER),$(GEN_KERNEL_IMG_BIN) $(BOOTLOADER_BIN),$(GEN_KERNEL_HEADER_BIN))
ifeq ($(CONFIG_BOOTLOADER),y)
	$(TOUCH) dummy.bin
	$(GZIP) dummy.bin
	$(GEN_KERNEL_IMG_BIN) $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux.bin.lzma \
		$(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/System.map dummy.bin.gz $(BOOTLOADER_BIN) \
		$(SIEMENS-LINUX-KERNEL-IMG) -$(CONFIG_M7X0_TYPE)
	$(RM) -f dummy.bin.gz
else
	($(GEN_KERNEL_HEADER_BIN) $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux.bin.gz \
		$(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/System.map && \
		cat $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux.bin.gz ) \
	> $(SIEMENS-LINUX-KERNEL-IMG)
endif
ifeq ($(strip $(HOST_BS)), OpenBSD)
	@if	$(TEST) `$(STAT) -f b $(SIEMENS-LINUX-KERNEL-IMG)` \
			-gt $(KERNEL_MAX_IMGSIZE); then \
		$(ECHO) Error: Kernel image too big!; \
		$(ECHO) "       You can increase gzip compression level setting in .config:"; \
		$(ECHO) "          CONFIG_SIEMENS-LINUX-KERNEL_GZIP_LEVEL = 9"; \
		$(ECHO) "       or you can try to use LZMA compression setting:"; \
		$(ECHO) "          CONFIG_BOOTLOADER = Y"; \
		$(RM) -f $(SIEMENS-LINUX-KERNEL-IMG); \
		exit 1; \
	fi
else
	@if	$(TEST) `$(STAT) --format="%s" $(SIEMENS-LINUX-KERNEL-IMG)` \
			-gt $(KERNEL_MAX_IMGSIZE); then \
		$(ECHO) Error: Kernel image too big!; \
		$(ECHO) "       You can increase gzip compression level setting in .config:"; \
		$(ECHO) "          CONFIG_SIEMENS-LINUX-KERNEL_GZIP_LEVEL = 9"; \
		$(ECHO) "       or you can try to use LZMA compression setting:"; \
		$(ECHO) "          CONFIG_BOOTLOADER = Y"; \
		$(RM) -f $(SIEMENS-LINUX-KERNEL-IMG); \
		exit 1; \
	fi
endif

#
# generate rootfs file list
#

siemens-linux-kernel_lst_dir = \
  $(ECHO) "$(1)       -                 d 755 0 0 - - - - -";

siemens-linux-kernel_lst_mod = \
  $(ECHO) "$(1) $(M7X0_KERNEL_DIR)/$(1) f 755 0 0 - - - - -";

$(FILELIST_DIR)/siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION).lst: \
      $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_installed
	($(foreach dir,$(SIEMENS-LINUX-KERNEL_DIRLST),\
		$(call siemens-linux-kernel_lst_dir,$(dir))) \
	 $(foreach mod,$(SIEMENS-LINUX-KERNEL_MODLST),\
		$(call siemens-linux-kernel_lst_mod,$(mod)))) \
			> $(FILELIST_DIR)/siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION).lst

.PHONY: clean-siemens-linux-kernel distclean-siemens-linux-kernel


clean-siemens-linux-kernel:
	-$(RM) -rf $(SIEMENS-LINUX-KERNEL_DIR)

distclean-siemens-linux-kernel:
	-$(RM) -f $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_patched
	-$(RM) -f $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_configured
	-$(RM) -f $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)_installed
	-$(RM) -f $(SIEMENS-LINUX-KERNEL-IMG)
	-$(RM) -f $(FILELIST_DIR)/siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION).lst
