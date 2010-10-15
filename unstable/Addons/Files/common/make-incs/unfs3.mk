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
UNFS3_DEPS = $(BASE_BUILD_STAGEFILE)

UNFS3_VERSION := 0.9.22
UNFS3_PATCHES_DIR := $(PATCHES_DIR)/unfs3/$(UNFS3_VERSION)

UNFS3_FILE := unfs3-$(UNFS3_VERSION).tar.gz
UNFS3_DLFILE := $(DOWNLOAD_DIR)/$(UNFS3_FILE)
UNFS3_URL := http://downloads.sourceforge.net/unfs3/$(UNFS3_FILE)
UNFS3_DIR := $(BUILD_DIR)/unfs3-$(UNFS3_VERSION)

UNFS3_INSTALLED = $(STAGEFILES_DIR)/.unfs3_installed

PACKS_RULES_$(CONFIG_UNFS3) += $(UNFS3_INSTALLED)
FILE_LISTS_$(CONFIG_UNFS3) += unfs3.lst

CLEAN_RULES += clean-unfs3
DISTCLEAN_RULES += distclean-unfs3

#
# download unfs3
#

$(UNFS3_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(UNFS3_DLFILE) ] ; then \
	$(WGET) $(UNFS3_URL) -O $(UNFS3_DLFILE) ; \
	fi );
	$(TOUCH) $(UNFS3_DLFILE)

#
# unpack unfs3
#

$(STAGEFILES_DIR)/.unfs3_unpacked: $(UNFS3_DLFILE) \
                                           $(wildcard $(UNFS3_PATCHES_DIR)/*.patch) \
                                           $$(UNFS3_DEPS)
	-$(RM) -rf $(UNFS3_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(UNFS3_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.unfs3_unpacked

#
# patch unfs3
#

$(STAGEFILES_DIR)/.unfs3_patched: $(STAGEFILES_DIR)/.unfs3_unpacked
	$(call patch_package, $(UNFS3_DIR), $(UNFS3_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.unfs3_patched

#
# configure unfs3
#

$(STAGEFILES_DIR)/.unfs3_configured: $(STAGEFILES_DIR)/.unfs3_patched
	($(CD) $(UNFS3_DIR) ; $(UCLIBC_ENV) \
		$(UNFS3_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET))
	$(TOUCH) $(STAGEFILES_DIR)/.unfs3_configured

#
# compile unfs3
#

$(STAGEFILES_DIR)/.unfs3_compiled: $(STAGEFILES_DIR)/.unfs3_configured
	$(UCLIBC_ENV) $(MAKE) -C $(UNFS3_DIR)
	$(TOUCH) $(STAGEFILES_DIR)/.unfs3_compiled

#
# install unfs3
#

$(STAGEFILES_DIR)/.unfs3_installed: $(STAGEFILES_DIR)/.unfs3_compiled
	$(CP) -f $(UNFS3_DIR)/unfsd $(TARGET_ROOT)/usr/sbin/unfsd
	$(TOUCH) $(STAGEFILES_DIR)/.unfs3_installed


$(FILELIST_DIR)/unfs3.lst: $(STAGEFILES_DIR)/.unfs3_installed
	$(TOUCH) $(FILELIST_DIR)/unfs3.lst

.PHONY: clean-unfs3 distclean-unfs3

clean-unfs3:
	-$(RM) -rf $(UNFS3_DIR)

#
# clean everthing else
#

distclean-unfs3:
	-$(RM) -f $(STAGEFILES_DIR)/.unfs3_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.unfs3_patched
	-$(RM) -f $(STAGEFILES_DIR)/.unfs3_configured
	-$(RM) -f $(STAGEFILES_DIR)/.unfs3_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.unfs3_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(UNFS3_DLFILE)
endif
