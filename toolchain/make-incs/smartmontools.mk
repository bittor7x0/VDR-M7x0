# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2013-2024 VDR-NG-EM Project
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
SMARTMONTOOLS_DEPS = $(BASE_BUILD_STAGEFILE)

SMARTMONTOOLS_VERSION := 6.2
SMARTMONTOOLS_PATCHES_DIR := $(PATCHES_DIR)/smartmontools/$(SMARTMONTOOLS_VERSION)

SMARTMONTOOLS_FILE := smartmontools-$(SMARTMONTOOLS_VERSION).tar.gz
SMARTMONTOOLS_DLFILE := $(DOWNLOAD_DIR)/$(SMARTMONTOOLS_FILE)
SMARTMONTOOLS_URL := http://sourceforge.net/projects/smartmontools/files/smartmontools/$(SMARTMONTOOLS_VERSION)/$(SMARTMONTOOLS_FILE)/download
SMARTMONTOOLS_DIR := $(BUILD_DIR)/smartmontools-$(SMARTMONTOOLS_VERSION)

SMARTMONTOOLS_INSTALLED = $(STAGEFILES_DIR)/.smartmontools_installed

PACKS_RULES_$(CONFIG_SMARTMONTOOLS) += $(SMARTMONTOOLS_INSTALLED)
FILE_LISTS_$(CONFIG_SMARTMONTOOLS) += smartmontools.lst  

CLEAN_RULES += clean-smartmontools
DISTCLEAN_RULES += distclean-smartmontools

#
# download smartmontools
#

$(SMARTMONTOOLS_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(SMARTMONTOOLS_DLFILE) ] ; then \
	$(WGET) $(SMARTMONTOOLS_URL) -O $(SMARTMONTOOLS_DLFILE) ; \
	fi );
	$(TOUCH) $(SMARTMONTOOLS_DLFILE)

#
# unpack smartmontools
#

$(STAGEFILES_DIR)/.smartmontools_unpacked: $(SMARTMONTOOLS_DLFILE) \
                                           $(wildcard $(SMARTMONTOOLS_PATCHES_DIR)/*.patch) \
                                           $$(SMARTMONTOOLS_DEPS)
	-$(RM) -rf $(SMARTMONTOOLS_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(SMARTMONTOOLS_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.smartmontools_unpacked

#
# patch smartmontools
#

$(STAGEFILES_DIR)/.smartmontools_patched: $(STAGEFILES_DIR)/.smartmontools_unpacked
	$(call patch_package, $(SMARTMONTOOLS_DIR), $(SMARTMONTOOLS_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.smartmontools_patched

#
# configure smartmontools
#

$(STAGEFILES_DIR)/.smartmontools_configured: $(STAGEFILES_DIR)/.smartmontools_patched
	($(CD) $(SMARTMONTOOLS_DIR) ; $(UCLIBC_ENV) $(SMARTMONTOOLS_DIR)/configure \
			--prefix=$(TARGET_ROOT) \
			--host=$(TARGET) \
			--sysconfdir=/etc \
			--with-drivedbdir=/etc \
			CXXFLAGS="$(UCLIBC_CXXFLAGS) -fno-builtin -fno-rtti" \
			CPPFLAGS="$(UCLIBC_CXXFLAGS) -I$(BUILD_DIR)/m7x0-linux-headers/include" \
			LDFLAGS="$(UCLIBC_LDFLAGS)")
	$(TOUCH) $(STAGEFILES_DIR)/.smartmontools_configured

#
# compile smartmontools
#

$(STAGEFILES_DIR)/.smartmontools_compiled: $(STAGEFILES_DIR)/.smartmontools_configured
	$(UCLIBC_ENV) $(MAKE) -C $(SMARTMONTOOLS_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.smartmontools_compiled

#
# install smartmontools
#

$(STAGEFILES_DIR)/.smartmontools_installed: $(STAGEFILES_DIR)/.smartmontools_compiled
	$(CP) -f $(SMARTMONTOOLS_DIR)/smartctl $(TARGET_ROOT)/sbin/smartctl
	$(CP) -f $(SMARTMONTOOLS_DIR)/smartd $(TARGET_ROOT)/sbin/smartd
	$(CP) -f $(SMARTMONTOOLS_DIR)/smartd.conf $(TARGET_ROOT)/etc/smartd.conf
	$(CP) -f $(SMARTMONTOOLS_DIR)/drivedb.h $(TARGET_ROOT)/etc/smart_drivedb.h
	$(TOUCH) $(STAGEFILES_DIR)/.smartmontools_installed

$(FILELIST_DIR)/smartmontools.lst: $(STAGEFILES_DIR)/.smartmontools_installed
	$(TOUCH) $(FILELIST_DIR)/smartmontools.lst

.PHONY: clean-smartmontools distclean-smartmontools

clean-smartmontools:
	-$(RM) -rf $(SMARTMONTOOLS_DIR)

#
# clean everthing else
#

distclean-smartmontools:
	-$(RM) -f $(STAGEFILES_DIR)/.smartmontools_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.smartmontools_patched
	-$(RM) -f $(STAGEFILES_DIR)/.smartmontools_configured
	-$(RM) -f $(STAGEFILES_DIR)/.smartmontools_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.smartmontools_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(SMARTMONTOOLS_DLFILE)
endif
