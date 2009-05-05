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
PIPE-TEST_DEPS = $(BASE_BUILD_STAGEFILE)

PIPE-TEST_PATCHES_DIR := $(PATCHES_DIR)/pipe-test

#http://people.redhat.com/mingo/cfs-scheduler/tools/pipe-test.c
PIPE-TEST_FILE := pipe-test.c
PIPE-TEST_DLFILE := $(DOWNLOAD_DIR)/$(PIPE-TEST_FILE)
PIPE-TEST_URL := http://people.redhat.com/mingo/cfs-scheduler/tools/$(PIPE-TEST_FILE)
PIPE-TEST_DIR := $(BUILD_DIR)/pipe-test

PIPE-TEST_INSTALLED = $(STAGEFILES_DIR)/.pipe-test_installed

PACKS_RULES_$(CONFIG_PIPE-TEST) += $(PIPE-TEST_INSTALLED)
FILE_LISTS_$(CONFIG_PIPE-TEST) += pipe-test.lst

CLEAN_RULES += clean-pipe-test
DISTCLEAN_RULES += distclean-pipe-test

#
# download pipe-test
#

$(PIPE-TEST_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(PIPE-TEST_DLFILE) ] ; then \
	$(WGET) $(PIPE-TEST_URL) -O $(PIPE-TEST_DLFILE) ; \
	fi );
	$(TOUCH) $(PIPE-TEST_DLFILE)

#
# patch pipe-test
#

$(STAGEFILES_DIR)/.pipe-test_patched: $(PIPE-TEST_DLFILE) \
                                           $(wildcard $(PIPE-TEST_PATCHES_DIR)/*.patch) \
                                           $$(PIPE-TEST_DEPS)
	$(call patch_package, $(PIPE-TEST_DIR), $(PIPE-TEST_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.pipe-test_patched

#
# compile pipe-test
#

$(STAGEFILES_DIR)/.pipe-test_compiled: $(STAGEFILES_DIR)/.pipe-test_patched
	-$(RM) -rf $(PIPE-TEST_DIR)
	$(MKDIR) -p $(PIPE-TEST_DIR)
	$(CP) $(PIPE-TEST_DLFILE) $(PIPE-TEST_DIR)/pipe-test.c
	$(PREFIX_BIN)/$(UCLIBC_CC) -g -Wall -o $(PIPE-TEST_DIR)/pipe-test $(PIPE-TEST_DIR)/pipe-test.c
	$(TOUCH) $(STAGEFILES_DIR)/.pipe-test_compiled

#
# install pipe-test
#

$(STAGEFILES_DIR)/.pipe-test_installed: $(STAGEFILES_DIR)/.pipe-test_compiled
	$(CP) $(PIPE-TEST_DIR)/pipe-test $(TARGET_ROOT)/usr/sbin/pipe-test
	chmod 755 $(TARGET_ROOT)/usr/sbin/pipe-test
	$(TOUCH) $(STAGEFILES_DIR)/.pipe-test_installed


$(FILELIST_DIR)/pipe-test.lst: $(STAGEFILES_DIR)/.pipe-test_installed
	$(TOUCH) $(FILELIST_DIR)/pipe-test.lst

.PHONY: clean-pipe-test distclean-pipe-test

clean-pipe-test:
	-$(RM) -rf $(PIPE-TEST_DIR)

#
# clean everthing else
#

distclean-pipe-test:
	-$(RM) -f $(STAGEFILES_DIR)/.pipe-test_patched
	-$(RM) -f $(STAGEFILES_DIR)/.pipe-test_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.pipe-test_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(PIPE-TEST_DLFILE)
endif
