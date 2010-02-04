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
RUNIT_DEPS = $(BASE_BUILD_STAGEFILE)

RUNIT_VERSION := 2.1.1
RUNIT_PATCHES_DIR := $(PATCHES_DIR)/runit/$(RUNIT_VERSION)

RUNIT_FILE := runit-$(RUNIT_VERSION).tar.gz
RUNIT_DLFILE := $(DOWNLOAD_DIR)/$(RUNIT_FILE)
RUNIT_URL := http://smarden.org/runit/$(RUNIT_FILE)
RUNIT_DIR := $(BUILD_DIR)/runit-$(RUNIT_VERSION)

RUNIT_INSTALLED = $(STAGEFILES_DIR)/.runit_installed

PACKS_RULES_$(CONFIG_RUNIT) += $(RUNIT_INSTALLED)
FILE_LISTS_$(CONFIG_RUNIT) += runit.lst

CLEAN_RULES += clean-runit
DISTCLEAN_RULES += distclean-runit

#
# download runit
#

$(RUNIT_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(RUNIT_DLFILE) ] ; then \
	$(WGET) $(RUNIT_URL) -O $(RUNIT_DLFILE) ; \
	fi );
	$(TOUCH) $(RUNIT_DLFILE)

#
# unpack runit
#

$(STAGEFILES_DIR)/.runit_unpacked: $(RUNIT_DLFILE) \
                                           $(wildcard $(RUNIT_PATCHES_DIR)/*.patch) \
                                           $$(RUNIT_DEPS)
	-$(RM) -rf $(RUNIT_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(RUNIT_DLFILE)
	$(MV) -f $(BUILD_DIR)/admin/* $(BUILD_DIR)
	-$(RM) -rf $(BUILD_DIR)/admin
	$(TOUCH) $(STAGEFILES_DIR)/.runit_unpacked

#
# patch runit
#

$(STAGEFILES_DIR)/.runit_patched: $(STAGEFILES_DIR)/.runit_unpacked
	$(call patch_package, $(RUNIT_DIR), $(RUNIT_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.runit_patched

#
# compile runit
#

$(STAGEFILES_DIR)/.runit_compiled: $(STAGEFILES_DIR)/.runit_patched
	$(UCLIBC_ENV) HOSTCC= $(MAKE) \
		-C $(RUNIT_DIR)/src \
		HOSTCC=$(UCLIBC_CC)
	$(TOUCH) $(STAGEFILES_DIR)/.runit_compiled

#
# install runit
#

$(STAGEFILES_DIR)/.runit_installed: $(STAGEFILES_DIR)/.runit_compiled
	$(CP) $(RUNIT_DIR)/src/chpst $(TARGET_ROOT)/sbin/chpst
	$(CP) $(RUNIT_DIR)/src/runit $(TARGET_ROOT)/sbin/runit
	$(CP) $(RUNIT_DIR)/src/runit-init $(TARGET_ROOT)/sbin/runit-init
	$(CP) $(RUNIT_DIR)/src/runsv $(TARGET_ROOT)/sbin/runsv
	$(CP) $(RUNIT_DIR)/src/runsvchdir $(TARGET_ROOT)/sbin/runsvchdir
	$(CP) $(RUNIT_DIR)/src/runsvdir $(TARGET_ROOT)/sbin/runsvdir
	$(CP) $(RUNIT_DIR)/src/sv $(TARGET_ROOT)/sbin/sv
	$(CP) $(RUNIT_DIR)/src/svlogd $(TARGET_ROOT)/sbin/svlogd
	$(CP) $(RUNIT_DIR)/src/utmpset $(TARGET_ROOT)/sbin/utmpset
	$(TOUCH) $(STAGEFILES_DIR)/.runit_installed


$(FILELIST_DIR)/runit.lst: $(STAGEFILES_DIR)/.runit_installed
	$(TOUCH) $(FILELIST_DIR)/runit.lst

.PHONY: clean-runit distclean-runit

clean-runit:
	-$(RM) -rf $(RUNIT_DIR)

#
# clean everthing else
#

distclean-runit:
	-$(RM) -f $(STAGEFILES_DIR)/.runit_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.runit_patched
	-$(RM) -f $(STAGEFILES_DIR)/.runit_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.runit_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(RUNIT_DLFILE)
endif
