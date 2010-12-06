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
# $Id: generate_rootfs.mk 374 2008-09-01 18:22:42Z andreas $
#


ifeq ($(strip $(CONFIG_ROOTFS_DIR)),)
  ROOTFS_DIR = $(TOP_DIR)/rootfs_dir_$(CONFIG_M7X0_TYPE)_$(CONFIG_VDR_TREE)
else
  ROOTFS_DIR := $(abspath $(CONFIG_ROOTFS_DIR))
endif
ROOTFS_FILE_TABLE := $(STAGEFILES_DIR)/file_tab$(subst /,.,$(subst $(TOP_DIR),,$(ROOTFS_DIR))).lst
FQ_FILE_LISTS = $(addprefix $(FILELIST_DIR)/, $(FILE_LISTS))
ROOTFS_DIR_DEPS = $(PACKS_BUILD_STAGEFILE) $$(FQ_FILE_LISTS) \
   $(COPY_LISTS_BIN) $(TOP_DIR)/.config

ifndef CONFIG_EXT2_ROOTFS_SIZE
  EXT2_ROOTFS_SIZE := 65536
else
  EXT2_ROOTFS_SIZE := CONFIG_EXT2_ROOTFS_SIZE
endif

ifeq ($(strip $(notdir $(CONFIG_EXT2_ROOTFS_IMG))),)
  EXT2_ROOTFS_IMG := rootfs-ext2_$(CONFIG_M7X0_TYPE)_$(CONFIG_VDR_TREE).img
else
  EXT2_ROOTFS_IMG :=$(notdir $(CONFIG_EXT2_ROOTFS_IMG))
endif

ifeq ($(strip $(notdir $(CONFIG_CRAM_ROOTFS_IMG))),)
  CRAM_ROOTFS_IMG = rootfs-cram_$(CONFIG_M7X0_TYPE)_$(CONFIG_VDR_TREE).img
else
  CRAM_ROOTFS_IMG =$(notdir $(CONFIG_CRAM_ROOTFS_IMG))
endif

ifeq ($(strip $(notdir $(CONFIG_SQUASH_ROOTFS_IMG))),)
  SQUASH_ROOTFS_IMG = rootfs-squash_$(CONFIG_M7X0_TYPE)_$(CONFIG_VDR_TREE).img
else
  SQUASH_ROOTFS_IMG =$(notdir $(CONFIG_SQUASH_ROOTFS_IMG))
endif

POST_RULES_$(CONFIG_GENERATE_ROOTFS_DIR) += $(ROOTFS_FILE_TABLE)
POST_RULES_$(CONFIG_GENERATE_EXT2_ROOTFS_IMAGE) += $(TOP_DIR)/$(EXT2_ROOTFS_IMG)
POST_RULES_$(CONFIG_GENERATE_CRAM_ROOTFS_IMAGE) += $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
POST_RULES_$(CONFIG_GENERATE_SQUASH_ROOTFS_IMAGE) += $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)

DISTCLEAN_RULES += distclean-generate-rootfs

AWK_LST_TRANS_PRG := '$$1 !~ /\#/ && $$3 !~ /l/ && $$3 !~ /s/ \
   { file="/"$$1 ; gsub("//","/",file) ; \
    print file, $$3, $$4, $$5, $$6, $$7, $$8, $$9, $$10, $$11}'


$(ROOTFS_FILE_TABLE): $(ROOTFS_DIR_DEPS)
	-$(RM) -rf $(ROOTFS_DIR)
	-$(RM) -f $(ROOTFS_FILE_TABLE)
	$(MKDIR) -p $(ROOTFS_DIR)
	$(COPY_LISTS_BIN) -s '$(ROOTFS_DIR)' '$(TARGET_ROOT)' \
		'$(PREFIX_BIN)/$(UCLIBC_STRIP)' $(FQ_FILE_LISTS)
	$(CAT) $(FQ_FILE_LISTS) | $(AWK) $(AWK_LST_TRANS_PRG) > \
		$(ROOTFS_FILE_TABLE)

$(TOP_DIR)/$(EXT2_ROOTFS_IMG): $$(GENEXT2FS_BIN) $(ROOTFS_FILE_TABLE)
	-$(RM) -f $(TOP_DIR)/$(EXT2_ROOTFS_IMG)
	$(GENEXT2FS_BIN) -D $(ROOTFS_FILE_TABLE) -d $(ROOTFS_DIR) \
		-b $(EXT2_ROOTFS_SIZE) $(TOP_DIR)/$(EXT2_ROOTFS_IMG)

