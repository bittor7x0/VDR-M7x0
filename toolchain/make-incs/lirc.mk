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

LIRC_DEPS = $(BASE_BUILD_STAGEFILE)

LIRC_VERSION := 0.6.4
LIRC_PATCHES_DIR := $(PATCHES_DIR)/lirc/$(LIRC_VERSION)
LIRC_FILE := lirc-$(LIRC_VERSION).tar.bz2
LIRC_DLFILE := $(DOWNLOAD_DIR)/$(LIRC_FILE)
LIRC_URL := http://sourceforge.net/projects/lirc/files/LIRC/$(LIRC_VERSION)/$(LIRC_FILE)/download
LIRC_DIR := $(BUILD_DIR)/lirc-$(LIRC_VERSION)

LIRC_INSTALLED = $(STAGEFILES_DIR)/.lirc_installed

PACKS_RULES_$(CONFIG_LIRC) += $(LIRC_INSTALLED)
FILE_LISTS_$(CONFIG_LIRC) += lirc.lst

CLEAN_RULES += clean-lirc
DISTCLEAN_RULES += distclean-lirc

#
# download lirc
#

$(LIRC_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LIRC_DLFILE) ] ; then \
	$(WGET) $(LIRC_URL) -O $(LIRC_DLFILE) ; \
	fi );
	$(TOUCH) $(LIRC_DLFILE)

#
# unpack lirc
#

$(STAGEFILES_DIR)/.lirc_unpacked: $(LIRC_DLFILE) \
                                  $(wildcard $(LIRC_PATCHES_DIR)/*.patch) \
                                  $$(LIRC_DEPS)
	-$(RM) -rf $(LIRC_DIR)
	$(BZCAT) $(LIRC_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.lirc_unpacked

#
# patch lirc
#

$(STAGEFILES_DIR)/.lirc_patched: $(STAGEFILES_DIR)/.lirc_unpacked
	$(call patch_package, $(LIRC_DIR), $(LIRC_PATCHES_DIR)/common)
	$(call patch_package, $(LIRC_DIR), $(LIRC_PATCHES_DIR)/host/$(HOST_BS))
	$(TOUCH) $(STAGEFILES_DIR)/.lirc_patched

#
# configure lirc
#			

$(STAGEFILES_DIR)/.lirc_configured: $(STAGEFILES_DIR)/.lirc_patched
	($(CD) $(LIRC_DIR) ; $(UCLIBC_ENV_LTO_GC) \
		./configure \
			--prefix=$(TARGET_ROOT)/usr \
			--build=$(HOST_TARGET) \
			--host=$(TARGET) \
			--target=$(TARGET) \
			--with-gnu-ld \
			--with-driver=any \
			--without-x \
			--with-devdir=/var/tmp \
			--with-syslog=LOG_DAEMON \
			--without-kerneldir)
	$(TOUCH) $(STAGEFILES_DIR)/.lirc_configured

#
# compile lirc
#

$(STAGEFILES_DIR)/.lirc_compiled: $(STAGEFILES_DIR)/.lirc_configured
	$(MAKE) -C $(LIRC_DIR) $(UCLIBC_ENV_LTO_GC) all
	$(TOUCH) $(STAGEFILES_DIR)/.lirc_compiled

#
# install lirc
#

$(STAGEFILES_DIR)/.lirc_installed: $(STAGEFILES_DIR)/.lirc_compiled
	$(MAKE) -C $(LIRC_DIR) $(UCLIBC_ENV_LTO_GC) install
	$(TOUCH) $(STAGEFILES_DIR)/.lirc_installed


$(FILELIST_DIR)/lirc.lst: $(STAGEFILES_DIR)/.lirc_installed
	$(TOUCH) $(FILELIST_DIR)/lirc.lst

.PHONY: clean-lirc distclean-lirc

clean-lirc:
	-$(RM) -rf $(LIRC_DIR)

distclean-lirc:
	-$(RM) -f $(STAGEFILES_DIR)/.lirc_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.lirc_patched
	-$(RM) -f $(STAGEFILES_DIR)/.lirc_configured
	-$(RM) -f $(STAGEFILES_DIR)/.lirc_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.lirc_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LIRC_DLFILE)
endif
