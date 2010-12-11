# Copyright (C) 2006,2007 Andreas Koch - the open7x0.org group
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
# Or, point your browser to http://www.gnu.org/copyleft/gpl.html
#
# The author can be reached at andreas@open7x0.org
#
# The project's page is at http://www.open7x0.org
#
# $Id: siemens-linux-kernel.mk 401 2009-03-26 12:29:09Z andreas $
#

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
SIEMENS-LINUX-KERNEL_DEPS = $(BASE_BUILD_STAGEFILE) $(SIEMENS-GPL-SRC_DLFILE) $(EGCS_INSTALLED)
ifeq ($(CONFIG_SQUASHFS_LZMA),y)
   SIEMENS-LINUX-KERNEL_DEPS += $(LZMA_BIN)
endif

ifeq ($(CONFIG_SIEMENS-LINUX-KERNEL),y)
ifneq ($(CONFIG_EGCS),y)
# Siemens has chosen this old compiler for compiling the kernel not me.
# IMO this is irresponsible.
   $(error dependency error: siemens-linux-kernel needs egcs enabled)
endif
endif

ifeq ($(CONFIG_GENERATE_SIEMENS-LINUX-KERNEL-IMG),y)
ifneq ($(CONFIG_SIEMENS-LINUX-KERNEL),y)
   $(error dependency error: generating kernel image needs siemens-linux-kernel enabled)
endif
endif
SIEMENS-LINUX-KERNEL-IMG = $(TOP_DIR)/$(or $(notdir \
   $(CONFIG_SIEMENS-LINUX-KERNEL-IMG)),siemens-linux-kernel-$(CONFIG_M7X0_TYPE).img)

SIEMENS-LINUX-KERNEL_PATCHES_DIR := $(PATCHES_DIR)/siemens-linux-kernel
SIEMENS-LINUX-KERNEL_BASEDIR := slin_$(CONFIG_M7X0_TYPE)
SIEMENS-LINUX-KERNEL_DIR := $(BUILD_DIR)/$(SIEMENS-LINUX-KERNEL_BASEDIR)
SIEMENS-LINUX-KERNEL_CONFIG := $(CONFIGS_DIR)/siemens-linux-kernel/$(CONFIG_M7X0_TYPE).config

SIEMENS-LINUX-KERNEL_INSTALLED = $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_installed


PACKS_RULES_$(CONFIG_SIEMENS-LINUX-KERNEL) += $(SIEMENS-LINUX-KERNEL_INSTALLED)
FILE_LISTS_$(CONFIG_SIEMENS-LINUX-KERNEL) += siemens-linux-kernel_$(CONFIG_M7X0_TYPE).lst
POST_RULES_$(CONFIG_GENERATE_SIEMENS-LINUX-KERNEL-IMG) += $(SIEMENS-LINUX-KERNEL-IMG)

CLEAN_RULES += clean-siemens-linux-kernel
DISTCLEAN_RULES += distclean-siemens-linux-kernel

SIEMENS-LINUX-KERNEL_MODLST := \
   lib/modules/2.4.21-xfs/kernel/fs/cifs/cifs.o \
   lib/modules/2.4.21-xfs/kernel/fs/ext2/ext2.o \
   lib/modules/2.4.21-xfs/kernel/fs/ext3/ext3.o \
   lib/modules/2.4.21-xfs/kernel/fs/fat/fat.o \
   lib/modules/2.4.21-xfs/kernel/fs/jbd/jbd.o \
   lib/modules/2.4.21-xfs/kernel/fs/lockd/lockd.o \
   lib/modules/2.4.21-xfs/kernel/fs/nfs/nfs.o \
   lib/modules/2.4.21-xfs/kernel/fs/nls/nls_iso8859-15.o \
   lib/modules/2.4.21-xfs/kernel/fs/vfat/vfat.o \
   lib/modules/2.4.21-xfs/kernel/net/packet/af_packet.o \
   lib/modules/2.4.21-xfs/kernel/net/sunrpc/sunrpc.o \
   lib/modules/2.4.21-xfs/kernel/drivers/scsi/scsi_mod.o \
   lib/modules/2.4.21-xfs/kernel/drivers/scsi/sd_mod.o \
   lib/modules/2.4.21-xfs/kernel/drivers/usb/usbcore.o \
   lib/modules/2.4.21-xfs/kernel/drivers/usb/host/ehci-hcd.o \
   lib/modules/2.4.21-xfs/kernel/drivers/usb/host/usb-ohci.o \
   lib/modules/2.4.21-xfs/kernel/drivers/usb/storage/usb-storage.o

