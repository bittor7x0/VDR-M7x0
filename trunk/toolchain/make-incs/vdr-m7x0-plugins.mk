# Copyright (C) 2007 Andreas Koch - the open7x0.org group
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
# The author can be reached at andreas@open7x0.org
#
# The project's page is at http://www.open7x0.org
#

VDR-PLUGINS_DEPS = $(BASE_BUILD_STAGEFILE) $(VDR_INSTALLED) $(TOP_DIR)/.config

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
	fi);
	$(TOUCH) $(STAGEFILES_DIR)/.vdr-plugins_configured

#
# compile vdr-plugins
#

$(STAGEFILES_DIR)/.vdr-plugins_compiled: $(STAGEFILES_DIR)/.vdr-plugins_configured
	$(UCLIBC_ENV) $(MAKE) -C $(VDR_DIR) clean-plugins
	$(UCLIBC_ENV) $(MAKE) -C $(VDR_DIR) plugins
	$(TOUCH) $(STAGEFILES_DIR)/.vdr-plugins_compiled

#
# install vdr-plugins
#

$(STAGEFILES_DIR)/.vdr-plugins_installed: $(STAGEFILES_DIR)/.vdr-plugins_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(VDR_DIR) \
		PLUGINLIBDIR=$(TARGET_ROOT)/usr/lib/vdr install-plugins
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