# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2022 VDR-NG-EM Project
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

BOOTLOADER_DEPS = $(BASE_BUILD_STAGEFILE) $(TC_INIT_RULE)

BOOTLOADER_DIR := $(BUILDIN_DIR)/bootloader
BOOTLOADER_BUILDDIR := $(BUILD_DIR)/bootloader
BOOTLOADER_INSTALLED = $(STAGEFILES_DIR)/.bootloader_installed
BOOTLOADER_BIN = $(TARGET_ROOT)/boot/bootloader.bin

PACKS_RULES_$(CONFIG_BOOTLOADER) += $(BOOTLOADER_INSTALLED)

CLEAN_RULES += clean-bootloader
DISTCLEAN_RULES += distclean-bootloader


$(STAGEFILES_DIR)/.bootloader_copied: $(wildcard $(BOOTLOADER_DIR)/*) \
   $$(BOOTLOADER_DEPS)
	-$(RM) -rf $(BOOTLOADER_BUILDDIR)
	$(CP) -RPp $(BOOTLOADER_DIR) $(BOOTLOADER_BUILDDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.bootloader_copied

#
# compile bootloader
#

$(STAGEFILES_DIR)/.bootloader_compiled: $(STAGEFILES_DIR)/.bootloader_copied
	$(UCLIBC_ENV) $(MAKE) -C $(BOOTLOADER_BUILDDIR) $(UCLIBC_ENV) \
		PREFIX=$(TARGET_ROOT) all
	$(TOUCH) $(STAGEFILES_DIR)/.bootloader_compiled

#
# install bootloader
#

$(STAGEFILES_DIR)/.bootloader_installed: $(STAGEFILES_DIR)/.bootloader_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(BOOTLOADER_BUILDDIR) $(UCLIBC_ENV) \
		PREFIX=$(TARGET_ROOT) install
	$(TOUCH) $(STAGEFILES_DIR)/.bootloader_installed

$(BOOTLOADER_BIN): $(STAGEFILES_DIR)/.bootloader_installed
	$(TOUCH) $(BOOTLOADER_BIN)

.PHONY: clean-bootloader distclean-bootloader

clean-bootloader:
	-$(RM) -rf $(BOOTLOADER_BUILDDIR)

distclean-bootloader:
	-$(RM) -f $(STAGEFILES_DIR)/.bootloader_copied
	-$(RM) -f $(STAGEFILES_DIR)/.bootloader_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.bootloader_installed
