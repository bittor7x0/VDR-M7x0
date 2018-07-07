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

O7O_FLASHER_DEPS = $(BASE_BUILD_STAGEFILE) $(LIBOSD_INSTALLED)

ifeq ($(CONFIG_O7O_FLASHER),y)
ifneq ($(CONFIG_LIBOSD),y)
   $(error dependency error: o7o_flasher needs libosd enabled)
endif
endif

O7O_FLASHER_DIR := $(BUILDIN_DIR)/o7o_flasher
O7O_FLASHER_BUILDDIR := $(BUILD_DIR)/o7o_flasher
O7O_FLASHER_INSTALLED = $(STAGEFILES_DIR)/.o7o_flasher_installed
O7O_FLASHER_BIN = $(TARGET_ROOT)/usr/sbin/o7o_flasher

PACKS_RULES_$(CONFIG_O7O_FLASHER) += $(O7O_FLASHER_INSTALLED)

CLEAN_RULES += clean-o7o_flasher
DISTCLEAN_RULES += distclean-o7o_flasher


$(STAGEFILES_DIR)/.o7o_flasher_copied: $(wildcard $(O7O_FLASHER_DIR)/*) \
   $$(O7O_FLASHER_DEPS)
	-$(RM) -rf $(O7O_FLASHER_BUILDDIR)
	$(CP) -RPp $(O7O_FLASHER_DIR) $(O7O_FLASHER_BUILDDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.o7o_flasher_copied

#
# compile o7o_flasher
#

$(STAGEFILES_DIR)/.o7o_flasher_compiled: $(STAGEFILES_DIR)/.o7o_flasher_copied
	$(UCLIBC_ENV_LTO_GC) $(MAKE) -C $(O7O_FLASHER_BUILDDIR) $(UCLIBC_ENV_LTO_GC) \
		PREFIX=$(TARGET_ROOT)/usr all
	$(TOUCH) $(STAGEFILES_DIR)/.o7o_flasher_compiled

#
# install o7o_flasher
#

$(STAGEFILES_DIR)/.o7o_flasher_installed: $(STAGEFILES_DIR)/.o7o_flasher_compiled
	$(UCLIBC_ENV_LTO_GC) $(MAKE) -C $(O7O_FLASHER_BUILDDIR) $(UCLIBC_ENV_LTO_GC) \
		PREFIX=$(TARGET_ROOT)/usr install
	$(TOUCH) $(STAGEFILES_DIR)/.o7o_flasher_installed


.PHONY: clean-o7o_flasher distclean-o7o_flasher

clean-o7o_flasher:
	-$(RM) -rf $(O7O_FLASHER_BUILDDIR)

distclean-o7o_flasher:
	-$(RM) -f $(STAGEFILES_DIR)/.o7o_flasher_copied
	-$(RM) -f $(STAGEFILES_DIR)/.o7o_flasher_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.o7o_flasher_installed
