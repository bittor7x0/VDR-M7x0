# Copyright (C) 2006,2007 Andreas Koch - the open7x0.org group
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

VDR_DEPS = $(BASE_BUILD_STAGEFILE)

VDR_SVN_URL := http://svn.assembla.com/svn/VDR-M7x0/trunk/vdr-m7x0
VDR_DIR := $(BUILD_DIR)/vdr-m7x0
VDR_CONF_COMMON_DIR := $(PRG_CONFIGS_DIR)/vdr-m7x0/common
VDR_CONF_SYSTYPE_DIR := $(PRG_CONFIGS_DIR)/vdr-m7x0/$(CONFIG_M7X0_TYPE)
VDR_INSTALLED = $(STAGEFILES_DIR)/.vdr_installed

PACKS_RULES_$(CONFIG_VDR) += $(VDR_INSTALLED)
FILE_LISTS_$(CONFIG_VDR) += vdr.lst vdr-configs.lst vdr-configs-$(CONFIG_M7X0_TYPE).lst

CLEAN_RULES += clean-vdr
DISTCLEAN_RULES += distclean-vdr

ifneq ($(and $(filter y,$(CONFIG_VDR)), $(wildcard $(VDR_DIR))),)
$(info Updating vdr download ...)
vdr_svn_changed := $(call svn_changed, $(VDR_DIR))

ifeq ($(vdr_svn_changed),0)
$(info vdr is up to date)
else
$(info $(vdr_svn_changed) file(s) updated)
vdr_tmp := $(shell $(TOUCH) $(STAGEFILES_DIR)/.vdr_downloaded)
endif
endif

#
# download vdr
#

$(STAGEFILES_DIR)/.vdr_downloaded: $(TC_INIT_RULE)
	$(SVN) co $(VDR_SVN_URL) $(VDR_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.vdr_downloaded

$(STAGEFILES_DIR)/.vdr_local_downloaded: $(CONFIG_VDR_LOCAL_PATH)/*.c \
                                         $(CONFIG_VDR_LOCAL_PATH)/*.h \
                                         $(CONFIG_VDR_LOCAL_PATH)/libsi/*.c \
                                         $(CONFIG_VDR_LOCAL_PATH)/libsi/*.h
	-$(MKDIR) -p $(VDR_DIR)
	$(CP) -RPp $(CONFIG_VDR_LOCAL_PATH)/* $(VDR_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.vdr_local_downloaded

#
# compile vdr
#

$(STAGEFILES_DIR)/.vdr_compiled: $$(VDR_DEPS) \
        $(STAGEFILES_DIR)/.vdr_$(filter local_,_)downloaded
	$(UCLIBC_ENV) $(MAKE) -C $(VDR_DIR) clean
	$(UCLIBC_ENV) $(MAKE) -C $(VDR_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.vdr_compiled

#
# install vdr
#

$(STAGEFILES_DIR)/.vdr_installed: $(STAGEFILES_DIR)/.vdr_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(VDR_DIR) BINDIR=$(TARGET_ROOT)/usr/bin \
		 install-bin
	$(TOUCH) $(STAGEFILES_DIR)/.vdr_installed


$(FILELIST_DIR)/vdr.lst: $(STAGEFILES_DIR)/.vdr_installed
	$(TOUCH) $(FILELIST_DIR)/vdr.lst

$(eval $(call gen_copy_file_lst,$(VDR_CONF_COMMON_DIR),,0,0,vdr-configs.lst,check))

$(eval $(call gen_copy_file_lst,$(VDR_CONF_SYSTYPE_DIR),,0,0,vdr-configs-$(CONFIG_M7X0_TYPE).lst,check))

.PHONY: clean-vdr distclean-vdr

clean-vdr:
	-$(RM) -rf $(VDR_DIR)
	-$(RM) -f $(STAGEFILES_DIR)/.vdr_downloaded

distclean-vdr:
	-$(RM) -f $(STAGEFILES_DIR)/.vdr_*
	-$(RM) -f $(FILELIST_DIR)/vdr-configs*.lst

recompile-vdr:
	-$(RM) -rf $(STAGEFILES_DIR)/.vdr_compiled
	-$(RM) -rf $(STAGEFILES_DIR)/.vdr_installed
