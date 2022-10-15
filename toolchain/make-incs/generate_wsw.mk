# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2006-2007 Andreas Koch - the open7x0.org group
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

WSW-IMG_DEPS = $(GEN_WSW_BIN) $(RSAENCODE_BIN)

ifeq ($(CONFIG_GENERATE_WSW-IMG),y)
  ifneq ($(or $(CONFIG_GENERATE_CRAM_ROOTFS_IMAGE),\
     $(CONFIG_GENERATE_SQUASH_ROOTFS_IMAGE),\
     $(CONFIG_GENERATE_SIEMENS-LINUX-KERNEL-IMG)),y)
     $(error dependency error: wsw needs cramfs,squashfs or kernel image enabled)
  endif
endif
RSAKEYS_DIR := $(BUILDIN_DIR)/m7x0-keys
RSAPRIVKEY1 := $(RSAKEYS_DIR)/rsaprivate1.key
RSAPRIVKEY2 := $(RSAKEYS_DIR)/rsaprivate2.key

GEN_WSW_CMD_LINE = -m $(MD5) -c $(RSAENCODE_BIN) -1 $(RSAPRIVKEY1) -2 $(RSAPRIVKEY2) \
                    --$(CONFIG_M7X0_TYPE)
ifeq ($(CONFIG_GENERATE_SQUASH_ROOTFS_IMAGE),y)
   GEN_WSW_CMD_LINE += -r $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)
   WSW-IMG_DEPS += $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)
   WSW-IMG_TIME_REF_all += $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)
else
ifeq ($(CONFIG_GENERATE_CRAM_ROOTFS_IMAGE),y)
   GEN_WSW_CMD_LINE += -r $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
   WSW-IMG_DEPS += $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
   WSW-IMG_TIME_REF_all += $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
endif
endif
ifeq ($(CONFIG_GENERATE_SIEMENS-LINUX-KERNEL-IMG),y)
   GEN_WSW_CMD_LINE += -k $(SIEMENS-LINUX-KERNEL-IMG)
   WSW-IMG_DEPS += $(SIEMENS-LINUX-KERNEL-IMG)
   WSW-IMG_TIME_REF_all += $(SIEMENS-LINUX-KERNEL-IMG)
endif
ifeq ($(CONFIG_GENERATE_JFFS2_IMAGE),y)
   GEN_WSW_CMD_LINE += -d $(TOP_DIR)/$(JFFS2_IMG)
   WSW-IMG_DEPS += $(TOP_DIR)/$(JFFS2_IMG)
   WSW-IMG_TIME_REF_all += $(TOP_DIR)/$(JFFS2_IMG)
endif
GEN_WSW_FWNAME = $(or $(subst .wsw,,$(notdir $(CONFIG_WSW-IMG))), \
                 vdr-ng-em-$(CONFIG_M7X0_TYPE)-$(CONFIG_FW_VERSION)-%Y-%m-%d-%M%H)
WSW-IMG := $(TOP_DIR)/$(or $(notdir $(CONFIG_WSW-IMG)),vdr-ng-em-$(CONFIG_M7X0_TYPE)-$(CONFIG_FW_VERSION).wsw)
WSW-IMG_TIME_REF = $(firstword $(WSW-IMG_TIME_REF_all))
GEN_WSW_CMD_LINE += -o $(WSW-IMG)

POST_RULES_$(CONFIG_GENERATE_WSW-IMG) += $(WSW-IMG)
DISTCLEAN_RULES += distclean-generate-wsw

$(WSW-IMG): $$(WSW-IMG_DEPS) $(RSAPRIVKEY1) $(RSAPRIVKEY2)
	$(GEN_WSW_BIN) $(GEN_WSW_CMD_LINE) \
		-s `$(DATE) -r $(WSW-IMG_TIME_REF) +"%s"` \
		-n `$(DATE) -r $(WSW-IMG_TIME_REF) +"$(GEN_WSW_FWNAME)"`

.PHONY: distclean-generate-wsw

distclean-generate-wsw:
	-$(RM) -f $(WSW-IMG)
