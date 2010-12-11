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
# $Id: generate_wsw.mk 366 2008-08-17 18:54:21Z andreas $
#

WSW-IMG_DEPS = $(GEN_WSW_BIN) $(RSAENCODE_BIN)

ifeq ($(CONFIG_GENERATE_WSW-IMG),y)
  ifneq ($(or $(CONFIG_GENERATE_CRAM_ROOTFS_IMAGE),\
     $(CONFIG_GENERATE_SQUASH_ROOTFS_IMAGE),\
     $(CONFIG_GENERATE_SIEMENS-LINUX-KERNEL-IMG)),y)
     $(error dependency error: wsw needs cramfs,squashfs or kernel image enabled)
  endif
endif
RSAKEYS_TARGET_DIR := $(TARGET_ROOT)/etc/rsa
RSAPRIVKEY1 := $(RSAKEYS_TARGET_DIR)/rsaprivate1.key
RSAPRIVKEY2 := $(RSAKEYS_TARGET_DIR)/rsaprivate2.key

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
                 open7x0.org-$(CONFIG_M7X0_TYPE)-%Y-%m-%d-%M%H)
WSW-IMG := $(TOP_DIR)/$(or $(notdir $(CONFIG_WSW-IMG)),open7x0.org-$(CONFIG_M7X0_TYPE).wsw)
WSW-IMG_TIME_REF = $(firstword $(WSW-IMG_TIME_REF_all))
GEN_WSW_CMD_LINE += -o $(WSW-IMG)


RSAKEYS_BUILD_BASEDIR = siemens-build-env-rsakeys
RSAKEYS_BUILD_DIR := $(BUILD_DIR)/$(RSAKEYS_BUILD_BASEDIR)

POST_RULES_$(CONFIG_GENERATE_WSW-IMG) += $(WSW-IMG)
CLEAN_RULES += clean-generate-wsw
DISTCLEAN_RULES += distclean-generate-wsw

$(RSAPRIVKEY1) $(RSAPRIVKEY2): $$(SIEMENS-BUILD-ENV_DLFILE)
	-$(RM) -rf $(RSAKEYS_BUILD_DIR)
	$(TAR) --wildcards -C $(BUILD_DIR) -xzf $(SIEMENS-BUILD-ENV_DLFILE) \
		'$(SIEMENS-BUILD-ENV_TAR_BASE_DIR)/.rsa*'
	$(MV) $(BUILD_DIR)/$(SIEMENS-BUILD-ENV_TAR_BASE_DIR) $(RSAKEYS_BUILD_DIR)
	-$(MKDIR) -p $(RSAKEYS_TARGET_DIR)
	$(CP) $(RSAKEYS_BUILD_DIR)/.rsasecret $(RSAPRIVKEY1)
	$(CP) $(RSAKEYS_BUILD_DIR)/.rsasecret2 $(RSAPRIVKEY2)

$(WSW-IMG): $$(WSW-IMG_DEPS) $(RSAPRIVKEY1) $(RSAPRIVKEY2)
	$(GEN_WSW_BIN) $(GEN_WSW_CMD_LINE) \
		-s `$(DATE) -r $(WSW-IMG_TIME_REF) +"%s"` \
		-n `$(DATE) -r $(WSW-IMG_TIME_REF) +"$(GEN_WSW_FWNAME)"`

.PHONY: clean-generate-wsw distclean-generate-wsw

clean-generate-wsw:
	-$(RM) -rf $(RSAKEYS_BUILD_DIR)

distclean-generate-wsw:
	-$(RM) -f $(WSW-IMG)
