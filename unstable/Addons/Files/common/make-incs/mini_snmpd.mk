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
MINI_SNMPD_DEPS = $(BASE_BUILD_STAGEFILE)

MINI_SNMPD_VERSION := 1.0
MINI_SNMPD_PATCHES_DIR := $(PATCHES_DIR)/mini_snmpd/$(MINI_SNMPD_VERSION)

MINI_SNMPD_FILE := mini_snmpd.tar.gz
MINI_SNMPD_DLFILE := $(DOWNLOAD_DIR)/$(MINI_SNMPD_FILE)
MINI_SNMPD_URL := http://members.aon.at/linuxfreak/linux/$(MINI_SNMPD_FILE)
MINI_SNMPD_DIR := $(BUILD_DIR)/mini_snmpd

MINI_SNMPD_INSTALLED = $(STAGEFILES_DIR)/.mini_snmpd_installed

PACKS_RULES_$(CONFIG_MINI_SNMPD) += $(MINI_SNMPD_INSTALLED)
FILE_LISTS_$(CONFIG_MINI_SNMPD) += mini_snmpd.lst

CLEAN_RULES += clean-mini_snmpd
DISTCLEAN_RULES += distclean-mini_snmpd

#
# download mini_snmpd
#

$(MINI_SNMPD_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(MINI_SNMPD_DLFILE) ] ; then \
	$(WGET) $(MINI_SNMPD_URL) -O $(MINI_SNMPD_DLFILE) ; \
	fi );
	$(TOUCH) $(MINI_SNMPD_DLFILE)

#
# unpack mini_snmpd
#

$(STAGEFILES_DIR)/.mini_snmpd_unpacked: $(MINI_SNMPD_DLFILE) \
                                           $(wildcard $(MINI_SNMPD_PATCHES_DIR)/*.patch) \
                                           $$(MINI_SNMPD_DEPS)
	-$(RM) -rf $(MINI_SNMPD_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(MINI_SNMPD_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.mini_snmpd_unpacked

#
# patch mini_snmpd
#

$(STAGEFILES_DIR)/.mini_snmpd_patched: $(STAGEFILES_DIR)/.mini_snmpd_unpacked
	$(call patch_package, $(MINI_SNMPD_DIR), $(MINI_SNMPD_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.mini_snmpd_patched

#
# compile mini_snmpd
#

$(STAGEFILES_DIR)/.mini_snmpd_compiled: $(STAGEFILES_DIR)/.mini_snmpd_patched
	$(UCLIBC_ENV) $(MAKE) \
		-C $(MINI_SNMPD_DIR) \
		CC="$(UCLIBC_CC)" \
		OFLAGS="$(UCLIBC_CFLAGS) -DSYSLOG"
	$(TOUCH) $(STAGEFILES_DIR)/.mini_snmpd_compiled

#
# install mini_snmpd
#

$(STAGEFILES_DIR)/.mini_snmpd_installed: $(STAGEFILES_DIR)/.mini_snmpd_compiled
	$(CP) $(MINI_SNMPD_DIR)/mini_snmpd $(TARGET_ROOT)/usr/sbin/mini_snmpd
	$(TOUCH) $(STAGEFILES_DIR)/.mini_snmpd_installed


$(FILELIST_DIR)/mini_snmpd.lst: $(STAGEFILES_DIR)/.mini_snmpd_installed
	$(TOUCH) $(FILELIST_DIR)/mini_snmpd.lst

.PHONY: clean-mini_snmpd distclean-mini_snmpd

clean-mini_snmpd:
	-$(RM) -rf $(MINI_SNMPD_DIR)

#
# clean everthing else
#

distclean-mini_snmpd:
	-$(RM) -f $(STAGEFILES_DIR)/.mini_snmpd_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.mini_snmpd_patched
	-$(RM) -f $(STAGEFILES_DIR)/.mini_snmpd_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.mini_snmpd_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(MINI_SNMPD_DLFILE)
endif
