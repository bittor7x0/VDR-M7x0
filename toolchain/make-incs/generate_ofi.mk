# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006-2007 Andreas Koch - the open7x0.org group
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

OFI-IMG_DEPS = $(GEN_OFI_BIN) $(O7O_FLASHER_INSTALLED)

ifeq ($(CONFIG_GENERATE_OFI-IMG),y)
  ifneq ($(CONFIG_O7O_FLASHER),y)
     $(error dependency error: ofi needs o7o_flasher enabled)
  endif
  ifneq ($(or $(CONFIG_GENERATE_CRAM_ROOTFS_IMAGE),\
     $(CONFIG_GENERATE_SQUASH_ROOTFS_IMAGE),\
     $(CONFIG_GENERATE_SIEMENS-LINUX-KERNEL-IMG)),y)
     $(error dependency error: ofi needs cramfs, squashfs and/or kernel image enabled)
  endif
endif

GEN_OFI_CMD_LINE = -s $(SHA1) --$(CONFIG_M7X0_TYPE) -f $(O7O_FLASHER_BIN)

OFI-OFI_TIME_REF_all =
ifeq ($(CONFIG_GENERATE_SQUASH_ROOTFS_IMAGE),y)
   GEN_OFI_CMD_LINE += -r $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)
   OFI-IMG_DEPS += $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)
   OFI-IMG_TIME_REF_all += $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)
else
ifeq ($(CONFIG_GENERATE_CRAM_ROOTFS_IMAGE),y)
   GEN_OFI_CMD_LINE += -r $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
   OFI-IMG_DEPS += $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
   OFI-IMG_TIME_REF_all += $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
endif
endif
ifeq ($(CONFIG_GENERATE_SIEMENS-LINUX-KERNEL-IMG),y)
   GEN_OFI_CMD_LINE += -k $(SIEMENS-LINUX-KERNEL-IMG)
   OFI-IMG_DEPS += $(SIEMENS-LINUX-KERNEL-IMG)
   OFI-IMG_TIME_REF_all += $(SIEMENS-LINUX-KERNEL-IMG)
endif
ifeq ($(CONFIG_GENERATE_JFFS2_IMAGE),y)
   GEN_OFI_CMD_LINE += -j $(TOP_DIR)/$(JFFS2_IMG)
   OFI-IMG_DEPS += $(TOP_DIR)/$(JFFS2_IMG)
   OFI-IMG_TIME_REF_all += $(TOP_DIR)/$(JFFS2_IMG)
endif
OFI-IMG := $(TOP_DIR)/$(or $(notdir $(CONFIG_OFI-IMG)),vdr-ng-em-$(CONFIG_M7X0_TYPE)-$(CONFIG_FW_VERSION).ofi)
OFI-IMG_TIME_REF = $(firstword $(OFI-IMG_TIME_REF_all))
GEN_OFI_CMD_LINE += -o $(OFI-IMG)

POST_RULES_$(CONFIG_GENERATE_OFI-IMG) += $(OFI-IMG)
DISTCLEAN_RULES += distclean-generate-ofi

$(OFI-IMG): $$(OFI-IMG_DEPS)
	$(GEN_OFI_BIN) $(GEN_OFI_CMD_LINE) \
		-d `$(DATE) -r $(OFI-IMG_TIME_REF) +"%s"`

.PHONY: distclean-generate-ofi

distclean-generate-ofi:
	-$(RM) -f $(OFI-IMG)