ifeq ($(CONFIG_PPTPD),y)
   SIEMENS-LINUX-KERNEL_MODLST += \
	lib/modules/2.4.21-xfs/kernel/crypto/sha1.o \
	lib/modules/2.4.21-xfs/kernel/crypto/arc4.o \
	lib/modules/2.4.21-xfs/kernel/drivers/net/slhc.o \
	lib/modules/2.4.21-xfs/kernel/drivers/net/ppp_generic.o \
	lib/modules/2.4.21-xfs/kernel/drivers/net/ppp_async.o \
	lib/modules/2.4.21-xfs/kernel/drivers/net/ppp_mppe.o
endif

SIEMENS-LINUX-KERNEL_DIRLST := \
   lib/modules \
   lib/modules/2.4.21-xfs \
   lib/modules/2.4.21-xfs/kernel \
   lib/modules/2.4.21-xfs/kernel/fs \
   lib/modules/2.4.21-xfs/kernel/fs/cifs \
   lib/modules/2.4.21-xfs/kernel/fs/ext2 \
   lib/modules/2.4.21-xfs/kernel/fs/ext3 \
   lib/modules/2.4.21-xfs/kernel/fs/fat \
   lib/modules/2.4.21-xfs/kernel/fs/jbd \
   lib/modules/2.4.21-xfs/kernel/fs/lockd \
   lib/modules/2.4.21-xfs/kernel/fs/nfs \
   lib/modules/2.4.21-xfs/kernel/fs/nls \
   lib/modules/2.4.21-xfs/kernel/fs/vfat \
   lib/modules/2.4.21-xfs/kernel/net \
   lib/modules/2.4.21-xfs/kernel/net/packet \
   lib/modules/2.4.21-xfs/kernel/net/sunrpc \
   lib/modules/2.4.21-xfs/kernel/drivers \
   lib/modules/2.4.21-xfs/kernel/drivers/scsi \
   lib/modules/2.4.21-xfs/kernel/drivers/usb \
   lib/modules/2.4.21-xfs/kernel/drivers/usb/host \
   lib/modules/2.4.21-xfs/kernel/drivers/usb/storage

ifeq ($(CONFIG_PPTPD),y)
   SIEMENS-LINUX-KERNEL_DIRLST += \
	lib/modules/2.4.21-xfs/kernel/crypto \
	lib/modules/2.4.21-xfs/kernel/drivers/net
endif

#
# unpack siemens linux kernel
#

