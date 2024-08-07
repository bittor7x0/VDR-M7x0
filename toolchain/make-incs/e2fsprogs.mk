# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006-2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2024 VDR-NG-EM Project
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

E2FSPROGS_DEPS = $(BASE_BUILD_STAGEFILE)

E2FSPROGS_VERSION := 1.47.0
E2FSPROGS_PATCHES_DIR := $(PATCHES_DIR)/e2fsprogs/$(E2FSPROGS_VERSION)

E2FSPROGS_FILE := e2fsprogs-$(E2FSPROGS_VERSION).tar.gz
E2FSPROGS_DLFILE := $(DOWNLOAD_DIR)/$(E2FSPROGS_FILE)
E2FSPROGS_URL := http://sourceforge.net/projects/e2fsprogs/files/e2fsprogs/v$(E2FSPROGS_VERSION)/$(E2FSPROGS_FILE)/download
E2FSPROGS_DIR := $(BUILD_DIR)/e2fsprogs-$(E2FSPROGS_VERSION)
E2FSPROGS_BUILD_DIR := $(BUILD_DIR)/e2fsprogs.build

E2FSPROGS_INSTALLED = $(STAGEFILES_DIR)/.e2fsprogs_installed

PACKS_RULES_$(CONFIG_E2FSPROGS) += $(E2FSPROGS_INSTALLED)
FILE_LISTS_$(CONFIG_E2FSPROGS) += e2fsprogs.lst

CLEAN_RULES += clean-e2fsprogs
DISTCLEAN_RULES += distclean-e2fsprogs

#
# download e2fsprogs
#

$(E2FSPROGS_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(E2FSPROGS_DLFILE) ] ; then \
	$(WGET) $(E2FSPROGS_URL) -O $(E2FSPROGS_DLFILE) ; \
	fi );
	$(TOUCH) $(E2FSPROGS_DLFILE)

#
# unpack e2fsprogs
#

$(STAGEFILES_DIR)/.e2fsprogs_unpacked: $(E2FSPROGS_DLFILE) \
                                       $(wildcard $(E2FSPROGS_PATCHES_DIR)/*.patch)
	-$(RM) -rf $(E2FSPROGS_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(E2FSPROGS_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.e2fsprogs_unpacked

#
# patch e2fsprogs
#

$(STAGEFILES_DIR)/.e2fsprogs_patched: $(STAGEFILES_DIR)/.e2fsprogs_unpacked
	$(call patch_package, $(E2FSPROGS_DIR), $(E2FSPROGS_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.e2fsprogs_patched

#
# configure e2fsprogs
#

$(STAGEFILES_DIR)/.e2fsprogs_configured: $(STAGEFILES_DIR)/.e2fsprogs_patched \
                                         $$(E2FSPROGS_DEPS)
	-$(RM) -rf $(E2FSPROGS_BUILD_DIR)
	$(MKDIR) -p $(E2FSPROGS_BUILD_DIR)
	($(CD) $(E2FSPROGS_BUILD_DIR) ; $(UCLIBC_ENV_LTO_GC) \
		$(E2FSPROGS_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--sbindir=$(TARGET_ROOT)/sbin \
			--sysconfdir=$(TARGET_ROOT)/etc \
			--host=$(TARGET) \
			--enable-symlink-install \
			--disable-debugfs \
			--disable-backtrace \
			--disable-imager \
			--disable-resizer \
			--disable-fsck \
			--disable-defrag \
			--disable-e2initrd-helper \
			--disable-testio-debug \
			--disable-fuse2fs \
			--disable-mmp \
			--disable-tdb \
			--disable-bmap-stats \
			--disable-uuidd \
			--disable-rpath \
			--with-udev_rules_dir=no \
			--with-crond-dir=no \
			--with-systemd-unit-dir=no \
			$(if $(CONFIG_LIBUUID),--enable-elf-shlibs,--disable-elf-shlibs) \
			--disable-nls )
	$(TOUCH) $(STAGEFILES_DIR)/.e2fsprogs_configured

#
# compile e2fsprogs
#

$(STAGEFILES_DIR)/.e2fsprogs_compiled: $(STAGEFILES_DIR)/.e2fsprogs_configured
	$(UCLIBC_ENV_LTO_GC) $(MAKE) -C $(E2FSPROGS_BUILD_DIR) libs
	$(UCLIBC_ENV_LTO_GC) $(MAKE) -C $(E2FSPROGS_BUILD_DIR)/e2fsck e2fsck
	$(UCLIBC_ENV_LTO_GC) $(MAKE) -C $(E2FSPROGS_BUILD_DIR)/misc mke2fs
	#$(UCLIBC_ENV_LTO_GC) $(MAKE) -C $(E2FSPROGS_BUILD_DIR)/misc tune2fs
	$(TOUCH) $(STAGEFILES_DIR)/.e2fsprogs_compiled

#
# install e2fsprogs
#

$(STAGEFILES_DIR)/.e2fsprogs_installed: $(STAGEFILES_DIR)/.e2fsprogs_compiled
	$(CP) -f $(E2FSPROGS_BUILD_DIR)/e2fsck/e2fsck $(TARGET_ROOT)/sbin/e2fsck
	$(CP) -f $(E2FSPROGS_BUILD_DIR)/misc/mke2fs $(TARGET_ROOT)/sbin/mke2fs
	$(CP) -f $(E2FSPROGS_BUILD_DIR)/misc/mke2fs.conf $(TARGET_ROOT)/etc/mke2fs.conf
	$(CP) -f $(E2FSPROGS_DIR)/e2fsck.conf $(TARGET_ROOT)/etc/e2fsck.conf
	#$(CP) -f $(E2FSPROGS_BUILD_DIR)/misc/tune2fs $(TARGET_ROOT)/sbin/tune2fs
	$(TOUCH) $(STAGEFILES_DIR)/.e2fsprogs_installed


$(FILELIST_DIR)/e2fsprogs.lst: $(STAGEFILES_DIR)/.e2fsprogs_installed
	$(TOUCH) $(FILELIST_DIR)/e2fsprogs.lst

.PHONY: clean-e2fsprogs distclean-e2fsprogs

clean-e2fsprogs:
	-$(RM) -rf $(E2FSPROGS_BUILD_DIR)
	-$(RM) -rf $(E2FSPROGS_DIR)

distclean-e2fsprogs:
	-$(RM) -f $(STAGEFILES_DIR)/.e2fsprogs_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.e2fsprogs_patched
	-$(RM) -f $(STAGEFILES_DIR)/.e2fsprogs_configured
	-$(RM) -f $(STAGEFILES_DIR)/.e2fsprogs_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.e2fsprogs_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(E2FSPROGS_DLFILE)
endif
