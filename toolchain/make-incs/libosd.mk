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

LIBOSD_DEPS = $(BASE_BUILD_STAGEFILE)

LIBOSD_DIR := $(BUILDIN_DIR)/libosd
LIBOSD_BUILDDIR := $(BUILD_DIR)/libosd
LIBOSD_INSTALLED = $(STAGEFILES_DIR)/.libosd_installed

PACKS_RULES_$(CONFIG_LIBOSD) += $(LIBOSD_INSTALLED)
FILE_LISTS_$(CONFIG_LIBOSD) += libosd.lst

CLEAN_RULES += clean-libosd
DISTCLEAN_RULES += distclean-libosd


$(STAGEFILES_DIR)/.libosd_copied: $(wildcard $(LIBOSD_DIR)/*) \
   $$(LIBOSD_DEPS)
	-$(RM) -rf $(LIBOSD_BUILDDIR)
	$(CP) -RPp $(LIBOSD_DIR) $(LIBOSD_BUILDDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.libosd_copied

#
# compile libosd
#

$(STAGEFILES_DIR)/.libosd_compiled: $(STAGEFILES_DIR)/.libosd_copied
	$(UCLIBC_ENV_LTO_GC) $(MAKE) -C $(LIBOSD_BUILDDIR) $(UCLIBC_ENV_LTO_GC) \
		PREFIX=$(TARGET_ROOT)/usr all
	$(TOUCH) $(STAGEFILES_DIR)/.libosd_compiled

#
# install libosd
#

$(STAGEFILES_DIR)/.libosd_installed: $(STAGEFILES_DIR)/.libosd_compiled
	$(UCLIBC_ENV_LTO_GC) $(MAKE) -C $(LIBOSD_BUILDDIR) $(UCLIBC_ENV_LTO_GC) \
		PREFIX=$(TARGET_ROOT)/usr install
	$(TOUCH) $(STAGEFILES_DIR)/.libosd_installed

$(FILELIST_DIR)/libosd.lst: $(STAGEFILES_DIR)/.libosd_installed
	$(TOUCH) $(FILELIST_DIR)/libosd.lst

.PHONY: clean-libosd distclean-libosd

clean-libosd:
	-$(RM) -rf $(LIBOSD_BUILDDIR)

distclean-libosd:
	-$(RM) -f $(STAGEFILES_DIR)/.libosd_copied
	-$(RM) -f $(STAGEFILES_DIR)/.libosd_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.libosd_installed
