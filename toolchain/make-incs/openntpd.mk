# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
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

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
OPENNTPD_DEPS = $(BASE_BUILD_STAGEFILE)

OPENNTPD_VERSION := 3.9p1
OPENNTPD_PATCHES_DIR := $(PATCHES_DIR)/openntpd/$(OPENNTPD_VERSION)

OPENNTPD_FILE := openntpd-$(OPENNTPD_VERSION).tar.gz
OPENNTPD_DLFILE := $(DOWNLOAD_DIR)/$(OPENNTPD_FILE)
OPENNTPD_URL := http://ftp.OpenBSD.org/pub/OpenBSD/OpenNTPD/$(OPENNTPD_FILE)
OPENNTPD_DIR := $(BUILD_DIR)/openntpd-$(OPENNTPD_VERSION)

OPENNTPD_INSTALLED = $(STAGEFILES_DIR)/.openntpd_installed

PACKS_RULES_$(CONFIG_OPENNTPD) += $(OPENNTPD_INSTALLED)
FILE_LISTS_$(CONFIG_OPENNTPD) += openntpd.lst

CLEAN_RULES += clean-openntpd
DISTCLEAN_RULES += distclean-openntpd

#
# download openntpd
#

$(OPENNTPD_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(OPENNTPD_DLFILE) ] ; then \
	$(WGET) $(OPENNTPD_URL) -O $(OPENNTPD_DLFILE) ; \
	fi );
	$(TOUCH) $(OPENNTPD_DLFILE)

#
# unpack openntpd
#

$(STAGEFILES_DIR)/.openntpd_unpacked: $(OPENNTPD_DLFILE) \
                                           $(wildcard $(OPENNTPD_PATCHES_DIR)/*.patch) \
                                           $$(OPENNTPD_DEPS)
	-$(RM) -rf $(OPENNTPD_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(OPENNTPD_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.openntpd_unpacked

#
# patch openntpd
#

$(STAGEFILES_DIR)/.openntpd_patched: $(STAGEFILES_DIR)/.openntpd_unpacked
	$(call patch_package, $(OPENNTPD_DIR), $(OPENNTPD_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.openntpd_patched

#
# configure openntpd
#

$(STAGEFILES_DIR)/.openntpd_configured: $(STAGEFILES_DIR)/.openntpd_patched
	($(CD) $(OPENNTPD_DIR) ; $(UCLIBC_ENV) \
		$(OPENNTPD_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--sysconfdir=/etc/ntp \
			--with-privsep-user=root)
	$(TOUCH) $(STAGEFILES_DIR)/.openntpd_configured

#
# compile openntpd
#

$(STAGEFILES_DIR)/.openntpd_compiled: $(STAGEFILES_DIR)/.openntpd_configured
	$(UCLIBC_ENV) $(MAKE) \
		-C $(OPENNTPD_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.openntpd_compiled

#
# install openntpd
#

$(STAGEFILES_DIR)/.openntpd_installed: $(STAGEFILES_DIR)/.openntpd_compiled
	$(CP) $(OPENNTPD_DIR)/ntpd $(TARGET_ROOT)/usr/sbin/ntpd
	$(MKDIR) -p $(TARGET_ROOT)/etc/ntp
	$(CP) $(OPENNTPD_DIR)/ntpd.conf $(TARGET_ROOT)/etc/ntp/ntpd.conf
	$(TOUCH) $(STAGEFILES_DIR)/.openntpd_installed


$(FILELIST_DIR)/openntpd.lst: $(STAGEFILES_DIR)/.openntpd_installed
	$(TOUCH) $(FILELIST_DIR)/openntpd.lst

.PHONY: clean-openntpd distclean-openntpd

clean-openntpd:
	-$(RM) -rf $(OPENNTPD_DIR)

#
# clean everthing else
#

distclean-openntpd:
	-$(RM) -f $(STAGEFILES_DIR)/.openntpd_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.openntpd_patched
	-$(RM) -f $(STAGEFILES_DIR)/.openntpd_configured
	-$(RM) -f $(STAGEFILES_DIR)/.openntpd_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.openntpd_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(OPENNTPD_DLFILE)
endif
