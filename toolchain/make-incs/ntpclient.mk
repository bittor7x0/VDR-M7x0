# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016 VDR-NG-EM Project
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

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
NTPCLIENT_DEPS = $(BASE_BUILD_STAGEFILE)

NTPCLIENT_VERSION := 2010_365
NTPCLIENT_YEAR := 2010
NTPCLIENT_PATCHES_DIR := $(PATCHES_DIR)/ntpclient/$(NTPCLIENT_VERSION)

NTPCLIENT_FILE := ntpclient_$(NTPCLIENT_VERSION).tar.gz
NTPCLIENT_DLFILE := $(DOWNLOAD_DIR)/$(NTPCLIENT_FILE)
NTPCLIENT_URL := http://www.spectrumcontrols.com/OpenSource/$(NTPCLIENT_FILE)
NTPCLIENT_DIR := $(BUILD_DIR)/ntpclient-$(NTPCLIENT_YEAR)

NTPCLIENT_INSTALLED = $(STAGEFILES_DIR)/.ntpclient_installed

PACKS_RULES_$(CONFIG_NTPCLIENT) += $(NTPCLIENT_INSTALLED)
FILE_LISTS_$(CONFIG_NTPCLIENT) += ntpclient.lst

CLEAN_RULES += clean-ntpclient
DISTCLEAN_RULES += distclean-ntpclient

#
# download ntpclient
#

$(NTPCLIENT_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(NTPCLIENT_DLFILE) ] ; then \
	$(WGET) $(NTPCLIENT_URL) -O $(NTPCLIENT_DLFILE) ; \
	fi );
	$(TOUCH) $(NTPCLIENT_DLFILE)

#
# unpack ntpclient
#

$(STAGEFILES_DIR)/.ntpclient_unpacked: $(NTPCLIENT_DLFILE) \
                                           $(wildcard $(NTPCLIENT_PATCHES_DIR)/*.patch) \
                                           $$(NTPCLIENT_DEPS)
	-$(RM) -rf $(NTPCLIENT_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(NTPCLIENT_DLFILE)
	$(MV) $(BUILD_DIR)/ntpclient $(NTPCLIENT_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.ntpclient_unpacked

#
# patch ntpclient
#

$(STAGEFILES_DIR)/.ntpclient_patched: $(STAGEFILES_DIR)/.ntpclient_unpacked
	$(call patch_package, $(NTPCLIENT_DIR), $(NTPCLIENT_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.ntpclient_patched

#
# compile ntpclient
#

$(STAGEFILES_DIR)/.ntpclient_compiled: $(STAGEFILES_DIR)/.ntpclient_patched
	$(UCLIBC_ENV_LTO_GC) $(MAKE) \
		-C $(NTPCLIENT_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.ntpclient_compiled

#
# install ntpclient
#

$(STAGEFILES_DIR)/.ntpclient_installed: $(STAGEFILES_DIR)/.ntpclient_compiled
	$(MKDIR) -p $(TARGET_ROOT)/usr/sbin
	$(CP) $(NTPCLIENT_DIR)/ntpclient $(TARGET_ROOT)/usr/sbin/ntpclient
	$(MKDIR) -p $(TARGET_ROOT)/etc/ntp
	$(TOUCH) $(STAGEFILES_DIR)/.ntpclient_installed


$(FILELIST_DIR)/ntpclient.lst: $(STAGEFILES_DIR)/.ntpclient_installed
	$(TOUCH) $(FILELIST_DIR)/ntpclient.lst

.PHONY: clean-ntpclient distclean-ntpclient

clean-ntpclient:
	-$(RM) -rf $(NTPCLIENT_DIR)

#
# clean everthing else
#

distclean-ntpclient:
	-$(RM) -f $(STAGEFILES_DIR)/.ntpclient_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.ntpclient_patched
	-$(RM) -f $(STAGEFILES_DIR)/.ntpclient_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.ntpclient_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(NTPCLIENT_DLFILE)
endif
