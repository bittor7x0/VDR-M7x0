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

BUSYBOX_DEPS = $(BASE_BUILD_STAGEFILE)

BUSYBOX_VERSION := 1.17.2
BUSYBOX_PATCHES_DIR := $(PATCHES_DIR)/busybox/$(BUSYBOX_VERSION)

BUSYBOX_FILE := busybox-$(BUSYBOX_VERSION).tar.bz2
BUSYBOX_DLFILE := $(DOWNLOAD_DIR)/$(BUSYBOX_FILE)
BUSYBOX_DIR := $(BUILD_DIR)/busybox-$(BUSYBOX_VERSION)
BUSYBOX_URL := http://busybox.net/downloads/$(BUSYBOX_FILE)
BUSYBOX_INSTALLED = $(STAGEFILES_DIR)/.busybox_installed

BUSYBOX_CONFIG := $(CONFIGS_DIR)/busybox/$(BUSYBOX_VERSION)/busybox-$(CONFIG_FW_VERSION).config

PACKS_RULES_$(CONFIG_BUSYBOX) += $(BUSYBOX_INSTALLED)
FILE_LISTS_$(CONFIG_BUSYBOX) += busybox.lst

CLEAN_RULES += clean-busybox
DISTCLEAN_RULES += distclean-busybox

#
# download busybox
#

$(BUSYBOX_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(BUSYBOX_DLFILE) ] ; then \
	$(WGET) $(BUSYBOX_URL) -O $(BUSYBOX_DLFILE) ; \
	fi );
	$(TOUCH) $(BUSYBOX_DLFILE)


#
# unpack busybox
#

$(STAGEFILES_DIR)/.busybox_unpacked: $(BUSYBOX_DLFILE) \
                                     $(wildcard $(BUSYBOX_PATCHES_DIR)/common/*.patch) \
                                     $(wildcard $(BUSYBOX_PATCHES_DIR)/$(CONFIG_FW_VERSION)/*.patch) \
                                     $(BUSYBOX_CONFIG) $$(BUSYBOX_DEPS)
	-$(RM) -rf $(BUSYBOX_DIR)
	$(BZCAT) $(BUSYBOX_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(TOUCH) $(STAGEFILES_DIR)/.busybox_unpacked

#
# patch busybox
#

$(STAGEFILES_DIR)/.busybox_patched: $(STAGEFILES_DIR)/.busybox_unpacked
	$(call patch_package, $(BUSYBOX_DIR), $(BUSYBOX_PATCHES_DIR)/common)
	$(call patch_package, $(BUSYBOX_DIR), $(BUSYBOX_PATCHES_DIR)/$(CONFIG_FW_VERSION))
	$(TOUCH) $(STAGEFILES_DIR)/.busybox_patched

#
# configure busybox
#

$(STAGEFILES_DIR)/.busybox_configured: $(STAGEFILES_DIR)/.busybox_patched
	$(SED) -ne 's,^CONFIG_PREFIX=.*,CONFIG_PREFIX=\"$(TARGET_ROOT)\",g' \
		-e 'w$(BUSYBOX_DIR)/.config' $(BUSYBOX_CONFIG)
  ifeq ($(CONFIG_BASH),y)
	$(SED) -i -e 's,^CONFIG_FEATURE_SH_IS_ASH=.*,# CONFIG_FEATURE_SH_IS_ASH is not set,g' \
		$(BUSYBOX_DIR)/.config
	$(SED) -i -e 's,^# CONFIG_FEATURE_SH_IS_NONE.*,CONFIG_FEATURE_SH_IS_NONE=y,g' \
		$(BUSYBOX_DIR)/.config
  endif
	$(UCLIBC_ENV) $(MAKE) -C $(BUSYBOX_DIR) CROSS_COMPILE=mips-linux-uclibc- \
		ARCH=mips oldconfig
	$(TOUCH) $(STAGEFILES_DIR)/.busybox_configured

#
# compile busybox
#

$(STAGEFILES_DIR)/.busybox_compiled: $(STAGEFILES_DIR)/.busybox_configured
	$(UCLIBC_ENV) $(MAKE) -C $(BUSYBOX_DIR) CROSS_COMPILE=mips-linux-uclibc- \
		ARCH=mips all
	$(TOUCH) $(STAGEFILES_DIR)/.busybox_compiled

#
# install busybox
#

$(STAGEFILES_DIR)/.busybox_installed: $(STAGEFILES_DIR)/.busybox_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(BUSYBOX_DIR) CROSS_COMPILE=mips-linux-uclibc- \
		ARCH=mips install
	$(TOUCH) $(STAGEFILES_DIR)/.busybox_installed

#
# generate root file list for busybox
#

$(FILELIST_DIR)/busybox.lst: $(STAGEFILES_DIR)/.busybox_installed
	($(ECHO) "bin/busybox              bin/busybox        f 4755 0 0 - - - - -"; \
	 $(ECHO) "bin/busybox              -                  s    - - - - - - - -"; \
	 $(FIND) $(TARGET_ROOT) -lname "*busybox" \
			-printf "%-24P %-18l l 777 0 0 - - - - -\n") \
		> $(FILELIST_DIR)/busybox.lst

.PHONY: clean-busybox distclean-busybox

clean-busybox:
	-$(RM) -rf $(BUSYBOX_DIR)

distclean-busybox:
	-$(RM) -f $(STAGEFILES_DIR)/.busybox_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.busybox_patched
	-$(RM) -f $(STAGEFILES_DIR)/.busybox_configured
	-$(RM) -f $(STAGEFILES_DIR)/.busybox_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.busybox_installed
	-$(RM) -f $(FILELIST_DIR)/busybox.lst
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(BUSYBOX_DLFILE)
endif