$(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_unpacked: \
      $(wildcard $(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/common/*.patch) \
      $(wildcard $(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/$(CONFIG_M7X0_TYPE)/*.patch) \
      $(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/common/cifs-fs.tar.bz2 \
      $$(SIEMENS-LINUX-KERNEL_DEPS) $(SIEMENS-LINUX-KERNEL_CONFIG)
	-$(RM) -rf $(SIEMENS-LINUX-KERNEL_DIR)
	$(BZCAT) $(SIEMENS-GPL-SRC_DLFILE) | $(TAR) -C $(BUILD_DIR) \
		-f - gigaset_m740av_gplsw/linux-2.4-xfs/linux
	$(MV) $(BUILD_DIR)/gigaset_m740av_gplsw/linux-2.4-xfs/linux $(SIEMENS-LINUX-KERNEL_DIR)
	$(RMDIR) $(BUILD_DIR)/gigaset_m740av_gplsw/linux-2.4-xfs
	$(RMDIR) $(BUILD_DIR)/gigaset_m740av_gplsw
	$(TOUCH) $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_unpacked

#
# patch siemens linux kernel
#

$(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_patched: \
      $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_unpacked
# CIFS for 2.4er kernel (it's much easier to handle as smbfs and faster)
	$(BZCAT) $(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/common/cifs-fs.tar.bz2 | \
		 $(TAR) -C $(SIEMENS-LINUX-KERNEL_DIR) -f -
# Needed patches to get kernel compiling
	$(call patch_package, $(SIEMENS-LINUX-KERNEL_DIR), \
		$(SIEMENS-LINUX-KERNEL_PATCHES_DIR)/common)
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
	$(TOUCH) $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_patched

#
# configure siemens linux kernel
#

$(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_configured: \
      $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_patched
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
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) CROSS_COMPILE=$(TARGET)- ARCH=mips \
		CC=$(EGCS_BIN) -C $(SIEMENS-LINUX-KERNEL_DIR) distclean
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
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) CROSS_COMPILE=$(TARGET)- ARCH=mips \
		BASH=$(BASH) CC=$(EGCS_BIN) -C $(SIEMENS-LINUX-KERNEL_DIR) oldconfig
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) CROSS_COMPILE=$(TARGET)- ARCH=mips \
		CC=$(EGCS_BIN) -C $(SIEMENS-LINUX-KERNEL_DIR) dep
	# this file is missed (and empty) should be autogenerated
	$(TOUCH) $(SIEMENS-LINUX-KERNEL_DIR)/include/asm-mips/setup.h
	$(TOUCH) $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_configured

#
# compile siemens linux kernel
#

$(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_compiled: \
      $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_configured
# we need a unpack kernel as the bootloader cannot read elf binaries
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) CROSS_COMPILE=$(TARGET)- ARCH=mips \
		CC=$(EGCS_BIN) -C $(SIEMENS-LINUX-KERNEL_DIR) vmlinux
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) CROSS_COMPILE=$(TARGET)- ARCH=mips \
		CC=$(EGCS_BIN) -C $(SIEMENS-LINUX-KERNEL_DIR) modules
	$(TOUCH) $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_compiled

#
# install siemens linux kernel
#

$(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_installed: \
      $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_compiled
	$(MKDIR) -p $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)
	$(CP) -f $(SIEMENS-LINUX-KERNEL_DIR)/vmlinux $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)
	$(CAT) $(SIEMENS-LINUX-KERNEL_DIR)/System.map | $(SED) -e "s/^ffffffff//" > \
		$(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/System.map
	$(RM) $(SIEMENS-LINUX-KERNEL_DIR)/System.map
# This is what siemens is doing with the kernel. bootloader cannot handle otherwise
	$(PREFIX_BIN)/$(UCLIBC_OBJCOPY) --strip-all -O binary \
		$(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux \
		$(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux.bin
	-$(RM) -f $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux.bin.gz
	$(GZIP) -1 $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux.bin
	PATH='$(PREFIX_BIN):$(PATH)' $(MAKE) CROSS_COMPILE=$(TARGET)- ARCH=mips \
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
	$(TOUCH) $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_installed


#
# generate siemens linux kernel flash image
#
$(SIEMENS-LINUX-KERNEL-IMG): \
      $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_installed \
      $(GEN_KERNEL_HEADER_BIN)
	($(GEN_KERNEL_HEADER_BIN) $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux.bin.gz \
		$(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/System.map && \
		cat $(TARGET_ROOT)/$(M7X0_KERNEL_DIR)/vmlinux.bin.gz ) \
	> $(SIEMENS-LINUX-KERNEL-IMG)
ifeq ($(strip $(HOST_BS)), OpenBSD)
	@if	$(TEST) `$(STAT) -f b $(SIEMENS-LINUX-KERNEL-IMG)` \
			-gt $(KERNEL_MAX_IMGSIZE); then \
		$(ECHO) kernel image too big!; \
		$(RM) -f $(SIEMENS-LINUX-KERNEL-IMG); \
		exit 1; \
	fi
else
	@if	$(TEST) `$(STAT) --format="%s" $(SIEMENS-LINUX-KERNEL-IMG)` \
			-gt $(KERNEL_MAX_IMGSIZE); then \
		$(ECHO) kernel image too big!; \
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

$(FILELIST_DIR)/siemens-linux-kernel_$(CONFIG_M7X0_TYPE).lst: \
      $(STAGEFILES_DIR)/.siemens-linux-kernel_$(CONFIG_M7X0_TYPE)_installed
	($(foreach dir,$(SIEMENS-LINUX-KERNEL_DIRLST),\
		$(call siemens-linux-kernel_lst_dir,$(dir))) \
	 $(foreach mod,$(SIEMENS-LINUX-KERNEL_MODLST),\
		$(call siemens-linux-kernel_lst_mod,$(mod)))) \
			> $(FILELIST_DIR)/siemens-linux-kernel_$(CONFIG_M7X0_TYPE).lst

.PHONY: clean-siemens-linux-kernel distclean-siemens-linux-kernel


clean-siemens-linux-kernel:
	-$(RM) -rf $(SIEMENS-LINUX-KERNEL_DIR)

distclean-siemens-linux-kernel:
	-$(RM) -f $(STAGEFILES_DIR)/.siemens-linux-kernel_*
	-$(RM) -f $(SIEMENS-LINUX-KERNEL-IMG)
	-$(RM) -f $(FILELIST_DIR)/siemens-linux-kernel_*.lst

