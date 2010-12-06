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

# Put dependencies here all pack should depend on $$(BASE_BUILD_STAGEFILE)
LMBENCH_DEPS = $(BASE_BUILD_STAGEFILE)

LMBENCH_VERSION := 3
LMBENCH_PATCHES_DIR := $(PATCHES_DIR)/lmbench/$(LMBENCH_VERSION)

LMBENCH_FILE := lmbench$(LMBENCH_VERSION).tar.gz
LMBENCH_DLFILE := $(DOWNLOAD_DIR)/$(LMBENCH_FILE)
LMBENCH_URL := http://www.bitmover.com/lmbench/$(LMBENCH_FILE)
LMBENCH_DIR := $(BUILD_DIR)/lmbench$(LMBENCH_VERSION)

LMBENCH_PACKED = $(STAGEFILES_DIR)/.lmbench_packed

PACKS_RULES_$(CONFIG_LMBENCH) += $(LMBENCH_PACKED)

CLEAN_RULES += clean-lmbench
DISTCLEAN_RULES += distclean-lmbench

#
# download lmbench
#

$(LMBENCH_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(LMBENCH_DLFILE) ] ; then \
	$(WGET) $(LMBENCH_URL) -O $(LMBENCH_DLFILE) ; \
	fi );
	$(TOUCH) $(LMBENCH_DLFILE)

#
# unpack lmbench
#

$(STAGEFILES_DIR)/.lmbench_unpacked: $(LMBENCH_DLFILE) \
                                           $(wildcard $(LMBENCH_PATCHES_DIR)/*.patch) \
                                           $$(LMBENCH_DEPS)
	-$(RM) -rf $(LMBENCH_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(LMBENCH_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.lmbench_unpacked

#
# patch lmbench
#

$(STAGEFILES_DIR)/.lmbench_patched: $(STAGEFILES_DIR)/.lmbench_unpacked
	$(call patch_package, $(LMBENCH_DIR), $(LMBENCH_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.lmbench_patched

#
# compile lmbench
#

$(STAGEFILES_DIR)/.lmbench_compiled: $(STAGEFILES_DIR)/.lmbench_patched
	$(UCLIBC_ENV) $(MAKE) \
		-C $(LMBENCH_DIR) \
		OS=mips-Linux \
		CC=$(UCLIBC_CC)
	$(TOUCH) $(STAGEFILES_DIR)/.lmbench_compiled

#
# strip lmbench
#

$(STAGEFILES_DIR)/.lmbench_stripped: $(STAGEFILES_DIR)/.lmbench_compiled
	$(UCLIBC_ENV) $(MAKE) strip \
		-C $(LMBENCH_DIR) \
		OS=mips-Linux \
		CC=$(UCLIBC_CC)
	$(TOUCH) $(STAGEFILES_DIR)/.lmbench_stripped

#
# pack lmbench
#

$(STAGEFILES_DIR)/.lmbench_packed: $(STAGEFILES_DIR)/.lmbench_stripped
	-$(RM) -f $(TOP_DIR)/lmbench$(LMBENCH_VERSION)-m7x0.tar.bz2
	chmod 755 $(LMBENCH_DIR)/m7x0-make-results.sh
	$(MV) -f $(LMBENCH_DIR)/CONFIG.m740av $(LMBENCH_DIR)/bin/mips-Linux/CONFIG.m740av
	tar jcvf $(TOP_DIR)/lmbench$(LMBENCH_VERSION)-m7x0.tar.bz2 \
		--directory=$(BUILD_DIR) lmbench$(LMBENCH_VERSION)/
	$(TOUCH) $(STAGEFILES_DIR)/.lmbench_packed

.PHONY: clean-lmbench distclean-lmbench

clean-lmbench:
	-$(RM) -rf $(LMBENCH_DIR)

#
# clean everthing else
#

distclean-lmbench:
	-$(RM) -f $(STAGEFILES_DIR)/.lmbench_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.lmbench_patched
	-$(RM) -f $(STAGEFILES_DIR)/.lmbench_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.lmbench_stripped
	-$(RM) -f $(STAGEFILES_DIR)/.lmbench_packed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(LMBENCH_DLFILE)
endif
