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
WEBIF_DEPS = $(BASE_BUILD_STAGEFILE)

ifeq ($(CONFIG_YUI_COMPRESSOR),y)
	WEBIF_DEPS +=  $(YUI_COMPRESSOR_JAR)
endif
ifeq ($(CONFIG_PNGOUT),y)
	WEBIF_DEPS +=  $(PNGOUT_BIN)
endif
ifeq ($(CONFIG_ZOPFLIPNG),y)
	WEBIF_DEPS +=  $(ZOPFLIPNG_BIN)
endif
ifeq ($(CONFIG_PNGWOLF-ZOPFLI),y)
	WEBIF_DEPS +=  $(PNGWOLF-ZOPFLI_BIN)
endif
ifeq ($(CONFIG_ECT),y)
	WEBIF_DEPS +=  $(ECT_BIN)
endif
ifeq ($(CONFIG_PINGO),y)
	WEBIF_DEPS +=  $(PINGO_BIN)
endif

WEBIF_DIR := $(BUILD_DIR)/webif
WEBIF_BUILD_DIR := $(BUILD_DIR)/webif.build
WEBIF_SVN := https://github.com/bittor7x0/VDR-M7x0/trunk/webif
WEBIF_TC_FILE := $(WEBIF_BUILD_DIR)/linux-mips-uclibc.tc
WEBIF_CONF_DIR := $(PRG_CONFIGS_DIR)/webif
WEBIF_KLAPP_VERSION := 2.0.2
WEBIF_KLONE_VERSION := 2.4.0

PACKS_RULES_$(CONFIG_WEBIF) += $(STAGEFILES_DIR)/.webif_installed
FILE_LISTS_$(CONFIG_WEBIF) += webif.lst webif-configs.lst

CLEAN_RULES += clean-webif
DISTCLEAN_RULES += distclean-webif

ifneq ($(and $(filter y,$(CONFIG_WEBIF)), $(wildcard $(WEBIF_DIR))),)
$(info Updating webif download ...)
webif_svn_changed := $(call svn_changed, $(WEBIF_DIR))

ifeq ($(webif_svn_changed),0)
$(info webif is up to date)
else
$(info $(webif_svn_changed) file(s) updated)
webif_tmp := $(shell $(TOUCH) $(STAGEFILES_DIR)/.webapp_downloaded)
endif
endif

#
# download webapp
#
$(STAGEFILES_DIR)/.webapp_downloaded: $(TC_INIT_RULE)
	(if [ ! -d $(WEBIF_DIR) ] ; then \
		$(SVN) co $(WEBIF_SVN) $(WEBIF_DIR) ; \
	else \
		$(SVN) up $(WEBIF_DIR) ; \
	fi );
	$(TOUCH) $(STAGEFILES_DIR)/.webapp_downloaded

