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

USBAUTOMOUNTER_DEPS = $(BASE_BUILD_STAGEFILE) $(LIBOSD_INSTALLED)

ifeq ($(CONFIG_USBAUTOMOUNTER),y)
ifneq ($(CONFIG_LIBOSD),y)
   $(error dependency error: usbautomounter needs libosd enabled)
endif
endif


USBAUTOMOUNTER_DIR := $(BUILDIN_DIR)/usbautomounter
USBAUTOMOUNTER_BUILDDIR := $(BUILD_DIR)/usbautomounter
USBAUTOMOUNTER_INSTALLED = $(STAGEFILES_DIR)/.usbautomounter_installed

PACKS_RULES_$(CONFIG_USBAUTOMOUNTER) += $(USBAUTOMOUNTER_INSTALLED)
FILE_LISTS_$(CONFIG_USBAUTOMOUNTER) += usbautomounter.lst

CLEAN_RULES += clean-usbautomounter
DISTCLEAN_RULES += distclean-usbautomounter


$(STAGEFILES_DIR)/.usbautomounter_copied: $(wildcard $(USBAUTOMOUNTER_DIR)/*) \
   $$(USBAUTOMOUNTER_DEPS)
	-$(RM) -rf $(USBAUTOMOUNTER_BUILDDIR)
	$(CP) -RPp $(USBAUTOMOUNTER_DIR) $(USBAUTOMOUNTER_BUILDDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.usbautomounter_copied

#
# compile usbautomounter
#

$(STAGEFILES_DIR)/.usbautomounter_compiled: $(STAGEFILES_DIR)/.usbautomounter_copied
	$(UCLIBC_ENV) $(MAKE) -C $(USBAUTOMOUNTER_BUILDDIR) $(UCLIBC_ENV) \
		PREFIX=$(TARGET_ROOT) all
	$(TOUCH) $(STAGEFILES_DIR)/.usbautomounter_compiled

#
# install usbautomounter
#

$(STAGEFILES_DIR)/.usbautomounter_installed: $(STAGEFILES_DIR)/.usbautomounter_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(USBAUTOMOUNTER_BUILDDIR) $(UCLIBC_ENV) \
		PREFIX=$(TARGET_ROOT) install
	$(TOUCH) $(STAGEFILES_DIR)/.usbautomounter_installed


$(FILELIST_DIR)/usbautomounter.lst: $(STAGEFILES_DIR)/.usbautomounter_installed
	$(TOUCH) $(FILELIST_DIR)/usbautomounter.lst

.PHONY: clean-usbautomounter distclean-usbautomounter

clean-usbautomounter:
	-$(RM) -rf $(USBAUTOMOUNTER_BUILDDIR)

distclean-usbautomounter:
	-$(RM) -f $(STAGEFILES_DIR)/.usbautomounter_copied
	-$(RM) -f $(STAGEFILES_DIR)/.usbautomounter_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.usbautomounter_installed
