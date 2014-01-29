# --- VDR-NG-EM-COPYRIGHT-NOTE-BEGIN ---
#
# Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 VDR-NG-EM Project
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

JFFS2_DIR_DEPS = $(PACKS_BUILD_STAGEFILE) $$(FQ_FILE_LISTS) \
   $(COPY_LISTS_BIN) $(UPX_BIN) $$(ROOTFS_FILE_TABLE) $(TOP_DIR)/.config

ifeq ($(strip $(CONFIG_JFFS2_DIR)),)
  JFFS2_DIR = $(TOP_DIR)/jffs2_dir_$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION)
else
  JFFS2_DIR := $(abspath $(CONFIG_JFFS2_DIR))
endif

JFFS2_FILE_COPY :=  $(STAGEFILES_DIR)/file_copy$(subst /,.,$(subst $(TOP_DIR),,$(JFFS2_DIR))).lst

ifeq ($(CONFIG_M7X0_TYPE),m740)
  JFFS2_MAX_IMGSIZE := 6029312
else ifeq ($(CONFIG_M7X0_TYPE),m750s)
  JFFS2_MAX_IMGSIZE := 3932160
else
$(error unknown system type given)
endif

ifeq ($(strip $(notdir $(CONFIG_JFFS2_IMG))),)
  JFFS2_IMG = jffs2-$(CONFIG_M7X0_TYPE)_$(CONFIG_FW_VERSION).img
else
  JFFS2_IMG =$(notdir $(CONFIG_JFFS2_IMG))
endif

ifeq ($(CONFIG_JFFS2_LZO),y)
  JFFS2_OPTIONS := -x zlib -x lzari -X lzo -X rtime
endif

POST_RULES_$(CONFIG_GENERATE_JFFS2_DIR) += $(JFFS2_FILE_COPY)
POST_RULES_$(CONFIG_GENERATE_JFFS2_IMAGE) += $(TOP_DIR)/$(JFFS2_IMG)

DISTCLEAN_RULES += distclean-generate-jffs2

AWK_JFFS2_LST_TRANS_PRG_COPY := '$$1 ~ /^\/?etc/ || $$1 ~ /^\/?root/ || $$1 ~ /^\/?home/ \
	{ print }'

$(JFFS2_FILE_COPY): $(JFFS2_DIR_DEPS)
	-$(RM) -rf $(JFFS2_DIR)
	-$(RM) -f $(JFFS2_FILE_COPY)
	# Create "SI" directory for factory reset
	$(MKDIR) -p $(JFFS2_DIR)/SI $(JFFS2_DIR)/log
ifeq ($(strip $(CONFIG_COPY_ROOTFS_TO_JFFS2_FIRST_RUN)),)
	$(TOUCH) $(JFFS2_DIR)/SI/.first_run
endif
	$(CAT) $(FQ_FILE_LISTS) | $(AWK) $(AWK_JFFS2_LST_TRANS_PRG_COPY) > \
		$(JFFS2_FILE_COPY)

$(TOP_DIR)/$(JFFS2_IMG): $$(MKJFFS2_BIN) $(JFFS2_FILE_COPY)
	-$(RM) -f $(TOP_DIR)/$(JFFS2_IMG)
	$(COPY_LISTS_BIN) -m '$(JFFS2_DIR)' '$(TARGET_ROOT)' \
		'$(PREFIX_BIN)/$(UCLIBC_STRIP)' '$(PREFIX_BIN)/upx' $(JFFS2_FILE_COPY)
	$(SED) -i -e "s,^export SYSTEMTYPE=.*,export SYSTEMTYPE=`$(CAT) $(JFFS2_DIR)/etc/systemtype`,g" $(JFFS2_DIR)/etc/rc.mini
	$(call set_lang, $(JFFS2_DIR))
	$(ECHO) "SVN ToolChain `svnversion -c \"$(TOP_DIR)\" | cut -d \":\" -f 2`" >> $(JFFS2_DIR)/etc/fw-version
ifneq ($(strip $(CONFIG_VDR)),)
	$(ECHO) "SVN VDR `svnversion -c \"$(VDR_DIR)\" | cut -d \":\" -f 2`" >> $(JFFS2_DIR)/etc/fw-version
endif
ifneq ($(strip $(CONFIG_VDR-PLUGINS)),)
	$(ECHO) "SVN VDR-plugins `svnversion -c \"$(VDR-PLUGINS_DIR)\" | cut -d \":\" -f 2`" >> $(JFFS2_DIR)/etc/fw-version
endif
ifneq ($(strip $(CONFIG_WEBIF)),)
	$(ECHO) "SVN webif `svnversion -c \"$(WEBIF_DIR)\" | cut -d \":\" -f 2`" >> $(JFFS2_DIR)/etc/fw-version
endif
	$(MKJFFS2_BIN) --big-endian --pad --squash \
		$(JFFS2_OPTIONS) \
		--root="$(JFFS2_DIR)" \
		--output=$(TOP_DIR)/$(JFFS2_IMG)
	# Add JFFS2 end-of-filesystem mark
	$(ECHO) $(shell $(ECHO) -ne '\xde\xad\xc0\xde') >> $(TOP_DIR)/$(JFFS2_IMG)
ifeq ($(strip $(HOST_BS)),OpenBSD)
	@if	$(TEST) `$(STAT) -f b $(TOP_DIR)/$(JFFS2_IMG)` \
	    		-gt $(JFFS2_MAX_IMGSIZE); then \
	    	$(ECHO) jffs2 image too big!; \
	    	$(RM) -f $(TOP_DIR)/$(JFFS2_IMG); \
	    	exit 1; \
	fi
else
	@if	$(TEST) `$(STAT) --format="%s" $(TOP_DIR)/$(JFFS2_IMG)` \
	    		-gt $(JFFS2_MAX_IMGSIZE); then \
	    	$(ECHO) jffs2 image too big!; \
	    	$(RM) -f $(TOP_DIR)/$(JFFS2_IMG); \
	    	exit 1; \
	fi
endif

.PHONY: distclean-generate-jffs2
distclean-generate-jffs2:
	-$(RM) -f $(JFFS2_FILE_COPY)
	-$(RM) -rf $(JFFS2_DIR)
	-$(RM) -f $(TOP_DIR)/$(JFFS2_IMG)