#
# copy webapp
#
$(STAGEFILES_DIR)/.webif_copied: $(STAGEFILES_DIR)/.webapp_downloaded $$(WEBIF_DEPS)
	-$(RM) -rf $(WEBIF_BUILD_DIR)
	$(MKDIR) -p $(WEBIF_BUILD_DIR)
	$(CP) -RPp $(WEBIF_DIR)/* $(WEBIF_BUILD_DIR)
	$(SED) -i -e 's/^[[:space:]]\+//g' $(WEBIF_BUILD_DIR)/webapp/etc/kloned.conf
ifeq ($(CONFIG_YUI_COMPRESSOR),y)
	$(call css_shrink_dir, $(WEBIF_BUILD_DIR)/webapp/www/css)
	$(call js_shrink_dir, $(WEBIF_BUILD_DIR)/webapp/www/js)
endif
ifeq ($(or $(CONFIG_PNGOUT),$(CONFIG_ZOPFLIPNG),$(CONFIG_PNGWOLF-ZOPFLI),$(CONFIG_ECT),$(CONFIG_PINGO)),y)
	$(call png_shrink_dir, $(WEBIF_BUILD_DIR)/webapp/www/css/images)
endif
	(if [ -f $(DOWNLOAD_DIR)/klapp-$(WEBIF_KLAPP_VERSION).mk ] ; then \
		$(CP) -f $(DOWNLOAD_DIR)/klapp-$(WEBIF_KLAPP_VERSION).mk $(WEBIF_BUILD_DIR)/klapp.mk ; \
	fi ); \
	(if [ -f $(DOWNLOAD_DIR)/klone-$(WEBIF_KLONE_VERSION).tar.gz ] ; then \
		$(CP) -f $(DOWNLOAD_DIR)/klone-$(WEBIF_KLONE_VERSION).tar.gz $(WEBIF_BUILD_DIR)/klone-$(WEBIF_KLONE_VERSION).tar.gz ; \
	fi );
	$(TOUCH) $(STAGEFILES_DIR)/.webif_copied

#
# compile webif
#
$(STAGEFILES_DIR)/.webif_compiled: $(STAGEFILES_DIR)/.webif_copied
	$(ECHO) \# gcc is in $(PREFIX_BIN) > $(WEBIF_TC_FILE)
	$(ECHO) CC = $(UCLIBC_CC) $(UCLIBC_CFLAGS_SIZE) -DIGNORE_TABS -flto=auto -fdevirtualize-at-ltrans -ffunction-sections -fdata-sections -std=gnu90 >> $(WEBIF_TC_FILE)
	$(ECHO) CXX = $(UCLIBC_CXX) $(UCLIBC_CXXFLAGS_SIZE) -DIGNORE_TABS -flto=auto -fdevirtualize-at-ltrans -ffunction-sections -fdata-sections >> $(WEBIF_TC_FILE)
	$(ECHO) AR = $(UCLIBC_AR) >> $(WEBIF_TC_FILE)
	$(ECHO) RANLIB = $(UCLIBC_RANLIB) >> $(WEBIF_TC_FILE)
	$(ECHO) LD = $(UCLIBC_LD) >> $(WEBIF_TC_FILE)
	$(ECHO) NM = $(UCLIBC_NM) >> $(WEBIF_TC_FILE)
	$(ECHO) STRIP = $(UCLIBC_STRIP) >> $(WEBIF_TC_FILE)
	$(ECHO) MAKE = $(MAKE) >> $(WEBIF_TC_FILE)
	$(CD) $(WEBIF_BUILD_DIR) && \
		env PATH="$(PREFIX_BIN):$(PATH)" $(MAKE) clean && \
		env PATH="$(PREFIX_BIN):$(PATH)" KLONE_CUSTOM_TC=$(WEBIF_TC_FILE) \
		WEBAPP_LDFLAGS="$(UCLIBC_LDFLAGS_SIZE) -flto=auto -fdevirtualize-at-ltrans -fuse-linker-plugin -Wl,--gc-sections" $(MAKE)
	$(TOUCH) $(STAGEFILES_DIR)/.webif_compiled

#
# install webif
#
$(STAGEFILES_DIR)/.webif_installed: $(STAGEFILES_DIR)/.webif_compiled
	$(CP) $(WEBIF_BUILD_DIR)/kloned $(TARGET_ROOT)/usr/sbin/webifd
ifeq ($(or $(CONFIG_PNGOUT),$(CONFIG_ZOPFLIPNG),$(CONFIG_PNGWOLF-ZOPFLI),$(CONFIG_ECT),$(CONFIG_PINGO)),y)
	$(call png_shrink_dir, $(WEBIF_CONF_DIR)/etc/webif/www/images)
endif
	(if [ ! -f $(DOWNLOAD_DIR)/klapp-$(WEBIF_KLAPP_VERSION).mk ] ; then \
		$(CP) $(WEBIF_BUILD_DIR)/klapp.mk $(DOWNLOAD_DIR)/klapp-$(WEBIF_KLAPP_VERSION).mk ; \
	fi );
	(if [ ! -f $(DOWNLOAD_DIR)/klone-$(WEBIF_KLONE_VERSION).tar.gz ] ; then \
		$(CP) $(WEBIF_BUILD_DIR)/klone-$(WEBIF_KLONE_VERSION).tar.gz $(DOWNLOAD_DIR)/klone-$(WEBIF_KLONE_VERSION).tar.gz ; \
	fi );
	$(TOUCH) $(STAGEFILES_DIR)/.webif_installed

$(FILELIST_DIR)/webif.lst: $(STAGEFILES_DIR)/.webif_installed
	$(TOUCH) $(FILELIST_DIR)/webif.lst

$(eval $(call gen_copy_file_lst,$(WEBIF_CONF_DIR),,0,0,webif-configs.lst,check))

.PHONY: clean-webif distclean-webif

clean-webif:
	(if [ -f $(WEBIF_BUILD_DIR)/src ] ; then \
		$(CD) $(WEBIF_BUILD_DIR) && env PATH="$(PREFIX_BIN):$(PATH)" $(MAKE) clean ; \
	fi );
	-$(RM) -f $(STAGEFILES_DIR)/.webif_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.webif_installed
	-$(RM) -f $(FILELIST_DIR)/webif-configs.lst

distclean-webif:
	-$(RM) -f $(STAGEFILES_DIR)/.webapp_downloaded
	-$(RM) -f $(STAGEFILES_DIR)/.webif_copied
	-$(RM) -f $(STAGEFILES_DIR)/.webif_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.webif_installed
	-$(RM) -f $(FILELIST_DIR)/webif-configs.lst
	-$(RM) -rf $(WEBIF_BUILD_DIR)