$(TOP_DIR)/$(SQUASH_ROOTFS_IMG): $$(MKSQUASHFS_BIN) $(ROOTFS_FILE_TABLE)
	-$(RM) -f $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)
	$(MKSQUASHFS_BIN) $(ROOTFS_DIR) $(TOP_DIR)/$(SQUASH_ROOTFS_IMG) \
		-all-root -be -noappend -noI -no-fragments -d $(ROOTFS_FILE_TABLE)
ifeq ($(strip $(HOST_BS)),OpenBSD)
	@if	$(TEST) `$(STAT) -f b $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)` \
	    		-gt $(ROOTFS_MAX_IMGSIZE); then \
	    	$(ECHO) squashfs image too big!; \
	    	$(RM) -f $(TOP_DIR)/$(SQUASH_ROOTFS_IMG); \
	    	exit 1; \
	fi
else
	@if	$(TEST) `$(STAT) --format="%s" $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)` \
	    		-gt $(ROOTFS_MAX_IMGSIZE); then \
	    	$(ECHO) squashfs image too big!; \
	    	$(RM) -f $(TOP_DIR)/$(SQUASH_ROOTFS_IMG); \
	    	exit 1; \
	fi
endif

ifeq ($(CONFIG_CRAMFS_NEEDS_SWAP),y)

$(TOP_DIR)/$(CRAM_ROOTFS_IMG): $$(MKCRAMFS_BIN) $$(CRAMFSSWAP_BIN) $(ROOTFS_FILE_TABLE)
	-$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
	-$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG).host
	$(MKCRAMFS_BIN) -D $(ROOTFS_FILE_TABLE) $(ROOTFS_DIR) \
		$(TOP_DIR)/$(CRAM_ROOTFS_IMG).host
	$(CRAMFSSWAP_BIN) $(TOP_DIR)/$(CRAM_ROOTFS_IMG).host \
                     $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
	$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG).host
	
ifeq ($(strip $(HOST_BS)),OpenBSD)
	@if	$(TEST) `$(STAT) -f b $(TOP_DIR)/$(CRAM_ROOTFS_IMG)` \
	    		-gt $(ROOTFS_MAX_IMGSIZE); then \
	    	$(ECHO) cramfs image too big!; \
	    	$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG); \
	    	exit 1; \
	fi
else
	@if	$(TEST) `$(STAT) --format="%s" $(TOP_DIR)/$(CRAM_ROOTFS_IMG)` \
	    		-gt $(ROOTFS_MAX_IMGSIZE); then \
	    	$(ECHO) cramfs image too big!; \
	    	$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG); \
	    	exit 1; \
	fi
endif
	
else

$(TOP_DIR)/$(CRAM_ROOTFS_IMG): $$(MKCRAMFS_BIN) $(ROOTFS_FILE_TABLE)
	-$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
	$(MKCRAMFS_BIN) -D $(ROOTFS_FILE_TABLE) $(ROOTFS_DIR) \
		$(TOP_DIR)/$(CRAM_ROOTFS_IMG)
	
ifeq ($(strip $(HOST_BS)),OpenBSD)
	@if	$(TEST) `$(STAT) -f b $(TOP_DIR)/$(CRAM_ROOTFS_IMG)` \
			-gt $(CRAMFS_MAX_IMGSIZE); then \
		$(ECHO) cramfs image too big!; \
		$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG); \
		exit 1; \
	fi
else
	@if	$(TEST) `$(STAT) --format="%s" $(TOP_DIR)/$(CRAM_ROOTFS_IMG)` \
			-gt $(CRAMFS_MAX_IMGSIZE); then \
		$(ECHO) cramfs image too big!; \
		$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG); \
		exit 1; \
	fi
endif

endif

.PHONY: distclean-generate-rootfs
distclean-generate-rootfs:
	-$(RM) -f $(ROOTFS_FILE_TABLE)
	-$(RM) -rf $(ROOTFS_DIR)
	-$(RM) -f $(TOP_DIR)/$(EXT2_ROOTFS_IMG)
	-$(RM) -f $(TOP_DIR)/$(CRAM_ROOTFS_IMG)
	-$(RM) -f $(TOP_DIR)/$(SQUASH_ROOTFS_IMG)
