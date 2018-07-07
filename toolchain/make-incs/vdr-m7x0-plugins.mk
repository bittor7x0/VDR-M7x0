# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2018 VDR-NG-EM Project
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

VDR-PLUGINS_DEPS = $(BASE_BUILD_STAGEFILE) $(VDR_INSTALLED) $(TOP_DIR)/.config

ifeq ($(or $(filter y,$(CONFIG_ZLIB)),$(filter y,$(CONFIG_ZLIB_STATIC))),y)
	VDR-PLUGINS_DEPS +=  $(ZLIB_INSTALLED)
endif
ifeq ($(or $(filter y,$(CONFIG_PCRE)),$(filter y,$(CONFIG_PCRE_CPP))),y)
	VDR-PLUGINS_DEPS +=  $(PCRE_INSTALLED)
endif
ifeq ($(CONFIG_GRAPHICSMAGICK),y)
	VDR-PLUGINS_DEPS +=  $(GRAPHICSMAGICK_INSTALLED)
endif
ifeq ($(CONFIG_FREETYPE),y)
	VDR-PLUGINS_DEPS +=  $(FREETYPE_INSTALLED)
endif
ifeq ($(CONFIG_SQLITE),y)
	VDR-PLUGINS_DEPS +=  $(SQLITE_INSTALLED)
endif
ifeq ($(CONFIG_CURL),y)
	VDR-PLUGINS_DEPS +=  $(CURL_INSTALLED)
endif
ifeq ($(CONFIG_LIBXML2),y)
	VDR-PLUGINS_DEPS +=  $(LIBXML2_INSTALLED)
endif
ifeq ($(CONFIG_JANSSON),y)
	VDR-PLUGINS_DEPS +=  $(JANSSON_INSTALLED)
endif

VDR-PLUGINS_SVN_URL := http://svn.assembla.com/svn/VDR-M7x0/trunk/vdr-m7x0-plugins
VDR-PLUGINS_DIR := $(BUILD_DIR)/vdr-m7x0-PLUGINS
VDR-PLUGINS_CONF_COMMON_DIR := $(PRG_CONFIGS_DIR)/vdr-m7x0-plugins/common
VDR-PLUGINS_CONF_SYSTYPE_DIR := $(PRG_CONFIGS_DIR)/vdr-m7x0-plugins/$(CONFIG_M7X0_TYPE)

vdr-plugins_rules = \
   $(foreach plugin,$(CONFIG_VDR-PLUGINS), \
       $(STAGEFILES_DIR)/.vdr-plugins_$(subst /,_,$(plugin))_$(1))

vdr-plugins_touch_stagefiles = \
   $(foreach stagefile, $(call vdr-plugins_rules,$(1)), \
      $(TOUCH) $(stagefile);)


VDR-PLUGINS_INSTALLED = $(call vdr-plugin_rules,installed)

ifneq ($(CONFIG_VDR-PLUGINS),)
ifneq ($(CONFIG_VDR),y)
   $(error dependency error: vdr plugins needs vdr enabled)
endif
ifeq ($(CONFIG_VDR-PLUGINS-LIBS),)
   $(error dependency error: plugins given but no plugins libnames)
endif
PACKS_RULES_y += $(VDR-PLUGINS_INSTALLED)
FILE_LISTS_y += vdr-plugins.lst \
   $(foreach plugin,$(notdir $(CONFIG_VDR-PLUGINS)), \
      vdr-plugin-$(plugin)-configs.lst) \
   $(foreach plugin,$(notdir $(CONFIG_VDR-PLUGINS)), \
      vdr-plugin-$(plugin)-configs-$(CONFIG_M7X0_TYPE).lst)
endif



CLEAN_RULES += clean-vdr-plugins
DISTCLEAN_RULES += distclean-vdr-plugins

VDR-PLUGINS_APIVERSION = \
   $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' $(VDR_DIR)/config.h)

VDR-PLUGINS_LIBNAMES = $(foreach plug,$(CONFIG_VDR-PLUGINS-LIBS), \
   libvdr-$(plug).so.$(VDR-PLUGINS_APIVERSION))

ifneq ($(and $(CONFIG_VDR-PLUGINS), $(wildcard $(VDR-PLUGINS_DIR))),)
$(info Updating vdr plugins download ...)
vdr-plugins_svn_changed := $(call svn_changed,$(VDR-PLUGINS_DIR))

