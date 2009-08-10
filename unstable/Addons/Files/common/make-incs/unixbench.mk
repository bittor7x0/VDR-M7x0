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
UNIXBENCH_DEPS = $(BASE_BUILD_STAGEFILE)

UNIXBENCH_VERSION := 4.1.0
UNIXBENCH_PATCHES_DIR := $(PATCHES_DIR)/unixbench/$(UNIXBENCH_VERSION)

UNIXBENCH_FILE := unixbench-$(UNIXBENCH_VERSION).tgz
UNIXBENCH_DLFILE := $(DOWNLOAD_DIR)/$(UNIXBENCH_FILE)
UNIXBENCH_URL := http://www.tux.org/pub/tux/niemi/unixbench/$(UNIXBENCH_FILE)
UNIXBENCH_DIR := $(BUILD_DIR)/unixbench-$(UNIXBENCH_VERSION)

UNIXBENCH_PACKED = $(STAGEFILES_DIR)/.unixbench_packed

PACKS_RULES_$(CONFIG_UNIXBENCH) += $(UNIXBENCH_PACKED)

CLEAN_RULES += clean-unixbench
DISTCLEAN_RULES += distclean-unixbench

#
# download unixbench
#

$(UNIXBENCH_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(UNIXBENCH_DLFILE) ] ; then \
	$(WGET) $(UNIXBENCH_URL) -O $(UNIXBENCH_DLFILE) ; \
	fi );
	$(TOUCH) $(UNIXBENCH_DLFILE)

#
# unpack unixbench
#

$(STAGEFILES_DIR)/.unixbench_unpacked: $(UNIXBENCH_DLFILE) \
                                           $(wildcard $(UNIXBENCH_PATCHES_DIR)/*.patch) \
                                           $$(UNIXBENCH_DEPS)
	-$(RM) -rf $(UNIXBENCH_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(UNIXBENCH_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.unixbench_unpacked

#
# patch unixbench
#

$(STAGEFILES_DIR)/.unixbench_patched: $(STAGEFILES_DIR)/.unixbench_unpacked
	$(call patch_package, $(UNIXBENCH_DIR), $(UNIXBENCH_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.unixbench_patched

#
# compile unixbench
#

$(STAGEFILES_DIR)/.unixbench_compiled: $(STAGEFILES_DIR)/.unixbench_patched
	$(UCLIBC_ENV) $(MAKE) \
		-C $(UNIXBENCH_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.unixbench_compiled

#
# strip unixbench
#

$(STAGEFILES_DIR)/.unixbench_stripped: $(STAGEFILES_DIR)/.unixbench_compiled
	$(UCLIBC_ENV) $(MAKE) strip \
		-C $(UNIXBENCH_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.unixbench_stripped

#
# pack unixbench
#

$(STAGEFILES_DIR)/.unixbench_packed: $(STAGEFILES_DIR)/.unixbench_stripped
	-$(RM) -f $(TOP_DIR)/unixbench-$(UNIXBENCH_VERSION)-m7x0.tar.bz2
	tar jcvf $(TOP_DIR)/unixbench-$(UNIXBENCH_VERSION)-m7x0.tar.bz2 \
		--directory=$(BUILD_DIR) \
		unixbench-$(UNIXBENCH_VERSION)/pgms/ \
		unixbench-$(UNIXBENCH_VERSION)/results/ \
		unixbench-$(UNIXBENCH_VERSION)/testdir/ \
		unixbench-$(UNIXBENCH_VERSION)/tmp/ \
		unixbench-$(UNIXBENCH_VERSION)/README \
		unixbench-$(UNIXBENCH_VERSION)/Run
	$(TOUCH) $(STAGEFILES_DIR)/.unixbench_packed

.PHONY: clean-unixbench distclean-unixbench

clean-unixbench:
	-$(RM) -rf $(UNIXBENCH_DIR)

#
# clean everthing else
#

distclean-unixbench:
	-$(RM) -f $(STAGEFILES_DIR)/.unixbench_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.unixbench_patched
	-$(RM) -f $(STAGEFILES_DIR)/.unixbench_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.unixbench_stripped
	-$(RM) -f $(STAGEFILES_DIR)/.unixbench_packed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(UNIXBENCH_DLFILE)
endif
