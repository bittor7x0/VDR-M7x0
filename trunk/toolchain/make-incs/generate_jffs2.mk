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
#
# The project's page is at http://www.open7x0.org
#
#

JFFS2_DIR_DEPS = $(PACKS_BUILD_STAGEFILE) $(TOP_DIR)/.config

ifeq ($(strip $(CONFIG_JFFS2_DIR)),)
  JFFS2_DIR = $(TOP_DIR)/jffs2_dir_$(CONFIG_M7X0_TYPE)
else
  JFFS2_DIR := $(abspath $(CONFIG_JFFS2_DIR))
endif

ifeq ($(CONFIG_M7X0_TYPE),m740)
  JFFS2_MAX_IMGSIZE := 6029312
else ifeq ($(CONFIG_M7X0_TYPE),m750s)
  JFFS2_MAX_IMGSIZE := 3932160
else
$(error unknown system type given)
endif

ifeq ($(strip $(notdir $(CONFIG_JFFS2_IMG))),)
  JFFS2_IMG = jffs2-$(CONFIG_M7X0_TYPE).img
else
  JFFS2_IMG =$(notdir $(CONFIG_JFFS2_IMG))
endif

ifeq ($(CONFIG_JFFS2_LZO),y)
  JFFS2_OPTIONS := -x zlib -x lzari -X lzo -X rtime
endif

POST_RULES_$(CONFIG_GENERATE_JFFS2_DIR) += $(JFFS2_DIR)
POST_RULES_$(CONFIG_GENERATE_JFFS2_IMAGE) += $(TOP_DIR)/$(JFFS2_IMG)

DISTCLEAN_RULES += distclean-generate-jffs2

$(JFFS2_DIR): $(JFFS2_DIR_DEPS)
	-$(RM) -rf $(JFFS2_DIR)
	# Create "SI" directory for factory reset
	$(MKDIR) -p $(JFFS2_DIR)/SI

$(TOP_DIR)/$(JFFS2_IMG): $$(MKJFFS2_BIN) $(JFFS2_DIR)
	-$(RM) -f $(TOP_DIR)/$(JFFS2_IMG)
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
	-$(RM) -rf $(JFFS2_DIR)
	-$(RM) -f $(TOP_DIR)/$(JFFS2_IMG)
