# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006-2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2021 VDR-NG-EM Project
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
SSMTP_DEPS = $(BASE_BUILD_STAGEFILE)

SSMTP_VERSION := 2.64
SSMTP_PATCHES_DIR := $(PATCHES_DIR)/ssmtp/$(SSMTP_VERSION)

SSMTP_FILE := ssmtp_$(SSMTP_VERSION).orig.tar.bz2
SSMTP_DLFILE := $(DOWNLOAD_DIR)/$(SSMTP_FILE)
SSMTP_URL := http://ftp.de.debian.org/debian/pool/main/s/ssmtp/$(SSMTP_FILE)
SSMTP_DIR := $(BUILD_DIR)/ssmtp-$(SSMTP_VERSION)

SSMTP_INSTALLED = $(STAGEFILES_DIR)/.ssmtp_installed

PACKS_RULES_$(CONFIG_SSMTP) += $(SSMTP_INSTALLED)
FILE_LISTS_$(CONFIG_SSMTP) += ssmtp.lst

CLEAN_RULES += clean-ssmtp
DISTCLEAN_RULES += distclean-ssmtp

#
# download ssmtp
#

$(SSMTP_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(SSMTP_DLFILE) ] ; then \
	$(WGET) $(SSMTP_URL) -O $(SSMTP_DLFILE) ; \
	fi );
	$(TOUCH) $(SSMTP_DLFILE)

#
# unpack ssmtp
#

$(STAGEFILES_DIR)/.ssmtp_unpacked: $(SSMTP_DLFILE) \
                                           $(wildcard $(SSMTP_PATCHES_DIR)/*.patch) \
                                           $$(SSMTP_DEPS)
	-$(RM) -rf $(SSMTP_DIR)
	$(TAR) -C $(BUILD_DIR) -jf $(SSMTP_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.ssmtp_unpacked

#
# patch ssmtp
#

$(STAGEFILES_DIR)/.ssmtp_patched: $(STAGEFILES_DIR)/.ssmtp_unpacked
	$(call patch_package, $(SSMTP_DIR), $(SSMTP_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.ssmtp_patched

#
# configure ssmtp
#

$(STAGEFILES_DIR)/.ssmtp_configured: $(STAGEFILES_DIR)/.ssmtp_patched
	($(CD) $(SSMTP_DIR) ; $(UCLIBC_ENV_LTO_GC) \
		$(SSMTP_DIR)/configure \
			--prefix=$(TARGET_ROOT) \
			--sysconfdir="/etc" \
			--enable-md5auth)
	$(TOUCH) $(STAGEFILES_DIR)/.ssmtp_configured

#
# compile ssmtp
#

$(STAGEFILES_DIR)/.ssmtp_compiled: $(STAGEFILES_DIR)/.ssmtp_configured
	$(UCLIBC_ENV_LTO_GC) $(MAKE) \
		-C $(SSMTP_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.ssmtp_compiled

#
# install ssmtp
#

$(STAGEFILES_DIR)/.ssmtp_installed: $(STAGEFILES_DIR)/.ssmtp_compiled
	cp $(SSMTP_DIR)/ssmtp $(TARGET_ROOT)/usr/sbin/ssmtp
	chmod 755 $(TARGET_ROOT)/usr/sbin/ssmtp
	mkdir -p $(TARGET_ROOT)/etc/ssmtp
	cp $(SSMTP_DIR)/revaliases $(TARGET_ROOT)/etc/ssmtp/revaliases
	chmod 644 $(TARGET_ROOT)/etc/ssmtp/revaliases
	cp $(SSMTP_DIR)/ssmtp.conf $(TARGET_ROOT)/etc/ssmtp/ssmtp.conf
	chmod 644 $(TARGET_ROOT)/etc/ssmtp/ssmtp.conf
	$(TOUCH) $(STAGEFILES_DIR)/.ssmtp_installed


$(FILELIST_DIR)/ssmtp.lst: $(STAGEFILES_DIR)/.ssmtp_installed
	$(TOUCH) $(FILELIST_DIR)/ssmtp.lst

.PHONY: clean-ssmtp distclean-ssmtp

clean-ssmtp:
	-$(RM) -rf $(SSMTP_DIR)

#
# clean everthing else
#

distclean-ssmtp:
	-$(RM) -f $(STAGEFILES_DIR)/.ssmtp_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.ssmtp_patched
	-$(RM) -f $(STAGEFILES_DIR)/.ssmtp_configured
	-$(RM) -f $(STAGEFILES_DIR)/.ssmtp_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.ssmtp_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(SSMTP_DLFILE)
endif
