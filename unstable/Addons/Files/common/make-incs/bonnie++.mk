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
BONNIE++_DEPS = $(BASE_BUILD_STAGEFILE)

BONNIE++_VERSION := 1.03e
BONNIE++_PATCHES_DIR := $(PATCHES_DIR)/bonnie++/$(BONNIE++_VERSION)

#http://www.coker.com.au/bonnie++/bonnie++-1.03e.tgz
BONNIE++_FILE := bonnie++-$(BONNIE++_VERSION).tgz
BONNIE++_DLFILE := $(DOWNLOAD_DIR)/$(BONNIE++_FILE)
BONNIE++_URL := http://www.coker.com.au/bonnie++/$(BONNIE++_FILE)
BONNIE++_DIR := $(BUILD_DIR)/bonnie++-$(BONNIE++_VERSION)

BONNIE++_INSTALLED = $(STAGEFILES_DIR)/.bonnie++_installed

PACKS_RULES_$(CONFIG_BONNIE++) += $(BONNIE++_INSTALLED)
FILE_LISTS_$(CONFIG_BONNIE++) += bonnie++.lst

CLEAN_RULES += clean-bonnie++
DISTCLEAN_RULES += distclean-bonnie++

#
# download bonnie++
#

$(BONNIE++_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(BONNIE++_DLFILE) ] ; then \
	$(WGET) $(BONNIE++_URL) -O $(BONNIE++_DLFILE) ; \
	fi );
	$(TOUCH) $(BONNIE++_DLFILE)

#
# unpack bonnie++
#

$(STAGEFILES_DIR)/.bonnie++_unpacked: $(BONNIE++_DLFILE) \
                                           $(wildcard $(BONNIE++_PATCHES_DIR)/*.patch) \
                                           $$(BONNIE++_DEPS)
	-$(RM) -rf $(BONNIE++_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(BONNIE++_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.bonnie++_unpacked

#
# patch bonnie++
#

$(STAGEFILES_DIR)/.bonnie++_patched: $(STAGEFILES_DIR)/.bonnie++_unpacked
	$(call patch_package, $(BONNIE++_DIR), $(BONNIE++_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.bonnie++_patched

#
# configure bonnie++
#

$(STAGEFILES_DIR)/.bonnie++_configured: $(STAGEFILES_DIR)/.bonnie++_patched
	($(CD) $(BONNIE++_DIR) ; $(UCLIBC_ENV) \
		$(BONNIE++_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET))
	$(TOUCH) $(STAGEFILES_DIR)/.bonnie++_configured

#
# compile bonnie++
#

$(STAGEFILES_DIR)/.bonnie++_compiled: $(STAGEFILES_DIR)/.bonnie++_configured
	$(UCLIBC_ENV) $(MAKE) \
		-C $(BONNIE++_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.bonnie++_compiled

#
# install bonnie++
#

$(STAGEFILES_DIR)/.bonnie++_installed: $(STAGEFILES_DIR)/.bonnie++_compiled
	$(CP) $(BONNIE++_DIR)/bonnie++ $(TARGET_ROOT)/usr/sbin/bonnie++
	chmod 755 $(TARGET_ROOT)/usr/sbin/bonnie++
	$(CP) $(BONNIE++_DIR)/zcav $(TARGET_ROOT)/usr/sbin/zcav
	chmod 755 $(TARGET_ROOT)/usr/sbin/zcav
	$(TOUCH) $(STAGEFILES_DIR)/.bonnie++_installed


$(FILELIST_DIR)/bonnie++.lst: $(STAGEFILES_DIR)/.bonnie++_installed
	$(TOUCH) $(FILELIST_DIR)/bonnie++.lst

.PHONY: clean-bonnie++ distclean-bonnie++

clean-bonnie++:
	-$(RM) -rf $(BONNIE++_DIR)

#
# clean everthing else
#

distclean-bonnie++:
	-$(RM) -f $(STAGEFILES_DIR)/.bonnie++_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.bonnie++_patched
	-$(RM) -f $(STAGEFILES_DIR)/.bonnie++_configured
	-$(RM) -f $(STAGEFILES_DIR)/.bonnie++_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.bonnie++_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(BONNIE++_DLFILE)
endif
