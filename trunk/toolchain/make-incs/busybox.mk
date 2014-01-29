# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006, 2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 VDR-NG-EM Project
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
# The project's page is at http://www.assembla.com/spaces/VDR-M7x0
# Our English forum is at  http://vdr-m7x0.foroactivo.com.es/f19-international-forum
# Our Spanish forum is at  http://vdr-m7x0.foroactivo.com.es
#
# --- VDR-NG-EM-COPYRIGHT-NOTE-END ---

BUSYBOX_DEPS = $(BASE_BUILD_STAGEFILE)

BUSYBOX_VERSION := 1.21.1
BUSYBOX_PATCHES_DIR := $(PATCHES_DIR)/busybox/$(BUSYBOX_VERSION)

BUSYBOX_FILE := busybox-$(BUSYBOX_VERSION).tar.bz2
BUSYBOX_DLFILE := $(DOWNLOAD_DIR)/$(BUSYBOX_FILE)
BUSYBOX_DIR := $(BUILD_DIR)/busybox-$(BUSYBOX_VERSION)_$(CONFIG_FW_VERSION)
BUSYBOX_URL := http://busybox.net/downloads/$(BUSYBOX_FILE)
BUSYBOX_INSTALLED = $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_installed

BUSYBOX_CONFIG := $(CONFIGS_DIR)/busybox/$(BUSYBOX_VERSION)/busybox-$(CONFIG_FW_VERSION).config

PACKS_RULES_$(CONFIG_BUSYBOX) += $(BUSYBOX_INSTALLED)
FILE_LISTS_$(CONFIG_BUSYBOX) += busybox_$(CONFIG_FW_VERSION).lst

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

$(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_unpacked: $(BUSYBOX_DLFILE) \
                                     $(wildcard $(BUSYBOX_PATCHES_DIR)/common/*.patch) \
                                     $(wildcard $(BUSYBOX_PATCHES_DIR)/$(CONFIG_FW_VERSION)/*.patch) \
                                     $(BUSYBOX_CONFIG) $$(BUSYBOX_DEPS)
	-$(RM) -rf $(BUSYBOX_DIR)
	-$(RM) -rf $(BUILD_DIR)/busybox-$(BUSYBOX_VERSION)
	$(BZCAT) $(BUSYBOX_DLFILE) | $(TAR) -C $(BUILD_DIR) -f -
	$(MV) $(BUILD_DIR)/busybox-$(BUSYBOX_VERSION) $(BUSYBOX_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_unpacked

#
# patch busybox
#

$(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_patched: $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_unpacked
	$(call patch_package, $(BUSYBOX_DIR), $(BUSYBOX_PATCHES_DIR)/common)
	$(call patch_package, $(BUSYBOX_DIR), $(BUSYBOX_PATCHES_DIR)/$(CONFIG_FW_VERSION))
	$(TOUCH) $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_patched

#
# configure busybox
#

$(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_configured: $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_patched
	$(SED) -ne 's,^CONFIG_PREFIX=.*,CONFIG_PREFIX=\"$(TARGET_ROOT)\",g' \
		-e 'w$(BUSYBOX_DIR)/.config' $(BUSYBOX_CONFIG)
  ifeq ($(CONFIG_BASH),y)
	$(SED) -i -e 's,^CONFIG_FEATURE_SH_IS_ASH=.*,# CONFIG_FEATURE_SH_IS_ASH is not set,g' \
		$(BUSYBOX_DIR)/.config
	$(SED) -i -e 's,^# CONFIG_FEATURE_SH_IS_NONE.*,CONFIG_FEATURE_SH_IS_NONE=y,g' \
		$(BUSYBOX_DIR)/.config
  endif
	$(UCLIBC_ENV) \
		$(MAKE) -C $(BUSYBOX_DIR) CROSS_COMPILE=mips-linux-uclibc- \
		LDLIBS="m" \
		PKG_CONFIG="pkg-config" \
		SKIP_STRIP=y \
		ARCH=mips oldconfig
	$(TOUCH) $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_configured

#
# compile busybox
#

$(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_compiled: $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_configured
	$(UCLIBC_ENV) \
		$(MAKE) -C $(BUSYBOX_DIR) CROSS_COMPILE=mips-linux-uclibc- \
		LDLIBS="m" \
		PKG_CONFIG="pkg-config" \
		SKIP_STRIP=y \
		ARCH=mips all
	$(TOUCH) $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_compiled

#
# install busybox
#

$(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_installed: $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_compiled \
	$(TOP_DIR)/.config
	$(FIND) $(TARGET_ROOT) -lname "*busybox" -exec rm \{\} \;
	$(UCLIBC_ENV) \
		$(MAKE) -C $(BUSYBOX_DIR) CROSS_COMPILE=mips-linux-uclibc- \
		LDLIBS="m" \
		PKG_CONFIG="pkg-config" \
		SKIP_STRIP=y \
		ARCH=mips install
	$(TOUCH) $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_installed

#
# generate root file list for busybox
#

$(FILELIST_DIR)/busybox_$(CONFIG_FW_VERSION).lst: $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_installed
	($(ECHO) "bin/busybox              bin/busybox        f 4755 0 0 - - - - -"; \
	 $(ECHO) "bin/busybox              -                  s    - - - - - - - -"; \
	 $(FIND) $(TARGET_ROOT) -lname "*busybox" \
			-printf "%-24P %-18l l 777 0 0 - - - - -\n") \
		> $(FILELIST_DIR)/busybox_$(CONFIG_FW_VERSION).lst

.PHONY: clean-busybox distclean-busybox

clean-busybox:
	-$(RM) -rf $(BUSYBOX_DIR)

distclean-busybox:
	-$(RM) -f $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_patched
	-$(RM) -f $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_configured
	-$(RM) -f $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.busybox_$(CONFIG_FW_VERSION)_installed
	-$(RM) -f $(FILELIST_DIR)/busybox_$(CONFIG_FW_VERSION).lst
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(BUSYBOX_DLFILE)
endif