ifeq ($(vdr-plugins_svn_changed),0)
$(info vdr plugins are up to date)
else
$(info $(vdr-plugins_svn_changed) file(s) updated)
vdr-plugins_tmp := $(shell $(TOUCH) $(STAGEFILES_DIR)/.vdr-plugins_downloaded)
endif
endif

#
# download vdr-plugins
#
$(STAGEFILES_DIR)/.vdr-plugins_downloaded: $(TC_INIT_RULE)
	$(SVN) co $(VDR-PLUGINS_SVN_URL) $(VDR-PLUGINS_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.vdr-plugins_downloaded

#
# configure vdr-plugins
#

vdr_plugins_pathes = $(filter trunk/%, $(CONFIG_VDR-PLUGINS)) \
   $(foreach p2, $(filter-out trunk/%, $(CONFIG_VDR-PLUGINS)), $(p2))

$(STAGEFILES_DIR)/.vdr-plugins_configured: $$(VDR-PLUGINS_DEPS) \
                                      $(STAGEFILES_DIR)/.vdr-plugins_downloaded
	-$(FIND) $(VDR_DIR)/PLUGINS/src -type l -exec $(RM) -f {} \;
	$(foreach plugin,$(vdr_plugins_pathes), \
		$(LN) -sf $(VDR-PLUGINS_DIR)/$(plugin) \
		$(VDR_DIR)/PLUGINS/src/$(lastword $(subst /, ,$(plugin)));) \
	(if [ -f $(VDR_DIR)/PLUGINS/src/pin/Makefile ]; then \
		$(SED) -i -e 's,^FSKCHKDIR = .*,FSKCHKDIR = \"$(TOP_DIR)\/prg-fw-configs\/vdr-m7x0-plugins\/common\/pin\/usr\/bin\",g' \
			$(VDR_DIR)/PLUGINS/src/pin/Makefile; \
	fi; \
	if [ -f $(VDR_DIR)/PLUGINS/src/vdrmanager/Makefile ]; then \
		if [ X"$(CONFIG_ZLIB)" != X"y" -a X"$(CONFIG_ZLIB_STATIC)" != X"y" ]; then \
			$(ECHO) dependency error: vdrmanager plugin needs zlib or zlib-static enabled; \
			exit 1; \
		fi; \
	fi; \
	if [ -f $(VDR_DIR)/PLUGINS/src/markad/Makefile ]; then \
		if [ X"$(CONFIG_MARKAD)" != X"y" ]; then \
			$(ECHO) dependency error: markad plugin needs markad program enabled; \
			exit 1; \
		fi; \
	fi; \
	if [ -f $(VDR_DIR)/PLUGINS/src/skinenigmang/Makefile ]; then \
		if [ X"$(CONFIG_GRAPHICSMAGICK)" = X"y" ]; then \
			$(SED) -i -e 's,^#HAVE_IMAGEMAGICK = 1,HAVE_IMAGEMAGICK = GRAPHICS,g' \
				$(VDR_DIR)/PLUGINS/src/skinenigmang/Makefile; \
		else \
			$(SED) -i -e 's,^HAVE_IMAGEMAGICK = GRAPHICS,#HAVE_IMAGEMAGICK = 1,g' \
				$(VDR_DIR)/PLUGINS/src/skinenigmang/Makefile; \
		fi; \
		if [ X"$(CONFIG_FREETYPE)" = X"y" ]; then \
			$(SED) -i -e 's,^#HAVE_FREETYPE = 1,HAVE_FREETYPE = 1,g' \
				$(VDR_DIR)/PLUGINS/src/skinenigmang/Makefile; \
			$(SED) -i -e 's,shell.*freetype-config"\?,shell \"$(TARGET_ROOT)/usr/bin/freetype-config\",g' \
				$(VDR_DIR)/PLUGINS/src/skinenigmang/Makefile; \
		else \
			$(SED) -i -e 's,^HAVE_FREETYPE = 1,#HAVE_FREETYPE = 1,g' \
				$(VDR_DIR)/PLUGINS/src/skinenigmang/Makefile; \
		fi; \
	fi; \
	if [ -f $(VDR_DIR)/PLUGINS/src/tvscraper/Makefile ]; then \
		if [ X"$(CONFIG_SQLITE)" != X"y" -o X"$(CONFIG_CURL)" != X"y" -o X"$(CONFIG_LIBXML2)" != X"y" -o X"$(CONFIG_JANSSON)" != X"y" ]; then \
			$(ECHO) dependency error: tvscraper plugin needs libsqlite3, libcurl, libXML2 and libjansson enabled; \
			exit 1; \
		fi; \
		$(SED) -i -e 's,shell.*xml2-config"\?,shell \"$(TARGET_ROOT)/usr/bin/xml2-config\",g' \
			$(VDR_DIR)/PLUGINS/src/tvscraper/Makefile; \
	fi; \
	if [ -f $(VDR_DIR)/PLUGINS/src/xmltv2vdr/Makefile ]; then \
		if [ X"$(CONFIG_SQLITE)" != X"y" -o X"$(CONFIG_LIBXML2)" != X"y" -o X"$(CONFIG_PCRE)" != X"y" -o X"$(CONFIG_PCRE_CPP)" != X"y" ]; then \
			$(ECHO) dependency error: xmltv2vdr plugin needs libsqlite3, libXML2 and pcre library with C++ support enabled; \
			exit 1; \
		fi; \
	fi; \
	if [ -f $(VDR_DIR)/PLUGINS/src/epgfixer/Makefile ]; then \
		if [ X"$(CONFIG_PCRE)" != X"y" ]; then \
			$(ECHO) dependency error: epgfixer plugin needs pcre library enabled; \
			exit 1; \
		else \
			$(SED) -i -e 's,shell.*pcre-config"\?,shell \"$(TARGET_ROOT)/usr/bin/pcre-config\",g' \
				$(VDR_DIR)/PLUGINS/src/epgfixer/Makefile; \
		fi; \
	fi; \
	if [ -f $(VDR_DIR)/PLUGINS/src/epgsearch/Makefile ]; then \
		if [ -f $(VDR_DIR)/PLUGINS/src/pin/Makefile ]; then \
			$(SED) -i -e 's,^#USE_PINPLUGIN = 1,USE_PINPLUGIN = 1,g' \
				$(VDR_DIR)/PLUGINS/src/epgsearch/Makefile; \
		else \
			$(SED) -i -e 's,^USE_PINPLUGIN = 1,#USE_PINPLUGIN = 1,g' \
				$(VDR_DIR)/PLUGINS/src/epgsearch/Makefile; \
		fi; \
		if [ X"$(CONFIG_PCRE)" = X"y" ]; then \
			$(SED) -i -e 's,^#REGEXLIB = pcre,REGEXLIB = pcre,g' \
				$(VDR_DIR)/PLUGINS/src/epgsearch/Makefile; \
			$(SED) -i -e 's,shell.*pcre-config"\?,shell \"$(TARGET_ROOT)/usr/bin/pcre-config\",g' \
				$(VDR_DIR)/PLUGINS/src/epgsearch/Makefile; \
		else \
			$(SED) -i -e 's,^REGEXLIB = pcre,#REGEXLIB = pcre,g' \
				$(VDR_DIR)/PLUGINS/src/epgsearch/Makefile; \
		fi; \
	fi;)
	$(TOUCH) $(STAGEFILES_DIR)/.vdr-plugins_configured

#
# compile vdr-plugins
#

$(STAGEFILES_DIR)/.vdr-plugins_compiled: $(STAGEFILES_DIR)/.vdr-plugins_configured
	$(UCLIBC_ENV_LTO) LDFLAGS="-Wl,-O1 -Wl,--gc-sections -flto" \
		$(if $(CONFIG_UCLIBC++), CXX="$(UCLIBC++_CXX)") \
		$(if $(CONFIG_UCLIBC_WITH_BACKTRACE), CRASHLOG=1) \
		$(if $(filter m750s,$(CONFIG_M7X0_TYPE)),M750S=1) \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		$(MAKE) -C $(VDR_DIR) clean-plugins
	$(UCLIBC_ENV_LTO) LDFLAGS="-Wl,-O1 -Wl,--gc-sections -flto" \
		$(if $(CONFIG_UCLIBC++), CXX="$(UCLIBC++_CXX)") \
		$(if $(CONFIG_UCLIBC_WITH_BACKTRACE), CRASHLOG=1) \
		$(if $(filter m750s,$(CONFIG_M7X0_TYPE)),M750S=1) \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		$(MAKE) -C $(VDR_DIR) plugins
	$(TOUCH) $(STAGEFILES_DIR)/.vdr-plugins_compiled

#
# install vdr-plugins
#

$(STAGEFILES_DIR)/.vdr-plugins_installed: $(STAGEFILES_DIR)/.vdr-plugins_compiled
	$(UCLIBC_ENV_LTO) LDFLAGS="-Wl,-O1 -Wl,--gc-sections -flto" \
		$(if $(CONFIG_UCLIBC++), CXX="$(UCLIBC++_CXX)") \
		$(if $(CONFIG_UCLIBC_WITH_BACKTRACE), CRASHLOG=1) \
		$(if $(filter m750s,$(CONFIG_M7X0_TYPE)),M750S=1) \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		$(MAKE) -C $(VDR_DIR) PLUGINLIBDIR=$(TARGET_ROOT)/usr/lib/vdr install-plugins
	$(TOUCH) $(STAGEFILES_DIR)/.vdr-plugins_installed


$(FILELIST_DIR)/vdr-plugins.lst: $(STAGEFILES_DIR)/.vdr-plugins_installed
	($(ECHO)   "usr/lib/vdr       -                 d 755 0 0 - - - - -"; \
	 $(ECHO)   "etc/plugins-lib   -                 d 755 0 0 - - - - -"; \
	for lib in $(VDR-PLUGINS_LIBNAMES) ; do \
		$(ECHO) "usr/lib/vdr/$$lib usr/lib/vdr/$$lib f 755 0 0 - - - - -"; \
		$(ECHO) "usr/lib/vdr/$$lib -                 s   - - - - - - - -"; \
	done) > $(FILELIST_DIR)/vdr-plugins.lst

$(foreach plugin,$(notdir $(CONFIG_VDR-PLUGINS)), \
   $(eval $(call gen_copy_file_lst,$(VDR-PLUGINS_CONF_COMMON_DIR)/$(plugin), \
      ,0,0,vdr-plugin-$(plugin)-configs.lst,nocheck)))

$(foreach plugin,$(notdir $(CONFIG_VDR-PLUGINS)), \
   $(eval $(call gen_copy_file_lst,$(VDR-PLUGINS_CONF_SYSTYPE_DIR)/$(plugin), \
      ,0,0,vdr-plugin-$(plugin)-configs-$(CONFIG_M7X0_TYPE).lst,nocheck)))

.PHONY: clean-vdr-plugins distclean-vdr-plugins

clean-vdr-plugins:
	-$(RM) -rf $(VDR-PLUGINS_DIR)

distclean-vdr-plugins:
	-$(RM) -f $(STAGEFILES_DIR)/.vdr-plugins_*
	-$(RM) -f $(FILELIST_DIR)/vdr-plugins.lst
	-$(RM) -f $(FILELIST_DIR)/vdr-plugin-*.lst

recompile-vdr-plugins:
	-$(RM) -rf $(STAGEFILES_DIR)/.vdr-plugins_configured
	-$(RM) -rf $(STAGEFILES_DIR)/.vdr-plugins_compiled
	-$(RM) -rf $(STAGEFILES_DIR)/.vdr-plugins_installed

cppcheck-vdr-plugins:
	(if [ -d $(VDR_DIR)/PLUGINS/src ] ; then \
	$(FIND) $(VDR_DIR)/PLUGINS/src -type l -exec $(RM) -f {} \; && \
	$(FIND) $(VDR-PLUGINS_DIR) -maxdepth 1 -type l -exec sh -c '$(LN) -sf "$(VDR-PLUGINS_DIR)/$$(basename {})" "$(VDR_DIR)/PLUGINS/src/$$(basename {})"' \; && \
	$(UCLIBC_ENV_LTO) LDFLAGS="-Wl,-O1 -Wl,--gc-sections -flto" \
		$(if $(CONFIG_UCLIBC++), CXX="$(UCLIBC++_CXX)") \
		$(if $(CONFIG_UCLIBC_WITH_BACKTRACE), CRASHLOG=1) \
		$(if $(filter m750s,$(CONFIG_M7X0_TYPE)),M750S=1) \
		PKG_CONFIG_PATH="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(TARGET_ROOT)/usr/lib/pkgconfig" \
		$(MAKE) -C $(VDR_DIR) cppcheck-plugins ; \
	fi );
