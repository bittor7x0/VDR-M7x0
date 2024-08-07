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
VSFTPD_DEPS = $(BASE_BUILD_STAGEFILE)

VSFTPD_VERSION := 3.0.5
VSFTPD_PATCHES_DIR := $(PATCHES_DIR)/vsftpd/$(VSFTPD_VERSION)

VSFTPD_FILE := vsftpd-$(VSFTPD_VERSION).tar.gz
VSFTPD_DLFILE := $(DOWNLOAD_DIR)/$(VSFTPD_FILE)
VSFTPD_URL := https://security.appspot.com/downloads/$(VSFTPD_FILE)
VSFTPD_DIR := $(BUILD_DIR)/vsftpd-$(VSFTPD_VERSION)

VSFTPD_INSTALLED = $(STAGEFILES_DIR)/.vsftpd_installed

PACKS_RULES_$(CONFIG_VSFTPD) += $(VSFTPD_INSTALLED)
FILE_LISTS_$(CONFIG_VSFTPD) += vsftpd.lst

CLEAN_RULES += clean-vsftpd
DISTCLEAN_RULES += distclean-vsftpd

#
# download vsftpd
#

$(VSFTPD_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(VSFTPD_DLFILE) ] ; then \
	$(WGET) $(VSFTPD_URL) -O $(VSFTPD_DLFILE) ; \
	fi );
	$(TOUCH) $(VSFTPD_DLFILE)

#
# unpack vsftpd
#

$(STAGEFILES_DIR)/.vsftpd_unpacked: $(VSFTPD_DLFILE) \
                                           $(wildcard $(VSFTPD_PATCHES_DIR)/*.patch) \
                                           $$(VSFTPD_DEPS)
	-$(RM) -rf $(VSFTPD_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(VSFTPD_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.vsftpd_unpacked

#
# patch vsftpd
#

$(STAGEFILES_DIR)/.vsftpd_patched: $(STAGEFILES_DIR)/.vsftpd_unpacked
	$(call patch_package, $(VSFTPD_DIR), $(VSFTPD_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.vsftpd_patched

#
# compile vsftpd
#

$(STAGEFILES_DIR)/.vsftpd_compiled: $(STAGEFILES_DIR)/.vsftpd_patched
	$(MAKE) -C $(VSFTPD_DIR) $(UCLIBC_ENV_LTO_GC) vsftpd
	$(TOUCH) $(STAGEFILES_DIR)/.vsftpd_compiled

#
# install vsftpd
#

$(STAGEFILES_DIR)/.vsftpd_installed: $(STAGEFILES_DIR)/.vsftpd_compiled
	$(CP) $(VSFTPD_DIR)/vsftpd $(TARGET_ROOT)/usr/sbin/vsftpd
	$(CP) $(VSFTPD_DIR)/vsftpd-m7x0.conf $(TARGET_ROOT)/etc/vsftpd.conf
	(if [ X"`$(GREP) vsftpd $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/inetd.conf`" = X"" ] ; then \
		$(SED) -i '$$aftp\tstream tcp nowait root /usr/sbin/vsftpd vsftpd' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/inetd.conf ; \
	fi);
	(if [ X"`$(GREP) nobody $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/passwd`" = X"" ] ; then \
		$(SED) -i '$$anobody::3:3:ftp:/:/bin/ash' $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/passwd ; \
	fi);
	(if [ X"`$(GREP) vsftpd $(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.mini`" = X"" ] ; then \
		$(SED) -i -e 's,^/bin/mkdir -p -m 755 /var/run /var/log /var/media /var/lock,/bin/mkdir -p -m 755 /var/run/vsftpd/empty /var/log /var/media /var/lock,g' \
			$(BUILDIN_DIR)/$(CONFIG_SCRIPT_BASE)/common/etc/rc.mini ; \
	fi);
	$(TOUCH) $(STAGEFILES_DIR)/.vsftpd_installed


$(FILELIST_DIR)/vsftpd.lst: $(STAGEFILES_DIR)/.vsftpd_installed
	$(TOUCH) $(FILELIST_DIR)/vsftpd.lst

.PHONY: clean-vsftpd distclean-vsftpd

clean-vsftpd:
	-$(RM) -rf $(VSFTPD_DIR)

#
# clean everthing else
#

distclean-vsftpd:
	-$(RM) -f $(STAGEFILES_DIR)/.vsftpd_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.vsftpd_patched
	-$(RM) -f $(STAGEFILES_DIR)/.vsftpd_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.vsftpd_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(VSFTPD_DLFILE)
endif
