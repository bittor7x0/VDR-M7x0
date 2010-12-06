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
HACKBENCH_DEPS = $(BASE_BUILD_STAGEFILE)

HACKBENCH_PATCHES_DIR := $(PATCHES_DIR)/hackbench

HACKBENCH_FILE := hackbench.c
HACKBENCH_DLFILE := $(DOWNLOAD_DIR)/$(HACKBENCH_FILE)
HACKBENCH_URL := http://people.redhat.com/mingo/cfs-scheduler/tools/$(HACKBENCH_FILE)
HACKBENCH_DIR := $(BUILD_DIR)/hackbench

HACKBENCH_INSTALLED = $(STAGEFILES_DIR)/.hackbench_installed

PACKS_RULES_$(CONFIG_HACKBENCH) += $(HACKBENCH_INSTALLED)
FILE_LISTS_$(CONFIG_HACKBENCH) += hackbench.lst

CLEAN_RULES += clean-hackbench
DISTCLEAN_RULES += distclean-hackbench

#
# download hackbench
#

$(HACKBENCH_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(HACKBENCH_DLFILE) ] ; then \
	$(WGET) $(HACKBENCH_URL) -O $(HACKBENCH_DLFILE) ; \
	fi );
	$(TOUCH) $(HACKBENCH_DLFILE)

#
# patch hackbench
#

$(STAGEFILES_DIR)/.hackbench_patched: $(HACKBENCH_DLFILE) \
                                           $(wildcard $(HACKBENCH_PATCHES_DIR)/*.patch) \
                                           $$(HACKBENCH_DEPS)
	$(call patch_package, $(HACKBENCH_DIR), $(HACKBENCH_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.hackbench_patched

#
# compile hackbench
#

$(STAGEFILES_DIR)/.hackbench_compiled: $(STAGEFILES_DIR)/.hackbench_patched
	-$(RM) -rf $(HACKBENCH_DIR)
	$(MKDIR) -p $(HACKBENCH_DIR)
	$(CP) $(HACKBENCH_DLFILE) $(HACKBENCH_DIR)/hackbench.c
	$(PREFIX_BIN)/$(UCLIBC_CC) -g -Wall -o $(HACKBENCH_DIR)/hackbench $(HACKBENCH_DIR)/hackbench.c -lpthread
	$(TOUCH) $(STAGEFILES_DIR)/.hackbench_compiled

#
# install hackbench
#

$(STAGEFILES_DIR)/.hackbench_installed: $(STAGEFILES_DIR)/.hackbench_compiled
	$(CP) $(HACKBENCH_DIR)/hackbench $(TARGET_ROOT)/usr/sbin/hackbench
	$(TOUCH) $(STAGEFILES_DIR)/.hackbench_installed


$(FILELIST_DIR)/hackbench.lst: $(STAGEFILES_DIR)/.hackbench_installed
	$(TOUCH) $(FILELIST_DIR)/hackbench.lst

.PHONY: clean-hackbench distclean-hackbench

clean-hackbench:
	-$(RM) -rf $(HACKBENCH_DIR)

#
# clean everthing else
#

distclean-hackbench:
	-$(RM) -f $(STAGEFILES_DIR)/.hackbench_patched
	-$(RM) -f $(STAGEFILES_DIR)/.hackbench_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.hackbench_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(HACKBENCH_DLFILE)
endif
