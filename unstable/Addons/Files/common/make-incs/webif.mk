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
#
# The project's page is at http://www.open7x0.org
#
#

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
WEBIF_DEPS = $(BASE_BUILD_STAGEFILE)

KLONE_VERSION := 2.1.1
KLONE_FILE := klone-$(KLONE_VERSION).tar.gz
KLONE_DLFILE := $(DOWNLOAD_DIR)/$(KLONE_FILE)
KLONE_URL := http://koanlogic.com/download/klone/$(KLONE_FILE)
LOGOS_FILE := logos-webif.tgz
LOGOS_DLFILE := $(DOWNLOAD_DIR)/logos-webif.3.tgz
LOGOS_URL := https://www.assembla.com/spaces/VDR-M7x0/documents/a3Ht1sqbur37laeJe5d-aX/download/$(LOGOS_FILE)

ifeq ($(strip $(CONFIG_WEBIF_TREE)),)
  WEBIF_BRANCH := branches/atinar-unstable
else
  WEBIF_BRANCH := $(or $(strip $(filter trunk,$(CONFIG_WEBIF_TREE))),branches/$(CONFIG_WEBIF_TREE))
endif
WEBIF_DIR := $(BUILD_DIR)/webif
WEBIF_TC_FILE := $(WEBIF_DIR)/linux-mips-uclibc.tc

LASTDIR := `pwd`

PACKS_RULES_$(CONFIG_WEBIF) += $(STAGEFILES_DIR)/.webif_installed
FILE_LISTS_$(CONFIG_WEBIF) += webif.lst

CLEAN_RULES += clean-webif
DISTCLEAN_RULES += distclean-webif

#
# download webif
#
$(KLONE_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(KLONE_DLFILE) ] ; then \
	$(WGET) $(KLONE_URL) -O $(KLONE_DLFILE) ; \
	fi );
	$(TOUCH) $(KLONE_DLFILE)

#
# unpack webif
#
$(STAGEFILES_DIR)/.webif_unpacked: $(KLONE_DLFILE) 
	-$(RM) -rf $(WEBIF_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(KLONE_DLFILE)
	$(MV) $(BUILD_DIR)/klone-$(KLONE_VERSION) $(WEBIF_DIR)
	-$(RM) -rf $(WEBIF_DIR)/webapp
	-$(RM) -rf $(WEBIF_DIR)/Makefile
	$(MV) $(WEBIF_DIR)/README $(WEBIF_DIR)/README.klone
	$(MV) $(WEBIF_DIR)/LICENSE $(WEBIF_DIR)/LICENSE.klone
	$(MV) $(WEBIF_DIR)/ChangeLog $(WEBIF_DIR)/ChangeLog.klone
	$(TOUCH) $(STAGEFILES_DIR)/.webif_unpacked

#
# webapp download
#
$(STAGEFILES_DIR)/.webapp_downloaded: $(STAGEFILES_DIR)/.webif_unpacked
	$(CP) -Trf ../webif/$(WEBIF_BRANCH) $(WEBIF_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.webapp_downloaded

#
# compile webif
#
$(STAGEFILES_DIR)/.webif_compiled: $(STAGEFILES_DIR)/.webapp_downloaded $$(WEBIF_DEPS)
	$(ECHO) \# gcc is in $(PREFIX_BIN) > $(WEBIF_TC_FILE)
	$(ECHO) CC = $(UCLIBC_CC) $(UCLIBC_CFLAGS) >> $(WEBIF_TC_FILE)
	$(ECHO) CXX = $(UCLIBC_CXX) $(UCLIBC_CFLAGS) >> $(WEBIF_TC_FILE)
	$(ECHO) AR = $(UCLIBC_AR) >> $(WEBIF_TC_FILE)
	$(ECHO) RANLIB = $(UCLIBC_RANLIB) >> $(WEBIF_TC_FILE)
	$(ECHO) LD = $(UCLIBC_LD) >> $(WEBIF_TC_FILE)
	$(ECHO) NM = $(UCLIBC_NM) >> $(WEBIF_TC_FILE)
	$(ECHO) STRIP = $(UCLIBC_STRIP) >> $(WEBIF_TC_FILE)
	$(ECHO) MAKE = $(MAKE) >> $(WEBIF_TC_FILE)
	$(CD) $(WEBIF_DIR) && \
		env PATH="$(PREFIX_BIN):$(PATH)" ./configure && \
		env PATH="$(PREFIX_BIN):$(PATH)" $(MAKE) clean && \
		env PATH="$(PREFIX_BIN):$(PATH)" KLONE_VERSION=$(KLONE_VERSION) XENO_NO_FETCH=1 $(MAKE)
	$(CD) $(LASTDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.webif_compiled

#
# download logos
#
$(LOGOS_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LOGOS_DLFILE) ] ; then \
	$(WGET) $(LOGOS_URL) -O $(LOGOS_DLFILE) ; \
	fi );
	$(TOUCH) $(LOGOS_DLFILE)

#
# install webif
#
$(STAGEFILES_DIR)/.webif_installed: $(STAGEFILES_DIR)/.webif_compiled $(LOGOS_DLFILE)
	$(MKDIR) -p $(TARGET_ROOT)/usr/sbin
	$(CP) $(WEBIF_DIR)/kloned $(TARGET_ROOT)/usr/sbin/webifd
	-$(RM) -rf $(PRG_CONFIGS_DIR)/vdr-m7x0/common/etc/webif/www/images
	$(MKDIR) -p $(PRG_CONFIGS_DIR)/vdr-m7x0/common/etc/webif/www/images
	$(TAR) -C $(PRG_CONFIGS_DIR)/vdr-m7x0/common/etc/webif/www/images -zf $(LOGOS_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.webif_installed

$(FILELIST_DIR)/webif.lst: $(STAGEFILES_DIR)/.webif_installed
	$(TOUCH) $(FILELIST_DIR)/webif.lst

.PHONY: clean-webif distclean-webif

clean-webif:
	(if [ -f $(WEBIF_DIR)/src ] ; then \
		$(CD) $(WEBIF_DIR) && env PATH="$(PREFIX_BIN):$(PATH)" $(MAKE) clean ; \
	fi );

distclean-webif:
	-$(RM) -f $(STAGEFILES_DIR)/.webif_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.webif_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.webif_installed
	-$(RM) -f $(STAGEFILES_DIR)/.webapp_downloaded
#	-$(RM) -f $(LOGOS_DLFILE)
	-$(RM) -rf $(PRG_CONFIGS_DIR)/vdr-m7x0/common/etc/webif/www/images
