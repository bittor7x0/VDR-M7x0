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
# The project's page is at http://www.open7x0.org
#

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
WEBIF_DEPS = $(BASE_BUILD_STAGEFILE)

ifeq ($(CONFIG_CSSOPTIMIZER),y)
	WEBIF_DEPS +=  $(CSSOPTIMIZER_BIN)
endif
ifeq ($(CONFIG_CLOSURE_COMPILER),y)
	WEBIF_DEPS +=  $(CLOSURE_COMPILER_JAR)
endif
ifeq ($(CONFIG_PNGOUT),y)
	WEBIF_DEPS +=  $(PNGOUT_BIN)
endif

WEBIF_DIR := $(BUILD_DIR)/webif
WEBIF_SVN := http://svn.assembla.com/svn/VDR-M7x0/trunk/webif
WEBIF_TC_FILE := $(WEBIF_DIR)/linux-mips-uclibc.tc
LOGOS_FILE := logos-webif.tgz
LOGOS_DLFILE := $(DOWNLOAD_DIR)/logos-webif.4.tgz
LOGOS_URL := https://www.assembla.com/spaces/VDR-M7x0/documents/c6zJ780MGr36EzeJe5cbCb/download/$(LOGOS_FILE)

PACKS_RULES_$(CONFIG_WEBIF) += $(STAGEFILES_DIR)/.webif_installed
FILE_LISTS_$(CONFIG_WEBIF) += webif.lst

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
# webapp download
#
$(STAGEFILES_DIR)/.webapp_downloaded: $(TC_INIT_RULE)
	$(SVN) co $(WEBIF_SVN) $(WEBIF_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.webapp_downloaded

#
# compile webif
#
$(STAGEFILES_DIR)/.webif_compiled: $(STAGEFILES_DIR)/.webapp_downloaded $$(WEBIF_DEPS)
ifeq ($(CONFIG_CSSOPTIMIZER),y)
	$(call css_shrink_dir, $(WEBIF_DIR)/webapp/www/css)
endif
ifeq ($(CONFIG_CLOSURE_COMPILER),y)
	$(call js_shrink_dir, $(WEBIF_DIR)/webapp/www/js)
endif
ifeq ($(CONFIG_PNGOUT),y)
	$(call png_shrink_dir, $(WEBIF_DIR)/webapp/www/css/images)
endif
	$(ECHO) \# gcc is in $(PREFIX_BIN) > $(WEBIF_TC_FILE)
	$(ECHO) CC = $(UCLIBC_CC) $(UCLIBC_CFLAGS_SIZE) -flto >> $(WEBIF_TC_FILE)
	$(ECHO) CXX = $(UCLIBC_CXX) $(UCLIBC_CXXFLAGS_SIZE) -flto >> $(WEBIF_TC_FILE)
	$(ECHO) AR = $(UCLIBC_AR) >> $(WEBIF_TC_FILE)
	$(ECHO) RANLIB = $(UCLIBC_RANLIB) >> $(WEBIF_TC_FILE)
	$(ECHO) LD = $(UCLIBC_LD) >> $(WEBIF_TC_FILE)
	$(ECHO) NM = $(UCLIBC_NM) >> $(WEBIF_TC_FILE)
	$(ECHO) STRIP = $(UCLIBC_STRIP) >> $(WEBIF_TC_FILE)
	$(ECHO) MAKE = $(MAKE) >> $(WEBIF_TC_FILE)
	$(CD) $(WEBIF_DIR) && \
		env PATH="$(PREFIX_BIN):$(PATH)" $(MAKE) clean && \
		env PATH="$(PREFIX_BIN):$(PATH)" KLONE_CUSTOM_TC=$(WEBIF_TC_FILE) \
		WEBAPP_LDFLAGS="$(UCLIBC_LDFLAGS_SIZE) -flto -fwhole-program" $(MAKE)
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
ifeq ($(CONFIG_PNGOUT),y)
	$(call png_shrink_dir, $(PRG_CONFIGS_DIR)/vdr-m7x0/common/etc/webif/www/images)
endif
	$(TOUCH) $(STAGEFILES_DIR)/.webif_installed

$(FILELIST_DIR)/webif.lst: $(STAGEFILES_DIR)/.webif_installed
	$(TOUCH) $(FILELIST_DIR)/webif.lst

.PHONY: clean-webif distclean-webif

clean-webif:
	(if [ -f $(WEBIF_DIR)/src ] ; then \
		$(CD) $(WEBIF_DIR) && env PATH="$(PREFIX_BIN):$(PATH)" $(MAKE) clean ; \
	fi );

distclean-webif:
	-$(RM) -f $(STAGEFILES_DIR)/.webif_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.webif_installed
	-$(RM) -f $(STAGEFILES_DIR)/.webapp_downloaded
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -f $(LOGOS_DLFILE)
endif
	-$(RM) -rf $(PRG_CONFIGS_DIR)/vdr-m7x0/common/etc/webif/www/images
