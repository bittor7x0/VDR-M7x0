#
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
# $Id: tool-box.mk 321 2008-02-22 17:21:34Z andreas $
#
#

TOOL-BOX_DEPS = $(BASE_BUILD_STAGEFILE) $(LIBOSD_INSTALLED)

ifeq ($(CONFIG_TOOL-BOX),y)
ifneq ($(CONFIG_LIBOSD),y)
   $(error dependency error: tool-box needs libosd enabled)
endif
endif

TOOL-BOX_DIR := $(BUILDIN_DIR)/tool-box
TOOL-BOX_BUILDDIR := $(BUILD_DIR)/tool-box
TOOL-BOX_INSTALLED = $(STAGEFILES_DIR)/.tool-box_installed

PACKS_RULES_$(CONFIG_TOOL-BOX) += $(TOOL-BOX_INSTALLED)
FILE_LISTS_$(CONFIG_TOOL-BOX) += tool-box.lst

CLEAN_RULES += clean-tool-box
DISTCLEAN_RULES += distclean-tool-box


$(STAGEFILES_DIR)/.tool-box_copied: $(wildcard $(TOOL-BOX_DIR)/*) \
   $$(TOOL-BOX_DEPS)
	-$(RM) -rf $(TOOL-BOX_BUILDDIR)
	$(CP) -RPp $(TOOL-BOX_DIR) $(TOOL-BOX_BUILDDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.tool-box_copied

#
# compile tool-box
#

$(STAGEFILES_DIR)/.tool-box_compiled: $(STAGEFILES_DIR)/.tool-box_copied
	$(UCLIBC_ENV) $(MAKE) -C $(TOOL-BOX_BUILDDIR) $(UCLIBC_ENV) \
		PREFIX=$(TARGET_ROOT)/usr all
	$(TOUCH) $(STAGEFILES_DIR)/.tool-box_compiled

#
# install tool-box
#

$(STAGEFILES_DIR)/.tool-box_installed: $(STAGEFILES_DIR)/.tool-box_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(TOOL-BOX_BUILDDIR) $(UCLIBC_ENV) \
		PREFIX=$(TARGET_ROOT)/usr install
	$(TOUCH) $(STAGEFILES_DIR)/.tool-box_installed


$(FILELIST_DIR)/tool-box.lst: $(STAGEFILES_DIR)/.tool-box_installed
	$(TOUCH) $(FILELIST_DIR)/tool-box.lst

.PHONY: clean-tool-box distclean-tool-box

clean-tool-box:
	-$(RM) -rf $(TOOL-BOX_BUILDDIR)

distclean-tool-box:
	-$(RM) -f $(STAGEFILES_DIR)/.tool-box_copied
	-$(RM) -f $(STAGEFILES_DIR)/.tool-box_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.tool-box_installed

