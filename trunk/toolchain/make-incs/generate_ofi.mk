# Copyright (C) 2006,2007 Andreas Koch - the open7x0.org group
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
# Or, point your browser to http://www.gnu.org/copyleft/gpl.html
#
# The author can be reached at andreas@open7x0.org
#
# The project's page is at http://www.open7x0.org
#
# $Id: generate_ofi.mk 366 2008-08-17 18:54:21Z andreas $
#

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
OFI-IMG := $(TOP_DIR)/$(or $(notdir $(CONFIG_OFI-IMG)),open7x0.org-$(CONFIG_M7X0_TYPE).ofi)
OFI-IMG_TIME_REF = $(firstword $(OFI-IMG_TIME_REF_all))
GEN_OFI_CMD_LINE += -o $(OFI-IMG)

POST_RULES_$(CONFIG_GENERATE_OFI-IMG) += $(OFI-IMG)
DISTCLEAN_RULES += distclean-generate-wsw

$(OFI-IMG): $$(OFI-IMG_DEPS)
	$(GEN_OFI_BIN) $(GEN_OFI_CMD_LINE) \
		-d `$(DATE) -r $(OFI-IMG_TIME_REF) +"%s"`

.PHONY: distclean-generate-ofi

distclean-generate-ofi:
	-$(RM) -f $(OFI-IMG)
