# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007 Andreas Koch - the open7x0.org group
# Copyright (C) 2007-2021 VDR-NG-EM Project
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

GEN_KERNEL_HEADER_BIN := $(HOSTUTILS_PREFIX_BIN)/gen_kernel_header
GEN_KERNEL_IMG_BIN := $(HOSTUTILS_PREFIX_BIN)/gen_kernel_img
GEN_WSW_BIN := $(HOSTUTILS_PREFIX_BIN)/gen_wsw
GEN_OFI_BIN := $(HOSTUTILS_PREFIX_BIN)/gen_ofi
COPY_LISTS_BIN := $(HOSTUTILS_PREFIX_BIN)/copy_lists

FW-UTILS_HOSTPATCHES_DIR := $(PATCHES_DIR)/fw-utils
FW-UTILS_HOSTDIR := $(BUILDIN_DIR)/fw-utils
FW-UTILS_HOSTBUILDDIR := $(HOSTUTILS_BUILD_DIR)/fw-utils

CLEAN_RULES += clean-fw-utils_host
DISTCLEAN_RULES += distclean-fw-utils_host

$(STAGEFILES_DIR)/.fw-utils_host_copied: $(wildcard $(FW-UTILS_HOSTDIR)/*) \
                                           $(wildcard $(FW-UTILS_HOSTPATCHES_DIR)/host/$(HOST_BS)/*.patch) \
                                           $(TC_INIT_RULE)
	-$(RM) -rf $(FW-UTILS_HOSTBUILDDIR)
	$(MKDIR) -p $(FW-UTILS_HOSTBUILDDIR)
	$(CP) -RPp $(FW-UTILS_HOSTDIR)/* $(FW-UTILS_HOSTBUILDDIR)
	$(TOUCH) $(STAGEFILES_DIR)/.fw-utils_host_copied

#
# patch fw-utils
#

$(STAGEFILES_DIR)/.fw-utils_host_patched: $(STAGEFILES_DIR)/.fw-utils_host_copied
	$(call patch_package, $(FW-UTILS_HOSTBUILDDIR), $(FW-UTILS_HOSTPATCHES_DIR)/host/$(HOST_BS))
	$(TOUCH) $(STAGEFILES_DIR)/.fw-utils_host_patched

#
# compile fw-utils
#

$(STAGEFILES_DIR)/.fw-utils_host_compiled: $(STAGEFILES_DIR)/.fw-utils_host_patched
	$(MAKE) -C $(FW-UTILS_HOSTBUILDDIR) distclean
	$(MAKE) -C $(FW-UTILS_HOSTBUILDDIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.fw-utils_host_compiled

#
# install fw-utils
#

$(STAGEFILES_DIR)/.fw-utils_host_installed: $(STAGEFILES_DIR)/.fw-utils_host_compiled
	$(MAKE) -C $(FW-UTILS_HOSTBUILDDIR) SBINDIR=$(HOSTUTILS_PREFIX_BIN) install
	$(TOUCH) $(STAGEFILES_DIR)/.fw-utils_host_installed

$(GEN_KERNEL_HEADER_BIN): $(STAGEFILES_DIR)/.fw-utils_host_installed
	$(TOUCH) $(GEN_KERNEL_HEADER_BIN)

$(GEN_KERNEL_IMG_BIN): $(STAGEFILES_DIR)/.fw-utils_host_installed
	$(TOUCH) $(GEN_KERNEL_IMG_BIN)

$(GEN_WSW_BIN): $(STAGEFILES_DIR)/.fw-utils_host_installed
	$(TOUCH) $(GEN_WSW_BIN)

$(GEN_OFI_BIN): $(STAGEFILES_DIR)/.fw-utils_host_installed
	$(TOUCH) $(GEN_OFI_BIN)

$(COPY_LISTS_BIN): $(STAGEFILES_DIR)/.fw-utils_host_installed
	$(TOUCH) $(COPY_LISTS_BIN)

.PHONY: clean-fw-utils_host distclean-fw-utils_host

clean-fw-utils-host:
	-$(RM) -rf $(FW-UTILS_HOSTBUILDDIR)

distclean-fw-utils-host:
	-$(RM) -f $(STAGEFILES_DIR)/.fw-utils_host_copied
	-$(RM) -f $(STAGEFILES_DIR)/.fw-utils_host_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.fw-utils_host_installed
