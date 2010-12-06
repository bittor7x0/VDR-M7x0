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
SQLITE_DEPS = $(BASE_BUILD_STAGEFILE)

SQLITE_VERSION := 3.6.12
SQLITE_PATCHES_DIR := $(PATCHES_DIR)/sqlite/$(SQLITE_VERSION)

SQLITE_FILE := sqlite-$(SQLITE_VERSION).tar.bz2
SQLITE_DLFILE := $(DOWNLOAD_DIR)/$(SQLITE_FILE)
SQLITE_URL := http://www.sqlite.org/sqlite-amalgamation-$(SQLITE_VERSION).tar.gz
SQLITE_DIR := $(BUILD_DIR)/sqlite-$(SQLITE_VERSION)

SQLITE_INSTALLED = $(STAGEFILES_DIR)/.sqlite_installed

PACKS_RULES_$(CONFIG_SQLITE) += $(SQLITE_INSTALLED)
FILE_LISTS_$(CONFIG_SQLITE) += sqlite.lst

CLEAN_RULES += clean-sqlite
DISTCLEAN_RULES += distclean-sqlite

#
# download sqlite
#

$(SQLITE_DLFILE): $(TC_INIT_RULE)
	(if [ ! -f $(SQLITE_DLFILE) ] ; then \
	$(WGET) $(SQLITE_URL) -O $(SQLITE_DLFILE) ; \
	fi );
	$(TOUCH) $(SQLITE_DLFILE)

#
# unpack sqlite
#

$(STAGEFILES_DIR)/.sqlite_unpacked: $(SQLITE_DLFILE) \
                                           $(wildcard $(SQLITE_PATCHES_DIR)/*.patch) \
                                           $$(SQLITE_DEPS)
	-$(RM) -rf $(SQLITE_DIR)
	$(TAR) -C $(BUILD_DIR) -zf $(SQLITE_DLFILE)
	$(TOUCH) $(STAGEFILES_DIR)/.sqlite_unpacked

#
# patch sqlite
#

$(STAGEFILES_DIR)/.sqlite_patched: $(STAGEFILES_DIR)/.sqlite_unpacked
	$(call patch_package, $(SQLITE_DIR), $(SQLITE_PATCHES_DIR))
	$(TOUCH) $(STAGEFILES_DIR)/.sqlite_patched

#
# configure sqlite
#

$(STAGEFILES_DIR)/.sqlite_configured: $(STAGEFILES_DIR)/.sqlite_patched
	($(CD) $(SQLITE_DIR) ; $(UCLIBC_ENV) \
		$(SQLITE_DIR)/configure \
			--prefix=$(TARGET_ROOT)/usr \
			--host=$(TARGET) \
			--enable-shared \
			--enable-static)
	$(TOUCH) $(STAGEFILES_DIR)/.sqlite_configured

#
# compile sqlite
#

$(STAGEFILES_DIR)/.sqlite_compiled: $(STAGEFILES_DIR)/.sqlite_configured
	$(UCLIBC_ENV) $(MAKE) -C $(SQLITE_DIR) all
	$(TOUCH) $(STAGEFILES_DIR)/.sqlite_compiled

#
# install sqlite
#

$(STAGEFILES_DIR)/.sqlite_installed: $(STAGEFILES_DIR)/.sqlite_compiled
	$(UCLIBC_ENV) $(MAKE) -C $(SQLITE_DIR) install
	$(TOUCH) $(STAGEFILES_DIR)/.sqlite_installed


$(FILELIST_DIR)/sqlite.lst: $(STAGEFILES_DIR)/.sqlite_installed
	$(TOUCH) $(FILELIST_DIR)/sqlite.lst

.PHONY: clean-sqlite distclean-sqlite

clean-sqlite:
	-$(RM) -rf $(SQLITE_DIR)

#
# clean everthing else
#

distclean-sqlite:
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_unpacked
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_patched
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_configured
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_compiled
	-$(RM) -f $(STAGEFILES_DIR)/.sqlite_installed
ifeq ($(DISTCLEAN_DLFILE),y)
	-$(RM) -rf $(SQLITE_DLFILE)
endif
